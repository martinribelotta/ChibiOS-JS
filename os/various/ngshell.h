#ifndef NGSHELL_H_
#define NGSHELL_H_

#include <stddef.h>
#include <rdline.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NGSHELL_MAXARGV 10

typedef struct {
	const char *command;
	int (*function)(int argc, char **argv);
} ngshell_cmd_t;

typedef struct {
	const char *promt;
	const ngshell_cmd_t *commands;
	const rdline_config_t *cfg;
	char *argvbuf[NGSHELL_MAXARGV];
} ngshell_t;

#define NGSHELL_DECLARE(name, promt, cmds, cfg) ngshell_t name = { \
		(promt), \
		(cmds), \
		(cfg), \
		{ NULL } \
	}

extern void *ngshell_init(ngshell_t *sh, void *wsp, size_t size, int pr);

#ifdef __cplusplus
}
#endif

#endif /* NGSHELL_H_ */
