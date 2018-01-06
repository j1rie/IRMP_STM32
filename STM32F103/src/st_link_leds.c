/*
 * Copyright (C) 2014-2017 Joerg Riechardt
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
	systicks = 0;
	int i;
	for(i=0; i<5; i++) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
#ifdef EXTLED_PORT
		EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
		while (systicks <= 50 * (2*i+1));
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
#ifdef EXTLED_PORT
		EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
		while (systicks <= 50 * (2*i+2));
	}
	restore();
}

/* yellow short on */
void yellow_short_on(void)
{
	if (!PA9_state)
		LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
	delay_ms(130);
	restore();
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}

/* red + yellow both on */
void both_on(void)
{
	if (!PA9_state)
		LED_init();
	systicks = 0;
	int i;
	for(i=0; i<250; i++) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		while (systicks <= 2*i+1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		while (systicks <= 2*i+2);
	}
	restore();
}
#else
void LED_deinit(void) {}
void fast_toggle(void)
{
	systicks = 0;
	int i;
	for(i=0; i<10; i++) {
		LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
		EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
		while (systicks <= 50 * (i+1));
	}
}
void both_on(void) {}
void red_on(void) {}
void yellow_short_on(void)
{
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
	delay_ms(130);
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}
#endif /* ST_Link */
