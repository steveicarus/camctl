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

# include  "CameraControl.h"
# include  <iostream>
# include  <sstream>
# include  <iomanip>

using namespace std;

/*
 * To define a new camera, add an entry in the table below. Order
 * doesn't matter, as long as the vendor/device pair is unique, but
 * keeping the table sorted helps for maintenence.
 */
CameraControl::usb_devices_struct CameraControl::usb_devices_table[] = {
	// Canon (Vendor 0x04a9)
      { 0x04a9, 0x3074,  "Canon", "PowerShot A60",  MacPTP },
      { 0x04a9, 0x309a,  "Canon", "PowerShot A80",  MacPTP },
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
      { 0x04b0, 0x041e,  "Nikon", "D60",            MacPTP },
      { 0x04b0, 0x0421,  "Nikon", "D90",            MacPTP },
	// Apple (Vendor 0x05ac)
      { 0x05ac, 0x1293,  "Apple", "iPod",           MacBlacklist },
	// Olympus (Vendor 0x07b4)
      { 0x07b4, 0x0118,  "Olympus", "E-3",          MacPTP },
	// Terminate the list
      { 0x0000, 0x0000,  0, 0, MacPTP }
};


map<CameraControl::usb_id_t,CameraControl::dev_name_t> CameraControl::usb_map_names;

map<CameraControl::usb_id_t,CameraControl::dev_class_t> CameraControl::usb_map_classes;

void CameraControl::load_usb_map(void)
{
	// We only want to execute this once, but the caller might not
	// know if it has been done. So include our own protection.
      static bool usb_devices_table_is_loaded = false;
      if (usb_devices_table_is_loaded)
	    return;

      for (unsigned idx = 0 ; usb_devices_table[idx].vendor_id ; idx += 1) {
	    usb_devices_struct&cur = usb_devices_table[idx];
	    usb_id_t   id (cur.vendor_id, cur.device_id);
	    dev_name_t name (cur.vendor_name, cur.device_name);

	    usb_map_names[id] = name;
	    usb_map_classes[id] = cur.device_class;
      }
      usb_devices_table_is_loaded = true;
}

const CameraControl::dev_name_t& CameraControl::id_to_name(const CameraControl::usb_id_t&id)
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

const CameraControl::dev_class_t CameraControl::id_to_class(const CameraControl::usb_id_t&id)
{
      map<usb_id_t,dev_class_t>::iterator cur = usb_map_classes.find(id);

      if (cur != usb_map_classes.end())
	    return cur->second;

	// If no other detail is given, then assume this is some sort
	// of PTP camera.
      usb_map_classes[id] = MacPTP;
      return usb_map_classes[id];
}
