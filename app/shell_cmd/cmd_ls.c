#include <ch.h>
#include <hal.h>

#include <stdio.h>

#include <posix/posix.h>

int cmd_ls(int argc, char *argv[]) {
	posix_dir_t dir;
	if (argc > 1)
		dir.path = argv[1];
	else
		dir.path = "/";
	if (posix_opendir(&dir) == 0) {
		posix_inode_info_t inode;
		posix_readdir_result_t r;
		while ((r = posix_readdir(&dir, &inode)) == POSIX_READDIR_OK)
			printf("> %s\r\n", inode.path);
		if (r == POSIX_READDIR_ERR)
			printf("Error reading dir\r\n");
		posix_closedir(&dir);
	} else
		printf("Can not open %s\r\n", dir.path);
	return 0;
}
