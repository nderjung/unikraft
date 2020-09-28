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

#include <sys/types.h>
#include <uk/socket.h>
#include <uk/errptr.h>
#include <uk/print.h>
#include <uk/trace.h>
#include <errno.h>

int
socket(int family, int type, int protocol)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
accept(int sock, struct sockaddr *restrict addr,
		socklen_t *restrict addr_len)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
bind(int sock, const struct sockaddr *addr, socklen_t addr_len)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
shutdown(int sock, int how)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
getpeername(int sock, struct sockaddr *restrict addr,
		socklen_t *restrict addr_len)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
getsockname(int sock, struct sockaddr *restrict addr,
		socklen_t *restrict addr_len)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
getsockopt(int sock, int level, int optname, void *restrict optval,
		socklen_t *restrict optlen)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
setsockopt(int sock, int level, int optname, const void *optval,
		socklen_t optlen)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
getnameinfo(const struct sockaddr *restrict sa, socklen_t sl,
		char *restrict node, socklen_t nodelen, char *restrict serv,
		socklen_t servlen, int flags)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
connect(int sock, const struct sockaddr *addr, socklen_t addr_len)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
listen(int sock, int backlog)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
recv(int sock, void *buf, size_t len, int flags)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
recvfrom(int sock, void *buf, size_t len, int flags,
		struct sockaddr *from, socklen_t *fromlen)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
recvmsg(int sock, struct msghdr *msg, int flags)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
send(int sock, const void *buf, size_t len, int flags)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
sendmsg(int sock, const struct msghdr *msg, int flags)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

ssize_t
sendto(int sock, const void *buf, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen)
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}

int
socketpair(int family, int type, int protocol, int usockfd[2])
{
	uk_pr_crit("%s: not implemented\n", __func__);
	errno = ENOTSUP;
	return -1;
}
