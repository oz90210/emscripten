.. Pthreads support:

==============================
Pthreads support
==============================

Emscripten has support for multithreading using SharedArrayBuffer in browsers. That API allows sharing memory between the main thread and web workers as well as atomic operations for synchronization, which enables Emscripten to implement support for the Pthreads (POSIX threads) API. This support is considered stable in Emscripten.

.. note:: As of Sep 2019, some browsers have disabled SharedArrayBuffer due to
          the Spectre set of vulnerabilities. Until it is restored you can still
          experiment with it if you flip a pref in those browsers. In other
          browsers (like Chrome on desktop), SharedArrayBuffer is fully enabled
          by default and you don't need to flip any flags.

Compiling with pthreads enabled
===============================

By default, support for pthreads is not enabled. To enable code generation for pthreads, the following command line flags exist:

- Pass the compiler flag ``-s USE_PTHREADS=1`` when compiling any .c/.cpp files, AND when linking to generate the final output .js file.
- Optionally, pass the linker flag ``-s PTHREAD_POOL_SIZE=<integer>`` to specify a predefined pool of web workers to populate at page preRun time before application main() is called. This is important because if the workers do not already exist then we may need to wait for the next browser event iteration for certain things, see below. (If -1 is passed to both PTHREAD_POOL_SIZE and PTHREAD_HINT_NUM_CORES, then a popup dialog will ask the user the size of the pool, which is useful for testing.)
- Optionally, pass the linker flag ``-s PTHREAD_HINT_NUM_CORES=<integer>`` to choose what the function emscripten_num_logical_cores(); will return if navigator.hardwareConcurrency is not supported. If -1 is specified here, a popup dialog will be shown at startup to let the user specify the value that is returned here. This can be helpful in order to dynamically test how an application behaves with different values here.

There should be no other changes required. In C/C++ code, the preprocessor check ``#ifdef __EMSCRIPTEN_PTHREADS__`` can be used to detect whether Emscripten is currently targeting pthreads.

.. note:: It is not possible to build one binary that would be able to leverage
    multithreading when available and fall back to single threaded when not. The
    best you can do is two separate builds, one with and one
    without threads, and pick between them at runtime.

Additional flags
================

- ``-s PROXY_TO_PTHREAD``: In this mode your original ``main()`` is replaced by
  a new one that creates a pthread and runs the original ``main()`` on it. As a
  result, your application's ``main()`` is run off the browser main (UI) thread,
  which is good for responsiveness. The browser main thread does still run code
  when things are proxied to it, for example to handle events, rendering, etc.

Note that Emscripten has the
``--proxy-to-worker`` :ref:`linker flag <proxy-to-worker>` which sounds similar
but is unrelated. That flag does not use pthreads or SharedArrayBuffer, and
instead uses a plain Web Worker to run your main program (and postMessage to
proxy messages back and forth).

Special considerations
======================

The Emscripten implementation for the pthreads API should follow the POSIX standard closely, but some behavioral differences do exist:

- When the linker flag ``-s PTHREAD_POOL_SIZE=<integer>`` is not specified and ``pthread_create()`` is called, the new thread will not start until control is yielded back to the browser's main event loop, because the web worker cannot be created while JS or wasm code is running. This is a violation of POSIX behavior and will break common code which creates a thread and immediately joins it or otherwise synchronously waits to observe an effect such as a memory write. Using a pool creates the web workers before main is called, allowing thread creation to be synchronous.

- Browser DOM access is only possible on the main browser thread, and therefore things that may access the DOM, like filesystem operations (``fopen()``, etc.) or changing the HTML page's title, etc., are proxied over to the main browser thread. This proxying can generate a deadlock in a special situation that native code running pthreads does not have. See `bug 3495 <https://github.com/emscripten-core/emscripten/issues/3495>`_ for more information and how to work around this until this proxying is no longer needed in Emscripten. (To check which operations are proxied, you can look for the function's implementation in the JS library (``src/library_*``) and see if it is annotated with ``__proxy: 'sync'`` or ``__proxy: 'async'``.)

- When doing a futex wait, e.g. ``usleep()``, ``emscripten_futex_wait()``, or ``pthread_mutex_lock()``, we use ``Atomics.wait`` on workers, which the browser should do pretty efficiently. But that is not available on the main thread, and so we busy-wait there. Busy-waiting is not recommended because it freezes the tab, and also wastes power.

- The Emscripten implementation does not support `POSIX signals <http://man7.org/linux/man-pages/man7/signal.7.html>`_, which are sometimes used in conjunction with pthreads. This is because it is not possible to send signals to web workers and pre-empt their execution. The only exception to this is pthread_kill() which can be used as normal to forcibly terminate a running thread.

- The Emscripten implementation does also not support multiprocessing via ``fork()`` and ``join()``.

- For web security purposes, there exists a fixed limit (by default 20) of threads that can be spawned when running in Firefox Nightly. `#1052398 <https://bugzilla.mozilla.org/show_bug.cgi?id=1052398>`_. To adjust the limit, navigate to about:config and change the value of the pref "dom.workers.maxPerDomain".

- Some of the features in the pthreads specification are unsupported since the upstream musl library that Emscripten utilizes does not support them, or they are marked optional and a conformant implementation need not support them. Such unsupported features in Emscripten include prioritization of threads, and pthread_rwlock_unlock() is not performed in thread priority order. The functions pthread_mutexattr_set/getprotocol(), pthread_mutexattr_set/getprioceiling() and pthread_attr_set/getscope() are no-ops.

- One particular note to pay attention to when porting is that sometimes in existing codebases the callback function pointers to pthread_create() and pthread_cleanup_push() omit the void* argument, which strictly speaking is undefined behavior in C/C++, but works in several x86 calling conventions. Doing this in Emscripten will issue a compiler warning, and can abort at runtime when attempting to call a function pointer with incorrect signature, so in the presence of such errors, it is good to check the signatures of the thread callback functions.

- Note that the function emscripten_num_logical_cores() will always return the value of navigator.hardwareConcurrency, i.e. the number of logical cores on the system, even when shared memory is not supported. This means that it is possible for emscripten_num_logical_cores() to return a value greater than 1, while at the same time emscripten_has_threading_support() can return false. The return value of emscripten_has_threading_support() denotes whether the browser has shared memory support available.

- Pthreads + memory growth (``ALLOW_MEMORY_GROWTH``) is especially tricky, see `wasm design issue #1271 <https://github.com/WebAssembly/design/issues/1271>`_. This currently causes JS accessing the wasm memory to be slow - but this will likely only be noticeable if the JS does large amounts of memory reads and writes (wasm runs at full speed, so moving work over can fix this). This also requires that your JS be aware that the HEAP* views may need to be updated - use the ``GROWABLE_HEAP_*`` helper functions which automatically handle that for you.

Also note that when compiling code that uses pthreads, an additional JavaScript file ``NAME.worker.js`` is generated alongside the output .js file (where ``NAME`` is the basename of the main file being emitted). That file must be deployed with the rest of the generated code files. By default, ``NAME.worker.js`` will be loaded relative to the main HTML page URL. If it is desirable to load the file from a different location e.g. in a CDN environment, then one can define the ``Module.locateFile(filename)`` function in the main HTML ``Module`` object to return the URL of the target location of the ``NAME.worker.js`` entry point. If this function is not defined in ``Module``, then the default location relative to the main HTML file is used.

Running code and tests
======================

Any code that is compiled with pthreads support enabled will currently only work in the Firefox Nightly channel, since the SharedArrayBuffer specification is still in an experimental research stage before standardization. There exists two test suites that can be used to verify the behavior of the pthreads API implementation in Emscripten:

- The Emscripten unit test suite contains several pthreads-specific tests in the "browser." suite. Run any of the tests named browser.test_pthread_*.

- An Emscripten-specialized version of the `Open POSIX Test Suite <http://posixtest.sourceforge.net/>`_ is available at `juj/posixtestsuite <https://github.com/juj/posixtestsuite>`_ GitHub repository. This suite contains about 300 tests for pthreads conformance. To run this suite, the pref dom.workers.maxPerDomain should first be increased to at least 50.

Please check these first in case of any issues. Bugs can be reported to the Emscripten bug tracker as usual.
