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
# include  <string>
# include  <utility>
# include  <ostream>
# include  <fstream>
# include  <inttypes.h>

class CameraControl {

    public:
	// This static function scans the system for all the cameras
	// that are available. It calls the equivilent static methods
	// in the known specific classes to locate the camera devices
	// and load them into the camera_list below.
      static void camera_inventory();

	// This is a list of all the cameras discovered by the camera
	// inventory. The camera_inventory methods of the derived
	// classes fill in this list. The user may use this list to
	// display/select the camera to use.
      static std::list<CameraControl*>camera_list;

	// Use this as a debug log for directing debug output from the
	// entire library.
      static std::ofstream debug_log;

    public:
      CameraControl();
      virtual ~CameraControl() =0;

    public: // Methods to get at common properties of the camera and
	    // the camera port.

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

	// Return the battery level 0-100, or -1 if unavailable or N/A
      virtual float battery_level(void) const;

      virtual void get_exposure_program_index(std::vector<std::string>&values);
      virtual int  get_exposure_program_index();
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
      virtual void get_exposure_time_index(std::vector<std::string>&values);
      virtual int  get_exposure_time_index();
      virtual void set_exposure_time_index(int);
      virtual bool set_exposure_time_ok();

      virtual void get_fnumber_index(std::vector<std::string>&values);
      virtual int  get_fnumber_index();
      virtual void set_fnumber_index(int);
      virtual bool set_fnumber_ok();

      virtual void get_iso_index(std::vector<std::string>&values);
      virtual int  get_iso_index();
      virtual void set_iso_index(int);
      virtual bool set_iso_ok();

      virtual void get_flash_mode_index(std::vector<std::string>&values);
      virtual int  get_flash_mode_index();
      virtual void set_flash_mode_index(int);
      virtual bool set_flash_mode_ok();

      virtual void get_focus_mode_index(std::vector<std::string>&values);
      virtual int  get_focus_mode_index();
      virtual void set_focus_mode_index(int);
      virtual bool set_focus_mode_ok();

    public: // Camera control methods.

	// Capture an image. This activates the shutter and captures
	// the image using the current settings. The image is left on
	// the camera.
      virtual void capture_image() =0;

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
      virtual void get_image_data(long key, char*&buf, size_t&buf_len);
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
	    virtual void camera_images(CameraControl*);
      };

	// Notify when images are added. Remove the notification by
	// passing a 0 pointer.
      void set_image_notification(Notification*);

    protected:
	// These are used by derived classes to announce events that
	// trigger notifications.
      void mark_image_notification();

    public: // Debug helper methods

	// Debug dump informatin about this camera.
      virtual void debug_dump(std::ostream&, const std::string&) const;

      virtual int debug_property_get(unsigned prop, unsigned dtype,
				     unsigned long&value);
      virtual int debug_property_set(unsigned prop, unsigned dtype,
				     unsigned long value);

      virtual std::string debug_property_describe(unsigned prop);

    protected:
	// The derived class implements this method to fill in the
	// image file list. The method is passed a reference to the
	// list to fill with the files of the image. The CameraControl
	// class will manage the lifetime of that list.
      virtual void scan_images(std::list<file_key_t>&) =0;

    private:
      std::list<file_key_t> image_list_;
      Notification*images_notification_;

    private: // Not implemented
      CameraControl(const CameraControl&);
      CameraControl& operator= (const CameraControl&);
};

#endif
