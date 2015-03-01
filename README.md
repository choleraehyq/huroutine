This repository contains a coroutine library, a non-preemptive user-thread library.
They have already been tested on Ubuntu14.04 amd64 and Debian7 amd64 compiled by clang3.4.1 and gcc4.8.2.

The huroutine.h/huroutine.c are the source of the coroutine library and the non-preemptive user-thread library called huroutine.
The non-preemptive user-thread library is based on the coroutine library, which was implemented with ucontext.h on linux. And the schedule algorithm is the simple round-robin algorithm.
The benchmark.c is the test program to test the coroutine-switching speed of the huroutine library. The speed is about 2300000 switches per second.

* the default stack of each huroutine is 4KB.
* a schedulor can schedule only 1000 huroutine at max by default.
* the initial capcity of the schedulor is 16 by default.

But you can change these options via adjusting the macro in huroutine.h

The nbio.h/nbio.c are the deadly simple non-blocking IO library. I have just implemented the non-blocking version of write and read.

The channel.h/channel.c are the channel library used to deliver variable between two huroutine. Attention: what the channel deliver is pointer, so make sure the memory pointer points to is always accessed. Don't use this to deliver a local variable.

The main.c is the demo of the librarys. The huroutine\_test() is the demo function of the former two librarys. 
