#ifndef BSC_H
#define BSC_H

#include <stddef.h>
#include <stdint.h>
#include "ARQ.h"

void bsc(void *Frame, size_t frame_size, double ber);

#endif
