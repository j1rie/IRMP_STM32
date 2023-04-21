/*
    Copyright (c) 2023 Joerg Riechardt. All rights reserved.

    Wear leveling inspired by https://github.com/MakerMatrix/RP2040_flash_programming

    Based on RP2040 EEPROM emulation, which is
    Copyright (c) 2021 Earle F. Philhower III. All rights reserved.

    Based on ESP8266 EEPROM library, which is
    Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.

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

#include "eeprom.h"
#include <hardware/flash.h>
#include <hardware/sync.h>

uint8_t* _sector;
uint8_t* _data = NULL;
size_t _size = 0;
bool _dirty = false;
int first_empty_page = -1;
uint8_t _nr_sectors = 0;

void eeprom_begin(size_t size, uint8_t nr_sectors) {
    _nr_sectors = nr_sectors;
    _sector = (uint8_t*)(PICO_FLASH_SIZE_BYTES - _nr_sectors * FLASH_SECTOR_SIZE + XIP_BASE);

    if ((size <= 0) || (size > _nr_sectors * FLASH_SECTOR_SIZE)) {
        size = _nr_sectors * FLASH_SECTOR_SIZE;
    }

    size = (size + 255) & (~255);  // Flash writes limited to 256 byte boundaries

    // In case begin() is called a 2nd+ time, don't reallocate if size is the same
    if (_data && size != _size) {
        free(_data);
        _data = malloc(size*sizeof(_data));
    } else if (!_data) {
        _data = malloc(size*sizeof(_data));
    }

    _size = size;

    for(int page = 0; page < _nr_sectors * FLASH_SECTOR_SIZE / _size; page++) {
        uint8_t last_byte = *(_sector + (page * _size) + _size - 1); // is last byte marked?
        // find first empty page
        if(last_byte == 0xFF){ // 0xFF means still unwritten after erase
            first_empty_page = page;
            break;
        }
    }

    if(first_empty_page > 0)
        memcpy(_data, _sector + (first_empty_page - 1) * _size, _size);
    else
        memcpy(_data, _sector, _size);

    _dirty = false; //make sure dirty is cleared in case begin() is called 2nd+ time
}

uint8_t eeprom_read(int const address) {
    if (address < 0 || (size_t)address >= _size) {
        return 0;
    }
    if (!_data) {
        return 0;
    }

    return _data[address];
}

void eeprom_write(int const address, uint8_t const value) {
    if (address < 0 || (size_t)address >= _size) {
        return;
    }
    if (!_data) {
        return;
    }

    // Optimise _dirty. Only flagged if data written is different.
    uint8_t* pData = &_data[address];
    if (*pData != value) {
        *pData = value;
        _dirty = true;
    }
}

bool eeprom_commit() {
    if (!_size) {
        return false;
    }
    if (!_dirty) {
        return true;
    }
    if (!_data) {
        return false;
    }

    uint32_t status = save_and_disable_interrupts();
    //rp2040.idleOtherCore();

    if(first_empty_page == -1){
        flash_range_erase((intptr_t)_sector - (intptr_t)XIP_BASE, _nr_sectors * FLASH_SECTOR_SIZE);
        first_empty_page = 0;
    }

    _data[_size - 1] = 0x00; // mark last byte
    flash_range_program((intptr_t)_sector - (intptr_t)XIP_BASE + first_empty_page * _size, _data, _size);
    first_empty_page++;
    if(!(first_empty_page < _nr_sectors * FLASH_SECTOR_SIZE / _size))
        first_empty_page = -1;

    //rp2040.resumeOtherCore();
    restore_interrupts(status);

    return true;
}

void eeprom_reset() {
    uint32_t status = save_and_disable_interrupts();
    //rp2040.idleOtherCore();
    flash_range_erase((intptr_t)_sector - (intptr_t)XIP_BASE, _nr_sectors * FLASH_SECTOR_SIZE);
    //rp2040.resumeOtherCore();
    restore_interrupts(status);
}

void eeprom_get_raw(uint8_t* ptr , uint8_t page, uint8_t chunk) {
    memcpy(ptr, _sector + (page * _size) + chunk * 32, 32);
}
