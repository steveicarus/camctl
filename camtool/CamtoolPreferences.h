#ifndef __CamtoolPreferences_H
#define __CamtoolPreferences_H

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
# include  "ui_preferences.h"
# include  <QSettings>

class CamtoolPreferences : public QDialog {

      Q_OBJECT

    public:
      CamtoolPreferences(QWidget*parent);
      ~CamtoolPreferences();

    private:
      Ui::PreferencesDialog ui;

      QSettings settings_;

    private slots:
      void select_logfile_slot_(void);
};

#endif
