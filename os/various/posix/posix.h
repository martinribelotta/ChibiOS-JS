#ifndef POSIX_H_
#define POSIX_H_

#include <ch.h>
#include <stddef.h>

#include <posix_conf.h>

#include "posix_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
	void *p_void;
	int *p_int;
	char *p_char;

	char d_char;
	int d_int;
	float d_float;

	uint8_t u8;
	uint16_t u16;
	uint32_t u32;

	posix_seek_t seek;
	posix_stat_t stat;
} posix_variant_t;

typedef struct {
	void (*init)(void *ip);
	void (*close)(void *ip);
	size_t (*write)(void *ip, const void *data, size_t size);
	size_t (*read)(void *ip, void *data, size_t size);
	int (*ioctl)(void *ip, posix_ioctl_t cmd, posix_variant_t *v);
} posix_stream_vmt_t;

typedef struct {
	void *ip;
	const posix_stream_vmt_t *vmt;
} posix_stream_t;

typedef struct {
	char path[POSIX_MAX_PATH];
	int flags;
	int size;
	void *raw;
} posix_inode_info_t;

struct posix_mountpoint_s;

typedef struct {
	const char *path;
	struct posix_mountpoint_s *mpoint;
	void *raw;
} posix_dir_t;

typedef enum {
	POSIX_READDIR_OK, /* good direntry read */
	POSIX_READDIR_EOD, /* end of directory */
	POSIX_READDIR_ERR /* read error */
} posix_readdir_result_t;

typedef struct {
	int (*init)(void *ip);
	int (*done)(void *ip);
	int (*open)(void *ip, posix_stream_t *s, const char *path, int mode);
	int (*opendir)(void *ip, posix_dir_t *dir);
	posix_readdir_result_t (*readdir)(void *ip, posix_dir_t *dir, posix_inode_info_t *info);
	int (*closedir)(void *ip, posix_dir_t *dir);
} posix_mountpoint_vmt_t;

typedef struct posix_mountpoint_s {
	void *ip;
	const posix_mountpoint_vmt_t *vmt;
	const char *mp_path;
	struct posix_mountpoint_s *next;
} posix_mountpoint_t;

extern void posix_init(void);
extern int posix_mount(posix_mountpoint_t *provider);
extern int posix_unmount(posix_mountpoint_t *provider);
extern void posix_init_stream(posix_stream_t *s, const posix_stream_vmt_t *v, void *p);
extern posix_stream_t *posix_get_stream(int fd);

extern int posix_opendir(posix_dir_t *dir);
extern posix_readdir_result_t posix_readdir(posix_dir_t *dir, posix_inode_info_t *in);
extern int posix_closedir(posix_dir_t *dir);

extern int posix_open(const char *name, int mode);
extern void posix_close(int fd);
extern int posix_write(int fd, const void *data, size_t size);
extern int posix_read(int fd, void *data, size_t size);
extern int posix_ioctl(int fd, posix_ioctl_t cmd, posix_variant_t *v);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_H_ */
