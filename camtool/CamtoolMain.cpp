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
# include  <QFileDialog>
# include  <QMessageBox>
# include  <iostream>

using namespace std;

Q_DECLARE_METATYPE(CameraControl*)

CamtoolMain::CamtoolMain(QWidget*parent)
: QMainWindow(parent)
{
      selected_camera_ = 0;

      ui.setupUi(this);
      detect_cameras_();

	// Set up the thumbnail display widget.
      action_thumbnail_scene_ = new QGraphicsScene;
      action_thumbnail_pixmap_ = new QGraphicsPixmapItem;

      action_thumbnail_scene_->addItem(action_thumbnail_pixmap_);
      ui.action_thumbnail_view->setScene(action_thumbnail_scene_);


	// Select Camera
      connect(ui.rescan_button,
	      SIGNAL(clicked()),
	      SLOT(rescan_cameras_slot_()));
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
      connect(ui.dump_device_button,
	      SIGNAL(clicked()),
	      SLOT(dump_device_slot_()));
      connect(ui.dump_capabilities_button,
	      SIGNAL(clicked()),
	      SLOT(dump_capabilities_slot_()));
      connect(ui.dump_data_button,
	      SIGNAL(clicked()),
	      SLOT(dump_data_slot_()));
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

      CameraControl::camera_inventory();

	// Handle the special case that there are no cameras. Put a
	// placeholder in the combo box and quit.
      if (CameraControl::camera_list.size() == 0) {
	    QString tmp ("No camera found");
	    ui.camera_list_box->addItem(tmp);
	    ui.camera_list_box->setEnabled(false);
	    ui.grab_check_box->setEnabled(false);
	    return;
      }

      for (list<CameraControl*>::iterator cur = CameraControl::camera_list.begin()
		 ; cur != CameraControl::camera_list.end() ; cur ++) {

	    CameraControl*item = *cur;

	    QString item_str (item->camera_make().c_str());
	    item_str.append(" ");
	    item_str.append(item->camera_model().c_str());

	    ui.camera_list_box->addItem(item_str, qVariantFromValue(item));
      }

      ui.camera_list_box->setEnabled(true);
      ui.grab_check_box->setEnabled(true);
}

void CamtoolMain::rescan_cameras_slot_(void)
{
      detect_cameras_();
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
	    ui.rescan_button->setEnabled(false);
	    ui.camera_list_box->setEnabled(false);
	    grab_camera_();
      } else {
	    ungrab_camera_();
	    ui.rescan_button->setEnabled(true);
	    ui.camera_list_box->setEnabled(true);
	    selected_camera_ = 0;
      }
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

void CamtoolMain::dump_device_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      selected_camera_->debug_dump(CameraControl::debug_log, "device");
      CameraControl::debug_log << flush;
}

void CamtoolMain::dump_capabilities_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      selected_camera_->debug_dump(CameraControl::debug_log, "capabilities");
      CameraControl::debug_log << flush;
}

void CamtoolMain::dump_data_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      selected_camera_->debug_dump(CameraControl::debug_log, "data");
      CameraControl::debug_log << flush;
}

void CamtoolMain::dump_generic_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }
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
