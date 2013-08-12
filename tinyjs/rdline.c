#include <stddef.h>

#include "rdline.h"

#ifdef __linux__
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#endif

static int is_printable(char c) {
	return c > 31 && c < 128;
}

static void insert_char(char c, const rdline_ctx_t *ctx) {
	int max = ctx->len;
	while (max >= ctx->pos) {
		ctx->buf[max + 1] = ctx->buf[max];
		max--;
	}
	ctx->buf[ctx->pos] = c;
}

static void remove_char(const rdline_ctx_t *ctx) {
	int pos = ctx->pos;
	while (pos < ctx->len) {
		ctx->buf[pos] = ctx->buf[pos + 1];
		pos++;
	}
	ctx->buf[pos] = 0;
}

static int rdline_write(const rdline_cfg_t *cfg, const char *ptr, size_t len) {
	int i = 0;
	while (i < len)
		cfg->put(ptr[i++]);
	return i;
}

static int rdline_print(const rdline_cfg_t *cfg, const char *ptr) {
	int i = 0;
	while (ptr[i])
		cfg->put(ptr[i++]);
	return i;
}

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

static void do_move(const rdline_cfg_t *cfg, int n, char dir) {
	char buf[15] = "\x1b[";
	int l = 2 + convascii(buf + 2, n);
	buf[l++] = dir;
	rdline_write(cfg, buf, l);
}

static void move_left(const rdline_cfg_t *cfg, int n) {
	if (n > 0)
		do_move(cfg, n, 'D');
}

static void move_right(const rdline_cfg_t *cfg, int n) {
	if (n > 0)
		do_move(cfg, n, 'C');
}

static int print_str(const rdline_cfg_t *cfg, const char *ptr) {
	return rdline_print(cfg, ptr);
}

static int print_line(const rdline_ctx_t *ctx) {
	const rdline_cfg_t *cfg = ctx->cfg;
	const char *ptr = &ctx->buf[ctx->pos];
	int len = print_str(cfg, ptr);
	if (len > 0)
		move_left(cfg, len);
	return len;
}

static void clear_line(const rdline_cfg_t *cfg) {
	print_str(cfg, "\x1b[K");
}

static void ncpy(char *dst, const char *src, size_t max) {
	int i;
	for (i = 0; src[i] && (i < max - 1); i++)
		dst[i] = src[i];
	dst[i] = 0;
}

static void mset(char *dst, char c, size_t z) {
	while (z--)
		*dst++ = c;
}

static void history_insert(history_buffer_t *h, const rdline_ctx_t *ctx) {
	if (h->next >= RDLINE_HISTORY_LEN)
		h->next = 0;
	history_entry_t *e = &h->history[h->next];
	e->len = ctx->len;
	e->pos = ctx->pos;
	ncpy(e->line, ctx->buf, RDLINE_EDIT_SIZE);
	h->next++;
}

static void history_get(unsigned int n, const history_buffer_t *h,
		rdline_ctx_t *ctx) {
	if (n < RDLINE_HISTORY_LEN) {
		const history_entry_t *e = &h->history[n];

		ncpy(ctx->buf, e->line, RDLINE_EDIT_SIZE);
		ctx->pos = e->pos;
		ctx->len = e->len;
	}
}

static void redraw_line(const rdline_ctx_t *ctx) {
	clear_line(ctx->cfg);
	rdline_write(ctx->cfg, ctx->buf, ctx->len);
	move_left(ctx->cfg, ctx->len - ctx->pos);
}

static void rdline_backspace(rdline_ctx_t *ctx) {
	if (ctx->pos > 0) {
		ctx->pos--;
		remove_char(ctx);
		move_left(ctx->cfg, 1);
		clear_line(ctx->cfg);
		print_line(ctx);
		ctx->len--;
	}
}

static void move_home(rdline_ctx_t *ctx) {
	move_left(ctx->cfg, ctx->pos);
	ctx->pos = 0;
}

static void rdline_clear(rdline_ctx_t *ctx) {
	mset(ctx->buf, 0, RDLINE_EDIT_SIZE);
	ctx->pos = 0;
	ctx->len = 0;
}

rdline_status_t rdline_input(rdline_ctx_t *ctx) {
	const rdline_cfg_t *cfg = ctx->cfg;
	while (1) {
		int c = cfg->get();
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
			c = cfg->get();
			if (c == 91) { // Arrow
				c = cfg->get();
				switch (c) {
				case 66: // down
					return RDLINE_ON_DOWN;
				case 65: // up
					return RDLINE_ON_UP;
				case 67: // right
					if (ctx->pos < ctx->len) {
						ctx->pos++;
						move_right(cfg, 1);
					}
					break;
				case 68: // left
					if (ctx->pos > 0) {
						ctx->pos--;
						move_left(cfg, 1);
					}
					break;
				case 72: // HOME
					move_home(ctx);
					break;
				case 70: // END
					move_right(cfg, ctx->len - ctx->pos);
					ctx->pos = ctx->len;
					break;
				case 51: // DEL
					cfg->get(); // read the ~ char
					if (ctx->pos < ctx->len) {
						remove_char(ctx);
						clear_line(cfg);
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
			if (ctx->len < RDLINE_EDIT_SIZE - 1 && is_printable(c)) {
				insert_char(c, ctx);
				print_line(ctx);
				move_right(cfg, 1);
				ctx->pos++;
				ctx->len++;
			}
			break;
		}
	}
	return RDLINE_ON_EOF;
}

void rdline_read(line_t buf, history_buffer_t *h, const rdline_cfg_t *cfg) {
	int current_h = h->next;
	rdline_ctx_t ctx = { buf, 0, 0, cfg };
#ifdef __linux__
	int i;
	struct termios param;
	struct termios tioparam;

	tcgetattr(0, &tioparam);
	param = tioparam;
	cfmakeraw(&param);
	tcsetattr(0, TCSADRAIN, &param);
#endif
	rdline_clear(&ctx);
	loop: switch (rdline_input(&ctx)) {
	case RDLINE_ON_ENTER:
		rdline_print(cfg, "\r\n");
		if (ctx.len > 0)
			history_insert(h, &ctx);
		break;
	case RDLINE_ON_DOWN:
		if (current_h < h->next) {
			move_home(&ctx);
			history_get(++current_h, h, &ctx);
			redraw_line(&ctx);
		}
		goto loop;
	case RDLINE_ON_UP:
		if (current_h > 0) {
			move_home(&ctx);
			history_get(--current_h, h, &ctx);
			redraw_line(&ctx);
		}
		goto loop;
	default:
		break;
	}
#ifdef __linux__
	tcsetattr(0, TCSADRAIN, &tioparam);
#if 0
	printf("\n<<%s>>\n", ctx.buf);
#endif
#endif
}
