#include "arena_utils.h"

arena_t *arena_new() {
	arena_t *arena = MMAP(sizeof(arena_t));
	if (!arena) ERR("Failed to allocate arena.", NULL);
	memset(arena, 0, sizeof(arena_t));
	arena->tail = &arena->head;
	arena->tail->ptr_list.tail = &arena->tail->ptr_list.head;
	return arena;
}

void *arena_alloc(arena_t *arena, size_t size) {
	if (!arena || !size) ERR("Invalid argument.", NULL);
	void *ptr = NULL;
	return ptr;
}

void arena_dealloc(void *ptr) {
	if (!ptr) ERR("Invalid argument.");
}

void arena_del(arena_t *arena) {
	if (!arena) ERR("Invalid argument.");
}
