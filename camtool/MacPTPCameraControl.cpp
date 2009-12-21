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
# include  "ptp_misc.h"
# include  <QTreeWidgetItem>
# include  <sstream>
# include  <iomanip>
# include  <stdlib.h>

using namespace std;

MacPTPCameraControl::MacPTPCameraControl(ICAObject dev)
: MacICACameraControl(dev),
    battery_level_(0x5001 /* PTP BatteryLevel */),
    image_size_(0x5003    /* PTP ImageSize */),
    white_balance_(0x5005 /* PTP WhiteBalance */),
    exposure_program_(0x500e /* PTP ExposureProgramMode */),
    exposure_time_(0x500d /* PTP ExposureTime */),
    fnumber_(0x5007 /* PTP FNumber */),
    iso_    (0x500f /* PTP ExposureIndex */),
    flash_mode_(0x500c /* PTP FlashMode */),
    focus_mode_(0x500a /* PTP FocusMode */)
{
      uint32_t result_code;

      result_code = ptp_get_device_info();

      ptp_get_property_desc_(battery_level_, result_code);
      ptp_get_property_desc_(image_size_, result_code);
      ptp_get_property_desc_(white_balance_, result_code);
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

      root->addChild(MacICACameraControl::describe_camera());

      return root;
}

CameraControl::capture_resp_t MacPTPCameraControl::capture_image(void)
{
      if (!ptp_operation_is_supported(0x100e))
	    return CAP_NOT_SUPPORTED;

      unsigned char buf[sizeof(ICAPTPPassThroughPB) + 0];
      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)buf;

      ptp_buf->commandCode = 0x100e; // InitiateCapture
      ptp_buf->numOfInputParams = 2;
      ptp_buf->params[0] = 0x00000000; // StorageID
      ptp_buf->params[1] = 0x00000000; // ObjectformatCode
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = kICACameraPassThruNotUsed;
      ptp_buf->dataSize = 0;

      ica_send_message_(ptp_buf, sizeof buf);
      uint32_t result_code = ptp_buf->resultCode;

      debug_log << TIMESTAMP << ": InitiateCapture (result_code="
		<< hex << result_code << dec << ")" << endl << flush;

      return CAP_OK;
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

uint32_t MacPTPCameraControl::ptp_command(uint16_t command,
				      const std::vector<uint32_t>&parms,
				      const unsigned char*send, size_t nsend,
				      unsigned char*recv, size_t nrecv)
{
      size_t pb_size = sizeof(ICAPTPPassThroughPB);
      uint32_t usage_mode = kICACameraPassThruNotUsed;

      if (nsend > 0) {
	    usage_mode = kICACameraPassThruSend;
	    pb_size += nsend - 1;
      } else if (nrecv > 0) {
	    usage_mode = kICACameraPassThruReceive;
	    pb_size += nrecv - 1;
      }

      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)malloc(pb_size);
      memset(ptp_buf, 0, pb_size);
      ptp_buf->commandCode = command;
      ptp_buf->numOfInputParams = parms.size();
      for (size_t idx = 0 ; idx < parms.size() ; idx += 1) {
	    ptp_buf->params[idx] = parms[idx];
      }
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = usage_mode;
      ptp_buf->dataSize = nsend + nrecv;

      if (nsend > 0) {
	    memcpy(ptp_buf->data, send, nsend);
      }

      /* ICAError irc = */ ica_send_message_(ptp_buf, pb_size);

      uint32_t result_code = ptp_buf->resultCode;
      if (nrecv > 0) {
	    memcpy(recv, ptp_buf->data, nrecv);
      }

      free(ptp_buf);

      return result_code;
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

MacPTPCameraControl::prop_desc_t::prop_desc_t(uint16_t prop_code)
{
      prop_code_ = prop_code;
      type_code_ = 0;
      support_flag_ = false;
      set_flag_ = false;
      range_flag_ = false;
}

MacPTPCameraControl::prop_desc_t::~prop_desc_t()
{
      clear_property_support();
}

void MacPTPCameraControl::prop_desc_t::clear_property_support()
{
      if (support_flag_ == false)
	    return;

      support_flag_ = false;

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
	  case 1: // INT8
	    if (enum_int8_) return enum_int8_->size();
	    else return 0;
	  case 2: // UINT8
	    if (enum_uint8_) return enum_uint8_->size();
	    else return 0;
	  case 3: // INT16
	    if (enum_int16_) return enum_int16_->size();
	    else return 0;
	  case 4: // UINT16
	    if (enum_uint16_) return enum_uint16_->size();
	    else return 0;
	  case 5: // INT32
	    if (enum_int32_) return enum_int32_->size();
	    else return 0;
	  case 6: // UINT32
	    if (enum_uint32_) return enum_uint32_->size();
	    else return 0;
	  case 0xffff: // String
	    if (enum_string_) return enum_string_->size();
	    else return 0;
	  default:
	    return 0;
      }
      return -1;
}

template<> int8_t MacPTPCameraControl::prop_desc_t::get_current<int8_t>(void)
{
      assert(support_flag_ && type_code_ == 1);
      return current_uint8_;
}

template<> uint8_t MacPTPCameraControl::prop_desc_t::get_current<uint8_t>(void)
{
      assert(support_flag_ && type_code_ == 2);
      return current_uint8_;
}

template<> int16_t MacPTPCameraControl::prop_desc_t::get_current<int16_t>(void)
{
      assert(support_flag_ && type_code_ == 3);
      return current_uint16_;
}

template<> uint16_t MacPTPCameraControl::prop_desc_t::get_current<uint16_t>(void)
{
      assert(support_flag_ && type_code_ == 4);
      return current_uint16_;
}

template<> int32_t MacPTPCameraControl::prop_desc_t::get_current<int32_t>(void)
{
      assert(support_flag_ && type_code_ == 5);
      return current_uint32_;
}

template<> uint32_t MacPTPCameraControl::prop_desc_t::get_current<uint32_t>(void)
{
      assert(support_flag_ && type_code_ == 6);
      return current_uint32_;
}

template<> QString MacPTPCameraControl::prop_desc_t::get_current<QString>(void)
{
      assert(support_flag_ && type_code_ == 0xffff);
      return *current_string_;
}

template<> uint8_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint8_t>(int idx)
{
      assert(support_flag_);
      assert(type_code_ == 2);
      assert(enum_uint8_ != 0);
      assert(range_flag_ == false);
      assert((int)enum_uint8_->size() > idx);
      return (*enum_uint8_)[idx];
}

template<> uint16_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint16_t>(int idx)
{
      assert(support_flag_);
      assert(type_code_ == 4);
      assert(enum_uint16_ != 0);
      assert((int)enum_uint16_->size() > idx);
      return (*enum_uint16_)[idx];
}

template<> uint32_t MacPTPCameraControl::prop_desc_t::get_enum_index<uint32_t>(int idx)
{
      assert(support_flag_);
      assert(type_code_ == 6);
      assert(enum_uint32_ != 0);
      assert((int)enum_uint32_->size() > idx);
      return (*enum_uint32_)[idx];
}

template<> QString MacPTPCameraControl::prop_desc_t::get_enum_index<QString>(int idx)
{
      assert(support_flag_);
      assert(type_code_ == 0xffff);
      assert(enum_string_ != 0);
      assert((int)enum_string_->size() > idx);
      return (*enum_string_)[idx];
}

void MacPTPCameraControl::prop_desc_t::set_type_code(uint16_t code)
{
      assert(type_code_ == 0);
      support_flag_ = true;
      type_code_ = code;
      switch (type_code_) {
	  case 0xffff: // String
	    fact_string_ = 0;
	    current_string_ = 0;
	    break;
      }
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
      assert(support_flag_ && type_code_ == 4);
      fact_uint16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<int32_t>(int32_t val)
{
      assert(support_flag_ && type_code_ == 5);
      fact_int32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<uint32_t>(uint32_t val)
{
      assert(support_flag_ && type_code_ == 6);
      fact_uint32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_factory_default<QString>(QString val)
{
      assert(support_flag_ && type_code_ == 0xffff);
      if (fact_string_ == 0)
	    fact_string_ = new QString(val);
      else
	    *fact_string_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<int8_t>(int8_t val)
{
      assert(support_flag_ && type_code_ == 1);
      current_int8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<uint8_t>(uint8_t val)
{
      assert(support_flag_ && type_code_ == 2);
      current_uint8_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<int16_t>(int16_t val)
{
      assert(support_flag_ && type_code_ == 3);
      current_int16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<uint16_t>(uint16_t val)
{
      assert(support_flag_ && type_code_ == 4);
      current_uint16_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<int32_t>(int32_t val)
{
      assert(support_flag_ && type_code_ == 5);
      current_int32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<uint32_t>(uint32_t val)
{
      assert(support_flag_ && type_code_ == 6);
      current_uint32_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_current<QString>(QString val)
{
      assert(support_flag_ && type_code_ == 0xffff);
      if (current_string_ == 0)
	    current_string_ = new QString(val);
      else
	    *current_string_ = val;
}

template <> void MacPTPCameraControl::prop_desc_t::set_range<int8_t>(int8_t val_min, int8_t val_max, int8_t step)
{
      assert(support_flag_ && type_code_ == 1);
      range_flag_ = true;
      enum_int8_ = new std::vector<int8_t> (3);
      (*enum_int8_)[0] = val_min;
      (*enum_int8_)[1] = val_max;
      (*enum_int8_)[2] = step;
}

template <> void MacPTPCameraControl::prop_desc_t::set_range<uint8_t>(uint8_t val_min, uint8_t val_max, uint8_t step)
{
      assert(support_flag_ && type_code_ == 2);
      range_flag_ = true;
      enum_uint8_ = new std::vector<uint8_t> (3);
      (*enum_uint8_)[0] = val_min;
      (*enum_uint8_)[1] = val_max;
      (*enum_uint8_)[2] = step;
}

template <> void MacPTPCameraControl::prop_desc_t::get_range<uint8_t>(uint8_t&val_min, uint8_t&val_max, uint8_t&step)
{
      assert(support_flag_ && type_code_ == 2);
      assert(range_flag_);
      assert(enum_uint8_->size() == 3);
      val_min = (*enum_uint8_)[0];
      val_max = (*enum_uint8_)[1];
      step    = (*enum_uint8_)[2];
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int8_t>(const std::vector<int8_t>&ref)
{
      assert(support_flag_ && type_code_ == 1);
      enum_int8_ = new std::vector<int8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint8_t>(const std::vector<uint8_t>&ref)
{
      assert(support_flag_ && type_code_ == 2);
      enum_uint8_ = new std::vector<uint8_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int16_t>(const std::vector<int16_t>&ref)
{
      assert(support_flag_ && type_code_ == 3);
      enum_int16_ = new std::vector<int16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint16_t>(const std::vector<uint16_t>&ref)
{
      assert(support_flag_ && type_code_ == 4);
      enum_uint16_ = new std::vector<uint16_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<int32_t>(const std::vector<int32_t>&ref)
{
      assert(support_flag_ && type_code_ == 5);
      enum_int32_ = new std::vector<int32_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<uint32_t>(const std::vector<uint32_t>&ref)
{
      assert(support_flag_ && type_code_ == 6);
      enum_uint32_ = new std::vector<uint32_t> (ref);
}

template <> void MacPTPCameraControl::prop_desc_t::set_enum_vector<QString>(const std::vector<QString>&ref)
{
      assert(support_flag_ && type_code_ == 0xffff);
      enum_string_ = new std::vector<QString> (ref);
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

      if (result_code != 0x2001) {
	    debug_log << "GetDevicePropDesc(0x"
		      << hex << desc.get_property_code() << ")"
		      << " --result_code--> 0x" << hex << result_code
		      << dec << endl << flush;
	    desc.clear_property_support();
	    return;
      }

      UInt8*dptr = ptp_buf->data;

	// data[0]
	// data[1] -- Property code back
      uint16_t prop = val_from_bytes<uint16_t>(dptr);
      assert(prop == desc.get_property_code());

	// data[2]
	// data[3] -- data type code
	// Setting the type code for the property also turns its
	// support flag on.
      uint16_t dtype = val_from_bytes<uint16_t>(dptr);
      desc.set_type_code(dtype);

	// data[4] -- GetSet flag
      uint8_t get_set_flag = val_from_bytes<uint8_t>(dptr);
      desc.set_flag(get_set_flag);

	// Starting at data[5]...
	//   -- Factory Default value
	//   -- Current value
      switch (dtype) {
	  case 0:  // UNDEFINED
	  case 1:  // INT8
	    desc.set_factory_default(val_from_bytes<int8_t>(dptr));
	    desc.set_current(val_from_bytes<int8_t>(dptr));
	    break;
	  case 2:  // UINT8
	    desc.set_factory_default(val_from_bytes<uint8_t>(dptr));
	    desc.set_current(val_from_bytes<uint8_t>(dptr));
	    break;
	  case 3:  // INT16
	    desc.set_factory_default(val_from_bytes<int16_t>(dptr));
	    desc.set_current(val_from_bytes<int16_t>(dptr));
	    break;
	  case 4:  // UINT16
	    desc.set_factory_default(val_from_bytes<uint16_t>(dptr));
	    desc.set_current(val_from_bytes<uint16_t>(dptr));
	    break;
	  case 5:  // INT32
	    desc.set_factory_default(val_from_bytes<int32_t>(dptr));
	    desc.set_current(val_from_bytes<int32_t>(dptr));
	    break;
	  case 6:  // UINT32
	    desc.set_factory_default(val_from_bytes<uint32_t>(dptr));
	    desc.set_current(val_from_bytes<uint32_t>(dptr));
	    break;
	  case 7:  // INT64
	  case 8:  // UINT64
	  case 9:  // INT128;
	  case 10: // UINT128;
	    break;
	  case 0xffff: // String
	    desc.set_factory_default(val_from_bytes<QString>(dptr));
	    desc.set_current(val_from_bytes<QString>(dptr));
	    break;
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
		case 0xffff: { // String
		      debug_log << "**** String enum with " << count << " items" << endl;
		      vector<QString> enum_array (count);
		      for (unsigned idx = 0 ; idx < count ; idx += 1) {
			    enum_array[idx] = val_from_bytes<QString>(dptr);
			    debug_log << "   " << idx << ": "
				      << enum_array[idx].toStdString() << endl;
		      }

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
      if (battery_level_.test_property_support() == false)
	    return -1;

	// The BatteryLevel is by definition (PTP) a UINT8.
      assert(battery_level_.get_type_code() == 2);

      uint32_t rc;
      uint8_t val = ptp_get_property_u8(battery_level_.get_property_code(),rc);
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

void MacPTPCameraControl::get_image_size_index(vector<string>&values)
{
      if (image_size_.test_property_support() == false) {
	    values.clear();
	    return;
      }

      values.resize(image_size_.get_enum_count());
	// The ImageSize is by definition (PTP) a String
      assert(image_size_.get_type_code() == 0xffff);

      debug_log << "**** get_image_size_index<string> count=" << values.size() << endl;
      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    QString tmp = image_size_.get_enum_index<QString>(idx);
	    values[idx] = tmp.toStdString();
	    debug_log << "   " << idx << ": " << values[idx] << endl;
      }
}

int MacPTPCameraControl::get_image_size_index()
{
      if (image_size_.test_property_support() == false)
	    return -1;

      QString val = image_size_.get_current<QString>();
      for (int idx = 0 ; idx < image_size_.get_enum_count() ; idx += 1) {
	    if (val == image_size_.get_enum_index<QString>(idx))
		  return idx;
      }

      return -1;
}

void MacPTPCameraControl::set_image_size_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= image_size_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_string(image_size_.get_property_code(),
			      image_size_.get_enum_index<QString>(use_index),
			      rc);
}

bool MacPTPCameraControl::set_image_size_ok()
{
      return image_size_.set_ok();
}


void MacPTPCameraControl::get_exposure_program_index(vector<string>&values)
{
      if (exposure_program_.test_property_support() == false) {
	    values.clear();
	    return;
      }

      values.resize(exposure_program_.get_enum_count());
	// The ExposureProgramMode is by definition (PTP) a UINT16.
      assert(exposure_program_.get_type_code() == 4);

      uint16_t use_code = exposure_program_.get_property_code();
      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    uint16_t value = exposure_program_.get_enum_index<uint16_t>(idx);
	    values[idx] = ptp_property_value16_string(use_code, value,
						      ptp_extension_vendor());
      }
}

int MacPTPCameraControl::get_exposure_program_index()
{
      if (exposure_program_.test_property_support() == false)
	    return -1;

      uint16_t val = exposure_program_.get_current<uint16_t>();
      for (int idx = 0 ; idx < exposure_program_.get_enum_count() ; idx += 1) {
	    if (val == exposure_program_.get_enum_index<uint16_t>(idx))
		  return idx;
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
      ptp_set_property_u16(exposure_program_.get_property_code(),
			   exposure_program_.get_enum_index<uint16_t>(use_index),
			   rc);
      exposure_program_.set_current(exposure_program_.get_enum_index<uint16_t>(use_index));
}

bool MacPTPCameraControl::set_exposure_program_ok()
{
      return exposure_program_.set_ok();
}

void MacPTPCameraControl::get_exposure_time_index(vector<string>&values)
{
      if (exposure_time_.test_property_support() == false) {
	    values.clear();
	    return;
      }

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
      if (exposure_time_.test_property_support() == false)
	    return -1;

      uint32_t val = exposure_time_.get_current<uint32_t>();
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
      if (exposure_time_.get_current<uint32_t>() == val)
	    return;

      uint32_t rc;
      ptp_set_property_u32(exposure_time_.get_property_code(), val, rc);
      exposure_time_.set_current(val);

      debug_log << "set_exposure_time_index: index=" << use_index
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
      if (fnumber_.test_property_support() == false) {
	    values.clear();
	    return;
      }

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
      if (fnumber_.test_property_support() == false)
	    return -1;

      uint16_t val = fnumber_.get_current<uint16_t>();
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

      uint16_t use_val = fnumber_.get_enum_index<uint16_t>(use_index);
      if (fnumber_.get_current<uint16_t>() == use_val)
	    return;

      debug_log << "Setting fnumber from "
		<< hex << fnumber_.get_current<uint16_t>()
		<< " to " << hex << use_val << endl << flush;

      uint32_t rc;
      ptp_set_property_u16(fnumber_.get_property_code(), use_val, rc);
      fnumber_.set_current(use_val);
}

bool MacPTPCameraControl::set_fnumber_ok()
{
      return fnumber_.set_ok();
}

void MacPTPCameraControl::get_iso_index(vector<string>&values)
{
      if (iso_.test_property_support() == false) {
	    values.clear();
	    return;
      }

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
      if (iso_.test_property_support() == false)
	    return -1;

      uint16_t val = iso_.get_current<uint16_t>();
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

      uint16_t use_val = iso_.get_enum_index<uint16_t>(use_index);
      if (iso_.get_current<uint16_t>() == use_val)
	    return;

      uint32_t rc;
      ptp_set_property_u16(iso_.get_property_code(), use_val, rc);
      iso_.set_current(use_val);
}

bool MacPTPCameraControl::set_iso_ok()
{
      return iso_.set_ok();
}

void MacPTPCameraControl::get_flash_mode_index(vector<string>&values)
{
      if (flash_mode_.test_property_support() == false) {
	    values.clear();
	    return;
      }

      values.resize(flash_mode_.get_enum_count());
	// The FlashMode is by definition (PTP) a UINT16.
      assert(flash_mode_.get_type_code() == 4);

      uint16_t prop_code = flash_mode_.get_property_code();
      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    uint16_t val_code = flash_mode_.get_enum_index<uint16_t>(idx);
	    values[idx] = ptp_property_value16_string(prop_code, val_code,
						      ptp_extension_vendor());
      }
}

int MacPTPCameraControl::get_flash_mode_index()
{
      if (flash_mode_.test_property_support() == false)
	    return -1;

      uint16_t val = flash_mode_.get_current<uint16_t>();
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

      uint16_t use_val = flash_mode_.get_enum_index<uint16_t>(use_index);
      if (flash_mode_.get_current<uint16_t>() == use_val)
	    return;

      uint32_t rc;
      ptp_set_property_u16(flash_mode_.get_property_code(), use_val, rc);
      flash_mode_.set_current(use_val);
}

bool MacPTPCameraControl::set_flash_mode_ok()
{
      return flash_mode_.set_ok();
}


void MacPTPCameraControl::get_focus_mode_index(vector<string>&values)
{
      if (focus_mode_.test_property_support() == false) {
	    values.clear();
	    return;
      }

      values.resize(focus_mode_.get_enum_count());
	// The FocusMode is by definition (PTP) a UINT16.
      assert(focus_mode_.get_type_code() == 4);

      uint16_t prop_code = focus_mode_.get_property_code();
      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    uint16_t val_code = focus_mode_.get_enum_index<uint16_t>(idx);
	    values[idx] = ptp_property_value16_string(prop_code, val_code,
						      ptp_extension_vendor());
      }
}

int MacPTPCameraControl::get_focus_mode_index()
{
      if (focus_mode_.test_property_support() == false)
	    return -1;

      uint16_t val = focus_mode_.get_current<uint16_t>();
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

      uint16_t use_val = focus_mode_.get_enum_index<uint16_t>(use_index);
      if (focus_mode_.get_current<uint16_t>() == use_val)
	    return;

      uint32_t rc;
      ptp_set_property_u16(focus_mode_.get_property_code(), use_val, rc);
      focus_mode_.set_current(use_val);
}

bool MacPTPCameraControl::set_focus_mode_ok()
{
      return focus_mode_.set_ok();
}


void MacPTPCameraControl::get_white_balance_index(vector<string>&values)
{
      if (white_balance_.test_property_support() == false) {
	    values.clear();
	    return;
      }

      values.resize(white_balance_.get_enum_count());
	// The WhiteBalance is by definition (PTP) a UINT16.
      assert(white_balance_.get_type_code() == 4);

      uint16_t prop_code = white_balance_.get_property_code();
      for (unsigned idx = 0 ; idx < values.size() ; idx += 1) {
	    uint16_t val_code = white_balance_.get_enum_index<uint16_t>(idx);
	    values[idx] = ptp_property_value16_string(prop_code, val_code,
						      ptp_extension_vendor());
      }
}

int MacPTPCameraControl::get_white_balance_index()
{
      if (white_balance_.test_property_support() == false)
	    return -1;

      uint16_t val = white_balance_.get_current<uint16_t>();
      for (int idx = 0 ; idx < white_balance_.get_enum_count() ; idx += 1) {
	    if (val == white_balance_.get_enum_index<uint16_t>(idx))
		  return (int)idx;
      }

      return -1;
}

void MacPTPCameraControl::set_white_balance_index(int use_index)
{
      if (use_index < 0)
	    return;
      if (use_index >= white_balance_.get_enum_count())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property_u16(white_balance_.get_property_code(),
			    white_balance_.get_enum_index<uint16_t>(use_index),
			    rc);
}

bool MacPTPCameraControl::set_white_balance_ok()
{
      return white_balance_.set_ok();
}

int MacPTPCameraControl::debug_property_get(unsigned prop,
					    unsigned dtype,
					    unsigned long&value)
{
      uint32_t rc = 0;
      switch (dtype) {
	  case 0x0004: // UINT16
	    value = ptp_get_property_u16(prop, rc);
	    break;
	  case 0x0006: // UINT32
	    value = ptp_get_property_u32(prop, rc);
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
	    ptp_set_property_u16(prop, value, rc);
	    break;
	  case 0x0006: // UINT32
	    ptp_set_property_u32(prop, value, rc);
	  default:
	    return -1;
      }
      return (int)rc;
}

static string format_val(UInt8*&dptr, uint16_t type_code)
{
      char buf[32];
      switch (type_code) {
	  case 0:
	    break;
	  case 1: { // 8
		int8_t tmp = val_from_bytes<int8_t>(dptr);
		snprintf(buf, sizeof buf, "0x%02x", tmp);
		return string(buf);
	  }
	  case 2: { // uint8
		uint8_t tmp = val_from_bytes<uint8_t>(dptr);
		snprintf(buf, sizeof buf, "0x%02x", tmp);
		return string(buf);
	  }
	  case 3: { // int16
		int16_t tmp = val_from_bytes<int16_t>(dptr);
		snprintf(buf, sizeof buf, "0x%04x", tmp);
		return string(buf);
	  }
	  case 4: { // uint16
		uint16_t tmp = val_from_bytes<uint16_t>(dptr);
		snprintf(buf, sizeof buf, "0x%04x", tmp);
		return string(buf);
	  }
	  case 5: { // int32
		int32_t tmp = val_from_bytes<int32_t>(dptr);
		snprintf(buf, sizeof buf, "0x%08x", tmp);
		return string(buf);
	  }
	  case 6: { // uint32
		uint32_t tmp = val_from_bytes<uint32_t>(dptr);
		snprintf(buf, sizeof buf, "0x%08x", tmp);
		return string(buf);
	  }
#if 0
	  case 7: { // int64
		int64_t tmp = val_from_bytes<int64_t>(dptr);
		snprintf(buf, sizeof buf, "0x%016x", tmp);
		return string(buf);
	  }
	  case 8: { // uint64
		uint64_t tmp = val_from_bytes<uint64_t>(dptr);
		snprintf(buf, sizeof buf, "0x%08x", tmp);
		return string(buf);
	  }
#endif
	  case 0xffff: { // String
		QString tmp = val_from_bytes<QString>(dptr);
		return string("\"") + tmp.toStdString() + "\"";
	  }
	  default:
	    break;
      }
      return string("");
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

      uint16_t type_code = (ptp_buf->data[3] << 8) | ptp_buf->data[2];
      snprintf(tmp_buf, sizeof tmp_buf, "0x%04lx\n", tmp_val);
      out += "Data type: ";
      out += tmp_buf;

      UInt8*dptr = ptp_buf->data+4;

      tmp_val = val_from_bytes<uint8_t>(dptr);
      snprintf(tmp_buf, sizeof tmp_buf, "0x%02lx\n", tmp_val);
      out += "GetSet flag: ";
      out += tmp_buf;

      out += "Factory default: " + format_val(dptr, type_code) + "\n";
      out += "Current value: "   + format_val(dptr,type_code)  + "\n";

      int form_flag = val_from_bytes<uint8_t>(dptr);
      snprintf(tmp_buf, sizeof tmp_buf, " 0x%02x\n", form_flag);
      out += "Form flag: ";
      out += tmp_buf;

      unsigned enum_count = 0;
      switch (form_flag) {
	  case 2: // ENUM
	    enum_count = val_from_bytes<uint8_t>(dptr);
	    snprintf(tmp_buf, sizeof tmp_buf, "%u\n", enum_count);
	    out += "Enum count: ";
	    out += tmp_buf;
	    for (unsigned enum_idx = 0; enum_idx < enum_count; enum_idx += 1) {
		  out += " " + format_val(dptr, type_code) + "\n";
	    }
	    break;
	  default:
	    break;
      }

      return out;
}
