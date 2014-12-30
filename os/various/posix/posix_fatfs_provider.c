#include "posix_fatfs_provider.h"
#include <FatFSWrapper/fatfsWrapper.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FIL_ENTRIES (POSIX_MAX_FD - 3)

static FIL posix_fil_fd[FIL_ENTRIES];
static DIR posix_dir_fd[FIL_ENTRIES];

static FIL *new_fil(void) {
	int i;
	for (i = 0; i < FIL_ENTRIES; i++)
		if (posix_fil_fd[i].fs == NULL)
			return &posix_fil_fd[i];
	return NULL;
}

static void release_fil(FIL *fd) {
	fd->fs = NULL;
}

static DIR *new_dir(void) {
	int i;
	for (i = 0; i < FIL_ENTRIES; i++)
		if (posix_dir_fd[i].fs == NULL)
			return &posix_dir_fd[i];
	return NULL;
}

static void release_dir(DIR *fd) {
	fd->fs = NULL;
}

static void fatfd_init(void *ip) {
	(void) ip;
}

static void fatfd_close(void *ip) {
	FIL *fd = (FIL*) ip;
	wf_close(fd);
	release_fil(fd);
}

static size_t fatfd_write(void *ip, const void *data, size_t size) {
	FIL *fd = (FIL*) ip;
	UINT count = 0;
	if (wf_write(fd, data, size, &count) != FR_OK)
		return -1;
	else
		return count;
}

static size_t fatfd_read(void *ip, void *data, size_t size) {
	FIL *fd = (FIL*) ip;
	UINT count = 0;
	if (wf_read(fd, data, size, &count) != FR_OK)
		return -1;
	else
		return count;
}

static int fatfs_seek(FIL *fd, posix_seek_t *sk) {
	DWORD newpos;
	long old_pos = wf_tell(fd);

	switch (sk->dir) {
	case SEEK_END:
		newpos = wf_size(fd) + sk->off;
		break;
	case SEEK_SET:
		newpos = sk->off;
		break;
	case SEEK_CUR:
		newpos = old_pos + sk->off;
		break;
	default:
		return -1; // TODO set errno properly
	}
	sk->off = old_pos;
	if (wf_lseek(fd, newpos) == FR_OK) {
		return wf_tell(fd);
	} else
		return -1;
}

static int fatfs_stat(FIL *fd, posix_stat_t *st) {
	st->st.st_dev = fd->fs->drv; /* ID of device containing file */
	st->st.st_ino = fd->id; /* inode number */
	st->st.st_mode = 0666; /* protection */
	st->st.st_nlink = 1; /* number of hard links */
	st->st.st_uid = 0; /* user ID of owner */
	st->st.st_gid = 0; /* group ID of owner */
	st->st.st_rdev = fd->fs->id; /* device ID (if special file) */
	st->st.st_size = fd->fsize; /* total size, in bytes */
	st->st.st_blksize = 512; /* blocksize for file system I/O */
	st->st.st_blocks = fd->fsize / 512; /* number of 512B blocks allocated */
	st->st.st_atime = 0; /* time of last access */
	st->st.st_mtime = 0; /* time of last modification */
	st->st.st_ctime = 0; /* time of last status change */
	return 0;
}

static int fatfd_ioctl(void *ip, posix_ioctl_t cmd, posix_variant_t *v) {
	FIL *fd = (FIL*) ip;
	switch (cmd) {
	case POSIX_SEEK:
		return fatfs_seek(fd, &v->seek);
		break;
	case POSIX_STAT:
		return fatfs_stat(fd, &v->stat);
		break;
	default:
		return -1; // TODO set errno properly
	}
}

static const posix_stream_vmt_t fatfd_vmt = { /* VMT */
fatfd_init, /* open */
fatfd_close, /* close */
fatfd_write, /* write */
fatfd_read, /* read */
fatfd_ioctl, /* ioctl */
};

BYTE posix_mode_to_ffmode(int fmode) {
	BYTE mode = FA_READ;

	if (fmode & O_WRONLY)
		mode |= FA_WRITE;
	if ((fmode & O_ACCMODE) & O_RDWR)
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

static int fatfs_open(void *ip, posix_stream_t *s, const char *path,
		int mode) {
	FIL *fd = new_fil();
	fd->fs = (FATFS*) ip;
	s->ip = fd;
	s->vmt = &fatfd_vmt;
	if (wf_open(fd, path, posix_mode_to_ffmode(mode)) != FR_OK)
		return -1;
	return 0;
}

static int fatfs_opendir(void *ip, posix_dir_t *dir) {
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

static posix_readdir_result_t fatfs_readdir(void *ip, posix_dir_t *dir,
		posix_inode_info_t *info) {
	DIR *d = (DIR*) dir->raw;
	FILINFO finfo;

	(void) ip;

	info->path[0] = '\0';
	finfo.lfname = info->path;
	if (wf_readdir(d, &finfo) == FR_OK) {
		info->flags = finfo.fattrib;
		info->size = finfo.lfsize;
		info->raw = NULL;
		if (!info->path[0])
			strncpy(info->path, finfo.fname, sizeof(info->path));
		if (finfo.fname[0] == 0)
			return POSIX_READDIR_EOD;
		else
			return POSIX_READDIR_OK;
	} else
		return POSIX_READDIR_ERR;
}

static int fatfs_closedir(void *ip, posix_dir_t *dir) {
	(void) ip;
	release_dir((DIR*) dir->raw);
	return 0;
}

static int fatfs_init(void *ip) {
	FATFS *fs = (FATFS*) ip;
	if (wf_mount(fs->drv, fs) == FR_OK)
		return 0;
	else
		return -1;
}

const posix_mountpoint_vmt_t fatfs_vmt = {
		fatfs_init, /**/
		NULL, /**/
		fatfs_open, /**/
		fatfs_opendir, /**/
		fatfs_readdir, /**/
		fatfs_closedir, /**/
};
