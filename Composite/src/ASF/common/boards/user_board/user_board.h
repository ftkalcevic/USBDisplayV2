/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
#define BOARD_XOSC_HZ          12000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
#define BOARD_XOSC_STARTUP_US  500000

#define LCD_BACKLIGHT_PWM	AVR32_PIN_PX14



#define LCDD0			AVR32_PIN_PX10
#define LCDD1			AVR32_PIN_PX09
#define LCDD2			AVR32_PIN_PX08
#define LCDD3			AVR32_PIN_PX07
#define LCDD4			AVR32_PIN_PX06
#define LCDD5			AVR32_PIN_PX05
#define LCDD6			AVR32_PIN_PX04
#define LCDD7			AVR32_PIN_PX03

#define LCDD8			AVR32_PIN_PX02
#define LCDD9			AVR32_PIN_PX01
#define LCDD10			AVR32_PIN_PX00
#define LCDD11			AVR32_PIN_PX39
#define LCDD12			AVR32_PIN_PX38
#define LCDD13			AVR32_PIN_PX37
#define LCDD14			AVR32_PIN_PX36
#define LCDD15			AVR32_PIN_PX35

#define LCDReset		AVR32_PIN_PX15
//#define LCDID			-1
//#define LCDFMark		AVR32_PIN_PX47
#define LCDRS			AVR32_PIN_PX56
#define LCDRD			AVR32_PIN_PX13
#define LCDWR			AVR32_PIN_PX12



#define EBI_D00			LCDD0
#define EBI_D01			LCDD1
#define EBI_D02			LCDD2
#define EBI_D03			LCDD3
#define EBI_D04			LCDD4
#define EBI_D05			LCDD5
#define EBI_D06			LCDD6
#define EBI_D07			LCDD7

#define EBI_D08			LCDD8
#define EBI_D09			LCDD9
#define EBI_D10			LCDD10
#define EBI_D11			LCDD11
#define EBI_D12			LCDD12
#define EBI_D13			LCDD13
#define EBI_D14			LCDD14
#define EBI_D15			LCDD15

#define EBI_ADDR21		LCDRS
#define EBI_NRD			LCDRD
#define EBI_NWE0		LCDWR

#define LCD_COMMAND_ADDRESS		(AVR32_EBI_CS0_ADDRESS + (1<<22))
#define LCD_DATA_ADDRESS		(AVR32_EBI_CS0_ADDRESS + (1<<21) )

#define LCD_COMMAND				((volatile uint16_t *)LCD_COMMAND_ADDRESS)
#define LCD_DATA				((volatile uint16_t *)LCD_DATA_ADDRESS)


#define TOUCH_DCLK          AVR32_PIN_PA15
#define TOUCH_DOUT          AVR32_PIN_PA16
#define TOUCH_nCS           AVR32_PIN_PA17
#define TOUCH_DIN           AVR32_PIN_PA19
#define TOUCH_BUSY          AVR32_PIN_PX40
#define TOUCH_nPENIRQ       AVR32_PIN_PX53

#define RS232_TX            AVR32_PIN_PA04
#define RS232_RX            AVR32_PIN_PA03
#define DEVICE_USART        AVR32_USART0




#endif // USER_BOARD_H
