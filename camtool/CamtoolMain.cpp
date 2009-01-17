
# include  <qapplication.h>
# include  "CamtoolMain.h"
# include  <QFileDialog>
# include  <iostream>

using namespace std;

Q_DECLARE_METATYPE(CameraControl*)

CamtoolMain::CamtoolMain(QWidget*parent)
: QMainWindow(parent)
{
      selected_camera_ = 0;

      ui.setupUi(this);
      detect_cameras_();

      connect(ui.rescan_button,
	      SIGNAL(clicked()),
	      SLOT(rescan_cameras_slot_()));
      connect(ui.grab_check_box,
	      SIGNAL(stateChanged(int)),
	      SLOT(grab_camera_slot_(int)));

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
      } else {
	    ui.rescan_button->setEnabled(true);
	    ui.camera_list_box->setEnabled(true);
	    selected_camera_ = 0;
      }
}

void CamtoolMain::select_logfile_slot_(void)
{
      QString filename = QFileDialog::getSaveFileName(this, tr("Log File"));
      if (debug_.is_open())
	    debug_.close();

      ui.logfile_path->setText(filename);

      debug_.open(filename.toAscii());
      debug_ << "Open log file " << filename.toAscii().data() << endl;
      debug_ << flush;
}

void CamtoolMain::dump_device_slot_(void)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->debug_dump(debug_, "device");
      debug_ << flush;
}

void CamtoolMain::dump_capabilities_slot_(void)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->debug_dump(debug_, "capabilities");
      debug_ << flush;
}

void CamtoolMain::dump_data_slot_(void)
{
      if (selected_camera_ == 0)
	    return;

      selected_camera_->debug_dump(debug_, "data");
      debug_ << flush;
}

void CamtoolMain::dump_generic_slot_(void)
{
      if (selected_camera_ == 0)
	    return;
}
