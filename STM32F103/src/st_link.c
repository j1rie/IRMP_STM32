/*
 * Copyright (C) 2014 Joerg Riechardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "st_link.h"
#include "stm32f10x.h"

#ifdef ST_Link
uint8_t PA9_state = 0;
void LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* PA9 (red + yellow LED on ST-Link Emus) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LED_deinit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* PA9 (red + yellow LED) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* red + yellow fast toggle */
void fast_toggle(void)
{
	LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(50);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(50);
	}
	/* off */
	LED_deinit();
	PA9_state = 0;
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
}

/* red + yellow both on */
void both_on(void)
{
	LED_init();
	systicks2 = 0;
	while (systicks2 <= 500) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		delay_ms(1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		delay_ms(1);
	}
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
	/* off */
	LED_deinit();
}

/* yellow on */
void yellow_on(void)
{
	LED_init();
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
	delay_ms(100);
	/* red on */
	/*GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);*/
	/* off */
	LED_deinit();
}
#else
void LED_init(void) {}
void LED_deinit(void) {}
void fast_toggle(void) {}
void both_on(void) {}
void yellow_on(void) {}
#endif /* ST_Link */

/* toggle red [/ yellow] and external LED */
void toggle_LED(void)
{
#ifdef ST_Link
	if (!PA9_state) {
		LED_init();
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		PA9_state = 1;
	} else {
		LED_deinit();
		PA9_state = 0;
	}
	/*GPIOA->ODR ^= GPIO_Pin_9;*/
#endif /* ST_Link */
	OUT_PORT->ODR ^= LED_PIN;
}
