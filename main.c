#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include "huroutine.h"
#include "nbio.h"
#include "channel.h"
#include "err.h"

struct arg {
    int n;
    char shit;
} arg1, arg2;

schedule_t *s;
channel *ch;


void func1(void *arg) {
	char *a = (char *)malloc(sizeof(char) * 100);
	if (a == NULL) {
		errexit("first malloc in func1");
	}
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
    huroutine_yield(s);
    printf("%d %d\n", huroutine_running(s), huroutine_status(s, huroutine_running(s)));
	int *n = (int *)malloc(sizeof(*n));
	if ( n == NULL) {
		errexit("second malloc in func1");
	}
	*n = nbread(STDIN_FILENO, a, 100);
	channel_push(ch, (void *)n);
	channel_push(ch, (void *)a);
}

void func2(void *arg) {
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
	huroutine_yield(s);
	int i, sum = 0;
	for (i = 0; i < 100; i++) {
		sum += i;
	}
	fprintf(stdout, "sum 1 to 100 is %d\n", sum);
	int n = *(int *)channel_pull(ch);
	char *a = (char *)channel_pull(ch);
	nbwrite(STDOUT_FILENO, a, n);
}


void huroutine_test(void) {
    s = huroutine_open();
    arg1.n = 1;
    arg1.shit = 'a';
    arg2.n = 2;
    arg2.shit = 'b';
	ch = channel_create(2, s);

    int id1 = huroutine_create(s, func1, (void *)&arg1, 0);
    int id2 = huroutine_create(s, func2, (void *)&arg2, 0);
	printf("New huroutines are %d and %d\n", id1, id2);
	printf("Start scheduling!\n");
	huroutine_schedule(s);

	channel_delete(ch);
    huroutine_close(s);
	printf("Scheduling end.\n");
}

int main(int argc, char **argv) {

	huroutine_test();
    
	return 0;
}
