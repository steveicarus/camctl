#ifndef __CamtoolDebug_H
#define __CamtoolDebug_H

/*
 * Copyright (c) 2009-2010 Stephen Williams (steve@icarus.com)
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
# include  "ui_debug.h"

class CamtoolMain;
class QString;

class CamtoolDebug : public QDialog {

      Q_OBJECT

    public:
      CamtoolDebug(CamtoolMain*parent);
      ~CamtoolDebug();

    private slots:
      void dump_generic_slot_(void);
      void debug_ptp_refresh_slot_(void);
      void debug_ptp_get_slot_(void);
      void debug_ptp_set_slot_(void);
      void debug_ptp_select_value_slot_(int);
      void debug_ptp_describe_slot_(void);

    private:
      CamtoolMain*main_window_;
      Ui::DebugWindow ui;
};

#endif
