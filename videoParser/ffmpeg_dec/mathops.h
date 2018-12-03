#pragma once
#include <stdint.h>

#define MAX_NEG_CROP 1024

extern const uint32_t ff_inverse[257];
extern const uint8_t ff_sqrt_tab[256];
extern const uint8_t ff_crop_tab[256 + 2 * MAX_NEG_CROP];
extern const uint8_t ff_zigzag_direct[64];
extern const uint8_t ff_zigzag_scan[16 + 1];
