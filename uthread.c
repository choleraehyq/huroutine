#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "huroutine.h"
#include "uthread.h"
#include "linklist.h"
#include "err.h"

static schedule_t *_sche;
struct itimerspec _ts;
struct sigaction _sa;
struct sigevent _sev;
sigset_t _set;
timer_t _tid;
pid_t _pid;

void _handler(int sig) {
	huroutine_yield(_sche);
}

void _sig_timer_init(pid_t pid) {
	if (sigemptyset(&_set) == -1) {
		errexit("sigemptyset to set in _sig_timer_init error");
	}
	if (sigaddset(&_set, TIMER_SIG) == -1) {
		errexit("sigaddset to set in _sig_timer_init error");
	}

	_sa.sa_flags = 0;
	_sa.sa_handler = &_handler;
	_sa.sa_mask = _set;
	if (sigaction(TIMER_SIG, &_sa, NULL) == -1) {
		errexit("sigaction in _sig_timer_init error");
	}

	_sev.sigev_notify = SIGEV_THREAD_ID;
	_sev.sigev_signo = TIMER_SIG;
	_sev._sigev_un._tid= pid; 
	if (timer_create(CLOCK_THREAD_CPUTIME_ID, &_sev, &_tid) == -1) {
		errexit("timer create in _sig_timer_init error");
	}

	_ts.it_interval.tv_sec = 0;
	_ts.it_interval.tv_nsec = 100;
	_ts.it_value.tv_sec = 0;
	_ts.it_value.tv_nsec = 100;
	if (timer_settime(_tid, 0, &_ts, NULL) == -1) {
		errexit("timer_settime in _sig_timer_init error");
	}
}

void uthread_init(pid_t pid) {
	_sche = huroutine_open();
	_pid = pid;
}

void uthread_create(huroutine_func func, void *arg) {	
	huroutine_create(_sche, func, arg, 0);
}

void uthread_schedule(void) {
	if (sigprocmask(SIG_BLOCK, &_set, NULL) == -1) {
		errexit("first sig block in uthread__schedule error");
	}
	_sig_timer_init(_pid);
	while (!huroutine_finish(_sche)) {
		if (_sche->currunning->next == NULL) {
			_sche->currunning = _sche->schequeue->next;
		}
		else 
			_sche->currunning = _sche->currunning->next;
		
		if (sigprocmask(SIG_UNBLOCK, &_set, NULL) == -1) {
			errexit("sig unblock in uthread__schedule error");
		}
		huroutine_resume(_sche, _sche->currunning->hid);
		if (sigprocmask(SIG_BLOCK, &_set, NULL) == -1) {
			errexit("second sig block in uthread__schedule error");
		}
	}
}

