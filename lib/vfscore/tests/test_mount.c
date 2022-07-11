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
#include <sys/stat.h>
#include <sys/mount.h>

UK_TESTCASE(vfscore_mount_testsuite, vfscore_test_multimount)
{
	UK_TEST_EXPECT_SNUM_EQ(mkdir("/sys", S_IRWXU), 0);
	UK_TEST_EXPECT_SNUM_EQ(mount("", "/sys", "ramfs", 0, NULL), 0);
#ifdef CONFIG_LIBDEVFS_AUTOMOUNT
	UK_TEST_EXPECT_SNUM_EQ(mkdir("/dev", S_IRWXU), -1);
	UK_TEST_EXPECT_SNUM_NQ(mount("", "/dev", "devfs", 0, NULL), 0);
#else
	UK_TEST_EXPECT_SNUM_EQ(mkdir("/dev", S_IRWXU), 0);
	UK_TEST_EXPECT_SNUM_EQ(mount("", "/dev", "devfs", 0, NULL), 0);
#endif /* CONFIG_LIBDEVFS_AUTOMOUNT */
	UK_TEST_EXPECT_SNUM_EQ(mkdir("/tmp", S_IRWXU), 0);
	UK_TEST_EXPECT_SNUM_EQ(mount("", "/tmp", "naivefs", 0, NULL), 0);
	UK_TEST_EXPECT_SNUM_NQ(mount("", "/tmp", "naivetmpfs", 0, NULL), 0);

#ifdef CONFIG_LIBUKSWRAND
	int rc;
	char buffer[10];

	UK_TEST_EXPECT_SNUM_GE(rc = open("/dev/random", O_RDONLY), 0);
	UK_TEST_EXPECT_SNUM_GT(read(rc, &buffer, sizeof(buffer)), 0);
#endif /* CONFIG_LIBUKSWRAND */
}

uk_testsuite_register(vfscore_mount_testsuite, NULL);
