#include <ch.h>
#include <hal.h>
#include "test.h"

#include <stdio.h>

extern SerialUSBDriver SDU1;

#define TEST_WA_SIZE    THD_WA_SIZE(256)

int cmd_mem(int argc, char *argv[]) {
	size_t n, size;

	(void) argv;
	if (argc > 1) {
		printf("Usage: mem\r\n");
		return 0;
	}
	n = chHeapStatus(NULL, &size);
	printf("core free memory : %u bytes\r\n", chCoreStatus());
	printf("heap fragments   : %u\r\n", n);
	printf("heap free total  : %u bytes\r\n", size);
	return 0;
}

int cmd_threads(int argc, char *argv[]) {
	static const char *states[] = { THD_STATE_NAMES };
	Thread *tp;

	(void) argv;
	if (argc > 1) {
		printf("Usage: threads\r\n");
		return 0;
	}
	printf("    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do {
		printf("%.8lx %.8lx %4lu %4lu %9s %lu\r\n", (uint32_t) tp,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t) (tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL );
	return 0;
}

int cmd_test(int argc, char *argv[]) {
	Thread *tp;

	(void) argv;
	if (argc > 1) {
		printf("Usage: test\r\n");
		return 0;
	}
	tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(), TestThread,
			&SDU1);
	if (tp == NULL ) {
		printf("out of memory\r\n");
		return 0;
	}
	chThdWait(tp);
	return 0;
}
