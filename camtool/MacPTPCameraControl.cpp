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
# include  <QTreeWidgetItem>
# include  <sstream>
# include  <iomanip>
# include  <stdlib.h>

using namespace std;

MacPTPCameraControl::MacPTPCameraControl(ICAObject dev)
: MacICACameraControl(dev),
    battery_level_(0x5001 /* PTP BatteryLevel */),
    exposure_program_(0x500e /* PTP ExposureProgramMode */),
    exposure_time_(0x500d /* PTP ExposureTime */),
    fnumber_(0x5007 /* PTP FNumber */),
    iso_    (0x500f /* PTP ExposureIndex */),
    flash_mode_(0x500c /* PTP FlashMode */),
    focus_mode_(0x500a /* PTP FocusMode */)
{
      uint32_t result_code;

      ptp_get_device_info_(result_code);

      ptp_get_property_desc_(battery_level_, result_code);
      ptp_get_property_desc_(exposure_program_, result_code);
      ptp_get_property_desc_(exposure_time_, result_code);
      ptp_get_property_desc_(fnumber_, result_code);
      ptp_get_property_desc_(iso_, result_code);
      ptp_get_property_desc_(flash_mode_, result_code);
      ptp_get_property_desc_(focus_mode_, result_code);
}

MacPTPCameraControl::~MacPTPCameraControl()
{
}

QTreeWidgetItem*MacPTPCameraControl::describe_camera(void)
{
      QTreeWidgetItem*item;
      QTreeWidgetItem*root = new QTreeWidgetItem;
      root->setText(0, "MacPTPCameraControl");
      root->setFirstColumnSpanned(true);

      item = new QTreeWidgetItem;
      item->setText(0, "StandardVersion");
      item->setText(1, QString("%1 (0x%2)")
		    .arg(standard_version_/100.0, 0, 'f', 2)
		    .arg(standard_version_, 4, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionID");
      item->setText(1, QString("0x%1").arg(vendor_extension_id_, 8, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionVersion");
      item->setText(1, QString("%1 (0x%2)")
		    .arg(vendor_extension_vers_/100.0, 0, 'f', 2)
		    .arg(vendor_extension_vers_, 4, 16, QLatin1Char('0')));
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "VendorExtensionDesc");
      item->setText(1, vendor_extension_desc_);
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "FunctionalMode");
      switch (functional_mode_) {
	  case 0x0000:
	    item->setText(1, "Standard Mode");
	    break;
	  case 0x0001:
	    item->setText(1, "Sleep Mode");
	    break;
	  default:
	    item->setText(1, QString("0x%1").arg(functional_mode_, 4, 16, QLatin1Char('0')));
	    break;
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "OperationsSupported");
      for (unsigned idx = 0 ; idx < operations_supported_.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    switch (operations_supported_[idx]) {
		case 0x1001:
		  tmp->setText(1, "GetDeviceInfo");
		  break;
		case 0x1002:
		  tmp->setText(1, "OpenSession");
		  break;
		case 0x1003:
		  tmp->setText(1, "CloseSession");
		  break;
		case 0x1009:
		  tmp->setText(1, "GetObject");
		  break;
		case 0x100a:
		  tmp->setText(1, "GetThumb");
		  break;
		case 0x100e:
		  tmp->setText(1, "InitiateCapture");
		  break;
		case 0x1014:
		  tmp->setText(1, "GetDevicePropDesc");
		  break;
		case 0x1015:
		  tmp->setText(1, "GetDevicePropValue");
		  break;
		case 0x1016:
		  tmp->setText(1, "SetDevicePropvalue");
		  break;
		case 0x101c:
		  tmp->setText(1, "InitiateOpenCapture");
		  break;
		default:
		  tmp->setText(1, QString("0x%1")
			       .arg(operations_supported_[idx],4,16));
		  break;
	    }
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "EventsSupported");
      for (unsigned idx = 0 ; idx < events_supported_.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    switch (events_supported_[idx]) {
		case 0x4002:
		  tmp->setText(1, "ObjectAdded");
		  break;
		case 0x4006:
		  tmp->setText(1, "DevicePropChanged");
		  break;
		case 0x4008:
		  tmp->setText(1, "DeviceInfoChanged");
		  break;
		case 0x400d:
		  tmp->setText(1, "CaptureComplete");
		  break;
		default:
		  tmp->setText(1, QString("0x%1")
			       .arg(events_supported_[idx],4,16));
		  break;
	    }
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "DevicePropertiesSupported");
      for (size_t idx = 0 ; idx < device_properties_supported_.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    tmp->setText(1, QString("0x%1").arg(device_properties_supported_[idx],4,16));
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "CaptureFormats");
      for (size_t idx = 0 ; idx < capture_formats_.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    switch (image_formats_[idx]) {
		case 0x3000:
		  tmp->setText(1, "Undefined non-image object");
		  break;
		case 0x3001:
		  tmp->setText(1, "Association (e.g. directory)");
		  break;
		case 0x3002:
		  tmp->setText(1, "Script (device-model specific)");
		  break;
		case 0x3006:
		  tmp->setText(1, "Digital Print Order Format (text)");
		  break;
		case 0x3800:
		  tmp->setText(1, "Unknown image object");
		  break;
		case 0x3801:
		  tmp->setText(1, "EXIF/JPEG");
		  break;
		case 0x3808:
		  tmp->setText(1, "JFIF");
		  break;
		case 0x380d:
		  tmp->setText(1, "TIFF");
		  break;
		default:
		  tmp->setText(1, QString("0x%1")
			       .arg(image_formats_[idx],4,16));
		  break;
	    }
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "ImageFormats");
      for (size_t idx = 0 ; idx < image_formats_.size() ; idx += 1) {
	    QTreeWidgetItem*tmp = new QTreeWidgetItem;
	    switch (image_formats_[idx]) {
		case 0x3000:
		  tmp->setText(1, "Undefined non-image object");
		  break;
		case 0x3001:
		  tmp->setText(1, "Association (e.g. directory)");
		  break;
		case 0x3002:
		  tmp->setText(1, "Script (device-model specific)");
		  break;
		case 0x3006:
		  tmp->setText(1, "Digital Print Order Format (text)");
		  break;
		case 0x3800:
		  tmp->setText(1, "Unknown image object");
		  break;
		case 0x3801:
		  tmp->setText(1, "EXIF/JPEG");
		  break;
		case 0x3808:
		  tmp->setText(1, "JFIF");
		  break;
		case 0x380d:
		  tmp->setText(1, "TIFF");
		  break;
		default:
		  tmp->setText(1, QString("0x%1")
			       .arg(image_formats_[idx],4,16));
		  break;
	    }
	    item->addChild(tmp);
      }
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "Manufacturer");
      item->setText(1, ptp_manufacturer_);
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "Model");
      item->setText(1, ptp_model_);
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "DeviceVersion");
      item->setText(1, device_version_);
      root->addChild(item);

      item = new QTreeWidgetItem;
      item->setText(0, "SerialNumber");
      item->setText(1, serial_number_);
      root->addChild(item);

      root->addChild(MacICACameraControl::describe_camera());

      return root;
}

template <class T> static T val_from_bytes(UInt8*&buf);

template <> static int8_t val_from_bytes<int8_t>(UInt8*&buf)
{
      int8_t val = (int8_t) buf[0];
      buf += 1;
      return val;
}

template <> static uint8_t val_from_bytes<uint8_t>(UInt8*&buf)
{
      uint8_t val = (uint8_t) buf[0];
      buf += 1;
      return val;
}

template <> static int16_t val_from_bytes<int16_t>(UInt8*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return (int16_t)val;
}

template <> static uint16_t val_from_bytes<uint16_t>(UInt8*&buf)
{
      uint16_t val = (uint16_t) buf[1];
      val <<= 8;
      val |= (uint16_t) buf[0];
      buf += 2;
      return val;
}

template <> static int32_t val_from_bytes<int32_t>(UInt8*&buf)
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

template <> static uint32_t val_from_bytes<uint32_t>(UInt8*&buf)
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

template <> static vector<uint16_t> val_from_bytes< vector<uint16_t> >(UInt8*&buf)
{
      uint32_t size = val_from_bytes<uint32_t>(buf);
      vector<uint16_t> val (size);
      for (size_t idx = 0 ; idx < val.size() ; idx += 1)
	    val[idx] = val_from_bytes<uint16_t>(buf);

      return val;
}

template<> static QString val_from_bytes<QString>(UInt8*&buf)
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

ICAError MacICACameraControl::ica_send_message_(void*buf, size_t buf_len)
{
      ICAObjectSendMessagePB msg;
      memset(&msg, 0, sizeof(msg));

      msg.object = dev_;
      msg.message.messageType = kICAMessageCameraPassThrough;
      msg.message.startByte = 0;
      msg.message.dataPtr = buf;
      msg.message.dataSize = buf_len;
      msg.message.dataType = kICATypeData;

      return ICAObjectSendMessage(&msg, 0);
}

void MacPTPCameraControl::ptp_get_device_info_(uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 1024-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1001; // GetDeviceInfo
      ptp_buf->numOfInputParams = 0;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1024;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

	// now the data[] has the DeviceInfo dataset
      UInt8*dptr = ptp_buf->data;

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
}

uint8_t MacPTPCameraControl::ptp_get_property_u8_(unsigned prop_code,
						  uint32_t&result_code)
{
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 1-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1015; // GetDevicePropValue
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = prop_code;
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1;
      ptp_buf->data[0] = 0;
      ptp_buf->data[1] = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      uint8_t val = ptp_buf->data[0];
      return val;
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

      debug_log << "ptp_set_property_u16_: commandCode=" << ptp_buf->commandCode
		<< " params[0]=" << ptp_buf->params[0]
		<< " dataSize=" << ptp_buf->dataSize
		<< " data=" << hex
		<< setw(2) << ptp_buf->data[0]
		<< setw(2) << ptp_buf->data[1]
		<< endl << flush;
	    
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

      debug_log << "ptp_set_property_u32_: commandCode=" << ptp_buf->commandCode
		<< " params[0]=" << ptp_buf->params[0]
		<< " dataSize=" << ptp_buf->dataSize
		<< " data=" << hex
		<< setw(2) << (unsigned)ptp_buf->data[0]
		<< setw(2) << (unsigned)ptp_buf->data[1]
		<< setw(2) << (unsigned)ptp_buf->data[2]
		<< setw(2) << (unsigned)ptp_buf->data[3]
		<< dec << endl << flush;

      ICAError err = ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;
      if (err != 0) result_code = -1;
}

MacPTPCameraControl::prop_desc_t::prop_desc_t(uint16_t prop_code)
{
      prop_code_ = prop_code;
      type_code_ = 0;
      set_flag_ = false;
      range_flag_ = false;
}

MacPTPCameraControl::prop_desc_t::~prop_desc_t()
{
      switch (type_code_) {
	  case 1:
	    if (enum_int8_) delete enum_int8_;
	    break;
	  case 2:
	    if (enum_uint8_) delete enum_uint8_;
	    break;
	  case 3:
	    if (enum_int16_) delete enum_int16_;
	    break;
	  case 4:
	    if (enum_uint16_) delete enum_uint16_;
	    break;
	  case 5:
	    if (enum_int32_) delete enum_int32_;
	    break;
	  case 6:
	    if (enum_uint32_) delete enum_uint32_;
	    break;
	  default:
	    break;
      }
}

int MacPTPCameraControl::prop_desc_t::get_enum_count() const
{
      switch (type_code_) {
	  case 4:
	    if (enum_uint16_) return enum_uint16_->size();
	    else return 0;
	  case 6:
	    if (enum_uint32_) return enum_uint32_->size();
	    else return 0;
	  default:
	    return 0;
      }
      return -1;
}

template<> uint8_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint8_t>(int idx)
{
      assert(type_code_ == 2);
      assert(enum_uint8_ != 0);
      assert(range_flag_ == false);
      assert((int)enum_uint8_->size() > idx);
      return (*enum_uint8_)[idx];
}

template<> uint16_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint16_t>(int idx)
{
      assert(type_code_ == 4);
      assert(enum_uint16_ != 0);
      assert((int)enum_uint16_->size() > idx);
      return (*enum_uint16_)[idx];
}

template<> uint32_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint32_t>(int idx)
{
      assert(type_code_ == 6);
      assert(enum_uint32_ != 0);
      assert((int)enum_uint32_->size() > idx);
      return (*enum_uint32_)[idx];
}

void MacPTPCameraControl::prop_desc_t::set_type_code(uint16_t code)
{
      assert(type_code_ == 0);
      type_code_ = code;
}

void MacPTPCameraControl::prop_desc_t::set_flag(bool flag)
{
      set_flag_ = flag;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int8_t>(int8_t val)
{
      assert(type_code_ == 1);
      fact_int8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint8_t>(uint8_t val)
{
      assert(type_code_ == 2);
      fact_uint8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int16_t>(int16_t val)
{
      assert(type_code_ == 3);
      fact_int16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint16_t>(uint16_t val)
{
      assert(type_code_ == 4);
      fact_uint16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int32_t>(int32_t val)
{
      assert(type_code_ == 5);
      fact_int32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint32_t>(uint32_t val)
{
      assert(type_code_ == 6);
      fact_uint32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_range<int8_t>(int8_t val_min, int8_t val_max, int8_t step)
{
      assert(type_code_ == 1);
      range_flag_ = true;
      enum_int8_ = new std::vector<int8_t> (3);
      (*enum_int8_)[0] = val_min;
      (*enum_int8_)[1] = val_max;
      (*enum_int8_)[2] = step;
}

template <> void MacPTPCameraControl::prop_desc_t::set_range<uint8_t>(uint8_t val_min, uint8_t val_max, uint8_t step)
{
      assert(type_code_ == 2);
      range_flag_ = true;
      enum_uint8_ = new std::vector<uint8_t> (3);
      (*enum_uint8_)[0] = val_min;
      (*enum_uint8_)[1] = val_max;
      (*enum_uint8_)[2] = step;
}

template <> void MacPTPCameraControl::prop_desc_t::get_range<uint8_t>(uint8_t&val_min, uint8_t&val_max, uint8_t&step)
{
      assert(type_code_ == 2);
      assert(range_flag_);
      assert(enum_uint8_->size() == 3);
      val_min = (*enum_uint8_)[0];
      val_max = (*enum_uint8_)[1];
      step    = (*enum_uint8_)[2];
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int8_t>(const std::vector<int8_t>&ref)
{
      assert(type_code_ == 1);
      enum_int8_ = new std::vector<int8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint8_t>(const std::vector<uint8_t>&ref)
{
      assert(type_code_ == 2);
      enum_uint8_ = new std::vector<uint8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int16_t>(const std::vector<int16_t>&ref)
{
      assert(type_code_ == 3);
      enum_int16_ = new std::vector<int16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint16_t>(const std::vector<uint16_t>&ref)
{
      assert(type_code_ == 4);
      enum_uint16_ = new std::vector<uint16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int32_t>(const std::vector<int32_t>&ref)
{
      assert(type_code_ == 5);
      enum_int32_ = new std::vector<int32_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint32_t>(const std::vector<uint32_t>&ref)
{
      assert(type_code_ == 6);
      enum_uint32_ = new std::vector<uint32_t> (ref);
}

void MacPTPCameraControl::ptp_get_property_desc_(prop_desc_t&desc,
						 uint32_t&result_code)
{
      const size_t data_buf_size = 1024;
      unsigned char buf[sizeof(ICAPTPPassThroughPB) + data_buf_size-1];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x1014; // GetDevicePropDesc
      ptp_buf->numOfInputParams = 1;
      ptp_buf->params[0] = desc.get_property_code();
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruReceive;
      ptp_buf->dataSize = 1024;

      ica_send_message_(ptp_buf, sizeof buf);
      result_code = ptp_buf->resultCode;

      UInt8*dptr = ptp_buf->data;

	// data[0]
	// data[1] -- Property code back
      uint16_t prop = val_from_bytes<uint16_t>(dptr);
      assert(prop == desc.get_property_code());

	// data[2]
	// data[3] -- data type code
      uint16_t dtype = val_from_bytes<uint16_t>(dptr);
      desc.set_type_code(dtype);

	// data[4] -- GetSet flag
      uint8_t get_set_flag = val_from_bytes<uint8_t>(dptr);
      desc.set_flag(get_set_flag);

	// Starting at data[5]...
	//   -- Factory Default value
	//   -- Current value  (we ignore the current value here)
      switch (dtype) {
	  case 0:  // UNDEFINED
	  case 1:  // INT8
	    desc.set_factory_default(val_from_bytes<int8_t>(dptr));
	    val_from_bytes<int8_t>(dptr);
	    break;
	  case 2:  // UINT8
	    desc.set_factory_default(val_from_bytes<uint8_t>(dptr));
	    val_from_bytes<uint8_t>(dptr);
	    break;
	  case 3:  // INT16
	    desc.set_factory_default(val_from_bytes<int16_t>(dptr));
	    val_from_bytes<int16_t>(dptr);
	    break;
	  case 4:  // UINT16
	    desc.set_factory_default(val_from_bytes<uint16_t>(dptr));
	    val_from_bytes<uint16_t>(dptr);
	    break;
	  case 5:  // INT32
	    desc.set_factory_default(val_from_bytes<int32_t>(dptr));
	    val_from_bytes<int32_t>(dptr);
	    break;
	  case 6:  // UINT32
	    desc.set_factory_default(val_from_bytes<uint32_t>(dptr));
	    val_from_bytes<uint32_t>(dptr);
	    break;
	  case 7:  // INT64
	  case 8:  // UINT64
	  case 9:  // INT128;
	  case 10: // UINT128;
	  default:
	    break;
      }

	// The form flag (2==ENUM)
      uint8_t form_flag = *dptr++;

      if (form_flag == 1) { // RANGE
	    switch (dtype) {
		case 1: { // INT8
		      int8_t val_min = val_from_bytes<int8_t>(dptr);
		      int8_t val_max = val_from_bytes<int8_t>(dptr);
		      int8_t step    = val_from_bytes<int8_t>(dptr);
		      desc.set_range(val_min, val_max, step);
		      break;
		}
		case 2: { // UINT8
		      uint8_t val_min = val_from_bytes<uint8_t>(dptr);
		      uint8_t val_max = val_from_bytes<uint8_t>(dptr);
		      uint8_t step    = val_from_bytes<uint8_t>(dptr);
		      desc.set_range(val_min, val_max, step);
		      break;
		}
		default: {
		      break;
		}
	    }

      } else if (form_flag == 2) { // ENUM
	    uint16_t count = val_from_bytes<uint16_t>(dptr);

	    switch (dtype) {
		case 1: { // INT8
		      vector<int8_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int8_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 2: { // UINT8
		      vector<uint8_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint8_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 3: { // INT16
		      vector<int16_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int16_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 4: { // UINT16
		      vector<uint16_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint16_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 5: { // INT32
		      vector<int32_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<int32_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		case 6: { // UINT32
		      vector<uint32_t> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1)
			    enum_array[idx] = val_from_bytes<uint32_t>(dptr);

		      desc.set_enum_vector(enum_array);
		      break;
		}
		default:
		  break;
	    }
      } else {
      }
}

float MacPTPCameraControl::battery_level(void)
{
	// The BatteryLevel is by definition (PTP) a UINT8.
      assert(battery_level_.get_type_code() == 2);

      uint32_t rc;
      uint8_t val = ptp_get_property_u8_(battery_level_.get_property_code(),rc);
      uint8_t val_min, val_max;
      if (battery_level_.is_range()) {
	    uint8_t val_stp;
	    battery_level_.get_range(val_min, val_max, val_stp);
      } else {
	    val_min = battery_level_.get_enum_index<uint8_t>(0);
	    val_max = val_min;
	    for (int idx = 1; idx < battery_level_.get_enum_count(); idx += 1) {
		  uint8_t tmp = battery_level_.get_enum_index<uint8_t>(idx);
		  if (tmp < val_min)
			val_min = tmp;
		  if (tmp > val_max)
			val_max = tmp;
	    }
      }

      if (val >= val_max)
	    return 100.0;
      if (val <= val_min)
	    return 0.0;
      val -= val_min;
      val_max -= val_min;
      return (val * 100.0) / (val_max * 1.0);
}

void MacPTPCameraControl::get_exposure_program_index(vector<string>&values)
{
      values.resize(exposure_program_.get_enum_count());
	// The ExposureProgramMode is by definition (PTP) a UINT16.
      assert(exposure_program_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    uint16_t value = exposure_program_.get_enum_index<uint16_t>(idx);
	    switch (value) {
		case 0x0000: // UNDEFINED
		  tmp << "NONE" << ends;
		  break;
		case 0x0001: // Manual
		  tmp << "Manual" << ends;
		  break;
		case 0x0002: // Automatic
		  tmp << "Automatic" << ends;
		  break;
		case 0x0003: // Aperture Priority
		  tmp << "Aperture Priority" << ends;
		  break;
		case 0x0004: // Shutter Priority
		  tmp << "Shutter Priority" << ends;
		  break;
		case 0x0005: // Program Creative (greater depth of field)
		  tmp << "Program Creative" << ends;
		  break;
		case 0x0006: // Program Action (faster shutter)
		  tmp << "Program Action" << ends;
		  break;
		case 0x0007: // Portrait
		  tmp << "Portrait" << ends;
		  break;
		default:
		  tmp << "Vendor program: 0x"
		      << setw(4) << hex << value << ends;
		  break;
	    }
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_exposure_program_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(exposure_program_.get_property_code(), rc);
      for (int idx = 0 ; idx < exposure_program_.get_enum_count() ; idx += 1) {
	    if (val == exposure_program_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_exposure_program_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= exposure_program_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(exposure_program_.get_property_code(),
			    exposure_program_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_exposure_program_ok()
{
      return exposure_program_.set_ok();
}

void MacPTPCameraControl::get_exposure_time_index(vector<string>&values)
{
      values.resize(exposure_time_.get_enum_count());
	// The ExposureTime is by definition (PTP) a UINT32.
      assert(exposure_time_.get_type_code() == 6);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    tmp.setf(ios_base::fixed);
	    uint32_t val = exposure_time_.get_enum_index<uint32_t>(idx);
	    if (val == 0xffffffff) {
		  tmp << "Bulb" << ends;
	    } else {
		  tmp << setprecision(1) << setw(10)
		      << (val / 10.0) << "ms" << ends;
	    }
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_exposure_time_index()
{
      uint32_t rc;
      uint32_t val = ptp_get_property_u32_(exposure_time_.get_property_code(), rc);
      for (int idx = 0 ; idx < exposure_time_.get_enum_count() ; idx += 1) {
	    if (val == exposure_time_.get_enum_index<uint32_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_exposure_time_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= exposure_time_.get_enum_count())
	    use_index = 0;

      uint32_t val = exposure_time_.get_enum_index<uint32_t>(use_index);
      uint32_t rc;
      ptp_set_property_u32_(exposure_time_.get_property_code(), val, rc);

      debug_log << "set_exposure_Time_index: index=" << use_index
		<< ", val=" << hex << val
		<< ", rc=" << rc
		<< dec << endl;
}

bool MacPTPCameraControl::set_exposure_time_ok()
{
      return exposure_time_.set_ok();
}

void MacPTPCameraControl::get_fnumber_index(vector<string>&values)
{
      values.resize(fnumber_.get_enum_count());
	// The FNumber is by definition (PTP) a UINT16.
      assert(fnumber_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    tmp << "f/" << (fnumber_.get_enum_index<uint16_t>(idx) / 100.0) << ends;
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_fnumber_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(fnumber_.get_property_code(), rc);
      for (int idx = 0 ; idx < fnumber_.get_enum_count() ; idx += 1) {
	    if (val == fnumber_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_fnumber_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= fnumber_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(fnumber_.get_property_code(),
			    fnumber_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_fnumber_ok()
{
      return fnumber_.set_ok();
}

void MacPTPCameraControl::get_iso_index(vector<string>&values)
{
      values.resize(iso_.get_enum_count());
	// The FNumber is by definition (PTP) a UINT16.
      assert(iso_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    tmp << iso_.get_enum_index<uint16_t>(idx) << ends;
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_iso_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(iso_.get_property_code(), rc);
      for (int idx = 0 ; idx < iso_.get_enum_count() ; idx += 1) {
	    if (val == iso_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_iso_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= iso_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(iso_.get_property_code(),
			    iso_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_iso_ok()
{
      return iso_.set_ok();
}

void MacPTPCameraControl::get_flash_mode_index(vector<string>&values)
{
      values.resize(flash_mode_.get_enum_count());
	// The FlashMode is by definition (PTP) a UINT16.
      assert(flash_mode_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    uint16_t val_code = flash_mode_.get_enum_index<uint16_t>(idx);
	    switch (val_code) {
		case 0x0000:
		  tmp << "Undefined" << ends;
		  break;
		case 0x0001:
		  tmp << "Auto" << ends;
		  break;
		case 0x0002:
		  tmp << "Flash Off" << ends;
		  break;
		case 0x0003:
		  tmp << "Fill Flash" << ends;
		  break;
		case 0x0004:
		  tmp << "Red-eye Auto" << ends;
		  break;
		case 0x0005:
		  tmp << "Red-eye Fill" << ends;
		  break;
		case 0x0006:
		  tmp << "External Sync" << ends;
		  break;
		default:
		  if (val_code < 0x8000)
			tmp << "Reserved-" << hex << val_code << ends;
		  else
			tmp << "Vendor-" << hex << val_code << ends;
		  break;
	    }
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_flash_mode_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(flash_mode_.get_property_code(), rc);
      for (int idx = 0 ; idx < flash_mode_.get_enum_count() ; idx += 1) {
	    if (val == flash_mode_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_flash_mode_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= flash_mode_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(flash_mode_.get_property_code(),
			    flash_mode_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_flash_mode_ok()
{
      return flash_mode_.set_ok();
}


void MacPTPCameraControl::get_focus_mode_index(vector<string>&values)
{
      values.resize(focus_mode_.get_enum_count());
	// The FocusMode is by definition (PTP) a UINT16.
      assert(focus_mode_.get_type_code() == 4);

      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    ostringstream tmp;
	    uint16_t val_code = focus_mode_.get_enum_index<uint16_t>(idx);
	    switch (val_code) {
		case 0x0000:
		  tmp << "Undefined" << ends;
		  break;
		case 0x0001:
		  tmp << "Manual" << ends;
		  break;
		case 0x0002:
		  tmp << "Automatic" << ends;
		  break;
		case 0x0003:
		  tmp << "Automatic/Macro" << ends;
		  break;
		default:
		  if (val_code < 0x8000)
			tmp << "Reserved-" << hex << val_code << ends;
		  else
			tmp << "Vendor-" << hex << val_code << ends;
		  break;
	    }
	    values[idx] = tmp.str();
      }
}

int MacPTPCameraControl::get_focus_mode_index()
{
      uint32_t rc;
      uint16_t val = ptp_get_property_u16_(focus_mode_.get_property_code(), rc);
      for (int idx = 0 ; idx < focus_mode_.get_enum_count() ; idx += 1) {
	    if (val == focus_mode_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_focus_mode_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= focus_mode_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16_(focus_mode_.get_property_code(),
			    focus_mode_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_focus_mode_ok()
{
      return focus_mode_.set_ok();
}

int MacPTPCameraControl::debug_property_get(unsigned prop,
					    unsigned dtype,
					    unsigned long&value)
{
      uint32_t rc = 0;
      switch (dtype) {
	  case 0x0004: // UINT16
	    value = ptp_get_property_u16_(prop, rc);
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
