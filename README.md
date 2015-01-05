This repository involve a coroutine library and a simple preemptive user-thread library.

The huroutine.h/huroutine.c are the source of the coroutine library called huroutine , it was implemented with ucontext.h on linux. It has been tested on Ubuntu14.04 amd64 and Debian7 amd64 compiled by clang3.4.1 and gcc 4.8.2.
* the default stack of each huroutine is 4KB.
* a schedulor can only schedule 1000 huroutine at max by default.
* the initial capcity of the schedulor is 16 by default.
But you can change these option via adjusting the macro in huroutine.h

The uthread.h/uthread.c are the source of the preemptive user-thread library, it is based on the coroutine library above. I implement the library with sigaction and posix timer. The schedule algorithm is the simple round-robin algorithm.

main.c is the demo of the uthread library. if it run correctly, it should print "shit1" to "shit100" and "fuck1" to "fuck100" alternately.

Now the problem is: main.c can be compiled successfully, but it cannot print anything when running, in the other word, the sigaction or timer isn't installed successfully. 
