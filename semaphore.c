
#include "semaphore.h"

void P(semaphore_t * s) {
	while (true == atomic_flag_test_and_set(s)) {
	}
}

void V(semaphore_t * s) {
	atomic_flag_clear(s);
}

void PHW(volatile uint32_t * const s, int c) {
	do {
		*s = (c<<1) | 1;				
		// printf_sync("%s sem %d\n", id, *SEM0);
	} while (*s != ((c<<1) | 1));
}

void VHW(volatile uint32_t * const s, int c) {
	*s = (c<<1);				
}
