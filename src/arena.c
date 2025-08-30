#include "arena_utils.h"

arena_t *arena_new(size_t size) {
	if (!size) ERR("size cannot be 0.", NULL);
	size_t total_size =
		roundup(sizeof(arena_t), alignof(max_align_t)) +
		roundup(size, (size_t)getpagesize());
	arena_t *arena = (arena_t*)MMAP(size);
	if (!arena) ERR("Failed to allocate arena with mmap().", NULL);
	arena->data = (unsigned char*)arena + roundup(sizeof(arena_t), alignof(max_align_t));
	arena->size = total_size - roundup(sizeof(arena_t), alignof(max_align_t));
	return arena;
}

void arena_del(arena_t *arena) {
	if (!arena) ERR("Invalid argument.");
	if (munmap(arena, arena->size + roundup(sizeof(arena_t), alignof(max_align_t))) == -1)
		ERR("Failed to unmap memory for arena.");
}

void *arena_get_ptr(arena_t *arena, size_t index) {
	if (!arena) ERR("Invalid argument", NULL);
	if (index >= arena->size) ERR("index is too big.", NULL);
	return &arena->data[index];
}
