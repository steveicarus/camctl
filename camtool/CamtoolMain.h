
# include  <qapplication.h>
# include  "ui_camtool.h"

class CamtoolMain : public QMainWindow {

      Q_OBJECT

    public:
      CamtoolMain(QWidget *parent =0);
      ~CamtoolMain();

    private:
      void detect_cameras_(void);

    private slots:
      void rescan_cameras_slot_(void);
      void grab_camera_slot_(int state);

    private:
      Ui::CamtoolMainWidget ui;
};
