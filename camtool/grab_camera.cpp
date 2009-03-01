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

# include  <qapplication.h>
# include  "CamtoolMain.h"
# include  <QMessageBox>

using namespace std;

void CamtoolMain::grab_camera_(void)
{
      selected_camera_->open_session();

      QString cam_label;
      cam_label.append(selected_camera_->camera_make().c_str());
      cam_label.append("  ");
      cam_label.append(selected_camera_->camera_model().c_str());
      ui.camera_label->setText(cam_label);

      vector<string> val_enum;
      int ext_cur;

	// Get ExposureProgram
      selected_camera_->get_exposure_program_index(val_enum);
      ext_cur = selected_camera_->get_exposure_program_index();

      ui.exposure_program_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.exposure_program_box->addItem(val_enum[idx].c_str());
      }
      ui.exposure_program_box->setCurrentIndex(ext_cur);
      ui.exposure_program_box->setEnabled(selected_camera_->set_exposure_program_ok());

	// Get ExposureTime
      selected_camera_->get_exposure_time_index(val_enum);
      ext_cur = selected_camera_->get_exposure_time_index();

      ui.set_exposure_time_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.set_exposure_time_box->addItem(val_enum[idx].c_str());
      }
      ui.set_exposure_time_box->setCurrentIndex(ext_cur);
      ui.set_exposure_time_box->setEnabled(selected_camera_->set_exposure_time_ok());

	// Get FNumber
      selected_camera_->get_fnumber_index(val_enum);
      ext_cur = selected_camera_->get_fnumber_index();

      ui.set_aperture_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.set_aperture_box->addItem(val_enum[idx].c_str());
      }
      ui.set_aperture_box->setCurrentIndex(ext_cur);
      ui.set_aperture_box->setEnabled(selected_camera_->set_fnumber_ok());

	// Get ISO (ExposureIndex)
      selected_camera_->get_iso_index(val_enum);
      ext_cur = selected_camera_->get_iso_index();

      ui.set_iso_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.set_iso_box->addItem(val_enum[idx].c_str());
      }
      ui.set_iso_box->setCurrentIndex(ext_cur);
      ui.set_iso_box->setEnabled(selected_camera_->set_iso_ok());

	// Get Flash Mode
      selected_camera_->get_flash_mode_index(val_enum);
      ext_cur = selected_camera_->get_flash_mode_index();

      ui.flash_mode_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.flash_mode_box->addItem(val_enum[idx].c_str());
      }
      ui.flash_mode_box->setCurrentIndex(ext_cur);
      ui.flash_mode_box->setEnabled(selected_camera_->set_flash_mode_ok());

	// Get Focus Mode
      selected_camera_->get_focus_mode_index(val_enum);
      ext_cur = selected_camera_->get_focus_mode_index();

      ui.focus_mode_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.focus_mode_box->addItem(val_enum[idx].c_str());
      }
      ui.focus_mode_box->setCurrentIndex(ext_cur);
      ui.focus_mode_box->setEnabled(selected_camera_->set_focus_mode_ok());

	// Get Image Size
      selected_camera_->get_image_size_index(val_enum);
      ext_cur = selected_camera_->get_image_size_index();

      ui.image_size_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.image_size_box->addItem(val_enum[idx].c_str());
      }
      ui.image_size_box->setCurrentIndex(ext_cur);
      ui.image_size_box->setEnabled(selected_camera_->set_image_size_ok());

	// Get White Balance
      selected_camera_->get_white_balance_index(val_enum);
      ext_cur = selected_camera_->get_white_balance_index();

      ui.white_balance_box->clear();
      for (size_t idx = 0 ; idx < val_enum.size() ; idx += 1) {
	    ui.white_balance_box->addItem(val_enum[idx].c_str());
      }
      ui.white_balance_box->setCurrentIndex(ext_cur);
      ui.white_balance_box->setEnabled(selected_camera_->set_white_balance_ok());

	// Get the initial image list, and register for new images.
      camera_images(selected_camera_);
      selected_camera_->set_image_notification(this);
}

void CamtoolMain::ungrab_camera_(void)
{
      selected_camera_->set_image_notification(0);
      selected_camera_->close_session();
      ui.set_exposure_time_box->setEnabled(false);
      ui.camera_label->setText("<select camera>");
}
