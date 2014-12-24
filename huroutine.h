#ifndef _HUROUTINE_H
#define _HUROUTINE_H

#include <ucontext.h>
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

schedule_t * huroutine_open(void);
void huroutine_close(schedule_t *);

int huroutine_create(schedule_t *, huroutine_func, void *arg);
void huroutine_resume(schedule_t *, int id);
int huroutine_status(schedule_t *, int id);
int huroutine_running(schedule_t *);
void huroutine_yield(schedule_t *);
int huroutine_finish(schedule_t *);

#endif

