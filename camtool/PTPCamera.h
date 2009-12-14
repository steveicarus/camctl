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
# include  <QString>

class PTPCamera {

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

	// Return a list of supported operations, by name. (This is
	// used for debugging or display purposes.)
      std::vector<QString> ptp_operations_list() const;
      std::vector<QString> ptp_events_list() const;
      std::vector<QString> ptp_properties_list() const;
      std::vector<QString> ptp_capture_formats_list() const;
      std::vector<QString> ptp_image_formats_list() const;

      uint8_t  ptp_get_property_u8(unsigned prop_code, uint32_t&rc);
      uint16_t ptp_get_property_u16(unsigned prop_code, uint32_t&rc);
      uint32_t ptp_get_property_u32(unsigned prop_code, uint32_t&rc);
      QString ptp_get_property_string(unsigned prop_code, uint32_t&rc);

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
      std::vector<uint16_t> device_properties_supported_;
      std::vector<uint16_t> capture_formats_;
      std::vector<uint16_t> image_formats_;
      QString ptp_manufacturer_;
      QString ptp_model_;
      QString device_version_;
      QString serial_number_;

    private: // Not implemented
      PTPCamera(const PTPCamera&);
      PTPCamera& operator= (const PTPCamera&);
};

#endif
