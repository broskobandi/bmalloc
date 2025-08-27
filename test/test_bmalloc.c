#include "test_bmalloc.h"
#include "bmalloc.h"
#include "bmalloc_utils.h"
#include <string.h>

void test_static_buff_alloc() {
	memset(&g_static_buff, 0, sizeof(static_buff_t));
	{ // Normal case
		int *ptr = static_buff_alloc(sizeof(int), alignof(int));
		BASSERT(ptr);
		size_t total_size =
			get_total_size(g_static_buff.offset, alignof(int), sizeof(int));
		BASSERT(g_static_buff.offset = total_size);
		*ptr = 5;

		int *ptr2 = static_buff_alloc(sizeof(int), alignof(int));
		BASSERT(ptr2);
		total_size =
			get_total_size(g_static_buff.offset, alignof(int), sizeof(int));
		BASSERT(g_static_buff.offset = total_size);
		*ptr2 = 10;

		BASSERT(*ptr == 5);
		BASSERT(*ptr2 == 10);
		memset(&g_static_buff, 0, sizeof(static_buff_t));
	}
}

void test_sbrk_alloc() {
	{ // Normal case
		g_sbrk.heap_top = sbrk(0);
		int *ptr = sbrk_alloc(sizeof(int), alignof(int));
		BASSERT(ptr);
		if (!ptr) berror_print();
		*ptr = 5;

		int *ptr2 = sbrk_alloc(sizeof(int), alignof(int));
		BASSERT(ptr2);
		*ptr2 = 10;

		BASSERT(*ptr == 5);
		BASSERT(*ptr2 == 10);
	}
	{ // heap_top negative
		g_sbrk.heap_top = (void*)-1;
		int *ptr = sbrk_alloc(sizeof(int), alignof(int));
		BASSERT(!ptr);
		berror_info_t err = berror_get();
		BASSERT(!strcmp(err.msg, "Cannot convert heap_top to size_t safely."));
		g_sbrk.heap_top = sbrk(0);
		berror_reset();
	}
	{ // size_to_allocate too big
		int *ptr = sbrk_alloc((size_t)LONG_MAX * 2, alignof(int));
		BASSERT(!ptr);
		berror_info_t err = berror_get();
		BASSERT(!strcmp(err.msg, "Cannot convert size_to_allocate to intptr_t safely."));
		g_sbrk.heap_top = sbrk(0);
	}
}

void test_mmap_alloc() {
	{ // Normal case
		int *ptr = mmap_alloc(sizeof(int));
		BASSERT(ptr);
		*ptr = 5;
		BASSERT(*ptr == 5);
	}
}
