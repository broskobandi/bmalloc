#include "test_arena.h"

BTEST_INIT;

int main(void) {
	test_arena_new();
	test_round_up();
	test_total_size();
	test_arena_expand();

	btest_print_results();
	return 0;
}
