#include "stm32_tm1637.h"
#include "stm32f10x.h"
// Internal Functions
void _tm1637Start(void);
void _tm1637Stop(void);
void _tm1637ReadResult(void);
void _tm1637WriteByte(unsigned char b);
uint32_t tm1637_TM_DELAY_Init(void);
uint8_t char2segments(char c);
extern volatile unsigned int dim_delay;
uint8_t dim_init = 4;

const char segmentMap[] = { (SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F), //0
		(SEG_B + SEG_C), //1
		(SEG_A + SEG_B + SEG_D + SEG_E + SEG_G), //2
		(SEG_A + SEG_B + SEG_C + SEG_D + SEG_G), //3
		(SEG_B + SEG_C + SEG_F + SEG_G), //4
		(SEG_A + SEG_C + SEG_D + SEG_F + SEG_G), //5
		(SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G), //6
		(SEG_A + SEG_B + SEG_C + SEG_F), //7
		(SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G), //8
		(SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G), //9
		(SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G), //A
		(SEG_C + SEG_D + SEG_E + SEG_F + SEG_G), //b
		(SEG_A + SEG_D + SEG_E + SEG_F), //C
		(SEG_B + SEG_C + SEG_D + SEG_E + SEG_G), //d
		(SEG_A + SEG_D + SEG_E + SEG_F + SEG_G), //E
		(SEG_A + SEG_E + SEG_F + SEG_G), //F
		0x00 }; // None
uint8_t _dig1 = None;

void tm1637Init(void) {
	tm1637_TM_DELAY_Init();
	CLK_PORT_CLK_ENABLE;
	DIO_PORT_CLK_ENABLE;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // ??
	GPIO_InitStruct.GPIO_Pin = CLK_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(CLK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = DIO_PIN;
	GPIO_Init(DIO_PORT, &GPIO_InitStruct);

	tm1637DisplayChar(char2segments('I'), 0);
	tm1637DisplayChar(char2segments('R'), 1);
	tm1637DisplayChar(char2segments('M'), 2);
	tm1637DisplayChar(char2segments('P'), 3);
	tm1637SetBrightness(dim_init);
	dim_delay = dim_init;
}

void tm1637DisplayChar(int ch, uint8_t column) {
	//coppy char to memory
	if (column == 1)
		_dig1 = ch;

	_tm1637Start();
	_tm1637WriteByte(WriteDataToDispReg);
	_tm1637ReadResult();
	_tm1637Stop();

	_tm1637Start();
	_tm1637WriteByte(AdressCmd + column);
	_tm1637ReadResult();

	_tm1637WriteByte(ch);
	_tm1637ReadResult();

	_tm1637Stop();

}

void tm1637DisplayDigit(int ch, uint8_t column) {

	tm1637DisplayChar(segmentMap[ch], column);
}
void tm1637ShowColon(uint8_t show) {

	_tm1637Start();
	_tm1637WriteByte(WriteDataToDispReg);
	_tm1637ReadResult();
	_tm1637Stop();

	_tm1637Start();
	_tm1637WriteByte(AdressCmd + 1);
	_tm1637ReadResult();

	if (show) //enable colon +128 to value
		_dig1 += 128;
	_tm1637WriteByte(_dig1);

	_tm1637ReadResult();
	_tm1637Stop();
}

void tm1637DisplayDecimal(int v, int displaySeparator) {
	unsigned char digitArr[4];
	for (int i = 0; i < 4; ++i) {
		digitArr[i] = segmentMap[v % 10];
		if (i == 2 && displaySeparator) {
			digitArr[i] |= 1 << 7;
		}
		v /= 10;
	}

	_tm1637Start();
	_tm1637WriteByte(WriteDataToDispReg);
	_tm1637ReadResult();
	_tm1637Stop();

	_tm1637Start();
	_tm1637WriteByte(AdressCmd);
	_tm1637ReadResult();

	for (int i = 0; i < 4; ++i) {
		_tm1637WriteByte(digitArr[3 - i]);
		_tm1637ReadResult();
	}

	_tm1637Stop();
}

void tm1637DisplayHex(uint16_t v)
{
	unsigned char digitArr[4];
	for (int i = 0; i < 4; ++i) {
		digitArr[i] = segmentMap[v&0xF];
		v >>= 4;
	}

	_tm1637Start();
	_tm1637WriteByte(WriteDataToDispReg);
	_tm1637ReadResult();
	_tm1637Stop();

	_tm1637Start();
	_tm1637WriteByte(AdressCmd);
	_tm1637ReadResult();

	for (int i = 0; i < 4; ++i) {
		_tm1637WriteByte(digitArr[3 - i]);
		_tm1637ReadResult();
	}

	_tm1637Stop();
	tm1637SetBrightness(dim_init);
	dim_delay = dim_init;

}

// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(char brightness) {
	// Brightness command:
	// 1000 0XXX = display off
	// 1000 1BBB = display on, brightness 0-7
	// X = don't care
	// B = brightness
	_tm1637Start();
	_tm1637WriteByte(DisplCntrl + brightness);
	_tm1637ReadResult();
	_tm1637Stop();
}

void _tm1637Start(void) {
	_tm1637ClkHigh
	_tm1637DioHigh
	_tm1637Delay(5);
	_tm1637DioLow
}

void _tm1637Stop(void) {
	_tm1637ClkLow
	_tm1637Delay(10);
	_tm1637DioLow
	_tm1637Delay(10);
	_tm1637ClkHigh
	_tm1637Delay(10);
	_tm1637DioHigh
}

void _tm1637ReadResult(void) {
	_tm1637ClkLow
	_tm1637Delay(10);
	// while (dio); // We're cheating here and not actually reading back the response.
	_tm1637ClkHigh
	_tm1637Delay(10);
	_tm1637ClkLow
}

void _tm1637WriteByte(unsigned char b) {
	for (int i = 0; i < 8; ++i) {
		_tm1637ClkLow
		if (b & 0x01) {
			_tm1637DioHigh
		} else {
			_tm1637DioLow
		}
		_tm1637Delay(10);
		b >>= 1;
		_tm1637ClkHigh
		_tm1637Delay(10);
	}
}

uint32_t tm1637_TM_DELAY_Init(void) {
#if !defined(STM32F0xx)
	uint32_t c;

	/* Enable TRC */
	CoreDebug->DEMCR &= ~0x01000000;
	CoreDebug->DEMCR |= 0x01000000;

	/* Enable counter */
	DWT->CTRL &= ~0x00000001;
	DWT->CTRL |= 0x00000001;

	/* Reset counter */
	DWT->CYCCNT = 0;

	/* Check if DWT has started */
	c = DWT->CYCCNT;

	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c);
#else
	/* Return OK */
	return 1;
#endif
}

uint8_t char2segments(char c) {
	switch (c) {
		case '0' : return 0x3f;
		case '1' : return 0x06;
		case '2' : return 0x5b;
		case '3' : return 0x4f;
		case '4' : return 0x66;
		case '5' : return 0x6d;
		case '6' : return 0x7d;
		case '7' : return 0x07;
		case '8' : return 0x7f;
		case '9' : return 0x6f;
		case '_' : return 0x08;
		case '^' : return 0x01; // ¯
		case '-' : return 0x40;
		case '*' : return 0x63; // °
		case ' ' : return 0x00; // space
		case 'A' : return 0x77; // upper case A
		case 'a' : return 0x5f; // lower case a
		case 'B' :		  // lower case b
		case 'b' : return 0x7c; // lower case b
		case 'C' : return 0x39; // upper case C
		case 'c' : return 0x58; // lower case c
		case 'D' :		  // lower case d
		case 'd' : return 0x5e; // lower case d
		case 'E' :		  // upper case E
		case 'e' : return 0x79; // upper case E
		case 'F' :		  // upper case F
		case 'f' : return 0x71; // upper case F
		case 'G' :		  // upper case G
		case 'g' : return 0x35; // upper case G
		case 'H' : return 0x76; // upper case H
		case 'h' : return 0x74; // lower case h
		case 'I' : return 0x06; // 1
		case 'i' : return 0x04; // lower case i
		case 'J' : return 0x1e; // upper case J
		case 'j' : return 0x16; // lower case j
		case 'K' :		  // upper case K
		case 'k' : return 0x75; // upper case K
		case 'L' :		  // upper case L
		case 'l' : return 0x38; // upper case L
		case 'M' :		  // twice tall n
		case 'm' : return 0x37; // twice tall ∩
		case 'N' :		  // lower case n
		case 'n' : return 0x54; // lower case n
		case 'O' :		  // lower case o
		case 'o' : return 0x5c; // lower case o
		case 'P' :		  // upper case P
		case 'p' : return 0x73; // upper case P
		case 'Q' : return 0x7b; // upper case Q
		case 'q' : return 0x67; // lower case q
		case 'R' :		  // lower case r
		case 'r' : return 0x50; // lower case r
		case 'S' :		  // 5
		case 's' : return 0x6d; // 5
		case 'T' :		  // lower case t
		case 't' : return 0x78; // lower case t
		case 'U' :		  // lower case u
		case 'u' : return 0x1c; // lower case u
		case 'V' :		  // twice tall u
		case 'v' : return 0x3e; // twice tall u
		case 'W' : return 0x7e; // upside down A
		case 'w' : return 0x2a; // separated w
		case 'X' :		  // upper case H
		case 'x' : return 0x76; // upper case H
		case 'Y' :		  // lower case y
		case 'y' : return 0x6e; // lower case y
		case 'Z' :		  // separated Z
		case 'z' : return 0x1b; // separated Z
	}
        return 0;
}
