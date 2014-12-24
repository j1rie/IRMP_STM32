/*
 * Copyright (C) 2014 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "st_link_leds.h"
#include "stm32f10x.h"
#include "config.h" /* CooCox workaround */

#ifdef ST_Link_LEDs
/* red + {red|yellow} LEDs on PA9 on ST-Link Emus) */
uint8_t PA9_state = 0;

void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	PA9_state = 0;
}

/* red + yellow fast toggle */
void fast_toggle(void)
{
#if 0 // causes strange problem, TODO find out why, related to check_wakeups, Wakeup
	if (!PA9_state)
		LED_init();
	//systicks2 = 0;
	//while (systicks2 <= 500) { //
	uint8_t k;
	for (k=0; k<5; k++) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(50);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(50);
	}
	LED_deinit();
#endif
}

/* red + yellow both on */
void both_on(void)
{
	if (!PA9_state)
		LED_init();
	//systicks2 = 0;
	//while (systicks2 <= 500) { //
	uint8_t k;
	for (k=0; k<200; k++) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(1);
	}
	LED_deinit();
}

/* red on */
void red_on(void)
{

	LED_init();

	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
	PA9_state = 1;
}

/* yellow short on */
void yellow_short_on(void)
{
#if 0 // causes strange problem, TODO find out why, related to check_macros, transmit_macro
	if (!PA9_state)
		LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
	delay_ms(130);
	LED_deinit();
#endif
	delay_ms(130);
}
#else
void fast_toggle(void) {}
void both_on(void) {}
void red_on(void) {}
void yellow_short_on(void) {delay_ms(130);}
#endif /* ST_Link_LEDs */

/* toggle red and external LED */
void toggle_LED(void)
{
#ifdef ST_Link_LEDs
	if (!PA9_state) {
		red_on();
	} else {
		LED_deinit();
	}
#endif /* ST_Link_LEDs */
	OUT_PORT->ODR ^= LED_PIN; // TODO make more readable
}
