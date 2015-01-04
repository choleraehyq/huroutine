#include <ucontext.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "huroutine.h"

schedule_t *huroutine_open(void) {
    schedule_t *s = (schedule_t *)malloc(sizeof(*s));
    s->hu_n = 0;
    s->cap = DEFAULT_HUROUTINE_N;
    s->running = 0;
    s->vector = (huroutine_t **)malloc(s->cap * sizeof(huroutine_t *));
    memset(s->vector, 0, sizeof(huroutine_t *) * s->cap);
    return s;
}

huroutine_t *_hu_new(schedule_t *s, huroutine_func func, void *arg) {
    huroutine_t *ret = (huroutine_t *)malloc(sizeof(*ret));
    ret->sch = s;
    ret->func = func;
    ret->arg = arg;
    ret->state = READY;
    ret->stack = (char *)malloc(sizeof(*ret->stack) * DEFAULT_STACK_SIZE);
    return ret;
}

void _hu_del(huroutine_t **hu) {
    free((*hu)->stack);
    free(*hu);
    *hu = NULL;
}

int huroutine_create(schedule_t *s, huroutine_func func, void *arg) {
    int id;
    ++s->hu_n;
    if (s->hu_n > s->cap) {
        s->vector = realloc(s->vector, s->cap * sizeof(*s->vector) * 2);
        memset(s->vector+s->cap, 0, s->cap * sizeof(*s->vector));
        s->vector[s->cap] = _hu_new(s, func, arg);
        id = s->cap;
        s->cap *= 2;
    }
    else {
        int i;
        for (i = 0; i < s->cap; i++) {
            id = (s->hu_n + i) % s->cap;
            if (s->vector[id] == NULL) {
                s->vector[id] = _hu_new(s, func, arg);
                break;
            }
            else if (s->vector[id]->state == DEAD) {
                s->vector[id]->func = func;
                s->vector[id]->arg = arg;
                s->vector[id]->state = READY;
                break;
            }
        } // end of for-loop
    }
    return id;
}

void huroutine_close(schedule_t *s) {
    int i;
    for (i = 0; i < s->cap; i++) {
        huroutine_t *tmp = s->vector[i];
        if (tmp) {
            _hu_del(&tmp);
        }
    }
    free(s->vector);
    free(s);
}

void _hu_func(uint32_t low32, uint32_t hi32) {
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    schedule_t *s = (schedule_t *)ptr;
    int id = s->running;
    huroutine_t *cur = s->vector[id];
    cur->func(cur->arg);
    cur->state = DEAD;
    --s->hu_n;
    s->running = 0;
}

void huroutine_resume(schedule_t *s, int id) {
    if (s == NULL || id <= 0 || id > s->cap || s->vector[id] == NULL) {
        return;
    }

    huroutine_t *hu = s->vector[id];

    switch (hu->state) {
        case READY:
            getcontext(&hu->ctx);
            hu->ctx.uc_stack.ss_sp = hu->stack;
            hu->ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
            hu->ctx.uc_link = &s->main;
            s->running = id;
            hu->state = RUNNING;
            uintptr_t ptr = (uintptr_t)s;
            makecontext(&hu->ctx, (void (*)(void))_hu_func, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));
            swapcontext(&s->main, &hu->ctx);
            break;
            
        case SUSPEND:
            s->running = id;
            hu->state = RUNNING;
            swapcontext(&s->main, &hu->ctx);
            break;

        case RUNNING:
        case DEAD:
        default:
            break;
    }
}

int huroutine_status(schedule_t *s, int id) {
    if (s == NULL || id <= 0 || id > s->cap || s->vector[id] == NULL) {
        return 0;
    }
    huroutine_t *tmp = s->vector[id];
    return (int)tmp->state;
}

int huroutine_running(schedule_t *s) {
    return s->running;
}

void huroutine_yield(schedule_t *s) {
    int id = s->running;
    if (id <= 0) {
        return;
    }
    s->running = 0;
    huroutine_t *tmp = s->vector[id];
    tmp->state = SUSPEND;
    swapcontext(&tmp->ctx, &s->main);
}

int huroutine_finish(schedule_t *s) {
    int ret;
    if (s->hu_n > 0) { 
        ret = 0;
    }
    else { 
        ret = 1;
    }
    return ret;
}

void huroutine_sigmask(schedule_t *s, int id, int sig) {
    if (s == NULL || id <= 0 || id > s->cap || s->vector[id] == NULL) {
        return;
    }
	sigaddset(&s->vector[id]->ctx.uc_sigmask, sig);
}
