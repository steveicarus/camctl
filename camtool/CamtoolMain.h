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
# include  <QByteArray>
# include  <QGraphicsPixmapItem>
# include  <QGraphicsScene>
# include  <QTimer>
# include  "ui_camtool.h"
# include  <CameraControl.h>
# include  "CrunchThread.h"
# include  <fstream>

class CamtoolPreferences;
class CamtoolAboutBox;
class CamtoolAboutDevice;
class CamtoolDebug;

class CamtoolMain : public QMainWindow, private CameraControl::Notification {

      Q_OBJECT

    public:
      CamtoolMain(QWidget *parent =0);
      ~CamtoolMain();

      CameraControl* get_selected_camera(void);

      bool preview_window_active(void);

      void display_preview_image(const QString&file_name,
				 const QByteArray&image_data);

    private:
	// Hold on to the camera that the user grabs.
      CameraControl*selected_camera_;

      QTimer heartbeat_timer_;
      QGraphicsScene* action_thumbnail_scene_;
	// Pixmap to hold a thumbnail image.
      enum { THUMB_WID = 160, THUMB_HEI = 140,
	     THUMB_HIST_WID = 64, THUMB_HIST_HEI=32 };
      QGraphicsPixmapItem*action_thumbnail_pixmap_;
      QGraphicsPixmapItem*action_thumbnail_hist_red_;
      QGraphicsPixmapItem*action_thumbnail_hist_green_;
      QGraphicsPixmapItem*action_thumbnail_hist_blue_;

      QGraphicsScene* preview_scene_;
      QGraphicsPixmapItem*preview_pixmap_;

      QGraphicsScene*     charts_scene_;
      QGraphicsPixmapItem*charts_red_hist_;
      QGraphicsPixmapItem*charts_green_hist_;
      QGraphicsPixmapItem*charts_blue_hist_;

    private:
	// Helper methods.
      void no_camera_selected_();

      void display_capture_error_message_(CameraControl::capture_resp_t rc);
      void display_battery_(void);

      void resync_camera_images_(void);

      void detect_cameras_(void);
      void grab_camera_(void);
      void ungrab_camera_(void);

      void display_thumbnail_(CameraControl*camera, long image_key);
      void write_tethered_image_(const QString&file_name, const QByteArray&image_data);

    private slots:
	// Slot for the heartbeat timer.
      void heartbeat_slot_();

	// Slots for the Menubar
      void preferences_slot_();
      void tools_preview_slot_();
      void tools_debug_slot_();
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

      void capture_interval_slot_(int val);
      void sequence_duration_slot_(int val);

	// Slots for the Actions page
      void action_capture_slot_(void);
      void action_tethered_slot_(void);
      void action_timelapse_slot_(void);
      void action_file_slot_(void);

	// Slots for preview
      void zoom_check_slot_(int);

	// These slots are for receiving the processing results from
	// the cruncher thread.
      void display_preview_image_slot_(QImage*pix);
      void display_rgb_hist_image_slot_(QImage*red, QImage*gre, QImage*blu);

	// Slots for the Images page
      void images_list_slot_(QListWidgetItem*);
      void images_refresh_slot_(void);

	// Slots for timers
      void timer_lapse_slot_(void);

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
      CamtoolDebug*debug_window_;

      bool tethered_in_progress_;

	// Timer for generating time-lapse events.
      unsigned lapse_remaining_;
      QTimer lapse_timer_;

      CrunchThread cruncher_;
};
#endif
