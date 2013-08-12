#ifndef POSIX_IOCTL_H_
#define POSIX_IOCTL_H_

#include <sys/stat.h>

typedef enum {
	/* cmd         param */
	POSIX_NOP, /* N/U */
	POSIX_SEEK, /* posix_seek_t* */
	POSIX_STAT, /* posix_stat_t* */
	POSIX_ISATTY, /* int* */
} posix_ioctl_t;

typedef struct {
	int dir;
	int off;
} posix_seek_t;

typedef struct {
	struct stat st;
} posix_stat_t;

#endif /* POSIX_IOCTL_H_ */
