#include "test_arena.h"

BTEST_INIT;

int main(void) {
	test_arena_new();
	test_round_up();
	test_total_size();
	test_arena_expand();
	test_alloc_with_mmap();
	test_alloc_in_arena();
	test_use_free_ptr();

	btest_print_results();
	return 0;
}
