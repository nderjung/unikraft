/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Alexander Jung <alexander.jung@neclab.eu>
 *
 * Copyright (c) 2020, NEC Laboratories Europe GmbH, NEC Corporation.d
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <uk/mbox.h>
#include <uk/print.h>
#include <uk/config.h>
#include <uk/assert.h>
#include <uk/socket.h>
#include <uk/unixsock.h>
#include <uk/semaphore.h>
#include <uk/essentials.h>
#include <uk/arch/limits.h>
#include <vfscore/dentry.h>

#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

static void *
unixsock_glue_create(struct posix_socket_driver *d, int family, int type,
          int protocol)
{
  void *ret = NULL;
  struct unixsock *unsock;
  UK_ASSERT(family == AF_UNIX);

  /* Unix domain sockets only support SOCK_STREAM and SOCK_DGRAM. */
  if (((type & SOCK_STREAM) != SOCK_STREAM) &&
      ((type & SOCK_DGRAM) != SOCK_DGRAM) &&
      ((type & SOCK_SEQPACKET) != SOCK_SEQPACKET)) {
    ret = NULL;
    SOCKET_ERR(ESOCKTNOSUPPORT, "socket type unsupported");
    goto EXIT;
  }

  unsock = uk_calloc(d->allocator, 1, sizeof(struct unixsock));
  if (!unsock) {
    ret = NULL;
    SOCKET_ERR(ENOMEM, "could not allocate socket");
    goto EXIT;
  }

  /* Initialize unixsock. */
  unsock->local_addr.sun_family = AF_UNIX;
  unsock->local_addr.sun_path[0] = '\0';
  
  /* Initialize connection wait. */
  uk_semaphore_init(&unsock->connections, 0);

#ifdef DEBUG_BUFRING
  uk_mutex_init(&unsock->buflock);
#endif

  /* Initialize a buffer for this socket. */
  unsock->buffer = unixsock_ring_alloc(CONFIG_LIBUKUNIXSOCK_BUFLEN, d->allocator
#ifdef DEBUG_BUFRING
          , &unsock->buflock
#endif
  );
  if (unsock->buffer == NULL) {
    ret = NULL;
    SOCKET_ERR(ENOMEM, "could not allocate socket buffer");
    goto LOCAL_SOCKET_CLEANUP;
  }

#if CONFIG_LIBUKSCHED
	uk_waitq_init(&unsock->state_wq);
#endif

  /* Set the initial state of the socket. */
  unsock->state = UNIXSOCK_OPEN;

  /* Return a successfully initialized unix domain socket. */
  ret = (void *)unsock;

EXIT:
  return ret;

LOCAL_SOCKET_CLEANUP:
  uk_free(d->allocator, unsock);
  goto EXIT;
}


static int
unixsock_glue_socketpair(struct posix_socket_driver *d, int family, int type,
          int protocol, void *usockvec[2])
{
  int ret;
  struct unixsock *u1, *u2;

  u1 = unixsock_glue_create(d, family, type, protocol);
  if (u1 == NULL) {
    ret = -1;
    goto EXIT;
  }

  u2 = unixsock_glue_create(d, family, type, protocol);
  if (u2 == NULL) {
    ret = -1;
    goto EXIT;
  }

  u1->peer = u2;
  u1->state |= UNIXSOCK_CONNECTED;
  u2->peer = u1;
  u2->state |= UNIXSOCK_CONNECTED;

  usockvec[0] = (void *)u1;
  usockvec[1] = (void *)u2;

  ret = 0;

EXIT:
  return ret;
}

static int
unixsock_glue_read(struct posix_socket_file *sock, void *buf, size_t count)
{
  int n;
  int ret = 0;
  uint64_t len;
  struct unixsock *unsock;
  
  /* Transform the socket descriptor to the unixsock pointer. */
  unsock = (struct unixsock *)sock->sock_data;
  if (!unsock) {
    ret = -1;
    SOCKET_ERR(EBADF, "failed to identify socket descriptor");
    goto EXIT;
  }

  if ((unsock->flags & SOCK_NONBLOCK) && unixsock_ring_full(unsock->buffer)) {
    ret = -1;
    SOCKET_ERR(EAGAIN, "socket ring buffer is full");
    goto EXIT;
  }

  if (unixsock_ring_empty(unsock->buffer)) {
    ret = -1;
    SOCKET_ERR(EAGAIN, "socket ring buffer is empty");
    goto EXIT;
  }

  do {
    len = MIN(unsock->buffer->br_cons_size, count);
    n = unixsock_ring_dequeue_bulk_mc(unsock->buffer, buf, len, NULL);

    if (n < 0) {
      SOCKET_ERR(n, "failed to read from socket buffer");
      goto EXIT;
    }

    ret += n;
    count -= n;
  } while((count > 0) && !unixsock_ring_empty(unsock->buffer));

#if 0 /* TODO: Notify writer destination */
  if (ret > 0 || (ret == 0 && sock->type == SOCK_DGRAM))
#if CONFIG_LIBUKSCHED
    uk_waitq_wait_event(&unsock->state_wq, unsock->state == UNIXSOCK_RECEIVED);
#else
    while(UK_READ_ONCE(unsock->state) != UNIXSOCK_RECEIVED) {};
#endif
#endif

EXIT:
  return ret;
}


static ssize_t
unixsock_glue_write(struct posix_socket_file *sock, const void *buf,
          size_t count)
{
  int n;
  int ret = 0;
  uint64_t len;
  struct unixsock *unsock;
  
  /* Transform the socket descriptor to the unixsock pointer. */
  unsock = (struct unixsock *)sock->sock_data;
  if (!unsock) {
    ret = -1;
    SOCKET_ERR(EBADF, "failed to identify socket descriptor");
    goto EXIT;
  }

  if (!unsock->peer) {
    ret = -1;
    SOCKET_ERR(EPIPE, "peer not connected");
    goto EXIT;
  }

  if (count > CONFIG_LIBUKUNIXSOCK_BUFLEN) {
    ret = -1;
    SOCKET_ERR(EMSGSIZE, "write size greater than buffer length");
    goto EXIT;
  }

  do {
    len = MIN(unsock->peer->buffer->br_prod_size, count);
    n = unixsock_ring_enqueue_bulk_mc(unsock->peer->buffer, buf, len, NULL);

    if (n < 0) {
      SOCKET_ERR(n, "failed to write to socket buffer");
      goto EXIT;
    }

    ret += n;
    count -= n;
    buf = (caddr_t *)buf + n;
  } while((count > 0) && !unixsock_ring_full(unsock->buffer));

#if 0 /* TODO: Notify reader destination */
  if (ret > 0 || (ret == 0 && sock->type == SOCK_DGRAM))
#if CONFIG_LIBUKSCHED
    uk_waitq_wait_event(&unsock->state_wq, unsock->state == UNIXSOCK_RECEIVED);
#else
    while(UK_READ_ONCE(unsock->state) != UNIXSOCK_RECEIVED) {};
#endif
#endif

EXIT:
  return ret;
}


static int
unixsock_glue_close(struct posix_socket_file *sock)
{
  int ret = 0;
  struct unixsock *unsock;
  
  /* Transform the socket descriptor to the unixsock pointer. */
  unsock = (struct unixsock *)sock->sock_data;
  if (!unsock) {
    ret = -1;
    SOCKET_ERR(EBADF, "failed to identify socket descriptor");
    goto EXIT;
  }

  if (unsock->peer) {
    unsock->peer->peer = 0;
    /* TODO: Notify EPOLLHUP */
  }
  if (unsock->buffer) {
    unixsock_ring_free(unsock->buffer, sock->driver->allocator);
  }
  if (unsock->connq) {
    uk_mbox_free(sock->driver->allocator, unsock->connq);
  }

EXIT:
  return ret;
}


static struct posix_socket_ops unixsock_ops = {
  /* POSIX interfaces */
  .create      = unixsock_glue_create,
  .socketpair  = unixsock_glue_socketpair,
  /* vfscore ops */
  .read        = unixsock_glue_read,
  .write       = unixsock_glue_write,
  .close       = unixsock_glue_close,
};

POSIX_SOCKET_FAMILY_REGISTER(AF_UNIX, &unixsock_ops, NULL);
