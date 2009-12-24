#ifndef __MacICACameraControl_H
#define __MacICACameraControl_H
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

/*
 * The MacICACameraControl class implements camera control that uses
 * Mac OS X ImageCapture. This is actually a category of interfaces
 * that use the Mac OS X ICA as a pass-through interface for advanced
 * controls.
 */

# include  "CameraControl.h"
# include  "PTPCamera.h"
# include  <ImageCapture/ICAApplication.h>
# include  <QString>
# include  <map>
# include  <vector>
# include  <string>
# include  <inttypes.h>

class MacICACameraControl : public CameraControl {

    public:
	// Inventory this kind of object.
      static void camera_inventory(void);

    protected:
      MacICACameraControl(ICAObject dev);
    public:
      ~MacICACameraControl();

    public:
      virtual QTreeWidgetItem*describe_camera();

      std::string control_class(void) const;
      std::string camera_make(void) const;

      int open_session(void);
      int close_session(void);

      std::string camera_model(void) const;

    public:
      capture_resp_t capture_image(void);
      void get_image_data(long key, QByteArray&data, bool delete_image);
      void get_image_thumbnail(long key, char*&buf, size_t&buf_len);

      void debug_dump(std::ostream&, const std::string&) const;

    protected:
      static usb_id_t get_usb_id_from_dict_(CFDictionaryRef);


      ICAError ica_send_message_(void*buf, size_t buf_len);

    private:
      ICAObject dev_;
      CFDictionaryRef dev_dict_;

      ICASessionID session_id_;

      dev_name_t make_model_;

    private:
      void refresh_dev_dict_(void);

      static void register_for_events(void);
      static void ica_notification(CFStringRef notification_type,
				   CFDictionaryRef notification_dict);
      static MacICACameraControl* notification_camera_;

    protected:
      static void scan_devices_(void);

    private:
      void scan_images(std::list<file_key_t>&);
      ICAObject ica_image_object_from_dev_(long key);

      void debug_dump_default_(std::ostream&) const;
      static std::ostream& dump_value(std::ostream&out, CFTypeRef ref);
};

/*
 * This class is derived from MacICACameraControl and is for handling
 * cameras that are known to not have capture capabilities.
 */
class MacICABlacklist : public MacICACameraControl {

    private:
	// The camera_inventory function from MacICACameraControl
	// actually creates objects of this type.
      friend void MacICACameraControl::scan_devices_(void);
      MacICABlacklist(ICAObject dev);

    public:
      ~MacICABlacklist();

    public:
      capture_resp_t capture_image(void);

};

/*
 * This class is derived from MacICACameraControl to provide a camera
 * control for PTP cameras. PTP Cameras use MacICA as the interface,
 * but also uses PTP commands that are passed through via ICA
 * functions.
 */
class MacPTPCameraControl  : public PTPCamera, public MacICACameraControl {

    private:
	// The camera_inventory function from MacICACameraControl
	// actually creates objects of this type.
      friend void MacICACameraControl::scan_devices_(void);
      MacPTPCameraControl(ICAObject dev);

    public:
      ~MacPTPCameraControl();

    public:
      virtual QTreeWidgetItem*describe_camera();

      capture_resp_t capture_image(void);

      virtual float battery_level(void);

      virtual int get_image_size_index(std::vector<QString>&values);
      virtual void set_image_size_index(int);
      virtual bool set_image_size_ok();

      virtual int get_exposure_program_index(std::vector<QString>&values);
      virtual void set_exposure_program_index(int);
      virtual bool set_exposure_program_ok();

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

    private:
      int get_property_index_(unsigned prop_code,
			      std::vector<PTPCamera::labeled_value_t>&table,
			      std::vector<QString>&values);

      void set_property_index_(unsigned prop_code,
			       std::vector<PTPCamera::labeled_value_t>&table,
			       int use_index);

	// Implement this for the PTPCamera base class.
      uint32_t ptp_command(uint16_t command,
			   const std::vector<uint32_t>&parms,
			   const unsigned char*send, size_t nsend,
			   unsigned char*recv, size_t nrecv);

    private:
	// Standard camera properties
      std::vector<PTPCamera::labeled_value_t> image_size_;
      std::vector<PTPCamera::labeled_value_t> white_balance_;
      std::vector<PTPCamera::labeled_value_t> exposure_program_;
      std::vector<PTPCamera::labeled_value_t> exposure_time_;
      std::vector<PTPCamera::labeled_value_t> fnumber_;
      std::vector<PTPCamera::labeled_value_t> iso_;
	// Flash properties
      std::vector<PTPCamera::labeled_value_t> flash_mode_;
	// Focus properties
      std::vector<PTPCamera::labeled_value_t> focus_mode_;
};

#endif
