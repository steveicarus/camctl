
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
# include  <assert.h>

using namespace std;

/*
 * Keep a map of ICAObjects to MacICACameraControl pointers. We use
 * this to detect when new devices arrive.
 */
struct control_info {
      control_info() : dev(0), flag(false) { }
      MacICACameraControl*dev;
      bool flag;
};

static map<ICAObject,control_info> ica2camera;

void MacICACameraControl::scan_devices_(void)
{

	// Get the list of all the ICA devices.
      ICAGetDeviceListPB dev_list_pb;
      memset(&dev_list_pb, 0, sizeof dev_list_pb);
      ICAGetDeviceList(&dev_list_pb, 0);
      ICAObject dev_list = dev_list_pb.object;

	// Count the devices...
      ICAGetChildCountPB count_pb;
      memset(&count_pb, 0, sizeof count_pb);
      count_pb.object = dev_list;
      OSErr err = ICAGetChildCount(&count_pb, NULL);
      assert(err == noErr);

      int dev_count = count_pb.count;

	// Clear all the flags.
      for (map<ICAObject,control_info>::iterator cur = ica2camera.begin()
		 ; cur != ica2camera.end() ; cur ++) {
	    cur->second.flag = false;
      }

	// Scan the list of devices, looking for cameras.

      for (int dev = 0 ; dev < dev_count ; dev += 1) {
	    ICAGetNthChildPB child_pb;
	    memset(&child_pb, 0, sizeof child_pb);
	    child_pb.parentObject = dev_list;
	    child_pb.index = dev;
	    err = ICAGetNthChild(&child_pb, 0);
	    assert(err == noErr);

	      // If this is not a device, then skip it.
	    if (child_pb.childInfo.objectType != kICADevice)
		  continue;

	      // If this device is not a camera, then skip it.
	    if (child_pb.childInfo.objectSubtype != kICADeviceCamera)
		  continue;

	      // If we already know about this camera, then skip it.
	    ICAObject obj = child_pb.childObject;
	    control_info&info = ica2camera[obj];
	    info.flag = true;

	    if (info.dev != 0) {
		  assert(info.dev->dev_ == obj);
		  continue;
	    }

	      // So now we know that this is a new camera. Figure out
	      // its class by the Vendor and Product ids, and announce
	      // what we've found.
	    CFDictionaryRef dev_dict;
	    ICACopyObjectPropertyDictionaryPB dev_dict_pb;
	    memset(&dev_dict_pb, 0, sizeof(dev_dict_pb));
	    dev_dict_pb.object = obj;
	    dev_dict_pb.theDict = &dev_dict;
	    ICACopyObjectPropertyDictionary(&dev_dict_pb, 0);

	    usb_id_t usb_id = get_usb_id_from_dict_(dev_dict);

	    switch (id_to_class(usb_id)) {

		case MacPTP:
		  info.dev = new MacPTPCameraControl(obj);
		  break;
		case MacGeneric:
		  info.dev = new MacICACameraControl(obj);
		  break;
		case MacBlacklist:
		  info.dev = new MacICABlacklist(obj);
		  break;
	    }

	    mark_camera_added(info.dev);
      }

	// Now look for all the devices that were removed.
      for (map<ICAObject,control_info>::iterator cur = ica2camera.begin()
		 ; cur != ica2camera.end() ; ) {
	    if (cur->second.flag == true) {
		  cur ++;
		  continue;
	    }

	    map<ICAObject,control_info>::iterator tmp = cur;
	    cur ++;

	    mark_camera_removed(tmp->second.dev);
	    tmp->second.dev = 0;
	    delete tmp->second.dev;
	    ica2camera.erase(tmp);
      }

}

/*
 * The camera_inventory function gets the device list and scans it
 * looking for cameras. For every camera it finds, create a
 * MacICACameraControl object and put it in the CameraControl::camera_list.
 */
void MacICACameraControl::camera_inventory(void)
{
      load_usb_map();
      scan_devices_();
      register_for_events();
}
