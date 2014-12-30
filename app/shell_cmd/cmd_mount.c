#include <ch.h>
#include <hal.h>

#include <stdio.h>

#include <posix/posix.h>
#include <posix/posix_fatfs_provider.h>

#include <string.h>

typedef struct fstab_entry_s fstab_entry_t;

struct fstab_entry_s {
	int (*devinit)(const fstab_entry_t *devdata);
	int (*devdone)(const fstab_entry_t *devdata);
	posix_mountpoint_t *mountpoint;
};

static FATFS SDC_FS;
static FATFS_DECLARE_MOUNTPOINT(fatfs_provider_for_sd1, "/SD1", SDC_FS);

static int sdc_init(const fstab_entry_t *dev) {
	(void) dev;
	return (sdcConnect(&SDCD1) == CH_FAILED) ? -1 : 0;
}

static int sdc_done(const fstab_entry_t *dev) {
	(void) dev;
	return (sdcDisconnect(&SDCD1) == CH_FAILED) ? -1 : 0;
}

static const fstab_entry_t fstab[] = { /**/
{ sdc_init, sdc_done, &fatfs_provider_for_sd1 }, /* SD FATFS */
};

#ifndef ASIZE
#define ASIZE(A) (sizeof(A)/sizeof(*A))
#endif

int cmd_mount(int argc, char *argv[]) {
	if (argc != 2) {
		int i;
		printf(
				"usage: mount <devname>\n"
				"\n"
				"Entries in FSTAB:\n");
		for (i = 0; i < ASIZE(fstab); i++)
			printf("- %s\n", fstab[i].mountpoint->mp_path);
	} else {
		int i;
		const char *dev = argv[1];

		printf("Mounting %s\r\n", dev);
		for (i = 0; i < ASIZE(fstab); i++) {
			const fstab_entry_t *e = &fstab[i];
			if (strcmp(e->mountpoint->mp_path, dev) == 0) {
				if (e->devinit(e) == 0) {
					int r = posix_mount(e->mountpoint);
					if (r == 0) {
						printf("Mount sucess\r\n");
					} else {
						printf("Error: mount return %d\r\n", r);
						if (e->devdone(e) != 0)
							printf("Error during deinit device\r\n");
					}
				} else
					printf("Mount fail init dev %s\r\n", dev);
				return 0;
			}
		}
		printf("%s not in fstab\r\n", dev);
	}
	return 0;
}

int cmd_unmount(int argc, char *argv[]) {
	int i;
	if (argc != 2) {
		printf("usage: umount <devname>\r\n");
	} else {
		const char *dev = argv[1];
		printf("Unmounting %s\r\n", dev);
		for (i = 0; i < ASIZE(fstab); i++) {
			const fstab_entry_t *e = &fstab[i];
			if (strcmp(e->mountpoint->mp_path, dev) == 0) {
				int r = posix_unmount(e->mountpoint);
				if (r == 0)
					printf("Unmount success\r\n");
				else
					printf("Error: mount return %d\r\n", r);
				if (e->devdone(e) != 0)
					printf("Error during deinit device\r\n");
				return 0;
			}
		}
		printf("%s not in fstab\r\n", dev);
	}
	return 0;
}
