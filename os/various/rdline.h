#ifndef RDLINE_H_
#define RDLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void (*put)(char c);
	int (*get)(void);
} rdline_io_t;

extern const rdline_io_t rdline_stdout;

/*  History arena layout
 * .---------------------.
 * | pos line entry 0    |
 * | len line entry 0    |
 * | byte 0 line entry 0 |
 * |         ...         |
 * | byte N line entry 0 | <- N == rdline_config_t::buflen
 * +---------------------+
 * | pos line entry 1    |
 * | len line entry 1    |
 * | byte 0 line entry 1 |
 * |         ...         |
 * | byte N line entry 1 |
 * +---------------------+
 * |         ...         |
 * +---------------------+
 * | pos line entry M    | <- M == rdline_config_t::history_size
 * | len line entry M    |
 * | byte 0 line entry M |
 * |         ...         |
 * | byte N line entry M |
 * `---------------------'
 */

typedef struct {
	int pos;
	int len;
} rdline_history_header_t;

#define RDLINE_HISTORY_ARENA_SIZE(line_len, history_len) \
		((line_len + sizeof(rdline_history_header_t)) * history_len)

typedef struct {
	char *buf;
	int buf_size;
	const rdline_io_t *io;
	void *history_arena;
	int history_size;
} rdline_config_t;

#define RDLINE_CONFIG(name, buf, buflen, io, arena) \
	rdline_config_t name = { buf, buflen, io, arena }

typedef struct {
	int pos;
	int len;
	const rdline_config_t *cfg;
} rdline_edit_ctx_t;

typedef struct {
	int curr;
	int len;
} rdline_history_ctx_t;

typedef struct {
	rdline_history_ctx_t history;
	rdline_edit_ctx_t edit;
} rdline_ctx_t;

extern void rdline_init(rdline_ctx_t *rd, const rdline_config_t *cfg);
extern void rdline_read(rdline_ctx_t *rd);

#ifdef __cplusplus
}
#endif

#endif /* RDLINE_H_ */
