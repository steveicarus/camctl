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
# include  "CamtoolDebug.h"
# include  "CamtoolMain.h"
# include  <iostream>

using namespace std;


CamtoolDebug::CamtoolDebug(CamtoolMain*parent)
: QDialog(parent), main_window_(parent)
{
      ui.setupUi(this);

      connect(ui.dump_generic_button,
	      SIGNAL(clicked()),
	      SLOT(dump_generic_slot_()));

      connect(ui.debug_ptp_set_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_set_slot_()));
      connect(ui.debug_ptp_get_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_get_slot_()));
      connect(ui.debug_ptp_describe_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_describe_slot_()));
}

CamtoolDebug::~CamtoolDebug()
{
}

void CamtoolDebug::dump_generic_slot_(void)
{
      CameraControl*camera = main_window_->get_selected_camera();
      if (camera == 0)
	    return;

      std::string argument = ui.dump_generic_entry->text().toAscii().data();
      camera->debug_dump(CameraControl::debug_log, argument);
}


void CamtoolDebug::debug_ptp_get_slot_(void)
{
      CameraControl*camera = main_window_->get_selected_camera();
      if (camera == 0)
	    return;

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);
      unsigned prop_type = ui.debug_ptp_type_box->currentIndex() + 1;
      unsigned long value = 0;

      int rc = camera->debug_property_get(prop_code,prop_type,value);

      QString prop_text;
      prop_text.setNum(value, 16);
      ui.debug_ptp_value_entry->setText(prop_text);

      QString rc_text;
      rc_text.setNum(rc, 16);
      ui.debug_ptp_rc_entry->setText(rc_text);
}

void CamtoolDebug::debug_ptp_set_slot_(void)
{
      CameraControl*camera = main_window_->get_selected_camera();
      if (camera == 0)
	    return;

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);
      unsigned prop_type = ui.debug_ptp_type_box->currentIndex() + 1;
      unsigned long value = ui.debug_ptp_value_entry->text().toULong(0,0);

      int rc = camera->debug_property_get(prop_code,prop_type,value);

      QString rc_text;
      rc_text.setNum(rc, 16);
      switch (rc) {
	  case 0x2001:
	    rc_text.append(" (OK)");
	    break;
	  case 0x2002:
	    rc_text.append(" (General Error)");
	    break;
	  case 0x2003:
	    rc_text.append(" (Session Not Open)");
	    break;
	  case 0x2005:
	    rc_text.append(" (Operation Not Supported)");
	    break;
	  default:
	    break;
      }

      ui.debug_ptp_rc_entry->setText(rc_text);
}

void CamtoolDebug::debug_ptp_describe_slot_(void)
{
      CameraControl*camera = main_window_->get_selected_camera();
      if (camera == 0)
	    return;

      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);

      string desc = camera->debug_property_describe(prop_code);

      CameraControl::debug_log << "**** Describe 0x" << hex << prop_code << " ****" << endl
			       << dec << desc << endl;
}
