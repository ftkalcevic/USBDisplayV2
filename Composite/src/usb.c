#include "asf.h"

bool udc_get_extra_string(uint8_t id, const uint8_t **pStr, uint8_t *pLen)
{
	switch ( id )
	{
		case UDI_HID_TOUCH_STRING_ID:
			*pStr = (uint8_t *)UDI_HID_TOUCH_STRING;
			*pLen = sizeof(UDI_HID_TOUCH_STRING)-1;
			break;
		case UDI_VENDOR_STRING_ID:
			*pStr = (uint8_t *)UDI_VENDOR_STRING;
			*pLen = sizeof(UDI_VENDOR_STRING)-1;
			break;
		default:
			return false;
	}
	return true;
}
