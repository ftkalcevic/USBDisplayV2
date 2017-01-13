/**
 * \file
 *
 * \brief USB Vendor class interface.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_vendor.h"
#include "udd.h"
#include "udc.h"
#include "udi_vendor.h"
#include <string.h>
#include "lcd_conf.h"
#include "displaycommands.h"

/**
 *
 * Structures and functions required by UDC.
 *
 * @{
 */

static bool udi_lcd_rx_start(void);
static bool udi_lcd_is_rx_ready(void);
static void udi_lcd_data_recevied(udd_ep_status_t status, iram_size_t n);

bool udi_vendor_enable(void);
void udi_vendor_disable(void);
bool udi_vendor_setup(void);
uint8_t udi_vendor_getsetting(void);

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_vendor = {
	.enable = udi_vendor_enable,
	.disable = udi_vendor_disable,
	.setup = udi_vendor_setup,
	.getsetting = udi_vendor_getsetting,
	.sof_notify = NULL,
};
//@}

#define UDI_LCD_RX_BUFFERS     (2)

#define NEXT_BUFFER(idx) ((idx==0)?1:0)
//#define NEXT_BUFFER(idx) ((idx==UDI_LCD_RX_BUFFERS-1)?0:idx+1)

//! Status of LCD interface
static volatile bool udi_lcd_running;

typedef union _RXBuffer
{
	CmdAll cmd;
	uint8_t buf[UDI_VENDOR_EPS_SIZE_BULK_HS];
} RXBuffer;

//! Buffer to receive data
COMPILER_WORD_ALIGNED static RXBuffer rx_buffer[UDI_LCD_RX_BUFFERS];
//! Data available in RX buffers
static uint16_t udi_lcd_rx_buf_nb[UDI_LCD_RX_BUFFERS];
//! Give the current RX buffer used (rx0 if 0, rx1 if 1)
static volatile uint8_t udi_lcd_rx_buf_sel;
//! Read position in current RX buffer
static volatile uint16_t udi_lcd_rx_pos;
static volatile uint32_t udi_lcd_blt_bytes_to_go;
//! Signal a transfer on-going
static volatile bool udi_lcd_rx_trans_ongoing;

/**
 * Class internal implementation
 * @{
 */


/**
 * \name Internal routines
 */
//@{
bool udi_vendor_enable(void)
{
	// Initialize RX management
	udi_lcd_rx_trans_ongoing = false;
	udi_lcd_rx_buf_sel = 0;
	udi_lcd_rx_buf_nb[0] = 0;
	udi_lcd_rx_pos = 0;
	udi_lcd_blt_bytes_to_go = 0;
	udi_lcd_running = udi_lcd_rx_start();
	return udi_lcd_running;
}


void udi_vendor_disable(void)
{
}


bool udi_vendor_setup(void)
{
	return false; // Not supported request
}

uint8_t udi_vendor_getsetting(void)
{
	return 0;
}
//@}




static bool udi_lcd_rx_start(void)
{
	irqflags_t flags;
	uint8_t buf_sel_trans;

	flags = cpu_irq_save();
	buf_sel_trans = udi_lcd_rx_buf_sel;
	if (udi_lcd_rx_trans_ongoing ||
	(udi_lcd_rx_pos < udi_lcd_rx_buf_nb[buf_sel_trans])) {
		// Transfer already on-going or current buffer no empty
		cpu_irq_restore(flags);
		return false;
	}

	// Change current buffer
	udi_lcd_rx_pos = 0;
	udi_lcd_rx_buf_sel = NEXT_BUFFER(buf_sel_trans);

	// Start transfer on RX
	udi_lcd_rx_trans_ongoing = true;
	cpu_irq_restore(flags);
	
	//if (udi_lcd_is_rx_ready()) {
		//UDI_LCD_RX_NOTIFY();
	//}

	if ( udi_lcd_blt_bytes_to_go == 0 )
	{
		// read next command
		return udd_ep_run( UDI_VENDOR_EP_BULK_OUT,
							true,
							rx_buffer[buf_sel_trans].buf,
							UDI_VENDOR_EPS_SIZE_BULK_HS,
							udi_lcd_data_recevied);
	}
	else
	{
		// copy data into graphics memory
		return udd_ep_run( UDI_VENDOR_EP_BULK_OUT,
							true,
							LCD_DATA,
							UDI_VENDOR_EPS_SIZE_BULK_HS,
							udi_lcd_data_recevied);
	}
}


static void udi_lcd_data_recevied(udd_ep_status_t status, iram_size_t n)
{
	uint8_t buf_sel_trans;

	if (UDD_EP_TRANSFER_OK != status) {
		// Abort reception
		return;
	}
	
	buf_sel_trans = NEXT_BUFFER(udi_lcd_rx_buf_sel);
	if ( udi_lcd_blt_bytes_to_go == 0 )
	{
		// New cmd
		if ( rx_buffer[buf_sel_trans].cmd.blt.cmd == CMD_BITBLT )
		{
			udi_lcd_blt_bytes_to_go = rx_buffer[buf_sel_trans].cmd.blt.width;
			udi_lcd_blt_bytes_to_go *= rx_buffer[buf_sel_trans].cmd.blt.height;
			udi_lcd_blt_bytes_to_go *= 2;
			
			LCD_BltStart(rx_buffer[buf_sel_trans].cmd.bltrle.x, rx_buffer[buf_sel_trans].cmd.bltrle.y, rx_buffer[buf_sel_trans].cmd.bltrle.width, rx_buffer[buf_sel_trans].cmd.bltrle.height );
		}
		else if ( rx_buffer[buf_sel_trans].cmd.light.cmd == CMD_SET_BACKLIGHT && n >= sizeof(rx_buffer[buf_sel_trans].cmd.light) )
		{
			
			LCD_SetBacklight( rx_buffer[buf_sel_trans].cmd.light.intensity );
		}
		else if ( rx_buffer[buf_sel_trans].cmd.boot.cmd == CMD_BOOTLOADER  && n >= sizeof(rx_buffer[buf_sel_trans].cmd.boot) )
		{
			if ( rx_buffer[buf_sel_trans].cmd.boot.magic[0] == BOOTLOADER_MAGIC1 &&
			rx_buffer[buf_sel_trans].cmd.boot.magic[1] == BOOTLOADER_MAGIC2 &&
			rx_buffer[buf_sel_trans].cmd.boot.magic[2] == BOOTLOADER_MAGIC3 &&
			rx_buffer[buf_sel_trans].cmd.boot.magic[3] == BOOTLOADER_MAGIC4 &&
			rx_buffer[buf_sel_trans].cmd.boot.magic[4] == BOOTLOADER_MAGIC5 )
			{
				// set the ISP_FORCE bit. Force the Watchdog to trigger
				flashc_erase_gp_fuse_bit(31, true);
				flashc_write_gp_fuse_bit(31, true);

				cpu_irq_disable();
				wdt_opt_t opt = {
					.us_timeout_period = 1000000
				};
				wdt_enable(&opt);
				while(1);
			}
		}
	}
	else
	{
		// Receive data.  That should have been copied via DMA.
		udi_lcd_blt_bytes_to_go -= n;
	}

	udi_lcd_rx_buf_nb[buf_sel_trans] = 0;
	udi_lcd_rx_trans_ongoing = false;
	udi_lcd_rx_start();
}


bool udi_lcd_is_rx_ready(void)
{
	return (udi_lcd_rx_pos < udi_lcd_rx_buf_nb[udi_lcd_rx_buf_sel]);
}



