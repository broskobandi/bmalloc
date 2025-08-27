#ifndef BMALLOC_UTILS_H
#define BMALLOC_UTILS_H

#include "bmalloc.h"
#include <stdalign.h>
#include <berror.h>

#define STATIC_BUFF_SIZE 1024LU * 4

typedef enum ptr_state {
	FREE,
	STATIC,
	SBRK,
	MMAP
} ptr_state_t;

typedef struct ptr {
	void *data;
	size_t size;
	size_t alignment;
	ptr_state_t state;
} ptr_t;

typedef struct static_buff {
	alignas(max_align_t) unsigned char buff[STATIC_BUFF_SIZE];
	size_t offset;
} static_buff_t;
extern _Thread_local static_buff_t g_static_buff;

typedef struct sbrk_buff {
	unsigned char *data;
	size_t offset;
	size_t capacity;
} sbrk_buff_t;
extern _Thread_local sbrk_buff_t g_sbrk_buff;

static inline size_t get_total_size(size_t offset, size_t alignment, size_t size) {
	return ((offset + alignment - 1) & ~(alignment - 1)) + size;
}

static inline void *static_buff_alloc(size_t size, size_t alignment) {
	size_t total_size = get_total_size(g_static_buff.offset, alignment, size);
	if (total_size > STATIC_BUFF_SIZE) ERR("size is too big.", NULL);
	void *ptr = &g_static_buff.buff[total_size - size];
	g_static_buff.offset = total_size;
	return ptr;
}

// static inline void *sbrk_buff_alloc(size_t size, size_t alignment) {
//
// }

#endif
