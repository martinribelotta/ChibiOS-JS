#ifndef POSIX_H_
#define POSIX_H_

#include <ch.h>
#include <stddef.h>

#include <posix_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void (*open)(void *ip, int mode);
	void (*close)(void *ip);
	size_t (*write)(void *ip, const void *data, size_t size);
	size_t (*read)(void *ip, void *data, size_t size);
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
} posix_inode_t;

struct posix_file_provider_t;

typedef struct {
	const char *path;
	struct posix_file_provider_t *provider;
	void *raw;
} posix_dir_t;

typedef enum {
	POSIX_READDIR_OK, /* good direntry read */
	POSIX_READDIR_EOD, /* end of directory */
	POSIX_READDIR_ERR /* read error */
} posix_readdir_result_t;

typedef struct posix_file_provider_t {
	void *ip;
	int (*open)(void *ip, posix_stream_t *s, const char *path, int mode);
	int (*opendir)(void *ip, posix_dir_t *dir);
	posix_readdir_result_t (*readdir)(void *ip, posix_dir_t *dir, posix_inode_t *info);
	int (*closedir)(void *ip, posix_dir_t *dir);
	struct posix_file_provider_t *next;
} posix_file_provider_t;

extern void posix_init(void);
extern void posix_add_fileprovider(posix_file_provider_t *provider);
extern void posix_init_stream(posix_stream_t *s, const posix_stream_vmt_t *v, void *p);
extern posix_stream_t *posix_get_stream(int fd);

extern int posix_opendir(posix_dir_t *dir);
extern posix_readdir_result_t posix_readdir(posix_dir_t *dir, posix_inode_t *in);
extern int posix_closedir(posix_dir_t *dir);

extern int posix_open(const char *name, int mode);
extern void posix_close(int fd);
extern int posix_write(int fd, const void *data, size_t size);
extern int posix_read(int fd, void *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* POSIX_H_ */
