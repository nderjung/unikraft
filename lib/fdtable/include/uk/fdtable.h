/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Yuri Volchkov <yuri.volchkov@neclab.eu>
 *          Alexander Jung <alexander.jung@neclab.eu>
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

#ifndef __UK_FDTABLE_H__
#define __UK_FDTABLE_H__

#include <stdint.h>
#include <uk/bitops.h>

/* Also used from posix-sysinfo to determine sysconf(_SC_OPEN_MAX). */
#define FDTABLE_MAX_FILES 1024

struct uk_fdtable {
	unsigned long bitmap[UK_BITS_TO_LONGS(FDTABLE_MAX_FILES)];
	uint32_t fd_start;
	/* pointers to mixed file descriptors */
	uintptr_t files[FDTABLE_MAX_FILES];
};

/**
 *
 */
int fdtable_alloc_fd(void);

/**
 * Reserve a file descriptor ID within the file descriptor table.
 *
 * @param fd
 *  File descriptor ID
 * @return
 *  
 */
int fdtable_reserve_fd(int fd);

/**
 *
 */
int fdtable_put_fd(int fd);

/**
 *
 */
int fdtable_install_fd(int fd, uintptr_t file);

/**
 *
 */
uintptr_t fdtable_get_fileptr(int fd);

/**
 *
 */
void fdtable_put_file(uintptr_t file);

/**
 *
 */
int fget(int fd, uintptr_t *out_fp);

/**
 *
 */
int fdalloc(uintptr_t fp, int *newfd);


#endif /* __UK_FDTABLE_H__ */