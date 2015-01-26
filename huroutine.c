#include <ucontext.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include "huroutine.h"
#include "linklist.h"
#include "err.h"

schedule_t *huroutine_open(void) {
    schedule_t *s = (schedule_t *)malloc(sizeof(*s));
    if (s == NULL) {
		errexit("schedule_t malloc error");
	}
    s->hu_n = 0;
    s->cap = DEFAULT_HUROUTINE_N;
    s->running = 0;
    s->vector = (huroutine_t **)malloc(s->cap * sizeof(huroutine_t *));
	if (s->vector == NULL) {
		errexit("initial huroutine_t malloc error");
	}
    memset(s->vector, 0, sizeof(huroutine_t *) * s->cap);
	s->schequeue = create_linklist();
	s->currunning = s->schequeue;
    return s;
}

static huroutine_t *_hu_new(schedule_t *s, 
		huroutine_func func, void *arg, int stksize) {
	
    huroutine_t *ret = (huroutine_t *)malloc(sizeof(*ret));
	if ( ret == NULL) {
		errexit("new huroutine_t malloc error");
	}
    ret->sch = s;
    ret->func = func;
    ret->arg = arg;
    ret->state = READY;
    ret->stack = (char *)malloc(sizeof(*ret->stack) * stksize);
	if (ret->stack == NULL) {
		errexit("new huroutine stack malloc error");
	}
    if (getcontext(&ret->ctx) < 0) {
		errexit("getcontext in _hu_new error");
	}
    ret->ctx.uc_stack.ss_sp = ret->stack;
    ret->ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    ret->ctx.uc_link = &s->main;
    return ret;
}

static void _hu_del(huroutine_t **hu) {
    free((*hu)->stack);
    free(*hu);
    *hu = NULL;
}

static void _add_schequeue(schedule_t *s, int id) {
	insert_head(s->schequeue, id);
	s->vector[id]->inqueue = fetchfirst_linklist(s->schequeue);
}

int huroutine_create(schedule_t *s,
		huroutine_func func, void *arg, int stksize) {
	stksize = stksize <= 0 ? DEFAULT_STACK_SIZE : stksize;
    int id;
    ++s->hu_n;
    if (s->hu_n > s->cap) {
        s->vector = realloc(s->vector, s->cap * sizeof(*s->vector) * 2);
		if (s->vector == NULL) {
			errexit("schedule_t vector realloc error");
		}
        memset(s->vector+s->cap, 0, s->cap * sizeof(*s->vector));
        s->vector[s->cap] = _hu_new(s, func, arg, stksize);
        id = s->cap;
		_add_schequeue(s, id);

        s->cap *= 2;
    }
    else {
        int i;
        for (i = 0; i < s->cap; i++) {
            id = (s->hu_n + i) % s->cap;
            if (s->vector[id] == NULL) {
                s->vector[id] = _hu_new(s, func, arg, stksize);
				_add_schequeue(s, id);
                break;
            }
            else if (s->vector[id]->state == DEAD) {
                s->vector[id]->func = func;
                s->vector[id]->arg = arg;
                s->vector[id]->state = READY;
				_add_schequeue(s, id);
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
	node *tmp = s->schequeue;
	while (tmp != NULL) {
		node *ttmp = tmp;
		tmp = tmp->next;
		free(ttmp);
	}
    free(s->vector);
    free(s);
}

static void _hu_func(uint32_t low32, uint32_t hi32) {
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    schedule_t *s = (schedule_t *)ptr;
    int id = s->running;
    huroutine_t *cur = s->vector[id];
    cur->func(cur->arg);
    cur->state = DEAD;
    --s->hu_n;
	delete_linklist(s->schequeue, s->currunning);
    s->running = 0;
	s->currunning = s->schequeue;
}

void huroutine_resume(schedule_t *s, int id) {
    if (s == NULL || id <= 0 || id > s->cap ||
			s->vector[id] == NULL) {
        return;
    }

    huroutine_t *hu = s->vector[id];

    switch (hu->state) {
        case READY:
            s->running = id;
			s->currunning = hu->inqueue;
            hu->state = RUNNING;
            uintptr_t ptr = (uintptr_t)s;
            makecontext(&hu->ctx, (void (*)(void))_hu_func, 2,
						(uint32_t)ptr, (uint32_t)(ptr >> 32));

            if (swapcontext(&s->main, &hu->ctx) < 0) {
				errexit("swapcontext in huroutine_resume error");
			}

            break;
            
        case SUSPEND:
            s->running = id;
			s->currunning = hu->inqueue;
            hu->state = RUNNING;
            if (swapcontext(&s->main, &hu->ctx) < 0) {
				errexit("swapcontext in huroutine_resume error");
			}
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
	//should not change s->currunning here,
	//	because huroutine_schedule() will use it
	//	to choose the next huroutine, and 
	//	it will be changed there.
    if (swapcontext(&tmp->ctx, &s->main) < 0) {
		errexit("swapcontext in huroutine_yield error");
	}
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
	if (sigaddset(&s->vector[id]->ctx.uc_sigmask, sig) < 0) {
		errexit("sigaddset in huroutine_sigmask error");
	}
}

void huroutine_schedule(schedule_t *s) {
	while (!huroutine_finish(s)) {
		if (s->currunning->next == NULL) {
			s->currunning = s->schequeue->next;
		}
		else 
			s->currunning = s->currunning->next;

		huroutine_resume(s, s->currunning->hid);
	}
}
