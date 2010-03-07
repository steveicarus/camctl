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
# define array_count(x) (sizeof(x) / sizeof((x)[0]))

/*
 * The tables below are organized as key-value pairs. The key_value
 * template defines the relationship between key and value.
 */
template<class KT, class VT> struct key_value {
      KT key;
      VT value;
};

template <class KT, class VT>
static size_t binary_search(KT key, key_value<KT,VT>*table, size_t table_size)
{
	// The key must be at >= lp (if present at all)
      size_t lp = 0;
	// The key must be at < hp (if present at all)
      size_t hp = table_size;

      while (lp < hp) {
	    size_t mid = (lp + hp) / 2;
	      // assert: mid >= lp
	      // assert: mid < hp

	    if (table[mid].key == key) // Lucky guess?
		  return mid;

	    if (table[mid].key < key) {
		    // This assures that the search space shrinks
		  assert(mid >= lp);
		    // This assures that we stay within the table
		  assert(mid < hp);
		  lp = mid+1;
	    } else { // table[mid] > key
		    // This assures that the search space shrinks
		  assert(mid < hp);
		  hp = mid;
	    }
      }

      assert(lp == hp);
      if (lp == table_size)
	    return lp;
      if (table[lp].key != key)
	    return table_size;
      return lp;
}

/*
 * In some cases, the key needs to be a pair of values. This template
 * allows the table to define the key as a pair of types. Note that
 * the types must support the <, != and == operators.
 */
template<class K1, class K2> struct key_pair {
      K1 key1;
      K2 key2;
};

template <class K1, class K2>
bool operator < (const key_pair<K1,K2>&a, const key_pair<K1,K2>&b)
{
      if (a.key1 < b.key1) return true;
      if (b.key1 < a.key1) return false;
      if (a.key2 < b.key2) return true;
      return false;
}

template <class K1, class K2>
static inline bool operator != (const key_pair<K1,K2>&a, const key_pair<K1,K2>&b)
{
      if (a.key1 != b.key1) return true;
      if (a.key2 != b.key2) return true;
      return false;
}

template <class K1, class K2>
static inline bool operator == (const key_pair<K1,K2>&a, const key_pair<K1,K2>&b)
{
      return ! (a != b);
}

/*
 * The standard opcodes are all within a contiguous range, so this is
 * an ordered lookup table instead of a map.
 */
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
      { 0x9805, "MTP GetObjPropList" }
};

static key_value<uint16_t,string> ptp_canon_operation_codes[] = {
      { 0x9001, "CANON GetPartialObjectInfo" },
      { 0x9002, "CANON SetObjectArchive" },
      { 0x9003, "CANON KeepDeviceOn" },
      { 0x9004, "CANON LockDeviceUI" },
      { 0x9005, "CANON UnlockDeviceUI" },
      { 0x9006, "CANON GetObjectHandleByName" },
      { 0x9008, "CANON InitiateReleaseControl" },
      { 0x9009, "CANON TerminateReleaseControl" },
      { 0x900a, "CANON TerminatePlaybackMode" },
      { 0x900b, "CANON ViewfinderOn" },
      { 0x900c, "CANON ViewfinderOff" },
      { 0x900d, "CANON DoAeAfAwb" },
      { 0x900e, "CANON GetCustomizeSpec" },
      { 0x900f, "CANON GetCustomizeItemInfo" },
      { 0x9010, "CANON GetCustomizeData" },
      { 0x9011, "CANON SetCustomizeData" },
      { 0x9012, "CANON GetCaptureStatus" },
      { 0x9013, "CANON CheckEvent" },
      { 0x9014, "CANON FocusLock" },
      { 0x9015, "CANON FocusUnlock" },
      { 0x9016, "CANON GetLocalReleaseParam" },
      { 0x9017, "CANON SetLocalReleaseParam" },
      { 0x9018, "CANON AskAboutPcEvf" },
      { 0x9019, "CANON SendPartialObject" },
      { 0x901a, "CANON InitiateCaptureInMemory" },
      { 0x901b, "CANON GetPartialObjectEx" },
      { 0x901c, "CANON SetObjectTime" },
      { 0x901d, "CANON GetViewfinderImage" },
      { 0x901e, "CANON GetObjectAttributes" },
      { 0x901f, "CANON ChangeUSBProtocol" },
      { 0x9020, "CANON GetChanges" },
      { 0x9021, "CANON GetObjectInfoEx" },
      { 0x9022, "CANON InitiateDirectTransfer" },
      { 0x9023, "CANON TerminateDirectTransfer" },
      { 0x9024, "CANON SendObjectInfoByPath" },
      { 0x9025, "CANON SendObjectByPath" },
      { 0x9026, "CANON InitiateDirectTransferEx" },
      { 0x9027, "CANON GetAncillaryObjectHandles" },
      { 0x9028, "CANON GetTreeInfo" },
      { 0x9029, "CANON GetTreeSize" },
      { 0x902a, "CANON NotifyProgress" },
      { 0x902b, "CANON NotifyCancelAccepted" },
      { 0x902d, "CANON GetDirectory" },
      { 0x9030, "CANON SetPairingInfo" },
      { 0x9031, "CANON GetPairingInfo" },
      { 0x9032, "CANON DeletePairingInfo" },
      { 0x9033, "CANON GetMACAddress" },
      { 0x9034, "CANON SetDisplayMonitor" },
      { 0x9035, "CANON PairingComplete" },
      { 0x9036, "CANON GetWirelessMAXChannel" }
};

static string ptp_standard_events[] = {
      "Undefined",         // 0x4000
      "CancelTransaction", // 0x4001
      "ObjectAdded",       // 0x4002
      "ObjectRemoved",     // 0x4003
      "StoreAdded",        // 0x4004
      "StoreRemoved",      // 0x4005
      "DevicePropChanged", // 0x4006
      "ObjectInfoChanged", // 0x4007
      "DeviceInfoChanged", // 0x4008
      "RequestObjectTransfer", // 0x4009
      "StoreFull",         // 0x400a
      "DeviceReset",       // 0x400b
      "StoreInfoChanged",  // 0x400c
      "CaptureComplete",   // 0x400d
      "UnreportedStatus"  // 0x400e
};

static string ptp_vendor_canon_events[] = {
      "CANON 0xc000",    // 0xc000
      "CANON 0xc001",    // 0xc001
      "CANON 0xc002",    // 0xc002
      "CANON 0xc003",    // 0xc003
      "CANON 0xc004",    // 0xc004
      "CANON ExtendedErrorcode",    // 0xc005
      "CANON 0xc006",    // 0xc006
      "CANON 0xc007",    // 0xc007
      "CANON ObjectInfoChanged",     // 0xc008
      "CANON RequestObjectTransfer", // 0xc009
      "CANON 0xc00a",    // 0xc00a
      "CANON 0xc00b",    // 0xc00b
      "CANON CameraModeChanged",     // 0xc00c
      "CANON 0xc00d",    // 0xc00d
      "CANON 0xc00e",    // 0xc00e
      "CANON 0xc00f",    // 0xc00f
      "CANON 0xc010",    // 0xc010
      "CANON StartDirectTransfer",   // 0xc011
      "CANON 0xc012",    // 0xc012
      "CANON StopDirectTransfer"     // 0xc013
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
      "ExposureMeteringMode",// 0x500b
      "FlashMode",          // 0x500c
      "ExposureTime",       // 0x500d
      "ExposureProgramMode",// 0x500e
      "ExposureIndex",      // 0x500f
      "ExposureBiasCompensation",// 0x5010
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

static string ptp_vendor_canon_properties[] = {
      "Undefined",          // 0xd000
      "BeepMode",           // 0xd001
      "BatteryKind",        // 0xd002
      "BatteryStatus",      // 0xd003
      "UILockType",         // 0xd004
      "CameraMode",         // 0xd005
      "ImageQuality",       // 0xd006
      "FullViewFileFormat", // 0xd007
      "ImageSize",          // 0xd008
      "SelfTime",           // 0xd009
      "FlashMode",          // 0xd00a
      "Beep",               // 0xd00b
      "ShootingMode",       // 0xd00c
      "ImageMode",          // 0xd00d
      "DriveMode",          // 0xd00e
      "EZoom",              // 0xd00f
      "MeteringMode",       // 0xd010
      "AFDistance",         // 0xd011
      "FocusingPoint",      // 0xd012
      "WhiteBalance",       // 0xd013
      "SlowShutterSetting", // 0xd014
      "AFMode",             // 0xd015
      "ImageStabilization", // 0xd016
      "Contrast",           // 0xd017
      "ColorGain",          // 0xd018
      "Sharpness",          // 0xd019
      "Sensitivity",        // 0xd01a
      "ParameterSet",       // 0xd01b
      "ISOSpeed",           // 0xd01c
      "Aperture",           // 0xd01d
      "ShutterSpeed",       // 0xd01e
      "ExpCompensation",    // 0xd01f
      "FlashCompensation",  // 0xd020
      "AEBExposureCompensation", // 0xd021
      "0xd022",             // 0xd022
      "AvOpen",             // 0xd023
      "AvMax",              // 0xd024
      "FocalLength",        // 0xd025
      "FocalLengthTele",    // 0xd026
      "FocalLengthWide",    // 0xd027
      "FocalLengthDenominator", // 0xd028
      "CaptureTransferMode",// 0xd029
      "Zoom",               // 0xd02a
      "NamePrefix",         // 0xd02b
      "SizeQualityMode",    // 0xd02c
      "SupportedThumbSize", // 0xd02d
      "SizeOfOutputDataFromCamera", // 0xd02e
      "SizeOfInputDataToCamera",    // 0xd02f
      "RemoteAPIVersion",   // 0xd030
      "FirmwareVersion",    // 0xd031
      "CameraModel",        // 0xd032
      "CameraOwner",        // 0xd033
      "UnixTime",           // 0xd034
      "CameraBodyID",       // 0xd035
      "CameraOutput",       // 0xd036
      "DispAv",             // 0xd037
      "AvOpenApex",         // 0xd038
      "DZoomMagnification", // 0xd039
      "MlSpotPos",          // 0xd03a
      "DispAvMax",          // 0xd03b
      "AvMaxApex",          // 0xd03c
      "EZoomStartPosition", // 0xd03d
      "FocalLengthOfTele",  // 0xd03e
      "EZoomSizeOfTele",    // 0xd03f
      "PhotoEffect",        // 0xd040
      "AssistLight",        // 0xd041
      "FlashQuantityCount", // 0xd042
      "RotationAngle",      // 0xd043
      "RotationScene",      // 0xd044
      "EventEmulationMode", // 0xd045
      "DPOFVersion",        // 0xd046
      "TypeOfSupportedSlideShow", // 0xd047
      "AverageFilesizes",   // 0xd048
      "ModelID"             // 0xd049
};

/*
 * Various properties with integer values are mapped to values. Keep
 * all the property-value maps to this single sorted
 * table. Vendor-specific property values are put into vendor-specific tables.
 */
static key_value<key_pair<uint16_t,uint16_t>,string> ptp_standard_property16_values[] = {
	// WhiteBalance
      { { 0x5005, 0x0000 }, "Undefined" },
      { { 0x5005, 0x0001 }, "Manual" },
      { { 0x5005, 0x0002 }, "Automatic" },
      { { 0x5005, 0x0003 }, "One-push Automatic" },
      { { 0x5005, 0x0004 }, "Daylight" },
      { { 0x5005, 0x0005 }, "Flourescent" },
      { { 0x5005, 0x0006 }, "Tungsten" },
      { { 0x5005, 0x0007 }, "Flash" },
	// FocusMode
      { { 0x500a, 0x0000 }, "Undefined" },
      { { 0x500a, 0x0001 }, "Manual" },
      { { 0x500a, 0x0002 }, "Automatic" },
      { { 0x500a, 0x0003 }, "Automatic/Macro" },
	// FlashMode
      { { 0x500c, 0x0000 }, "Undefined" },
      { { 0x500c, 0x0001 }, "auto flash" },
      { { 0x500c, 0x0002 }, "Flash off" },
      { { 0x500c, 0x0003 }, "Fill flash" },
      { { 0x500c, 0x0004 }, "Red eye auto" },
      { { 0x500c, 0x0005 }, "Red eye fill" },
      { { 0x500c, 0x0006 }, "External Sync" },
	// ExposureProgramMode
      { { 0x500e, 0x0000 }, "Undefined" },
      { { 0x500e, 0x0001 }, "Manual" },
      { { 0x500e, 0x0002 }, "Automatic" },
      { { 0x500e, 0x0003 }, "Aperture Priority" },
      { { 0x500e, 0x0004 }, "Shutter Priority" },
      { { 0x500e, 0x0005 }, "Program Creative" },
      { { 0x500e, 0x0006 }, "Program Action" },
      { { 0x500e, 0x0007 }, "Portait" }
	// ExposureIndex (0x500f) is handled like a range
};

static key_value<key_pair<uint16_t,uint16_t>,string> ptp_nikon_property16_values[] = {
	// WhiteBalance
      { { 0x5005, 0x8010 }, "NIKON Cloudy" },
      { { 0x5005, 0x8011 }, "NIKON Shade" },
      { { 0x5005, 0x8012 }, "NIKON Color Temperature" },
      { { 0x5005, 0x8013 }, "NIKON Preset White Balance" },
	// FlashMode
      { { 0x500c, 0x8010 }, "NIKON Fill-flash" },
      { { 0x500c, 0x8011 }, "NIKON Slow sync" },
      { { 0x500c, 0x8012 }, "NIKON Rear curtain + Slow sync" },
      { { 0x500c, 0x8013 }, "NIKON Slow sync + Red eye" },
	// ExposureProgramMode
      { { 0x500e, 0x8010 }, "NIKON Auto" },
      { { 0x500e, 0x8011 }, "NIKON Portrait" },
      { { 0x500e, 0x8012 }, "NIKON Landscape" },
      { { 0x500e, 0x8013 }, "NIKON Close up" },
      { { 0x500e, 0x8014 }, "NIKON Sports" },
      { { 0x500e, 0x8015 }, "NIKON Night portrait" },
      { { 0x500e, 0x8016 }, "NIKON Night Landscape" }
};

static key_value<key_pair<uint16_t,uint32_t>,string> ptp_standard_property32_values[] = {
};

static key_value<key_pair<uint16_t,uint32_t>,string> ptp_nikon_property32_values[] = {
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

	    key_value<uint16_t,string>*table;
	    size_t table_size = 0;
	    switch (extension_id) {
		case 0x00a: // Nikon
		  table = ptp_nikon_operation_codes;
		  table_size = array_count(ptp_nikon_operation_codes);
		  break;
		case 0x00b: // Canon
		  table = ptp_canon_operation_codes;
		  table_size = array_count(ptp_canon_operation_codes);
		  break;
		default: 
		  table = ptp_novendor_operation_codes;
		  table_size = array_count(ptp_novendor_operation_codes);
		  break;
	    }

	    size_t key = binary_search(code, table, table_size);
	    if (key < table_size) {
		  assert(table[key].key == code);
		  return table[key].value;
	    }

	    ostringstream tmp;
	    tmp << "Vendor[" << hex << extension_id << "]-" << hex << code << ends;
	    return tmp.str();
      }

      ostringstream tmp;
      tmp << "Invalid-" << hex << code << ends;
      return tmp.str();
}

string ptp_event_string(uint16_t code, uint32_t vend)
{
      if ( (code&0xf000) == 0x4000 ) { // PTP Standard Events
	    if ((code&0x0fff) <= 0x000f)
		  return ptp_standard_events[code&0x0fff];

	    ostringstream tmp;
	    tmp << "Reserved-" << hex << code << ends;
	    return tmp.str();
      }

      if ( (code&0xf000) == 0xc000 && vend==0x0000000b
	   && (code&0x0fff) < array_count(ptp_vendor_canon_properties)) {
	    return ptp_vendor_canon_events[code&0x0fff];
      }

      if ( (code&0xf000) == 0xc000 ) { // Vendor Opcodes

	    ostringstream tmp;
	    tmp << "Vendor[" << hex << vend << "]-" << hex << code << ends;
	    return tmp.str();
      }

      ostringstream tmp;
      tmp << "Invalid-" << hex << code << ends;
      return tmp.str();
}

string ptp_property_string(uint16_t code, uint32_t vend)
{
      if ( (code&0xf000) == 0x5000 ) { // PTP Standard Properties

	    if ( (code&0x0fff) <= 0x1f )
		  return ptp_standard_properties[code&0x0fff];

	    ostringstream tmp;
	    tmp << "Reserved-" << hex << code << ends;
	    return tmp.str();
      }

      if ( (code&0xf000) == 0xd000 && vend==0x0000000b
	   && (code&0x0fff) < array_count(ptp_vendor_canon_properties)) {
	    return ptp_vendor_canon_properties[code&0x0fff];
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

string ptp_property_uint8_string(uint16_t code, uint8_t val, uint32_t vend)
{
      ostringstream tmp;
      tmp << "0x" << hex << val << ends;
      return tmp.str();
}

string ptp_property_uint16_string(uint16_t code, uint16_t val, uint32_t vend)
{
      key_pair<uint16_t,uint16_t> use_key;
      use_key.key1 = code;
      use_key.key2 = val;

      key_value<key_pair<uint16_t,uint16_t>,string>*table = 0;
      size_t table_size = 0;

      if (code == 0x5007) { // FNumber
	    ostringstream tmp;
	    tmp << "f/" << (val/100.0);
	    return tmp.str();
      }

      if (code == 0x500f) { // ExposureIndex
	    if (val == 0xffff) return "Auto ISO";
	    ostringstream tmp;
	    tmp << "ISO " << val;
	    return tmp.str();
      }

      if ((val & 0x8000) == 0) {
	    table = ptp_standard_property16_values;
	    table_size = array_count(ptp_standard_property16_values);
	    size_t key = binary_search(use_key, table, table_size);

	    if (key < table_size) {
		  return table[key].value;

	    } else {
		  ostringstream tmp;
		  tmp << "Reserved-" << hex << val << ends;
		  return tmp.str();
	    }
      }

      switch (vend) {
	  case 0x0a: // Nikon
	    table = ptp_nikon_property16_values;
	    table_size = array_count(ptp_nikon_property16_values);
	    break;
	  default:
	    table = 0;
	    table_size = 0;
	    break;
      }

      size_t key = table? binary_search(use_key, table, table_size) : 0;

      if (key < table_size) {
	    return table[key].value;

      } else {
	    ostringstream tmp;
	    tmp << "Vendor[" << hex << vend << "]-" << hex << val << ends;
	    return tmp.str();
      }
}

string ptp_property_uint32_string(uint16_t code, uint32_t val, uint32_t vend)
{
      key_pair<uint16_t,uint32_t> use_key;
      use_key.key1 = code;
      use_key.key2 = val;

      key_value<key_pair<uint16_t,uint32_t>,string>*table = 0;
      size_t table_size = 0;

      if (code == 0x500d && val==0xffffffff) { // ExposureTime
	    return "Bulb";
      }

      if (code == 0x500d) { // ExposureTime
	      // Exposure time is defined by PTP to be in units of 0.1ms.
	    ostringstream tmp;
	    tmp << (val/10.0) << " ms" << ends;
	    return tmp.str();
      }

      if ((code & 0x8000) == 0) {
	    table = ptp_standard_property32_values;
	    table_size = array_count(ptp_standard_property32_values);
	    size_t key = binary_search(use_key, table, table_size);

	    if (key < table_size) {
		  return table[key].value;

	    } else {
		  ostringstream tmp;
		  tmp << "Reserved-" << hex << val << ends;
		  return tmp.str();
	    }
      }

      switch (vend) {
	  case 0x0a: // Nikon
	    table = ptp_nikon_property32_values;
	    table_size = array_count(ptp_nikon_property32_values);
	    break;
	  default:
	    table = 0;
	    table_size = 0;
	    break;
      }

      size_t key = table? binary_search(use_key, table, table_size) : 0;

      if (key < table_size) {
	    return table[key].value;

      } else {
	    ostringstream tmp;
	    tmp << "Vendor[" << hex << vend << "]-" << hex << val << ends;
	    return tmp.str();
      }
}
