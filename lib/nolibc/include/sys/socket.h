/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Alexander Jung <alexander.jung@neclab.eu>
 *
 * Copyright (c) 1982, 1985, 1986, 1988, 1993, 1994
 *         The Regents of the University of California.  All rights reserved.
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
/* Derived from OpenBSD commit 15b62b7 (22 Jul 2019) */

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <uk/config.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __NEED_socklen_t
#define __NEED_sa_family_t

#include <nolibc-internal/shareddefs.h>

#if CONFIG_LIBPOSIX_SOCKET

/*
 * Types
 */
#define SOCK_STREAM      1  /* stream socket */
#define SOCK_DGRAM       2  /* datagram socket */
#define SOCK_RAW         3  /* raw-protocol interface */
#define SOCK_RDM         4  /* reliably-delivered message */
#define SOCK_SEQPACKET	 5  /* sequenced packet stream */

/*
 * Unikraft defines a definitive list of protocol families which can be
 * registered by uksocket.  Newer or non-standard protocols should have their
 * respective AF_ number defined in the library and greater than what is 
 * defined here.
 */
/*
 * Address families.s
 */
#define AF_UNSPEC        0        /* unspecified */
#define AF_UNIX          1        /* local to host */
#define AF_LOCAL         AF_UNIX  /* draft POSIX compatibility */
#define AF_INET          2        /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK       3        /* arpanet imp addresses */
#define AF_PUP           4        /* pup protocols: e.g. BSP */
#define AF_CHAOS         5        /* mit CHAOS protocols */
#define AF_NS            6        /* XEROX NS protocols */
#define AF_ISO           7        /* ISO protocols */
#define AF_OSI           AF_ISO
#define AF_ECMA          8        /* european computer manufacturers */
#define AF_DATAKIT       9        /* datakit protocols */
#define AF_CCITT         10       /* CCITT protocols, X.25 etc */
#define AF_SNA           11       /* IBM SNA */
#define AF_DECnet        12       /* DECnet */
#define AF_DLI           13       /* DEC Direct data link interface */
#define AF_LAT           14       /* LAT */
#define AF_HYLINK        15       /* NSC Hyperchannel */
#define AF_APPLETALK     16       /* Apple Talk */
#define AF_ROUTE         17       /* Internal Routing Protocol */
#define AF_LINK          18       /* Link layer interface */
#define pseudo_AF_XTP    19       /* eXpress Transfer Protocol (no AF) */
#define AF_COIP          20       /* connection-oriented IP, aka ST II */
#define AF_CNT           21       /* Computer Network Technology */
#define pseudo_AF_RTIP   22       /* Help Identify RTIP packets */
#define AF_IPX           23       /* Novell Internet Protocol */
#define AF_INET6         24       /* IPv6 */
#define pseudo_AF_PIP    25       /* Help Identify PIP packets */
#define AF_ISDN          26       /* Integrated Services Digital Network*/
#define AF_E164          AF_ISDN  /* CCITT E.164 recommendation */
#define AF_NATM          27       /* native ATM access */
#define AF_ENCAP         28
#define AF_SIP           29       /* Simple Internet Protocol */
#define AF_KEY           30
#define pseudo_AF_HDRCM  31       /* Used by BPF to not rewrite headers 
                                   * in interface output routine */
#define AF_BLUETOOTH     32       /* Bluetooth */
#define AF_MPLS          33       /* MPLS */
#define pseudo_AF_PFLOW  34       /* pflow */
#define pseudo_AF_PIPEX  35       /* PIPEX */
#define AF_MAX           36

/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC        AF_UNSPEC
#define PF_LOCAL         AF_LOCAL
#define PF_UNIX          AF_UNIX
#define PF_INET          AF_INET
#define PF_IMPLINK       AF_IMPLINK
#define PF_PUP           AF_PUP
#define PF_CHAOS         AF_CHAOS
#define PF_NS            AF_NS
#define PF_ISO           AF_ISO
#define PF_OSI           AF_ISO
#define PF_ECMA          AF_ECMA
#define PF_DATAKIT       AF_DATAKIT
#define PF_CCITT         AF_CCITT
#define PF_SNA           AF_SNA
#define PF_DECnet        AF_DECnet
#define PF_DLI           AF_DLI
#define PF_LAT           AF_LAT
#define PF_HYLINK        AF_HYLINK
#define PF_APPLETALK     AF_APPLETALK
#define PF_ROUTE         AF_ROUTE
#define PF_LINK          AF_LINK
#define PF_XTP           pseudo_AF_XTP   /* really just proto family, no AF */
#define PF_COIP          AF_COIP
#define PF_CNT           AF_CNT
#define PF_IPX           AF_IPX          /* same format as AF_NS */
#define PF_INET6         AF_INET6
#define PF_RTIP          pseudo_AF_RTIP  /* same format as AF_INET */
#define PF_PIP           pseudo_AF_PIP
#define PF_ISDN          AF_ISDN
#define PF_NATM          AF_NATM
#define PF_ENCAP         AF_ENCAP
#define PF_SIP           AF_SIP
#define PF_KEY           AF_KEY
#define PF_BPF           pseudo_AF_HDRCMPLT
#define PF_BLUETOOTH     AF_BLUETOOTH
#define PF_MPLS          AF_MPLS
#define PF_PFLOW         pseudo_AF_PFLOW
#define PF_PIPEX         pseudo_AF_PIPEX
#define PF_MAX           AF_MAX

/*
 * These are the valid values for the "how" field used by shutdown(2).
 */
#define SHUT_RD          0
#define SHUT_WR          1
#define SHUT_RDWR        2

/*
 * Maximum queue length specifiable by listen(2).
 */
#define SOMAXCONN        128

struct msghdr;

struct cmsghdr;

#endif /* CONFIG_LIBPOSIX_SOCKET */

#ifdef __cplusplus
extern }
#endif

#endif /* _SYS_SOCKET_H */
