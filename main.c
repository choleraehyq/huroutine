#include <stdio.h>
#include "huroutine.h"

struct arg {
    int n;
    char shit;
} arg1, arg2;

schedule_t *s;

void func1(void *arg) {
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
    huroutine_yield(s);
    printf("%d %d\n", huroutine_running(s), huroutine_status(s, huroutine_running(s)));
}

void func2(void *arg) {
    printf("%d %c\n", ((struct arg *)arg)->n, ((struct arg *)arg)->shit);
}
void huroutine_test() {
    s = huroutine_open();
    arg1.n = 1;
    arg1.shit = 'a';
    arg2.n = 2;
    arg2.shit = 'b';

    int id1 = huroutine_create(s, func1, (void *)&arg1);
    int id2 = huroutine_create(s, func2, (void *)&arg2);

    while (!huroutine_finish(s)) {
        huroutine_resume(s, id1);
        huroutine_resume(s, id2);
        huroutine_resume(s, id1);
    }
}

int main(int argc, char **argv) {
    huroutine_test();
    return 0;
}
