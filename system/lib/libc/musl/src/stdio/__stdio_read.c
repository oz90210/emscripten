#include "stdio_impl.h"
#include <sys/uio.h>
#ifdef __EMSCRIPTEN__
#include <wasi/wasi.h>
#endif

size_t __stdio_read(FILE *f, unsigned char *buf, size_t len)
{
	struct iovec iov[2] = {
		{ .iov_base = buf, .iov_len = len - !!f->buf_size },
		{ .iov_base = f->buf, .iov_len = f->buf_size }
	};
	ssize_t cnt;

#if __EMSCRIPTEN__
	size_t num;
	__wasi_errno_t err = __wasi_fd_read(f->fd, (struct __wasi_iovec_t*)iov, 2, &num);
	if (err != __WASI_ESUCCESS) {
		num = -1;
	}
	cnt = num;
#else
	cnt = syscall(SYS_readv, f->fd, iov, 2);
#endif
	if (cnt <= 0) {
		f->flags |= F_EOF ^ ((F_ERR^F_EOF) & cnt);
		return cnt;
	}
	if (cnt <= iov[0].iov_len) return cnt;
	cnt -= iov[0].iov_len;
	f->rpos = f->buf;
	f->rend = f->buf + cnt;
	if (f->buf_size) buf[len-1] = *f->rpos++;
	return len;
}
