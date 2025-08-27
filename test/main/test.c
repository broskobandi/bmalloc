#include "test_bmalloc.h"

BTEST_INIT;

int main(void) {
	test_static_buff_alloc();
	test_sbrk_alloc();
	test_mmap_alloc();
	test_bmalloc();

	btest_print_results();
	return 0;
}
