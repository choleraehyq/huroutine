This repository involve a coroutine library, a non-preemptive user-thread library, a deadly simple nonblocking io library and a simple preemptive user-thread library.

The huroutine.h/huroutine.c are the source of the coroutine library and the non-preemptive user-thread library called huroutine.
The non-preemptive user-thread library is based on the coroutine library, which was implemented with ucontext.h on linux. And the schedule algorithm is the simple round-robin algorithm.

It has been tested on Ubuntu14.04 amd64 and Debian7 amd64 compiled by clang3.4.1 and gcc 4.8.2.

* the default stack of each huroutine is 4KB.
* a schedulor can schedule only 1000 huroutine at max by default.
* the initial capcity of the schedulor is 16 by default.

But you can change these options via adjusting the macro in huroutine.h

The uthread.h/uthread.c are the source of the preemptive user-thread library, it is also based on the coroutine library above. I implement the library with sigaction and posix timer. The schedule algorithm is the simple round-robin algorithm too.

The main.c is the demo of the librarys. The huroutine\_test() is the demo function of the former two librarys and the uthread\_test() is the demo function of the latter library. 

Now the problem is: When I test uthread\_test(), main.c can be compiled successfully, but it cannot print anything while running, and if it run correctly, it should print "shit1" to "shit100" and "fuck1" to "fuck100" alternately.In the other word, the sigaction or timer isn't installed successfully. 

The nbio.c/nbio.h are the source of the simple non-blocking io library fitting the library huroutine. I only implement the non-blocking version of read() and write().
