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
#include <uk/socket.h>

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define __NEED_socklen_t
#include <bits/alltypes.h>
#endif

#define POSIX_SOCKET_FAMILY_INIT_CLASS UK_INIT_CLASS_EARLY
#define POSIX_SOCKET_FAMILY_INIT_PRIO 0
#define POSIX_SOCKET_FAMILY_REGISTER_PRIO 2

struct posix_socket_ops;
struct posix_socket_file;
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
 * Close the socket.
 *
 * @param sock
 *  Reference to the socket.
 */
typedef int (*posix_socket_close_func_t)(struct posix_socket_file *sock);


/**
 * A structure containing the functions exported by the Unikraft socket driver
 */
struct posix_socket_ops {
	/* The initialization function on socket registration. */
	posix_socket_driver_init_func_t   init;
	/* vfscore ops */
	posix_socket_close_func_t         close;
};


static inline int
posix_socket_do_close(struct posix_socket_file *sock)
{
	UK_ASSERT(sock);
	UK_ASSERT(sock->driver->ops->close);
	return sock->driver->ops->close(sock);
}

static inline int
posix_socket_close(struct posix_socket_file *sock)
{
	if (unlikely(!sock))
		return -ENOSYS;

	return posix_socket_do_close(sock);
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
	} while (0)

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