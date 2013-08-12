#include "posix.h"

#include <stdio.h>
#include <string.h>

static posix_stream_t fd_list[POSIX_MAX_FD];
static posix_file_provider_t *fproviders = NULL;

void posix_init(void) {
	memset(fd_list, 0, POSIX_MAX_FD);
}

posix_stream_t *posix_get_stream(int fd) {
	return (fd >= 0 && fd < POSIX_MAX_FD) ? &fd_list[fd] : NULL ;
}

int posix_opendir(posix_dir_t *dir) {
	posix_file_provider_t *p;
	for (p = fproviders; p; p = p->next)
		if (p->opendir(p->ip, dir) == 0) {
			dir->provider = p;
			return 0;
		}
	return -1;
}

posix_readdir_result_t posix_readdir(posix_dir_t *dir, posix_inode_t *in) {
	return dir->provider->readdir(dir->provider->ip, dir, in);
}

int posix_closedir(posix_dir_t *dir) {
	return dir->provider->closedir(dir->provider->ip, dir);
}

static int is_provider_registered(posix_file_provider_t *provider) {
	posix_file_provider_t *p;
	for (p = fproviders; p; p = p->next)
		if (p == provider)
			return 1;
	return 0;
}

void posix_add_fileprovider(posix_file_provider_t *provider) {
	if (!is_provider_registered(provider)) {
		provider->next = fproviders;
		fproviders = provider;
	}
}

void posix_init_stream(posix_stream_t *s, const posix_stream_vmt_t *v, void *p) {
	s->ip = p;
	s->vmt = v;
}

static int posix_get_streamfree(void) {
	int i;
	for (i = 0; i < POSIX_MAX_FD; i++)
		if (fd_list[i].vmt == NULL )
			return i;
	return 0;
}

static void posix_retur_stream(posix_stream_t *s) {
	s->vmt = NULL;
}

int posix_open(const char *fname, int mode) {
	int newfd = posix_get_streamfree();
	if (newfd != -1) {
		posix_file_provider_t *p = fproviders;
		posix_stream_t *s = posix_get_stream(newfd);
		while (p) {
			if (p->open(p->ip, s, fname, mode) != -1) {
				s->vmt->open(s->ip, mode);
				return newfd;
			}
			p = p->next;
		}
	}
	// TODO Set errno properly
	return -1;
}

void posix_close(int fd) {
	posix_stream_t *ip = posix_get_stream(fd);
	ip->vmt->close(ip->ip);
	posix_retur_stream(ip);
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

int posix_ioctl(int fd, posix_ioctl_t cmd, posix_variant_t v) {
	posix_stream_t *ip = posix_get_stream(fd);
	if (ip->vmt && ip->vmt->ioctl)
		return ip->vmt->ioctl(ip->ip, cmd, v);
	else
		return -1;
}

