#ifndef ARENA_UTILS_H
#define ARENA_UTILS_H

#include "arena.h"
#include <stdbool.h>
#include <error.h>
#include <unistd.h>
#include <stdalign.h>
#include <sys/mman.h>

#define MMAP(size)\
	mmap(NULL, (size), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)

typedef struct ptr ptr_t;
typedef struct free_ptr free_ptr_t;
typedef struct storage storage_t;

struct ptr {
	void *data;
	storage_t *storage;
	size_t size;
	ptr_t *next;
	ptr_t *prev;
	bool is_valid;
};

struct free_ptr {
	ptr_t *ptr;
	free_ptr_t *next;
	free_ptr_t *prev;
};

struct storage {
	unsigned char *buff;
	storage_t *next;
	storage_t *prev;
	ptr_t *ptrs;
};

struct arena {
	storage_t storage;
	ptr_t *free_ptrs;
	size_t buff_size;
	size_t unit_size;
};

static inline size_t rounduptopage(size_t value) {
	return ((value + (size_t)getpagesize() - 1) & ~((size_t)getpagesize() - 1));
}

static inline size_t rounduptoalign(size_t value) {
	return ((value + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1));
}

#endif
