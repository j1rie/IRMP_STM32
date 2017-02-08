/*
 * Copyright (C) 2014-2015 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "st_link_leds.h"
#include "stm32f10x.h"
#include "config.h" /* CooCox workaround */

#ifdef ST_Link
/* red + {red|yellow} LEDs on PA9 on ST-Link Emus) */
uint8_t PA9_state = 0;

void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	PA9_state = 0;
}

/* red on */
void red_on(void)
{
	LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
	PA9_state = 1;
}

void restore(void)
{
	if (!PA9_state) {
		LED_deinit();
	} else {
		red_on();
	}
}

/* red + yellow fast toggle */
void fast_toggle(void)
{
	if (!PA9_state)
		LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(50);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(50);
	}
	restore();
}

/* yellow short on */
void yellow_short_on(void)
{
	if (!PA9_state)
		LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
	delay_ms(130);
	restore();
}

/* red + yellow both on */
void both_on(void)
{
	if (!PA9_state)
		LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(1);
	}
	restore();
}
#else
void LED_deinit(void) {}
void fast_toggle(void)
{
	systicks2 = 0;
	while (systicks2 <= 500) {
		LED_PORT->ODR ^= LED_PIN;
		delay_ms(50);  // ?!
	}
}
void both_on(void) {}
void red_on(void) {}
void yellow_short_on(void) {delay_ms(130);}
#endif /* ST_Link */
