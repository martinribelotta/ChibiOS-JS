#include "posix_fatfs_provider.h"
#include <FatFSWrapper/fatfsWrapper.h>

#include <string.h>
#include <fcntl.h>

#define FIL_ENTRIES (POSIX_MAX_FD - 3)

static FIL posix_fil_fd[FIL_ENTRIES];
static DIR posix_dir_fd[FIL_ENTRIES];

static FIL *new_fil(void) {
	int i;
	for (i = 0; i < FIL_ENTRIES; i++)
		if (posix_fil_fd[i].fs == NULL )
			return &posix_fil_fd[i];
	return NULL ;
}

static void release_fil(FIL *fd) {
	fd->fs = NULL;
}

static DIR *new_dir(void) {
	int i;
	for (i = 0; i < FIL_ENTRIES; i++)
		if (posix_dir_fd[i].fs == NULL )
			return &posix_dir_fd[i];
	return NULL ;
}

static void release_dir(DIR *fd) {
	fd->fs = NULL;
}

static void fatfs_open(void *ip, int mode) {
	(void) ip;
	(void) mode;
	// Done in Provider open
}

static void fatfs_close(void *ip) {
	FIL *fd = (FIL*) ip;
	wf_close(fd);
	release_fil(fd);
}

static size_t fatfs_write(void *ip, const void *data, size_t size) {
	FIL *fd = (FIL*) ip;
	UINT count = 0;
	if (wf_write(fd, data, size, &count) != FR_OK)
		return -1;
	else
		return count;
}

static size_t fatfs_read(void *ip, void *data, size_t size) {
	FIL *fd = (FIL*) ip;
	UINT count = 0;
	if (wf_read(fd, data, size, &count) != FR_OK)
		return -1;
	else
		return count;
}

static const posix_stream_vmt_t fatfs_vmt = { /* VMT */
fatfs_open, /* open */
fatfs_close, /* close */
fatfs_write, /* write */
fatfs_read /* read */
};

BYTE posix_mode_to_ffmode(int fmode) {
	BYTE mode = FA_READ;

	if (fmode & O_WRONLY)
		mode |= FA_WRITE;
	if ((fmode & O_ACCMODE)& O_RDWR)
		mode |= FA_WRITE;
	/* Opens the file, if it is existing. If not, a new file is created. */
	if (fmode & O_CREAT)
		mode |= FA_OPEN_ALWAYS;
	/* Creates a new file. If the file is existing, it is truncated and overwritten. */
	if (fmode & O_TRUNC)
		mode |= FA_CREATE_ALWAYS;
	/* Creates a new file. The function fails if the file is already existing. */
	if (fmode & O_EXCL)
		mode |= FA_CREATE_NEW;
	return mode;
}

static int fatfs_provider_open(void *ip, posix_stream_t *s, const char *path,
		int mode) {
	FATFS *fs = (FATFS*) ip;
	FIL *fd = new_fil();

	(void) fs;

	s->vmt = &fatfs_vmt;
	s->ip = fd;
	if (wf_open(fd, path, posix_mode_to_ffmode(mode)) != FR_OK)
		return -1;
	return 0;
}

static int fatfs_provider_opendir(void *ip, posix_dir_t *dir) {
	FATFS *fs = (FATFS*) ip;
	DIR *d = new_dir();

	(void) fs;

	if (!d)
		return -1;
	dir->raw = d;
	if (wf_opendir(d, dir->path) != FR_OK) {
		release_dir(d);
		return -1;
	}
	return 0;
}

static posix_readdir_result_t fatfs_provider_readdir(void *ip, posix_dir_t *dir,
		posix_inode_t *info) {
	DIR *d = (DIR*) dir->raw;
	FILINFO finfo;

	(void) ip;

	finfo.lfname = info->path;
	if (wf_readdir(d, &finfo) == FR_OK) {
		info->flags = finfo.fattrib;
		info->size = finfo.lfsize;
		info->raw = NULL;
#if _USE_LFN == 0
		strncpy(info->path, finfo.fname, 13);
#else
		if (!info->path[0])
			strncpy(info->path, finfo.fname, sizeof(info->path));
#endif
		if (finfo.fname[0] == 0)
			return POSIX_READDIR_EOD;
		return POSIX_READDIR_OK;
	} else
		return POSIX_READDIR_ERR;
}

static int fatfs_provider_closedir(void *ip, posix_dir_t *dir) {
	(void) ip;
	release_dir((DIR*) dir->raw);
	return 0;
}

FRESULT posix_fatfs_provider_init(posix_file_provider_t *p, FATFS *fs, int vol) {
	FRESULT err = wf_mount(vol, fs);
	if (err == FR_OK) {
		p->open = fatfs_provider_open;
		p->opendir = fatfs_provider_opendir;
		p->readdir = fatfs_provider_readdir;
		p->closedir = fatfs_provider_closedir;
		p->ip = fs;
		memset(&posix_fil_fd, 0, sizeof(posix_fil_fd));
	}
	return err;
}
