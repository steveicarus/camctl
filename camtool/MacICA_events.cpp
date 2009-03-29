
/*
 * Copyright (c) 2008 Stephen Williams (steve@icarus.com)
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
# include  <ImageCapture/ImageCapture.h>
# include  <iostream>
# include  <iomanip>
# include  <assert.h>

using namespace std;

MacICACameraControl*MacICACameraControl::notification_camera_ = 0;

void MacICACameraControl::register_for_events(void)
{
	// Enable some interesting notifications.
      CFMutableArrayRef events_array = CFArrayCreateMutable(0, 16, 0);
      CFArrayAppendValue(events_array, kICANotificationTypeCaptureComplete);
      CFArrayAppendValue(events_array, kICANotificationTypeDeviceAdded);
      CFArrayAppendValue(events_array, kICANotificationTypeDeviceConnectionProgress);
      CFArrayAppendValue(events_array, kICANotificationTypeDeviceInfoChanged);
      CFArrayAppendValue(events_array, kICANotificationTypeDevicePropertyChanged);
      CFArrayAppendValue(events_array, kICANotificationTypeDeviceRemoved);
      CFArrayAppendValue(events_array, kICANotificationTypeDeviceWasReset);
      CFArrayAppendValue(events_array, kICANotificationTypeObjectAdded);
      CFArrayAppendValue(events_array, kICANotificationTypeObjectInfoChanged);
      CFArrayAppendValue(events_array, kICANotificationTypeObjectRemoved);
      CFArrayAppendValue(events_array, kICANotificationTypeProprietary);
      CFArrayAppendValue(events_array, kICANotificationTypeRequestObjectTransfer);
      CFArrayAppendValue(events_array, kICANotificationTypeTransactionCanceled);
      CFArrayAppendValue(events_array, kICANotificationTypeUnreportedStatus);

      ICARegisterForEventNotificationPB register_pb;
      memset(&register_pb, 0, sizeof register_pb);
      register_pb.objectOfInterest = 0;;
      register_pb.eventsOfInterest = events_array;
      register_pb.notificationProc = ica_notification;
      register_pb.options = 0;
      ICARegisterForEventNotification(&register_pb, 0);

      CFRelease(events_array);
}

/*
 * This is the callback function for camera notifications.
 */
void MacICACameraControl::ica_notification(CFStringRef notification_type,
					   CFDictionaryRef notification_dict)
{
	// Start out by getting a description of the notification and
	// writing it to the debug log.
      char type_buf[1024];
      CFStringGetCString(notification_type, type_buf, sizeof type_buf,
			 kCFStringEncodingASCII);
      debug_log << TIMESTAMP << " **** ica_notification: type="
		<< type_buf << endl;
      dump_value(debug_log, notification_dict);
      debug_log << "****" << endl << flush;

	// What kind of notification?

      if (CFStringCompare(notification_type,kICANotificationTypeDeviceAdded,0) == kCFCompareEqualTo) {

	      // This is a DeviceAdded notification.
	    debug_log << "**** Device added - rescan devices. ****" << endl;
	    scan_devices_();

      } else if (CFStringCompare(notification_type,kICANotificationTypeDeviceRemoved,0) == kCFCompareEqualTo) {

	      // This is a DeviceRemoved notification
	    debug_log << "**** Device Removed ****" << endl;
	    scan_devices_();

      } else if (CFStringCompare(notification_type,kICANotificationTypeObjectAdded,0) == kCFCompareEqualTo) {

	      // This is an ObjectAdded notification. This usually
	      // means that an image file was added. Refresh the image
	      // list from the camera and debug the new files list.
	    debug_log << "**** Object added ****" << endl;

	    if (notification_camera_) {
		  notification_camera_->refresh_dev_dict_();
		  notification_camera_->mark_image_notification();
	    }

      } else if (CFStringCompare(notification_type,kICANotificationTypeObjectRemoved,0) == kCFCompareEqualTo) {

	      // This is an ObjectRemoved notification. This usually
	      // means that an image file was removed. Refresh the image
	      // list from the camera and debug the new files list.
	    debug_log << "**** Object removed ****" << endl;

	    if (notification_camera_) {
		  notification_camera_->refresh_dev_dict_();
		  notification_camera_->mark_image_notification();
	    }

      } else if (CFStringCompare(notification_type,kICANotificationTypeProprietary,0) == kCFCompareEqualTo) {

	      // Proprietary events may actually be standard events
	      // for a subordinate standard. (i.e. PTP)
	    CFStringRef class_key = (CFStringRef)CFDictionaryGetValue(notification_dict,
							 kICANotificationClassKey);
	    CFNumberRef raw_event = (CFNumberRef)CFDictionaryGetValue(notification_dict,
							 kICANotificationRawEventKey);

	    assert(class_key);
	    if (CFStringCompare(class_key,kICANotificationClassPTPStandard,0) == kCFCompareEqualTo) {
		    // This is a PTP event. Many interesting standard
		    // PTP events are not handled by the ICA itself,
		    // so we interpret them here.
		  int event_code;
		  assert(raw_event);
		  CFNumberGetValue(raw_event, kCFNumberIntType, &event_code);

		  debug_log << "PTP Event: " << hex << setw(4) << event_code << dec << endl;
		  switch (event_code) {
		      case 0x400d: // PTP Capture Complete
			notification_camera_->mark_capture_complete();
			break;
		      default:
			break;
		  }
	    }
      }

      debug_log << flush;
}

