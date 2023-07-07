/*
 *  IR receiver, sender, USB wakeup, motherboard switch wakeup, wakeup timer,
 *  USB HID device, eeprom emulation
 *
 *  Copyright (C) 2014-2023 Joerg Riechardt
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bsp/board.h"
#include "tusb.h"
#include "irmpmain.h"
#include "eeprom.h"
#include "usb_hid.h"
#include <hardware/flash.h>
#include <hardware/exception.h>
#include <hardware/structs/systick.h>
#include "timestamp.h"
#include "pico/bootrom.h"

#define BYTES_PER_QUERY	(HID_IN_REPORT_COUNT - 4)

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
	CMD_EEPROM_RESET,
	CMD_EEPROM_COMMIT,
	CMD_EEPROM_GET_RAW
};

enum status {
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
volatile unsigned int i = 0;
uint8_t Reboot = 0;
//volatile uint32_t boot_flag __attribute__((__section__(".noinit")));
volatile unsigned int send_ir_on_delay = 0;
static bool led_state = false;

void LED_Switch_init(void)
{
	/* start with wakeup switch off */
	gpio_init(WAKEUP_GPIO);
	gpio_init(EXTLED_GPIO);
	gpio_set_drive_strength(EXTLED_GPIO, GPIO_DRIVE_STRENGTH_12MA);
	//gpio_set_drive_strength(WAKEUP_GPIO, GPIO_DRIVE_STRENGTH_12MA); // TODO: once enough?!
	gpio_set_dir(WAKEUP_GPIO, GPIO_IN); // no open drain on RP2040
	gpio_set_dir(EXTLED_GPIO, GPIO_OUT);
}

void toggle_led(void)
{
	led_state = 1 - led_state;
	board_led_write(led_state);
	gpio_put(EXTLED_GPIO, led_state);
}

void blink_LED(void)
{
	toggle_led();
	sleep_ms(25);
	toggle_led();
}

void fast_toggle(void)
{
	int i;
	for(i=0; i<10; i++) {
		toggle_led();
		sleep_ms(50); 
	}
}

void yellow_short_on(void)
{
	toggle_led();
	sleep_ms(130);
	toggle_led();
}

void eeprom_store(int addr, uint8_t *buf)
{
	uint8_t i;
	for(i=0; i<6; i++) {
		eeprom_write(addr + i, buf[i]);
	}
}

void eeprom_restore(uint8_t *buf, int addr)
{
	uint8_t i, retVal = 0;
	for(i=0; i<6; i++) {
		buf[i] = eeprom_read(addr + i);
	}
}

void store_wakeup(IRMP_DATA *ir)
{
	uint16_t idx;
	uint8_t tmp[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS;
	eeprom_restore(tmp, idx);
	if (!memcmp(tmp, zeros, SIZEOF_IR)) {
		/* store received wakeup IRData in first wakeup slot */
		eeprom_store(idx, (uint8_t *) ir);
		if (eeprom_commit())
			fast_toggle();
	}
}

void SysTick_Handler(void)
{
	systicks++;
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

void Systick_Init(void)
{
	exception_set_exclusive_handler(SYSTICK_EXCEPTION, SysTick_Handler);
	systick_hw->csr = M0PLUS_SYST_CSR_ENABLE_BITS | M0PLUS_SYST_CSR_TICKINT_BITS | M0PLUS_SYST_CSR_CLKSOURCE_BITS;
	/* 1ms */
	systick_hw->rvr = 124999UL;
}

void Wakeup(void)
{
	AlarmValue = 0xFFFFFFFF;
	/* USB wakeup */
	tud_remote_wakeup();
	/* motherboard power switch: WAKEUP_PIN short low (SimpleCircuit) */
	gpio_set_drive_strength(WAKEUP_GPIO, GPIO_DRIVE_STRENGTH_12MA); // TODO: once enough?!
	gpio_set_dir(WAKEUP_GPIO, GPIO_OUT);
	gpio_put(WAKEUP_GPIO, 0);
	sleep_ms(500);
	gpio_set_dir(WAKEUP_GPIO, GPIO_IN);
	fast_toggle();
	/* let software know, PC was powered on by firmware */
	send_ir_on_delay = 90;
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
		sleep_ms(100);
		if (irmp_get_data(&wakeup_IRData)) {
			wakeup_IRData.flags = 0;
			idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS;
			/* store received wakeup IRData in first wakeup slot */
			eeprom_store(idx, (uint8_t *) &wakeup_IRData);
			blink_LED();
			return;
		}
	}
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
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * buf[4] + SIZEOF_IR * buf[5];
		eeprom_restore(&buf[4], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS + SIZEOF_IR * buf[4];
		eeprom_restore(&buf[4], idx);
		ret += SIZEOF_IR;
		break;
	case CMD_EEPROM_GET_RAW:
		eeprom_get_raw(&buf[4], buf[4], buf[5]);
		ret += 32;
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
	switch (buf[3]) {
	case CMD_EMIT:
		yellow_short_on();
		irsnd_send_data((IRMP_DATA *) &buf[4], 1);
		break;
	case CMD_ALARM:
		memcpy(&AlarmValue, &buf[4], sizeof(AlarmValue));
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * buf[4] + SIZEOF_IR * buf[5];
		eeprom_store(idx, &buf[6]);
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS + SIZEOF_IR * buf[4];
		eeprom_store(idx, &buf[5]);
		if (!buf[4]){
			if(!eeprom_commit())
				ret = -1;
		}
		break;
	case CMD_REBOOT:
		Reboot = 1;
		break;
	case CMD_HID_TEST:
		ret = HID_IN_REPORT_COUNT;
		break;
	case CMD_EEPROM_COMMIT:
		if(!eeprom_commit())
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
	int8_t ret = 4;
	uint16_t idx;
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	switch (buf[3]) {
	case CMD_ALARM:
		AlarmValue = 0xFFFFFFFF;
		break;
	case CMD_MACRO:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * buf[4] + SIZEOF_IR * buf[5];
		eeprom_store(idx, zeros);
		break;
	case CMD_WAKE:
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS + SIZEOF_IR * buf[4];
		eeprom_store(idx, zeros);
		break;
	case CMD_EEPROM_RESET:
		eeprom_reset();
		break;
	default:
		ret = -1;
	}
	return ret;
}

/* is received ir-code in one of the lower wakeup-slots? wakeup if true */
void check_wakeups(IRMP_DATA *ir)
{
	if(tud_ready())
		return;
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (i=0; i < WAKE_SLOTS/2; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS + SIZEOF_IR * i;
		eeprom_restore(buf, idx);
		/* first encounter of zero in check_wakeups() means end of check */
		if (!memcmp(buf, &zeros, sizeof(zeros)))
			break;
		if (!memcmp(buf, ir, sizeof(buf)))
			Wakeup();
	}
}

/* enter BOOTSEL mode, light board led on mass storage activity */
void reboot(void)
{
	fast_toggle();
	reset_usb_boot(PICO_DEFAULT_LED_PIN, 0);
}

/* is received ir-code in the last wakeup-slot? reboot µC if true */
void check_reboot(IRMP_DATA *ir)
{
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	idx = (MACRO_DEPTH + 1) * SIZEOF_IR * MACRO_SLOTS + SIZEOF_IR * (WAKE_SLOTS - 1);
	eeprom_restore(buf, idx);
	if (!memcmp(buf, ir, sizeof(buf)))
		reboot();
}

void transmit_macro(uint8_t macro)
{
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	/* we start from 1, since we don't want to tx the trigger code of the macro*/
	for (i=1; i < MACRO_DEPTH + 1; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * macro + SIZEOF_IR * i;
		eeprom_restore(buf, idx);
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

/* is received ir-code (trigger) in one of the macro-slots? transmit_macro if true */
void check_macros(IRMP_DATA *ir)
{
	uint8_t i;
	uint16_t idx;
	uint8_t buf[SIZEOF_IR];
	uint8_t zeros[SIZEOF_IR] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (i=0; i < MACRO_SLOTS; i++) {
		idx = (MACRO_DEPTH + 1) * SIZEOF_IR * i;
		eeprom_restore(buf, idx);
		/* first encounter of zero in check_macros() means end of check */
		if (!memcmp(buf, &zeros, sizeof(zeros)))
			break;
		if (!memcmp(buf, ir, sizeof(buf)))
			transmit_macro(i);
	}
}

void led_callback(uint_fast8_t on)
{
	toggle_led();
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

	board_init();
	LED_Switch_init();
	Systick_Init();
	tusb_init();
	IRMP_Init();
	irsnd_init();
	eeprom_begin(2*FLASH_PAGE_SIZE, 2); // 16 pages of 512 byte
	irmp_set_callback_ptr(led_callback);

	while (1)
	{
		tud_task(); // tinyusb device task

		if (board_button_read() && !tud_ready())
			Wakeup();

		if (!AlarmValue && !tud_ready())
			Wakeup();

		/* always wait for previous transfer to complete before sending again, consider using a send buffer */
		if (PrevXferComplete && send_ir_on_delay && last_magic_sent != send_ir_on_delay) {
			send_magic();
			last_magic_sent = send_ir_on_delay;
		}

		/* test if configuration command is received */
		if(PrevXferComplete && USB_HID_Data_Received && *bufptr == REPORT_ID_CONFIG_OUT && *(bufptr+1) == STAT_CMD) {
			USB_HID_Data_Received = 0;

			switch (*(bufptr+2)) {
			case ACC_GET:
				ret = get_handler(bufptr);
				break;
			case ACC_SET:
				ret = set_handler(bufptr);
				break;
			case ACC_RESET:
				ret = reset_handler(bufptr);
				break;
			default:
				ret = -1;
			}

			if (ret == -1) {
				*(bufptr+1) = STAT_FAILURE;
				ret = 4;
			} else {
				*(bufptr+1) = STAT_SUCCESS;
			}

			/* send configuration data */
			USB_HID_SendData(REPORT_ID_CONFIG_IN, bufptr, ret);
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
				check_reboot(&myIRData);
			}

			/* send IR-data */
			USB_HID_SendData(REPORT_ID_IR, (uint8_t *) &myIRData, sizeof(myIRData));
		}
	}
}
