#ifndef TEST_ARENA_H
#define TEST_ARENA_H

#include <btest.h>

void test_arena_new();
void test_arena_expand();
void test_round_up();
void test_total_size();
void test_alloc_with_mmap();
void test_alloc_in_arena();
void test_use_free_ptr();

#endif
