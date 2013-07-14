#include "posix.h"

#include <stdio.h>
#include "shell.h"

static BaseSequentialStream **fd_list = NULL;
static size_t fd_size = 0;

static BaseSequentialStream *get_fd(int fd) {
	return (fd>=0 && fd<fd_size)? fd_list[fd] : NULL;
}

void posix_set_static_fd(BaseSequentialStream **fd, size_t z) {
	fd_list = fd;
	fd_size = z;
}

void posix_set_stream(int fd, BaseSequentialStream *stream) {
	if (fd>=0 && fd<fd_size)
		fd_list[fd] = stream;
}

int posix_write(int fd, const void *data, size_t size) {
	BaseSequentialStream *ip = get_fd(fd);
	if (ip) {
		int cr_send = 0;
		int counter;
		for(counter = 0; counter<size; counter++) {
			char *ptr = (char*) data;
			if (ptr[counter] == '\r')
				cr_send = 1;
			else if (ptr[counter] == '\n' && !cr_send)
				chSequentialStreamPut(ip, '\r');
			else
				cr_send = 0;
			if (chSequentialStreamPut(ip, ptr[counter]) != Q_OK)
				return counter;
		}
		return counter;
	}
	return -1;
}

int posix_read(int fd, void *data, size_t size) {
	BaseSequentialStream *ip = get_fd(fd);
	if (ip)
		return chSequentialStreamRead(ip, data, size);
	return -1;
}

#if 0
char *fgets(char *line, int size, FILE *f) {
	int fd = fileno(f);
	BaseSequentialStream *chp = get_fd(fd);
	if (chp) {
		shellGetLine(chp, line, size);
		return line;
	} else
		return NULL;
}
#endif
