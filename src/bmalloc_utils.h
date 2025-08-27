#ifndef BMALLOC_UTILS_H
#define BMALLOC_UTILS_H

#include "bmalloc.h"
#include <stdalign.h>
#include <berror.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>

#define STATIC_BUFF_SIZE 1024LU * 4
#define MMAP_THRESHOLD 1024LU * 128

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

typedef struct sbrk {
	unsigned char *data;
	size_t offset;
	size_t capacity;
	void *heap_top;
} sbrk_t;
extern _Thread_local sbrk_t g_sbrk;

static inline size_t get_total_size(size_t offset, size_t alignment, size_t size) {
	return ((offset + alignment - 1) & ~(alignment - 1)) + size;
}

/** Evaluates whether using the static buffer for the allocation is appropriate.
 * \param alignment The alignment of the data to be allocated. 
 * \param size The size of the data to be allocated. 
 * \return A boolean indicating the result. */
static inline bool should_use_static(size_t alignment, size_t size) {
	size_t total_size = get_total_size(g_static_buff.offset, alignment, size);
	if (total_size <= STATIC_BUFF_SIZE)
		return true;
	return false;
}

/** Allocates memory in the static buffer.
 * This function assumes correct arguments and does not carry out checks.
 * \param size The size of the data.
 * \param alignment The alignment of the data. 
 * \return A pointer to the allocated memory. */
static inline void *static_buff_alloc(size_t size, size_t alignment) {
	size_t total_size = get_total_size(g_static_buff.offset, alignment, size);
	void *ptr = &g_static_buff.buff[total_size - size];
	g_static_buff.offset = total_size;
	return ptr;
}

/** Evaluates whether using the heap with sbrk for the allocation is appropriate.
 * \param alignment The alignment of the data to be allocated. 
 * \param size The size of the data to be allocated. 
 * \return A boolean indicating the result. */
static inline bool should_use_sbrk(size_t alignment, size_t size) {
	size_t total_size = get_total_size(g_static_buff.offset, alignment, size);
	if (total_size > STATIC_BUFF_SIZE && size < MMAP_THRESHOLD)
		return true;
	return false;
}

/** Allocates memory in the heap using sbrk.
 * This function assumes correct arguments and does not carry out checks.
 * \param size The size of the data.
 * \param alignment The alignment of the data. 
 * \return A pointer to the allocated memory. */
static inline void *sbrk_alloc(size_t size, size_t alignment) {
	if (!g_sbrk.heap_top) g_sbrk.heap_top = sbrk(0);
	if ((intptr_t)g_sbrk.heap_top < 0)
		ERR("Cannot convert heap_top to size_t safely.", NULL);
	size_t total_size = get_total_size((size_t)g_sbrk.heap_top, alignment, size);
	size_t padding = total_size - (size_t)g_sbrk.heap_top;
	size_t size_to_allocate = padding + size;
	if (size_to_allocate > LONG_MAX)
		ERR("Cannot convert size_to_allocate to intptr_t safely.", NULL);
	void *ptr = sbrk((intptr_t)size_to_allocate);
	if (!ptr) ERR("sbrk() failed.", NULL);
	g_sbrk.heap_top = ptr + size_to_allocate;
	return g_sbrk.heap_top - size;
}

#endif
