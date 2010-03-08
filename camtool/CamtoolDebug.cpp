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
# include  "CamtoolDebug.h"
# include  "CamtoolMain.h"
# include  "PTPCamera.h"
# include  <iostream>

using namespace std;

ostream& operator << (ostream&out, const PTPCamera::prop_value_t&val)
{
      switch (val.get_type()) {
	  case PTPCamera::TYPE_NONE:
	    out << "<NIL>";
	    break;
	  case PTPCamera::TYPE_INT8:
	    out << val.get_int8();
	    break;
	  case PTPCamera::TYPE_UINT8:
	    out << val.get_uint8();
	    break;
	  case PTPCamera::TYPE_INT16:
	    out << val.get_int16();
	    break;
	  case PTPCamera::TYPE_UINT16:
	    out << val.get_uint16();
	    break;
	  case PTPCamera::TYPE_INT32:
	    out << val.get_int32();
	    break;
	  case PTPCamera::TYPE_UINT32:
	    out << val.get_uint32();
	    break;
	  case PTPCamera::TYPE_STRING:
	    out << val.get_string().toStdString();
	    break;
	  default:
	    out << "<?>";
	    break;
      }

      return out;
}

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

static PTPCamera::prop_value_t to_prop_value(QVariant prop_val, PTPCamera::type_code_t type)
{
      PTPCamera::prop_value_t val;

      switch (type) {
	  case PTPCamera::TYPE_NONE:
	    break;
	  case PTPCamera::TYPE_INT8:
	    val.set_int8(prop_val.toInt());
	    break;
	  case PTPCamera::TYPE_UINT8:
	    val.set_uint8(prop_val.toUInt());
	    break;
	  case PTPCamera::TYPE_INT16:
	    val.set_int16(prop_val.toInt());
	    break;
	  case PTPCamera::TYPE_UINT16:
	    val.set_uint16(prop_val.toUInt());
	    break;
	  case PTPCamera::TYPE_INT32:
	    val.set_int32(prop_val.toInt());
	    break;
	  case PTPCamera::TYPE_UINT32:
	    val.set_uint32(prop_val.toUInt());
	    break;
	  case PTPCamera::TYPE_STRING:
	    val.set_string(prop_val.toString());
	    break;
      }

      return val;
}

static QVariant to_qvariant(const PTPCamera::prop_value_t&val)
{
      QVariant res;

      int tmp_int;
      unsigned tmp_uint;

      switch (val.get_type()) {
	  case PTPCamera::TYPE_INT8:
	    tmp_int = val.get_int8();
	    res.setValue(tmp_int);
	    break;
	  case PTPCamera::TYPE_INT16:
	    tmp_int = val.get_int16();
	    res.setValue(tmp_int);
	    break;
	  case PTPCamera::TYPE_INT32:
	    tmp_int = val.get_int32();
	    res.setValue(tmp_int);
	    break;
	  case PTPCamera::TYPE_UINT8:
	    tmp_uint = val.get_uint8();
	    res.setValue(tmp_uint);
	    break;
	  case PTPCamera::TYPE_UINT16:
	    tmp_uint = val.get_uint16();
	    res.setValue(tmp_uint);
	    break;
	  case PTPCamera::TYPE_UINT32:
	    tmp_uint = val.get_uint32();
	    res.setValue(tmp_uint);
	    break;
	  case PTPCamera::TYPE_STRING:
	    res.setValue(val.get_string());
	    break;
      }

      return res;
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
      connect(ui.debug_ptp_select_value,
	      SIGNAL(currentIndexChanged(int)),
	      SLOT(debug_ptp_select_value_slot_(int)));
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

	// The Properties combo box is a list of all the properties
	// that the camera supports. The display text is the PTP
	// standard name for the property, and the data is the
	// uint16_t value for the code.
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

      CameraControl::debug_log << "**** Describe 0x" << hex << prop_code << dec
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

	// Fill in the enumeration of values from the camera
	// object. The get_property_enum will create all the prop_enum
	// items for us as a vector of labeled values.
      vector<PTPCamera::labeled_value_t> prop_enum;
      int prop_enum_idx = camera->ptp_get_property_enum(prop_code, prop_enum);

	// Create ComboBox selections for the enumerated values. Use
	// the label as the display value, and store the actual data
	// as an associated data item.
      ui.debug_ptp_select_value->clear();
      for (vector<PTPCamera::labeled_value_t>::const_iterator cur = prop_enum.begin()
		 ; cur != prop_enum.end() ; cur ++) {

	    QVariant tmp = to_qvariant(cur->value);
	    assert(tmp.isValid());
	    ui.debug_ptp_select_value->addItem(cur->label, to_qvariant(cur->value));
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

/*
 * This slot is invoked when the user select a value from the
 * enumeration combo box. Get the value selected by the user and write
 * it into the entry box. Do not actually send the new value to the
 * camera, leave that to the "Set" button.
 */
void CamtoolDebug::debug_ptp_select_value_slot_(int value_idx)
{
      PTPCamera*camera = dynamic_cast<PTPCamera*> (main_window_->get_selected_camera());
      if (camera == 0)
	    return;

	// Get the property code for the property that is currently
	// active...
      int prop_idx = ui.debug_ptp_select_property->currentIndex();
      unsigned prop_code = ui.debug_ptp_select_property->itemData(prop_idx).toUInt();

	// Get the type for the property. Use this type to inform how
	// we process (format) the value that was selected.
      PTPCamera::type_code_t prop_type = camera->ptp_get_property_type(prop_code);

	// Get the value. Use the associated QVariant of the data, and
	// the prop_type of the property to create a prop_value_t
	// object with the right value.
      QVariant prop_val = ui.debug_ptp_select_value->itemData(value_idx);
      PTPCamera::prop_value_t val = to_prop_value(prop_val, prop_type);

	// Display the value into the value entry box.
      ui.debug_ptp_value_entry->setText(to_qstring(val));
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

      bool ok;
      switch (prop_type) {
	  case PTPCamera::TYPE_NONE:
	    break;
	  case PTPCamera::TYPE_INT8:
	    val.set_int8(val_string.toInt(&ok, 0));
	    break;
	  case PTPCamera::TYPE_UINT8:
	    val.set_uint8(val_string.toUInt(&ok, 0));
	    break;
	  case PTPCamera::TYPE_INT16:
	    val.set_int16(val_string.toInt(&ok, 0));
	    break;
	  case PTPCamera::TYPE_UINT16:
	    val.set_uint16(val_string.toUInt(&ok, 0));
	    break;
	  case PTPCamera::TYPE_INT32:
	    val.set_int32(val_string.toLong(&ok, 0));
	    break;
	  case PTPCamera::TYPE_UINT32:
	    val.set_uint32(val_string.toULong(&ok, 0));
	    break;
	  case PTPCamera::TYPE_STRING:
	    val.set_string(val_string);
	    break;

	  default:
	    ui.debug_ptp_value_entry->setText("----");
	    ui.debug_ptp_rc_entry->setText("----");
	    return;
      }

      CameraControl::debug_log << "XXXX Send value " << val_string.toStdString()
			       << ", prop_type=" << prop_type
			       << ", val_string.toUInt()=" << val_string.toUInt()
			       << ", val=" << val << ", to camera." << endl;
      uint32_t rc;
      camera->ptp_set_property(prop_code, val, rc);
      ui.debug_ptp_value_entry->setText(to_qstring(val));
      ui.debug_ptp_rc_entry->setText(PTPCamera::qresult_code(rc));
}
