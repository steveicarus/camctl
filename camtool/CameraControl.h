#ifndef __CameraControl_H
#define __CameraControl_H
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

# include  <list>
# include  <vector>
# include  <map>
# include  <string>
# include  <utility>
# include  <QByteArray>
# include  <QString>
# include  <ostream>
# include  <fstream>
# include  <inttypes.h>
class QTreeWidgetItem;

class CameraControl {

    public:
	// This static function scans the system for all the cameras
	// that are available. It calls the equivilent static methods
	// in the known specific classes to locate the camera devices
	// and load them into the camera_list below.
      static void camera_inventory();

	// Use this as a debug log for directing debug output from the
	// entire library.
      static std::ofstream debug_log;

    public:
      CameraControl();
      virtual ~CameraControl() =0;

    public: // Methods to get at common properties of the camera and
	    // the camera port.

	// This method makes a QTreeWidgetItem that describes the
	// device, in as much detail as is reasonably available. This
	// may include data that is available individually by the
	// other methods, and it may include details specific to a
	// derived class.
      virtual QTreeWidgetItem*describe_camera();

	// This is a short string that the derived class uses to
	// identify itself. This is for identifying the class
	// (i.e. Image Capture Architecture, USB, etc.) and not the
	// camera itself.
      virtual std::string control_class(void) const =0;

	// Information about the camera. Return the make/model strings
	// for the camera.
      virtual std::string camera_make(void) const;
      virtual std::string camera_model(void) const;

	// The open_session() method needs to be called to start a
	// session with this camera. The various camera controls
	// unless the session is opened. Close the session when done.
      virtual int open_session(void);
      virtual int close_session(void);

	// The client calls this periodically to check on the camera.
      virtual void heartbeat();

	// Return the battery level 0-100, or -1 if unavailable or N/A
      virtual float battery_level(void);

      virtual int get_image_size_index(std::vector<QString>&values);
      virtual void set_image_size_index(int);
      virtual bool set_image_size_ok();

      virtual int get_exposure_program_index(std::vector<QString>&values);
      virtual void set_exposure_program_index(int);
      virtual bool set_exposure_program_ok();

	// The exposure time is the time that the sensor is exposed to
	// light during a capture. The device knows what settings it
	// is capable of supporting, so first call the
	// get_exposure_time_index with a values array. The derived
	// class will fill in the array with display strings for the
	// supported settings. The other set_ and
	// set_exposure_time_index methods work with indices into that
	// vector.
      virtual int get_exposure_time_index(std::vector<QString>&values);
      virtual void set_exposure_time_index(int);
      virtual bool set_exposure_time_ok();

      virtual int get_fnumber_index(std::vector<QString>&values);
      virtual void set_fnumber_index(int);
      virtual bool set_fnumber_ok();

      virtual int get_iso_index(std::vector<QString>&values);
      virtual void set_iso_index(int);
      virtual bool set_iso_ok();

      virtual int get_flash_mode_index(std::vector<QString>&values);
      virtual void set_flash_mode_index(int);
      virtual bool set_flash_mode_ok();

      virtual int get_focus_mode_index(std::vector<QString>&values);
      virtual void set_focus_mode_index(int);
      virtual bool set_focus_mode_ok();

      virtual int get_white_balance_index(std::vector<QString>&values);
      virtual void set_white_balance_index(int);
      virtual bool set_white_balance_ok();

    public: // Camera control methods.

	// Capture an image. This activates the shutter and captures
	// the image using the current settings. The image is left on
	// the camera. The capture_volatile_image() method is similar,
	// except that it tries to get the camera to capture the image
	// into volatile memory instead of flash. If the derived class
	// doesn't implement this method, then resort to the
	// capture_image() method instead.
      enum capture_resp_t {
	    CAP_OK,
	    CAP_NOT_SUPPORTED,
	    CAP_ERROR
      };
      virtual capture_resp_t capture_image() =0;
      virtual capture_resp_t capture_volatile_image();

    public: // Image file manipulation methods.

	// Get a list of images that are known on the camera. The
	// result reference is not certain to be valid after a later
	// call of the image_list method. The reference also will not
	// reflect changes made on the camera until a new call to the
	// image_list method refreshes the list.
      typedef std::pair<long,std::string> file_key_t;
      const std::list<file_key_t>& image_list();

	// Get the image data into a dynamically allocated buffer. The
	// caller passes in the key for the desired image. The method
	// fills in the buf pointer and the buffer len. The method
	// allocates the memory of the buffer, it is up to the caller
	// to free the buffer when it is done.
      virtual void get_image_data(long key, QByteArray&data,
				  bool delete_image =false);
      virtual void get_image_thumbnail(long key, char*&buf, size_t&buf_len);


    public: // Notifications

	// Support some notifications from the camera device. The
	// Notificaion class is a base class for receiving
	// notifications from the device. The client derives from this
	// class types to handle specific needs. The
	// set_notification_* methods then provide a way to enable
	// desired notifications.
      class Notification {
	  public:
	    Notification();
	    virtual ~Notification() =0;
	    virtual void camera_image_added(CameraControl*, const file_key_t&file);
	    virtual void camera_image_deleted(CameraControl*, const file_key_t&file);
	    virtual void camera_capture_complete(CameraControl*);
	    virtual void camera_added(CameraControl*);
	    virtual void camera_removed(CameraControl*);
      };

	// Notify when images are added. Remove the notification by
	// passing a 0 pointer.
      void set_image_added_notification(Notification*);
      void set_image_deleted_notification(Notification*);

	// Notify when a capture is complete
      void set_capture_complete_notification(Notification*);

	// Notify me when a new camera device is added. This method is
	// static because it is about a new camera, and not the
	// existing camera.
      static void set_camera_added_notification(Notification*);

	// Notify when this camera is removed.
      void set_camera_removed_notification(Notification*);

    protected:
	// These are used by derived classes to announce events that
	// trigger notifications.
      void mark_image_notification();
      void mark_capture_complete();
      static void mark_camera_added(CameraControl*);
      static void mark_camera_removed(CameraControl*);
    private:
      void mark_image_added_(const file_key_t&new_file);
      void mark_image_deleted_(const file_key_t&old_file);

    public: // Debug helper methods

	// Debug dump informatin about this camera.
      virtual void debug_dump(std::ostream&, const std::string&) const;

    protected:
	// The derived class implements this method to fill in the
	// image file list. The method is passed a reference to the
	// list to fill with the files of the image. The CameraControl
	// class will manage the lifetime of that list.
      virtual void scan_images(std::list<file_key_t>&) =0;

    private:
      std::list<file_key_t> image_list_;
      Notification*image_added_notification_;
      Notification*image_deleted_notification_;
      Notification*capture_complete_notification_;
      static Notification*added_notification_;
      Notification*removed_notification_;

	// Support for device inventories. These are generic tables
	// and types that derived camera control platforms use.
    protected:
	// A USB id is a Vendor/Device pair
      typedef std::pair<uint16_t,uint16_t> usb_id_t;
	// A Device name is a Vendor/Device string pair
      typedef std::pair<std::string,std::string> dev_name_t;
	// A device class is the derived class that supports this device.
      enum dev_class_t {
	    MacGeneric = 0, // Supported by default ICA basics.
	    MacBlacklist,   // Positively not supported
	    MacPTP          // Supported through standard PTP
      };

      static void load_usb_map(void);
      static const dev_name_t&id_to_name(const usb_id_t&id);
      static const dev_class_t id_to_class(const usb_id_t&id);

    private:
	// The id_to_* functions use maps of device ids to the desired
	// type. These methods provide the infrastructure for mapping
	// device identification to the proper implementation.
      static std::map <usb_id_t,dev_name_t>  usb_map_names;
      static std::map <usb_id_t,dev_class_t> usb_map_classes;

      struct usb_devices_struct {
	    uint16_t vendor_id;
	    uint16_t device_id;
	    const char*vendor_name;
	    const char*device_name;
	    dev_class_t device_class;
      };
      static struct usb_devices_struct usb_devices_table[];


    private: // Not implemented
      CameraControl(const CameraControl&);
      CameraControl& operator= (const CameraControl&);
};

// Convenient manipulator to write a timestamp.
extern std::ostream& TIMESTAMP(std::ostream&);

#endif
