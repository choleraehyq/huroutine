#include <stdlib.h>
#include <stdio.h>
#include "channel.h"
#include "huroutine.h"
#include "err.h"

struct CHANNEL {
	void **data;
	int cap;
	int cursize;
	int next_in;
	int next_out;
	schedule_t *s;
};

channel *channel_create(int cap, schedule_t *s) {
	channel *ret = (channel *)malloc(sizeof(channel));
	if (ret == NULL) {
		errexit("ret malloc in channel_create error");
	}
	ret->cap = cap;
	ret->cursize = 0;
	ret->next_in = 0;
	ret->next_out = 0;
	ret->s = s;
	ret->data = (void **)malloc(cap * sizeof(void *));
	if (ret->data == NULL) {
		errexit("ret->data malloc in channel_create error");
	}
	return ret;
}

int channel_isfull(channel *chan) {
	return chan->cap == chan->cursize;
}

void channel_push(channel *chan, void *pkg) {
	while (channel_isfull(chan)) {
		huroutine_yield(chan->s);
	}
	chan->data[chan->next_in] = pkg;
	chan->next_in = (chan->next_in + 1) % chan->cap;
	chan->cursize++;
}

void *channel_pull(channel *chan) {
	while (chan->cursize == 0) {
		huroutine_yield(chan->s);
	}
	void *ret = chan->data[chan->next_out];
	chan->data[chan->next_out] = NULL;
	chan->next_out = (chan->next_out + 1) % chan->cap;
	chan->cursize--;
	return ret;
}

void channel_delete(channel *chan) {
	if (chan == NULL) {
		return;
	}
	while (chan->next_out != chan->next_in) {
		free(chan->data[chan->next_out]);
		chan->next_out = (chan->next_out + 1) % chan->cap;
	}
	free(chan);
}
