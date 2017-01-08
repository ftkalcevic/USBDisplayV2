/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdio.h>
#include <asf.h>
#include "common.h"
#include "touch.h"
#include "lcdtext.h"
#include "lcd_conf.h"
#include "lcdiface_hx8352a.h"
#include "lcdiface_ssd1963.h"
#include "lcdtext.h"

#include "../fonts/Font6x13.h"
//#include "../fonts/FontTypewriter.h"

static struct SFontData  fontData[] =
{
	/*45x17*/    { FONT6X13_WIDTH, FONT6X13_HEIGHT, SCREEN_WIDTH / FONT6X13_WIDTH, SCREEN_HEIGHT / FONT6X13_HEIGHT, Font6x13 },
	//	/*22x9 */    { FONTTYPEWRITER_WIDTH, FONTTYPEWRITER_HEIGHT, SCREEN_WIDTH / FONTTYPEWRITER_WIDTH, SCREEN_HEIGHT / FONTTYPEWRITER_HEIGHT, FontTypewriter },
};


TLCD lcd;
TLCDText lcdtext(lcd, fontData, countof(fontData) );

uint8_t usb_serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH];

static void init_usb_serial_number(void)
{
	volatile uint8_t *ptr_ID = AVR32_FLASHC_SERIAL_DATA;
			
	int n = 0;
	for (int i=0; i<SERIAL_NUMBER_BITS/8;i++)
	{
		#define HEX(x)	( (x) < 10 ? (x) + '0' : (x) + 'A' - 10 )
		uint8_t id = *(ptr_ID++);
		usb_serial_number[n++] = HEX( ((id >> 4) & 0x0f) );
		usb_serial_number[n++] = HEX( (id & 0x0f) );
	}
}



static void DisplayStartupMsg()
{
	lcdtext.SetAutoScroll(false);
	lcdtext.SetTextCursor(0,0);
	char s[200];
	snprintf( s, sizeof(s), "%s %s %d.%d\n", USB_DEVICE_PRODUCT_NAME, USB_DEVICE_MANUFACTURE_NAME, USB_DEVICE_MAJOR_VERSION, USB_DEVICE_MINOR_VERSION );
	s[sizeof(s)-1] = '\0';
	lcdtext.WriteString( s );
}


int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
    irq_initialize_vectors();
    cpu_irq_enable();

	sysclk_init();
	board_init();

	lcd.Init();
	touch_init();
	sleepmgr_init();
	init_usb_serial_number();
	udc_start();
	udc_attach();

	lcd.ClearScreen(0);
	DisplayStartupMsg();

	// Force read for the first time to reset the touch controller
	touch_init_read();

	int n = 0;
	for (;;)
	{

		if ( touch_busy() == 0 )
		{
			if ( gpio_get_pin_value(TOUCH_nPENIRQ) == 0 )
			{
				touch_init_read();
				n = 0;
			}
		}
		else if ( touch_complete() )
		{
			udi_hid_touch_move( touch_x, touch_y, (touch_z1+touch_z2)/2 );
			udi_cdc_putc(HEX((touch_x>>12)&0xf));
			udi_cdc_putc(HEX((touch_x>>8)&0xf));
			udi_cdc_putc(HEX((touch_x>>4)&0xf));
			udi_cdc_putc(HEX((touch_x>>0)&0xf));
			udi_cdc_putc('\n');
		}

		while ( udi_cdc_is_rx_ready() )
		{
			
			int c = udi_cdc_getc();
			udi_cdc_putc('-'); udi_cdc_putc(c);
		}
	}
}

/*
							iFace  EP		SIZE
CTRL						-      0		64
UDI_CDC_DATA_EP_IN_0        1      1
UDI_CDC_DATA_EP_OUT_0       1      2
UDI_CDC_COMM_EP_0           0      3
UDI_HID_MOUSE_EP_IN			2	   4
UDI_HID_GENERIC_EP_OUT		3	   5	
UDI_HID_GENERIC_EP_IN		3	   6
UDI_VENDOR_EP_BULK_OUT		4	   7

*/