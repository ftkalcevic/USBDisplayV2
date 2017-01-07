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
#include <asf.h>
#include "touch.h"


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

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
    irq_initialize_vectors();
    cpu_irq_enable();

	sysclk_init();
	board_init();
	touch_init();
	sleepmgr_init();
	init_usb_serial_number();
	udc_start();
	udc_attach();

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