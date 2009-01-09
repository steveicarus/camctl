
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
 * The camera_inventory function gets the device list and scans it
 * looking for cameras. For every camera it finds, create a
 * MacICACameraControl object and put it in the CameraControl::camera_list.
 */
void MacICACameraControl::camera_inventory(void)
{
	// Get the list of all the ICA devices.
      ICAGetDeviceListPB dev_list_pb = { };
      ICAGetDeviceList(&dev_list_pb, 0);
      ICAObject dev_list = dev_list_pb.object;

	// Count the devices...
      ICAGetChildCountPB count_pb = { };
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

		    // Definitely a device, and a camera at that.
		  CameraControl*dev = new MacICACameraControl(child_pb.childObject);
		  CameraControl::camera_list.push_back(dev);

	    } else {
		    // This device is apparently not a camera. Skip it.
		  cout << "Device type=" << hex << child_pb.childInfo.objectType
		       << "subtype=" << hex << child_pb.childInfo.objectSubtype
		       << endl;
	    }
      }
}
