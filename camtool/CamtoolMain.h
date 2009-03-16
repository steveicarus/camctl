#ifndef __CamtoolMain_H
#define __CamtoolMain_H

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
# include  <QGraphicsPixmapItem>
# include  <QGraphicsScene>
# include  <QTimer>
# include  "ui_camtool.h"
# include  <CameraControl.h>
# include  <fstream>

class CamtoolPreferences;
class CamtoolAboutBox;
class CamtoolAboutDevice;

class CamtoolMain : public QMainWindow, private CameraControl::Notification {

      Q_OBJECT

    public:
      CamtoolMain(QWidget *parent =0);
      ~CamtoolMain();

    private:
	// Hold on to the camera that the user grabs.
      CameraControl*selected_camera_;

      QTimer heartbeat_timer_;
      QGraphicsScene* action_thumbnail_scene_;
      QGraphicsPixmapItem*action_thumbnail_pixmap_;

    private:
	// Helper methods.
      void no_camera_selected_();

      void display_capture_error_message_(CameraControl::capture_resp_t rc);
      void display_battery_(void);

      void resync_camera_images_(void);

      void detect_cameras_(void);
      void grab_camera_(void);
      void ungrab_camera_(void);

    private slots:
	// Slot for the heartbeat timer.
      void heartbeat_slot_();

	// Slots for the Menubar
      void preferences_slot_();
      void help_about_slot_();
      void help_about_camera_slot_();

	// Slots for the Select Camera page
      void grab_camera_slot_(int state);

	// Slots for the Camera Settings page.
      void exposure_program_slot_(int state);
      void set_aperture_slot_(int state);
      void set_iso_slot_(int state);
      void set_exposure_time_slot_(int state);

      void flash_mode_slot_(int state);
      void focus_mode_slot_(int state);
      void image_size_slot_(int state);
      void white_balance_slot_(int state);

	// Slots for the Actions page
      void action_capture_slot_(void);
      void action_tethered_slot_(void);

	// Slots for the Images page
      void images_list_slot_(QListWidgetItem*);
      void images_refresh_slot_(void);

	// Slots for the (debug) page
      void dump_generic_slot_(void);

      void debug_ptp_set_slot_(void);
      void debug_ptp_get_slot_(void);
      void debug_ptp_describe_slot_(void);

    private:
	// Callbacks
      void camera_image_added(CameraControl*, const CameraControl::file_key_t&);
      void camera_image_deleted(CameraControl*, const CameraControl::file_key_t&);
      void camera_capture_complete(CameraControl*);
      void camera_added(CameraControl*);
      void camera_removed(CameraControl*);

    private:
      Ui::CamtoolMainWidget ui;
      CamtoolPreferences*preferences_;
      CamtoolAboutBox*about_;
      CamtoolAboutDevice*about_device_;

      bool tethered_in_progress_;
};
#endif
