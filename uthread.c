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
	sigprocmask(SIG_BLOCK, &set, NULL);
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
			sigprocmask(SIG_UNBLOCK, &set, NULL);
			huroutine_resume(sche, cur->hid);
			return;
		}
	}	
}

void _sig_timer_init(pid_t pid) {
	sigemptyset(&set);
	sigaddset(&set, TIMER_SIG);

	sa.sa_flags = SA_NODEFER;
	sa.sa_handler = &_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(TIMER_SIG, &sa, NULL);

	sev.sigev_notify = SIGEV_THREAD_ID;
	sev.sigev_signo = TIMER_SIG;
	sev._sigev_un._tid= pid; 
	timer_create(CLOCK_THREAD_CPUTIME_ID, &sev, &tid);

	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 100;
	ts.it_value.tv_sec = 0;
	ts.it_value.tv_nsec = 0;
	timer_settime(tid, 0, &ts, NULL);
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
	sigprocmask(SIG_BLOCK, &set, NULL);
	int hid = huroutine_create(sche, func, arg);
	insert_head(hid_list, hid);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void uthread_waitall(void) {
	for (;;) {
		if (huroutine_finish(sche)) 
			return;
	}
}
