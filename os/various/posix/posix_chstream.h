#ifndef POSIX_CHSTREAM_H_
#define POSIX_CHSTREAM_H_

#include <ch.h>
#include <chstreams.h>

extern void posix_init_chstream(int fd, BaseSequentialStream *chs);

#endif /* POSIX_CHSTREAM_H_ */
