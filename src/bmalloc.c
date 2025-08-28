#include "bmalloc_utils.h"

_Thread_local static_buff_t g_static_buff = {0};
_Thread_local sbrk_t g_sbrk = {0};
_Thread_local ptr_list_t g_ptr_list = {0};
_Thread_local ptr_list_t g_ptr_free_list = {0};
_Thread_local size_t g_num_free_ptrs = 0;

void *bmalloc(size_t size) {
	if (!size) ERR("size must not be 0.", NULL);
	void *ptr = NULL;
	if (g_num_free_ptrs)
		ptr = get_free_ptr(size);
	if (ptr) return ptr;
	if (should_use_static(size))
		ptr = static_buff_alloc(size);
	if (should_use_sbrk(size))
		ptr = sbrk_alloc(size);
	if (should_use_mmap(size))
		ptr = mmap_alloc(size);
	if (!ptr) ERR("Failed to find appropriate allocation method.", NULL);
	return ptr;
}
