#ifndef RDLINE_H_
#define RDLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RDLINE_EDIT_SIZE 128
#define RDLINE_HISTORY_LEN 10

typedef char line_t[RDLINE_EDIT_SIZE];

typedef struct {
	int pos;
	int len;
	line_t line;
} history_entry_t;

typedef struct {
	int next;
	history_entry_t history[RDLINE_HISTORY_LEN];
} history_buffer_t;

typedef struct {
	void (*put)(char c);
	int (*get)(void);
} rdline_cfg_t;

typedef struct {
	char *buf;
	int pos;
	int len;
	const rdline_cfg_t *cfg;
} rdline_ctx_t;

typedef enum {
	RDLINE_ON_EOF,
	RDLINE_ON_ENTER,
	RDLINE_ON_UP,
	RDLINE_ON_DOWN,
	RDLINE_ON_CTRLC,
	RDLINE_ON_CTRLD
} rdline_status_t;

extern void history_init(history_buffer_t *h);
extern rdline_status_t rdline_input(rdline_ctx_t *ctx);
extern void rdline_read(line_t buf, history_buffer_t *h, const rdline_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* RDLINE_H_ */
