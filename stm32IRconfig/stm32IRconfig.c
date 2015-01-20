/**********************************************************************************************************  
    stm32config: configure and monitor STM32IR

    Copyright (C) 2014 Jörg Riechardt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

************************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>
#include <termios.h>
#include <fcntl.h>

static int stm32fd = -1;
uint8_t inBuf[17];
uint8_t outBuf[17];

static bool open_stm32(const char *devicename) {
	stm32fd = open(devicename, O_RDWR );
	if (stm32fd == -1) {
		printf("error opening stm32 device: %s\n",strerror(errno));
		return false;
	}
	printf("opened stm32 device\n");
	return true;
}

static void read_stm32() {
	int i;
	int retVal;
	retVal = read(stm32fd, inBuf, sizeof(inBuf));
	if (retVal < 0) {
	    printf("read error\n");
        } else {
                printf("read %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", inBuf[i]);
                puts("\n");
        }
} 

static void write_stm32() {
	int i;
	int retVal;
	retVal = write(stm32fd, outBuf, sizeof(outBuf));
	if (retVal < 0) {
	    printf("write error\n");
        } else {
                printf("written %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", outBuf[i]);
                puts("\n");
        }
}

void write_and_check() {
        write_stm32();
        usleep(2000);
        read_stm32();
        while (inBuf[0] == 0x01)
		read_stm32();
	if (inBuf[1] == 0x01) { // STAT_SUCCESS
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

int main(int argc, const char **argv) {
	
	uint64_t i;
	char c, d;
	uint8_t s, m, k, l, idx;
	int retValm;
	
	open_stm32(argc>1 ? argv[1] : "/dev/hidraw2");
	
	outBuf[0] = 0x03; // Report ID
	outBuf[1] = 0x00; // STAT_CMD
	
cont:   printf("program eeprom: wakeups and macros (p)\nget eeprom (wakeups, macros and capabilities) (g)\nreset (wakeups, macros and alarm) (r)\nset alarm (s)\nget alarm (a)\nsend IR (i)\nmonitor until ^C (m)\nexit (x)\n");
	scanf("%s", &c);
	
	switch (c) {
	
	case 'p':
prog:	    printf("set wakeup(w)\nset macro slot(m)\n");
	    scanf("%s", &d);
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x01; // ACC_SET
	    switch (d) {
		case 'w':
		    printf("enter slot number (starting with 0)\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = 0x05; // CMD_WAKE
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		case 'm':
		    printf("enter macro number (starting with 0)\n");
		    scanf("%"SCNd8"", &m);
		    outBuf[idx++] = 0x04; // CMD_MACRO
		    outBuf[idx++] = m;    // (m+1)-th macro
		    printf("enter slot number, 0 for trigger\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		default:
		    goto prog;
	    }
	    printf("enter IRData (protocoladdresscommandflag)\n");
	    scanf("%"SCNx64"", &i);
	    outBuf[idx++] = (i>>40) & 0xFF;
	    outBuf[idx++] = (i>>24) & 0xFF;
	    outBuf[idx++] = (i>>32) & 0xFF;
	    outBuf[idx++] = (i>>8) & 0xFF;
	    outBuf[idx++] = (i>>16) & 0xFF;
	    outBuf[idx++] = i & 0xFF;
	    write_and_check();
	    break;
	    
	case 'g':
get:	    printf("get wakeup(w)\nget macro slot(m)\nget caps(c)\n");
	    scanf("%s", &d);
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x00; // ACC_GET
	    switch (d) {
		case 'w':
		    printf("enter slot number (starting with 0)\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = 0x05; // CMD_WAKE
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		case 'm':
		    printf("enter macro number (starting with 0)\n");
		    scanf("%"SCNd8"", &m);
		    outBuf[idx++] = 0x04; // CMD_MACRO
		    outBuf[idx++] = m;    // (m+1)-th macro
		    printf("enter slot number, 0 for trigger\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		case 'c':
		    outBuf[idx++] = 0x01; // CMD_CAPS
		    for (l = 0; l < 4; l++) {
			outBuf[idx] = l;    // 0 slots, ++ protocols
			write_stm32();
			usleep(2000);
			read_stm32();
			while (inBuf[0] == 0x01)
			    read_stm32();
			if (!l) {
			    printf("macro_slots: %u\n", inBuf[4]);
			    printf("macro_depth: %u\n", inBuf[5]);
			    printf("wake_slots: %u\n", inBuf[6]);
			} else {
			    printf("protocols: ");
			    for (k = 4; k < 17; k++) {
			    if (!inBuf[k]) {
				printf("\n\n");
				goto out;
			    }
			    printf("%u ", inBuf[k]);
			    }
			}
			printf("\n\n");
		    }
		    break;
		default:
		    goto get;
	    }
	    write_and_check();
out:	    break;

	case 'r':
reset:	    printf("reset wakeup(w)\nreset macro slot(m)\nreset alarm(a)\n");
	    scanf("%s", &d);
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x02; // ACC_RESET
	    switch (d) {
		case 'w':
		    printf("enter slot number (starting with 0)\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = 0x05; // CMD_WAKE
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		case 'm':
		    printf("enter macro number (starting with 0)\n");
		    scanf("%"SCNd8"", &m);
		    outBuf[idx++] = 0x04; // CMD_MACRO
		    outBuf[idx++] = m;    // (m+1)-th macro
		    printf("enter slot number, 0 for trigger\n");
		    scanf("%"SCNd8"", &s);
		    outBuf[idx++] = s;    // (s+1)-th slot
		    break;
		case 'a':
		    outBuf[idx++] = 0x03; // CMD_ALARM
		    break;
		default:
		    goto reset;
	    }
	    write_and_check();
	    break;

	case 's':
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x01; // ACC_SET
	    outBuf[idx++] = 0x03; // CMD_ALARM
	    printf("enter alarm\n");
	    scanf("%"SCNx64"", &i);
	    memcpy(&outBuf[idx++], &i, 4);
	    write_and_check();
	    break;
	    
	case 'a':
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x00; // ACC_GET
	    outBuf[idx++] = 0x03; // CMD_ALARM
	    write_and_check();
	    break;	    
	    
	case 'i':
	    printf("enter IRData (protocoladdresscommandflag)\n");
	    scanf("%"SCNx64"", &i);
	    memset(&outBuf[2], 0, 15);
	    idx = 2;
	    outBuf[idx++] = 0x01; // ACC_SET
	    outBuf[idx++] = 0x00; // CMD_EMIT
	    outBuf[idx++] = (i>>40) & 0xFF;
	    outBuf[idx++] = (i>>24) & 0xFF;
	    outBuf[idx++] = (i>>32) & 0xFF;
	    outBuf[idx++] = (i>>8) & 0xFF;
	    outBuf[idx++] = (i>>16) & 0xFF;
	    outBuf[idx++] = i & 0xFF;
	    write_and_check();
	    break;
	    
	case 'm':
	    goto monit;
	    break;
	
	case 'x':
	    goto exit;
	    break;
	    
	default:
	    goto cont;
	}
	
	goto cont;
	
monit:  while(true) {
	    retValm = read(stm32fd, inBuf, sizeof(inBuf));
	    if (retValm >= 0) {
		printf("read %d bytes:\n\t", retValm);
		for (l = 0; l < retValm; l++)
		    printf("%02hhx ", inBuf[l]);
		printf("\n");
		printf("converted to protocoladdresscommandflag:\n\t");
		printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
		printf("\n\n");
	    }
	}
	
exit:	if (stm32fd >= 0) close(stm32fd);
	return 0;
}
