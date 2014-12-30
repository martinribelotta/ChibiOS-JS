#include <exception>

#define USE_MALLOC_NEW

#ifdef USE_MALLOC_NEW
#include <stdlib.h>
#endif

void *operator new(size_t size) throw () {
#ifdef USE_MALLOC_NEW
	void *ptr = malloc(size);
	if (!ptr)
		abort();
	return ptr;
#else
	return 0l;
#endif
}

void *operator new[](size_t size) throw () {
#ifdef USE_MALLOC_NEW
	void *ptr = malloc(size);
	if (!ptr)
		abort();
	return ptr;
#else
	return 0l;
#endif
}

void operator delete(void *p) throw () {
#ifdef USE_MALLOC_NEW
	free(p);
#endif
}

void operator delete[](void *p) throw () {
#ifdef USE_MALLOC_NEW
	free(p);
#endif
}

// FIXME: Integrate it with object destruction engine
void *__dso_handle = 0;
extern "C" int __aeabi_atexit(void *obj, void (*dtr)(void *), void *dso_h) {
	(void) obj;
	(void) dtr;
	(void) dso_h;
	return 0;
}

// Handle pure virtual call. Replace for libstdcxx free over 64K
extern "C" void __cxa_pure_virtual() {
	while (1)
		;
}

namespace __gnu_cxx {

void __verbose_terminate_handler() {
	while (1)
		;
}

}
