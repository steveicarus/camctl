
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

      make_model_ = id_to_name(get_usb_id_from_dict_(dev_dict_));
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

      notification_camera_ = this;
      return 0;
}

int MacICACameraControl::close_session(void)
{
      notification_camera_ = 0;

      ICACloseSessionPB pb;
      memset(&pb, 0, sizeof pb);
      pb.sessionID = session_id_;
      ICACloseSession(&pb, 0);
      return 0;
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

void MacICACameraControl::get_image_data(long key, QByteArray&data,
					 bool delete_image)
{
      ICAObject image = ica_image_object_from_dev_(key);
      if (image == 0) {
	    data.clear();
	    return;
      }

	// Given the image object, we first get the image details by
	// getting the kICAPropertyImageData property. That will have
	// the size of the image data, which I can in turn use to
	// retrieve the full image via the ICACopyObjectData function.
      ICAGetPropertyByTypePB image_data_pb;
      memset(&image_data_pb, 0, sizeof image_data_pb);
      image_data_pb.object = image;
      image_data_pb.propertyType = kICAPropertyImageData;
      ICAGetPropertyByType(&image_data_pb, 0);
      size_t buf_len = image_data_pb.propertyInfo.dataSize;

	// Now get the image data directly from the image object.
      CFDataRef dataRef;
      ICACopyObjectDataPB data_pb;
      memset(&data_pb, 0, sizeof data_pb);
      data_pb.object = image;
      data_pb.startByte = 0;
      data_pb.requestedSize = buf_len;
      data_pb.data = &dataRef;
      ICAError rc = ICACopyObjectData(&data_pb, 0);
      assert(rc == 0);

	// Put the image data into the QByteArray that the caller
	// passed in.
      data.resize(CFDataGetLength(dataRef));
      CFDataGetBytes(dataRef, CFRangeMake(0, buf_len), (UInt8*)data.data());
      CFRelease(dataRef);

      if (delete_image) {
	    ICAObjectSendMessagePB send_pb;
	    memset(&send_pb, 0, sizeof send_pb);
	    send_pb.object = image;
	    send_pb.message.messageType = kICAMessageCameraDeleteOne;
	    send_pb.message.dataPtr = 0;
	    send_pb.message.dataSize = 0;
	    send_pb.message.dataType = 0;
	    ICAObjectSendMessage(&send_pb, 0);
      }
}

void MacICACameraControl::get_image_thumbnail(long key, char*&buf, size_t&buf_len)
{
      ICAObject image = ica_image_object_from_dev_(key);
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
