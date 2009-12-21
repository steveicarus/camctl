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
# include  "PTPCamera.h"
# include  <iostream>

using namespace std;


CamtoolDebug::CamtoolDebug(CamtoolMain*parent)
: QDialog(parent), main_window_(parent)
{
      ui.setupUi(this);

      ui.debug_ptp_type_box->clear();
      ui.debug_ptp_type_box->addItem("UNDEF",   QVariant(PTPCamera::TYPE_NONE));
      ui.debug_ptp_type_box->addItem("INT8",    QVariant(PTPCamera::TYPE_INT8));
      ui.debug_ptp_type_box->addItem("UINT8",   QVariant(PTPCamera::TYPE_UINT8));
      ui.debug_ptp_type_box->addItem("INT16",   QVariant(PTPCamera::TYPE_INT16));
      ui.debug_ptp_type_box->addItem("UINT16",  QVariant(PTPCamera::TYPE_UINT16));
      ui.debug_ptp_type_box->addItem("INT32",   QVariant(PTPCamera::TYPE_INT32));
      ui.debug_ptp_type_box->addItem("UINT32",  QVariant(PTPCamera::TYPE_UINT32));
      ui.debug_ptp_type_box->addItem("INT64",   QVariant(PTPCamera::TYPE_INT64));
      ui.debug_ptp_type_box->addItem("UINT64",  QVariant(PTPCamera::TYPE_UINT64));
      ui.debug_ptp_type_box->addItem("INT128",  QVariant(PTPCamera::TYPE_INT128));
      ui.debug_ptp_type_box->addItem("UINT128", QVariant(PTPCamera::TYPE_UINT128));
      ui.debug_ptp_type_box->addItem("String",  QVariant(PTPCamera::TYPE_STRING));

      connect(ui.dump_generic_button,
	      SIGNAL(clicked()),
	      SLOT(dump_generic_slot_()));

      connect(ui.debug_ptp_refresh_button,
	      SIGNAL(clicked()),
	      SLOT(debug_ptp_refresh_slot_()));
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

void CamtoolDebug::debug_ptp_refresh_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

      vector<PTPCamera::code_string_t> properties = camera->ptp_properties_list();

      ui.debug_ptp_select_property->clear();
      ui.debug_ptp_select_property->addItem(QString("<select property>"), QVariant());
      for (vector<PTPCamera::code_string_t>::const_iterator cur = properties.begin()
		 ; cur != properties.end() ; cur ++ ) {
	    ui.debug_ptp_select_property->addItem(cur->second, QVariant(cur->first));
      }
}

void CamtoolDebug::debug_ptp_describe_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

      int prop_idx = ui.debug_ptp_select_property->currentIndex();
      unsigned prop_code = ui.debug_ptp_select_property->itemData(prop_idx).toUInt();

      CameraControl::debug_log << "**** Describe 0x" << hex << prop_code
			       << " ****" << endl;

      uint32_t rc;

	// Probe the camera to refresh details about the property
      camera->ptp_probe_property(prop_code, rc);
	// Get the property type and display it in the property select box.
      PTPCamera::type_code_t prop_type = camera->ptp_get_property_type(prop_code);
      int type_idx = ui.debug_ptp_type_box->findData(QVariant(prop_type));
      ui.debug_ptp_type_box->setCurrentIndex(type_idx);

	// Get and display the "setable" flag.
      if (camera->ptp_get_property_is_setable(prop_code))
	    ui.debug_ptp_setable->setCheckState(Qt::Checked);
      else
	    ui.debug_ptp_setable->setCheckState(Qt::Unchecked);

      vector<QString> prop_enum;
      int prop_enum_idx = camera->ptp_get_property_enum(prop_code, prop_enum);

      ui.debug_ptp_select_value->clear();
      for (vector<QString>::const_iterator cur = prop_enum.begin()
		 ; cur != prop_enum.end() ; cur ++) {

	    ui.debug_ptp_select_value->addItem(*cur);
      }
      ui.debug_ptp_select_value->setCurrentIndex(prop_enum_idx);
}

void CamtoolDebug::debug_ptp_get_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;
#if 0
      unsigned prop_code = ui.debug_ptp_code_entry->text().toULong(0,0);
      unsigned prop_type = ui.debug_ptp_type_box->currentIndex() + 1;
      unsigned long value = 0;

      int rc = camera->debug_property_get(prop_code,prop_type,value);

      QString prop_text;
      prop_text.setNum(value, 16);
      ui.debug_ptp_value_entry->setText(prop_text);

      QString rc_text;
      rc_text.setNum(rc, 16);
#else
      QString rc_text = "----";
#endif
      ui.debug_ptp_rc_entry->setText(rc_text);
}

void CamtoolDebug::debug_ptp_set_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;
#if 0
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
#else
      QString rc_text = "----";
#endif
      ui.debug_ptp_rc_entry->setText(rc_text);
}
