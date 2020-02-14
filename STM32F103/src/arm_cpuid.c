/* MIT License
*
* Copyright (c) 2018, 2019 André van Schoubroeck, https://github.com/a-v-s
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <stdint.h>

#include <stdio.h>

//#include "stm32f103xb.h"
#include "stm32f10x.h"

#include "arm_cpuid.h"

#include "core_cm3.h"

#include <inttypes.h>

#include <stdbool.h>

char* cpuid(){
	static char buff[64] = "Unknown";
	// ARM v7m manual,page 702, section B4.1.2.
  // 0xE000ED00 // CPUID
	SCB->CPUID;
	cpuid_t *cpuid =(cpuid_t *)(&SCB->CPUID);
	//
	if ((cpuid->PartNo & (0b11<<10)) == (0b11<<10)) {
		// Cortex family
		if ((cpuid->PartNo & (0b10 << 4)) == (0b10 << 4)) {
			// Cortex M
			sprintf(buff,"Cortex-M%d r%dp%d"  , cpuid->PartNo & 0xF ,cpuid->Variant, cpuid->Revision);
		}
	}
	return buff;
	// But the interesting part comes at 0xE000ED40 // ID_PFR0 (Processor Feature Register) and following
	// We should be able to access it through
	SCB->PFR[0];
	SCB->PFR[1];
	/*
  __IM  uint32_t PFR[2U];         		 //< Offset: 0x040 (R/ )  Processor Feature Register
  __IM  uint32_t DFR;                    //< Offset: 0x048 (R/ )  Debug Feature Register
  __IM  uint32_t ADR;                    //< Offset: 0x04C (R/ )  Auxiliary Feature Register
  __IM  uint32_t MMFR[4U];               //< Offset: 0x050 (R/ )  Memory Model Feature Register
  __IM  uint32_t ISAR[5U];               //< Offset: 0x060 (R/ )  Instruction Set Attributes Register
	 */


}

char rt[128];

void stm_romtable() {

        intptr_t ROMTABLE = (intptr_t) (0xE00FF000);

        // ID080813 // PAGE 202
        // So, now I understand how to interpret the ROM Table
        // The ARMv7 TRM just says, these are the values,
        // and the peripheral exists at some address
        // But the ARM Debug Interface  Architecture Specification (ID080813)
        // Actually explains the values. Which part is the offset, and then
        // I add the offset to the address of the current table and I find another entry
        //--
        // And how I know too what the CID in the romtable id is

        /*
         cpuid_t *cpuid = (cpuid_t*) (&SCB->CPUID);

         intptr_t ROMTABLE = NULL;


         // Looking at the Cortex-M TRM, it seems it should always be
         // at 0xE00FF000, so where is the 0xF00FF000 address
         // coming from we've seen at PSoC and nRF ?????
         switch (cpuid->Architecture) {
         case 0xC:
         // ARMV6m
         ROMTABLE = (intptr_t) (0xE00FF000);
         // ID051917 page 282
         break;
         case 0xf:
         // Other, assume ARMv7m
         ROMTABLE = (intptr_t) (0xE00FF000);
         // ID120114 page 741
         break;
         }
         */
        /*
         * int *pid = (int*) (0xE00FFFE0);
         int *pid = (int *) (ROMTABLE | 0xFE0);

         bool used = pid[2] & 8;
         uint8_t identity_code = ((pid[1] & 0xf0) >> 4) | ((pid[2] & 0x7) << 4);
         uint8_t continuation_code = (*(int*) (0xE00FFFD0)) & 0xF;

         sprintf(rt, "%sRT USED:%1d CONT:%3d ID:%3d", cpuid(), used,    continuation_code, identity_code);
         */

        // These parts have been tested before
        // stm32: id = 32, cont = 0 "STMicroelectronics"
        // gd32:  id = 81, cont = 7 "GigaDevice Semiconductor (Beijing) Inc."
        // These are the new parts
        // apm32: id = 59, cont = 4 "ARM Ltd."
        // cs32:  id = 59, cont = 4 "ARM Ltd."
        // We can read this value on stm32, cs32, (gd and apm still need to be tested)
        // So, we can determine what core we're running on. We still need to find
        // a way to tell apm32 and cs32 apart.

        romtable_id_t *rid = (romtable_id_t*) (ROMTABLE | 0xFD0);

        bool jep106_used = (rid->pid2 & (1 << 3)) == (1 << 3);
        int continuation_code = rid->pid4 & 0xF;
        int identity_code = ((rid->pid1 & 0xf0) >> 4) | ((rid->pid2 & 0x7) << 4);
        int partno = rid->pid0 | ((rid->pid1 & 0xF) << 8);
        int revision = (rid->pid3 & 0x0F) >> 4;
        //int modified = (rid->pid3 & 0xF);

        //int _4kcount = (rid->pid4 & 0xF0) >> 4; // always 0b0000 ?
        //https://developer.arm.com/docs/103489663/10/peripheralid-values-for-the-coresight-rom-table

        char *prob = "Unknown";
        if (identity_code == 32 && continuation_code == 0) {
                prob = "STM32";
        }
        if (identity_code == 81 && continuation_code == 7) {
                prob = "GD32";
        }
        if (identity_code == 59 && continuation_code == 4) {
                // APM or CS
                romtable_t *rom = (romtable_id_t*) (ROMTABLE);
                if (rom->etm & 1) {
                        prob = "CS32";
                } else {
                        prob = "APM32";
                }
        }

        sprintf(rt, "%s %s  V:%1d CONT:%3d ID:%3d PART: %3X REV:%3d ", prob,
                        cpuid(), jep106_used, continuation_code, identity_code, partno,
                        revision);

}
