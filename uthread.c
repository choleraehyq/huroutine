#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "huroutine.h"
#include "uthread.h"
#include "linklist.h"

schedule_t *sche;
struct itimerspec ts;
struct sigaction sa;
struct sigevent sev;
sigset_t set;
timer_t tid;
node *cur;

void _handler(int sig) {
	if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
		errexit("sig block in _handler error");
	}
	huroutine_yield(sche);
	while (sche->schequeue->next != NULL) {
		if (sche->currunning->next == NULL) {
			sche->currunning = sche->schequeue->next;
		}
		else 
			sche->currunning = sche->currunning->next;
		enum huroutine_state curstate = 
			huroutine_status(sche, sche->currunning->hid);
		if (curstate == DEAD) {
			delete_linklist(sche->schequeue, sche->currunning);
		}
		else if (curstate == SUSPEND || curstate == READY) {
			if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
				errexit("sig unblock in _handler error");
			}
			huroutine_resume(sche, sche->currunning->hid);
			return;
		}
	}	
}

void _sig_timer_init(pid_t pid) {
	if (sigemptyset(&set) == -1) {
		errexit("sigemptyset to set in _sig_timer_init error");
	}
	if (sigaddset(&set, TIMER_SIG) == -1) {
		errexit("sigaddset to set in _sig_timer_init error");
	}

	sa.sa_flags = SA_NODEFER;
	sa.sa_handler = &_handler;
	if (sigemptyset(&sa.sa_mask) == -1) {
		errexit("sigemptymask to sa_handler in _sig_timer_init error");
	}
	if (sigaction(TIMER_SIG, &sa, NULL) == -1) {
		errexit("sigaction in _sig_timer_init error");
	}

	sev.sigev_notify = SIGEV_THREAD_ID;
	sev.sigev_signo = TIMER_SIG;
	sev._sigev_un._tid= pid; 
	if (timer_create(CLOCK_THREAD_CPUTIME_ID, &sev, &tid) == -1) {
		errexit("timer create in _sig_timer_init error");
	}

	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 100;
	ts.it_value.tv_sec = 0;
	ts.it_value.tv_nsec = 0;
	if (timer_settime(tid, 0, &ts, NULL) == -1) {
		errexit("timer settime in _sig_timer_init error");
	}
}

void init_uthread(pid_t pid) {
	sche = huroutine_open();
	huroutine_t *main = (huroutine_t *)malloc(sizeof(huroutine_t));
	if (main == NULL) {
		errexit("malloc in init_uthread error");
	}
	main->sch = sche;
	main->state = RUNNING;
	if (getcontext(&main->ctx) == -1) {
		errexit("getcontext in init_uthread error");
	}
	sche->hu_n++;
	sche->running = sche->hu_n;
	insert_head(sche->schequeue, sche->hu_n);
	main->inqueue = fetchfirst_linklist(sche->schequeue);
	sche->currunning = main->inqueue;
	sche->vector[sche->hu_n] = main;
	_sig_timer_init(pid);
}

void new_uthread(huroutine_func func, void *arg) {	
	if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
		errexit("sig block to set in new_uthread error");
	}
	huroutine_create(sche, func, arg, 0);
	if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
		errexit("sig unblock to set in new_uthread error");
	}
}

void uthread_waitall(void) {
	for (;;) {
		if (huroutine_finish(sche)) 
			return;
	}
}
