/**********************************************************************************************************
	stm32IRconfig: configure and monitor IRMP_STM32

	Copyright (C) 2014-2022 Jörg Riechardt

	based on work by Alan Ott
	Copyright 2010  Alan Ott

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

***********************************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#include <inttypes.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#define	HID_IN_REPORT_COUNT	17 /* STM32->PC */
#define	HID_OUT_REPORT_COUNT	17 /* PC->STM32 */

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
	CMD_REBOOT
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_IR = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3
};

hid_device *handle;
unsigned char inBuf[HID_IN_REPORT_COUNT];
unsigned char outBuf[HID_OUT_REPORT_COUNT];

static bool open_stm32() {
	// Open the device using the VID, PID.
	handle = hid_open(0x1209, 0x4444, NULL);
	if (!handle) {
		printf("error opening stm32 device\n");
		return false;
	}
	printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	int retVal;
	retVal = hid_read(handle, inBuf, sizeof(inBuf));
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (int i = 0; i < retVal; i++)
			printf("%02x ", (unsigned int)inBuf[i]);
		puts("\n");
	}
} 

static void write_stm32(int out_size) {
	int retVal;
	retVal = hid_write(handle, outBuf, out_size);
	if (retVal < 0) {
		printf("write error: %ls\n", hid_error(handle));
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < retVal; i++)
			printf("%02x ", (unsigned int)outBuf[i]);
		puts("\n");
	}
}

void write_and_check(int idx) {
	write_stm32(idx);
	#ifdef WIN32
	Sleep(3);
	#else
	usleep(3000);
	#endif
	read_stm32(); // blocking per default, waits until data arrive
	while (inBuf[0] == REPORT_ID_IR)
		read_stm32();
	if (inBuf[1] == STAT_SUCCESS) {
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

int main(int argc, char* argv[])
{
	uint64_t i;
	char c, d;
	uint8_t k, l, idx;
	unsigned int s, m;
	int retValm, jump_to_firmware;
	FILE *fp;
	char testfilename[10];
	uint16_t j = 0;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	if (hid_init())
		return -1;

	open_stm32();

	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;

cont:	printf("program eeprom: wakeups and macros (p)\nprogram eeprom: wakeups and macros with remote control(P)\nget eeprom (wakeups, macros and capabilities) (g)\nreset (wakeups, macros and alarm) (r)\nset alarm (s)\nget alarm (a)\nsend IR (i)\nreboot (b)\nmonitor until ^C (m)\nrun test (t)\nhid test (h)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 'p':
prog:		printf("set wakeup(w)\nset macro(m)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		default:
			goto prog;
		}
		printf("enter IRData (protocoladdresscommandflag)\n");
		scanf("%I64x", &i);
		outBuf[idx++] = (i>>40) & 0xFF;
		outBuf[idx++] = (i>>24) & 0xFF;
		outBuf[idx++] = (i>>32) & 0xFF;
		outBuf[idx++] = (i>>8) & 0xFF;
		outBuf[idx++] = (i>>16) & 0xFF;
		outBuf[idx++] = i & 0xFF;
		write_and_check(idx);
		break;

	case 'P':
Prog:		printf("set wakeup with remote control(w)\nset macro with remote control(m)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		default:
			goto Prog;
		}
		printf("enter IRData by pressing a button on the remote control\n");
		read_stm32();
		/* it is necessary, to have *all* IR codes received, *before* calling
		* write_and_check(), in order to avoid, that these disturb later! */
		#ifdef WIN32
		Sleep(500); 
		#else
		usleep(500000);
		#endif
		outBuf[idx++] = inBuf[1];
		outBuf[idx++] = inBuf[2];
		outBuf[idx++] = inBuf[3];
		outBuf[idx++] = inBuf[4];
		outBuf[idx++] = inBuf[5];
		outBuf[idx++] = inBuf[6];
		write_and_check(idx);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget macro slot(m)\nget caps(c)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'c':
			jump_to_firmware = 0;
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_stm32(idx+1);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				read_stm32();
				while (inBuf[0] == 0x01)
					read_stm32();
				if (!l) { // first query for slots and depth
					printf("macro_slots: %u\n", inBuf[4]);
					printf("macro_depth: %u\n", inBuf[5]);
					printf("wakeup_slots: %u\n", inBuf[6]);
				} else {
					if(!jump_to_firmware) { // queries for supported_protocols
						printf("protocols: ");
						for (k = 4; k < sizeof(inBuf); k++) {
							if (!inBuf[k]) { // NULL termination
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[k]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (k = 4; k < sizeof(inBuf); k++) {
							if (!inBuf[k]) { // NULL termination
								printf("\n\n");
								goto out;
							}
							printf("%c", inBuf[k]);
						}
					}
				}
				printf("\n\n");
again:			;
			}
			break;
		default:
			goto get;
		}
		write_and_check(idx);
out:
		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset macro slot(m)\nreset alarm(a)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			break;
		default:
			goto reset;
		}
		write_and_check(idx);
		break;

	case 's':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_ALARM;
		printf("enter alarm\n");
		scanf("%I64x", &i);
		memcpy(&outBuf[idx++], &i, 4);
		write_and_check(idx);
		break;

	case 'a':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		outBuf[idx++] = CMD_ALARM;
		write_and_check(idx);
		break;

	case 'i':
		printf("enter IRData (protocoladdresscommandflag)\n");
		scanf("%I64x", &i);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_EMIT;
		outBuf[idx++] = (i>>40) & 0xFF;
		outBuf[idx++] = (i>>24) & 0xFF;
		outBuf[idx++] = (i>>32) & 0xFF;
		outBuf[idx++] = (i>>8) & 0xFF;
		outBuf[idx++] = (i>>16) & 0xFF;
		outBuf[idx++] = i & 0xFF;
		write_and_check(idx);
		break;

	case 'b':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_REBOOT;
		write_and_check(idx);
		hid_close(handle);
		#ifdef WIN32
		Sleep(1900);
		#else
		usleep(1900000);
		#endif
		for(l=0;l<6;l++) {
			if(open_stm32() == true)
				break;
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
		}
		break;

	case 'm':
		goto monit;
		break;

	case 't':
		goto test;
		break;

	case 'x':
		goto exit;
		break;

	case 'h':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		for(l = 2; l < sizeof(outBuf); l++){
			outBuf[l] = l - 1; // ACC_SET CMD_HID_TEST ...
		}
		write_and_check(sizeof(outBuf));
		break;

	default:
		goto cont;
	}

	goto cont;

monit:	while(true) {
		retValm = hid_read(handle, inBuf, sizeof(inBuf));
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < retValm; l++)
				printf("%02x ", (unsigned int)inBuf[l]);
			printf("\n");
			printf("converted to protocoladdresscommandflag:\n\t");
			printf("%02x%02x%02x%02x%02x%02x", (unsigned int)inBuf[1],(unsigned int)inBuf[3],(unsigned int)inBuf[2],(unsigned int)inBuf[5],(unsigned int)inBuf[4],(unsigned int)inBuf[6]);
			printf("\n\n");
		}
	}

test:	sprintf(testfilename, "test%u", j); printf("write into %s\n", testfilename); // if directory, it needs to exist (or be created)!
	fp = fopen(testfilename, "w");
	while(true) {
		retValm = hid_read(handle, inBuf, sizeof(inBuf));
		if (retValm >= 0) {
			printf("%02x%02x%02x%02x%02x%02x", (unsigned int)inBuf[1],(unsigned int)inBuf[3],(unsigned int)inBuf[2],(unsigned int)inBuf[5],(unsigned int)inBuf[4],(unsigned int)inBuf[6]);
			fprintf(fp, "%02x%02x%02x%02x%02x%02x", (unsigned int)inBuf[1],(unsigned int)inBuf[3],(unsigned int)inBuf[2],(unsigned int)inBuf[5],(unsigned int)inBuf[4],(unsigned int)inBuf[6]);
			if (inBuf[1] == 0x3c && inBuf[3] == 0 && inBuf[2] == 0 && inBuf[5] == 0 && inBuf[4] == 0x3f && inBuf[6] == 1) { // 3c0000003f01, stopsequence TODO make configurable
				printf("received stopsequence\n");
				fclose(fp);
				j++;
				if (j >= 200) { // TODO make number of tests configurable
					printf("exit\n");
					goto exit;
				}
				goto test;
			}
		}
	}


exit:	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	Sleep(2);
#endif

	return 0;
}
