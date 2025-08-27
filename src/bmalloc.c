#include "bmalloc_utils.h"

_Thread_local static_buff_t g_static_buff;
_Thread_local sbrk_buff_t g_sbrk_buff;

// void *bmalloc(size_t size, size_t alignment) {
// 	if (!size) ERR("size must not be 0.", NULL);
// 	if (!alignment) ERR("alignment must not be 0.", NULL);
// 	if (alignment & (alignment - 1)) ERR("alignment must be a power of 2.", NULL);
// 	void *ptr = static_buff_alloc(size, alignment);
// 	if (!ptr) 
// }
