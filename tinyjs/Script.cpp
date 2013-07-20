/*
 * TinyJS
 *
 * A single-file Javascript-alike engine
 *
 * Authored By Gordon Williams <gw@pur3.co.uk>
 *
 * Copyright (C) 2009 Pur3 Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This is a simple program showing how to use TinyJS
 */

#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#ifdef __linux__
#include <termios.h>
#endif

void js_print(CScriptVar *v, void *userdata) {
	printf("> %s\n", v->getParameter("text")->getString().c_str());
}

void js_dump(CScriptVar *v, void *userdata) {
	CTinyJS *js = (CTinyJS*) userdata;
	js->root->trace(">  ");
}

static void insert_char(char c, char *buffer, int pos, int len) {
	int max = len;
	while (max >= pos) {
		buffer[max + 1] = buffer[max];
		max--;
	}
	buffer[pos] = c;
}

static void remove_char(char *buffer, int pos, int len) {
	while (pos < len) {
		buffer[pos] = buffer[pos + 1];
		pos++;
	}
	buffer[pos] = 0;
}

static void move_left(int fd, int n) {
	char buf[10];
	int l = sprintf(buf, "\x1b[%dD", n);
	write(fd, buf, l);
}

static void move_right(int fd, int n) {
	if (n) {
		char buf[10];
		int l = sprintf(buf, "\x1b[%dC", n);
		write(fd, buf, l);
	}
}

static int print_str(int fd, const char *ptr) {
	int len = strlen(ptr);
	if (len>0)
		return write(fd, ptr, len);
	return 0;
}

static int print_line(int fd, const char *ptr) {
	int len = print_str(fd, ptr);
	if (len > 0)
		move_left(fd, len);
	return len;
}

static void clear_line(int fd) {
	print_str(fd, "\x1b[K");
}

typedef enum {
	READ_NONE,
	READ_ENTER,
	READ_UP,
	READ_DOWN
} read_status_t;

read_status_t read_command(char *buf, int size, FILE *f) {
	read_status_t stat = READ_NONE;
	bool on_loop = true;
	int len = strlen(buf);
	int pos = len;
	int fd = fileno(f);
	print_line(fd, buf);
	move_right(fd, pos);
#ifdef __linux__
	int i;
	struct termios param;
	struct termios tioparam;
	tcgetattr(0, &tioparam);
	param = tioparam;
	cfmakeraw(&param);
	tcsetattr(0, TCSADRAIN, &param);
#endif
	// memset(buf, 0, size);
	setvbuf(stdin, NULL, _IONBF, 0);
	while (on_loop) {
		char c;
		if (read(fd, &c, 1) >= 1)
			switch (c) {
			case '\r':
			case '\n':
				print_str(fd, "\r\n");
				stat = READ_ENTER;
				on_loop = false;
				break;
			case '\b':
			case 127:
				if (pos > 0) {
					pos--;
					remove_char(buf, pos, len);
					move_left(fd, 1);
					clear_line(fd);
					print_line(fd, buf + pos);
					len--;
				}
				break;
			case 27:
				read(fd, &c, 1);
				if (c == 91) // Arrow
					read(fd, &c, 1);
					switch (c) {
					case 65: // up TODO
						//move_left(fd, pos - 1);
						//clear_line(fd);
						print_str(fd, "\r\n");
						stat = READ_UP;
						on_loop = false;
						break;
					case 66: // down TODO
						//move_left(fd, pos - 1);
						//clear_line(fd);
						print_str(fd, "\r\n");
						stat = READ_DOWN;
						on_loop = false;
						break;
					case 67: // right
						if (pos < len) {
							pos++;
							move_right(fd, 1);
						}
						break;
					case 68: // left
						if (pos > 0) {
							pos--;
							move_left(fd, 1);
						}
						break;
					case 72: // HOME
						move_left(fd, pos);
						pos = 0;
						break;
					case 70: // END
						move_right(fd, len - pos);
						pos = len;
						break;
					case 51: // DEL
						read(fd, &c, 1); // read the ~ char
						if (pos<len) {
							remove_char(buf, pos, len);
							clear_line(fd);
							print_line(fd, buf + pos);
							len--;
						}
						break;
					default:
						break;
					}
				break;
			default:
				if (len < size - 1 && isprint(c)) {
					insert_char(c, buf, pos, len);
					print_line(fd, buf + pos);
					move_right(fd, 1);
					pos++;
					len++;
				}
				break;
			}
	}
#ifdef __linux__
	tcsetattr(0, TCSADRAIN, &tioparam);
#endif
#if 1
	printf("\n<<%s>>\n", buf);
#endif
	return stat;
}

static char buffer[10][2048];

#ifdef __linux__
int main(int argc, char **argv)
#else
extern "C" int js_main(int argc, char **argv)
#endif
		{
	/*
	 * Get environ variable DMALLOC_OPTIONS and pass the settings string
	 * on to dmalloc_debug_setup to setup the dmalloc debugging flags.
	 */

	CTinyJS *js = new CTinyJS();
	/* add the functions from TinyJS_Functions.cpp */
	registerFunctions(js);
	/* Add a native function */
	js->addNative("function print(text)", &js_print, 0);
	js->addNative("function dump()", &js_dump, js);
	/* Execute out bit of code - we could call 'evaluate' here if
	 we wanted something returned */
	try {
		js->execute(
				"var lets_quit = 0;"
				"function quit() {"
				"  lets_quit = 1;"
				"}");
		js->execute(
				"print(\"Interactive mode...\n"
				"Type quit(); to exit,\n"
				"or print(...); to print something,\n"
				"or dump() to dump the symbol table!\");");
	} catch (CScriptException *e) {
		printf("ERROR: %s\n", e->text.c_str());
	}

	int current_buffer = 0;
	int max_buffer = 0;
	while (js->evaluate("lets_quit") == "0") {
		retry:
		printf("js(%d-%d)> ", current_buffer, max_buffer);
		fflush(stdout);
		char *ptr = &buffer[current_buffer][0];
#if 0
		fgets(buffer, sizeof(buffer), stdin);
#else
		switch (read_command(ptr, 2048, stdin)) {
		case READ_ENTER:
			max_buffer++;
			if (max_buffer >= 10)
				max_buffer = 0;
			current_buffer = max_buffer;
			break;
		case READ_UP:
			if (current_buffer > 0)
				current_buffer--;
			goto retry;
			break;
		case READ_DOWN:
			if (current_buffer < max_buffer && max_buffer < 10)
				current_buffer++;
			goto retry;
			break;
		}
#endif
		try {
			js->execute(ptr);
		} catch (CScriptException *e) {
			printf("ERROR: %s\n", e->text.c_str());
		}
	}
	delete js;
	return 0;
}
