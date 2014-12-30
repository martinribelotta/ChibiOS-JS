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

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>       // std::string

#include <rdline.h>

void js_print(CScriptVar *v, void *userdata) {
	printf("> %s\n", v->getParameter("text")->getString().c_str());
}

void js_dump(CScriptVar *v, void *userdata) {
	CTinyJS *js = (CTinyJS*) userdata;
	js->root->trace(">  ");
}

#define HISTORY_LEN 10
#define LINE_LEN 80

static char history_arena[RDLINE_HISTORY_ARENA_SIZE(LINE_LEN, HISTORY_LEN)];
static char buffer[LINE_LEN];
static const RDLINE_CONFIG(rd, buffer, sizeof(buffer), &rdline_stdout, history_arena);

std::string readall(const char *filename) {
	std::ifstream is(filename, std::ifstream::in);
	if (is) {
		std::string text;
		is.seekg(0, std::ifstream::end);
		text.resize(is.tellg());
		is.seekg(0, std::ifstream::beg);
		is.read(&text[0], text.size());
		is.close();
		return text;
	}
	return std::string();
}

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
	if (argc > 1) {
		for (int i = 1; i < argc; i++)
			js->execute(readall(argv[1]));
	} else {
		try {
			js->execute("var lets_quit = 0;"
					"function quit() {"
					"  lets_quit = 1;"
					"}");
			js->execute("print(\"Interactive mode...\n"
					"Type quit(); to exit,\n"
					"or print(...); to print something,\n"
					"or dump() to dump the symbol table!\");");
		} catch (CScriptException *e) {
			printf("ERROR: %s\n", e->text.c_str());
		}

		rdline_ctx_t rdctx;
		rdline_init(&rdctx, &rd);
		while (js->evaluate("lets_quit") == "0") {
			printf("js> ");
			fflush(stdout);
#if 0
			fgets(buffer, sizeof(buffer), stdin);
#else
			rdline_read(&rdctx);
#endif
			try {
				js->execute(buffer);
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
		}
	}
	delete js;
	return 0;
}
