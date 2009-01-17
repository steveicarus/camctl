
# include  <qapplication.h>
# include  "ui_camtool.h"
# include  <CameraControl.h>
# include  <fstream>

class CamtoolMain : public QMainWindow {

      Q_OBJECT

    public:
      CamtoolMain(QWidget *parent =0);
      ~CamtoolMain();

    private:
      CameraControl*selected_camera_;
      std::ofstream debug_;

      void detect_cameras_(void);

    private slots:
	// Slots for the Select Camera page
      void rescan_cameras_slot_(void);
      void grab_camera_slot_(int state);

	// Slots for the (debug) page
      void select_logfile_slot_(void);
      void dump_device_slot_(void);
      void dump_capabilities_slot_(void);
      void dump_data_slot_(void);
      void dump_generic_slot_(void);

    private:
      Ui::CamtoolMainWidget ui;
};
