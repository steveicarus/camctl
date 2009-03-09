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

using namespace std;

CamtoolPreferences::CamtoolPreferences(QWidget*parent)
: QDialog(parent), settings_("icarus.com", "Icarus Camera Control", parent)
{
      ui.setupUi(this);

	// (debug)
      connect(ui.select_logfile_button,
	      SIGNAL(clicked()),
	      SLOT(select_logfile_slot_()));
}

CamtoolPreferences::~CamtoolPreferences()
{
}

void CamtoolPreferences::select_logfile_slot_(void)
{
      QString filename = QFileDialog::getSaveFileName(this, tr("Log File"));
      if (CameraControl::debug_log.is_open())
	    CameraControl::debug_log.close();

      ui.logfile_path->setText(filename);

      CameraControl::debug_log.open(filename.toAscii());
      CameraControl::debug_log << "Open log file " << filename.toAscii().data() << endl;
      CameraControl::debug_log << flush;
}
