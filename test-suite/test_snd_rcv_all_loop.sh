#!/bin/bash

# 01 Sony needs address greater 0x0100
# 06 Recs80 3 address bits
# 08 Denon only even commands
# 0c Recs80ext 4 address bits
# 0d Nubert 0 address bits
# 0e B&O 455kHz, 0 address bits
# 0f Grundig 0 address bits
# 11 Siemens last bit inverted to last but one
# 12 FDC special
# 13 RCCAR 2 address bits
# 14 JVC 4 address bits
# 16 Nikon 0 address bits, 2 data bits
# 17 Ruwido conflicts with denon, 56kHz
# 18 IR-60, 0 address bits
# 19 Kathrein can't send
# 1a Netbox can't send
# 1d Lego 0 address bits
# 1e Thomson 4 address bits
# 1f Bose 0 address bits
# 20 A1TVBox special
# 21 Ortek can't send
# 22 Telefunken 0 address bits
# 23 Roomba conflicts with RC6
# 24 RCMM32 can't send
# 25 RCMM24 can't send
# 26 RCMM12 can't send
# 27 Speaker 0 address bits
# 29 Samsung48 double
# 2a Merlin
# 2b Pentax max 16kHz
# 2c Fan conflicts with Nubert
# 2d S100 conflicts with RC5, can't send
# 2e ACP24 conflicts with Denon
# 2f Technics can't send
# 30 Panasonic conflicts with Kaseikyo and Mitsu-Heavy 
# 31 Mitsu-Heavy conflicts with Kaseikyo and Panasonic 
# 3c Melinera 0 address bits
for i in $(seq 0 199); do
for irdata in	01081f003f01 \
		02001f003f01 \
		03001f003f01 \
		04001f003f01 \
		05001f003f01 \
		06001f003f01 \
		07001f003f01 \
		08001f003e01 \
		09001f003f01 \
		0a001f003f01 \
		0b001f003f01 \
		0c001f003f01 \
		0d001f003f01 \
		0f001f003f01 \
		10001f003f01 \
		11001f003e01 \
		12003f002f01 \
		13001f003f01 \
		14001f003f01 \
		15001f003f01 \
		16001f003f01 \
		18001f003f01 \
		1b001f003f01 \
		1c001f003f01 \
		1d001f003f01 \
		1e001f003f01 \
		1f001f003f01 \
		20004f003f01 \
		22001f003f01 \
		27001f003f01 \
		28001f003f01 \
		29001f003f01 \
		3c001f003f01 ;
do
	echo "${irdata}"
	./irctl -dstm32 -e -s0x${irdata} /dev/hidraw1 > /dev/null 2>&1
	sleep 0.3
done
done