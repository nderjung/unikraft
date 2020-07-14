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

#ifndef __UK_SOCKET_DRIVER_H__
#define __UK_SOCKET_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <uk/list.h>
#include <uk/assert.h>
#include <uk/init.h>
#include <uk/list.h>
#include <uk/ctors.h>
#include <uk/alloc.h>
#include <uk/essentials.h>
#include <errno.h>
#include <sys/socket.h>

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define __NEED_socklen_t
#include <bits/alltypes.h>
#endif

#define POSIX_SOCKET_FAMILY_INIT_CLASS UK_INIT_CLASS_EARLY
#define POSIX_SOCKET_FAMILY_INIT_PRIO 0
#define POSIX_SOCKET_FAMILY_REGISTER_PRIO 2

struct posix_socket_ops;
struct posix_socket_driver;

/**
 * The POSIX socket driver defines the operations to be used for the
 * specified AF family as well as the memory allocator.
 */
struct posix_socket_driver {
  /* The AF family ID */
	const int af_family;
  const char *libname;
  /* The interfaces for this socket */
	const struct posix_socket_ops *ops;
  /* The memory allocator to be used for this socket driver */
  struct uk_alloc *allocator;
  /* Private data for this socket driver. */
  void *private;
  /* Entry for list of socket drivers. */
	UK_TAILQ_ENTRY(struct posix_socket_driver) _list;
};


/**
 * The initialization function called for this socket family.  It's here that
 * additional configuration for the driver can be made after it has been
 * registered.  For instance, an alternative memory allocator can be provided.
 *
 * @param d
 *  The socket driver.
 */
typedef int (*posix_socket_driver_init_func_t)(struct posix_socket_driver *d);

/**
 * Create a connection on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param family
 *  Specifies a communication family domain and thus driver.
 * @param type
 *  Specifies communication semantics.
 * @param protocol
 *  Specifies a particular protocol to be used with the socket. 
 */
typedef void *(*posix_socket_create_func_t)(struct posix_socket_driver *d,
          int family, int type, int protocol);


/**
 * Accept a connection on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The address of the peer socket.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef void *(*posix_socket_accept_func_t)(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len);


/**
 * Bind a name to a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The assigned address.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef int (*posix_socket_bind_func_t)(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len);


/**
 * Shut down part of a full-duplex connection.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param how
 *  The flag to specify the means of shuting down the socket.
 */
typedef int (*posix_socket_shutdown_func_t)(struct posix_socket_driver *d,
          void *sock, int how);


/**
 * Get name of connected peer socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The assigned address.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef int (*posix_socket_getpeername_func_t)(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len);


/**
 * Get socket name.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The assigned address.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef int (*posix_socket_getsockname_func_t)(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len);

/**
 * Get socket name.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The assigned address.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef int (*posix_socket_getnameinfo_func_t)(struct posix_socket_driver *d,
          const struct sockaddr *restrict sa, socklen_t sl, char *restrict node,
          socklen_t nodelen, char *restrict serv, socklen_t servlen, int flags);

/**
 * Get options on the socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param level
 *  Maniipulate the socket at either the API level or protocol level.
 * @param optname
 *  Any specified options are passed uninterpreted to the appropriate protocol
 *  module for interpretation.
 * @param optval
 *  The option value.
 * @param optlen
 *  The option value length.
 */
typedef int (*posix_socket_getsockopt_func_t)(struct posix_socket_driver *d,
          void *sock, int level, int optname, void *restrict optval,
          socklen_t *restrict optlen);


/**
 * Set options on the socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param level
 *  Maniipulate the socket at either the API level or protocol level.
 * @param optname
 *  Any specified options are passed uninterpreted to the appropriate protocol
 *  module for interpretation.
 * @param optval
 *  The option value.
 * @param optlen
 *  The option value length.
 */
typedef int (*posix_socket_setsockopt_func_t)(struct posix_socket_driver *d,
          void *sock, int level, int optname, const void *optval,
          socklen_t optlen);


/**
 * Initiate a connection on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param addr
 *  The address to connect to on the socket.
 * @param addr_len
 *  Specifies the size, in bytes, of the address structure pointed to by addr.
 */
typedef int (*posix_socket_connect_func_t)(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len);


/**
 * Listen for connections on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param backlog
 *  Defines the maximum length to which the queue of pending connections for 
 *  the socket.
 */
typedef int (*posix_socket_listen_func_t)(struct posix_socket_driver *d,
          void *sock, int backlog);


/**
 * Receive a message from a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param buf
 *  The buffer for recieved data from the socket.
 * @param len
 *  The size of the buffer.
 * @param flags
 *  Bitwise OR of zero or more flags for the socket.
 */
typedef ssize_t (*posix_socket_recv_func_t)(struct posix_socket_driver *d,
          void *sock, void *buf, size_t len, int flags);


/**
 * Read from a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param buf
 *  The buffer for recieved data from the socket.
 * @param len
 *  The size of the buffer.
 * @param flags
 *  Bitmap of options for receiving a message.
 * @param from
 *  The source address.
 * @param fromlen
 *  The source address length.
 */
typedef ssize_t (*posix_socket_recvfrom_func_t)(struct posix_socket_driver *d,
          void *sock, void *restrict buf, size_t len, int flags,
          struct sockaddr *from, socklen_t *restrict fromlen);


/**
 * Read from a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param msg
 *  Message structure to minimize the number of directly supplied arguments.
 * @param flags
 *  Bitwise OR of zero or more flags for the socket.
 */
typedef ssize_t (*posix_socket_recvmsg_func_t)(struct posix_socket_driver *d,
          void *sock, struct msghdr *msg, int flags);


/**
 * Send a message on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param buf
 *  The buffer for sending data to the socket.
 * @param len
 *  The length of the data to send on the socket.
 * @param flags
 *  Bitwise OR of zero or more flags for the socket.
 */
typedef ssize_t (*posix_socket_send_func_t)(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags);


/**
 * Send a message on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param msg
 *  Message structure to minimize the number of directly supplied arguments.
 * @param flags
 *  Bitwise OR of zero or more flags for the socket.
 */
typedef ssize_t (*posix_socket_sendmsg_func_t)(struct posix_socket_driver *d,
          void *sock, const struct msghdr *msg, int flags);


/**
 * Send a message on a socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 * @param buf
 *  The buffer for sending data to the socket.
 * @param len
 *  The length of the data to send on the socket.
 * @param flags
 *  Bitwise OR of zero or more flags for the socket.
 * @param dest_addr
 *  The destination address to send data.
 * @param addrlen
 *  The length of the address to send data to.
 */
typedef ssize_t (*posix_socket_sendto_func_t)(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags,
          const struct sockaddr *dest_addr, socklen_t addrlen);


/**
 * Create a pair of connected sockets.
 *
 * @param driver
 *  The socket driver.
 * @param family
 *  The domain of the sockets.
 * @param type
 *  The specified type of the sockets.
 * @param protocol
 *  Optionally the protocol.
 * @param usockvec
 *  The structure used in referencing the new sockets are returned in
 *  usockvec[0] and usockvec[1].
 */
typedef int (*posix_socket_socketpair_func_t)(struct posix_socket_driver *d,
          int family, int type, int protocol, void **usockvec);


/**
 * Read from a socket file descriptor.
 *
 * @param driver
 *  The socket driver.
 * @param buf
 *  The buffer to read the data from the socket into.
 * @param count
 *  The number of bytes to be read.
 */
typedef int (*posix_socket_read_func_t)(struct posix_socket_driver *d,
          void *sock, void *buf, size_t count);


/**
 * Write to a socket file descriptor.
 *
 * @param driver
 *  The socket driver.
 * @param buf
 *  The pointer to the buffer to write to the socket.
 * @param count
 *  The number of bytes to be written.
 */
typedef int (*posix_socket_write_func_t)(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t count);

/**
 * Close the socket.
 *
 * @param driver
 *  The socket driver.
 * @param sock
 *  Reference to the socket.
 */
typedef int (*posix_socket_close_func_t)(struct posix_socket_driver *d,
          void *sock);


/**
 * Manipulate the socket.
 *
 * @param driver
 *  The socket driver.
 * @TODO
 */
typedef int (*posix_socket_ioctl_func_t)(struct posix_socket_driver *d,
          void *sock, int request, void *argp);


/**
 * A structure containing the functions exported by the Unikraft socket driver
 */
struct posix_socket_ops {
  /* The initialization function on socket registration. */
  posix_socket_driver_init_func_t   init;
  /* POSIX interfaces */
  posix_socket_create_func_t        create;
  posix_socket_accept_func_t        accept;
  posix_socket_bind_func_t          bind;
  posix_socket_shutdown_func_t      shutdown;
  posix_socket_getpeername_func_t   getpeername;
  posix_socket_getsockname_func_t   getsockname;
  posix_socket_getnameinfo_func_t   getnameinfo;
  posix_socket_getsockopt_func_t    getsockopt;
  posix_socket_setsockopt_func_t    setsockopt;
  posix_socket_connect_func_t       connect;
  posix_socket_listen_func_t        listen;
  posix_socket_recv_func_t          recv;
  posix_socket_recvfrom_func_t      recvfrom;
  posix_socket_recvmsg_func_t       recvmsg;
  posix_socket_send_func_t          send;
  posix_socket_sendmsg_func_t       sendmsg;
  posix_socket_sendto_func_t        sendto;
  posix_socket_socketpair_func_t    socketpair;
  /* vfscore ops */
  posix_socket_read_func_t          read;
  posix_socket_write_func_t         write;
  posix_socket_close_func_t         close;
  posix_socket_ioctl_func_t         ioctl;
};

static inline void *
posix_socket_do_create(struct posix_socket_driver *d,
          int family, int type, int protocol)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->create);
  return d->ops->create(d, family, type, protocol);
}

static inline void *
posix_socket_create(struct posix_socket_driver *d,
          int family, int type, int protocol)
{
  if (unlikely(!d))
    return NULL;

  return posix_socket_do_create(d, family, type, protocol);
}

static inline void *
posix_socket_do_accept(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->accept);
  return d->ops->accept(d, sock, addr, addr_len);
}

static inline void *
posix_socket_accept(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  if (unlikely(!d))
    return NULL;

  return posix_socket_do_accept(d, sock, addr, addr_len);
}


static inline int
posix_socket_do_bind(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->bind);
  return d->ops->bind(d, sock, addr, addr_len);
}

static inline int
posix_socket_bind(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_bind(d, sock, addr, addr_len);
}


static inline int
posix_socket_do_shutdown(struct posix_socket_driver *d,
          void *sock, int how)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->shutdown);
	return d->ops->shutdown(d, sock, how);
}

static inline int
posix_socket_shutdown(struct posix_socket_driver *d,
          void *sock, int how)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_shutdown(d, sock, how);
}


static inline int
posix_socket_do_getpeername(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->getpeername);
	return d->ops->getpeername(d, sock, addr, addr_len);
}

static inline int
posix_socket_getpeername(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_getpeername(d, sock, addr, addr_len);
}


static inline int
posix_socket_do_getsockname(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->getsockname);
	return d->ops->getsockname(d, sock, addr, addr_len);
}

static inline int
posix_socket_getsockname(struct posix_socket_driver *d,
          void *sock, struct sockaddr *restrict addr,
          socklen_t *restrict addr_len)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_getsockname(d, sock, addr, addr_len);
}


static inline int
posix_socket_do_getnameinfo(struct posix_socket_driver *d,
          const struct sockaddr *restrict sa, socklen_t sl, char *restrict node,
          socklen_t nodelen, char *restrict serv, socklen_t servlen, int flags)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->getnameinfo);
	return d->ops->getnameinfo(d, sa, sl, node, nodelen, serv, servlen, flags);
}

static inline int
posix_socket_getnameinfo(struct posix_socket_driver *d,
          const struct sockaddr *restrict sa, socklen_t sl, char *restrict node,
          socklen_t nodelen, char *restrict serv, socklen_t servlen, int flags)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_getnameinfo(d, sa, sl, node, nodelen, serv, servlen,
          flags);
}


static inline int
posix_socket_do_getsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, void *restrict optval,
          socklen_t *restrict optlen)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->getsockopt);
  return d->ops->getsockopt(d, sock, level, optname, optval, optlen);
}

static inline int
posix_socket_getsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, void *restrict optval,
          socklen_t *restrict optlen)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_getsockopt(d, sock, level, optname, optval, optlen);
}


static inline int

posix_socket_do_setsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, const void *optval,
          socklen_t optlen)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->setsockopt);
  return d->ops->setsockopt(d, sock, level, optname, optval, optlen);
}

static inline int
posix_socket_setsockopt(struct posix_socket_driver *d,
          void *sock, int level, int optname, const void *optval,
          socklen_t optlen)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_setsockopt(d, sock, level, optname, optval, optlen);
}


static inline int
posix_socket_do_connect(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->connect);
  return d->ops->connect(d, sock, addr, addr_len);
}

static inline int
posix_socket_connect(struct posix_socket_driver *d,
          void *sock, const struct sockaddr *addr, socklen_t addr_len)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_connect(d, sock, addr, addr_len);
}


static inline int
posix_socket_do_listen(struct posix_socket_driver *d,
          void *sock, int backlog)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->listen);
  return d->ops->listen(d, sock, backlog);
}

static inline int
posix_socket_listen(struct posix_socket_driver *d,
          void *sock, int backlog)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_listen(d, sock, backlog);
}


static inline ssize_t
posix_socket_do_recv(struct posix_socket_driver *d,
          void *sock, void *buf, size_t len, int flags)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->recv);
  return d->ops->recv(d, sock, buf, len, flags);
}

static inline ssize_t
posix_socket_recv(struct posix_socket_driver *d,
          void *sock, void *buf, size_t len, int flags)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_recv(d, sock, buf, len, flags);
}


static inline ssize_t
posix_socket_do_recvfrom(struct posix_socket_driver *d,
          void *sock, void *buf, size_t len, int flags, struct sockaddr *from, 
          socklen_t *fromlen)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->recvfrom);
  return d->ops->recvfrom(d, sock, buf, len, flags, from, fromlen);
}

static inline ssize_t
posix_socket_recvfrom(struct posix_socket_driver *d,
          void *sock, void *restrict buf, size_t len, int flags,
          struct sockaddr *from, socklen_t *fromlen)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_recvfrom(d, sock, buf, len, flags, from, fromlen);
}


static inline ssize_t
posix_socket_do_recvmsg(struct posix_socket_driver *d,
          void *sock, struct msghdr *msg, int flags)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->recvmsg);
  return d->ops->recvmsg(d, sock, msg, flags);
}

static inline ssize_t
posix_socket_recvmsg(struct posix_socket_driver *d,
          void *sock, struct msghdr *msg, int flags)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_recvmsg(d, sock, msg, flags);
}


static inline ssize_t
posix_socket_do_send(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->send);
  return d->ops->send(d, sock, buf, len, flags);
}

static inline ssize_t
posix_socket_send(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_send(d, sock, buf, len, flags);
}


static inline ssize_t
posix_socket_do_sendmsg(struct posix_socket_driver *d,
          void *sock, const struct msghdr *msg, int flags)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->sendmsg);
  return d->ops->sendmsg(d, sock, msg, flags);
}

static inline ssize_t
posix_socket_sendmsg(struct posix_socket_driver *d,
          void *sock, const struct msghdr *msg, int flags)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_sendmsg(d, sock, msg, flags);
}


static inline ssize_t
posix_socket_do_sendto(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags,
          const struct sockaddr *dest_addr, socklen_t addrlen)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->sendto);
  return d->ops->sendto(d, sock, buf, len, flags, dest_addr, addrlen);
}

static inline ssize_t
posix_socket_sendto(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t len, int flags,
          const struct sockaddr *dest_addr, socklen_t addrlen)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_sendto(d, sock, buf, len, flags, dest_addr, addrlen);
}


static inline int
posix_socket_do_socketpair(struct posix_socket_driver *d,
          int family, int type, int protocol, void **usockvec)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->socketpair);
  return d->ops->socketpair(d, family, type, protocol, usockvec);
}

static inline int
posix_socket_socketpair(struct posix_socket_driver *d,
          int family, int type, int protocol, void **usockvec)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_socketpair(d, family, type, protocol, usockvec);
}


static inline int
posix_socket_do_read(struct posix_socket_driver *d,
          void *sock, void *buf, size_t count)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->read);
  return d->ops->read(d, sock, buf, count);
}

static inline int
posix_socket_read(struct posix_socket_driver *d,
          void *sock, void *buf, size_t count)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_read(d, sock, buf, count);
}


static inline int
posix_socket_do_write(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t count)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->write);
  return d->ops->write(d, sock, buf, count); 
}

static inline int
posix_socket_write(struct posix_socket_driver *d,
          void *sock, const void *buf, size_t count)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_write(d, sock, buf, count);
}


static inline int
posix_socket_do_close(struct posix_socket_driver *d,
          void *sock)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->close);
  return d->ops->close(d, sock);
}

static inline int
posix_socket_close(struct posix_socket_driver *d,
          void *sock)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_close(d, sock);
}


static inline int
posix_socket_do_ioctl(struct posix_socket_driver *d,
          void *sock, int request, void *argp)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->ioctl);
  return d->ops->ioctl(d, sock, request, argp);
}

static inline int
posix_socket_ioctl(struct posix_socket_driver *d,
          void *sock, int request, void *argp)
{
  if (unlikely(!d))
    return -ENOSYS;

  return posix_socket_do_ioctl(d, sock, request, argp);
}

/**
 * Return the driver to the corresponding AF family number
 *
 * @param af_family
 *  Af family number
 */
struct posix_socket_driver *
posix_socket_driver_get(int af_family);

/**
 * Shortcut for doing a registration a socket to an AF number.
 */
#define new_posix_socket_family(d, fam, alloc, ops) \
  do {                                              \
    (d)->allocator = (alloc);                       \
    (d)->ops       = (ops);                         \
  } while(0)

/**
 * Returns the number of registered sockets.
 */
unsigned int
posix_socket_family_count(void);

/* Do not use this function directly */
void
_posix_socket_family_register(struct posix_socket_driver *d,
          int fam,
          struct posix_socket_ops *ops,
          struct uk_alloc *alloc);

/**
 * Registers a socket family driver to the socket system.
 */
#define _POSIX_SOCKET_FAMILY_REGISTER_CTOR(fam, ctor) \
  UK_CTOR_PRIO(ctor, POSIX_SOCKET_FAMILY_REGISTER_PRIO)

#define _POSIX_SOCKET_FAMILY_REGFNNAME(x, y) x##_posix_socket_af_##y##_register
#define _POSIX_SOCKET_FAMILY_DRVRNAME(x, y) x##_posix_socket_af_##y

/*
 * Creates a static struct posix_socket_driver with a unique name for the AF
 * family which can later be referenced.
 */
#define _POSIX_SOCKET_FAMILY_REGISTER(lib, fam, ops, alloc)                     \
  static struct posix_socket_driver _POSIX_SOCKET_FAMILY_DRVRNAME(lib, fam) = { \
    .af_family = fam,                                                           \
    .libname   = STRINGIFY(lib)                                                 \
  };                                                                            \
  static void                                                                   \
  _POSIX_SOCKET_FAMILY_REGFNNAME(lib, fam)(void)                                \
  {                                                                             \
    _posix_socket_family_register(&_POSIX_SOCKET_FAMILY_DRVRNAME(lib, fam),     \
        fam, ops, alloc);                                                       \
  }                                                                             \
  _POSIX_SOCKET_FAMILY_REGISTER_CTOR(fam, _POSIX_SOCKET_FAMILY_REGFNNAME(lib, fam))

#define POSIX_SOCKET_FAMILY_REGISTER(fam, ops, alloc) \
  _POSIX_SOCKET_FAMILY_REGISTER(__LIBNAME__, fam, ops, alloc)

/* Do not use this function directly: */
void
_posix_socket_family_unregister(struct posix_socket_driver *driver);

#ifdef __cplusplus
}

#endif /* __cplusplus */
#endif /*  __UK_SOCKET_DRIVER_H__ */