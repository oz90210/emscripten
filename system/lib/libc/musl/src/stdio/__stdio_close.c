#include "stdio_impl.h"
#ifdef __EMSCRIPTEN__
#include <wasi/wasi.h>
#endif

static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int __stdio_close(FILE *f)
{
#ifdef __EMSCRIPTEN__
	return __wasi_fd_close(__aio_close(f->fd)) != __WASI_ESUCCESS;
#else
	return syscall(SYS_close, __aio_close(f->fd));
#endif
}
