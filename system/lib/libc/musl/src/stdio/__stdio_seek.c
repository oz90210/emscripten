#include "stdio_impl.h"
#ifdef __EMSCRIPTEN__
#include <wasi/wasi.h>
#endif

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
	off_t ret;
#ifdef SYS__llseek
#ifdef __EMSCRIPTEN__
  if (whence == SEEK_SET) {
		whence = __WASI_WHENCE_SET;
	} else if (whence == SEEK_CUR) {
		whence = __WASI_WHENCE_CUR;
	} else if (whence == SEEK_END) {
		whence = __WASI_WHENCE_END;
	}
	if (__wasi_fd_seek(f->fd, off, whence, &ret) != __WASI_ESUCCESS)
#else
	if (syscall(SYS__llseek, f->fd, off>>32, off, &ret, whence)<0)
#endif // __EMSCRIPTEN__
		ret = -1;
#else
	ret = syscall(SYS_lseek, f->fd, off, whence);
#endif
	return ret;
}
