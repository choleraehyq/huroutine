#ifndef _UTHREAD_H
#define _UTHREAD_H

#include <signal.h>
#include <sys/syscall.h>
#include "huroutine.h"

#define TIMER_SIG SIGUSR1
#define gettid() syscall(SYS_gettid)

void uthread_init(pid_t);
void uthread_create(huroutine_func, void *);
void uthread_schedule(void);

#endif
