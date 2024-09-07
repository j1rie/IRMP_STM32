/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ws2812.pio.h"
inline void put_pixel(uint8_t red, uint8_t green, uint8_t blue) {
    pio_sm_put_blocking(pio0, 0, (uint32_t)(green << 24 | red << 16 | blue << 8));
}
void ws2812_init();
