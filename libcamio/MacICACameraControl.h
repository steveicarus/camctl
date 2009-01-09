#ifndef __MacICACameraControl_H
#define __MacICACameraControl_H
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

/*
 * The MacICACameraControl class implements camera control that uses
 * Mac OS X ImageCapture.
 */

# include  "CameraControl.h"
# include  <ImageCapture/ICAApplication.h>

class MacICACameraControl : public CameraControl {

    public:
	// Inventory this kind of object.
      static void camera_inventory(void);

    private:
      MacICACameraControl(ICAObject dev);
    public:
      ~MacICACameraControl();

      std::string control_class(void) const;
      std::string camera_make(void) const;
      std::string camera_model(void) const;
      int battery_level(void) const;
      std::string exposure_program_mode(void) const;

    public:
      void capture_image(void);
      void get_image_data(long key, char*&buf, size_t&buf_len);

      void debug_dump(std::ostream&, const std::string&) const;

    private:
      ICAObject dev_;
      CFDictionaryRef dev_dict_;
      CFDictionaryRef dev_prop_dict_;

      usb_name_t make_model_;

    private:
      void scan_images(std::list<file_key_t>&);

      void debug_dump_default_(std::ostream&) const;
      void debug_dump_capabilities_(std::ostream&) const;
      void debug_dump_data_(std::ostream&) const;
      void debug_dump_device_(std::ostream&) const;
};

#endif
