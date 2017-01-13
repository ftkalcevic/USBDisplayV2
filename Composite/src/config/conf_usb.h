/**
 * \file
 *
 * \brief USB configuration file
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             0x1C40
#define  USB_DEVICE_PRODUCT_ID            0x04DC
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 200 // Consumption on VBUS line (mA)
#define  USB_DEVICE_ATTR                  \
 (USB_CONFIG_ATTR_BUS_POWERED)
//		(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "Frank's Workshop"
#define  USB_DEVICE_PRODUCT_NAME          "USB Display"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF" // Disk SN for MSC

#define	 SERIAL_NUMBER_BITS						120
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER		usb_serial_number // Give serial array pointer
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH		(SERIAL_NUMBER_BITS/4) // Give size of serial array
extern uint8_t usb_serial_number[]; // Declare external serial array 

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
#define  USB_DEVICE_HS_SUPPORT
#endif
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
// #define  UDC_VBUS_EVENT(b_vbus_high)      user_callback_vbus_action(b_vbus_high)
// extern void user_callback_vbus_action(bool b_vbus_high);
// #define  UDC_SOF_EVENT()                  user_callback_sof_action()
// extern void user_callback_sof_action(void);
// #define  UDC_SUSPEND_EVENT()              user_callback_suspend_action()
// extern void user_callback_suspend_action(void);
// #define  UDC_RESUME_EVENT()               user_callback_resume_action()
// extern void user_callback_resume_action(void);
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
#define  UDC_GET_EXTRA_STRING(id,pStr,pLen)		 udc_get_extra_string(id,pStr,pLen)
extern bool udc_get_extra_string(uint8_t id, const uint8_t **pStr, uint8_t *pLen);


//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Number of interfaces for this device
#define  USB_DEVICE_NB_INTERFACE       5 // 1 or more

//! Total endpoint used by all interfaces
//! Note:
//! It is possible to define an IN and OUT endpoints with the same number on XMEGA product only
//! E.g. MSC class can be have IN endpoint 0x81 and OUT endpoint 0x01
#define  USB_DEVICE_MAX_EP             6 // 0 to max endpoint requested by interfaces
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */


 
/**
 * Configuration of Class Vendor interface (if used)
 * @{
 */

//! Interface callback definition
#define  UDI_VENDOR_ENABLE_EXT()          true
#define  UDI_VENDOR_DISABLE_EXT()
#define  UDI_VENDOR_SETUP_OUT_RECEIVED()  false
#define  UDI_VENDOR_SETUP_IN_RECEIVED()   false
/*
 * #define UDI_VENDOR_ENABLE_EXT() my_callback_vendor_enable()
 * extern bool my_callback_vendor_enable(void);
 * #define UDI_VENDOR_DISABLE_EXT() my_callback_vendor_disable()
 * extern void my_callback_vendor_disable(void);
 *
 * #define  UDI_VENDOR_SETUP_OUT_RECEIVED()  my_vendor_setup_out_received()
 * extern bool my_vendor_setup_out_received(void);
 * #define  UDI_VENDOR_SETUP_IN_RECEIVED()   my_vendor_setup_in_received()
 * extern bool my_vendor_setup_in_received(void);
 */

//! endpoints size for full speed
#define  UDI_VENDOR_EPS_SIZE_INT_FS    0
#define  UDI_VENDOR_EPS_SIZE_BULK_FS   64
#define  UDI_VENDOR_EPS_SIZE_ISO_FS    0

//! endpoints size for high speed
#define  UDI_VENDOR_EPS_SIZE_INT_HS    0
#define  UDI_VENDOR_EPS_SIZE_BULK_HS   512
#define  UDI_VENDOR_EPS_SIZE_ISO_HS    0

/**
 * USB Class Vendor low level configuration
 * In standalone these configurations are defined by the Class Vendor module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_VENDOR_EP_BULK_OUT      (1 | USB_EP_DIR_OUT)


//! Interface number
#define  UDI_VENDOR_IFACE_NUMBER     0
//@}
//@}


/**
 * Configuration of CDC interface (if used)
 * @{
 */

//! Number of communication port used (1 to 3)
#define  UDI_CDC_PORT_NB 1

//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)             true
#define  UDI_CDC_DISABLE_EXT(port)
#define  UDI_CDC_RX_NOTIFY(port)
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)
/*
 * #define UDI_CDC_ENABLE_EXT(port) my_callback_cdc_enable()
 * extern bool my_callback_cdc_enable(void);
 * #define UDI_CDC_DISABLE_EXT(port) my_callback_cdc_disable()
 * extern void my_callback_cdc_disable(void);
 * #define  UDI_CDC_RX_NOTIFY(port) my_callback_rx_notify(port)
 * extern void my_callback_rx_notify(uint8_t port);
 * #define  UDI_CDC_TX_EMPTY_NOTIFY(port) my_callback_tx_empty_notify(port)
 * extern void my_callback_tx_empty_notify(uint8_t port);
 * #define  UDI_CDC_SET_CODING_EXT(port,cfg) my_callback_config(port,cfg)
 * extern void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg);
 * #define  UDI_CDC_SET_DTR_EXT(port,set) my_callback_cdc_set_dtr(port,set)
 * extern void my_callback_cdc_set_dtr(uint8_t port, bool b_enable);
 * #define  UDI_CDC_SET_RTS_EXT(port,set) my_callback_cdc_set_rts(port,set)
 * extern void my_callback_cdc_set_rts(uint8_t port, bool b_enable);
 */

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
//#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoints' numbers used by single or first CDC port
#define  UDI_CDC_COMM_EP_0             (2 | USB_EP_DIR_IN)  // Notify endpoint
#define  UDI_CDC_DATA_EP_IN_0          (3 | USB_EP_DIR_IN)  // TX
#define  UDI_CDC_DATA_EP_OUT_0         (4 | USB_EP_DIR_OUT) // RX


//! Interface numbers used by single or first CDC port
#define  UDI_CDC_COMM_IFACE_NUMBER_0   1
#define  UDI_CDC_DATA_IFACE_NUMBER_0   2
//@}
//@}



/**
 * Configuration of HID touch interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_HID_TOUCH_ENABLE_EXT()       true
#define  UDI_HID_TOUCH_DISABLE_EXT()
// #define UDI_HID_TOUCH_ENABLE_EXT() my_callback_touch_enable()
// extern bool my_callback_touch_enable(void);
// #define UDI_HID_TOUCH_DISABLE_EXT() my_callback_touch_disable()
// extern void my_callback_touch_disable(void);

/**
 * USB HID touch low level configuration
 * In standalone these configurations are defined by the HID touch module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_HID_TOUCH_EP_IN           (5 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_TOUCH_IFACE_NUMBER    3
//@}
//@}


/**
 * Configuration of HID Generic interface (if used)
 * @{
 */
//! Interface callback definition
#define  UDI_HID_GENERIC_ENABLE_EXT()       true
#define  UDI_HID_GENERIC_DISABLE_EXT()
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)
#define  UDI_HID_GENERIC_SET_FEATURE(f)
/*
 * #define UDI_HID_GENERIC_ENABLE_EXT() my_callback_generic_enable()
 * extern bool my_callback_generic_enable(void);
 * #define UDI_HID_GENERIC_DISABLE_EXT() my_callback_generic_disable()
 * extern void my_callback_generic_disable(void);
 * #define  UDI_HID_GENERIC_REPORT_OUT(ptr) my_callback_generic_report_out(ptr)
 * extern void my_callback_generic_report_out(uint8_t *report);
 * #define  UDI_HID_GENERIC_SET_FEATURE(f) my_callback_generic_set_feature(f)
 * extern void my_callback_generic_set_feature(uint8_t *report_feature);
 */
#define  UDI_HID_REPORT_IN_SIZE             1
//#define  UDI_HID_REPORT_OUT_SIZE            64
//#define  UDI_HID_REPORT_FEATURE_SIZE        4
#define  UDI_HID_GENERIC_EP_SIZE            64

/**
 * USB HID Generic low level configuration
 * In standalone these configurations are defined by the HID generic module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
//#define  UDI_HID_GENERIC_EP_OUT   (5 | USB_EP_DIR_OUT)
#define  UDI_HID_GENERIC_EP_IN    (6 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_HID_GENERIC_IFACE_NUMBER    4
//@}
//@}


//... Eventually add other Interface Configuration

//@}


/**
 * Description of Composite Device
 * @{
 */



#define UDI_COMPOSITE_DESC_T \
	udi_vendor_desc_t udi_vendor; \
	usb_iad_desc_t udi_cdc_iad; \
	udi_cdc_comm_desc_t udi_cdc_comm; \
	udi_cdc_data_desc_t udi_cdc_data; \
	udi_hid_touch_desc_t udi_hid_touch; \
	udi_hid_generic_desc_t udi_hid_generic;

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
	.udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data              = UDI_CDC_DATA_DESC_0_FS, \
	.udi_hid_touch             = UDI_HID_TOUCH_DESC, \
	.udi_hid_generic           = UDI_HID_GENERIC_DESC, \
	.udi_vendor				   = UDI_VENDOR_DESC(UDI_VENDOR_EPS_SIZE_BULK_FS)

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
	.udi_cdc_iad               = UDI_CDC_IAD_DESC_0, \
	.udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
	.udi_cdc_data              = UDI_CDC_DATA_DESC_0_HS, \
	.udi_hid_touch             = UDI_HID_TOUCH_DESC, \
	.udi_hid_generic           = UDI_HID_GENERIC_DESC, \
	.udi_vendor				   = UDI_VENDOR_DESC(UDI_VENDOR_EPS_SIZE_BULK_HS)

//! USB Interface APIs
#define UDI_COMPOSITE_API   \
	&udi_api_vendor,		\
	&udi_api_cdc_comm,      \
	&udi_api_cdc_data,      \
	&udi_api_hid_touch,		\
	&udi_api_hid_generic


//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error

// Example of include for interface
#include "udi_cdc.h"
#include "udi_hid_touch.h"
#include "udi_hid_generic.h"
#include "udi_vendor.h"

/* Declaration of callbacks used by USB */
//#include "callback_def.h"

#define UDD_BULK_NB_BANK(ep) (ep==(UDI_VENDOR_EP_BULK_OUT & USB_EP_ADDR_MASK)?2:1)	


#endif // _CONF_USB_H_
