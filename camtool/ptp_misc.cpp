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

# include  "ptp_misc.h"
# include  <sstream>

using namespace std;

static string ptp_standard_opcodes[] = {
      "Undefined",        // 0x1000
      "GetDeviceInfo",    // 0x1001
      "OpenSession",      // 0x1002
      "CloseSession",     // 0x1003
      "GetStorageIDs",    // 0x1004
      "GetStorageInfo",   // 0x1005
      "GetNumObjects",    // 0x1006
      "GetObjectHandles", // 0x1007
      "GetObjectInfo",    // 0x1008
      "GetObject",        // 0x1009
      "GetThumb",         // 0x100a
      "DeteteObject",     // 0x100b
      "SendObjectInfo",   // 0x100c
      "SendObject",       // 0x100d
      "InitiateCapture",  // 0x100e
      "FormatStore",      // 0x100f
      "ResetDevice",      // 0x1010
      "SelfTest",         // 0x1011
      "SetObjectProtection",// 0x1012
      "PowerDown",          // 0x1013
      "GetDevicePropDesc",  // 0x1014
      "GetDevicePropValue", // 0x1015
      "SetDevicePropValue", // 0x1016
      "ResetDevicePropValue", // 0x1017
      "TerminateOpenCapture", // 0x1018
      "MoveObject",         // 0x1019
      "CopyObject",         // 0x101a
      "GetPartialObject",   // 0x101b
      "InitiateOpenCapture" // 0x101c
};

static string ptp_standard_properties[] = {
      "Undefined",          // 0x5000
      "BatteryLevel",       // 0x5001
      "FunctionalMode",     // 0x5002
      "ImageSize",          // 0x5003
      "CompressionSetting", // 0x5004
      "WhiteBalance",       // 0x5005
      "RGB Gain",           // 0x5006
      "F-Number",           // 0x5007
      "FocalLength",        // 0x5008
      "FocusDistance",      // 0x5009
      "FocusMode",          // 0x500a
      "ExposureMeteringMode", // 0x500b
      "FlashMode",          // 0x500c
      "ExposureTime",       // 0x500d
      "ExposureProgramMode",// 0x500e
      "ExposureIndex",      // 0x500f
      "ExposureBiasCompensation", // 0x5010
      "DateTime",           // 0x5011
      "CaptureDelay",       // 0x5012
      "StillCaptureMode",   // 0x5013
      "Contrast",           // 0x5014
      "Sharpness",          // 0x5015
      "DigitalZoom",        // 0x5016
      "EffectMode",         // 0x5017
      "BurstNumber",        // 0x5018
      "BurstInterval",      // 0x5019
      "TimelapseNumber",    // 0x501a
      "TimelapseInterval",  // 0x501b
      "FocusMeteringMode",  // 0x501c
      "UploadURL",          // 0x501d
      "Artist",             // 0x501e
      "CopyrightInfo"       // 0x501f
};

template<class KT, class VT> struct key_value {
      KT key;
      VT value;
};

static key_value<uint16_t,string> ptp_novendor_operation_codes[] = {
      { 0xffff, "" }
};

static key_value<uint16_t,string> ptp_nikon_operation_codes[] = {
      { 0x90c0, "NIKON Capture" },
      { 0x90c1, "NIKON AfDrive" },
      { 0x90c2, "NIKON SetControlMode" },
      { 0x90c3, "NIKON DelImageSDRAM" },
      { 0x90c4, "NIKON 90c4" },
      { 0x90c5, "NIKON CurveDownload" },
      { 0x90c6, "NIKON CurveUpload" },
      { 0x90c7, "NIKON CheckEvent" },
      { 0x90c8, "NIKON DeviceReady" },
      { 0x90c9, "NIKON SetPreWBData" },
      { 0x90ca, "NIKON 90ca" },
      { 0x90cb, "NIKON AfCaptureSDRAM" },
      { 0x9801, "MTP GetObjectPropsSupported" },
      { 0x9802, "MTP GetObjectPropDesc" },
      { 0x9803, "MTP GetObjectPropValue" },
      { 0x9804, "MTP SetObjectPropValue" },
      { 0x9805, "MTP GetObjPropList" },
      { 0xffff, "" }
};

string ptp_opcode_string(uint16_t code, uint32_t extension_id)
{
      if ( (code&0xf000) == 0x1000 ) { // PTP Standard Opcodes
	    if ((code&0x0fff) <= 0x001c)
		  return ptp_standard_opcodes[code&0x0fff];

	    ostringstream tmp;
	    tmp << "Reserved-" << hex << code << ends;
	    return tmp.str();
      }

      if ( (code&0xf000) == 0x9000 ) { // Vendor Opcodes

	    key_value<uint16_t,string>*vendor_operation_codes;
	    switch (extension_id) {
		case 0x00a:
		  vendor_operation_codes = ptp_nikon_operation_codes;
		  break;
		default:
		  vendor_operation_codes = ptp_novendor_operation_codes;
		  break;
	    }

	    while (vendor_operation_codes->key < code)
		  vendor_operation_codes += 1;

	    if (vendor_operation_codes->key == code)
		  return vendor_operation_codes->value;

	    ostringstream tmp;
	    tmp << "Vendor[" << hex << extension_id << "]-" << hex << code << ends;
	    return tmp.str();
      }

      ostringstream tmp;
      tmp << "Invalid-" << hex << code << ends;
      return tmp.str();
}

string ptp_property_string(uint16_t code, uint32_t)
{
      if ( (code&0xf000) == 0x5000 ) { // PTP Standard Properties

	    if ( (code&0x0fff) <= 0x1f )
		  return ptp_standard_properties[code&0x0fff];

	    ostringstream tmp;
	    tmp << "Reserved-" << hex << code << ends;
	    return tmp.str();
      }

      if ( (code&0xf000) == 0xd000 ) { // Vendor Properties

	    ostringstream tmp;
	    tmp << "Vendor-" << hex << code << ends;
	    return tmp.str();
      }

      ostringstream tmp;
      tmp << "Invalid-" << hex << code << ends;
      return tmp.str();
}
