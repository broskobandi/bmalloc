#include "bmalloc_utils.h"
#include <string.h>

_Thread_local storage_list_t g_storage_list = {0};
_Thread_local ptr_list_t g_ptr_list = {0};
_Thread_local ptr_list_t g_ptr_free_list = {0};
_Thread_local size_t g_num_free_ptrs = 0;

void *bmalloc(size_t size) {
	if (!size) ERR("size must not be 0.", NULL);
	void *ptr = NULL;
	if (g_num_free_ptrs)
		ptr = get_free_ptr(size);
	if (ptr) return ptr;
	ptr = mmap_alloc(size);
	if (!ptr) ERR("Failed to find appropriate allocation method.", NULL);
	return ptr;
}

void bfree(void *ptr) {
	if (!ptr) ERR("Invalid argument.");
	char *char_ptr = (char*)ptr;
	ptr_t *ptr_meta = (ptr_t*)(char_ptr - PTR_ALIGNED_SIZE);
	if (ptr_meta->size > BUFF_SIZE) {
		if (munmap(ptr_meta->data, ptr_meta->size) == -1)
			ERR("munmap() failed.");
		if (munmap(ptr_meta, sizeof(ptr_t)) == -1)
			ERR("munmap() failed.");
		return;
	}
	ptr_meta->is_free = true;
	// storage_t *storage = ptr_meta->storage;
	if (storage->next && storage->prev) {
		storage->next->prev = storage->prev;
		storage->prev->next = storage->next;
	}
	if (!storage->next && storage->prev)
		storage->prev->next = NULL;
	munmap(storage, sizeof(storage_t));
}

void *brealloc(void *ptr, size_t size) {
	if (!ptr || !size) ERR("Invalid argument.", NULL);
	void *new_data = bmalloc(size);
	if (!new_data) ERR("bmalloc() failed.", NULL);
	char *char_ptr = (char*)ptr;
	ptr_t *ptr_meta = (ptr_t*)(char_ptr - PTR_ALIGNED_SIZE);
	size_t size_to_copy = ptr_meta->size > size ? size : ptr_meta->size;
	memcpy(new_data, ptr_meta->data, size_to_copy);
	bfree(ptr);
	return new_data;
}
