
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

// XXXX HACK!
// ICA Notifications don't have a private data pointer, so when I open
// a session, I save the camera object here. In the long run, the
// proper thing for me to do is to create a map of camera objects here.
static MacICACameraControl*notification_camera_ = 0;

/*
 * Convenience function to get a C-long value from a dictionary.
 */
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

/*
 * Create a Mac ICA camera from an ICAObject. That object should
 * already be known to be a camera. (The inventory method assure me of
 * that.) Get the vendor and product, and use that to generate the
 * make and model strings.
 */
MacICACameraControl::MacICACameraControl(ICAObject dev)
{
      dev_ = dev;
      dev_dict_ = 0;
      refresh_dev_dict_();

      long idVendor  = get_dict_long_value(dev_dict_, "idVendor");
      long idProduct = get_dict_long_value(dev_dict_, "idProduct");

      make_model_ = id_to_name(usb_id_t(idVendor,idProduct));
}

MacICACameraControl::~MacICACameraControl()
{
      CFRelease(dev_dict_);
}

/*
 * Sometimes we have to reread the property dictionary from the
 * device.
 */
void MacICACameraControl::refresh_dev_dict_(void)
{
      if (dev_dict_ != 0) CFRelease(dev_dict_);

      ICACopyObjectPropertyDictionaryPB dev_dict_pb ;
      memset(&dev_dict_pb, 0, sizeof dev_dict_pb);
      dev_dict_pb.object = dev_;
      dev_dict_pb.theDict = &dev_dict_;
      ICACopyObjectPropertyDictionary(&dev_dict_pb, 0);
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

	// Enable some interesting notifications.
      notification_camera_ = this;
      CFMutableArrayRef events_array = CFArrayCreateMutable(0, 0, 0);
      CFArrayAppendValue(events_array, kICANotificationTypeCaptureComplete);
      CFArrayAppendValue(events_array, kICANotificationTypeObjectAdded);
      CFArrayAppendValue(events_array, kICANotificationTypeObjectRemoved);

      ICARegisterForEventNotificationPB register_pb;
      memset(&register_pb, 0, sizeof register_pb);
      register_pb.objectOfInterest = dev_;
      register_pb.eventsOfInterest = events_array;
      register_pb.notificationProc = ica_notification;
      register_pb.options = 0;
      ICARegisterForEventNotification(&register_pb, 0);

      return 0;
}

int MacICACameraControl::close_session(void)
{
      ICARegisterForEventNotificationPB register_pb;
      memset(&register_pb, 0, sizeof register_pb);
      register_pb.objectOfInterest = dev_;
      register_pb.eventsOfInterest = 0;
      register_pb.notificationProc = ica_notification;
      register_pb.options = 0;
      ICARegisterForEventNotification(&register_pb, 0);
      notification_camera_ = 0;

      ICACloseSessionPB pb;
      memset(&pb, 0, sizeof pb);
      pb.sessionID = session_id_;
      ICACloseSession(&pb, 0);
      return 0;
}

/*
 * This is the callback function for camera notifications.
 */
void MacICACameraControl::ica_notification(CFStringRef notification_type,
					   CFDictionaryRef notification_dict)
{
	// Start out by getting a description of the notification and
	// writing it to the debug log.
      char type_buf[1024];
      CFStringGetCString(notification_type, type_buf, sizeof type_buf,
			 kCFStringEncodingASCII);
      debug_log << "**** ica_notification: type="
		<< type_buf << endl;
      dump_value(debug_log, notification_dict);
      debug_log << "****" << endl << flush;

	// What kind of notification?
      if (CFStringCompare(notification_type,kICANotificationTypeObjectAdded,0) == kCFCompareEqualTo) {

	      // This is an ObjectAdded notification. This usually
	      // means that an image file was added. Refresh the image
	      // list from the camera and debug the new files list.
	    debug_log << "**** Object added ****" << endl;

	    assert(notification_camera_);
	    notification_camera_->refresh_dev_dict_();
	    notification_camera_->mark_image_notification();
      }
}

CameraControl::capture_resp_t MacICACameraControl::capture_image(void)
{
      ICAObjectSendMessagePB send_pb;
      memset(&send_pb, 0, sizeof send_pb);
      send_pb.object = dev_;
      send_pb.message.messageType = kICAMessageCameraCaptureNewImage;
      send_pb.message.dataPtr = 0;
      send_pb.message.dataSize = 0;
      send_pb.message.dataType = 0;
      ICAObjectSendMessage(&send_pb, 0);

      return CAP_OK;
}

/*
 * Scan the images on the camera by refreshing the device dictionary
 * and getting the "data" array from the device. This is an array of
 * images that I format into the file list for the caller.
 */
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

static ICAObject ica_image_object_from_dev(CFDictionaryRef dev, long key)
{
	// The images are listed as an array in the dev
	// dictionary. Get a ref to that array, and make sure the key fits.
      CFArrayRef aref = (CFArrayRef)CFDictionaryGetValue(dev, CFSTR("data"));
      assert(aref);
      CFIndex asiz = CFArrayGetCount(aref);

      if (key >= asiz)
	    return 0;

	// The file we are after is itself another dictionary within
	// the "data" array.
      CFDictionaryRef cur = (CFDictionaryRef)CFArrayGetValueAtIndex(aref,key);
      assert(cur);

	// The ICAObject key ("icao") key gets for us the image
	// object. It is a NumberRef that I can convert into an ICAObject.
      CFNumberRef icao = (CFNumberRef) CFDictionaryGetValue(cur, CFSTR("icao"));
      assert(icao);

      ICAObject image;
      CFNumberGetValue(icao, kCFNumberLongType, &image);

      return image;
}

void MacICACameraControl::get_image_data(long key, char*&buf, size_t&buf_len)
{
      ICAObject image = ica_image_object_from_dev(dev_dict_, key);

      if (image == 0) {
	    buf = 0;
	    buf_len = 0;
	    return;
      }

	// Given the image object, we can get the image data itself by
	// getting the kICAPropertyImageData property. That will have
	// the size of the image data, that I can in turn retrieve
	// with the ICAGetPropertyData function.
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

void MacICACameraControl::get_image_thumbnail(long key, char*&buf, size_t&buf_len)
{
      ICAObject image = ica_image_object_from_dev(dev_dict_, key);
      CFDataRef data;

      ICACopyObjectThumbnailPB pb;
      memset(&pb, 0, sizeof pb);
      pb.object = image;
      pb.thumbnailFormat = kICAThumbnailFormatPNG;
      pb.thumbnailData = &data;
      ICACopyObjectThumbnail(&pb, 0);

      buf_len = CFDataGetLength(data);
      buf = new char[buf_len];
      CFDataGetBytes(data, CFRangeMake(0, buf_len), (UInt8*)buf);
      CFRelease(data);
}
