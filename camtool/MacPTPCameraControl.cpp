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
# include  <sstream>
# include  <stdlib.h>

using namespace std;

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
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
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
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
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
      ptp_buf->dataUsageMode = kICACameraPassThruSend;
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
      ptp_buf->dataUsageMode = kICACameraPassThruSend;
      ptp_buf->dataSize = 4;
      ptp_buf->data[0] = (val >> 0) & 0xff;
      ptp_buf->data[1] = (val >> 8) & 0xff;
      ptp_buf->data[2] = (val >>16) & 0xff;
      ptp_buf->data[3] = (val >>24) & 0xff;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
}

static const unsigned nikon_exposure_list_cnt = 53;
static const struct {
      const char*text;
      uint32_t code;
} nikon_exposure_list[nikon_exposure_list_cnt] = {
      { "1/4000", 0x00010f0a },
      { "1/3200", 0x00010c80 },
      { "1/2500", 0x000109c4 },
      { "1/2000", 0x000107d0 },
      { "1/1600", 0x00010640 },
      { "1/1250", 0x000104e2 },
      { "1/1000", 0x000103e8 },
      { "1/800",  0x00010320 },
      { "1/640",  0x00010280 },
      { "1/500",  0x000101f4 },
      { "1/400",  0x00010190 },
      { "1/320",  0x000100fa },
      { "1/250",  0x000100fa },
      { "1/200",  0x000100c8 },
      { "1/160",  0x000100a0 },
      { "1/125",  0x0001007d },
      { "1/100",  0x00010064 },
      { "1/80",   0x00010050 },
      { "1/60",   0x0001003c },
      { "1/50",   0x00010032 },
      { "1/40",   0x00010028 },
      { "1/30",   0x0001001e },
      { "1/25",   0x00010019 },
      { "1/20",   0x00010014 },
      { "1/15",   0x0001000f },
      { "1/13",   0x0001000d },
      { "1/10",   0x0001000a },
      { "1/8",    0x00010008 },
      { "1/6",    0x00010006 },
      { "1/5",    0x00010005 },
      { "1/4",    0x00010004 },
      { "1/3",    0x00010003 },
      { "1/2.5",  0x000a0019 },
      { "1/2",    0x00010002 },
      { "1/1.6",  0x000a0010 },
      { "1/1.3",  0x000a000d },
      { "1s",     0x00010001 },
      { "1.3s",   0x000d000a },
      { "1.6s",   0x0010000a },
      { "2s",     0x00020001 },
      { "2.5s",   0x0019000a },
      { "3s",     0x00030001 },
      { "4s",     0x00040001 },
      { "5s",     0x00050001 },
      { "6s",     0x00060001 },
      { "8s",     0x00080001 },
      { "10s",    0x000a0001 },
      { "13s",    0x000d0001 },
      { "15s",    0x000f0001 },
      { "20s",    0x00140001 },
      { "25s",    0x00190001 },
      { "30s",    0x001e0001 },
      { "Bulb",   0xffffffff }
};

void MacPTPCameraControl::get_exposure_time_index(vector<string>&values)
{
      values.resize(nikon_exposure_list_cnt);
      for (unsigned idx = 0 ; idx < nikon_exposure_list_cnt ; idx += 1)
	    values[idx] = nikon_exposure_list[idx].text;
}

int MacPTPCameraControl::get_exposure_time_index()
{
      uint32_t rc;
      uint32_t val = ptp_get_property_u32_(0xd100 /* NIKON ExposureTime */, rc);
      for (unsigned idx = 0 ; idx < nikon_exposure_list_cnt ; idx += 1) {
	    if (val == nikon_exposure_list[idx].code)
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_exposure_time_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= (int)nikon_exposure_list_cnt)
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u32_(0xd100 /* NIKON ExposureTime */,
			    nikon_exposure_list[use_index].code,
			    rc);
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

static unsigned long val_from_bytes(UInt8*buf, size_t size)
{
      unsigned long val = 0;
      for (size_t idx = 0 ; idx < size ; idx += 1) {
	    val <<= 8UL;
	    val |= buf[size-idx-1];
      }
      return val;
}

string MacPTPCameraControl::debug_property_describe(unsigned prop_code)
{
      const size_t data_buf_size = 1024;
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + data_buf_size-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1014; // GetDevicePropDesc
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1024;

      ica_send_message_(ptp_buf, sizeof buf);
	//result_code = ptp_buf->resultCode;

	// The send_message_ should cause the data to be written to
	// the data[] array, and the dataSize replaced with the actual
	// data count retrieved.

      char tmp_buf[32];
      string out ("Description of property: ");
      unsigned long tmp_val = 0;
      tmp_val = (ptp_buf->data[1] << 8) | ptp_buf->data[0];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%04lx\n", tmp_val);
      out += tmp_buf;

      tmp_val = (ptp_buf->data[3] << 8) | ptp_buf->data[2];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%04lx\n", tmp_val);
      out += "Data type: ";
      out += tmp_buf;

      size_t value_size = 0;
      switch (tmp_val) {
	  case 0:
	    break;
	  case 1: // 8
	  case 2:
	    value_size = 1;
	    break;
	  case 3:
	  case 4:
	    value_size = 2;
	    break;
	  case 5:
	  case 6:
	    value_size = 4;
	    break;
	  case 7:
	  case 8:
	    value_size = 8;
	    break;
	  default:
	    value_size = 0;
	    break;
      }

      tmp_val = ptp_buf->data[4];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%02lx\n", tmp_val);
      out += "GetSet flag: ";
      out += tmp_buf;

      int idx = 5;
      tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
      out += "Factury default: ";
      out += tmp_buf;
      idx += value_size;

      tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
      out += "Current value: ";
      out += tmp_buf;
      idx += value_size;

      int form_flag = ptp_buf->data[idx++];
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%02x\n", form_flag);
      out += "Form flag: ";
      out += tmp_buf;

      unsigned enum_count = 0;
      switch (form_flag) {
	  case 2: // ENUM
	    enum_count = val_from_bytes(ptp_buf->data+idx, 2);
	    idx += 2;

	    snprintf(tmp_buf, sizeof tmp_buf, "%u\n", enum_count);
	    out += "Enum count: ";
	    out += tmp_buf;
	    for (unsigned enum_idx = 0; enum_idx < enum_count; enum_idx += 1) {
		  tmp_val = val_from_bytes(ptp_buf->data+idx, value_size);
		  idx += value_size;
		  snprintf(tmp_buf, sizeof tmp_buf, " 0x%lx\n", tmp_val);
		  out += tmp_buf;
	    }
	  default:
	    break;
      }

      return out;
}
