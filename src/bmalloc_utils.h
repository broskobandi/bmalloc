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

#define MMAP(size)\
	mmap(NULL, (size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#define ASSUMED_PAGE_SIZE (size_t)(1024 * 4)
#define BUFF_SIZE ASSUMED_PAGE_SIZE * 8
#define PTR_ALIGN alignof(ptr_t)
#define MAX_ALIGN alignof(max_align_t)
#define PTR_ALIGNED_SIZE ((sizeof(ptr_t) + MAX_ALIGN - 1) & ~(MAX_ALIGN - 1))

typedef struct storage storage_t;
typedef struct ptr ptr_t;
struct ptr {
	storage_t *storage;
	void *data;
	ptr_t *next;
	ptr_t *prev;
	size_t size;
	bool is_free;
};

typedef struct ptr_list {
	ptr_t head;
	ptr_t *tail;
} ptr_list_t;

struct storage {
	alignas(max_align_t) unsigned char buff[BUFF_SIZE];
	size_t offset;
	storage_t *next;
	storage_t *prev;
};
typedef struct storage_list {
	storage_t head;
	storage_t *tail;
} storage_list_t;

typedef struct uintptr_data {
	uintptr_t ptr_start;
	uintptr_t ptr_end;
	uintptr_t data_start;
	uintptr_t data_end;
	size_t total_size;
} uintptr_data_t;

extern _Thread_local storage_list_t g_storage_list;
extern _Thread_local ptr_list_t g_ptr_list;
extern _Thread_local ptr_list_t g_ptr_free_list;
extern _Thread_local size_t g_num_free_ptrs;

static inline uintptr_data_t get_uintrptr_data(size_t offset, size_t size) {
	uintptr_data_t data = {0};
	data.ptr_start = ((offset + PTR_ALIGN - 1) & ~(PTR_ALIGN - 1));
	data.ptr_end = data.ptr_start + sizeof(ptr_t);
	data.data_start = ((data.ptr_end + MAX_ALIGN - 1) & ~(MAX_ALIGN - 1));
	data.data_end = data.data_start + size;
	data.total_size = data.data_end - data.data_start;
	return data;
}

static inline ptr_t *get_free_ptr(size_t size) {
	ptr_t *cur = g_ptr_free_list.tail;
	while (cur) {
		if (cur->size > size && cur->size < size * 2)
			return cur;
		cur = cur->next;
	}
	ERR("No suitable free pointer found.", NULL);
}

static inline ptr_t *mmap_alloc(size_t size) {
	if (!g_block_list.tail) g_block_list.tail = &g_block_list.head;
	if (!g_ptr_list.tail) g_ptr_list.tail = &g_ptr_list.head;
	if (!g_ptr_free_list.tail) g_ptr_free_list.tail = &g_ptr_free_list.head;
	return NULL;
}

#endif
