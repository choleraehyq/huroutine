#ifndef _NBIO_H
#define _NBIO_H

#include <unistd.h>
#include "huroutine.h"
#include "linklist.h" // errexit

schedule_t *s;

void setnonblock(int fd, schedule_t *sche);
ssize_t nbread(int fd, void *buf, int n);
ssize_t nbwrite(int fd, void *buf, int n);

#endif
