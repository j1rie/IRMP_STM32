#ifndef STM32_TM1637_H_
#define STM32_TM1637_H_

#include "irmpmain.h"
#include "stm32f10x.h"

/**************** START CONFIGURATION HERE **********************/
/******** CHOSE PORT AND PIN FOR DATA AND CLOCK *****************/
#define CLK_PORT GPIOB
#define DIO_PORT GPIOB
#define CLK_PIN GPIO_Pin_4
#define DIO_PIN GPIO_Pin_11
/**************** END OF CONFIGURATION *************************/

// Internal of Configuration DO NOT EDIT
#define CLK_PORT_CLK_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#define DIO_PORT_CLK_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

#define _tm1637ClkHigh CLK_PORT->BSRR = CLK_PIN;
#define _tm1637ClkLow CLK_PORT->BSRR = ((uint32_t)CLK_PIN << 16U);
#define _tm1637DioHigh  DIO_PORT->BSRR = DIO_PIN;
#define _tm1637DioLow DIO_PORT->BSRR = ((uint32_t)DIO_PIN << 16U);
/********************************************************************/

#define None 16
#define SEG_A 1
#define SEG_B 2
#define SEG_C 4
#define SEG_D 8
#define SEG_E 16
#define SEG_F 32
#define SEG_G 64

///// DEFINE REGISTERS
#define WriteDataToDispReg 0x40
#define AdressCmd 0xc0
#define DisplCntrl 0x87

void tm1637Init(void);
void tm1637DisplayDecimal(int v, int displaySeparator);
void tm1637DisplayHex(uint16_t v);
void tm1637SetBrightness(char brightness);
void tm1637DisplayDigit (int ch, uint8_t column);
void tm1637ShowColon (uint8_t show);
void tm1637DisplayChar (int ch, uint8_t column);

extern const char segmentMap[];  //you can use segmentMap in main.c

__STATIC_INLINE void _tm1637Delay(__IO uint32_t micros) {

#if !defined(STM32F0xx)
	uint32_t start = DWT->CYCCNT;

	/* Go to number of cycles for system */
	micros *= (SysCtlClockGet() / 1000000);

	/* Delay till end */
	while ((DWT->CYCCNT - start) < micros);
#else
	/* Go to clock cycles */
	micros *= (SystemCoreClock / 1000000) / 5;

	/* Wait till done */
	while (micros--);
#endif
}

#endif
