/**********************************************************************************************************  
    stm32IRalarm: set alarm to and get alarm from STM32IR
    Copyright (C) 2014-2015 Joerg Riechardt
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
************************************************************************************************************/

#include <stdio.h>
/*
#include <sys/socket.h>
#include <sys/un.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
*/
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#include <inttypes.h>

#ifdef _WIN32
	#include <windows.h>
	#include "getopt.h"
	#include <tchar.h>
	#define STATIC_GETOPT
	#define _GETOPT_API
#else
	#include <unistd.h>
#endif

hid_device *handle;
unsigned char inBuf[17];
unsigned char outBuf[17];

static bool open_stm32() {
	// Open the device using the VID, PID.
	handle = hid_open(0x0483, 0x5750, NULL);
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
	    #ifdef WIN32
		Sleep(2);
		#else
		usleep(2000);
		#endif
	    read_stm32(); /* necessary to avoid, that echo is read by first alarm read */
	    while (inBuf[0] == 0x01)
		read_stm32();
	}

	if (aflag) {
	    //memset(&outBuf[3], 0, 14);
	    outBuf[2] = 0x00; // ACC_GET
	    outBuf[3] = 0x03; // CMD_ALARM
	    write_stm32();
	    #ifdef WIN32
		Sleep(2);
		#else
		usleep(2000);
		#endif
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

#ifdef WIN32
	Sleep(2);
#endif

	return 0;
}
