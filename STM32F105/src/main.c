/*
 *  IR receiver, sender, USB wakeup, motherboard switch wakeup, wakeup timer,
 *  USB HID device, eeprom emulation
 *
 *  Copyright (C) 2014 Joerg Riechardt
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
#include "config.h" /* CooCox workaround */

#define BYTES_PER_QUERY	(HID_IN_BUFFER_SIZE - 4)

enum __attribute__ ((__packed__)) access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum __attribute__ ((__packed__)) command {
	CMD_EMIT,
	CMD_CAPS,
	CMD_FW,
	CMD_ALARM,
	CMD_MACRO,
	CMD_WAKE
};

enum __attribute__ ((__packed__)) status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

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
#if IRMP_SUPPORT_KASEIKYO_PROTOCOL==1
IRMP_KASEIKYO_PROTOCOL,
#endif
#if IRMP_SUPPORT_JVC_PROTOCOL==1
IRMP_JVC_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC16_PROTOCOL==1
IRMP_NEC16_PROTOCOL,
#endif
#if IRMP_SUPPORT_NEC42_PROTOCOL==1
IRMP_NEC42_PROTOCOL,
#endif
#if IRMP_SUPPORT_MATSUSHITA_PROTOCOL==1
IRMP_MATSUSHITA_PROTOCOL,
#endif
#if IRMP_SUPPORT_DENON_PROTOCOL==1
IRMP_DENON_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC5_PROTOCOL==1
IRMP_RC5_PROTOCOL,
#endif
#if IRMP_SUPPORT_RC6_PROTOCOL==1
IRMP_RC6_PROTOCOL,
IRMP_RC6A_PROTOCOL,
#endif
#if IRMP_SUPPORT_IR60_PROTOCOL==1
IRMP_IR60_PROTOCOL,
#endif
#if IRMP_SUPPORT_GRUNDIG_PROTOCOL==1
IRMP_GRUNDIG_PROTOCOL,
#endif
#if IRMP_SUPPORT_SIEMENS_PROTOCOL==1
IRMP_SIEMENS_PROTOCOL,
#endif
#if IRMP_SUPPORT_NOKIA_PROTOCOL==1
IRMP_NOKIA_PROTOCOL,
#endif
#if IRMP_SUPPORT_BOSE_PROTOCOL==1
IRMP_BOSE_PROTOCOL,
#endif
#if IRMP_KATHREIN_PROTOCOL==1
IRMP_KATHREIN_PROTOCOL,
#endif
#if IRMP_SUPPORT_NUBERT_PROTOCOL==1
IRMP_NUBERT_PROTOCOL,
#endif
#if IRMP_SUPPORT_SPEAKER_PROTOCOL==1
IRMP_SPEAKER_PROTOCOL,
#endif
#if IRMP_SUPPORT_BANG_OLUFSEN_PROTOCOL==1
IRMP_BANG_OLUFSEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80_PROTOCOL==1
IRMP_RECS80_PROTOCOL,
#endif
#if IRMP_SUPPORT_RECS80EXT_PROTOCOL==1
IRMP_RECS80EXT_PROTOCOL,
#endif
#if IRMP_SUPPORT_THOMSON_PROTOCOL==1
IRMP_THOMSON_PROTOCOL,
#endif
#if IRMP_SUPPORT_NIKON_PROTOCOL==1
IRMP_NIKON_PROTOCOL,
#endif
#if IRMP_SUPPORT_NETBOX_PROTOCOL==1
IRMP_NETBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_ORTEK_PROTOCOL==1
IRMP_ORTEK_PROTOCOL,
#endif
#if IRMP_SUPPORT_TELEFUNKEN_PROTOCOL==1
IRMP_TELEFUNKEN_PROTOCOL,
#endif
#if IRMP_SUPPORT_FDC_PROTOCOL==1
IRMP_FDC_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCCAR_PROTOCOL==1
IRMP_RCCAR_PROTOCOL,
#endif
#if IRMP_SUPPORT_ROOMBA_PROTOCOL==1
IRMP_ROOMBA_PROTOCOL,
#endif
#if IRMP_SUPPORT_RUWIDO_PROTOCOL==1
IRMP_RUWIDO_PROTOCOL,
#endif
#if IRMP_SUPPORT_A1TVBOX_PROTOCOL==1
IRMP_A1TVBOX_PROTOCOL,
#endif
#if IRMP_SUPPORT_LEGO_PROTOCOL==1
IRMP_LEGO_PROTOCOL,
#endif
#if IRMP_SUPPORT_RCMM_PROTOCOL==1
IRMP_RCMM_PROTOCOL,
#endif
#if IRMP_SUPPORT_LGAIR_PROTOCOL==1
IRMP_LGAIR_PROTOCOL,
#endif
#if IRMP_SUPPORT_SAMSUNG48_PROTOCOL==1
IRMP_SAMSUNG48_PROTOCOL,
#endif
#if IRMP_SUPPORT_RADIO1_PROTOCOL==1
IRMP_RADIO1_PROTOCOL,
#endif
0
};

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
uint32_t AlarmValue = 0xFFFFFFFF;
volatile unsigned int systicks = 0;

void delay_ms(unsigned int msec)
{
	systicks = 0;
	while (systicks <= msec);
}

void LED_Switch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(OUT_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = WAKEUP_PIN;
	GPIO_Init(OUT_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = WAKEUP_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(RESET_PORT, &GPIO_InitStructure);
	/* start with LED on */
	GPIO_WriteBit(OUT_PORT, LED_PIN, Bit_SET);
}

void toggle_LED(void)
{
	OUT_PORT->ODR ^= LED_PIN;
}

/* buf[0 ... 5] -> eeprom[virt_addr ... virt_addr + 2] */
/* buffer: 012345 -> arguments for Write: (10)(32)(54) -> eeprom: 01,23,45 */
void eeprom_store(uint8_t virt_addr, uint8_t *buf)
{
	EE_WriteVariable(virt_addr, (buf[1] << 8) | buf[0]);
	EE_WriteVariable(virt_addr + 1, (buf[3] << 8) | buf[2]);
	EE_WriteVariable(virt_addr + 2, (buf[5] << 8) | buf[4]);
}

/* eeprom[virt_addr ... virt_addr + 2] -> buf[0-5] */
/* eeprom: 01,23,45 -> Read results: (10)(32)(54) -> memcpy 01|23|45 -> buffer: 012345 */
uint8_t eeprom_restore(uint8_t *buf, uint8_t virt_addr)
{
	uint8_t i, retVal = 0;
	uint16_t EE_Data;
	for(i=0; i<3; i++) {
		if (EE_ReadVariable(virt_addr + i, &EE_Data)) {
			/* the variable was not found or no valid page was found */
			EE_Data = 0xFFFF;
			retVal = 1;
		}
		memcpy(&buf[2*i], &EE_Data, 2);
	}
	return retVal;
}

void Systick_Init(void)
{
	/* 1ms */
	SysTick_Config((SysCtlClockGet()/1000));
}

void SysTick_Handler(void)
{
	static uint_fast16_t i = 0;
	systicks++;
	if (i == 1000) {
		if (AlarmValue)
			AlarmValue--;
		i = 0;
	} else {
		i++;
	}
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	/* USB wakeup */
	USB_OTG_ActiveRemoteWakeup(&USB_OTG_dev);
	/* motherboard switch: WAKEUP_PIN short high */
	GPIO_WriteBit(OUT_PORT, WAKEUP_PIN, Bit_SET);
	delay_ms(500);
	GPIO_WriteBit(OUT_PORT, WAKEUP_PIN, Bit_RESET);
}

void store_new_wakeup(void)
{
	uint8_t idx;
	IRMP_DATA wakeup_IRData;
	toggle_LED();
	/* 5 seconds to press button on remote */
	delay_ms(5000);
	if (irmp_get_data(&wakeup_IRData)) {
		wakeup_IRData.flags = 0;
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS;
		/* store received wakeup IRData in first wakeup slot */
		eeprom_store(idx, (uint8_t *) &wakeup_IRData);
		toggle_LED();
	}
}

void wakeup_reset(void)
{
	/* wakeup reset pin pulled low? */
	if (!GPIO_ReadInputDataBit(RESET_PORT, WAKEUP_RESET_PIN)) {
		store_new_wakeup();
	}
}

int8_t get_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	switch ((enum command) buf[2]) {
	case CMD_CAPS:
		/* in first query we give information about slots and depth */
		if (!buf[3]) {
			buf[3] = MACRO_SLOTS;
			buf[4] = MACRO_DEPTH;
			buf[5] = WAKE_SLOTS;
			ret += 3;
			break;
		}
		/* in later queries we give information about supported protocols */
		idx = BYTES_PER_QUERY * (buf[3] - 1);
		if (idx >= sizeof(supported_protocols))
			return -1;
		strncpy((char *) &buf[3], &supported_protocols[idx], BYTES_PER_QUERY);
		/* actually this is not true for the last transmission,
		 * but it doesn't matter since it's NULL terminated
		 */
		ret = HID_IN_BUFFER_SIZE-1;
		break;
	case CMD_ALARM:
		/* AlarmValue -> buf[3-6] */
		memcpy(&buf[3], &AlarmValue, sizeof(AlarmValue));
		ret += sizeof(AlarmValue);
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_restore(&buf[3], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_restore(&buf[3], idx);
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
	int8_t ret = 3;
	uint8_t idx;
	uint8_t tmp[SIZEOF_IR];
	switch ((enum command) buf[2]) {
	case CMD_EMIT:
		delay_ms(130);
		irsnd_send_data((IRMP_DATA *) &buf[3], 1);
		break;
	case CMD_ALARM:
		memcpy(&AlarmValue, &buf[3], sizeof(AlarmValue));
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, &buf[5]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[5], tmp, sizeof(tmp)))
			ret = -1;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, &buf[4]);
		/* validate stored value in eeprom */
		eeprom_restore(tmp, idx);
		if (memcmp(&buf[4], tmp, sizeof(tmp)))
			ret = -1;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int8_t reset_handler(uint8_t *buf)
{
	/* number of valid bytes in buf, -1 signifies error */
	int8_t ret = 3;
	uint8_t idx;
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	switch ((enum command) buf[2]) {
	case CMD_ALARM:
		AlarmValue = 0xFFFFFFFF;
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * buf[3] + SIZEOF_IR/2 * buf[4];
		eeprom_store(idx, zeros);
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * buf[3];
		eeprom_store(idx, zeros);
		break;
	default:
		ret = -1;
	}
	return ret;
}

/* is received ir-code in one of the wakeup-slots? wakeup if true */
void check_wakeups(IRMP_DATA *ir)
{
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < WAKE_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS + SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		if (!memcmp(buf, ir, sizeof(ir)))
			Wakeup();
	}
}

void transmit_macro(uint8_t macro)
{
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0};
	/* we start from 1, since we don't want to tx the trigger code of the macro*/
	for (i=1; i < MACRO_DEPTH + 1; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * macro + SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		/* first encounter of zero in macro means end of macro */
		if (!memcmp(buf, &zeros, sizeof(zeros)))
			break;
		/* Depending on the protocol we need a pause between the trigger and the transmission
		 * and between two transmissions. The highest known pause is 130 ms for Denon. */
		delay_ms(130);
		irsnd_send_data((IRMP_DATA *) buf, 1);
	}
}

/* is received ir-code (trigger) in one of the macro-slots? transmit_macro if true */
void check_macros(IRMP_DATA *ir)
{
	uint8_t i, idx;
	uint8_t buf[SIZEOF_IR];
	for (i=0; i < MACRO_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR/2 * i;
		eeprom_restore(buf, idx);
		if (!memcmp(buf, ir, sizeof(ir)))
			transmit_macro(i);
	}
}

int main(void)
{
	uint8_t buf[HID_OUT_BUFFER_SIZE-1], RepeatCounter = 0;
	IRMP_DATA myIRData;
	int8_t ret;
	/* first wakeup slot empty? */
	uint8_t learn_wakeup = eeprom_restore(buf, (MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS);

	USB_HID_Init();
	LED_Switch_init();
	IRMP_Init();
	irsnd_init();
	FLASH_Unlock();
	EE_Init();
	Systick_Init();

	while (1) {
		if (!AlarmValue)
			Wakeup();

		wakeup_reset();

		/* test if USB is connected to PC and command is received */
		if (USB_HID_GetStatus() == USB_HID_CONNECTED && USB_HID_ReceiveData(buf) == RX_READY && buf[0] == STAT_CMD) {

			switch ((enum access) buf[1]) {
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
				buf[0] = STAT_FAILURE;
				ret = 3;
			} else {
				buf[0] = STAT_SUCCESS;
			}

			/* send configuration data */
			USB_HID_SendData(REPORT_ID_CONFIG, buf, ret);
			toggle_LED();
		}

		/* poll IR-data */
		if (irmp_get_data(&myIRData)) {
			if (learn_wakeup) {
				/* store received wakeup IRData in first wakeup slot */
				eeprom_store((MACRO_DEPTH + 1) * SIZEOF_IR/2 * MACRO_SLOTS, (uint8_t *) &myIRData);
				learn_wakeup = 0;
			}

			if (!(myIRData.flags)) {
				RepeatCounter = 0;
			} else {
				RepeatCounter++;
			}

			if (RepeatCounter == 0 || RepeatCounter >= MIN_REPEATS) {
				toggle_LED();
				/* if macros are sent already, while the trigger IR data are still repeated,
				 * the receiving device may crash */
				check_macros(&myIRData);
				check_wakeups(&myIRData);
			}

			/* send IR-data */
			memcpy(buf, &myIRData, sizeof(myIRData));
			USB_HID_SendData(REPORT_ID_IR, buf, sizeof(myIRData));
		}
	}
}
