
# include  <qapplication.h>
# include  "ui_camtool.h"
# include  <CameraControl.h>
# include  <fstream>

class CamtoolMain : public QMainWindow, private CameraControl::Notification {

      Q_OBJECT

    public:
      CamtoolMain(QWidget *parent =0);
      ~CamtoolMain();

    private:
      CameraControl*selected_camera_;

    private:
	// Helper methods.
      void no_camera_selected_();

      void detect_cameras_(void);
      void grab_camera_(void);
      void ungrab_camera_(void);

    private slots:
	// Slots for the Select Camera page
      void rescan_cameras_slot_(void);
      void grab_camera_slot_(int state);

	// Slots for the Camera Settings page.
      void exposure_program_slot_(int state);
      void set_aperture_slot_(int state);
      void set_iso_slot_(int state);
      void set_exposure_time_slot_(int state);

      void flash_mode_slot_(int state);
      void focus_mode_slot_(int state);

	// Slots for the Actions page
      void action_capture_slot_(void);

	// Slots for the (debug) page
      void select_logfile_slot_(void);
      void dump_device_slot_(void);
      void dump_capabilities_slot_(void);
      void dump_data_slot_(void);
      void dump_generic_slot_(void);

      void debug_ptp_set_slot_(void);
      void debug_ptp_get_slot_(void);
      void debug_ptp_describe_slot_(void);

    private:
	// Callbacks
      void camera_images(CameraControl*);

    private:
      Ui::CamtoolMainWidget ui;
};
