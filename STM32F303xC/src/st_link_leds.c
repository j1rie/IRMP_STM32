/*
 * Copyright (C) 2014-2024 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "st_link_leds.h"
#include "stm32f30x.h"
#include "config.h" /* CooCox workaround */

#ifdef ST_Link
/* red + {red|yellow} LEDs on PA9 on ST-Link Emus) */

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
}

/* red on */
void red_on(void)
{
	LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
}

/* red + yellow fast toggle */
void fast_toggle(void)
{
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
	LED_deinit();
}

/* yellow short on */
void yellow_short_on(void)
{
	LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
	delay_ms(130);
	LED_deinit();
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}

/* red + yellow both on */
void both_on(void)
{
	LED_init();
	systicks = 0;
	int i;
	for(i=0; i<250; i++) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		while (systicks <= 2*i+1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		while (systicks <= 2*i+2);
	}
	LED_deinit();
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
void statusled_write (uint8_t led_state) {
#ifdef STATUSLED_PORT
		GPIO_WriteBit(STATUSLED_PORT, STATUSLED_PIN, led_state? Bit_SET : Bit_RESET);
#endif
}
#endif /* ST_Link */
