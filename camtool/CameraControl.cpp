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

std::ofstream CameraControl::debug_log;

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

void CameraControl::get_exposure_program_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_exposure_program_index()
{
      return -1;
}

void CameraControl::set_exposure_program_index(int)
{
}

bool CameraControl::set_exposure_program_ok()
{
      return false;
}

void CameraControl::get_exposure_time_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_exposure_time_index()
{
      return -1;
}

void CameraControl::set_exposure_time_index(int)
{
}

bool CameraControl::set_exposure_time_ok()
{
      return false;
}

void CameraControl::get_fnumber_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_fnumber_index()
{
      return -1;
}

void CameraControl::set_fnumber_index(int)
{
}

bool CameraControl::set_fnumber_ok()
{
      return false;
}

void CameraControl::get_iso_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_iso_index()
{
      return -1;
}

void CameraControl::set_iso_index(int)
{
}

bool CameraControl::set_iso_ok()
{
      return false;
}

void CameraControl::get_flash_mode_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_flash_mode_index()
{
      return -1;
}

void CameraControl::set_flash_mode_index(int)
{
}

bool CameraControl::set_flash_mode_ok()
{
      return false;
}

void CameraControl::get_focus_mode_index(vector<string>&values)
{
      values.clear();
}

int32_t CameraControl::get_focus_mode_index()
{
      return -1;
}

void CameraControl::set_focus_mode_index(int)
{
}

bool CameraControl::set_focus_mode_ok()
{
      return false;
}

const list<CameraControl::file_key_t>&CameraControl::image_list()
{
      image_list_.clear();
      scan_images(image_list_);
      return image_list_;
}

void CameraControl::get_image_data(long, char*&buf, size_t&buf_len)
{
      buf = 0;
      buf_len = 0;
      cerr << "CameraControl::get_image_data: Not implemented" << endl;
}

void CameraControl::debug_dump(std::ostream&out, const std::string&detail) const
{
      out << "CameraControl::debug_dump(" << detail << ")" << endl;
}

int CameraControl::debug_property_get(unsigned, unsigned, unsigned long&)
{
      return -1;
}

int CameraControl::debug_property_set(unsigned, unsigned, unsigned long)
{
      return -1;
}

string CameraControl::debug_property_describe(unsigned)
{
      return "N/A";
}
