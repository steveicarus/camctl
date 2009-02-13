
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
# include  <iomanip>
# include  <sstream>
# include  <assert.h>

using namespace std;

/*
 * To define a new camera, add an entry in the table below. Order
 * doesn't matter, as long as the vendor/device pair is unique.
 */
MacICACameraControl::usb_devices_struct MacICACameraControl::usb_devices_table[] = {
	// Canon
      { 0x04a9, 0x314d,  "Canon", "Powershot A560", MacBlacklist },
	// Nikon devices
      { 0x04b0, 0x0104,  "Nikon", "Coolpix 995",    MacBlacklist },
      { 0x04b0, 0x0412,  "Nikon", "D80",            MacPTP },
	// Terminate the list
      { 0x0000, 0x0000,  0, 0, MacGeneric }
};

map<MacICACameraControl::usb_id_t,MacICACameraControl::dev_name_t> MacICACameraControl::usb_map_names;

map<MacICACameraControl::usb_id_t,MacICACameraControl::dev_class_t> MacICACameraControl::usb_map_classes;

void MacICACameraControl::load_usb_map(void)
{
      for (unsigned idx = 0 ; usb_devices_table[idx].vendor_id ; idx += 1) {
	    usb_devices_struct&cur = usb_devices_table[idx];
	    usb_id_t   id (cur.vendor_id, cur.device_id);
	    dev_name_t name (cur.vendor_name, cur.device_name);

	    usb_map_names[id] = name;
	    usb_map_classes[id] = cur.device_class;
      }
}

const MacICACameraControl::dev_name_t& MacICACameraControl::id_to_name(const MacICACameraControl::usb_id_t&id)
{
      map<usb_id_t,dev_name_t>::iterator cur = usb_map_names.find(id);

      if (cur != usb_map_names.end())
	    return cur->second;

      string vendor_str;
      string device_str;

      { ostringstream tmp;
	tmp << "Vendor(" << setw(4) << hex << id.first << ")" << ends;
	vendor_str = tmp.str();
      }

      { ostringstream tmp;
	tmp << "Device(" << setw(4) << hex << id.second << ")" << ends;
	device_str = tmp.str();
      }

      usb_map_names[id] = dev_name_t(vendor_str,device_str);
      return usb_map_names[id];
}

const MacICACameraControl::dev_class_t MacICACameraControl::id_to_class(const MacICACameraControl::usb_id_t&id)
{
      map<usb_id_t,dev_class_t>::iterator cur = usb_map_classes.find(id);

      if (cur != usb_map_classes.end())
	    return cur->second;

      usb_map_classes[id] = MacGeneric;
      return usb_map_classes[id];
}

/*
 * The camera_inventory function gets the device list and scans it
 * looking for cameras. For every camera it finds, create a
 * MacICACameraControl object and put it in the CameraControl::camera_list.
 */
void MacICACameraControl::camera_inventory(void)
{
      load_usb_map();

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

	// Scan the list of devices, looking for cameras.

      for (int dev = 0 ; dev < dev_count ; dev += 1) {
	    ICAGetNthChildPB child_pb;
	    memset(&child_pb, 0, sizeof child_pb);
	    child_pb.parentObject = dev_list;
	    child_pb.index = dev;
	    err = ICAGetNthChild(&child_pb, 0);
	    assert(err == noErr);

	    if (child_pb.childInfo.objectType == kICADevice
		&& child_pb.childInfo.objectSubtype == kICADeviceCamera) {

		  ICAObject obj = child_pb.childObject;
		  CFDictionaryRef dev_dict;
		  ICACopyObjectPropertyDictionaryPB dev_dict_pb;
		  memset(&dev_dict_pb, 0, sizeof(dev_dict_pb));
		  dev_dict_pb.object = obj;
		  dev_dict_pb.theDict = &dev_dict;
		  ICACopyObjectPropertyDictionary(&dev_dict_pb, 0);
		  long idVendor  = get_dict_long_value(dev_dict, "idVendor");
		  long idProduct = get_dict_long_value(dev_dict, "idProduct");

		  CameraControl*dev = 0;
		  switch (id_to_class(usb_id_t(idVendor,idProduct))) {

		      case MacPTP:
			dev = new MacPTPCameraControl(obj);
			break;
		      case MacGeneric:
			dev = new MacICACameraControl(obj);
			break;
		      case MacBlacklist:
			dev = new MacICABlacklist(obj);
			break;
		  }

		  CameraControl::camera_list.push_back(dev);

	    } else {
		    // This device is apparently not a camera. Skip it.
		  cout << "Device type=" << hex << child_pb.childInfo.objectType
		       << "subtype=" << hex << child_pb.childInfo.objectSubtype
		       << endl;
	    }
      }
}
