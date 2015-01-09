#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include "huroutine.h"

schedule_t *s;
int n = 0;
struct itimerspec ts;
struct sigaction sa;
struct sigevent sev;
sigset_t set;
timer_t tid;

void func1(void *args) {
	while (1) {
		n++;
		huroutine_yield(s);
	}
}

void handler(int sig) {
	printf("%d\n", n);
	exit(0);
}

int main() {
	sa.sa_flags = 0;
	sa.sa_handler = &handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaction(SIGUSR1, &sa, NULL);

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	sev.sigev_value.sival_ptr = &tid;
	timer_create(CLOCK_REALTIME, &sev, &tid);
	ts.it_value.tv_sec = 1;
	ts.it_value.tv_nsec = 0;

	s = huroutine_open();
	huroutine_create(s, &func1, NULL, 0);
	huroutine_create(s, &func1, NULL, 0);

	timer_settime(tid, 0, &ts, NULL);
	huroutine_schedule(s);
	return 0;
}
