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
# include  "CamtoolAboutBox.h"
# include  "CamtoolAboutDevice.h"
# include  <QFileDialog>
# include  <QMessageBox>
# include  <iostream>

using namespace std;

Q_DECLARE_METATYPE(CameraControl*)

CamtoolMain::CamtoolMain(QWidget*parent)
: QMainWindow(parent), heartbeat_timer_(this)
{
      about_ = 0;
      about_device_ = 0;
      selected_camera_ = 0;

      ui.setupUi(this);
      CameraControl::set_camera_added_notification(this);
      detect_cameras_();

      ui.battery_display->display("   ");

	// Set up the thumbnail display widget.
      action_thumbnail_scene_ = new QGraphicsScene;
      action_thumbnail_pixmap_ = new QGraphicsPixmapItem;

      action_thumbnail_scene_->addItem(action_thumbnail_pixmap_);
      ui.action_thumbnail_view->setScene(action_thumbnail_scene_);

	// Heartbeat timer
      connect(&heartbeat_timer_,
	      SIGNAL(timeout()),
	      SLOT(heartbeat_slot_()));

	// Menu bar
      connect(ui.help_about_action,
	      SIGNAL(triggered()),
	      SLOT(help_about_slot_()));
      connect(ui.help_about_camera_action,
	      SIGNAL(triggered()),
	      SLOT(help_about_camera_slot_()));

	// Select Camera
      connect(ui.grab_check_box,
	      SIGNAL(stateChanged(int)),
	      SLOT(grab_camera_slot_(int)));

	// Settings
      connect(ui.exposure_program_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(exposure_program_slot_(int)));
      connect(ui.set_aperture_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(set_aperture_slot_(int)));
      connect(ui.set_iso_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(set_iso_slot_(int)));
      connect(ui.set_exposure_time_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(set_exposure_time_slot_(int)));

      connect(ui.flash_mode_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(flash_mode_slot_(int)));
      connect(ui.focus_mode_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(focus_mode_slot_(int)));
      connect(ui.image_size_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(image_size_slot_(int)));
      connect(ui.white_balance_box,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(white_balance_slot_(int)));

	// Actions
      connect(ui.action_capture_button,
	      SIGNAL(clicked()),
	      SLOT(action_capture_slot_()));

	// Images
      connect(ui.images_list,
	      SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	      SLOT(images_list_slot_(QListWidgetItem*)));

	// (debug)
      connect(ui.select_logfile_button,
	      SIGNAL(clicked()),
	      SLOT(select_logfile_slot_()));
      connect(ui.dump_generic_button,
	      SIGNAL(clicked()),
	      SLOT(dump_generic_slot_()));

      connect(ui.debug_ptp_set_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_set_slot_()));
      connect(ui.debug_ptp_get_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_get_slot_()));
      connect(ui.debug_ptp_describe_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_describe_slot_()));
}

CamtoolMain::~CamtoolMain()
{
      if (about_)
	    delete about_;
}

void CamtoolMain::no_camera_selected_(void)
{
      QMessageBox::information(0, QString("No Camera"),
			       QString("No Camera selected. "
				       "Go back to the Select Camera page, "
				       "select the desired camera, and "
				       "\"Grab\" the camera to activate it."));
}

void CamtoolMain::detect_cameras_(void)
{
      ui.camera_list_box->clear();
      ui.camera_list_box->setEnabled(false);
      ui.grab_check_box->setEnabled(false);
      CameraControl::camera_inventory();
}

void CamtoolMain::camera_added(CameraControl*camera)
{
      CameraControl::debug_log << "CamtoolMain: Add camera:" << endl;

      camera->set_camera_removed_notification(this);

      QString item_str (camera->camera_make().c_str());
      item_str.append(" ");
      item_str.append(camera->camera_model().c_str());

      ui.camera_list_box->addItem(item_str, qVariantFromValue(camera));
      ui.camera_list_box->setEnabled(selected_camera_ == 0? true : false);
      ui.grab_check_box->setEnabled(true);
}

void CamtoolMain::camera_removed(CameraControl*camera)
{
      CameraControl::debug_log << "CamtoolMain: Remove camera:" << endl;

      int index = ui.camera_list_box->findData(qVariantFromValue(camera));
      if (index < 0) {
	    CameraControl::debug_log << "Removed camera not in my list" << endl;
	    return;
      }

      ui.camera_list_box->removeItem(index);
      if (selected_camera_ == camera) {
	      // Uncheck the "grab" check box. This should send a
	      // signal that causes all the other ungrab activities to happen.
	    ui.grab_check_box->setCheckState(Qt::Unchecked);
	      // Rely on the grab_check_box slot to take care of the rest.
	    return;
      }

	// If this removal empties the list, then disable the select
	// box and the grab checl box.
      if (ui.camera_list_box->count() == 0) {
	    ui.camera_list_box->setEnabled(false);
	    ui.grab_check_box->setEnabled(false);
      }
}

void CamtoolMain::display_battery_(void)
{
      if (selected_camera_ == 0) {
	    ui.battery_display->display("   ");
	    return;
      }

      float level_f = selected_camera_->battery_level();
      if (level_f < 0) {
	    ui.battery_display->display("---");
      } else {
	    level_f += 0.5;
	    int level = (int)level_f;
	    ui.battery_display->display(level);
      }
}

void CamtoolMain::heartbeat_slot_(void)
{
	// If the heartbeat timer was somehow left running after the
	// device has been unselected, then turn it off.
      if (selected_camera_ == 0) {
	    heartbeat_timer_.stop();
	    return;
      }

      selected_camera_->heartbeat();
      display_battery_();
}

void CamtoolMain::help_about_slot_(void)
{
      if (about_ == 0)
	    about_ = new CamtoolAboutBox(this);

      about_->show();
      about_->raise();
      about_->activateWindow();
}

void CamtoolMain::help_about_camera_slot_(void)
{
      if (about_device_ == 0)
	    about_device_ = new CamtoolAboutDevice(this);

      if (selected_camera_) {
	    about_device_->set_devicetree(selected_camera_->describe_camera());
      } else {
	    QString root_title ("Camera not selected");
	    QTreeWidgetItem*root = new QTreeWidgetItem;
	    root->setText(0,root_title);
	    about_device_->set_devicetree(root);
      }

      about_device_->show();
      about_device_->raise();
      about_device_->activateWindow();
}

/*
 * If the "Grab selected camera" box is checked, we disable the rescan
 * button and the camera list box. This effectively locks down the
 * selection.
 */
void CamtoolMain::grab_camera_slot_(int state)
{
      if (state) {
	    int cur_idx = ui.camera_list_box->currentIndex();
	    if (cur_idx >= 0) {
		  QVariant item_ptr = ui.camera_list_box->itemData(cur_idx);
		  selected_camera_ = item_ptr.value<CameraControl*>();
	    } else {
		  selected_camera_ = 0;
	    }
	    ui.camera_list_box->setEnabled(false);
	    grab_camera_();
      } else {
	    heartbeat_timer_.stop();
	    ungrab_camera_();
	      // After releasing the current camera, re-enable the
	      // camera list box. (Although if the list is empty, we
	      // really want to disable it.
	    if (ui.camera_list_box->count() > 0) {
		  ui.camera_list_box->setEnabled(true);
	    } else {
		  ui.camera_list_box->setEnabled(false);
		  ui.grab_check_box->setEnabled(false);
	    }

	    selected_camera_ = 0;
      }

	// Get and display the current battery level, and start the
	// heartbeat timer for the device.
      display_battery_();
      if (selected_camera_)
	    heartbeat_timer_.start(1000);
}

void CamtoolMain::exposure_program_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_exposure_program_index(index);
}

void CamtoolMain::set_aperture_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_fnumber_index(index);
}

void CamtoolMain::set_exposure_time_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_exposure_time_index(index);
}

void CamtoolMain::set_iso_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_iso_index(index);
}

void CamtoolMain::flash_mode_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_flash_mode_index(index);
}

void CamtoolMain::focus_mode_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_focus_mode_index(index);
}

void CamtoolMain::image_size_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_image_size_index(index);
}

void CamtoolMain::white_balance_slot_(int index)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->set_white_balance_index(index);
}

void CamtoolMain::action_capture_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      CameraControl::capture_resp_t rc = selected_camera_->capture_image();
      switch (rc) {
	  case CameraControl::CAP_OK:
	    break;
	  case CameraControl::CAP_ERROR:
	    QMessageBox::information(0, tr("Error"),
			     tr("Error attempting to initiate a capture"));
	    break;
	  case CameraControl::CAP_NOT_SUPPORTED: {
		QString text ("This camera, a ");
		text.append (selected_camera_->camera_make().c_str());
		text.append (" ");
		text.append (selected_camera_->camera_model().c_str());
		text.append (", does not support remote control capture.");
		QMessageBox::information(0, tr("Not supported"), text);
		break;
	  }
      }
}

void CamtoolMain::select_logfile_slot_(void)
{
      QString filename = QFileDialog::getSaveFileName(this, tr("Log File"));
      if (CameraControl::debug_log.is_open())
	    CameraControl::debug_log.close();

      ui.logfile_path->setText(filename);

      CameraControl::debug_log.open(filename.toAscii());
      CameraControl::debug_log << "Open log file " << filename.toAscii().data() << endl;
      CameraControl::debug_log << flush;
}

void CamtoolMain::dump_generic_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      std::string argument = ui.dump_generic_entry->text().toAscii().data();
      selected_camera_->debug_dump(CameraControl::debug_log, argument);
}

void CamtoolMain::debug_ptp_get_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);
      unsigned prop_type = ui.debug_ptp_type_box->currentIndex() + 1;
      unsigned long value = 0;

      int rc = selected_camera_->debug_property_get(prop_code,prop_type,value);

      QString prop_text;
      prop_text.setNum(value, 16);
      ui.debug_ptp_value_entry->setText(prop_text);

      QString rc_text;
      rc_text.setNum(rc, 16);
      ui.debug_ptp_rc_entry->setText(rc_text);
}

void CamtoolMain::debug_ptp_set_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);
      unsigned prop_type = ui.debug_ptp_type_box->currentIndex() + 1;
      unsigned long value = ui.debug_ptp_value_entry->text().toULong(0,0);

      int rc = selected_camera_->debug_property_get(prop_code,prop_type,value);

      QString rc_text;
      rc_text.setNum(rc, 16);
      switch (rc) {
	  case 0x2001:
	    rc_text.append(" (OK)");
	    break;
	  case 0x2002:
	    rc_text.append(" (General Error)");
	    break;
	  case 0x2003:
	    rc_text.append(" (Session Not Open)");
	    break;
	  case 0x2005:
	    rc_text.append(" (Operation Not Supported)");
	    break;
	  default:
	    break;
      }

      ui.debug_ptp_rc_entry->setText(rc_text);
}

void CamtoolMain::debug_ptp_describe_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);

      string desc = selected_camera_->debug_property_describe(prop_code);

      CameraControl::debug_log << "**** Describe 0x" << hex << prop_code << " ****" << endl
			       << dec << desc << endl;
}
