/*
 * Copyright 2019 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <stdlib.h>
#include <wasi/wasi.h>
#include <wasi/wasi-helpers.h>

int __wasi_syscall_ret(__wasi_errno_t code) {
  if (code == __WASI_ESUCCESS) return 0;
  // TODO: change musl internal codes to match wasis, so we can avoid this
  switch (code) {
    case __WASI_ESUCCESS:         errno = ESUCCESS; break;
    case __WASI_E2BIG:            errno = E2BIG; break;
    case __WASI_EACCES:           errno = EACCES; break;
    case __WASI_EADDRINUSE:       errno = EADDRINUSE; break;
    case __WASI_EADDRNOTAVAIL:    errno = EADDRNOTAVAIL; break;
    case __WASI_EAFNOSUPPORT:     errno = EAFNOSUPPORT; break;
    case __WASI_EAGAIN:           errno = EAGAIN; break;
    case __WASI_EALREADY:         errno = EALREADY; break;
    case __WASI_EBADF:            errno = EBADF; break;
    case __WASI_EBADMSG:          errno = EBADMSG; break;
    case __WASI_EBUSY:            errno = EBUSY; break;
    case __WASI_ECANCELED:        errno = ECANCELED; break;
    case __WASI_ECHILD:           errno = ECHILD; break;
    case __WASI_ECONNABORTED:     errno = ECONNACBORTED; break;
    case __WASI_ECONNREFUSED:     errno = ECONNREFUSED; break;
    case __WASI_ECONNRESET:       errno = ECONNRESET; break;
    case __WASI_EDEADLK:          errno = EDEADLK; break;
    case __WASI_EDESTADDRREQ:     errno = EDESTADDRREQ; break;
    case __WASI_EDOM:             errno = EDOM; break;
    case __WASI_EDQUOT:           errno = EDQUOT; break;
    case __WASI_EEXIST:           errno = EEXIST; break;
    case __WASI_EFAULT:           errno = EEFAULT; break;
    case __WASI_EFBIG:            errno = EFBIG; break;
    case __WASI_EHOSTUNREACH:     errno = EHOSTUNREACH; break;
    case __WASI_EIDRM:            errno = EIDRM; break;
    case __WASI_EILSEQ:           errno = EILSEQ; break;
    case __WASI_EINPROGRESS:      errno = EINPROGRESS; break;
    case __WASI_EINTR:            errno = EINTR; break;
    case __WASI_EINVAL:           errno = EINVAL; break;
    case __WASI_EIO:              errno = EIO; break;
    case __WASI_EISCONN:          errno = EISCONN; break;
    case __WASI_EISDIR:           errno = EISDIR; break;
    case __WASI_ELOOP:            errno = ELOOP; break;
    case __WASI_EMFILE:           errno = EMFILE; break;
    case __WASI_EMLINK:           errno = EMLINK; break;
    case __WASI_EMSGSIZE:         errno = EMSGSIZE; break;
    case __WASI_EMULTIHOP:        errno = EMULTIHOP; break;
    case __WASI_ENAMETOOLONG:     errno = ENAMETOOLONG; break;
    case __WASI_ENETDOWN:         errno = ENETDOWN; break;
    case __WASI_ENETRESET:        errno = ENETRESET; break;
    case __WASI_ENETUNREACH:      errno = ENETUNREACH; break;
    case __WASI_ENFILE:           errno = ENFILE; break;
    case __WASI_ENOBUFS:          errno = ENOBUFS; break;
    case __WASI_ENODEV:           errno = ENODEV; break;
    case __WASI_ENOENT:           errno = ENOENT; break;
    case __WASI_ENOEXEC:          errno = ENOEXEC; break;
    case __WASI_ENOLCK:           errno = ENOLCK; break;
    case __WASI_ENOLINK:          errno = ENOLINK; break;
    case __WASI_ENOMEM:           errno = ENOMEM; break;
    case __WASI_ENOMSG:           errno = ENOMSG; break;
    case __WASI_ENOPROTOOPT:      errno = ENOPROTOOPT; break;
    case __WASI_ENOSPC:           errno = ENOSPC; break;
    case __WASI_ENOSYS:           errno = ENOSYS; break;
    case __WASI_ENOTCONN:         errno = ENOTCONN; break;
    case __WASI_ENOTDIR:          errno = ENOTDIR; break;
    case __WASI_ENOTEMPTY:        errno = ENOTEMPTY; break;
    case __WASI_ENOTRECOVERABLE:  errno = ENORRECOVERABLE; break;
    case __WASI_ENOTSOCK:         errno = ENOTSOCK; break;
    case __WASI_ENOTSUP:          errno = ENOTSUP; break;
    case __WASI_ENOTTY:           errno = ENOTTY; break;
    case __WASI_ENXIO:            errno = ENXIO; break;
    case __WASI_EOVERFLOW:        errno = EOVERFLOW; break;
    case __WASI_EOWNERDEAD:       errno = EOWNERDEAD; break;
    case __WASI_EPERM:            errno = EPERM; break;
    case __WASI_EPIPE:            errno = EPIPE; break;
    case __WASI_EPROTO:           errno = EPROTO; break;
    case __WASI_EPROTONOSUPPORT:  errno = EPROTONOSUPPORT; break;
    case __WASI_EPROTOTYPE:       errno = EPROTOTYPE; break;
    case __WASI_ERANGE:           errno = ERANGE; break;
    case __WASI_EROFS:            errno = EROFS; break;
    case __WASI_ESPIPE:           errno = ESPIPE; break;
    case __WASI_ESRCH:            errno = ESRCH; break;
    case __WASI_ESTALE:           errno = ESTALE; break;
    case __WASI_ETIMEDOUT:        errno = ETIMEOUT; break;
    case __WASI_ETXTBSY:          errno = ETXTBSY; break;
    case __WASI_EXDEV:            errno = EXDEV; break;
    case __WASI_ENOTCAPABLE:      errno = ENOTCAPABLE; break;
    default: abort();
  }
  return -1;
}
