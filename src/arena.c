#include "arena_utils.h"

arena_t *arena_new(size_t size, size_t unit) {
	if (!size || !unit) ERR("Invalid argument.", NULL);
	if (unit & (unit - 1)) ERR("unit must be a power of 2.", NULL);
	size_t total_size =
		rounduptopage((rounduptoalign(sizeof(arena_t)) + size  * unit));
	arena_t *arena = MMAP(total_size);
	if (!arena) ERR("Failed to allocate arena with mmap().", NULL);
	arena->storage.buff = (unsigned char*)arena + rounduptoalign(sizeof(arena_t));
	arena->unit_size = unit;
	arena->buff_size = total_size - rounduptoalign(sizeof(arena_t));
	return arena;
}

void *arena_alloc(arena_t *arena, size_t unit, size_t len) {
	if (!arena || !unit || !len) ERR("Invalid argument.", NULL);
	if (unit & (unit - 1)) ERR("unit must be a power of 2.", NULL);
	if (unit != arena->unit_size) ERR("unit sizes don't match.", NULL);
	void *ptr = NULL;
	return ptr;
}

void arena_dealloc(void *ptr) {
	if (!ptr) ERR("Invalid argument.");
}

void arena_del(arena_t *arena) {
	if (!arena) ERR("Invalid argument.");
}

void *arena_realloc(arena_t *arena, void *ptr, size_t len, size_t unit) {
	if (!arena || !ptr || !len || !unit) ERR("Invalid argument.", NULL);
	if (unit & (unit - 1)) ERR("unit must be a power of 2.", NULL);
	if (unit != arena->unit_size) ERR("unit sizes don't match.", NULL);
	return NULL;
}
