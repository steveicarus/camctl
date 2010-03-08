
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
# include  "CameraControl.h"
# include  <QTreeWidgetItem>
# include  <stddef.h>
using namespace std;

static map<uint16_t,PTPCamera::type_code_t> generate_type_code_map(void)
{
      map<uint16_t,PTPCamera::type_code_t> tmp;
      tmp[PTPCamera::TYPE_NONE] = PTPCamera::TYPE_NONE;
      tmp[PTPCamera::TYPE_INT8] = PTPCamera::TYPE_INT8;
      tmp[PTPCamera::TYPE_UINT8] = PTPCamera::TYPE_UINT8;
      tmp[PTPCamera::TYPE_INT16] = PTPCamera::TYPE_INT16;
      tmp[PTPCamera::TYPE_UINT16] = PTPCamera::TYPE_UINT16;
      tmp[PTPCamera::TYPE_INT32] = PTPCamera::TYPE_INT32;
      tmp[PTPCamera::TYPE_UINT32] = PTPCamera::TYPE_UINT32;
      tmp[PTPCamera::TYPE_INT64] = PTPCamera::TYPE_INT64;
      tmp[PTPCamera::TYPE_UINT64] = PTPCamera::TYPE_UINT64;
      tmp[PTPCamera::TYPE_INT128] = PTPCamera::TYPE_INT128;
      tmp[PTPCamera::TYPE_UINT128] = PTPCamera::TYPE_UINT128;
      tmp[PTPCamera::TYPE_STRING] = PTPCamera::TYPE_STRING;
      return tmp;
}

map<uint16_t,PTPCamera::type_code_t> PTPCamera::ptp_type_to_type_code_ = generate_type_code_map();

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

const PTPCamera::prop_value_t PTPCamera::prop_value_nil;

QString PTPCamera::qresult_code(uint32_t rc)
{
      QString rc_text;
      rc_text.setNum(rc, 16);
      switch (rc) {
	  case 0x2001:
	    rc_text.append(" (OK)");
	    break;
	  case 0x2002:
	    rc_text.append(" (General Error)");
	    break;
	  case 0x2003:
	    rc_text.append(" (Session Not Open)");
	    break;
	  case 0x2005:
	    rc_text.append(" (Operation Not Supported)");
	    break;
	  default:
	    break;
      }

      return rc_text;
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
      vector<uint16_t>properties_list;

      standard_version_ = val_from_bytes<uint16_t>(dptr);
      vendor_extension_id_ = val_from_bytes<uint32_t>(dptr);
      vendor_extension_vers_ = val_from_bytes<uint16_t>(dptr);
      vendor_extension_desc_ = val_from_bytes<QString>(dptr);
      functional_mode_ = val_from_bytes<uint16_t>(dptr);
      operations_supported_ = val_from_bytes< vector<uint16_t> >(dptr);
      events_supported_ = val_from_bytes< vector<uint16_t> >(dptr);
      properties_list = val_from_bytes< vector<uint16_t> >(dptr);
      capture_formats_ = val_from_bytes< vector<uint16_t> >(dptr);
      image_formats_ = val_from_bytes< vector<uint16_t> >(dptr);
      ptp_manufacturer_ = val_from_bytes<QString>(dptr);
      ptp_model_ = val_from_bytes<QString>(dptr);
      device_version_ = val_from_bytes<QString>(dptr);
      serial_number_ = val_from_bytes<QString>(dptr);

      device_properties_supported_.clear();
      for (size_t idx = 0 ; idx < properties_list.size() ; idx += 1) {
	    prop_info_t tmp;
	    device_properties_supported_[properties_list[idx]] = tmp;
      }

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

float PTPCamera::ptp_battery_level()
{
      uint32_t rc;
	// Get the PTP standard BatteryLevel property value.
      prop_value_t val = ptp_get_property(0x5001 /* PTP BatteryLevel */, rc);

      if (val.get_type() == TYPE_NONE)
	    return -1;

	// The BatteryLevel is by definition (PTP) a UINT8.
      assert(val.get_type() == TYPE_UINT8);

      map<uint16_t,prop_info_t>::const_iterator cur = device_properties_supported_.find(0x5001);
      assert(cur != device_properties_supported_.end());

      uint8_t val_min, val_max;
      if (cur->second.form_flag == 1) { // RANGE
	    val_min = cur->second.range[0].get_uint8();
	    val_max = cur->second.range[1].get_uint8();

      } else if (cur->second.form_flag == 2) { // ENUM
	    val_min = cur->second.range[0].get_uint8();
	    val_max = val_min;
	    for (size_t idx = 1 ; idx < cur->second.range.size() ; idx += 1) {
		  uint8_t tmp = cur->second.range[idx].get_uint8();
		  if (tmp < val_min)
			val_min = tmp;
		  if (tmp > val_max)
			val_max = tmp;
	    }
      }

      uint8_t use_val = val.get_uint8();
      if (use_val >= val_max)
	    return 100.0;
      if (use_val <= val_min)
	    return 0.0;

      use_val -= val_min;
      val_max -= val_min;
      return (use_val*100.0) / (val_max*1.0);
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

vector< pair<uint16_t,QString> > PTPCamera::ptp_properties_list() const
{
      uint32_t use_extension_id = ptp_extension_vendor();
      vector<code_string_t>res (device_properties_supported_.size());

      size_t idx = 0;
      for (map<uint16_t,prop_info_t>::const_iterator cur = device_properties_supported_.begin()
		 ; cur != device_properties_supported_.end() ; cur ++) {
	    string prop_string = ptp_property_string(cur->first, use_extension_id);
	    pair<uint16_t,QString> item;
	    item.first = cur->first;
	    item.second = prop_string.c_str();
	    res[idx++] = item;
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

PTPCamera::type_code_t PTPCamera::ptp_get_property_type(unsigned prop_code) const
{
      std::map<uint16_t, prop_info_t>::const_iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end())
	    return TYPE_NONE;
      else
	    return cur->second.type_code;
}

bool PTPCamera::ptp_get_property_is_setable(unsigned prop_code) const
{
      std::map<uint16_t, prop_info_t>::const_iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end())
	    return false;

      if (cur->second.get_set_flag)
	    return true;
      else
	    return false;
}

PTPCamera::type_form_t PTPCamera::ptp_get_property_form(unsigned prop_code) const
{
      std::map<uint16_t, prop_info_t>::const_iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end())
	    return FORM_NONE;

      if (cur->second.form_flag == FORM_ENUM)
	    return FORM_ENUM;
      else
	    return FORM_RANGE;
}

int PTPCamera::ptp_get_property_enum(unsigned prop_code, vector<labeled_value_t>&table) const
{
      table.clear();
      map<uint16_t,prop_info_t>::const_iterator info = device_properties_supported_.find(prop_code);
      if (info == device_properties_supported_.end())
	    return -1;

      if (info->second.form_flag != 2) // ENUM
	    return -1;

      table.resize(info->second.range.size());
      switch (info->second.type_code) {
	  case TYPE_STRING:
	    for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
		  table[idx].label = info->second.range[idx].get_string();
		  table[idx].value = info->second.range[idx];
	    }
	    break;
	  case TYPE_UINT8:
	    for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
		  uint8_t val = info->second.range[idx].get_uint8();
		  string tmp = ptp_property_uint8_string(prop_code, val,
							  ptp_extension_vendor());
		  table[idx].label = QString(tmp.c_str());
		  table[idx].value = info->second.range[idx];
	    }
	    break;
	  case TYPE_UINT16:
	    for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
		  uint16_t val = info->second.range[idx].get_uint16();
		  string tmp = ptp_property_uint16_string(prop_code, val,
							   ptp_extension_vendor());
		  table[idx].label = QString(tmp.c_str());
		  table[idx].value = info->second.range[idx];
	    }
	    break;
	  case TYPE_UINT32:
	    for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
		  uint32_t val = info->second.range[idx].get_uint32();
		  string tmp = ptp_property_uint32_string(prop_code, val,
							   ptp_extension_vendor());
		  table[idx].label = QString(tmp.c_str());
		  table[idx].value = info->second.range[idx];
	    }
	    break;
	  default:
	    for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
		  table[idx].value = info->second.range[idx];
	    }
	    break;
      }

      int cur_idx = 0;
      for (size_t idx = 0 ; idx < table.size() ; idx += 1) {
	    if (info->second.current == table[idx].value) {
		  cur_idx = idx;
		  break;
	    }
      }

      return cur_idx;
}

bool PTPCamera::ptp_get_property_range(unsigned prop_code, prop_value_t&min, prop_value_t&max, prop_value_t&step) const
{
      map<uint16_t,prop_info_t>::const_iterator info = device_properties_supported_.find(prop_code);
      if (info == device_properties_supported_.end())
	    return false;

      if (info->second.form_flag != 1) // This method only works with RANGE types
	    return false;

      assert(info->second.range.size() == 3);

      min = info->second.range[0];
      max = info->second.range[1];
      step = info->second.range[2];
      return true;
}

const PTPCamera::prop_value_t& PTPCamera::ptp_get_property_current(unsigned prop_code) const
{
      map<uint16_t, prop_info_t>::const_iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end())
	    return prop_value_nil;

      return cur->second.current;
}

const PTPCamera::prop_value_t& PTPCamera::ptp_get_property_factory(unsigned prop_code) const
{
      map<uint16_t, prop_info_t>::const_iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end())
	    return prop_value_nil;

      return cur->second.factory;
}

int8_t PTPCamera::ptp_get_property_int8_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [1];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);
      return recv_buf[0];
}

uint8_t PTPCamera::ptp_get_property_uint8_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [1];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);
      return recv_buf[0];
}

int16_t PTPCamera::ptp_get_property_int16_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [2];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      int16_t val = recv_buf[1];
      val <<= 8;
      val |= recv_buf[0];
      return val;
}

uint16_t PTPCamera::ptp_get_property_uint16_(unsigned prop_code, uint32_t&result_code)
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

int32_t PTPCamera::ptp_get_property_int32_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [4];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      uint32_t val = recv_buf[3];
      val <<= 8; val |= recv_buf[2];
      val <<= 8; val |= recv_buf[1];
      val <<= 8; val |= recv_buf[0];
      return val;
}

uint32_t PTPCamera::ptp_get_property_uint32_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [4];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      uint32_t val = recv_buf[3];
      val <<= 8; val |= recv_buf[2];
      val <<= 8; val |= recv_buf[1];
      val <<= 8; val |= recv_buf[0];
      return val;
}

QString PTPCamera::ptp_get_property_string_(unsigned prop_code, uint32_t&result_code)
{
      vector<uint32_t> params (1);
      unsigned char recv_buf [512];

      params[0] = prop_code;
      result_code = ptp_command(0x1015 /* GetDevicePropValue */, params,
				0, 0, recv_buf, sizeof recv_buf);

      unsigned char*dptr = recv_buf;
      return val_from_bytes<QString>(dptr);
}

const PTPCamera::prop_value_t& PTPCamera::ptp_get_property(unsigned prop_code, uint32_t&rc)
{
      map<uint16_t,prop_info_t>::iterator cur = device_properties_supported_.find(prop_code);
      if (cur == device_properties_supported_.end()) {
	    CameraControl::debug_log << "PTPCamera::ptp_get_property("
				     << hex << prop_code << dec
				     << "): Property not found?" << endl << flush;
	    rc = 0;
	    return prop_value_nil;
      }

      if (cur->second.current.get_type() == TYPE_NONE) {
	    CameraControl::debug_log << "PTPCamera::ptp_get_property("
				     << hex << prop_code << dec
				     << "): Probe property" << endl << flush;
	    ptp_probe_property(prop_code, rc);
      }

      switch (cur->second.type_code) {
	  case TYPE_NONE:
	    cur->second.current = prop_value_nil;
	    break;
	  case TYPE_INT8:
	    cur->second.current.set_int8( ptp_get_property_int8_(prop_code, rc) );
	    break;
	  case TYPE_UINT8:
	    cur->second.current.set_uint8( ptp_get_property_uint8_(prop_code, rc) );
	    break;
	  case TYPE_INT16:
	    cur->second.current.set_int16( ptp_get_property_int16_(prop_code, rc) );
	    break;
	  case TYPE_UINT16:
	    cur->second.current.set_uint16( ptp_get_property_uint16_(prop_code, rc) );
	    break;
	  case TYPE_INT32:
	    cur->second.current.set_int32( ptp_get_property_int32_(prop_code, rc) );
	    break;
	  case TYPE_UINT32:
	    cur->second.current.set_uint32( ptp_get_property_uint32_(prop_code, rc) );
	    break;
	  case TYPE_STRING:
	    cur->second.current.set_string( ptp_get_property_string_(prop_code, rc) );
	    break;
	  case TYPE_INT64:
	  case TYPE_UINT64:
	  case TYPE_INT128:
	  case TYPE_UINT128:
	    assert(0);
	    break;
      }

      return cur->second.current;
}

void PTPCamera::ptp_set_property_int8_(unsigned prop_code, int8_t val,
					uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[1];

      send_buf[0] = val;

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);
}

void PTPCamera::ptp_set_property_uint8_(unsigned prop_code, uint8_t val,
					uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[1];

      send_buf[0] = val;

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);
}

void PTPCamera::ptp_set_property_int16_(unsigned prop_code, int16_t val,
					uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[2];

      send_buf[0] = (val >> 0) & 0xff;
      send_buf[1] = (val >> 8) & 0xff;

      CameraControl::debug_log << "PTPCamera::ptp_set_property_int16_("
			       << "prop=0x" << hex << prop_code
			       << ", val=0x" << hex << val << dec << ")";

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);

      CameraControl::debug_log << " --> 0x" << hex << result_code << dec << endl;
}

void PTPCamera::ptp_set_property_uint16_(unsigned prop_code, uint16_t val,
				     uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[2];

      send_buf[0] = (val >> 0) & 0xff;
      send_buf[1] = (val >> 8) & 0xff;

      CameraControl::debug_log << "PTPCamera::ptp_set_property_uint16_("
			       << "prop=0x" << hex << prop_code
			       << ", val=0x" << hex << val << dec << ")";

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);

      CameraControl::debug_log << " --> 0x" << hex << result_code << dec << endl;
}

void PTPCamera::ptp_set_property_int32_(unsigned prop_code, int32_t val,
					uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[4];

      send_buf[0] = (val >> 0) & 0xff;
      send_buf[1] = (val >> 8) & 0xff;
      send_buf[2] = (val >>16) & 0xff;
      send_buf[3] = (val >>24) & 0xff;

      CameraControl::debug_log << "PTPCamera::ptp_set_property_int32_("
			       << "prop=0x" << hex << prop_code
			       << ", val=0x" << hex << val << dec << ")";

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);

      CameraControl::debug_log << " --> 0x" << hex << result_code << dec << endl;
}

void PTPCamera::ptp_set_property_uint32_(unsigned prop_code, uint32_t val,
				     uint32_t&result_code)
{
      vector<uint32_t> params(1);
      unsigned char send_buf[4];

      send_buf[0] = (val >> 0) & 0xff;
      send_buf[1] = (val >> 8) & 0xff;
      send_buf[2] = (val >>16) & 0xff;
      send_buf[3] = (val >>24) & 0xff;

      CameraControl::debug_log << "PTPCamera::ptp_set_property_uint32_("
			       << "prop=0x" << hex << prop_code
			       << ", val=0x" << hex << val << dec << ")";

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, sizeof send_buf, 0, 0);

      CameraControl::debug_log << " --> 0x" << hex << result_code << dec << endl;
}

void PTPCamera::ptp_set_property_string_(unsigned prop_code, const QString&val,
					uint32_t&result_code)
{
      vector<uint32_t> params(1);
      size_t send_siz = val.size()*2 + 3;
      unsigned char*send_buf = new unsigned char[send_siz];

      unsigned char*dptr = send_buf;
      *dptr++ = val.size() + 1;

      const ushort*tmp = val.utf16();
      for (int idx = 0 ; idx < val.size() ; idx += 1) {
	    *dptr++ = (tmp[idx] >> 0) & 0xff;
	    *dptr++ = (tmp[idx] >> 8) & 0xff;
      }
      *dptr++ = 0;
      *dptr++ = 0;
      assert((dptr-send_buf) == send_siz);

      params[0] = prop_code;
      result_code = ptp_command(0x1016 /* SetDevicePropValue */, params,
				send_buf, send_siz, 0, 0);

      delete[]send_buf;
}

void PTPCamera::ptp_set_property(unsigned prop_code, const prop_value_t&val, uint32_t&result_code)
{
      switch (val.get_type()) {
	  case TYPE_NONE:
	    break;
	  case TYPE_INT8:
	    ptp_set_property_int8_(prop_code, val.get_uint8(), result_code);
	    break;
	  case TYPE_UINT8:
	    ptp_set_property_uint8_(prop_code, val.get_uint8(), result_code);
	    break;
	  case TYPE_INT16:
	    ptp_set_property_int16_(prop_code, val.get_uint16(), result_code);
	    break;
	  case TYPE_UINT16:
	    ptp_set_property_uint16_(prop_code, val.get_uint16(), result_code);
	    break;
	  case TYPE_INT32:
	    ptp_set_property_int32_(prop_code, val.get_uint32(), result_code);
	    break;
	  case TYPE_UINT32:
	    ptp_set_property_uint32_(prop_code, val.get_uint32(), result_code);
	    break;
	  case TYPE_STRING:
	    ptp_set_property_string_(prop_code, val.get_string(), result_code);
	    break;

	  default:
	    assert(0);
      }
}

void PTPCamera::ptp_probe_property(unsigned prop_code, uint32_t&result_code)
{
	// First get a reference to the entry for this property in the
	// support table.
      std::map<uint16_t, prop_info_t>::iterator desc = device_properties_supported_.find(prop_code);
      if (desc == device_properties_supported_.end()) {
	    return;
      }

	// Send a command to the device to describe this property.
      unsigned char recv_buf [1024];
      vector<uint32_t> params(1);
      params[0] = prop_code;
      result_code = ptp_command(0x1014 /* GetDevicePropDesc */, params,
				0, 0, recv_buf, sizeof recv_buf);

      CameraControl::debug_log << "GetDevicePropDesc(" << hex << prop_code
			       << ") result_code=" << hex << result_code
			       << dec << endl;

      if (result_code != 0x2001)
	    return;

      unsigned char*dptr = recv_buf;

	// data[0]
	// data[1] -- Property code back
      uint16_t prop = val_from_bytes<uint16_t>(dptr);
      assert(prop == prop_code);

	// data[2]
	// data[3] -- data type code
	// Setting the type code for the property also turns its
	// support flag on.
      uint16_t type = val_from_bytes<uint16_t>(dptr);
      desc->second.type_code = ptp_type_to_type_code_[type];

	// data[4] -- GetSet flag
      desc->second.get_set_flag = val_from_bytes<uint8_t>(dptr);

      CameraControl::debug_log << "    prop: 0x" << hex << prop << dec << endl;
      CameraControl::debug_log << "    type: 0x" << hex << type << dec << endl;
      CameraControl::debug_log << "  GetSet: " << (int)desc->second.get_set_flag << endl;

	// Starting at data[5]...
	//   -- Factory Default value
	//   -- Current value
      switch (desc->second.type_code) {
	  case 0:  // UNDEFINED
	  case 1:  // INT8
	    desc->second.factory.set_int8(val_from_bytes<int8_t>(dptr));
	    desc->second.current.set_int8(val_from_bytes<int8_t>(dptr));
	    break;
	  case 2:  // UINT8
	    desc->second.factory.set_uint8(val_from_bytes<uint8_t>(dptr));
	    desc->second.current.set_uint8(val_from_bytes<uint8_t>(dptr));
	    break;
	  case 3:  // INT16
	    desc->second.factory.set_int16(val_from_bytes<int16_t>(dptr));
	    desc->second.current.set_int16(val_from_bytes<int16_t>(dptr));
	    break;
	  case 4:  // UINT16
	    desc->second.factory.set_uint16(val_from_bytes<uint16_t>(dptr));
	    desc->second.current.set_uint16(val_from_bytes<uint16_t>(dptr));
	    break;
	  case 5:  // INT32
	    desc->second.factory.set_int32(val_from_bytes<int32_t>(dptr));
	    desc->second.current.set_int32(val_from_bytes<int32_t>(dptr));
	    break;
	  case 6:  // UINT32
	    desc->second.factory.set_uint32(val_from_bytes<uint32_t>(dptr));
	    desc->second.current.set_uint32(val_from_bytes<uint32_t>(dptr));
	    break;
	  case 7:  // INT64
	  case 8:  // UINT64
	  case 9:  // INT128;
	  case 10: // UINT128;
	    break;
	  case 0xffff: // String
	    desc->second.factory.set_string(val_from_bytes<QString>(dptr));
	    desc->second.current.set_string(val_from_bytes<QString>(dptr));
	    break;
	  default:
	    break;
      }

	// The form flag...
      desc->second.form_flag = val_from_bytes<uint8_t>(dptr);

      CameraControl::debug_log << "    form: " << (int)desc->second.form_flag << endl;

      if (desc->second.form_flag == 1) { // RANGE
	      // The range description includes 3 values: the minimum
	      // value, the maximum value and the step.
	    desc->second.range = vector<prop_value_t>(3);
	    switch (desc->second.type_code) {
		case 1: { // INT8
		      desc->second.range[0].set_int8(val_from_bytes<int8_t>(dptr));
		      desc->second.range[1].set_int8(val_from_bytes<int8_t>(dptr));
		      desc->second.range[2].set_int8(val_from_bytes<int8_t>(dptr));
		      break;
		}
		case 2: { // UINT8
		      desc->second.range[0].set_uint8(val_from_bytes<uint8_t>(dptr));
		      desc->second.range[1].set_uint8(val_from_bytes<uint8_t>(dptr));
		      desc->second.range[2].set_uint8(val_from_bytes<uint8_t>(dptr));
		      break;
		}
		default: {
		      break;
		}
	    }

      } else if (desc->second.form_flag == 2) { // ENUM
	      // An enumeration is a complete list of the possible
	      // value that the property can take.
	    uint16_t count = val_from_bytes<uint16_t>(dptr);
	    desc->second.range = vector<prop_value_t>(count);

	    switch (desc->second.type_code) {
		case 1: // INT8
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_int8(val_from_bytes<int8_t>(dptr));
		  break;
		case 2: // UINT8
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_uint8(val_from_bytes<uint8_t>(dptr));
		  break;
		case 3: // INT16
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_int16(val_from_bytes<int16_t>(dptr));
		  break;
		case 4: // UINT16
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_uint16(val_from_bytes<uint16_t>(dptr));
		  break;
		case 5: // INT32
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_int32(val_from_bytes<int32_t>(dptr));
		  break;
		case 6: // UINT32
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_uint32(val_from_bytes<uint32_t>(dptr));
		  break;
		case 0xffff: // String
		  for (unsigned idx = 0 ; idx < count ; idx += 1)
			desc->second.range[idx].set_string(val_from_bytes<QString>(dptr));
		  break;
		default:
		  break;
	    }
      } else {
      }
}

bool PTPCamera::ptp_InitiateCapture(uint32_t&rc)
{
      if (!ptp_operation_is_supported(0x100e))
	    return false;

      vector<uint32_t>params (2);
      params[0] = 0x00000000; // StorageId
      params[1] = 0x00000000; // ObjectFormatCode
      rc = ptp_command(0x100e /* InitiateCapture */, params, 0, 0, 0, 0);

      return true;
}

QTreeWidgetItem*PTPCamera::ptp_describe_camera(void)
{
      QTreeWidgetItem*item;
      QTreeWidgetItem*root = new QTreeWidgetItem;
      root->setText(0, "PTPCamera");
      root->setFirstColumnSpanned(true);

      item = new QTreeWidgetItem;
      item->setText(0, "StandardVersion");
      item->setText(1, QString("%1 (0x%2)")
		    .arg(ptp_standard_version()/100.0, 0, 'f', 2)
		    .arg(ptp_standard_version(), 4, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionID");
      item->setText(1, QString("0x%1").arg(ptp_extension_vendor(true), 8, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionVersion");
      item->setText(1, QString("%1 (0x%2)")
		    .arg(ptp_extension_version()/100.0, 0, 'f', 2)
		    .arg(ptp_extension_version(), 4, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionDesc");
      item->setText(1, ptp_extension_desc());
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "FunctionalMode");
      switch (ptp_FunctionalMode()) {
	  case 0x0000:
	    item->setText(1, "Standard Mode");
	    break;
	  case 0x0001:
	    item->setText(1, "Sleep Mode");
	    break;
	  default:
	    item->setText(1, QString("0x%1").arg(ptp_FunctionalMode(), 4, 16, QLatin1Char('0')));
	    break;
      }
      root->addChild(item);

      std::vector<QString>res;
      std::vector<code_string_t> res2;

      item = new QTreeWidgetItem;
      item->setText(0, "OperationsSupported");

      res = ptp_operations_list();
      for (unsigned idx = 0 ; idx < res.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(1, res[idx]);
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "EventsSupported");

      res = ptp_events_list();
      for (unsigned idx = 0 ; idx < res.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(1, res[idx]);
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "DevicePropertiesSupported");

      res2 = ptp_properties_list();
      for (size_t idx = 0 ; idx < res2.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(0, res2[idx].second);
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "CaptureFormats");

      res = ptp_capture_formats_list();
      for (size_t idx = 0 ; idx < res.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(0, res[idx]);
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "ImageFormats");

      res = ptp_image_formats_list();
      for (size_t idx = 0 ; idx < res.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(0, res[idx]);
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "Manufacturer");
      item->setText(1, ptp_Manufacturer());
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "Model");
      item->setText(1, ptp_Model());
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "DeviceVersion");
      item->setText(1, ptp_DeviceVersion());
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "SerialNumber");
      item->setText(1, ptp_SerialNumber());
      root->addChild(item);

      return root;
}

PTPCamera::prop_value_t::prop_value_t()
: type_code_(PTPCamera::TYPE_NONE)
{
}

PTPCamera::prop_value_t::prop_value_t(const prop_value_t&that)
{
      copy_(that);
}

PTPCamera::prop_value_t& PTPCamera::prop_value_t::operator= (const prop_value_t&that)
{
      if (this != &that) copy_(that);
      return *this;
}

void PTPCamera::prop_value_t::copy_(const prop_value_t&that)
{
      type_code_ = that.type_code_;
      switch (type_code_) {
	  case 0:
	    break;
	  case 1: // INT8
	    val_int8_ = that.val_int8_;
	    break;
	  case 2: // UINT8
	    val_uint8_ = that.val_uint8_;
	    break;
	  case 3: // INT16
	    val_int16_ = that.val_int16_;
	    break;
	  case 4: // UINT16
	    val_uint16_ = that.val_uint16_;
	    break;
	  case 5: // INT32
	    val_int32_ = that.val_int32_;
	    break;
	  case 6: // UINT32
	    val_uint32_ = that.val_uint32_;
	    break;
	  case 0xffff: // String
	    val_string_ = new QString(*that.val_string_);
	    break;
	  default:
	    assert(0);
      }
}

bool PTPCamera::prop_value_t::operator == (const PTPCamera::prop_value_t&that) const
{
      if (type_code_ != that.type_code_)
	    return false;

      switch (type_code_) {
	  case TYPE_NONE:
	    return true;
	  case TYPE_INT8:
	    return get_int8() == that.get_int8();
	  case TYPE_UINT8:
	    return get_uint8() == that.get_uint8();
	  case TYPE_INT16:
	    return get_int16() == that.get_int16();
	  case TYPE_UINT16:
	    return get_uint16() == that.get_uint16();
	  case TYPE_INT32:
	    return get_int32() == that.get_int32();
	  case TYPE_UINT32:
	    return get_uint32() == that.get_uint32();
	  case TYPE_INT64:
	    return get_int64() == that.get_int64();
	  case TYPE_UINT64:
	    return get_uint64() == that.get_uint64();
	  case TYPE_INT128:
	    return get_int128() == that.get_int128();
	  case TYPE_UINT128:
	    return get_uint128() == that.get_uint128();
	  case TYPE_STRING:
	    return get_string() == that.get_string();
      }

      return false;
}

void PTPCamera::prop_value_t::clear()
{
      switch (type_code_) {
	  case 0xffff: // String
	    delete val_string_;
	    break;
	  default:
	    break;
      }
      type_code_ = TYPE_NONE;
}

PTPCamera::prop_value_t::~prop_value_t()
{
      clear();
}

int8_t PTPCamera::prop_value_t::get_int8(void) const
{
      assert(type_code_ == TYPE_INT8);
      return val_int8_;
}

uint8_t PTPCamera::prop_value_t::get_uint8(void) const
{
      assert(type_code_ == TYPE_UINT8);
      return val_uint8_;
}

int16_t PTPCamera::prop_value_t::get_int16(void) const
{
      assert(type_code_ == TYPE_INT16);
      return val_int16_;
}

uint16_t PTPCamera::prop_value_t::get_uint16(void) const
{
      assert(type_code_ == TYPE_UINT16);
      return val_uint16_;
}

int32_t PTPCamera::prop_value_t::get_int32(void) const
{
      assert(type_code_ == TYPE_INT32);
      return val_int32_;
}

uint32_t PTPCamera::prop_value_t::get_uint32(void) const
{
      assert(type_code_ == TYPE_UINT32);
      return val_uint32_;
}

int64_t PTPCamera::prop_value_t::get_int64(void) const
{
      assert(type_code_ == TYPE_INT64);
      return val_int64_;
}

uint64_t PTPCamera::prop_value_t::get_uint64(void) const
{
      assert(type_code_ == TYPE_UINT64);
      return val_uint64_;
}

PTP_INT128_t PTPCamera::prop_value_t::get_int128(void) const
{
      assert(type_code_ == TYPE_INT128);
      return val_int128_;
}

PTP_UINT128_t PTPCamera::prop_value_t::get_uint128(void) const
{
      assert(type_code_ == TYPE_UINT128);
      return val_uint128_;
}

QString PTPCamera::prop_value_t::get_string(void) const
{
      assert(type_code_ == TYPE_STRING);
      return *val_string_;
}

void PTPCamera::prop_value_t::set_int8(int8_t val)
{
      clear();
      type_code_ = TYPE_INT8;
      val_int8_ = val;
}

void PTPCamera::prop_value_t::set_uint8(uint8_t val)
{
      clear();
      type_code_ = TYPE_UINT8;
      val_uint8_ = val;
}

void PTPCamera::prop_value_t::set_int16(int16_t val)
{
      clear();
      type_code_ = TYPE_INT16;
      val_int16_ = val;
}

void PTPCamera::prop_value_t::set_uint16(uint16_t val)
{
      clear();
      type_code_ = TYPE_UINT16;
      val_uint16_ = val;
}

void PTPCamera::prop_value_t::set_int32(int32_t val)
{
      clear();
      type_code_ = TYPE_INT32;
      val_int32_ = val;
}

void PTPCamera::prop_value_t::set_uint32(uint32_t val)
{
      clear();
      type_code_ = TYPE_UINT32;
      val_uint32_ = val;
}

void PTPCamera::prop_value_t::set_string(const QString&val)
{
      clear();
      type_code_ = TYPE_STRING;
      val_string_ = new QString(val);
}
