#ifndef __PTPCamera_H
#define __PTPCamera_H
/*
 * Copyright (c) 2009 Stephen Williams (steve@icarus.com)
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

# include  <inttypes.h>
# include  <vector>
# include  <map>
# include  <ostream>
# include  <QString>

class QTreeWidgetItem;

class PTP_INT128_t {
    public:

      bool operator == (const PTP_INT128_t&that) const
      { return h_ == that.h_ && l_ == that.l_; }

    private:
      uint64_t h_, l_;
};

class PTP_UINT128_t {
    public:

      bool operator == (const PTP_UINT128_t&that) const
      { return h_ == that.h_ && l_ == that.l_; }

    private:
      uint64_t h_, l_;
};

class PTPCamera {

    public:
      enum type_code_t { TYPE_NONE   = 0x0000,
			 TYPE_INT8   = 0x0001,
			 TYPE_UINT8  = 0x0002,
			 TYPE_INT16  = 0x0003,
			 TYPE_UINT16 = 0x0004,
			 TYPE_INT32  = 0x0005,
			 TYPE_UINT32 = 0x0006,
			 TYPE_INT64  = 0x0007,
			 TYPE_UINT64 = 0x0008,
			 TYPE_INT128 = 0x0009,
			 TYPE_UINT128= 0x000a,
			 TYPE_STRING = 0xffff };

      enum type_form_t { FORM_NONE = 0, FORM_RANGE = 1, FORM_ENUM = 2 };

      enum prop_code_t { PROP_NONE  = 0x0000,
			 PROP_ImageSize = 0x5003,
                         PROP_WhiteBalance = 0x5005,
			 PROP_FNumber = 0x5007,
			 PROP_FocusMode = 0x500a,
			 PROP_FlashMode = 0x500c,
			 PROP_ExposureTime = 0x500d,
			 PROP_ExposureProgramMode = 0x500e,
			 PROP_ExposureIndex = 0x500f };

	// PTP defines some collection of property value types. This
	// class organizes (and unifies) the handling of PTP typed values.
      class prop_value_t {
	  public:
	    prop_value_t();
	    ~prop_value_t();

	    prop_value_t(const prop_value_t&);
	    prop_value_t& operator= (const prop_value_t&);

	    bool operator == (const prop_value_t&that) const;
	    bool operator != (const prop_value_t&that) const { return ! (*this == that); }

	      // Clear the value to a nil.
	    void clear();

	    void set_int8  (int8_t);
	    void set_uint8 (uint8_t);
	    void set_int16 (int16_t);
	    void set_uint16(uint16_t);
	    void set_int32 (int32_t);
	    void set_uint32(uint32_t);
	    void set_string(const QString&);

	      // Get the last set type code.
	    type_code_t get_type() const { return type_code_; }

	    int8_t   get_int8  () const;
	    uint8_t  get_uint8 () const;
	    int16_t  get_int16 () const;
	    uint16_t get_uint16() const;
	    int32_t  get_int32 () const;
	    uint32_t get_uint32() const;
	    int64_t  get_int64 () const;
	    uint64_t get_uint64() const;
	    PTP_INT128_t  get_int128 () const;
	    PTP_UINT128_t get_uint128() const;
	    QString  get_string() const;

	  private:
	    PTPCamera::type_code_t type_code_ :16;
	    union {
		  int8_t val_int8_;
		  uint8_t val_uint8_;
		  int16_t val_int16_;
		  uint16_t val_uint16_;
		  int32_t val_int32_;
		  uint32_t val_uint32_;
		  int64_t val_int64_;
		  uint64_t val_uint64_;
		  PTP_INT128_t val_int128_;
		  PTP_UINT128_t val_uint128_;
		  QString* val_string_;
	    };

	    void copy_(const prop_value_t&that);
      };

      static const prop_value_t prop_value_nil;

	// Return a pretty string for the given result code.
      static QString qresult_code(uint32_t result_code);

    public:
      PTPCamera();
      virtual ~PTPCamera() =0;

	// Actively get device information from the camera by sending
	// a command to the device and processing the response.
      uint32_t ptp_get_device_info(void);

	// Version of the PTP standard that the device supports.
      uint16_t ptp_standard_version() const;
	// Description of the vendor extensions that the device supports.
      uint32_t ptp_extension_vendor(bool raw_id =false) const;
      uint16_t ptp_extension_version() const;
      QString  ptp_extension_desc() const; 

	// Return TRUE if the operation code is supported.
      bool ptp_operation_is_supported(uint16_t code) const;

	// Some standard PTP properties
      uint16_t ptp_FunctionalMode() const;
      QString ptp_Manufacturer() const;
      QString ptp_Model() const;
      QString ptp_DeviceVersion() const;
      QString ptp_SerialNumber() const;

	// Get the battery level, and process it so that it is a
	// floating value from 0-100.0.
      float ptp_battery_level();

	// Some standard PTP operatins. These will return true if the
	// operation is supported, and will set rc to the result code
	// from the device. If the operation is not supported, return
	// false, and the result code is untouched.
      bool ptp_InitiateCapture(uint32_t&rc);

	// Map a PTP code to a descriptive string. The PTP code may be
	// any of a variety of codes, and the QString is a human
	// readable description.
      typedef std::pair<uint16_t,QString> code_string_t;

	// Return a list of supported operations, by name. (This is
	// used for debugging or display purposes.)
      std::vector<QString> ptp_operations_list() const;
      std::vector<QString> ptp_events_list() const;
      std::vector<code_string_t> ptp_properties_list() const;
      std::vector<QString> ptp_capture_formats_list() const;
      std::vector<QString> ptp_image_formats_list() const;

	// Probe the device to get a description for the
	// property. This will use the GetDevicePropDesc command to
	// get all the property details, and refresh the current and
	// factory values.
      void ptp_probe_property(unsigned prop_code, uint32_t&rc);

	// Get the property for the type.
      type_code_t ptp_get_property_type(unsigned prop_code) const;
	// Return true if the property can be set.
      bool ptp_get_property_is_setable(unsigned prop_code) const;
	// Get the form of the property (ENUM or RANGE) or FORM_NONE
	// if the property is not present.
      type_form_t ptp_get_property_form(unsigned prop_code) const;

	// Get the ordered list of enumerated property values, and
	// return the current value as an index. Return -1 if this is
	// not an enumerated type.
      struct labeled_value_t { QString label; prop_value_t value; };
      int ptp_get_property_enum(unsigned prop_code, std::vector<labeled_value_t>&table) const;
	// Get the range for the type and return true. If this is not
	// a range type, then return false.
      bool ptp_get_property_range(unsigned prop_code, prop_value_t&min, prop_value_t&max, prop_value_t&step) const;

	// Get the property current or factory default values. The
	// values are cached from the last ptp_probe_property.
      const prop_value_t& ptp_get_property_current(unsigned prop_code) const;
      const prop_value_t& ptp_get_property_factory(unsigned prop_code) const;

	// Get the property directly from the camera, and save the
	// result as the current value for the property. This command
	// goes directly to the camera.
      const prop_value_t& ptp_get_property(unsigned prop_code, uint32_t&rc);

	// Set the property. The type of the property must match the
	// type that the camera expects.
      void ptp_set_property(unsigned prop_code, const prop_value_t&val, uint32_t&rc);

      QTreeWidgetItem*ptp_describe_camera(void);

    protected:
	// The derived class implements this method to pass commands
	// (and responses) between this instance and the actual device.
      virtual uint32_t ptp_command(uint16_t command,
				   const std::vector<uint32_t>&parms,
				   const unsigned char*send, size_t nsend,
				   unsigned char*recv, size_t nrecv) =0;

    private:
      uint16_t standard_version_;
      uint32_t vendor_extension_id_;
      uint16_t vendor_extension_vers_;
      QString  vendor_extension_desc_;
      uint16_t functional_mode_;
      std::vector<uint16_t> operations_supported_;
      std::vector<uint16_t> events_supported_;
      std::vector<uint16_t> capture_formats_;
      std::vector<uint16_t> image_formats_;
      QString ptp_manufacturer_;
      QString ptp_model_;
      QString device_version_;
      QString serial_number_;


      struct prop_info_t {
	    PTPCamera::type_code_t type_code : 16;
	    uint8_t  get_set_flag;
	      // 1: RANGE, 2: ENUM
	    uint8_t  form_flag;
	      // Describe the factory default value for this property
	    prop_value_t factory;
	      // The current value for this value
	    prop_value_t current;
	      // The range is the list of possible values if this is
	      // an enum, or is the range in 3 values: min, max, step.
	    std::vector<prop_value_t> range;
      };

	// This map lists the properties that a device supports, along
	// with the descriptions for the value.
      std::map<uint16_t, prop_info_t> device_properties_supported_;

      static std::map<uint16_t,type_code_t> ptp_type_to_type_code_;

	// Get (raw) the current value of the given property directly
	// from the camera.
      uint8_t  ptp_get_property_uint8_(unsigned prop_code, uint32_t&rc);
      uint16_t ptp_get_property_uint16_(unsigned prop_code, uint32_t&rc);
      uint32_t ptp_get_property_uint32_(unsigned prop_code, uint32_t&rc);
      int8_t  ptp_get_property_int8_(unsigned prop_code, uint32_t&rc);
      int16_t ptp_get_property_int16_(unsigned prop_code, uint32_t&rc);
      int32_t ptp_get_property_int32_(unsigned prop_code, uint32_t&rc);
      QString ptp_get_property_string_(unsigned prop_code, uint32_t&rc);

	// Set the value to the given property.
      void ptp_set_property_uint8_(unsigned prop_code, uint8_t val, uint32_t&rc);
      void ptp_set_property_uint16_(unsigned prop_code, uint16_t val, uint32_t&rc);
      void ptp_set_property_uint32_(unsigned prop_code, uint32_t val, uint32_t&rc);
      void ptp_set_property_int8_(unsigned prop_code, int8_t val, uint32_t&rc);
      void ptp_set_property_int16_(unsigned prop_code, int16_t val, uint32_t&rc);
      void ptp_set_property_int32_(unsigned prop_code, int32_t val, uint32_t&rc);
      void ptp_set_property_string_(unsigned prop_code, const QString&val, uint32_t&rc);


    private: // Not implemented
      PTPCamera(const PTPCamera&);
      PTPCamera& operator= (const PTPCamera&);
};

extern std::ostream& operator << (std::ostream&out, const PTPCamera::prop_value_t&val);

#endif
