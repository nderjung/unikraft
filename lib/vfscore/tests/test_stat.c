/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Sharan Santhanam <sharan.santhanam@neclab.eu>
 *          Alexander Jung <alex@unikraft.io>
 *
 * Copyright (c) 2020, NEC Laboratories Europe GmbH, NEC Corporation.
 *               2022, Unikraft GmbH.
 *               All rights reserved.
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

#include <uk/test.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>

typedef struct vfscore_stat {
	int rc;
	int errcode;
	char *filename;
} vfscore_stat_t;

static vfscore_stat_t test_stats [] = {
	{ .rc = 0,	.errcode = 0,		.filename = "/foo/file.txt" },
	{ .rc = -1,	.errcode = EINVAL,	.filename = NULL },
};

static int fd;

UK_TESTCASE(vfscore_stat_testsuite, vfscore_test_newfile)
{
	int ret = mount("", "/", "ramfs", 0, NULL);
	UK_TEST_EXPECT_ZERO(ret);

	ret = mkdir("/foo", S_IRWXU);
	UK_TEST_EXPECT_ZERO(ret);

	fd = open("/foo/file.txt", O_WRONLY | O_CREAT, S_IRWXU);
	UK_TEST_EXPECT_SNUM_GT(fd, 2);

	UK_TEST_EXPECT_SNUM_EQ(
		write(fd, "hello\n", sizeof("hello\n")),
		sizeof("hello\n")
	);
	fsync(fd);
}

UK_TESTCASE(vfscore_stat_testsuite, vfscore_test_stat)
{
	int rc;
	struct stat buf;
	long unsigned int i;

	vfscore_stat_t *ptr = &test_stats[0];

	for (i = 0; i < sizeof(test_stats) / sizeof(test_stats[0]); i++, ptr++) {
		errno = 0;
		rc = stat(ptr->filename, &buf);
		UK_TEST_EXPECT_SNUM_EQ(rc, ptr->rc);
		rc = errno;
		UK_TEST_EXPECT_SNUM_EQ(rc, ptr->errcode);
	}

	close(fd);
}

uk_testsuite_register(vfscore_stat_testsuite, NULL);
