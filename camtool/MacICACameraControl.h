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

      virtual void get_image_size_index(std::vector<std::string>&values);
      virtual int  get_image_size_index();
      virtual void set_image_size_index(int);
      virtual bool set_image_size_ok();

      virtual void get_exposure_program_index(std::vector<std::string>&values);
      virtual int  get_exposure_program_index();
      virtual void set_exposure_program_index(int);
      virtual bool set_exposure_program_ok();

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

      virtual void get_white_balance_index(std::vector<std::string>&values);
      virtual int  get_white_balance_index();
      virtual void set_white_balance_index(int);
      virtual bool set_white_balance_ok();

    public:
	// Debug aids
      virtual int debug_property_get(unsigned prop, unsigned dtype,
				     unsigned long&value);
      virtual int debug_property_set(unsigned prop, unsigned dtype,
				     unsigned long value);
      virtual std::string debug_property_describe(unsigned prop);

    private:
	// Implement this for the PTPCamera base class.
      uint32_t ptp_command(uint16_t command,
			   const std::vector<uint32_t>&parms,
			   const unsigned char*send, size_t nsend,
			   unsigned char*recv, size_t nrecv);

    private:
	// The PTP standard defines a canonical way to describe
	// properties. The prop_desc_t class encapsulates those
	// descriptions and allows me to work with them in a more
	// donvenient form. The ptp_get_property_desc_() method is a
	// convenient way to refresh a property description from the
	// device.
      class prop_desc_t {
	  public:
	    prop_desc_t(uint16_t prop_code);
	    ~prop_desc_t();

	    uint16_t get_property_code() const { return prop_code_; }

	      // This method causes the property to be marked as
	      // unsupported by the device.
	    void clear_property_support();

	      // Return if this property is supported.
	    bool test_property_support() const { return support_flag_; }

	      // Set the PTP defined type code.
	    void set_type_code(uint16_t val);
	    uint16_t get_type_code() const { return type_code_; }

	      // Set the flag indicating whether this property is can
	      // be set. TRUE means it can be set, and FALSE means not.
	    void set_flag(bool flag);
	    bool set_ok(void) const { return support_flag_ && set_flag_; }

	      // properties generally have a factory default value.
	      //template <class T> T get_factory_default();

	      // Get the current value and the number for the enum. If
	      // the current value is not in the enum, or this is not
	      // and enum, then return -1;
	    template <class T> T get_current(void);

	      // Properties may come as enum values. In that case, the
	      // get_enum_count returns a value >0 that is the number
	      // of enum values that the property supports. The
	      // get_enum_index() gets specific enum values.
	    int get_enum_count() const;
	    template <class T> T get_enum_index(int idx);

	    bool is_range() const { return range_flag_; }
	    template <class T> void get_range(T&val_min, T&val_max, T&step);

	  public:
	      // These are some methods for filling in the property description.
	    template <class T> void set_factory_default(T val);
	    template <class T> void set_current(T val);

	    template <class T> void set_enum_vector(const std::vector<T>&vec);
	    template <class T> void set_range(T val_min, T val_max, T step);

	  private:
	    uint16_t prop_code_;
	    uint16_t type_code_;
	    bool support_flag_;
	    bool range_flag_;
	    bool set_flag_;
	    union {
		  int8_t fact_int8_;
		  uint8_t fact_uint8_;
		  int16_t fact_int16_;
		  uint16_t fact_uint16_;
		  int32_t fact_int32_;
		  uint32_t fact_uint32_;
		  QString* fact_string_;
	    };

	    union {
		  int8_t current_int8_;
		  uint8_t current_uint8_;
		  int16_t current_int16_;
		  uint16_t current_uint16_;
		  int32_t current_int32_;
		  uint32_t current_uint32_;
		  QString* current_string_;
	    };

	    union {
		  std::vector<int8_t>* enum_int8_;
		  std::vector<uint8_t>* enum_uint8_;
		  std::vector<int16_t>* enum_int16_;
		  std::vector<uint16_t>* enum_uint16_;
		  std::vector<int32_t>* enum_int32_;
		  std::vector<uint32_t>* enum_uint32_;
		  std::vector<QString>* enum_string_;
	    };
      };
      void ptp_get_property_desc_(prop_desc_t&, uint32_t&result_code);

    private:
	// Standard camera properties
      prop_desc_t battery_level_;
      prop_desc_t image_size_;
      prop_desc_t white_balance_;
      prop_desc_t exposure_program_;
      prop_desc_t exposure_time_;
      prop_desc_t fnumber_;
      prop_desc_t iso_;
	// Flash properties
      prop_desc_t flash_mode_;
	// Focus properties
      prop_desc_t focus_mode_;
};

#endif
