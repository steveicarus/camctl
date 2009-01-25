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
# include  <iostream>
# include  <iomanip>

using namespace std;

void CameraControl::camera_inventory(void)
{
      while (! camera_list.empty()) {
	    CameraControl*cur = camera_list.front();
	    camera_list.pop_front();
	    delete cur;
      }

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

int CameraControl::open_session(void)
{
      return 0;
}

int CameraControl::close_session(void)
{
      return 0;
}

float CameraControl::battery_level(void) const
{
      return -1;
}

std::string CameraControl::exposure_program_mode(void) const
{
      return "";
}

void CameraControl::get_exposure_time(int32_t&min, int32_t&max, int32_t&step)
{
      min  = 0;
      max  = 0;
      step = 0;
}

int32_t CameraControl::get_exposure_time()
{
      return -1;
}

void CameraControl::set_exposure_time(int32_t)
{
}

int CameraControl::get_aperture()
{
      return -1;
}

int CameraControl::get_exposure_index()
{
      return -1;
}

void CameraControl::set_aperture(int)
{
}

void CameraControl::set_exposure_index(int)
{
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

int CameraControl::debug_property_get(unsigned prop, unsigned dtype, unsigned long&value)
{
}

int CameraControl::debug_property_set(unsigned prop, unsigned dtype, unsigned long value)
{
}
