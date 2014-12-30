#include <stddef.h>

#include "rdline.h"

/****************************************************************************/
#include <unistd.h>

static void stdout_put(char c) {
	write(STDOUT_FILENO, &c, 1);
}

static int stdout_get(void) {
	char c;
	if (read(STDIN_FILENO, &c, 1) == 1)
		return c;
	else
		return -1;
}

const rdline_io_t rdline_stdout = { stdout_put, stdout_get };

/****************************************************************************/

static void swp(char *a, char *b) {
	char t = *a;
	*a = *b;
	*b = t;
}

static void rev(char *s1, char *s2) {
	char *a = s1;
	char *b = s2;
	while (a < b)
		swp(a++, b--);
}

static const char DIGITS[] = "0123456789";

static int convascii(char *ptr, unsigned int n) {
	int cnt = 0;
	while (n >= 10) {
		ptr[cnt++] = DIGITS[n % 10];
		n /= 10;
	}
	ptr[cnt++] = DIGITS[n % 10];
	rev(ptr, ptr + cnt - 1);
	return cnt;
}

static void mset(char *dst, char c, size_t z) {
	while (z--)
		*dst++ = c;
}

static int is_printable(char c) {
	return c > 31 && c < 128;
}

/****************************************************************************/

static int rdline_write(const rdline_io_t *io, const char *ptr, size_t len) {
	int i = 0;
	while (i < len)
		io->put(ptr[i++]);
	return i;
}

static int rdline_print(const rdline_io_t *io, const char *ptr) {
	int i = 0;
	while (ptr[i])
		io->put(ptr[i++]);
	return i;
}

static void do_move(const rdline_io_t *io, int n, char dir) {
	char buf[15] = "\x1b[";
	int l = 2 + convascii(buf + 2, n);
	buf[l++] = dir;
	rdline_write(io, buf, l);
}

static void move_left(const rdline_io_t *io, int n) {
	if (n > 0)
		do_move(io, n, 'D');
}

static void move_right(const rdline_io_t *io, int n) {
	if (n > 0)
		do_move(io, n, 'C');
}

static int print_str(const rdline_io_t *io, const char *ptr) {
	return rdline_print(io, ptr);
}

static void clear_line(const rdline_io_t *io) {
	print_str(io, "\x1b[K");
}

/****************************************************************************/

static void insert_char(char c, const rdline_edit_ctx_t *ctx) {
	char *buf = ctx->cfg->buf;
	int max = ctx->len;
	while (max >= ctx->pos) {
		buf[max + 1] = buf[max];
		max--;
	}
	buf[ctx->pos] = c;
}

static void remove_char(const rdline_edit_ctx_t *ctx) {
	char *buf = ctx->cfg->buf;
	int pos = ctx->pos;
	while (pos < ctx->len) {
		buf[pos] = buf[pos + 1];
		pos++;
	}
	buf[pos] = 0;
}

static int print_line(const rdline_edit_ctx_t *ctx) {
	const rdline_io_t *io = ctx->cfg->io;
	const char *ptr = &ctx->cfg->buf[ctx->pos];
	int len = print_str(io, ptr);
	if (len > 0)
		move_left(io, len);
	return len;
}

/****************************************************************************/

static void ncpy(char *dst, const char *src, size_t max) {
	int i;
	for (i = 0; src[i] && (i < max - 1); i++)
		dst[i] = src[i];
	dst[i] = 0;
}

static rdline_history_header_t *get_entry(rdline_ctx_t *h, int n) {
	return (rdline_history_header_t *) (h->edit.cfg->history_arena
			+ (h->edit.cfg->buf_size + sizeof(rdline_history_header_t)) * n);
}

static void history_insert(rdline_ctx_t *h) {
	rdline_history_header_t *e = get_entry(h, h->history.len);
	char *line = (void*) (e + 1);

	e->len = h->edit.len;
	e->pos = h->edit.pos;
	ncpy(line, h->edit.cfg->buf, h->edit.cfg->buf_size);
	h->history.len++;
}

static void history_get(rdline_ctx_t *h) {
	rdline_history_header_t *e = get_entry(h, h->history.curr);
	char *line = (void*) (e + 1);
	ncpy(h->edit.cfg->buf, line, h->edit.cfg->buf_size);
	h->edit.pos = e->pos;
	h->edit.len = e->len;
}

static void redraw_line(const rdline_edit_ctx_t *ctx) {
	const rdline_io_t *io = ctx->cfg->io;
	clear_line(io);
	rdline_write(io, ctx->cfg->buf, ctx->len);
	move_left(io, ctx->len - ctx->pos);
}

static void rdline_backspace(rdline_edit_ctx_t *ctx) {
	if (ctx->pos > 0) {
		const rdline_io_t *io = ctx->cfg->io;
		ctx->pos--;
		remove_char(ctx);
		move_left(io, 1);
		clear_line(io);
		print_line(ctx);
		ctx->len--;
	}
}

static void move_home(rdline_edit_ctx_t *ctx) {
	move_left(ctx->cfg->io, ctx->pos);
	ctx->pos = 0;
}

static void rdline_clear(rdline_edit_ctx_t *ctx) {
	mset(ctx->cfg->buf, 0, ctx->cfg->buf_size);
	ctx->pos = 0;
	ctx->len = 0;
}

typedef enum {
	RDLINE_ON_EOF,
	RDLINE_ON_ENTER,
	RDLINE_ON_UP,
	RDLINE_ON_DOWN,
	RDLINE_ON_CTRLC,
	RDLINE_ON_CTRLD
} rdline_status_t;

rdline_status_t rdline_input(rdline_edit_ctx_t *ctx) {
	const rdline_io_t *io = ctx->cfg->io;
	while (1) {
		int c = io->get();
		switch (c) {
		case -1:
			return RDLINE_ON_EOF;
		case '\r':
		case '\n':
			return RDLINE_ON_ENTER;
		case '\b':
		case 127:
			rdline_backspace(ctx);
			break;
		case 27:
			c = io->get();
			if (c == 91) { // Arrow
				c = io->get();
				switch (c) {
				case 66: // down
					return RDLINE_ON_DOWN;
				case 65: // up
					return RDLINE_ON_UP;
				case 67: // right
					if (ctx->pos < ctx->len) {
						ctx->pos++;
						move_right(io, 1);
					}
					break;
				case 68: // left
					if (ctx->pos > 0) {
						ctx->pos--;
						move_left(io, 1);
					}
					break;
				case 72: // HOME
					move_home(ctx);
					break;
				case 70: // END
					move_right(io, ctx->len - ctx->pos);
					ctx->pos = ctx->len;
					break;
				case 51: // DEL
					io->get(); // read the ~ char
					if (ctx->pos < ctx->len) {
						remove_char(ctx);
						clear_line(io);
						print_line(ctx);
						ctx->len--;
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			if (ctx->len < ctx->cfg->buf_size - 1 && is_printable(c)) {
				insert_char(c, ctx);
				print_line(ctx);
				move_right(io, 1);
				ctx->pos++;
				ctx->len++;
			}
			break;
		}
	}
	return RDLINE_ON_EOF;
}

void rdline_init(rdline_ctx_t *rd, const rdline_config_t *cfg) {
	rd->edit.cfg = cfg;
	rd->history.curr = 0;
	rd->history.len = 0;
}

void rdline_read(rdline_ctx_t *ctx) {
	rdline_clear(&ctx->edit);
	ctx->history.curr = ctx->history.len;
loop:
	switch (rdline_input(&ctx->edit)) {
	case RDLINE_ON_ENTER:
		rdline_print(ctx->edit.cfg->io, "\r\n");
		if (ctx->edit.len > 0)
			history_insert(ctx);
		break;
	case RDLINE_ON_UP:
		if (ctx->history.curr > 0) {
			move_home(&ctx->edit);
			ctx->history.curr--;
			history_get(ctx);
			redraw_line(&ctx->edit);
		}
		goto loop;
	case RDLINE_ON_DOWN:
		move_home(&ctx->edit);
		if (ctx->history.curr < ctx->history.len) {
			ctx->history.curr++;
			history_get(ctx);
		} else
			rdline_clear(&ctx->edit);
		redraw_line(&ctx->edit);
		goto loop;
	default:
		break;
	}
}
