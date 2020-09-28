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

#ifndef __UK_SOCKET_VNOPS__
#define __UK_SOCKET_VNOPS__

#include <vfscore/file.h>
#include <vfscore/vnode.h>

struct posix_socket_driver;

#define SOCKFILE_DATA_AS_INT(data) \
	*((int *) data)


/**
 * Return the socket file structure used by a file descriptor.
 *
 * @param sock_fd
 *  The vfscore file descriptor number.
 */
struct posix_socket_file *
posix_socket_file_get(int sock_fd);

/**
 * Return the socket driver used by a file descriptor.
 *
 * @param sock_fd
 *  The vfscore file descriptor number.
 */
struct posix_socket_driver *
posix_socket_get_family(int sock_fd);

/**
 * Allocate a file descriptor in vfscore for the given socket driver.
 *
 * @param d
 *  The driver used to create the socket.
 * @param type
 *  The socket type.
 * @param sock_data
 *  The socket implementation's private data.
 */
int
socket_alloc_fd(struct posix_socket_driver *d, int type, void *sock_data);

#endif /* __UK_SOCKET_VNOPS__ */
