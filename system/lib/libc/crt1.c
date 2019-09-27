/*
 * Copyright 2018 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

// Much of this code comes from:
// https://github.com/CraneStation/wasi-libc/blob/master/libc-bottom-half/crt/crt1.c

#include <stdlib.h>
#include <sysexits.h>
#include <wasi/wasi.h>
#include <stdio.h>

extern void __wasm_call_ctors(void) __attribute__((weak));
extern int main(int argc, char** argv);

static __wasi_errno_t populate_args(size_t *argc, char ***argv) {
  __wasi_errno_t err;

  /* Get the sizes of the arrays we'll have to create to copy in the args. */
  size_t argv_buf_size;
  err = __wasi_args_sizes_get(argc, &argv_buf_size);
  if (err != __WASI_ESUCCESS) {
    return err;
  }
  if (*argc == 0) {
    return __WASI_ESUCCESS;
  }

  /* Allocate memory for the array of pointers, adding null terminator. */
  *argv = malloc(sizeof(char *) * (*argc + 1));
  /* Allocate memory for storing the argument chars. */
  char *argv_buf = malloc(sizeof(char) * argv_buf_size);
  if (*argv == NULL || argv_buf == NULL) {
    return __WASI_ENOMEM;
  }

  /* Make sure the last pointer in the array is NULL. */
  (*argv)[*argc] = NULL;

  /* Fill the argument chars, and the argv array with pointers into those chars. */
  return __wasi_args_get(*argv, argv_buf);
}

void _start(void) {
  /* Fill in the arguments from WASI syscalls. */
  size_t argc;
  char **argv;
  if (populate_args(&argc, &argv) != __WASI_ESUCCESS) {
    __wasi_proc_exit(EX_OSERR);
  }

  /* The linker synthesizes this to call constructors. */
  if (__wasm_call_ctors)
    __wasm_call_ctors();

  int r = main(argc, argv);

  /* If main exited successfully, just return, otherwise call _Exit.
   * TODO(sbc): switch to _Exit */
  if (r != 0) {
    __wasi_proc_exit(r);
  }
}
