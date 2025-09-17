/*
    RP2xxx EEPROM emulation

    Based on ESP8266 EEPROM library, which is
    Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
    This file is part of the esp8266 core for Arduino environment.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

extern bool _dirty;

void eeprom_begin(size_t size, uint8_t sectors);
uint8_t eeprom_read(int const address);
void eeprom_write(int const address, uint8_t const val);
bool eeprom_commit();
void eeprom_reset();
void eeprom_get_raw(uint8_t*, uint8_t, uint8_t);