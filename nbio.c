#include <fcntl.h>
#include <errno.h>
#include "nbio.h"
#include "err.h"

void setnonblock(int fd, schedule_t *sche) {
	s = sche;
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
		errexit("fcntl F_GETFL in setnonblock error");
	}
	if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		errexit("fcntl F_SETFL in setnonblock error");
	}
}

ssize_t nbread(int fd, void *buf, int n) {
	ssize_t ret;
	while ((ret = read(fd, buf, n)) < 0 && errno == EAGAIN) {
		huroutine_yield(s);
	}
	if (ret == -1 && errno != EAGAIN) {
		errexit("read in nbread error");
	}
	return ret;
}

ssize_t nbwrite(int fd, void *buf, int n) {
	ssize_t ret;
	while ((ret = write(fd, buf, n)) < 0 && errno == EAGAIN) {
		huroutine_yield(s);
	}
	if (ret == -1 && errno != EAGAIN) {
		errexit("write in nbwrite error");
	}
	return ret;
}
