/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Alexander Jung <alexander.jung@neclab.eu>
 *
 * Copyright (c) 2020, NEC Laboratories Europe GmbH, NEC Corporation.
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

#include <uk/socket.h>
#include <sys/socket.h>
#include <virtio/virtio_vsock.h>

int
vsock_lib_socket_init(struct posix_socket_driver *d)
{
  return 0;
}

static void *
vsock_glue_create(struct posix_socket_driver *d,
          int family, int type, int protocol)
{
  return 0;
}

static void *
vsock_glue_accept(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  return 0;
}

static int
vsock_glue_bind(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len)
{
  return 0;
}

static int
vsock_glue_shutdown(struct posix_socket_driver *d,
          void *sock, int how)
{
  return 0;
}

static int
vsock_glue_getpeername(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  return 0;
}

static int
vsock_glue_getsockname(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  return 0;
}

static int
vsock_glue_getsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, void *restrict optval,
          socklen_t *restrict optlen)
{
  return 0;
}

static int
vsock_glue_setsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, const void *optval,
          socklen_t optlen)
{
  return 0;
}

static int
vsock_glue_connect(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr,
          socklen_t addr_len)
{
  return 0;
}

static int
vsock_glue_listen(struct posix_socket_driver *d,
          void *sock, int backlog)
{
  return 0;
}

static ssize_t
vsock_glue_recv(struct posix_socket_driver *d,
          void *sock, void *buf, size_t len, int flags)
{
  return 0;
}

static ssize_t
vsock_glue_recvfrom(struct posix_socket_driver *d,
          void *sock, void *restrict buf, size_t len, int flags,
          struct sockaddr *from, socklen_t *restrict fromlen)
{
  return 0;
}

static ssize_t
vsock_glue_recvmsg(struct posix_socket_driver *d,
          void *sock, struct msghdr *msg, int flags)
{
  return 0;
}

static ssize_t
vsock_glue_send(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags)
{
  return 0;
}

static ssize_t
vsock_glue_sendmsg(struct posix_socket_driver *d,
          void *sock, const struct msghdr *msg, int flags)
{
  return 0;
}

static ssize_t
vsock_glue_sendto(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags,
          const struct sockaddr *dest_addr, socklen_t addrlen)
{
  return 0;
}

static int
vsock_glue_read(struct posix_socket_driver *d,
          void *sock, void *buf, size_t count)
{
  return 0;
}

static int
vsock_glue_write(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t count)
{
  return 0;
}

static int
vsock_glue_close(struct posix_socket_driver *d,
          void *sock)
{
  return 0;
}

static int
vsock_glue_ioctl(struct posix_socket_driver *d,
          void *sock, int request, void *argp)
{
  return 0;
}

static struct posix_socket_ops vsock_socket_ops = {
  /* The initialization function on socket registration. */
  .init        = vsock_lib_socket_init,
  /* POSIX interfaces */
  .create      = vsock_glue_create,
  .accept      = vsock_glue_accept,
  .bind        = vsock_glue_bind,
  .shutdown    = vsock_glue_shutdown,
  .getpeername = vsock_glue_getpeername,
  .getsockname = vsock_glue_getsockname,
  .getsockopt  = vsock_glue_getsockopt,
  .setsockopt  = vsock_glue_setsockopt,
  .connect     = vsock_glue_connect,
  .listen      = vsock_glue_listen,
  .recv        = vsock_glue_recv,
  .recvfrom    = vsock_glue_recvfrom,
  .recvmsg     = vsock_glue_recvmsg,
  .send        = vsock_glue_send,
  .sendmsg     = vsock_glue_sendmsg,
  .sendto      = vsock_glue_sendto,
  /* vfscore ops */
  .read        = vsock_glue_read,
  .write       = vsock_glue_write,
  .close       = vsock_glue_close,
  .ioctl       = vsock_glue_ioctl,
};

POSIX_SOCKET_FAMILY_REGISTER(AF_VSOCK, &vsock_socket_ops, NULL);
