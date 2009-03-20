
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
	// Canon (Vendor 0x04a9)
      { 0x04a9, 0x30b7,  "Canon", "PowerShot A400", MacPTP },
      { 0x04a9, 0x30b8,  "Canon", "PowerShot A310", MacPTP },
      { 0x04a9, 0x30c0,  "Canon", "PowerShot SD200",MacPTP },
      { 0x04a9, 0x30c1,  "Canon", "PowerShot A520", MacPTP },
      { 0x04a9, 0x30c2,  "Canon", "PowerShot A510", MacPTP },
      { 0x04a9, 0x30ea,  "Canon", "EOS 1D Mark II", MacPTP },
      { 0x04a9, 0x30ec,  "Canon", "EOS 20D",        MacPTP },
      { 0x04a9, 0x30ef,  "Canon", "EOS 350D",       MacPTP },
      { 0x04a9, 0x30f0,  "Canon", "PowerShot S2 IS",MacPTP },
      { 0x04a9, 0x30f2,  "Canon", "Digital IXUS 700",MacPTP },
      { 0x04a9, 0x30f9,  "Canon", "PowerShot A410", MacPTP },
      { 0x04a9, 0x30fc,  "Canon", "PowerShot A620", MacPTP },
      { 0x04a9, 0x30fd,  "Canon", "PowerShot A610", MacPTP },
      { 0x04a9, 0x30fe,  "Canon", "Digital IXUS 65",MacPTP },
      { 0x04a9, 0x30ff,  "Canon", "Digital IXUS 55",MacPTP },
      { 0x04a9, 0x310e,  "Canon", "Digital IXUS 50",MacPTP },
      { 0x04a9, 0x3116,  "Canon", "Digital IXUS 750",MacPTP },
      { 0x04a9, 0x314d,  "Canon", "Powershot A560", MacBlacklist },
	// Nikon devices (Vendor 0x04b0)
      { 0x04b0, 0x0104,  "Nikon", "Coolpix 995",    MacBlacklist },
      { 0x04b0, 0x0402,  "Nikon", "D100",           MacPTP },
      { 0x04b0, 0x0404,  "Nikon", "D2H SLR",        MacPTP },
      { 0x04b0, 0x0406,  "Nikon", "D70",            MacPTP },
      { 0x04b0, 0x0408,  "Nikon", "D2X SLR",        MacPTP },
      { 0x04b0, 0x0409,  "Nikon", "D50",            MacPTP },
      { 0x04b0, 0x040a,  "Nikon", "D50",            MacPTP },
      { 0x04b0, 0x040e,  "Nikon", "D70s",           MacPTP },
      { 0x04b0, 0x0412,  "Nikon", "D80",            MacPTP },
      { 0x04b0, 0x041a,  "Nikon", "D300",           MacPTP },
      { 0x04b0, 0x0421,  "Nikon", "D90",            MacPTP },
	// Apple (Vendor 0x05ac)
      { 0x05ac, 0x1293,  "Apple", "iPod",           MacBlacklist },
	// Olympus (Vendor 0x07b4)
      { 0x07b4, 0x0118,  "Olympus", "E-3",          MacPTP },
	// Terminate the list
      { 0x0000, 0x0000,  0, 0, MacPTP }
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

	// If no other detail is given, then assume this is some sort
	// of PTP camera.
      usb_map_classes[id] = MacPTP;
      return usb_map_classes[id];
}

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
	    long idVendor  = get_dict_long_value(dev_dict, "idVendor");
	    long idProduct = get_dict_long_value(dev_dict, "idProduct");

	    switch (id_to_class(usb_id_t(idVendor,idProduct))) {

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
