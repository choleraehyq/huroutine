#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include "huroutine.h"
#include "uthread.h"
#include "nbio.h"

struct arg {
    int n;
    char shit;
} arg1, arg2;

schedule_t *s;


void func1(void *arg) {
	char a[100];
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
    huroutine_yield(s);
    printf("%d %d\n", huroutine_running(s), huroutine_status(s, huroutine_running(s)));
	int n = nbread(STDIN_FILENO, a, 100);
	nbwrite(STDOUT_FILENO, a, n);
}

void func2(void *arg) {
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
	huroutine_yield(s);
	int i, sum = 0;
	for (i = 0; i < 100; i++) {
		sum += i;
	}
	fprintf(stdout, "sum 1 to 100 is %d\n", sum);
}


void huroutine_test(void) {
    s = huroutine_open();
    arg1.n = 1;
    arg1.shit = 'a';
    arg2.n = 2;
    arg2.shit = 'b';

    int id1 = huroutine_create(s, func1, (void *)&arg1, 0);
    int id2 = huroutine_create(s, func2, (void *)&arg2, 0);
	printf("New huroutines are %d and %d\n", id1, id2);
	printf("Start scheduling!\n");
	huroutine_schedule(s);

    huroutine_close(s);
	printf("Scheduling end.\n");
}

void u_func1(void *arg) {
	int i;
	for (i = 0; i < ((struct arg *)arg)->n; i++)
		printf("shit%d\n", i);
}

void u_func2(void *arg) {
	int i;
	for (i = 0; i < ((struct arg *)arg)->n; i++)
		printf("fuck%d\n", i);
}

void *uthread_test(void *arg) {
	init_uthread(gettid());
	arg1.n = 100;
	new_uthread(&u_func1, (void *)&arg1);
	new_uthread(&u_func2, (void *)&arg1);
	uthread_waitall();
	return NULL;
}

int main(int argc, char **argv) {
	huroutine_test();
	/*
	pthread_t pid;
	pthread_create(&pid, NULL, &uthread_test, (void *)NULL);
	pthread_join(pid, NULL);
	*/
	//uthread_test((void *)NULL);
    return 0;
}
