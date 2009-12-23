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

static QString to_qstring(const PTPCamera::prop_value_t&val)
{
      switch (val.get_type()) {
	  case PTPCamera::TYPE_STRING:
	    return val.get_string();
	  case PTPCamera::TYPE_INT8:
	    return QString("%1").arg(val.get_int8());
	  case PTPCamera::TYPE_UINT8:
	    return QString("0x%1").arg(val.get_uint8(), 2, 16,QLatin1Char('0'));
	  case PTPCamera::TYPE_INT16:
	    return QString("%1").arg(val.get_int16());
	  case PTPCamera::TYPE_UINT16:
	    return QString("0x%1").arg(val.get_uint16(), 4, 16,QLatin1Char('0'));
	  case PTPCamera::TYPE_INT32:
	    return QString("%1").arg(val.get_int32());
	  case PTPCamera::TYPE_UINT32:
	    return QString("0x%1").arg(val.get_uint32(), 8, 16,QLatin1Char('0'));
	  default:
	    return QString("?type=%1?").arg(val.get_type(), 4, 16, QLatin1Char('0'));
      }
}

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

/*
 * This slot re-draws the list of supported properties.
 */
void CamtoolDebug::debug_ptp_refresh_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

      vector<PTPCamera::code_string_t> properties = camera->ptp_properties_list();

      ui.debug_ptp_select_property->clear();
      ui.debug_ptp_select_property->addItem(QString("<select a property>"), QVariant());
      for (vector<PTPCamera::code_string_t>::const_iterator cur = properties.begin()
		 ; cur != properties.end() ; cur ++ ) {
	    ui.debug_ptp_select_property->addItem(cur->second, QVariant(cur->first));
      }
}

/*
 * This slot probes the camera for the complete description of the
 * selected property, and sets the entry boxes to reflect the
 * description of the property.
 */
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

	// Fill in the enumeration of values
      vector<QString> prop_enum;
      int prop_enum_idx = camera->ptp_get_property_enum(prop_code, prop_enum);

      ui.debug_ptp_select_value->clear();
      for (vector<QString>::const_iterator cur = prop_enum.begin()
		 ; cur != prop_enum.end() ; cur ++) {

	    ui.debug_ptp_select_value->addItem(*cur);
      }
      ui.debug_ptp_select_value->setCurrentIndex(prop_enum_idx);

	// Display the actual current value in the value entry box
      ui.debug_ptp_value_entry->setText(to_qstring(camera->ptp_get_property_current(prop_code)));
      ui.debug_ptp_rc_entry->setText(PTPCamera::qresult_code(rc));
}

/*
 * This slot re-reads the current value from the camera (without doing
 * a full describe) and displays the returned value into the value
 * entry box.
 */
void CamtoolDebug::debug_ptp_get_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

      int prop_idx = ui.debug_ptp_select_property->currentIndex();
      unsigned prop_code = ui.debug_ptp_select_property->itemData(prop_idx).toUInt();
      uint32_t rc;
      ui.debug_ptp_value_entry->setText(to_qstring(camera->ptp_get_property(prop_code,rc)));
      ui.debug_ptp_rc_entry->setText(PTPCamera::qresult_code(rc));
}

void CamtoolDebug::debug_ptp_set_slot_(void)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

	// Get the property code that the user selected...
      int prop_idx = ui.debug_ptp_select_property->currentIndex();
      unsigned prop_code = ui.debug_ptp_select_property->itemData(prop_idx).toUInt();

      PTPCamera::type_code_t prop_type = camera->ptp_get_property_type(prop_code);

	// Get the value that the user entered...
      QString val_string = ui.debug_ptp_value_entry->text();
      PTPCamera::prop_value_t val;

      switch (prop_type) {
	  case PTPCamera::TYPE_NONE:
	    break;
	  case PTPCamera::TYPE_INT8:
	    val.set_int8(val_string.toInt());
	    break;
	  case PTPCamera::TYPE_UINT8:
	    val.set_uint8(val_string.toUInt());
	    break;
	  case PTPCamera::TYPE_INT16:
	    val.set_int16(val_string.toInt());
	    break;
	  case PTPCamera::TYPE_UINT16:
	    val.set_uint16(val_string.toUInt());
	    break;
	  case PTPCamera::TYPE_INT32:
	    val.set_int32(val_string.toLong());
	    break;
	  case PTPCamera::TYPE_UINT32:
	    val.set_uint32(val_string.toULong());
	    break;
	  case PTPCamera::TYPE_STRING:
	    val.set_string(val_string);
	    break;

	  default:
	    ui.debug_ptp_value_entry->setText("----");
	    ui.debug_ptp_rc_entry->setText("----");
	    return;
      }

      uint32_t rc;
      camera->ptp_set_property(prop_code, val, rc);
      ui.debug_ptp_value_entry->setText(to_qstring(val));
      ui.debug_ptp_rc_entry->setText(PTPCamera::qresult_code(rc));
}
