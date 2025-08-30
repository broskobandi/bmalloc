#include "test_arena.h"

TEST_INIT;

int main(void) {
	test_roundup();
	test_arena_new();

	test_print_results();
	return 0;
}
