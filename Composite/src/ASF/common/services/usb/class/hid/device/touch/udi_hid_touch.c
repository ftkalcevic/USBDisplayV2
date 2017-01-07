/**
 * \file
 *
 * \brief USB Device Human Interface Device (HID) mouse interface.
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_hid.h"
#include "udi_hid_touch.h"
#include <string.h>

/**
 * \ingroup udi_hid_touch_group
 * \defgroup udi_hid_touch_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */
bool udi_hid_touch_enable(void);
void udi_hid_touch_disable(void);
bool udi_hid_touch_setup(void);
uint8_t udi_hid_touch_getsetting(void);

//! Global structure which contains standard UDI interface for UDC
UDC_DESC_STORAGE udi_api_t udi_api_hid_touch = {
	.enable = (bool(*)(void))udi_hid_touch_enable,
	.disable = (void (*)(void))udi_hid_touch_disable,
	.setup = (bool(*)(void))udi_hid_touch_setup,
	.getsetting = (uint8_t(*)(void))udi_hid_touch_getsetting,
	.sof_notify = NULL,
};
//@}


/**
 * \ingroup udi_hid_touch_group
 * \defgroup udi_hid_touch_group_internal Implementation of UDI HID touch
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Internal defines and variables to manage HID touch
 */
//@{

//! Size of report for standard HID touch
#define UDI_HID_TOUCH_REPORT_SIZE   9
//! To store current rate of HID touch
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_touch_rate;
//! To store current protocol of HID touch
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_touch_protocol;
//! To signal if a valid report is ready to send
static bool udi_hid_touch_b_report_valid;
//! Report ready to send
static uint8_t udi_hid_touch_report[UDI_HID_TOUCH_REPORT_SIZE];
//! Signal if a report transfer is on going
static bool udi_hid_touch_report_trans_ongoing;
//! Buffer used to send report
COMPILER_WORD_ALIGNED
		static uint8_t
		udi_hid_touch_report_trans[UDI_HID_TOUCH_REPORT_SIZE];


/**
 * \brief Callback for set report setup request
 *
 * \return \c 1 always, because it is not used on touch interface
 */
static bool udi_hid_touch_setreport(void);

//@}

//! HID report descriptor for standard HID touch
UDC_DESC_STORAGE udi_hid_touch_report_desc_t udi_hid_touch_report_desc = {
	{
    0x05, 0x0d,                    // USAGE_PAGE (Digitizers)
    0x09, 0x04,                    // USAGE (Touch Screen)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x09, 0x20,                    //   USAGE (Stylus)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x09, 0x30,                    //     USAGE (X)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x09, 0x31,                    //     USAGE (Y)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x0d,                    //     USAGE_PAGE (Digitizers)
    0x09, 0x30,                    //     USAGE (Tip Pressure)
    0x81, 0x00,                    //     INPUT (Data,Ary,Abs)
    0x09, 0x42,                    //     USAGE (Tip Switch)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x0f,                    //     REPORT_SIZE (15)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION

	}
};

/**
 * \name Internal routines
 */
//@{


/**
 * \brief Send the report
 *
 * \return \c 1 if send on going, \c 0 if delay.
 */
static bool udi_hid_touch_send_report(void);

/**
 * \brief Callback called when the report is sent
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer finish
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer aborted
 * \param nb_sent    number of data transfered
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
static void udi_hid_touch_report_sent(udd_ep_status_t status,
		iram_size_t nb_sent, udd_ep_id_t ep);

//@}


//--------------------------------------------
//------ Interface for UDI HID level

bool udi_hid_touch_enable(void)
{
	// Initialize internal value
	udi_hid_touch_rate = 0;
	udi_hid_touch_protocol = 0;
	udi_hid_touch_report_trans_ongoing = false;
	memset(udi_hid_touch_report, 0, UDI_HID_TOUCH_REPORT_SIZE);
	udi_hid_touch_b_report_valid = false;
	return UDI_HID_TOUCH_ENABLE_EXT();
}


void udi_hid_touch_disable(void)
{
	UDI_HID_TOUCH_DISABLE_EXT();
}


bool udi_hid_touch_setup(void)
{
	return udi_hid_setup(&udi_hid_touch_rate,
						&udi_hid_touch_protocol,
						(uint8_t *) &udi_hid_touch_report_desc,
						udi_hid_touch_setreport);
}


uint8_t udi_hid_touch_getsetting(void)
{
	return 0;
}


static bool udi_hid_touch_setreport(void)
{
	return false;
}


bool udi_hid_touch_move(uint16_t x, uint16_t y, uint16_t pressure)
{
	irqflags_t flags = cpu_irq_save();
	uint8_t *p = (uint8_t *)udi_hid_touch_report;
	*(p++) = 1;

	*(p++) = (x >> 8) & 0xFF;
	*(p++) = x & 0xFF;
	*(p++) = (y >> 8) & 0xFF;
	*(p++) = y & 0xFF;
	*(p++) = (pressure >> 8) & 0xFF;
	*(p++) = pressure & 0xFF;
	*(p++) = 1;

	// Valid and send report
	udi_hid_touch_b_report_valid = true;
	udi_hid_touch_send_report();

	cpu_irq_restore(flags);
	return true;
}



static bool udi_hid_touch_send_report(void)
{
	if (udi_hid_touch_report_trans_ongoing)
		return false;	// Transfer on going then send this one after transfer complete

	// Copy report on other array used only for transfer
	memcpy(udi_hid_touch_report_trans, udi_hid_touch_report,
			UDI_HID_TOUCH_REPORT_SIZE);
	udi_hid_touch_b_report_valid = false;

	// Send report
	udi_hid_touch_report_trans_ongoing =
			udd_ep_run(	UDI_HID_TOUCH_EP_IN,
							false,
							udi_hid_touch_report_trans,
							UDI_HID_TOUCH_REPORT_SIZE,
							udi_hid_touch_report_sent);
	return udi_hid_touch_report_trans_ongoing;
}


static void udi_hid_touch_report_sent(udd_ep_status_t status,
		iram_size_t nb_sent, udd_ep_id_t ep)
{
	UNUSED(ep);
	UNUSED(status);
	UNUSED(nb_sent);
	// Valid report sending
	udi_hid_touch_report_trans_ongoing = false;
	if (udi_hid_touch_b_report_valid) {
		// Send new valid report
		udi_hid_touch_send_report();
	}
}

//@}
