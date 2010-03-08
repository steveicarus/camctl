/*
 * Copyright (c) 2008-2009 Stephen Williams (steve@icarus.com)
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

# include  "MacICACameraControl.h"
# include  <QTreeWidgetItem>
# include  <iomanip>
# include  <stdlib.h>

using namespace std;

MacPTPCameraControl::MacPTPCameraControl(ICAObject dev)
: MacICACameraControl(dev)
{
      uint32_t result_code;
      result_code = ptp_get_device_info();
}

MacPTPCameraControl::~MacPTPCameraControl()
{
}

QTreeWidgetItem*MacPTPCameraControl::describe_camera(void)
{
      QTreeWidgetItem*root = ptp_describe_camera();
      root->addChild(MacICACameraControl::describe_camera());
      return root;
}

CameraControl::capture_resp_t MacPTPCameraControl::capture_image(void)
{
      uint32_t result_code;
      if (! ptp_InitiateCapture(result_code))
	    return CAP_NOT_SUPPORTED;

      debug_log << TIMESTAMP << ": InitiateCapture (result_code="
		<< hex << result_code << dec << ")" << endl << flush;

      return CAP_OK;

}

uint32_t MacPTPCameraControl::ptp_command(uint16_t command,
				      const std::vector<uint32_t>&parms,
				      const unsigned char*send, size_t nsend,
				      unsigned char*recv, size_t nrecv)
{
      size_t pb_size = sizeof(ICAPTPPassThroughPB);
      uint32_t usage_mode = kICACameraPassThruNotUsed;

      if (nsend > 0) {
	    usage_mode = kICACameraPassThruSend;
	    pb_size += nsend - 1;
      } else if (nrecv > 0) {
	    usage_mode = kICACameraPassThruReceive;
	    pb_size += nrecv - 1;
      }

      ICAPTPPassThroughPB*ptp_buf = (ICAPTPPassThroughPB*)malloc(pb_size);
      memset(ptp_buf, 0, pb_size);
      ptp_buf->commandCode = command;
      ptp_buf->numOfInputParams = parms.size();
      for (size_t idx = 0 ; idx < parms.size() ; idx += 1) {
	    ptp_buf->params[idx] = parms[idx];
      }
      ptp_buf->numOfOutputParams = 0;
      ptp_buf->dataUsageMode = usage_mode;
      ptp_buf->dataSize = nsend + nrecv;

      if (nsend > 0) {
	    memcpy(ptp_buf->data, send, nsend);
      }

      /* ICAError irc = */ ica_send_message_(ptp_buf, pb_size);

      uint32_t result_code = ptp_buf->resultCode;
      if (nrecv > 0) {
	    memcpy(recv, ptp_buf->data, nrecv);
      }

      free(ptp_buf);

      return result_code;
}

ICAError MacICACameraControl::ica_send_message_(void*buf, size_t buf_len)
{
      ICAObjectSendMessagePB msg;
      memset(&msg, 0, sizeof(msg));

      msg.object = dev_;
      msg.message.messageType = kICAMessageCameraPassThrough;
      msg.message.startByte = 0;
      msg.message.dataPtr = buf;
      msg.message.dataSize = buf_len;
      msg.message.dataType = kICATypeData;

      return ICAObjectSendMessage(&msg, 0);
}

float MacPTPCameraControl::battery_level(void)
{
      return ptp_battery_level();
}

int MacPTPCameraControl::get_property_index_(unsigned prop_code,
					     std::vector<PTPCamera::labeled_value_t>&table,
					     std::vector<QString>&values)
{
      uint32_t result_code;
      ptp_probe_property(prop_code, result_code);

	// Get the enumeration list
      int rc = ptp_get_property_enum(prop_code, table);
      if (rc < 0) {
	    debug_log << "MacPTPCameraControl::get_property_index_: "
		      << "Property " << hex << prop_code << dec
		      << "is not an enum?!" << endl << flush;
	    table.clear();
	    values.clear();
	    return -1;
      }

      values.resize(table.size());

      debug_log << "MacPTPCameraControl::get_property_index_ "
		<< "prop=" << hex << prop_code << dec
		<< " count=" << values.size() << endl;
      for (unsigned idx = 0 ; idx < table.size() ; idx += 1) {
	    values[idx] = table[idx].label;
	    debug_log << "   " << idx << ": " << values[idx].toStdString() << endl;
      }

      debug_log << flush;
      return rc;
}

void MacPTPCameraControl::set_property_index_(unsigned prop_code,
					      std::vector<PTPCamera::labeled_value_t>&table,
					      int use_index)
{
      if (use_index < 0)
	    return;
      if (table.size() == 0)
	    return;
      if (use_index >= table.size())
	    use_index = 0;

      uint32_t rc;
      ptp_set_property(prop_code, table[use_index].value, rc);
}

int MacPTPCameraControl::get_image_size_index(vector<QString>&values)
{
      return get_property_index_(PROP_ImageSize, image_size_, values);
}

void MacPTPCameraControl::set_image_size_index(int use_index)
{
      set_property_index_(PROP_ImageSize, image_size_, use_index);
}

bool MacPTPCameraControl::set_image_size_ok()
{
      return ptp_get_property_is_setable(PROP_ImageSize);
}

int MacPTPCameraControl::get_exposure_program_index(vector<QString>&values)
{
      return get_property_index_(PROP_ExposureProgramMode, exposure_program_, values);
}

void MacPTPCameraControl::set_exposure_program_index(int use_index)
{
      set_property_index_(PROP_ExposureProgramMode, exposure_program_, use_index);
}

bool MacPTPCameraControl::set_exposure_program_ok()
{
      return ptp_get_property_is_setable(PROP_ExposureProgramMode);
}

int MacPTPCameraControl::get_exposure_time_index(vector<QString>&values)
{
      return get_property_index_(PROP_ExposureTime, exposure_time_, values);
}

void MacPTPCameraControl::set_exposure_time_index(int use_index)
{
      set_property_index_(PROP_ExposureTime, exposure_time_, use_index);
}

bool MacPTPCameraControl::set_exposure_time_ok()
{
      return ptp_get_property_is_setable(PROP_ExposureTime);
}

int MacPTPCameraControl::get_fnumber_index(vector<QString>&values)
{
      return get_property_index_(PROP_FNumber, fnumber_, values);
}

void MacPTPCameraControl::set_fnumber_index(int use_index)
{
      set_property_index_(PROP_FNumber, fnumber_, use_index);
}

bool MacPTPCameraControl::set_fnumber_ok()
{
      return ptp_get_property_is_setable(PROP_FNumber);
}

int MacPTPCameraControl::get_iso_index(vector<QString>&values)
{
      return get_property_index_(PROP_ExposureIndex, iso_, values);
}

void MacPTPCameraControl::set_iso_index(int use_index)
{
      set_property_index_(PROP_ExposureIndex, iso_, use_index);
}

bool MacPTPCameraControl::set_iso_ok()
{
      return ptp_get_property_is_setable(PROP_ExposureIndex);
}

int MacPTPCameraControl::get_flash_mode_index(vector<QString>&values)
{
      return get_property_index_(PROP_FlashMode, flash_mode_, values);
}

void MacPTPCameraControl::set_flash_mode_index(int use_index)
{
      set_property_index_(PROP_FlashMode, flash_mode_, use_index);
}

bool MacPTPCameraControl::set_flash_mode_ok()
{
      return ptp_get_property_is_setable(PROP_FlashMode);
}


int MacPTPCameraControl::get_focus_mode_index(vector<QString>&values)
{
      return get_property_index_(PROP_FocusMode, focus_mode_, values);
}

void MacPTPCameraControl::set_focus_mode_index(int use_index)
{
      set_property_index_(PROP_FocusMode, focus_mode_, use_index);
}

bool MacPTPCameraControl::set_focus_mode_ok()
{
      return ptp_get_property_is_setable(PROP_FocusMode);
}

int MacPTPCameraControl::get_white_balance_index(vector<QString>&values)
{
      return get_property_index_(PROP_WhiteBalance, white_balance_, values);
}

void MacPTPCameraControl::set_white_balance_index(int use_index)
{
      set_property_index_(PROP_WhiteBalance, white_balance_, use_index);
}

bool MacPTPCameraControl::set_white_balance_ok()
{
      return ptp_get_property_is_setable(PROP_WhiteBalance);
}
