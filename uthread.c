#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "huroutine.h"
#include "uthread.h"
#include "linklist.h"

schedule_t *sche;
node *hid_list;
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
	while (hid_list->next != NULL) {
		if (cur->next == NULL) {
			cur = hid_list->next;
		}
		enum huroutine_state curstate = huroutine_status(sche, cur->hid);
		if (curstate == DEAD) {
			delete_linklist(hid_list, cur);
		}
		else if (curstate == SUSPEND || curstate == READY) {
			if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
				errexit("sig unblock in _handler error");
			}
			huroutine_resume(sche, cur->hid);
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
	hid_list = create_linklist();
	cur = hid_list->next;
	_sig_timer_init(pid);
	sche = huroutine_open();
	huroutine_t *main = (huroutine_t *)malloc(sizeof(huroutine_t));
	main->sch = sche;
	main->state = RUNNING;
	getcontext(&main->ctx);
	sche->hu_n++;
	sche->running = sche->hu_n;
	sche->vector[sche->hu_n] = main;
	insert_head(hid_list, sche->hu_n);
}

void new_uthread(huroutine_func func, void *arg) {	
	if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
		errexit("sig block to set in new_uthread error");
	}
	int hid = huroutine_create(sche, func, arg);
	insert_head(hid_list, hid);
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
