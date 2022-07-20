/**********************************************************************************************************  
    stm32IRalarm: set alarm to and get alarm from STM32IR

    Copyright (C) 2014-2015 Joerg Riechardt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

************************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <wchar.h>
#include <string.h>
#include "hidapi.h"
#include <stdint.h>
#include <windows.h>
#include "getopt.h"
#include <tchar.h>

#define STATIC_GETOPT
#define _GETOPT_API
#define _CRT_SECURE_NO_WARNINGS

hid_device *handle;
unsigned char inBuf[17];
unsigned char outBuf[17];

static bool open_stm32() {
	// Open the device using the VID, PID.
	handle = hid_open(0x1209, 0x4444, NULL);
	if (!handle) {
		printf("error opening stm32 device\n");
		return false;
	}
	//printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	//int i;
	int retVal;
	retVal = hid_read(handle, inBuf, sizeof(inBuf));
	if (retVal < 0) {
	    printf("read error\n");
        }/* else {
                printf("read %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", inBuf[i]);
                puts("\n");
        }*/
} 

static void write_stm32() {
	//int i;
	int retVal;
	retVal = hid_write(handle, outBuf, sizeof(outBuf));
	if (retVal < 0) {
	    printf("write error: %ls\n", hid_error(handle));
        }/* else {
                printf("written %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", outBuf[i]);
                puts("\n");
        }*/
}

int _tmain(int argc, TCHAR** argv) {

	uint32_t setalarm;
	uint32_t alarm = 0;
	int opt = 0;
	int aflag = 0;
	char *dvalue = NULL;
	char *svalue = NULL;
	time_t wakeup;
	struct tm *ts;
	
	while ((opt = getopt(argc, argv, _T(":d:as:"))) != -1) {
	    switch (opt) {
	    case _T('d'):
		dvalue = optarg;
		break;
	    case _T('a'):
		aflag = 1;
		break;
	    case _T('s'):
		svalue = optarg;
		break;
	    default:
		break;
	    }
	}

	open_stm32();
        outBuf[0] = 0x03; // Report ID
	outBuf[1] = 0x00; // STAT_CMD

	if (svalue != NULL) {
	    outBuf[2] = 0x01; // ACC_SET
	    outBuf[3] = 0x03; // CMD_ALARM
	    setalarm = strtoul(svalue, NULL, 0);
	    memcpy(&outBuf[4], &setalarm, sizeof(setalarm));
	    write_stm32();
	    Sleep(2);
	    read_stm32(); /* necessary to avoid, that echo is read by first alarm read */
	    while (inBuf[0] == 0x01)
		read_stm32();
	}

	if (aflag) {
	    //memset(&outBuf[3], 0, 14);
	    outBuf[2] = 0x00; // ACC_GET
	    outBuf[3] = 0x03; // CMD_ALARM
	    write_stm32();
	    Sleep(2);
	    read_stm32();
	    while (inBuf[0] == 0x01)
		read_stm32();
	    alarm = *((uint32_t *)&inBuf[4]);
	    printf("\tSTM32alarm: %"PRIu16" days %d hours %d minutes %d seconds\n", alarm/60/60/24, (alarm/60/60) % 24, (alarm/60) % 60, alarm % 60);
	    wakeup = time(NULL);
	    wakeup += alarm;
	    ts = (localtime(&wakeup));
	    printf("\tVDRwakeup: %s", asctime(ts));
	}

	hid_close(handle);
	/* Free static HIDAPI objects. */
	hid_exit();
	Sleep(2);
	return 0;
}
