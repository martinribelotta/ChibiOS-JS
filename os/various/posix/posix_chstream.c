#include "posix_chstream.h"
#include "posix.h"

static void chstream_init(void *ip) {
	BaseSequentialStream *stream = (BaseSequentialStream*) ip;
	(void) stream;
}

static void chstream_close(void *ip) {
	BaseSequentialStream *stream = (BaseSequentialStream*) ip;
	(void) stream;
}

static size_t chstream_write(void *ip, const void *data, size_t size) {
	BaseSequentialStream *chp = (BaseSequentialStream *) ip;
	int cr_send = 0;
	int counter = 0;
	while (counter < size) {
		char *ptr = (char*) data;
#if 1
		if (ptr[counter] == '\r')
			cr_send = 1;
		else if (ptr[counter] == '\n' && !cr_send)
			chSequentialStreamPut(chp, '\r');
		else
			cr_send = 0;
#endif
		if (chSequentialStreamPut(chp, ptr[counter]) != Q_OK)
			return counter;
		counter++;
	}
	return counter;
}

static size_t chstream_read(void *ip, void *data, size_t size) {
	return chSequentialStreamRead((BaseSequentialStream *) ip, data, size) ;
}

static const posix_stream_vmt_t posix_chstream_vmt = { /* vmt */
chstream_init, /* open */
chstream_close, /* close */
chstream_write, /* write */
chstream_read /* read */
};

void posix_init_chstream(int fd, BaseSequentialStream *s) {
	posix_stream_t *stream = posix_get_stream(fd);
	stream->vmt = &posix_chstream_vmt;
	stream->ip = s;
}
