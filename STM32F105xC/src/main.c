/*
 *  IR receiver, sender, USB wakeup, motherboard switch wakeup, wakeup timer,
 *  USB HID device, eeprom emulation
 *
 *  Copyright (C) 2014-2022 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "stm32f10x.h"
#include "usb_hid.h"
#include "irmpmain.h"
#include "eeprom.h"
#include "st_link_leds.h"
#include "config.h" /* CooCox workaround */

#define BYTES_PER_QUERY	(HID_IN_REPORT_COUNT - 4)
/* after plugging in, it takes some time, until SOF's are being sent to the device */
#define SOF_TIMEOUT 500

enum access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum command {
	CMD_EMIT,
	CMD_CAPS,
	CMD_HID_TEST,
	CMD_ALARM,
	CMD_MACRO,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_EEPROM_RESET
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

const char firmware[] = FW_STR;

const char supported_protocols[] = {
#if IRMP_SUPPORT_SIRCS_PROTOCOL==1
IRMP_SIRCS_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC_PROTOCOL==1
IRMP_NEC_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG_PROTOCOL==1
IRMP_SAMSUNG_PROTOCOL,
#endif
#if IRMP_SUPPORT_MATSUSHITA_PROTOCOL==1
IRMP_MATSUSHITA_PROTOCOL,
#endif
#if IRMP_SUPPORT_KASEIKYO_PROTOCOL==1
IRMP_KASEIKYO_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80_PROTOCOL==1
IRMP_RECS80_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC5_PROTOCOL==1
IRMP_RC5_PROTOCOL,
#endif
#if IRMP_SUPPORT_DENON_PROTOCOL==1
IRMP_DENON_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG_PROTOCOL==1
IRMP_SAMSUNG32_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC_PROTOCOL==1
IRMP_APPLE_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80EXT_PROTOCOL==1
IRMP_RECS80EXT_PROTOCOL,
#endif
#if IRMP_SUPPORT_NUBERT_PROTOCOL==1
IRMP_NUBERT_PROTOCOL,
#endif
#if IRMP_SUPPORT_BANG_OLUFSEN_PROTOCOL==1
IRMP_BANG_OLUFSEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_GRUNDIG_PROTOCOL==1
IRMP_GRUNDIG_PROTOCOL,
#endif
#if IRMP_SUPPORT_NOKIA_PROTOCOL==1
IRMP_NOKIA_PROTOCOL,
#endif
#if IRMP_SUPPORT_SIEMENS_PROTOCOL==1
IRMP_SIEMENS_PROTOCOL,
#endif
#if IRMP_SUPPORT_FDC_PROTOCOL==1
IRMP_FDC_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCCAR_PROTOCOL==1
IRMP_RCCAR_PROTOCOL,
#endif
#if IRMP_SUPPORT_JVC_PROTOCOL==1
IRMP_JVC_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6A_PROTOCOL,
#endif
#if IRMP_SUPPORT_NIKON_PROTOCOL==1
IRMP_NIKON_PROTOCOL,
#endif
#if IRMP_SUPPORT_RUWIDO_PROTOCOL==1
IRMP_RUWIDO_PROTOCOL,
#endif
#if IRMP_SUPPORT_IR60_PROTOCOL==1
IRMP_IR60_PROTOCOL,
#endif
#if IRMP_SUPPORT_KATHREIN_PROTOCOL==1
IRMP_KATHREIN_PROTOCOL,
#endif
#if IRMP_SUPPORT_NETBOX_PROTOCOL==1
IRMP_NETBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC16_PROTOCOL==1
IRMP_NEC16_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC42_PROTOCOL==1
IRMP_NEC42_PROTOCOL,
#endif
#if IRMP_SUPPORT_LEGO_PROTOCOL==1
IRMP_LEGO_PROTOCOL,
#endif
#if IRMP_SUPPORT_THOMSON_PROTOCOL==1
IRMP_THOMSON_PROTOCOL,
#endif
#if IRMP_SUPPORT_BOSE_PROTOCOL==1
IRMP_BOSE_PROTOCOL,
#endif
#if IRMP_SUPPORT_A1TVBOX_PROTOCOL==1
IRMP_A1TVBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_ORTEK_PROTOCOL==1
IRMP_ORTEK_PROTOCOL,
#endif
#if IRMP_SUPPORT_TELEFUNKEN_PROTOCOL==1
IRMP_TELEFUNKEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_ROOMBA_PROTOCOL==1
IRMP_ROOMBA_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCMM_PROTOCOL==1
IRMP_RCMM32_PROTOCOL,
IRMP_RCMM24_PROTOCOL,
IRMP_RCMM12_PROTOCOL,
#endif
#if IRMP_SUPPORT_SPEAKER_PROTOCOL==1
IRMP_SPEAKER_PROTOCOL,
#endif
#if IRMP_SUPPORT_LGAIR_PROTOCOL==1
IRMP_LGAIR_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG48_PROTOCOL==1
IRMP_SAMSUNG48_PROTOCOL,
#endif
#if IRMP_SUPPORT_MERLIN_PROTOCOL==1
IRMP_MERLIN_PROTOCOL,
#endif
#if IRMP_SUPPORT_PENTAX_PROTOCOL==1
IRMP_PENTAX_PROTOCOL,
#endif
#if IRMP_SUPPORT_FAN_PROTOCOL==1
IRMP_FAN_PROTOCOL,
#endif
#if IRMP_SUPPORT_S100_PROTOCOL==1
IRMP_S100_PROTOCOL,
#endif
#if IRMP_SUPPORT_ACP24_PROTOCOL==1
IRMP_ACP24_PROTOCOL,
#endif
#if IRMP_SUPPORT_TECHNICS_PROTOCOL==1
IRMP_TECHNICS_PROTOCOL,
#endif
#if IRMP_SUPPORT_PANASONIC_PROTOCOL==1
IRMP_PANASONIC_PROTOCOL,
#endif
#if IRMP_SUPPORT_MITSU_HEAVY_PROTOCOL==1
IRMP_MITSU_HEAVY_PROTOCOL,
#endif
#if IRMP_SUPPORT_VINCENT_PROTOCOL==1
IRMP_VINCENT_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNGAH_PROTOCOL==1
IRMP_SAMSUNGAH_PROTOCOL,
#endif
#if IRMP_SUPPORT_GREE_PROTOCOL==1
IRMP_GREE_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCII_PROTOCOL==1
IRMP_RCII_PROTOCOL,
#endif
#if IRMP_SUPPORT_METZ_PROTOCOL==1
IRMP_METZ_PROTOCOL,
#endif
#if IRMP_SUPPORT_ONKYO_PROTOCOL==1
IRMP_ONKYO_PROTOCOL,
#endif
#if IRMP_SUPPORT_MELINERA_PROTOCOL==1
IRMP_MELINERA_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6A20_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6A28_PROTOCOL,
#endif
0
};

uint32_t AlarmValue = 0xFFFFFFFF;
volatile unsigned int systicks = 0;
volatile unsigned int sof_timeout = 0;
volatile unsigned int i = 0;
uint8_t Reboot = 0;
volatile uint32_t boot_flag __attribute__((__section__(".noinit")));
volatile unsigned int send_ir_on_delay = 0;

void delay_ms(unsigned int msec)
{
	systicks = 0;
	while (systicks <= msec);
}

void LED_Switch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* start with wakeup and reset switch off */
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#ifdef RESET_PORT
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_SET);
#endif
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#ifdef RESET_PORT
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_RESET);
#endif
#endif /* SimpleCircuit */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
#ifdef EXTLED_PORT
	GPIO_InitStructure.GPIO_Pin = EXTLED_PIN;
	GPIO_Init(EXTLED_PORT, &GPIO_InitStructure);
#endif
	GPIO_InitStructure.GPIO_Pin = WAKEUP_PIN;
#ifdef SimpleCircuit
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
#endif /* SimpleCircuit */
	GPIO_Init(WAKEUP_PORT, &GPIO_InitStructure);
#ifdef RESET_PORT
	GPIO_InitStructure.GPIO_Pin = RESET_PIN;
	GPIO_Init(RESET_PORT, &GPIO_InitStructure);
#endif
	//GPIO_InitStructure.GPIO_Pin = WAKEUP_RESET_PIN;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_Init(WAKEUP_RESET_PORT, &GPIO_InitStructure);
	/* start with LED off */
	GPIO_WriteBit(LED_PORT, LED_PIN, Bit_RESET);
#ifdef EXTLED_PORT
	GPIO_WriteBit(EXTLED_PORT, EXTLED_PIN, Bit_RESET);
#endif
}

void blink_LED(void)
{
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
	delay_ms(25);
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}

/* buf[0 ... 5] -> eeprom[virt_addr ... virt_addr + 2] */
/* buffer: 012345 -> arguments for Write: (10)(32)(54) -> eeprom: 01,23,45 */
void eeprom_store(uint16_t virt_addr, uint8_t *buf)
{
	uint8_t i;
	for(i=0; i<3; i++) {
		EE_WriteVariable(virt_addr + i, *(uint16_t*)&buf[2*i]);
	}
}

/* eeprom[virt_addr ... virt_addr + 2] -> buf[0-5] */
/* eeprom: 01,23,45 -> Read results: (10)(32)(54) -> buffer: 012345 */
uint8_t eeprom_restore(uint8_t *buf, uint16_t virt_addr)
{
	uint8_t i, retVal = 0;
	for(i=0; i<3; i++) {
		if (EE_ReadVariable(virt_addr + i, (uint16_t *) &buf[2*i])) {
			/* the variable was not found or no valid page was found */
			*((uint16_t *) &buf[2*i]) = 0xFFFF;
			retVal = 1;
		}
	}
	return retVal;
}

void store_wakeup(IRMP_DATA *ir)
{
	uint16_t idx;
	uint8_t tmp[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS;
	eeprom_restore(tmp, idx);
	if (!memcmp(tmp, zeros, SIZEOF_IR)) {
		/* store received wakeup IRData in first wakeup slot */
		eeprom_store(idx, (uint8_t *) ir);
		fast_toggle();
	}
}

void Systick_Init(void)
{
	/* 1ms */
	SysTick_Config((SysCtlClockGet()/1000));
}

void SysTick_Handler(void)
{
	systicks++;
	if (sof_timeout != SOF_TIMEOUT)
		sof_timeout++;
	if (i == 999) {
		if (AlarmValue)
			AlarmValue--;
		if (send_ir_on_delay)
			send_ir_on_delay--;
		i = 0;
	} else {
		i++;
	}
}

uint8_t host_running(void)
{
	return (sof_timeout != SOF_TIMEOUT);
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	/* USB wakeup */
	USB_OTG_ActiveRemoteWakeup(&USB_OTG_dev);
	/* motherboard power switch: WAKEUP_PIN short high (resp. low in case of SimpleCircuit) */
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#endif /* SimpleCircuit */
	delay_ms(500);
#ifdef SimpleCircuit
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_SET);
#else
	GPIO_WriteBit(WAKEUP_PORT, WAKEUP_PIN, Bit_RESET);
#endif /* SimpleCircuit */
	fast_toggle();
	/* let software know, PC was powered on by firmware */
	send_ir_on_delay = 90;
}

void Reset(void)
{
#ifdef RESET_PORT
	/* motherboard reset switch: RESET_PIN short high (resp. low in case of SimpleCircuit) */
#ifdef SimpleCircuit
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_RESET);
#else
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_SET);
#endif /* SimpleCircuit */
	delay_ms(500);
#ifdef SimpleCircuit
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_SET);
#else
	GPIO_WriteBit(RESET_PORT, RESET_PIN, Bit_RESET);
#endif /* SimpleCircuit */
	fast_toggle();
#endif
}

void store_new_wakeup(void)
{
	int8_t loop;
	uint16_t idx;
	IRMP_DATA wakeup_IRData;
	irmp_get_data(&wakeup_IRData); // flush input of irmp data
	blink_LED();
	/* 5 seconds to press button on remote */
	for(loop=0; loop < 50; loop++) {
		delay_ms(100);
		if (irmp_get_data(&wakeup_IRData)) {
			wakeup_IRData.flags = 0;
			idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS;
			/* store received wakeup IRData in first wakeup slot */
			eeprom_store(idx, (uint8_t *) &wakeup_IRData);
			blink_LED();
			return;
		}
	}
}

void wakeup_reset(void)
{
#if (0)
	/* wakeup reset pin pulled low? */
	if (!GPIO_ReadInputDataBit(WAKEUP_RESET_PORT, WAKEUP_RESET_PIN)) {
		store_new_wakeup();
	}
#endif
}

int8_t get_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 4;
	uint16_t idx;
	switch (buf[3]) {
	case CMD_CAPS:
		/* in first query we give information about slots and depth */
		if (!buf[4]) {
			buf[4] = MACRO_SLOTS;
			buf[5] = MACRO_DEPTH;
			buf[6] = WAKE_SLOTS;
			buf[7] = HID_IN_REPORT_COUNT;
			buf[8] = HID_OUT_REPORT_COUNT;
			ret += 5;
			break;
		}
		/* in later queries we give information about supported protocols and firmware */
		idx = BYTES_PER_QUERY * (buf[4] - 1);
		if (idx < sizeof(supported_protocols)) {
			strncpy((char *) &buf[4], &supported_protocols[idx], BYTES_PER_QUERY);
			/* actually this is not true for the last transmission,
			 * but it doesn't matter since it's NULL terminated
			 */
			ret = HID_IN_REPORT_COUNT;
			break;
		}
		if (idx >= sizeof(firmware) + (sizeof(supported_protocols) / BYTES_PER_QUERY + 1) * BYTES_PER_QUERY)
			return -1;
		strncpy((char *) &buf[4], &firmware[idx - (sizeof(supported_protocols) / BYTES_PER_QUERY + 1) * BYTES_PER_QUERY], BYTES_PER_QUERY);
		ret = HID_IN_REPORT_COUNT;
		break;
	case CMD_ALARM:
		/* AlarmValue -> buf[3-6] */
		memcpy(&buf[4], &AlarmValue, sizeof(AlarmValue));
		ret += sizeof(AlarmValue);
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[4] + SIZEOF_IR/2 * buf[5];
		eeprom_restore(&buf[4], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[4];
		eeprom_restore(&buf[4], idx);
		ret += SIZEOF_IR;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t set_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 4;
	uint16_t idx;
	uint8_t tmp[SIZEOF_IR];
	switch (buf[3]) {
	case CMD_EMIT:
		yellow_short_on();
		irsnd_send_data((IRMP_DATA *) &buf[4], 1);
		break;
	case CMD_ALARM:
		memcpy(&AlarmValue, &buf[4], sizeof(AlarmValue));
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[4] + SIZEOF_IR/2 * buf[5];
		eeprom_store(idx, &buf[6]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[6], tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, &buf[5]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[5], tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_REBOOT:
		Reboot = 1;
		break;
	case CMD_HID_TEST:
		ret = HID_IN_REPORT_COUNT;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t reset_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 4;
	uint16_t idx;
	uint8_t tmp[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	switch (buf[3]) {
	case CMD_ALARM:
		AlarmValue = 0xFFFFFFFF;
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[4] + SIZEOF_IR/2 * buf[5];
		eeprom_store(idx, zeros);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(zeros, tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, zeros);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(zeros, tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_EEPROM_RESET:
		if(EE_Format() != FLASH_COMPLETE)
			ret = -1;
		break;
	default:
		ret = -1;
	}
	return ret;
}

/* is received ir-code in one of the lower wakeup-slots? wakeup if true */
void check_wakeups(IRMP_DATA *ir)
{
	if(host_running())
		return;
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (i=0; i < WAKE_SLOTS/2; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * i;
		if (!eeprom_restore(buf, idx)) {
			/* first encounter of zero in check_wakeups() means end of check */
			if (!memcmp(buf, &zeros, sizeof(zeros))) //
				break;
			if (!memcmp(buf, ir, sizeof(buf)))
				Wakeup();
		}
	}
}

/* is received ir-code in one of the upper wakeup-slots except last one? reset if true */
void check_resets(IRMP_DATA *ir)
{
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (i=WAKE_SLOTS/2; i < WAKE_SLOTS - 1; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * i;
		if (!eeprom_restore(buf, idx)) {
			/* first encounter of zero in check_resets() means end of check */
			if (!memcmp(buf, &zeros, sizeof(zeros))) //
				break;
			if (!memcmp(buf, ir, sizeof(buf)))
				Reset();
		}
	}
}

void reboot(void)
{
	boot_flag = 0x12094444;// let bootloader know reset is from here
	fast_toggle();
	NVIC_SystemReset();
}

/* is received ir-code in the last wakeup-slot? reboot µC if true */
void check_reboot(IRMP_DATA *ir)
{
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * (WAKE_SLOTS - 1);
	if (!eeprom_restore(buf, idx)) {
		if (!memcmp(buf, ir, sizeof(buf)))
			reboot();
	}
}

void transmit_macro(uint8_t macro)
{
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	/* we start from 1, since we don't want to tx the trigger code of the macro*/
	for (i=1; i < MACRO_DEPTH + 1; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * macro + SIZEOF_IR/2 * i;
		if (!eeprom_restore(buf, idx)) {
			/* first encounter of zero in macro means end of macro */
			if (!memcmp(buf, &zeros, sizeof(zeros)))
				break;
			/* if macros are sent already, while the trigger IR data are still repeated,
			* the receiving device may crash
			* Depending on the protocol we need a pause between the trigger and the transmission
			* and between two transmissions. The highest known pause is 130 ms for Denon. */
			yellow_short_on();
			irsnd_send_data((IRMP_DATA *) buf, 1);
		}
	}
}

/* is received ir-code (trigger) in one of the macro-slots? transmit_macro if true */
void check_macros(IRMP_DATA *ir)
{
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (i=0; i < MACRO_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * i;
		if (!eeprom_restore(buf, idx)) {
			/* first encounter of zero in check_macros() means end of check */
			if (!memcmp(buf, &zeros, sizeof(zeros))) //
				break;
		    if (!memcmp(buf, ir, sizeof(buf)))
			transmit_macro(i);
		}
	}
}

void USB_DISC_release(void)
{
#if defined(Bootloader) && defined(PullDown) || defined(Maple)
	/* bootloader must activate disconnect, here we release the disconnect */
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(USB_DISC_RCC_APB2Periph, ENABLE);
	GPIO_InitStructure.GPIO_Pin = USB_DISC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USB_DISC_PORT, &GPIO_InitStructure);
#if defined(Maple)
	GPIO_WriteBit(USB_DISC_PORT, USB_DISC_PIN, Bit_RESET);
#else
	GPIO_WriteBit(USB_DISC_PORT, USB_DISC_PIN, Bit_SET);
#endif
#endif
}

void USB_Reset(void)
{
#if defined(Bootloader) && !defined(PullDown) && !defined(Maple)
	/* reset USB */
	RCC_AHBPeriphResetCmd(RCC_AHBPeriph_OTG_FS, ENABLE);
	RCC_AHBPeriphResetCmd(RCC_AHBPeriph_OTG_FS, DISABLE);
	/* USB reset by pulling USBDP shortly low. A pullup resistor is needed, most
	 * boards have it. */
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
	delay_ms(15);
#endif
}

void led_callback (uint_fast8_t on)
{
	LED_PORT->ODR ^= LED_PIN;
#ifdef EXTLED_PORT
	EXTLED_PORT->ODR ^= EXTLED_PIN;
#endif
}

void send_magic(void)
{
	uint8_t magic[SIZEOF_IR] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
	USB_HID_SendData(REPORT_ID_IR, magic, SIZEOF_IR);
}

int main(void)
{
	IRMP_DATA myIRData;
	int8_t ret;
	uint8_t last_magic_sent = 0;

	LED_Switch_init();
	Systick_Init();
	USB_Reset();
	USB_HID_Init();
	USB_DISC_release();
	IRMP_Init();
	irsnd_init();
	FLASH_Unlock();
	EE_Init();
	irmp_set_callback_ptr (led_callback);

	while (1) {
		if (!AlarmValue && !host_running())
			Wakeup();

		/* always wait for previous transfer to complete before sending again, consider using a send buffer */
		if (PrevXferComplete && host_running() && send_ir_on_delay && last_magic_sent != send_ir_on_delay) {
			send_magic();
			last_magic_sent = send_ir_on_delay;
		}

		wakeup_reset();

		/* test if configuration command is received */
		if(PrevXferComplete && USB_HID_Data_Received && buf[0] == REPORT_ID_CONFIG_OUT && buf[1] == STAT_CMD) {
			USB_HID_Data_Received = 0;

			switch (buf[2]) {
			case ACC_GET:
				ret = get_handler(buf);
				break;
			case ACC_SET:
				ret = set_handler(buf);
				break;
			case ACC_RESET:
				ret = reset_handler(buf);
				break;
			default:
				ret = -1;
			}

			if (ret == -1) {
				buf[1] = STAT_FAILURE;
				ret = 4;
			} else {
				buf[1] = STAT_SUCCESS;
			}

			/* send configuration data */
			USB_HID_SendData(REPORT_ID_CONFIG_IN, buf, ret);
			blink_LED();
			if(Reboot)
				reboot();
		}

		/* poll IR-data */
		if (PrevXferComplete && irmp_get_data(&myIRData)) {
			myIRData.flags = myIRData.flags & IRMP_FLAG_REPETITION;
			if (!(myIRData.flags)) {
				store_wakeup(&myIRData);
				check_macros(&myIRData);
				check_wakeups(&myIRData);
				check_resets(&myIRData);
				check_reboot(&myIRData);
			}

			/* send IR-data, but only if host is running, otherwise the transfer will not complete, and we are stuck */
			if(host_running())
				USB_HID_SendData(REPORT_ID_IR, (uint8_t *) &myIRData, sizeof(myIRData));
		}
	}
}
