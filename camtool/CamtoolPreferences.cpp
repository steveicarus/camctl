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
# include  "CamtoolPreferences.h"
# include  <QFileDialog>
# include  "CameraControl.h"
# include  <iostream>
# include  <stdlib.h>

using namespace std;

/*
 * These are constants for the settings keys
 */
static const char KEY_TETHERED_PATH[] = "images/tethered_path";
static const char KEY_LOGFILE_PATH[]  = "debug/logfile_path";

CamtoolPreferences::CamtoolPreferences(QWidget*parent)
: QDialog(parent),
    settings_("icarus.com", "Icarus Camera Control", parent),
    home_path_(getenv("HOME"))
{
      ui.setupUi(this);

	// Image output
      connect(ui.select_tethered_button,
	      SIGNAL(clicked()),
	      SLOT(select_tethered_slot_()));

      connect(ui.tethered_path,
	      SIGNAL(editingFinished()),
	      SLOT(tethered_path_slot_()));

      connect(ui.image_output_buttons,
	      SIGNAL(clicked(QAbstractButton*)),
	      SLOT(image_output_buttons_slot_(QAbstractButton*)));

	// (debug)
      connect(ui.select_logfile_button,
	      SIGNAL(clicked()),
	      SLOT(select_logfile_slot_()));

      connect(ui.logfile_path,
	      SIGNAL(editingFinished()),
	      SLOT(logfile_path_slot_()));

      connect(ui.debug_buttons,
	      SIGNAL(clicked(QAbstractButton*)),
	      SLOT(debug_buttons_slot_(QAbstractButton*)));

      QString tmp;

      tmp = settings_.value(KEY_TETHERED_PATH).toString();
      if (tmp.isEmpty()) {
	    clear_tethered_path_();
      } else {
	    ui.tethered_path->setText(tmp);
      }

	// Retrieve the saved value for the logfile path.
      tmp = settings_.value(KEY_LOGFILE_PATH).toString();
      if (! tmp.isEmpty()) {
	    ui.logfile_path->setText(tmp);
	    logfile_path_slot_();
      }
}

CamtoolPreferences::~CamtoolPreferences()
{
}

QString CamtoolPreferences::get_tethered_path() const
{
      return ui.tethered_path->text();
}

void CamtoolPreferences::clear_tethered_path_(void)
{
      QString tmp;
      tmp = home_path_ + "/Pictures/Icarus Camera Control";
      ui.tethered_path->setText(tmp);
}

void CamtoolPreferences::select_tethered_slot_(void)
{
      QString dirname = QFileDialog::getExistingDirectory(this, tr("Destination for Tethered Capture"));

      if (dirname.isEmpty())
	    return;

      dirname = QDir::cleanPath(dirname);
      ui.tethered_path->setText(dirname);
      tethered_path_slot_();
}

void CamtoolPreferences::tethered_path_slot_(void)
{
      QString filename = ui.tethered_path->text();
      settings_.setValue(KEY_TETHERED_PATH, filename);
}

void CamtoolPreferences::image_output_buttons_slot_(QAbstractButton*button)
{
      switch (ui.image_output_buttons->standardButton(button)) {

	  case QDialogButtonBox::RestoreDefaults:
	    clear_tethered_path_();
	    tethered_path_slot_();
	    break;

	  default:
	    break;
      }
}

void CamtoolPreferences::select_logfile_slot_(void)
{
      QString filename = QFileDialog::getSaveFileName(this, tr("Log File"));
      if (CameraControl::debug_log.is_open())
	    CameraControl::debug_log.close();

      ui.logfile_path->setText(filename);
      logfile_path_slot_();
}

void CamtoolPreferences::logfile_path_slot_(void)
{
      QString filename = ui.logfile_path->text();
      settings_.setValue(KEY_LOGFILE_PATH, filename);

      CameraControl::debug_log.open(filename.toAscii(), ios_base::out|ios_base::app);
      CameraControl::debug_log << "Open log file " << filename.toAscii().data() << endl;
      CameraControl::debug_log << flush;
}
