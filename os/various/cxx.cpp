#include <stdlib.h>
#include <exception>

#include <stdio.h>

#define USE_NEW
// #define TRACE_NEW

void *operator new(size_t size) throw () {
#ifndef USE_NEW
	return 0l;
#else
	void *ptr = malloc(size);
	if (!ptr)
		abort();
#ifdef TRACE_NEW
	iprintf("new: Allocating %d bytes on 0x%08X\n", size, (unsigned) ptr);
#endif
	return ptr;
#endif
}

void *operator new[](size_t size) throw () {
#ifndef USE_NEW
	return 0l;
#else
	void *ptr = malloc(size);
	if (!ptr)
		abort();
#ifdef TRACE_NEW
	iprintf("new[]: Allocating %d bytes on 0x%08X\n", size, (unsigned) ptr);
#endif
	return ptr;
#endif
}

void operator delete(void *p) throw () {
#ifdef USE_NEW
	free(p);
#endif
}

void operator delete[](void *p) throw () {
#ifdef USE_NEW
	free(p);
#endif
}

extern "C" int __aeabi_atexit(void *obj, void (*dtr)(void *), void *dso_h) {
	(void) obj;
	(void) dtr;
	(void) dso_h;
	return 0;
}

void *__dso_handle = 0;

/**
 * This is an error handler that is invoked by the C++ runtime when a pure virtual function is called.
 * If anywhere in the runtime of your program an object is created with a virtual function pointer not
 * filled in, and when the corresponding function is called, you will be calling a 'pure virtual function'.
 * The handler you describe should be defined in the default libraries that come with your development environment.
 */
extern "C" void __cxa_pure_virtual() {
	while (1)
		;
}

namespace __gnu_cxx {

void __verbose_terminate_handler() {
	while(1)
		;
}

}
 // namespace
