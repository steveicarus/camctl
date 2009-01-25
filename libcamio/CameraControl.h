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
# include  <string>
# include  <utility>
# include  <ostream>
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

      virtual int open_session(void);
      virtual int close_session(void);

	// Return the battery level 0-100, or -1 if unavailable or N/A
      virtual float battery_level(void) const;
	// Program mode, or "" if N/A
      virtual std::string exposure_program_mode(void) const;

	// The exposure time is the time that the sensor is exposed to
	// light during a capture. The units used by this method are
	// micro-seconds, which the derived class converts to whatever
	// the device suports. The exposure setting has an available
	// range, which include a minimum value, a maximum value, and
	// a set increment that the device can support. If step==0,
	// then the exposure time cannot be controlled by the user.
      virtual void    get_exposure_time(int32_t&min,int32_t&max,int32_t&step);
      virtual int32_t get_exposure_time();
      virtual void    set_exposure_time(int32_t);

      virtual int    get_aperture();       // aka f-number
      virtual int    get_exposure_index(); // aka ISO

      virtual void set_aperture(int);
      virtual void set_exposure_index(int);

    public: // Camera control methods.

	// Capture an image.
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

    public: // Debug helper methods

	// Debug dump informatin about this camera.
      virtual void debug_dump(std::ostream&, const std::string&) const;

      virtual int debug_property_get(unsigned prop, unsigned dtype,
				     unsigned long&value);
      virtual int debug_property_set(unsigned prop, unsigned dtype,
				     unsigned long value);

    protected:
	// The derived class implements this method to fill in the
	// image file list. The method is passed a reference to the
	// list to fill with the files of the image. The CameraControl
	// class will manage the lifetime of that list.
      virtual void scan_images(std::list<file_key_t>&) =0;

    private:
      std::list<file_key_t> image_list_;

    private: // Not implemented
      CameraControl(const CameraControl&);
      CameraControl& operator= (const CameraControl&);
};

#endif
