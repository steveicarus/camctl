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
# include  <QTreeWidgetItem>
# include  <QString>
# include  <iostream>
# include  <iomanip>
# include  <sys/time.h>

using namespace std;

CameraControl::Notification* CameraControl::added_notification_ = 0;

void CameraControl::camera_inventory(void)
{
      MacICACameraControl::camera_inventory();
}

std::ofstream CameraControl::debug_log;

CameraControl::CameraControl()
{
      image_added_notification_ = 0;
      image_deleted_notification_ = 0;
      removed_notification_ = 0;
      capture_complete_notification_ = 0;
}

CameraControl::~CameraControl()
{
}

QTreeWidgetItem*CameraControl::describe_camera()
{
      QTreeWidgetItem*root = new QTreeWidgetItem;
      root->setText(0, "CameraControl");

      QTreeWidgetItem*tmp = new QTreeWidgetItem;
      tmp->setText(0, "Make");
      tmp->setText(1, camera_make().c_str());
      root->addChild(tmp);

      tmp = new QTreeWidgetItem;
      tmp->setText(0, "Model");
      tmp->setText(1, camera_model().c_str());
      root->addChild(tmp);

      tmp = new QTreeWidgetItem;
      tmp->setText(0, "control_class");
      tmp->setText(1, control_class().c_str());
      root->addChild(tmp);

      return root;
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

void CameraControl::heartbeat()
{
}

float CameraControl::battery_level(void)
{
      return -1;
}

int CameraControl::get_image_size_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_image_size_index(int)
{
}

bool CameraControl::set_image_size_ok()
{
      return false;
}

int CameraControl::get_exposure_program_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_exposure_program_index(int)
{
}

bool CameraControl::set_exposure_program_ok()
{
      return false;
}

int CameraControl::get_exposure_time_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_exposure_time_index(int)
{
}

bool CameraControl::set_exposure_time_ok()
{
      return false;
}

int CameraControl::get_fnumber_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_fnumber_index(int)
{
}

bool CameraControl::set_fnumber_ok()
{
      return false;
}

int CameraControl::get_iso_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_iso_index(int)
{
}

bool CameraControl::set_iso_ok()
{
      return false;
}

int CameraControl::get_flash_mode_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_flash_mode_index(int)
{
}

bool CameraControl::set_flash_mode_ok()
{
      return false;
}

int CameraControl::get_focus_mode_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_focus_mode_index(int)
{
}

bool CameraControl::set_focus_mode_ok()
{
      return false;
}

int CameraControl::get_white_balance_index(vector<QString>&values)
{
      values.clear();
      return -1;
}

void CameraControl::set_white_balance_index(int)
{
}

bool CameraControl::set_white_balance_ok()
{
      return false;
}

CameraControl::capture_resp_t CameraControl::capture_volatile_image()
{
      return capture_image();
}

const list<CameraControl::file_key_t>&CameraControl::image_list()
{
      mark_image_notification();
      return image_list_;
}

void CameraControl::get_image_data(long, QByteArray&data, bool)
{
      data.clear();
      cerr << "CameraControl::get_image_data: Not implemented" << endl;
}

void CameraControl::get_image_thumbnail(long, char*&buf, size_t&buf_len)
{
      buf = 0;
      buf_len = 0;
      cerr << "CameraControl::get_image_thumbnail: Not implemented" << endl;
}

void CameraControl::debug_dump(std::ostream&out, const std::string&detail) const
{
      out << "CameraControl::debug_dump(" << detail << ")" << endl;
}

CameraControl::Notification::Notification()
{
}

CameraControl::Notification::~Notification()
{
}

void CameraControl::Notification::camera_image_added(CameraControl*, const CameraControl::file_key_t&)
{
      debug_log << "**** CameraControl: unimplemented camera_image_added notification"
		<< endl << flush;
}

void CameraControl::Notification::camera_image_deleted(CameraControl*, const CameraControl::file_key_t&)
{
      debug_log << "**** CameraControl: unimplemented camera_image_deleted notification"
		<< endl << flush;
}

void CameraControl::Notification::camera_capture_complete(CameraControl*)
{
      debug_log << "**** CameraControl: unimplemented camera_capture_complete notification"
		<< endl << flush;
}

void CameraControl::Notification::camera_added(CameraControl*)
{
      debug_log << "**** CameraControl: unimplemented camera_added notification"
		<< endl << flush;
}

void CameraControl::Notification::camera_removed(CameraControl*)
{
      debug_log << "**** CameraControl: unimplemented camera_removed notification"
		<< endl << flush;
}

void CameraControl::set_image_added_notification(CameraControl::Notification*that)
{
      assert(image_added_notification_ == 0 || that==0);
      image_added_notification_ = that;
}

void CameraControl::set_image_deleted_notification(CameraControl::Notification*that)
{
      assert(image_deleted_notification_ == 0 || that==0);
      image_deleted_notification_ = that;
}

void CameraControl::mark_image_notification(void)
{
      debug_log << TIMESTAMP << ": CameraControl::mark_image_notification..." << endl << flush;

	// Get the current state of the image list from the derived
	// class.
      list<file_key_t> new_image_list;
      scan_images(new_image_list);

	// Get a map of all the existing items. This map will help us
	// detect added/deleted images.
      map<file_key_t,int> flag_map;
      for (list<file_key_t>::iterator cur = image_list_.begin()
		 ; cur != image_list_.end() ; cur ++) {
	    flag_map[*cur] = -1;
      }

      for (list<file_key_t>::iterator cur = new_image_list.begin()
		 ; cur != new_image_list.end() ; cur ++) {
	    flag_map[*cur] += 1;
      }

	// Done building the map of image changes, so replace the new
	// list with the updated list.
      image_list_ = new_image_list;

	// Now the map contains all the images that were present, and
	// are now present. If the image is present before and after,
	// then the flag will be 0. If the image is deleted, the flag
	// will be <0 and if the image is new, >0.

      for (map<file_key_t,int>::const_iterator cur = flag_map.begin()
		 ; cur != flag_map.end() ; cur ++) {
	    if (cur->second == 0)
		  continue;
	    if (cur->second > 0)
		  mark_image_added_(cur->first);
	    else
		  mark_image_deleted_(cur->first);
      }
}

void CameraControl::mark_image_added_(const file_key_t&file)
{
      debug_log << TIMESTAMP << ": Camera added file " << file.second
		<< " (id=" << file.first << ")"
		<< endl << flush;
      if (image_added_notification_)
	    image_added_notification_->camera_image_added(this, file);
}

void CameraControl::mark_image_deleted_(const file_key_t&file)
{
      debug_log << TIMESTAMP << ": Camera deleted file " << file.second
		<< " (id=" << file.first << ")"
		<< endl << flush;
      if (image_deleted_notification_)
	    image_deleted_notification_->camera_image_deleted(this, file);
}

void CameraControl::set_capture_complete_notification(CameraControl::Notification*that)
{
      assert(capture_complete_notification_ == 0 || that == 0);
      capture_complete_notification_ = that;
}

void CameraControl::mark_capture_complete(void)
{
      debug_log << TIMESTAMP << ": CameraControl::mark_capture_complete..." << endl << flush;

      if (capture_complete_notification_)
	    capture_complete_notification_->camera_capture_complete(this);
}

void CameraControl::set_camera_added_notification(CameraControl::Notification*that)
{
      assert(added_notification_ == 0 || that == 0);
      added_notification_ = that;
}

void CameraControl::mark_camera_added(CameraControl*camera)
{
      if (added_notification_)
	    added_notification_->camera_added(camera);
}

void CameraControl::set_camera_removed_notification(CameraControl::Notification*that)
{
      assert(removed_notification_ == 0 || that == 0);
      removed_notification_ = that;
}

void CameraControl::mark_camera_removed(CameraControl*camera)
{
      if (camera->removed_notification_)
	    camera->removed_notification_->camera_removed(camera);
}

std::ostream& TIMESTAMP (std::ostream&out)
{
      struct timeval tp;
      gettimeofday(&tp, 0);
      long secs = tp.tv_sec;
      unsigned long usecs = tp.tv_usec;
      while (usecs >= 1000000) {
	    secs += 1;
	    usecs -= 1000000;
      }

      out << secs << "." << setw(6) << usecs;
      return out;
}
