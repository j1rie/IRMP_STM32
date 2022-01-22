### STM32 TM1637 7-segment LED display library
Its forked from **rogerdahl**, but with small improvments.

This a tiny library to write to a TM1637 7-segment LED display from an STM32 MCU. Porting to other MCUs should be simple. Tested on [STM32F429I-DISCO] but should works also with others STM32Fxx MCU. Based on the code example in the [TM1637 datasheet](http://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf).

#### Usage

Example:

```
    tm1637Init(); //Init the Display 
    tm1637SetBrightness(3); // Set brightness to lvl 3
    tm1637DisplayDecimal(1234, 1); // Display the value "1234" and turn on the `:` that is between digits 2 and 3.
    tm1637DisplayDigit(0,0); //Display Digit 0 in column 0 
    tm1637DisplayDigit(None,0); //Turn off display in column 0
    
```

The `#define CLK_PORT GPIOC`
`#define DIO_PORT GPIOC`
`#define CLK_PIN GPIO_PIN_0`
`#define DIO_PIN GPIO_PIN_1` statements in `stm32_tm1637.h` must be updated to match the actual port and pins to which the display is connected.

The library takes a shortcut in that it does not read back or act on the status values returned by the display. This could be a problem for use cases in which the display is written to only rarely.
