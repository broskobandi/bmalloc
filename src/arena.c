#include "arena_utils.h"

arena_t *arena_new() {
	if (sizeof(arena_t) >= STORAGE_SIZE) ERR("STORAGE_SIZE is too small.", NULL);
	arena_t *arena = MMAP(STORAGE_SIZE);
	if (!arena) ERR("mmap() failed.", NULL);
	memset(arena, 0, sizeof(arena_t));
	arena->tail = &arena->head;
	arena->tail->ptr_list.tail = &arena->tail->ptr_list.head;
	arena->tail->buff = (unsigned char*)arena->tail + round_up(sizeof(arena_t));
	arena->size = STORAGE_SIZE - round_up(sizeof(arena_t));
	return arena;
}

void *arena_alloc(arena_t *arena, size_t size) {
	if (!arena || !size) ERR("Invalid argument.", NULL);
	void *ptr = NULL;
	if (arena->num_free_ptrs) {
		ptr = use_free_ptr(arena, size);
		if (!ptr) ERR("use_free_ptr() failed.", NULL);
	}
	if (total_size(size) > arena->size) {
		ptr = alloc_with_mmap(size);
		if (!ptr) ERR("alloc_with_mmap() failed.", NULL);
	} else {
		ptr = alloc_in_arena(arena, size);
		if (!ptr) ERR("alloc_in_arena() failed.", NULL);
	}
	return ptr;
}

void arena_dealloc(void *ptr) {
	if (!ptr) ERR("Invalid argument.");
	// get ptr
	// if heap do heap stuff
	// else do arena stuff
}

void arena_del(arena_t *arena) {
	if (!arena) ERR("Invalid argument.");
}
