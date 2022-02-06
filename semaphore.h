
#include "stdatomic.h"
#include "stdbool.h"
#include "stdint.h"

typedef atomic_flag semaphore_t;

void P(semaphore_t * s);

void V(semaphore_t * s);

void PHW(volatile uint32_t * const s, int c);

void VHW(volatile uint32_t * const s, int c);
