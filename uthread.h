#ifndef _UTHREAD_H
#define _UTHREAD_H

#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "huroutine.h"

#define TIMER_SIG SIGRTMAX
#define gettid() syscall(SYS_gettid)

void init_uthread(pid_t);
void new_uthread(huroutine_func, void *);
void uthread_waitall(void);

#endif
