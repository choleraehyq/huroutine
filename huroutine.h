#ifndef _HUROUTINE_H
#define _HUROUTINE_H

#include <ucontext.h>
#include <signal.h>
#define DEFAULT_STACK_SIZE (1024*4)
#define DEFAULT_HUROUTINE_N 16
#define MAX_HUROUTINE 1000

enum huroutine_state {
    DEAD = 0,
    READY,
    RUNNING,
    SUSPEND
};

typedef void (*huroutine_func)(void *);

struct huroutine;
struct schedulor;

typedef struct schedulor schedule_t;

typedef struct {
    struct schedulor *sch;
    ucontext_t ctx;
    huroutine_func func;
    void *arg;
    enum huroutine_state state;
    char *stack;
} huroutine_t;

struct schedulor {
    int running;
    int hu_n;
    int cap;
    huroutine_t **vector;
    ucontext_t main;
};

schedule_t * huroutine_open(void);
void huroutine_close(schedule_t *);

int huroutine_create(schedule_t *, huroutine_func, void *arg, int stksize);
void huroutine_resume(schedule_t *, int id);
int huroutine_status(schedule_t *, int id);
int huroutine_running(schedule_t *);
void huroutine_yield(schedule_t *);
int huroutine_finish(schedule_t *);
void huroutine_sigmask(schedule_t *, int , int);

void errexit(char *s);

#endif

