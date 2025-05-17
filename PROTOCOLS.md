## Protocol for IR Data

byte 0:	   0x01 (µC->PC) 
byte 1:    protocol
bytes 2+3: address
bytes 4+5: command
byte 6:    flags


## Protocol for Configuration

byte 0: 0x03 (PC->µC) or 0x02 (µC->PC) 

The PC sends obligatory bytes:

byte 1: status: STAT_CMD (command PC->µC), STAT_SUCCESS (success report µC->PC), STAT_FAILURE (error report µC->PC)
byte 2: access: ACC_GET (query value from µC), ACC_SET (set value in µC), ACC_RESET (reset value in µC)
byte 3: command: CMD_EMIT (transmit IR data), CMD_CAPS (capabilities of µC), CMD_ALARM (alarm timer), CMD_MACRO (macro), CMD_WAKE (wakeup), CMD_REBOOT (restart µC),
	CMD_EEPROM_RESET (reset eeprom), CMD_EEPROM_COMMIT (write to eeprom), CMD_EEPROM_GET_RAW (dump eeprom), CMD_STATUSLED (statusled), CMD_NEOPIXEL (rgb-led)

and optional bytes:

byte 4, (5): possible arguments, e.g. number of macro (and content of macro)
following bytes: for ACC_SET: value to be set


The device answers with the 3 obligatory bytes, the first byte (status) is set to STAT_SUCCESS or STAT_FAILURE. More bytes may follow, e.g. for ACC_GET.

## Examples:
Query the alarmtimer (in 60 seconds WakeUp):
PC->µC: STAT_CMD ACC_GET CMD_ALARM
µC->PC: STAT_SUCCESS ACC_GET CMD_ALARM 0x00 0x00 0x00 0x3C

Set first (slot 0) wakeup IR data to 0x112233445566:
PC->µC: STAT_CMD ACC_SET CMD_WAKE 0x00 0x11 0x22 0x33 0x44 0x55 0x66
µC->PC: STAT_SUCCESS ACC_SET CMD_WAKE

Set third (slot 2) macro trigger (slot 0) to 0x112233445566:
PC->µC: STAT_CMD ACC_SET CMD_MACRO 0x02 0x00 0x11 0x22 0x33 0x44 0x55 0x66
µC->PC: STAT_SUCCESS ACC_SET CMD_MACRO

Query the fourth (slot 4) IR data of the third (slot 2) macro:
PC->µC: STAT_CMD ACC_GET CMD_MACRO 0x02 0x04
µC->PC: STAT_SUCCESS ACC_GET CMD_MACRO 0x77 0x88 0x99 0xAA 0xBB 0xCC

Neopixel (RP2xxx only): byte 4 = total length of payload = (length of strip) * 3, the total length is divided into chunks of 57, byte 5 = chunk number, byte 6 set only one led: number of data start byte + 1, byte 7 ... 63 rgb data
Set eight neopixels to red, green, blue, red, green, blue, red, green (length = 8 * 3, chunk = 0, unused, rgb, rgb, rgb, ...):
PC->µC: STAT_CMD ACC_SET CMD_NEOPIXEL 0x18 0x00 0x00 0xff 0x00 0x00 0x00 0xff 0x00 0x00 0x00 0xff 0xff 0x00 0x00 0x00 0xff 0x00 0x00 0x00 0xff 0xff 0x00 0x00 0x00 0xff 0x00
µC->PC: STAT_SUCCESS ACC_SET CMD_NEOPIXEL
Set 21th neopixel to red (length = 21 * 3, chunk = 1, start at data byte number 3, 000, rgb):
PC->µC: STAT_CMD ACC_SET CMD_NEOPIXEL 0x3F 0x01 0x04 0x00 0x00 0x00 0xff 0x00 0x00
µC->PC: STAT_SUCCESS ACC_SET CMD_NEOPIXEL
