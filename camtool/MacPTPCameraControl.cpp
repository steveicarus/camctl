/*
 * Copyright (c) 2008-2009 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "MacICACameraControl.h"
# include  <stdlib.h>

MacPTPCameraControl::MacPTPCameraControl(ICAObject dev)
: MacICACameraControl(dev)
{
}

MacPTPCameraControl::~MacPTPCameraControl()
{
}

void MacICACameraControl::ica_send_message_(void*buf, size_t buf_len)
{
      ICAObjectSendMessagePB msg;
      memset(&msg, 0, sizeof(msg));

      msg.object = dev_;
      msg.message.messageType = kICAMessageCameraPassThrough;
      msg.message.startByte = 0;
      msg.message.dataPtr = buf;
      msg.message.dataSize = buf_len;
      msg.message.dataType = kICATypeData;

      /* ICAError err =*/ ICAObjectSendMessage(&msg, 0);
}

uint16_t MacPTPCameraControl::ptp_get_property_u16_(unsigned prop_code,
						    uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 2-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1015; // GetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kPTPPassThruReceive;
      ptp_buf->dataSize = 2;
      ptp_buf->data[0] = 0;
      ptp_buf->data[1] = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      uint16_t val = ptp_buf->data[1];
      val <<= 8; val |= ptp_buf->data[0];
      return val;
}

uint32_t MacPTPCameraControl::ptp_get_property_u32_(unsigned prop_code,
						    uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 4-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1015; // GetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kPTPPassThruReceive;
      ptp_buf->dataSize = 4;
      ptp_buf->data[0] = 0;
      ptp_buf->data[1] = 0;
      ptp_buf->data[2] = 0;
      ptp_buf->data[3] = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      uint32_t val = ptp_buf->data[3];
      val <<= 8; val |= ptp_buf->data[2];
      val <<= 8; val |= ptp_buf->data[1];
      val <<= 8; val |= ptp_buf->data[0];
      return val;
}

void MacPTPCameraControl::ptp_set_property_u16_(unsigned prop_code,
						uint16_t val,
						uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 2-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1016; // SetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kPTPPassThruSend;
      ptp_buf->dataSize = 2;
      ptp_buf->data[0] = (val >> 0) & 0xff;
      ptp_buf->data[1] = (val >> 8) & 0xff;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
}

void MacPTPCameraControl::ptp_set_property_u32_(unsigned prop_code,
						uint32_t val,
						uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 4-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1016; // SetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kPTPPassThruSend;
      ptp_buf->dataSize = 4;
      ptp_buf->data[0] = (val >> 0) & 0xff;
      ptp_buf->data[1] = (val >> 8) & 0xff;
      ptp_buf->data[2] = (val >>16) & 0xff;
      ptp_buf->data[3] = (val >>24) & 0xff;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
}

void MacPTPCameraControl::get_exposure_time(int32_t&min, int32_t&max, int32_t&step)
{
      min  = 250;      // Minimum: 1/4000s
      max  = 30000000; // Maximim: 30s
      step = 250;      // Steps of 1/4000s
}

int32_t MacPTPCameraControl::get_exposure_time()
{
      uint32_t rc;
      uint32_t val = ptp_get_property_u32_(0x500d /* ExposureTime */, rc);
      return val;
}

void MacPTPCameraControl::set_exposure_time(int32_t)
{
}

int MacPTPCameraControl::get_aperture()
{
      return -1;
}

int MacPTPCameraControl::get_exposure_index()
{
      return -1;
}

void MacPTPCameraControl::set_aperture(int)
{
}

void MacPTPCameraControl::set_exposure_index(int)
{
}

int MacPTPCameraControl::debug_property_get(unsigned prop,
					    unsigned dtype,
					    unsigned long&value)
{
      uint32_t rc = 0;
      switch (dtype) {
	  case 0x0004: // UINT16
	    value = ptp_get_property_u32_(prop, rc);
	    break;
	  case 0x0006: // UINT32
	    value = ptp_get_property_u32_(prop, rc);
	    break;
	  default:
	    return -1;
      }

      return (int)rc;
}

int MacPTPCameraControl::debug_property_set(unsigned prop,
					    unsigned dtype,
					    unsigned long value)
{
      uint32_t rc;
      switch (dtype) {
	  case 0x0004: // UINT16
	    ptp_set_property_u16_(prop, value, rc);
	    break;
	  case 0x0006: // UINT32
	    ptp_set_property_u32_(prop, value, rc);
	  default:
	    return -1;
      }
      return (int)rc;
}
