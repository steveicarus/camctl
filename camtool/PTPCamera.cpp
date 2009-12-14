
/*
 * Copyright (c) 2009 Stephen Williams (steve@icarus.com)
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

# include  "PTPCamera.h"
# include  "ptp_misc.h"
using namespace std;

template <class T> static T val_from_bytes(unsigned char*&buf);

template <> static int8_t val_from_bytes<int8_t>(unsigned char*&buf)
{
      int8_t val = (int8_t) buf[0];
      buf += 1;
      return val;
}

template <> static uint8_t val_from_bytes<uint8_t>(unsigned char*&buf)
{
      uint8_t val = (uint8_t) buf[0];
      buf += 1;
      return val;
}

template <> static int16_t val_from_bytes<int16_t>(unsigned char*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return (int16_t)val;
}

template <> static uint16_t val_from_bytes<uint16_t>(unsigned char*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return val;
}

template <> static int32_t val_from_bytes<int32_t>(unsigned char*&buf)
{
      uint32_t val = (uint32_t) buf[3];
      val <<= 8;
      val |= (uint32_t) buf[2];
      val <<= 8;
      val |= (uint32_t) buf[1];
      val <<= 8;
      val |= (uint32_t) buf[0];
      buf += 4;
      return (int32_t)val;
}

template <> static uint32_t val_from_bytes<uint32_t>(unsigned char*&buf)
{
      uint32_t val = (uint32_t) buf[3];
      val <<= 8;
      val |= (uint32_t) buf[2];
      val <<= 8;
      val |= (uint32_t) buf[1];
      val <<= 8;
      val |= (uint32_t) buf[0];
      buf += 4;
      return val;
}

template <> static vector<uint16_t> val_from_bytes< vector<uint16_t> >(unsigned char*&buf)
{
      uint32_t size = val_from_bytes<uint32_t>(buf);
      vector<uint16_t> val (size);
      for (size_t idx = 0 ; idx < val.size() ; idx += 1)
	    val[idx] = val_from_bytes<uint16_t>(buf);

      return val;
}

template<> static QString val_from_bytes<QString>(unsigned char*&buf)
{
      QString result;
      uint8_t slen = val_from_bytes<uint8_t>(buf);
      if (slen > 0) {
	    unsigned short*str = new unsigned short[slen];
	    for (uint8_t idx = 0 ; idx < slen ; idx += 1)
		  str[idx] = val_from_bytes<uint16_t>(buf);
	    result.setUtf16(str, slen);
	    delete[]str;
      }
      return result;
}

PTPCamera::PTPCamera()
{
}

PTPCamera::~PTPCamera()
{
}

uint32_t PTPCamera::ptp_get_device_info(void)
{
      unsigned char recv_buf[1024];

      uint32_t rc = ptp_command(0x1001 /* GetDeviceInfo */, vector<uint32_t>(),
				0, 0, recv_buf, sizeof recv_buf);

      unsigned char*dptr = recv_buf;

      standard_version_ = val_from_bytes<uint16_t>(dptr);
      vendor_extension_id_ = val_from_bytes<uint32_t>(dptr);
      vendor_extension_vers_ = val_from_bytes<uint16_t>(dptr);
      vendor_extension_desc_ = val_from_bytes<QString>(dptr);
      functional_mode_ = val_from_bytes<uint16_t>(dptr);
      operations_supported_ = val_from_bytes< vector<uint16_t> >(dptr);
      events_supported_ = val_from_bytes< vector<uint16_t> >(dptr);
      device_properties_supported_ = val_from_bytes< vector<uint16_t> >(dptr);
      capture_formats_ = val_from_bytes< vector<uint16_t> >(dptr);
      image_formats_ = val_from_bytes< vector<uint16_t> >(dptr);
      ptp_manufacturer_ = val_from_bytes<QString>(dptr);
      ptp_model_ = val_from_bytes<QString>(dptr);
      device_version_ = val_from_bytes<QString>(dptr);
      serial_number_ = val_from_bytes<QString>(dptr);

      return rc;
}

uint16_t PTPCamera::ptp_standard_version() const
{
      return standard_version_;
}

uint32_t PTPCamera::ptp_extension_vendor(bool raw_id) const
{
      if (raw_id) return vendor_extension_id_;

      uint32_t use_extension_id = vendor_extension_id_;
	// Some Nikon cameras forgot to set their vendor extension ID
	// to Nikon, and instead report themselves as "Microsoft." So
	// define a practical extension id to use.
      if (use_extension_id == 0x006 && ptp_Manufacturer() == "Nikon") {
	    use_extension_id = 0x000a;
      }

      return use_extension_id;
}

uint16_t PTPCamera::ptp_extension_version() const
{
      return vendor_extension_vers_;
}

QString PTPCamera::ptp_extension_desc() const
{
      return vendor_extension_desc_;
}

bool PTPCamera::ptp_operation_is_supported(uint16_t code) const
{
      for (size_t idx = 0 ; idx < operations_supported_.size() ; idx += 1) {
	    if (code == operations_supported_[idx]) return true;
      }

      return false;
}

uint16_t PTPCamera::ptp_FunctionalMode() const
{
      return functional_mode_;
}

QString PTPCamera::ptp_Manufacturer() const
{
      return ptp_manufacturer_;
}

QString PTPCamera::ptp_Model() const
{
      return ptp_model_;
}

QString PTPCamera::ptp_DeviceVersion() const
{
      return device_version_;
}

QString PTPCamera::ptp_SerialNumber() const
{
      return serial_number_;
}

vector<QString> PTPCamera::ptp_operations_list() const
{
      uint32_t use_extension_id = ptp_extension_vendor();
      vector<QString>res (operations_supported_.size());

      for (unsigned idx = 0 ; idx < operations_supported_.size() ; idx += 1) {
	    string opcode_string = ptp_opcode_string(operations_supported_[idx],
						     use_extension_id);
	    res[idx] = opcode_string.c_str();
      }

      return res;
}

vector<QString> PTPCamera::ptp_events_list() const
{
      uint32_t use_extension_id = ptp_extension_vendor();
      vector<QString>res (events_supported_.size());

      for (unsigned idx = 0 ; idx < events_supported_.size() ; idx += 1) {
	    string event_string = ptp_event_string(events_supported_[idx],
						   use_extension_id);
	    res[idx] = event_string.c_str();
      }

      return res;
}

vector<QString> PTPCamera::ptp_properties_list() const
{
      uint32_t use_extension_id = ptp_extension_vendor();
      vector<QString>res (device_properties_supported_.size());

      for (unsigned idx = 0 ; idx < device_properties_supported_.size() ; idx += 1) {
	    string prop_string = ptp_property_string(device_properties_supported_[idx],
						   use_extension_id);
	    res[idx] = prop_string.c_str();
      }

      return res;
}

static QString map_image_format_to_string(uint16_t code)
{
      switch (code) {
	  case 0x3000:
	    return QString("Undefined non-image object");
	  case 0x3001:
	    return QString("Association (e.g. directory)");
	  case 0x3002:
	    return QString("Script (device-model specific)");
	  case 0x3006:
	    return QString("Digital Print Order Format (text)");
	  case 0x3800:
	    return QString("Unknown image object");
	  case 0x3801:
	    return QString("EXIF/JPEG");
	  case 0x3808:
	    return QString("JFIF");
	  case 0x380d:
	    return QString("TIFF");
	  default:
	    return QString("0x%1") .arg(code,4,16);
      }
}

vector<QString> PTPCamera::ptp_capture_formats_list() const
{
      vector<QString>res (capture_formats_.size());

      for (unsigned idx = 0 ; idx < capture_formats_.size() ; idx += 1) {
	    res[idx] = map_image_format_to_string(capture_formats_[idx]);
      }

      return res;
}

vector<QString> PTPCamera::ptp_image_formats_list() const
{
      vector<QString>res (image_formats_.size());

      for (unsigned idx = 0 ; idx < image_formats_.size() ; idx += 1) {
	    res[idx] = map_image_format_to_string(image_formats_[idx]);
      }

      return res;
}

uint8_t PTPCamera::ptp_get_property_u8(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [1];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);
      return recv_buf[0];
}

uint16_t PTPCamera::ptp_get_property_u16(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [2];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      uint16_t val = recv_buf[1];
      val <<= 8;
      val |= recv_buf[0];
      return val;
}

uint32_t PTPCamera::ptp_get_property_u32(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [4];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      uint16_t val = recv_buf[3];
      val <<= 8; val |= recv_buf[2];
      val <<= 8; val |= recv_buf[1];
      val <<= 8; val |= recv_buf[0];
      return val;
}

QString PTPCamera::ptp_get_property_string(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [512];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      unsigned char*dptr = recv_buf;
      return val_from_bytes<QString>(dptr);
}
