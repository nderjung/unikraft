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
#ifndef __UK_UNIXSOCK__
#define __UK_UNIXSOCK__

#include <uk/mbox.h>
#include <uk/ring.h>
#include <uk/mutex.h>
#include <uk/semaphore.h>

#include <sys/un.h>

UK_RING_DEFINE(unixsock_ring, char);

struct unixsock;

struct unixsock {
  /* The socket family's assigned address. */
  struct sockaddr_un local_addr;
  /* Connection queue. */
  struct uk_mbox *connq;
#if CONFIG_LIBUKSCHED
  /* Wait-queue for state changes. */
  struct uk_waitq state_wq;
#endif
  /* Inbound connections wait timeout.  */
  struct uk_semaphore connections;
  /* Socket state. */
  uint32_t state;
  /* Socket option flags. */
  uint32_t flags;
  /*  */
  struct linger *linger;
  /* Data buffer between connected sockets. */
  unixsock_ring_t *buffer;
  /* Buffer debug lock. */
#ifdef DEBUG_BUFRING
  struct uk_mutex *buflock;
#endif
  /* A reference to the peer socket. */
  struct unixsock *peer;
};


#endif /* __UK_UNIXSOCK__ */
