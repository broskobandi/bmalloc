#include "test_arena.h"

TEST_INIT;

int main(void) {
	test_arena_new();
	test_arena_del();
	test_arena_get_ptr();

	test_print_results();
	return 0;
}
