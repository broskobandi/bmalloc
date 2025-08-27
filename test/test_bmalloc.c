#include "test_bmalloc.h"
#include "bmalloc.h"
#include "bmalloc_utils.h"
#include <string.h>

void test_static_buff_alloc() {
	memset(&g_static_buff, 0, sizeof(static_buff_t));
	{ // Normal case
		void *ptr = static_buff_alloc(sizeof(int), alignof(int));
		size_t total_size =
			get_total_size(g_static_buff.offset, alignof(int), sizeof(int));
		BASSERT(ptr);
		BASSERT(g_static_buff.offset = total_size);
		memset(&g_static_buff, 0, sizeof(static_buff_t));
	}
	{ // size is too big
		void *ptr = static_buff_alloc(STATIC_BUFF_SIZE * 2, alignof(int));
		BASSERT(!ptr);
		berror_info_t err = berror_get();
		BASSERT(!strcmp(err.msg, "size is too big."));
		memset(&g_static_buff, 0, sizeof(static_buff_t));
	}
}
