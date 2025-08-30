#ifndef ARENA_UTILS_H
#define ARENA_UTILS_H

#include "arena.h"
#include <error.h>
#include <stdalign.h>
#include <unistd.h>
#include <sys/mman.h>

#define MMAP(size)\
	mmap(NULL, (size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)

struct arena {
	unsigned char *data;
	size_t size;
};

/** Rounds up the input value to a specified number.
 * \param value The value to be rounded up.
 * \param to The value to round up to (must be a power of 2).
 * \return The rounded up value. */
static inline size_t roundup(size_t value, size_t to) {
	return ((value + to - 1) & ~(to - 1));
}

#endif
