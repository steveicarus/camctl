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
# include  "CamtoolPreferences.h"
# include  "CamtoolAboutBox.h"
# include  "CamtoolAboutDevice.h"
# include  "CamtoolDebug.h"
# include  <QFileDialog>
# include  <QMessageBox>
# include  <iostream>

using namespace std;

Q_DECLARE_METATYPE(CameraControl*)

CamtoolMain::CamtoolMain(QWidget*parent)
: QMainWindow(parent), heartbeat_timer_(this)
{
      preferences_ = 0;
      about_ = 0;
      about_device_ = 0;
      selected_camera_ = 0;
      tethered_in_progress_ = 0;
      lapse_remaining_ = 0;

      ui.setupUi(this);
      preferences_ = new CamtoolPreferences(this);
      debug_window_ = new CamtoolDebug(this);

	// Set the maximum for the various time entry boxes based on
	// the size of an integer.
      int max_interval = INT_MAX / 1000;
      ui.capture_interval->setMaximum(max_interval);
      ui.sequence_duration->setMaximum(max_interval);

      CameraControl::set_camera_added_notification(this);
      detect_cameras_();

      ui.battery_display->display("   ");

	// Set up the thumbnail display widget.
      action_thumbnail_scene_ = new QGraphicsScene;
      action_thumbnail_pixmap_ = new QGraphicsPixmapItem;
      action_thumbnail_hist_red_ = new QGraphicsPixmapItem;
      action_thumbnail_hist_green_ = new QGraphicsPixmapItem;
      action_thumbnail_hist_blue_ = new QGraphicsPixmapItem;

      action_thumbnail_scene_->setBackgroundBrush(QBrush(QColor(80,80,80)));
      action_thumbnail_scene_->addItem(action_thumbnail_pixmap_);
      action_thumbnail_scene_->addItem(action_thumbnail_hist_red_);
      action_thumbnail_scene_->addItem(action_thumbnail_hist_green_);
      action_thumbnail_scene_->addItem(action_thumbnail_hist_blue_);

      action_thumbnail_pixmap_->setPos(0,0);
      action_thumbnail_hist_red_  ->setPos(THUMB_WID+6,  0);
      action_thumbnail_hist_green_->setPos(THUMB_WID+6, 34);
      action_thumbnail_hist_blue_ ->setPos(THUMB_WID+6, 68);

      ui.action_thumbnail_view->setScene(action_thumbnail_scene_);

      preview_scene_ = new QGraphicsScene;
      preview_pixmap_ = new QGraphicsPixmapItem;
      preview_scene_->addItem(preview_pixmap_);
      ui.preview_image->setScene(preview_scene_);

      charts_scene_ = new QGraphicsScene;
      charts_red_hist_ = new QGraphicsPixmapItem;
      charts_green_hist_ = new QGraphicsPixmapItem;
      charts_blue_hist_ = new QGraphicsPixmapItem;

      charts_scene_->setBackgroundBrush(QBrush(QColor(0,0,0)));
      charts_scene_->addItem(charts_red_hist_);
      charts_scene_->addItem(charts_green_hist_);
      charts_scene_->addItem(charts_blue_hist_);

      charts_red_hist_  ->setPos(0, 0);
      charts_green_hist_->setPos(0, 1*(CrunchThread::CHART_HEI+10));
      charts_blue_hist_ ->setPos(0, 2*(CrunchThread::CHART_HEI+10));

      ui.preview_charts->setScene(charts_scene_);

	// Heartbeat timer
      connect(&heartbeat_timer_,
	      SIGNAL(timeout()),
	      SLOT(heartbeat_slot_()));

	// Menu bar
      connect(ui.preferences_action,
	      SIGNAL(triggered()),
	      SLOT(preferences_slot_()));
      connect(ui.tools_preview,
	      SIGNAL(triggered()),
	      SLOT(tools_preview_slot_()));
      connect(ui.tools_debug,
	      SIGNAL(triggered()),
	      SLOT(tools_debug_slot_()));
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

      connect(ui.capture_interval,
	      SIGNAL(valueChanged(int)),
	      SLOT(capture_interval_slot_(int)));
      connect(ui.sequence_duration,
	      SIGNAL(valueChanged(int)),
	      SLOT(sequence_duration_slot_(int)));

	// Actions
      connect(ui.action_capture_button,
	      SIGNAL(clicked()),
	      SLOT(action_capture_slot_()));
      connect(ui.action_tethered_button,
	      SIGNAL(clicked()),
	      SLOT(action_tethered_slot_()));
      connect(ui.action_timelapse_button,
	      SIGNAL(clicked()),
	      SLOT(action_timelapse_slot_()));
      connect(ui.action_file_button,
	      SIGNAL(clicked()),
	      SLOT(action_file_slot_()));

	// Images
      connect(ui.images_list,
	      SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	      SLOT(images_list_slot_(QListWidgetItem*)));
      connect(ui.refresh_button,
	      SIGNAL(clicked()),
	      SLOT(images_refresh_slot_()));

	// Preview
      connect(ui.zoom_check,
	      SIGNAL(stateChanged(int)),
	      SLOT(zoom_check_slot_(int)));

	// The Cruncher thread sends results back to me by sending
	// signals. Connect those signals to slots in this class.
      connect(&cruncher_,
	      SIGNAL(display_preview_image(QImage*)),
	      SLOT(display_preview_image_slot_(QImage*)));
      connect(&cruncher_,
	      SIGNAL(display_rgb_hist_image(QImage*,QImage*,QImage*)),
	      SLOT(display_rgb_hist_image_slot_(QImage*,QImage*,QImage*)));

	// Timers
      connect(&lapse_timer_, SIGNAL(timeout()), SLOT(timer_lapse_slot_()));

	// Recover stored preferences

      if (preferences_->get_preview_raised()) {
	    ui.tools_preview->setChecked(true);
	    tools_preview_slot_();
      }

      { int interval = preferences_->get_capture_interval();
	int duration = preferences_->get_sequence_duration();
	ui.capture_interval ->setValue(interval);
	ui.sequence_duration->setValue(duration);
      }

      cruncher_.start();
}

CamtoolMain::~CamtoolMain()
{
      cruncher_.wait();

      delete preview_pixmap_;
      delete preview_scene_;

      delete charts_red_hist_;
      delete charts_green_hist_;
      delete charts_blue_hist_;
      delete charts_scene_;
      if (about_)
	    delete about_;
}

CameraControl* CamtoolMain::get_selected_camera(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return 0;
      }

      return selected_camera_;
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

void CamtoolMain::preferences_slot_(void)
{
      assert(preferences_);
      preferences_->show();
      preferences_->raise();
      preferences_->activateWindow();
}

void CamtoolMain::tools_preview_slot_(void)
{
      if (ui.tools_preview->isChecked()) {
	    preferences_->set_preview_raised(true);
      } else {
	    preferences_->set_preview_raised(false);
      }
}

bool CamtoolMain::preview_window_active(void)
{
      return ui.tools_preview->isChecked();
}

void CamtoolMain::tools_debug_slot_(void)
{
      assert(debug_window_);
      debug_window_->show();
      debug_window_->raise();
      debug_window_->activateWindow();
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
      if (index < 0)
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

void CamtoolMain::capture_interval_slot_(int val)
{
      assert(val > 0);
      ui.sequence_duration->setSingleStep(val);
      ui.sequence_duration->setMinimum(val);

      int dur = ui.sequence_duration->value();
      if (dur < val) {
	    dur = val;
	    ui.sequence_duration->setValue(val);
      }

      preferences_->set_capture_interval(val);
      preferences_->set_sequence_duration(dur);
}

void CamtoolMain::sequence_duration_slot_(int val)
{
      int interval = ui.capture_interval->value();
      if (val < interval) {
	    val = interval;
	    ui.sequence_duration->setValue(val);
      } else if (val%interval != 0) {
	    int tmp = val / interval;
	    val = tmp * interval;
	    ui.sequence_duration->setValue(val);
      }

      preferences_->set_sequence_duration(val);
}

void CamtoolMain::display_capture_error_message_(CameraControl::capture_resp_t rc)
{
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

void CamtoolMain::action_capture_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      if (lapse_remaining_ > 0) {
	    QMessageBox::information(0, QString("Camera Busy"),
				     QString("Camera is busy with a time lapse sequence."));
	    return;
      }

      tethered_in_progress_ = false;
      CameraControl::capture_resp_t rc = selected_camera_->capture_image();
      display_capture_error_message_(rc);
}

void CamtoolMain::action_tethered_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    return;
      }

      if (lapse_remaining_ > 0) {
	    QMessageBox::information(0, QString("Camera Busy"),
				     QString("Camera is busy with a time lapse sequence."));
	    return;
      }

	// Mark that a tethered capture is in progress. This will
	// effect how received images are processed.
      tethered_in_progress_ = true;

	// First, capture to volatile memory on the device
      CameraControl::capture_resp_t rc = selected_camera_->capture_volatile_image();
      display_capture_error_message_(rc);
}

void CamtoolMain::action_timelapse_slot_(void)
{
      if (selected_camera_ == 0) {
	    no_camera_selected_();
	    ui.action_timelapse_button->setChecked(false);
	    return;
      }

      int lapse_interval = ui.capture_interval->value();
      int lapse_duration = ui.sequence_duration->value();

      if (ui.action_timelapse_button->isChecked()) {
	    if (lapse_interval == 0) {
		  QMessageBox::information(0, QString("Bad Parameter"),
					   QString("Time lapse interval is zero."));
		  return;
	    }

	      // Calculate the number of captures needed to run the duration.
	    lapse_remaining_ = lapse_duration / lapse_interval;
	    if (lapse_duration % lapse_interval != 0)
		  lapse_remaining_ += 1;

	      // Start the time lapse sequence with a initial
	      // capture. This is the time-0 capture, which the math
	      // above didn't count.
	    lapse_remaining_ += 1;
	    timer_lapse_slot_();

	      // The lapse interval is in seconds. Convert it to ms
	      // and start the interval timer.
	    lapse_timer_.start(lapse_interval * 1000);

      } else {
	      // Make certain any existing time lapse capture is cancelled.
	    lapse_remaining_ = 0;
	    lapse_timer_.stop();
      }

}

void CamtoolMain::action_file_slot_(void)
{
      QString file_name = QFileDialog::getOpenFileName(this);
      if (preview_window_active()) {
	    QFile image_file (file_name);
	    image_file.open(QIODevice::ReadOnly);
	    QByteArray image_data = image_file.readAll();
	    display_preview_image(file_name, image_data);
      }
}

void CamtoolMain::timer_lapse_slot_(void)
{
      if (selected_camera_ == 0) {
	    ui.action_timelapse_button->setChecked(false);
	    lapse_timer_.stop();
	    lapse_remaining_ = 0;
	    return;
      }

      if (lapse_remaining_ <= 1) {
	    ui.action_timelapse_button->setChecked(false);
	    lapse_timer_.stop();
      }

      if (lapse_remaining_ == 0)
	    return;

	// One less lapse event to go...
      lapse_remaining_ -= 1;

	// Capture a time lapse image just like a tethered image.
      tethered_in_progress_ = true;
      CameraControl::capture_resp_t rc = selected_camera_->capture_image();
      display_capture_error_message_(rc);
}

void CamtoolMain::camera_capture_complete(CameraControl*)
{
      if (tethered_in_progress_) {
	    preferences_->step_tethered_number();
	    tethered_in_progress_ = false;
      }
}

/*
 * The main thread calls this method when it has a preview image to
 * process. Pass the data directly back to the cruncher thread. The
 * "process_preview_data" method will return when it no longer needs
 * the data pointer.
 */
void CamtoolMain::display_preview_image(const QString&file_name,
					   const QByteArray&image_data)
{
      CameraControl::debug_log << TIMESTAMP
			       << ": CamtoolMain::display_preview_image: "
			       << "Process preview display of "
			       << file_name.toStdString()
			       << "." << endl << flush;

      cruncher_.process_preview_data(file_name, image_data);
}

void CamtoolMain::display_preview_image_slot_(QImage*pix)
{
      CameraControl::debug_log << TIMESTAMP
			       << ": CamtoolMain::display_preview_image_slot_"
			       << endl << flush;

      preview_pixmap_->setPixmap(QPixmap::fromImage(*pix));
      preview_pixmap_->update();

	// If the "Zoom 1:1" button is checked, then set the display
	// to 1-to-1. Otherwise, fit the image into the view.
      if (ui.zoom_check->isChecked()) {
	    ui.preview_image->setMatrix(QMatrix());
      } else {
	    ui.preview_image->fitInView(preview_pixmap_,
					Qt::KeepAspectRatioByExpanding);
      }
}

void CamtoolMain::display_rgb_hist_image_slot_(QImage*red, QImage*gre, QImage*blu)
{
      charts_red_hist_  ->setPixmap(QPixmap::fromImage(*red));
      charts_green_hist_->setPixmap(QPixmap::fromImage(*gre));
      charts_blue_hist_ ->setPixmap(QPixmap::fromImage(*blu));
      charts_red_hist_  ->update();
      charts_green_hist_->update();
      charts_blue_hist_ ->update();
}

void CamtoolMain::zoom_check_slot_(int state)
{
      if (state) {
	    ui.preview_image->setMatrix(QMatrix());
      } else {
	    ui.preview_image->fitInView(preview_pixmap_,
					Qt::KeepAspectRatioByExpanding);
      }
}
