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

# include  "CameraControl.h"
# include  "MacICACameraControl.h"
# include  <map>
# include  <iostream>
# include  <iomanip>
# include  <sstream>

using namespace std;

typedef map <CameraControl::usb_id_t,CameraControl::usb_name_t>::iterator usb_map_ref;
static map <CameraControl::usb_id_t,CameraControl::usb_name_t> usb_map;

static void load_usb_map(void)
{
      usb_map[CameraControl::usb_id_t(0x04b0,0x0412)] = CameraControl::usb_name_t("Nikon","D80");
}

const CameraControl::usb_name_t& CameraControl::id_to_name(const CameraControl::usb_id_t&id)
{
      usb_map_ref cur = usb_map.find(id);

      if (cur != usb_map.end())
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

      usb_map[id] = usb_name_t(vendor_str,device_str);
      return usb_map[id];
}

void CameraControl::camera_inventory(void)
{
      load_usb_map();

      MacICACameraControl::camera_inventory();
}

std::list<CameraControl*> CameraControl::camera_list;

CameraControl::CameraControl()
{
}

CameraControl::~CameraControl()
{
}

string CameraControl::camera_make(void) const
{
      return "NONE";
}

string CameraControl::camera_model(void) const
{
      return "NONE";
}

int CameraControl::battery_level(void) const
{
      return -1;
}

std::string CameraControl::exposure_program_mode(void) const
{
      return "";
}

const list<CameraControl::file_key_t>&CameraControl::image_list()
{
      image_list_.clear();
      scan_images(image_list_);
      return image_list_;
}

void CameraControl::get_image_data(long key, char*&buf, size_t&buf_len)
{
      buf = 0;
      buf_len = 0;
      cerr << "CameraControl::get_image_data: Not implemented" << endl;
}

void CameraControl::debug_dump(std::ostream&out, const std::string&detail) const
{
      out << "CameraControl::debug_dump(" << detail << ")" << endl;
}
