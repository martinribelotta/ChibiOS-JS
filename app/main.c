/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "test.h"
#include <FatFSWrapper/fatfsWrapper.h>
#include <posix/posix.h>
#include <posix/posix_chstream.h>
#include <posix/posix_fatfs_provider.h>

#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"

#include <stdio.h>

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

/**
 * @brief FS object.
 */
static FATFS SDC_FS;
static posix_file_provider_t fatfs_provider;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(1024*32)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

static void cmd_uname(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void) argc;
	(void) argv;
	chprintf(chp, "ChibiOS build on %s %s\r\n", __DATE__, __TIME__);
}

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
	size_t n, size;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_js(BaseSequentialStream *chp, int argc, char *argv[]) {
	static const char *default_param[] = { "js" };
	extern int js_main(int argc, const char **argv);
	js_main(1, default_param);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
	static const char *states[] = { THD_STATE_NAMES };
	Thread *tp;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do {
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n", (uint32_t) tp,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t) (tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL );
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
	Thread *tp;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: test\r\n");
		return;
	}
	tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(), TestThread,
			chp);
	if (tp == NULL ) {
		chprintf(chp, "out of memory\r\n");
		return;
	}
	chThdWait(tp);
}

static int conv_int(const char *str, int *ok) {
	int n = 0;
	int result = sscanf(str, "%i", &n) == 1;
	if (ok)
		*ok = result;
	return n;
}

static void cmd_mount(BaseSequentialStream *chp, int argc, char *argv[]) {
	if (argc > 0 && argc < 2) {
		if (sdcConnect(&SDCD1) == CH_FAILED) {
			chprintf(chp, "ERROR: SD not detected\r\n");
			return;
		} else {
			int ok = FALSE;
			int n = conv_int(argv[0], &ok);
			if (ok) {
				int err = posix_fatfs_provider_init(&fatfs_provider, &SDC_FS, n);
				// FRESULT err = wf_mount(n, &SDC_FS);
				if (err != FR_OK) {
					sdcDisconnect(&SDCD1);
					chprintf(chp, "ERROR: Can not mount SD filesystem %s\r\n",
							wf_strerror(err));
					return;
				} else {
					posix_add_fileprovider(&fatfs_provider);
					chprintf(chp, "Filesystem %d mounted succefull\r\n", n);
				}
			} else
				chprintf(chp, "Invalid file number %d\r\n", n);
		}
	} else
		chprintf(chp, "USAGE: mount <unit>\r\n");
}

static void cmd_umount(BaseSequentialStream *chp, int argc, char *argv[]) {
	sdcDisconnect(&SDCD1);
	chprintf(chp, "umount done\r\n");
}

static void cmd_ls(BaseSequentialStream *chp, int argc, char *argv[]) {
	posix_dir_t dir;
	if (argc>0)
		dir.path = argv[1];
	else
		dir.path = "0:/";
	if (posix_opendir(&dir) == 0) {
		posix_inode_t inode;
		posix_readdir_result_t r;
		while((r = posix_readdir(&dir, &inode)) == POSIX_READDIR_OK)
			chprintf(chp, "> %s\r\n", inode.path);
		if (r == POSIX_READDIR_ERR)
			chprintf(chp, "Error reading dir\r\n");
		posix_closedir(&dir);
	} else
		chprintf(chp, "Can not open %s\r\n", dir.path);
}

static const ShellCommand commands[] = { /**/
{ "uname", cmd_uname }, /**/
{ "mem", cmd_mem }, /**/
{ "threads", cmd_threads }, /**/
{ "js", cmd_js }, /**/
{ "test", cmd_test }, /**/
{ "mount", cmd_mount }, /**/
{ "umount", cmd_umount }, /**/
{ "ls", cmd_ls }, /**/
{ NULL, NULL } /**/
};

static const ShellConfig shell_cfg1 =
		{ (BaseSequentialStream *) &SDU1, commands };

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void) {
	Thread *shelltp = NULL;

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * SD/FATFS initialization
	 */
	sdcStart(&SDCD1, NULL );
	wf_init(NORMALPRIO);

	/*
	 * POSIX compativility layer initialization
	 */
	posix_init_chstream(0, (BaseSequentialStream *) &SDU1); /* STDIN */
	posix_init_chstream(1, (BaseSequentialStream *) &SDU1); /* STDOUT */
	posix_init_chstream(2, (BaseSequentialStream *) &SDU1); /* STDERR */

	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	/*
	 * Normal main() thread activity, in this demo it just performs
	 * a shell respawn upon its termination.
	 */
	while (TRUE) {
		if (!shelltp) {
			if (SDU1.config->usbp->state == USB_ACTIVE) {
				/* Spawns a new shell.*/
				shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
			}
		} else {
			/* If the previous shell exited.*/
			if (chThdTerminated(shelltp)) {
				/* Recovers memory of the previous shell.*/
				chThdRelease(shelltp);
				shelltp = NULL;
			}
		}
		chThdSleepMilliseconds(500);
	}
}
