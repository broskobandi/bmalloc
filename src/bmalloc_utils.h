#ifndef BMALLOC_UTILS_H
#define BMALLOC_UTILS_H

#include "bmalloc.h"
#include <stdalign.h>
#include <berror.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

#define STATIC_BUFF_SIZE 1024LU * 4
#define MMAP_THRESHOLD 1024LU * 128
#define SBRK_PTR_BUFF_SIZE 1024LU
#define PTR_ALIGN alignof(ptr_t)
#define MAX_ALIGN alignof(max_align_t)
#define PTR_ALIGNED_SIZE ((sizeof(ptr_t) + MAX_ALIGN - 1) & ~(MAX_ALIGN - 1))

typedef enum ptr_storage {
	STATIC,
	SBRK,
	MMAP
} ptr_storage_t;

typedef struct ptr ptr_t;
struct ptr {
	void *data;
	ptr_t *next;
	ptr_t *prev;
	size_t size;
	ptr_storage_t storage;
	bool is_free;
};

typedef struct ptr_list {
	ptr_t head;
	ptr_t *tail;
} ptr_list_t;

extern _Thread_local ptr_list_t g_ptr_list;
extern _Thread_local ptr_list_t g_ptr_free_list;
extern _Thread_local size_t g_num_free_ptrs;

typedef struct static_buff {
	alignas(max_align_t) unsigned char buff[STATIC_BUFF_SIZE];
	size_t offset;
} static_buff_t;
extern _Thread_local static_buff_t g_static_buff;

typedef struct sbrk {
	void *heap_top; // this has to be a pointer to avoid having to call sbrk twice
	uintptr_t offset;
} sbrk_t;
extern _Thread_local sbrk_t g_sbrk;

typedef struct uintptr_data {
	// padding
	uintptr_t ptr_start;
	uintptr_t ptr_end;
	// padding
	uintptr_t data_start;
	uintptr_t data_end;
	size_t total_size;
} uintptr_data_t;

static inline uintptr_data_t get_uintrptr_data(size_t offset, size_t size) {
	uintptr_data_t data = {0};
	data.ptr_start = ((offset + PTR_ALIGN - 1) & ~(PTR_ALIGN - 1));
	data.ptr_end = data.ptr_start + sizeof(ptr_t);
	data.data_start = ((data.ptr_end + MAX_ALIGN - 1) & ~(MAX_ALIGN - 1));
	data.data_end = data.data_start + size;
	data.total_size = data.data_end - data.data_start;
	return data;
}

static inline bool should_use_static(size_t size) {
	uintptr_data_t data = get_uintrptr_data(g_static_buff.offset, size);
	if (data.data_end <= STATIC_BUFF_SIZE)
		return true;
	return false;
}

static inline ptr_t *get_free_ptr(size_t size) {
	if (!g_num_free_ptrs) ERR("g_ptr_free_list is empty.", NULL);
	ptr_t *cur = g_ptr_free_list.tail;
	while (cur) {
		if (cur->size > size && cur->size < size * 2)
			return cur->data;
		cur = cur->next;
	}
	ERR("No suitable free ptr found.", NULL);
}

static inline void *static_buff_alloc(size_t size) {
	uintptr_data_t data = get_uintrptr_data(g_static_buff.offset, size);
	if (!g_ptr_list.tail) g_ptr_list.tail = &g_ptr_list.head;
	g_ptr_list.tail->next = (ptr_t*)&g_static_buff.buff[data.ptr_start];
	g_ptr_list.tail->next->prev = g_ptr_list.tail;
	g_ptr_list.tail = g_ptr_list.tail->next;
	g_ptr_list.tail->next = NULL;
	g_ptr_list.tail->size = size;
	g_ptr_list.tail->storage = STATIC;
	g_ptr_list.tail->data = &g_static_buff.buff[data.data_start];
	g_ptr_list.tail->is_free = false;
	g_static_buff.offset = data.data_end;
	return g_ptr_list.tail->data;
}

static inline bool should_use_sbrk(size_t size) {
	uintptr_data_t data = get_uintrptr_data(g_static_buff.offset, size);
	if (data.data_end > STATIC_BUFF_SIZE && data.data_end < MMAP_THRESHOLD)
		return true;
	return false;
}

static inline void *sbrk_alloc(size_t size) {
	if ((intptr_t)g_sbrk.heap_top < 0) ERR("heap_top is negative.", NULL);
	if (!g_sbrk.heap_top) g_sbrk.heap_top = sbrk(0);
	if (!g_sbrk.heap_top) ERR("Failed to get heap top.", NULL);
	if (!g_sbrk.offset) g_sbrk.offset = (uintptr_t)g_sbrk.heap_top;
	uintptr_data_t data = get_uintrptr_data(g_sbrk.offset, size);
	if (!g_ptr_list.tail) g_ptr_list.tail = &g_ptr_list.head;
	if (data.data_end > LONG_MAX) ERR("data_end is too big to convert to intptr_t.", NULL);
	while (data.data_end > (uintptr_t)g_sbrk.heap_top)
		g_sbrk.heap_top = sbrk((intptr_t)(data.total_size * 2));
	g_ptr_list.tail->next = (ptr_t*)data.ptr_start;
	g_ptr_list.tail->next->prev = g_ptr_list.tail;
	g_ptr_list.tail = g_ptr_list.tail->next;
	g_ptr_list.tail->next = NULL;
	g_ptr_list.tail->size = size;
	g_ptr_list.tail->storage = STATIC;
	g_ptr_list.tail->data = (void*)data.data_start;
	g_ptr_list.tail->is_free = false;
	g_sbrk.offset = data.data_end;
	return g_ptr_list.tail->data;
}

#endif
