#ifndef POSIX_H_
#define POSIX_H_

#include <ch.h>
#include <chstreams.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void posix_set_static_fd(BaseSequentialStream **fd, size_t z);
extern void posix_set_stream(int fd, BaseSequentialStream *stream);
extern int posix_write(int fd, const void *data, size_t size);
extern int posix_read(int fd, void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_H_ */
