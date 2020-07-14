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

#ifndef __UK_FD_DRIVER_H__
#define __UK_FD_DRIVER_H__

struct uk_fd_ops;
struct uk_fd_driver;

struct uk_fd_driver {
  /* The library registering the file descriptor driver. */
  const char *libname;
  /* The interfaces for this file descriptor driver. */
	const struct uk_fd_ops *ops;
  /* The memory allocator to be used for this file descriptor driver */
  struct uk_alloc *allocator;
  /* Entry for list of socket drivers. */
	UK_TAILQ_ENTRY(struct uk_fd_driver) _list;
};


/**
 * The initialization function called for this file descriptor.  It's here that
 * additional configuration for the driver can be made after it has been
 * registered.  For instance, an alternative memory allocator can be provided.
 *
 * @param d
 *  The socket driver.
 */
typedef int (*uk_fd_driver_init_func_t)(struct uk_fd_driver *d);


/**
 * Read from a file descriptor.
 *
 * @param driver
 *  The file descriptor driver.
 * @param buf
 *  The buffer to read the data from the file descriptor into.
 * @param count
 *  The number of bytes to be read.
 */
typedef int (*uk_fd_read_func_t)(struct uk_fd_driver *d,
          void *fd, void *buf, size_t count);


/**
 * Write to a file descriptor.
 *
 * @param driver
 *  The file descriptor driver.
 * @param buf
 *  The pointer to the buffer to write to the file descriptor.
 * @param count
 *  The number of bytes to be written.
 */
typedef int (*uk_fd_write_func_t)(struct uk_fd_driver *d,
          void *fd, const void *buf, size_t count);

/**
 * Close the file descriptor.
 *
 * @param driver
 *  The file descriptor driver.
 * @param sock
 *  Reference to the file descriptor.
 */
typedef int (*uk_fd_close_func_t)(struct uk_fd_driver *d,
          void *fd);


/**
 * Manipulate the socket.
 *
 * @param driver
 *  The socket driver.
 * @TODO
 */
typedef int (*uk_fd_ioctl_func_t)(struct uk_fd_driver *d,
          void *fd, int request, void *argp);


struct uk_fd_ops {
  /* The initialization function on driver registration. */
  uk_fd_driver_init_func_t  init;
  /* POSIX interfaces */
  uk_fd_read_func_t         read;
  uk_fd_write_func_t        write;
  uk_fd_close_func_t        close;
  uk_fd_ioctl_func_t        ioctl;
};


static inline int
uk_fd_do_read(struct uk_fd_driver *d,
          void *fd, void *buf, size_t count)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->read);
  return d->ops->read(d, fd, buf, count);
}

static inline int
uk_fd_read(struct uk_fd_driver *d,
          void *fd, void *buf, size_t count)
{
  if (unlikely(!d))
    return -ENOSYS;

  return uk_fd_do_read(d, fd, buf, count);
}


static inline int
uk_fd_do_write(struct uk_fd_driver *d,
          void *fd, const void *buf, size_t count)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->write);
  return d->ops->write(d, fd, buf, count); 
}

static inline int
uk_fd_write(struct uk_fd_driver *d,
          void *fd, const void *buf, size_t count)
{
  if (unlikely(!d))
    return -ENOSYS;

  return uk_fd_do_write(d, fd, buf, count);
}


static inline int
uk_fd_do_close(struct uk_fd_driver *d,
          void *fd)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->close);
  return d->ops->close(d, fd);
}

static inline int
uk_fd_close(struct uk_fd_driver *d,
          void *fd)
{
  if (unlikely(!d))
    return -ENOSYS;

  return uk_fd_do_close(d, fd);
}


static inline int
uk_fd_do_ioctl(struct uk_fd_driver *d,
          void *fd, int request, void *argp)
{
  UK_ASSERT(d);
  UK_ASSERT(d->ops->ioctl);
  return d->ops->ioctl(d, fd, request, argp);
}

static inline int
uk_fd_ioctl(struct uk_fd_driver *d,
          void *fd, int request, void *argp)
{
  if (unlikely(!d))
    return -ENOSYS;

  return uk_fd_do_ioctl(d, fd, request, argp);
}


#endif /* __UK_FD_DRIVER_H__ */