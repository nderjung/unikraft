/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Yuri Volchkov <yuri.volchkov@neclab.eu>
 *          Alexander Jung <alexander.jung@neclab.eu>
 *
 * Copyright (c) 2020, NEC Europe Ltd., NEC Corporation. All rights reserved.
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

#include <uk/ctors.h>
#include <uk/bitmap.h>
#include <uk/assert.h>
#include <uk/plat/lcpu.h>
#include <uk/essentials.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <fdtable.h>

void init_stdio(void);

struct fdtable fdtable;

int fdtable_alloc_fd(void)
{
	unsigned long flags;
	int ret;

	flags = ukplat_lcpu_save_irqf();
	ret = uk_find_next_zero_bit(fdtable.bitmap, FDTABLE_MAX_FILES, 0);

	if (ret == FDTABLE_MAX_FILES) {
		ret = -ENFILE;
		goto exit;
	}

	uk_bitmap_set(fdtable.bitmap, ret, 1);

exit:
	ukplat_lcpu_restore_irqf(flags);
	return ret;
}

int fdtable_reserve_fd(int fd)
{
	unsigned long flags;
	int ret = 0;

	flags = ukplat_lcpu_save_irqf();
	if (uk_test_bit(fd, fdtable.bitmap)) {
		ret = -EBUSY;
		goto exit;
	}

	uk_bitmap_set(fdtable.bitmap, fd, 1);

exit:
	ukplat_lcpu_restore_irqf(flags);
	return ret;
}

int fdtable_put_fd(int fd)
{
	uintptr_t fp;
	unsigned long flags;

	UK_ASSERT(fd < (int) FDTABLE_MAX_FILES);

	/* FIXME Currently it is not allowed to free std(in|out|err):
	 * if (fd <= 2) return -EBUSY;
	 *
	 * However, returning -EBUSY in this case breaks dup2 with stdin, out,
	 * err. Ignoring this should be fine as long as those are not fdrop-ed
	 * twice, in which case the static fp would be freed, and here be
	 * dragons.
	 */

	flags = ukplat_lcpu_save_irqf();
	uk_bitmap_clear(fdtable.bitmap, fd, 1);
	fp = fdtable.files[fd];
	fdtable.files[fd] = NULL;
	ukplat_lcpu_restore_irqf(flags);

	/*
	 * Since we can alloc a fd without assigning a
	 * vfsfile we must protect against NULL ptr
	 */
	if (fp)
		fdrop(fp);

	return 0;
}

int fdtable_install_fd(int fd, uintptr_t file)
{
	unsigned long flags;
	uintptr_t orig;

	if ((fd >= (int) FDTABLE_MAX_FILES) || (!file))
		return -EBADF;

	fhold(file);

	file->fd = fd;

	flags = ukplat_lcpu_save_irqf();
	orig = fdtable.files[fd];
	fdtable.files[fd] = file;
	ukplat_lcpu_restore_irqf(flags);

	fdrop(file);

	if (orig)
		fdrop(orig);

	return 0;
}

uintptr_t fdtable_get_fileptr(int fd)
{
	unsigned long flags;
	uintptr_t ret = NULL;

	UK_ASSERT(fd < (int) FDTABLE_MAX_FILES);

	flags = ukplat_lcpu_save_irqf();
	if (!uk_test_bit(fd, fdtable.bitmap))
		goto exit;
	ret = fdtable.files[fd];
	fhold(ret);

exit:
	ukplat_lcpu_restore_irqf(flags);
	return ret;
}

void fdtable_put_file(uintptr_t file)
{
	fdrop(file);
}

int fget(int fd, uintptr_t *out_fp)
{
	int ret = 0;
	uintptr_t fp = fdtable_get_fileptr(fd);

	if (!fp)
		ret = EBADF;
	else
		*out_fp = fp;

	return ret;
}

int fdalloc(uintptr_t fp, int *newfd)
{
	int fd, ret = 0;

	fhold(fp);

	fd = fdtable_alloc_fd();
	if (fd < 0) {
		ret = fd;
		goto exit;
	}

	ret = fdtable_install_fd(fd, fp);
	if (ret)
		fdrop(fp);
	else
		*newfd = fd;

exit:
	return ret;
}

/* TODO: move this constructor to main.c */
static void fdtable_init(void)
{
	memset(&fdtable, 0, sizeof(fdtable));

	init_stdio();
}

UK_CTOR_PRIO(fdtable_init, 1);
