#include "posix.h"

#include <stdio.h>
#include <string.h>

static posix_stream_t fd_list[POSIX_MAX_FD];
static posix_mountpoint_t *mountpoints;

static int rootfs_open(void *ip, posix_stream_t *s, const char *path, int mode);
static int rootfs_opendir(void *ip, posix_dir_t *dir);
static posix_readdir_result_t rootfs_readdir(void *ip, posix_dir_t *dir,
		posix_inode_info_t *info);
static int rootfs_closedir(void *ip, posix_dir_t *dir);

static const posix_mountpoint_vmt_t root_fs_vmt = { /* VMT */
NULL, /* init */
NULL, /* done */
rootfs_open, /* open */
rootfs_opendir, /* opendir */
rootfs_readdir, /* readdir */
rootfs_closedir /* closedir */
};

static posix_mountpoint_t root_fs = { /* */
NULL, /* this */
&root_fs_vmt, /* vmt */
"/", /* path */
NULL /* next */
};

void posix_init(void) {
	memset(fd_list, 0, POSIX_MAX_FD);
	root_fs.ip = &root_fs;
	mountpoints = &root_fs;
}

static posix_mountpoint_t *find_mountpoint(const char *path, int *mpath_len) {
	posix_mountpoint_t *p;
	for (p = mountpoints; p; p = p->next) {
		int mplen = strlen(p->mp_path);
		if (strncmp(p->mp_path, path, mplen) == 0) {
			*mpath_len = mplen;
			return p;
		}
	}
	return NULL;
}

posix_stream_t *posix_get_stream(int fd) {
	return (fd >= 0 && fd < POSIX_MAX_FD) ? &fd_list[fd] : NULL;
}

int posix_opendir(posix_dir_t *dir) {
	int n = 0;
	posix_mountpoint_t *p = find_mountpoint(dir->path, &n);
	if (p && n) {
		dir->path += n; /* find real name */
		if (p->vmt->opendir(p->ip, dir) == 0) {
			dir->mpoint = p;
			return 0;
		}
	}
	return -1;
}

posix_readdir_result_t posix_readdir(posix_dir_t *dir, posix_inode_info_t *in) {
	return dir->mpoint->vmt->readdir(dir->mpoint->ip, dir, in);
}

int posix_closedir(posix_dir_t *dir) {
	return dir->mpoint->vmt->closedir(dir->mpoint->ip, dir);
}

static int is_mounted(const char *path) {
	posix_mountpoint_t *p;
	for (p = mountpoints; p; p = p->next)
		if (strcmp(p->mp_path, path) == 0)
			return 1;
	return 0;
}

int posix_mount(posix_mountpoint_t *mpoint) {
	int r = -1;
	if (!is_mounted(mpoint->mp_path)) {
		if (mpoint->vmt->init && (r = mpoint->vmt->init(mpoint->ip)) == 0) {
			mpoint->next = mountpoints;
			mountpoints = mpoint;
		}
	}
	return r;
}

int posix_unmount(posix_mountpoint_t *mpoint) {
	if (mpoint == mountpoints) {
		mountpoints = mountpoints->next; // Special head case
		return 0;
	} else {
		posix_mountpoint_t *prev = mountpoints;
		// Find previous mountpoint
		while(prev) {
			if (prev->next == mpoint) {
				if (mpoint->vmt->done)
					mpoint->vmt->done(mpoint->ip);
				prev->next = prev->next->next;
				return 0;
			}
			prev = prev->next;
		}
		return -1; // Can't find mount point
	}
}

void posix_init_stream(posix_stream_t *s, const posix_stream_vmt_t *v, void *p) {
	s->ip = p;
	s->vmt = v;
}

static int posix_get_streamfree(void) {
	int i;
	for (i = 0; i < POSIX_MAX_FD; i++)
		if (fd_list[i].vmt == NULL)
			return i;
	return 0;
}

static void posix_return_stream(posix_stream_t *s) {
	s->vmt = NULL;
}

int posix_open(const char *fname, int mode) {
	int mpath_len = 0;
	posix_mountpoint_t *p = find_mountpoint(fname, &mpath_len);
	if (p && mpath_len) {
		int newfd = posix_get_streamfree();
		if (newfd == -1)
			return -1; // TODO set errno properly
		else {
			posix_stream_t *s = posix_get_stream(newfd);
			const char *realname = fname + mpath_len;
			if (p->vmt->open(p->ip, s, realname, mode) != -1) {
				s->vmt->init(s->ip);
				return newfd;
			}
			return -1; // TODO Set errno properly
		}
	} else
		return -1; // TODO set errno properly
}

void posix_close(int fd) {
	posix_stream_t *ip = posix_get_stream(fd);
	ip->vmt->close(ip->ip);
	posix_return_stream(ip);
}

int posix_write(int fd, const void *data, size_t size) {
	posix_stream_t *ip = posix_get_stream(fd);
	if (ip->vmt)
		return ip->vmt->write(ip->ip, data, size);
	else
		return -1;
}

int posix_read(int fd, void *data, size_t size) {
	posix_stream_t *ip = posix_get_stream(fd);
	if (ip->vmt)
		return ip->vmt->read(ip->ip, data, size);
	else
		return -1;
}

int posix_ioctl(int fd, posix_ioctl_t cmd, posix_variant_t *v) {
	posix_stream_t *ip = posix_get_stream(fd);
	if (ip->vmt && ip->vmt->ioctl)
		return ip->vmt->ioctl(ip->ip, cmd, v);
	else
		return -1;
}

static int rootfs_open(void *ip, posix_stream_t *s, const char *path, int mode) {
	// ROOT FS onlyt allow mount
	(void) ip;
	(void) s;
	(void) path;
	(void) mode;
	return -1;
}

static int rootfs_opendir(void *ip, posix_dir_t *dir) {
	if (dir->path[0] == '\0') {
		dir->raw = mountpoints;
		return 0;
	} else
		return -1; // This is very bad (very difficult for Tevez)
}

static posix_readdir_result_t rootfs_readdir(void *ip, posix_dir_t *dir,
		posix_inode_info_t *info) {
	(void) ip;
	posix_mountpoint_t *curr = (posix_mountpoint_t *) dir->raw;
	if (!curr)
		return POSIX_READDIR_ERR; // ERROR over rootfs (the last)
	if (!curr->next)
		return POSIX_READDIR_EOD;
	strncpy(info->path, curr->mp_path, POSIX_MAX_PATH);
	info->flags = S_IFDIR;
	info->size = 0;
	dir->raw = curr->next;
	return POSIX_READDIR_OK;
}

static int rootfs_closedir(void *ip, posix_dir_t *dir) {
	(void) ip;
	(void) dir;
	return 0; /* Always ok */
}
