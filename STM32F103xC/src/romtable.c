/*
 Copyright (C) 2020 Joerg Riechardt

 File:		main.c
 License: 	MIT

 Copyright (c) 2020 André van Schoubroeck

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <stdio.h>
#include "stm32f10x.h"
#include "romtable.h"

char rt[8];

void parse_romtable() {
	// ARM v7-M Architecture Reference Manual D1.1 and C1.2.2: peripheral ID at offset 0xFD0, ETM at offset 0x14
	// STM32 Reference manual RM0008 31.6.4: romtable base at  0xE00FF000
	uint8_t continuation_code = (*(int*)(0xE00FFFD0)) & 0x0F;
	uint8_t etm = (*(int*)(0xE00FF014)) & 0x01;

	char *prob = "Unknown";
	if (continuation_code == 0)
		prob = "STM32";
	if (continuation_code == 7)
		prob = "GD32";
	if (continuation_code == 4) {
		if (etm) {
			prob = "CS32";
		} else {
			prob = "APM32";
		}
	}
	if (continuation_code == 5) {
		prob = "HK32";
	}

	sprintf(rt, "%s", prob);
}
