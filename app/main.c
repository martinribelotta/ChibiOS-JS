#include <ch.h>
#include <hal.h>
#include <FatFSWrapper/fatfsWrapper.h>
#include <posix/posix.h>
#include <posix/posix_chstream.h>

#include <ngshell.h>

#include "usbcfg.h"
#include "shell_cmd/cmd.h"

#include <stdio.h>

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(1024*32)

static WORKING_AREA(sh1_wa, SHELL_WA_SIZE);

static int cmd_uname(int argc, char *argv[]) {
	(void) argc;
	(void) argv;
	printf("Kernel:       %s\n", CH_KERNEL_VERSION);
#ifdef CH_COMPILER_NAME
	printf("Compiler:     %s\n", CH_COMPILER_NAME);
#endif
	printf("Architecture: %s\n", CH_ARCHITECTURE_NAME);
#ifdef CH_CORE_VARIANT_NAME
	printf("Core Variant: %s\n", CH_CORE_VARIANT_NAME);
#endif
#ifdef CH_PORT_INFO
	printf("Port Info:    %s\n", CH_PORT_INFO);
#endif
#ifdef PLATFORM_NAME
	printf("Platform:     %s\n", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
	printf("Board:        %s\n", BOARD_NAME);
#endif
	printf("Build time:   %s%s%s\n", __DATE__, " - ", __TIME__);
	return 0;
}

extern int js_main(int argc, char **argv);

static const ngshell_cmd_t cmdlist[] = {
		{ "uname", cmd_uname }, /**/
		{ "mem", cmd_mem }, /**/
		{ "threads", cmd_threads }, /**/
		{ "js", js_main }, /**/
		{ "test", cmd_test }, /**/
		{ "mount", cmd_mount }, /**/
		{ "umount", cmd_unmount }, /**/
		{ "ls", cmd_ls }, /**/
		{ "cat", cmd_cat }, /**/
		{ NULL, NULL } /**/
};

#define SH1_LINELEN 80
#define SH1_HISTORYLEN 10

static char sh1_b[SH1_LINELEN];
static char sh1_hist[RDLINE_HISTORY_ARENA_SIZE(SH1_LINELEN, SH1_HISTORYLEN)];
static RDLINE_CONFIG(sh1_rd, sh1_b, sizeof(sh1_b), &rdline_stdout, sh1_hist);
static NGSHELL_DECLARE(sh1, "sh> ", cmdlist, &sh1_rd);

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void) {
	/* HAL&RTOS init */
	halInit();
	chSysInit();

	/* SD/FATFS init */
	sdcStart(&SDCD1, NULL );
	wf_init(NORMALPRIO);

	/* POSIX compatibility layer initialization */
	posix_init();
	posix_init_chstream(STDIN_FILENO, (BaseSequentialStream *) &SDU1);
	posix_init_chstream(STDOUT_FILENO, (BaseSequentialStream *) &SDU1);
	posix_init_chstream(STDERR_FILENO, (BaseSequentialStream *) &SDU1);

	/* Initializes a serial-over-USB CDC driver */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	ngshell_init(&sh1, sh1_wa, SHELL_WA_SIZE, NORMALPRIO);

	while (TRUE)
		chThdSleepMilliseconds(500);
}
