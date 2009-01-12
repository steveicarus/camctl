
# include  <qapplication.h>
# include  "CamtoolMain.h"
# include  <CameraControl.h>
# include  <iostream>

using namespace std;

CamtoolMain::CamtoolMain(QWidget*parent)
: QMainWindow(parent)
{
      ui.setupUi(this);
      detect_cameras_();

      connect(ui.rescan_button,
	      SIGNAL(clicked()),
	      SLOT(rescan_cameras_slot_()));
      connect(ui.grab_check_box,
	      SIGNAL(stateChanged(int)),
	      SLOT(grab_camera_slot_(int)));
}

CamtoolMain::~CamtoolMain()
{
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

	    ui.camera_list_box->addItem(item_str);
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
	    ui.rescan_button->setEnabled(false);
	    ui.camera_list_box->setEnabled(false);
      } else {
	    ui.rescan_button->setEnabled(true);
	    ui.camera_list_box->setEnabled(true);
      }
}
