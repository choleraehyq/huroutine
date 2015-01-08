#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "huroutine.h"

struct channel_node;
struct CHANNEL;
typedef struct CHANNEL channel ; 

channel *channel_create(int cap, schedule_t *s);
int channel_isfull(channel *);
void channel_push(channel *, void *);
void *channel_pull(channel *);
void channel_delete(channel *);

#endif
