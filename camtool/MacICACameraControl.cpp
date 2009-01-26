
/*
 * Copyright (c) 2008 Stephen Williams (steve@icarus.com)
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
# include  <ImageCapture/ImageCapture.h>
# include  <iostream>
# include  <assert.h>

using namespace std;

long MacICACameraControl::get_dict_long_value(CFDictionaryRef dict, const char*key)
{
      CFStringRef key_ref = CFStringCreateWithCString(0, key, kCFStringEncodingASCII);
      CFNumberRef ref = (CFNumberRef)CFDictionaryGetValue(dict, key_ref);
      CFRelease(key_ref);

      long val;
      CFNumberGetValue(ref, kCFNumberLongType, &val);

      return val;
}

string MacICACameraControl::get_dict_string_value(CFDictionaryRef dict, const char*key)
{
      CFStringRef key_ref = CFStringCreateWithCString(0, key, kCFStringEncodingASCII);
      CFStringRef ref = (CFStringRef)CFDictionaryGetValue(dict, key_ref);
      CFRelease(key_ref);

      CFIndex buf_len = CFStringGetLength(ref);
      char*buf = new char [buf_len + 1];
      CFStringGetCString(ref, buf, buf_len+1, kCFStringEncodingASCII);

      string val (buf);
      delete[]buf;

      return val;
}

long MacICACameraControl::get_dev_prop_long_value_(const char*key)
{
      return get_dict_long_value(dev_prop_dict_, key);
}

MacICACameraControl::MacICACameraControl(ICAObject dev)
{
      dev_ = dev;

      ICACopyObjectPropertyDictionaryPB dev_dict_pb ;
      memset(&dev_dict_pb, 0, sizeof dev_dict_pb);
      dev_dict_pb.object = dev_;
      dev_dict_pb.theDict = &dev_dict_;
      ICACopyObjectPropertyDictionary(&dev_dict_pb, 0);

      long idVendor  = get_dict_long_value(dev_dict_, "idVendor");
      long idProduct = get_dict_long_value(dev_dict_, "idProduct");

      make_model_ = id_to_name(usb_id_t(idVendor,idProduct));

      dev_prop_dict_ = (CFDictionaryRef)CFDictionaryGetValue(dev_dict_, CFSTR("device properties"));
}

MacICACameraControl::~MacICACameraControl()
{
}

string MacICACameraControl::control_class(void) const
{
      return "Image Capture Architecture";
}

string MacICACameraControl::camera_make(void) const
{
      return make_model_.first;
}

string MacICACameraControl::camera_model(void) const
{
      return make_model_.second;
}

int MacICACameraControl::open_session(void)
{
      ICAOpenSessionPB pb;
      memset(&pb, 0, sizeof pb);
      pb.deviceObject = dev_;
      ICAOpenSession(&pb, 0);
      session_id_ = pb.sessionID;
      return 0;
}

int MacICACameraControl::close_session(void)
{
      ICACloseSessionPB pb;
      memset(&pb, 0, sizeof pb);
      pb.sessionID = session_id_;
      ICACloseSession(&pb, 0);
      return 0;
}

float MacICACameraControl::battery_level(void) const
{
      assert(dev_prop_dict_);
      long val = get_dict_long_value(dev_prop_dict_, "ICADevicePropBatteryLevel");
      return val;
}

void MacICACameraControl::capture_image(void)
{
      ICAObjectSendMessagePB send_pb;
      memset(&send_pb, 0, sizeof send_pb);
      send_pb.object = dev_;
      send_pb.message.messageType = kICAMessageCameraCaptureNewImage;
      send_pb.message.dataPtr = 0;
      send_pb.message.dataSize = 0;
      send_pb.message.dataType = 0;
      ICAObjectSendMessage(&send_pb, 0);
}

void MacICACameraControl::scan_images(std::list<file_key_t>&dst)
{
      CFArrayRef aref = (CFArrayRef)CFDictionaryGetValue(dev_dict_, CFSTR("data"));
      assert(aref);
      CFIndex asiz = CFArrayGetCount(aref);

      for (CFIndex idx = 0 ; idx < asiz ; idx += 1) {
	    CFDictionaryRef cur = (CFDictionaryRef)CFArrayGetValueAtIndex(aref,idx);
	    assert(cur);

	    CFStringRef nam = (CFStringRef)CFDictionaryGetValue(cur,CFSTR("ifil"));
	    assert(nam);

	    char buf[128];
	    CFStringGetCString(nam, buf, sizeof buf, kCFStringEncodingASCII);
	    string nam_str = buf;

	    dst.push_back( file_key_t(idx,nam_str) );
      }
}

void MacICACameraControl::get_image_data(long key, char*&buf, size_t&buf_len)
{
      CFArrayRef aref = (CFArrayRef)CFDictionaryGetValue(dev_dict_, CFSTR("data"));
      assert(aref);
      CFIndex asiz = CFArrayGetCount(aref);

      if (key >= asiz) {
	    buf = 0;
	    buf_len = 0;
	    return;
      }

      CFDictionaryRef cur = (CFDictionaryRef)CFArrayGetValueAtIndex(aref,key);
      assert(cur);

      CFNumberRef icao = (CFNumberRef) CFDictionaryGetValue(cur,CFSTR("icao"));
      assert(icao);

      ICAObject image;
      CFNumberGetValue(icao, kCFNumberLongType, &image);

      ICAGetPropertyByTypePB image_data_pb;
      memset(&image_data_pb, 0, sizeof image_data_pb);
      image_data_pb.object = image;
      image_data_pb.propertyType = kICAPropertyImageData;
      ICAGetPropertyByType(&image_data_pb, 0);

      buf_len = image_data_pb.propertyInfo.dataSize;
      buf = new char[buf_len];
      assert(buf);

      ICAGetPropertyDataPB data_pb;
      memset(&data_pb, 0, sizeof data_pb);
      data_pb.property = image_data_pb.property;
      data_pb.startByte = 0;
      data_pb.requestedSize = buf_len;
      data_pb.dataPtr = buf;
      ICAGetPropertyData(&data_pb, 0);
}
