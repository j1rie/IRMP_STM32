/*
 File:		arm_cpuid.h
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

#ifndef LIBHALGLUE_COMMON_ARM_CPUID_H_
#define LIBHALGLUE_COMMON_ARM_CPUID_H_

char* cpuid();
void parse_romtable();
extern char rt[128];

// This is the ROM TABLE as defined in the Cortex-M reference manual
// Please note a non conforming implementation might differ from this
// but still be a valid ROM TABLE when being parsed as being a ROM TABLE.
// Meaning, this expect the entry for these components at this location
typedef struct {
	uint32_t nvic;
	uint32_t dwt;
	uint32_t fpb;
	uint32_t itm;
	uint32_t tpiu;
	uint32_t etm;
	uint32_t end;
	// ... ///
} cortex_m_romtable_t;

typedef struct {
	uint32_t pid4;
	uint32_t pid5;
	uint32_t pid6;
	uint32_t pid7;
	uint32_t pid0;
	uint32_t pid1;
	uint32_t pid2;
	uint32_t pid3;
	uint32_t cid0;
	uint32_t cid1;
	uint32_t cid2;
	uint32_t cid3;
} romtable_id_t; // at ROMTABLE + 0xFD0

typedef struct {
	union {
		uint32_t cpuid;
		struct {
			unsigned int Revision :4;		// p value in rnpn
			unsigned int PartNo :12;        //
			unsigned int Architecture :4;	// ID051414 page 1650, cm0+ trm
			unsigned int Variant :4;			// r value in rnpn
			unsigned int Implementer :8;
		};
	};

} cpuid_t;



// partno  ( I believe this was from the Cortex M3 docs)
// I haven't located some more details, like the reserved bits
// However, they're set on for example the M33. M33 uses a
// modified scheme, as obviouosly 33 can't be encoded with 4 bits.
// I've only found the list of values, rather then the breakdown
// of their parts. So far I've only found it in CM3 docs, other
// CMx docs seem only to mention the expected value.
//  [11:10] b11 = Cortex family
//  [ 9: 8] b00 = version
//  [ 7: 6] b00 = reserved
//  [ 5: 4] b10 = M (v7-M)
//  [ 3: 0] X = family member. Cortex-M3 family is b0011.
//
// https://www.sciencedirect.com/topics/engineering/system-control-block
//
// CM0  0xC20
// CMO+ 0xC60
// CM1  0xC21
// CM3  0xC23
// CM4  0xC24
// CM7  0xC27

// CM23
// CM33 0xD21

// Architecture
//-------// ID051414
// 0x1 ARMv4
// 0x2 ARMv4T
// 0x3 ARMv5 (obsolete)
// 0x4 ARMv5T
// 0x5 ARMv5TE
// 0x6 ARMv5TEJ
// 0x7 ARMv6
// 0xF Defined by CPUID scheme
//
// 0xC	ARMv6m  // Cortex m0+ trm

typedef enum {
	JAZELLE_ABSENT = 0b0000,
	JAZELLE_PRESENT_NOCLEAR = 0b0001,
	JAZELLE_PRESENT_CLEAR = 0b0010,
} pfr0_state2_t;

typedef enum {
	THUMB_ABSENT = 0b0000, THUMB_1_PRESENT = 0b0001, THUMB_2_PRESENT = 0b0011,
} pfr0_state1_t;

typedef enum {
	ARM_ABSENT = 0b0000, ARM_PRESENT = 0b0001,
} pfr0_state0_t;

typedef struct {
	unsigned int State0 :4;	// ARM Support
	unsigned int State1 :4;	// Thumb Support
	unsigned int State2 :4;	// Jazelle Support
	unsigned int State3 :4;	// ThumbEE SUpport
	unsigned int Reserved :16;
} pfr0_t;

typedef struct {
	unsigned int Programmers_model :4;
	unsigned int Security_Extensions :4;
	unsigned int M_profile_programmers_model :4;
	unsigned int Virtualization_Extensions :4;
	unsigned int Generic_Timer :4;
} pfr1_t;


typedef struct {
	union {
		uint64_t pid;
		struct {
			unsigned int partno :12;
			unsigned int identity_code :7;
			unsigned int jep106_used :1;
			unsigned int revision :4;
			unsigned int customer_modified :4;
			unsigned int revand :4;
			unsigned int continuation_code :4;
			unsigned int four_kb_cout :4;
			unsigned int reserved :24;
		};
	};
} romtable_pid_t;
romtable_pid_t extract_romtable_pid(romtable_id_t* rid) ;

#endif /* LIBHALGLUE_COMMON_ARM_CPUID_H_ */
