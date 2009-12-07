
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

# include  "MacICACameraControl.h"
# import  <Cocoa/Cocoa.h>
# import  <Foundation/Foundation.h>
# include  <assert.h>

static NSString*data_key = (NSString*) CFSTR("data");
static NSString*icao_key = (NSString*) CFSTR("icao");
static NSString*ifil_key = (NSString*) CFSTR("ifil");

static NSString*idVendor_key  = (NSString*) CFSTR("idVendor");
static NSString*idProduct_key = (NSString*) CFSTR("idProduct");

// Convenience function to convert an NSString obj-c object to a
// std::string C++ object.
static std::string to_string(NSString*src)
{
      const char*str = [src cStringUsingEncoding:NSUTF8StringEncoding];
      std::string res_str = str;
      return res_str;
}

CameraControl::usb_id_t MacICACameraControl::get_usb_id_from_dict_(CFDictionaryRef dict)
{
      NSDictionary*use_dict = (NSDictionary*)dict;

      NSNumber*idVendor = [use_dict objectForKey: idVendor_key];
      assert(idVendor && [idVendor isKindOfClass:[NSNumber class]]);

      NSNumber*idProduct = [use_dict objectForKey: idProduct_key];
      assert(idProduct && [idProduct isKindOfClass:[NSNumber class]]);

      return usb_id_t([idVendor longValue],[idProduct longValue]);
}

/*
 * Scan the images in the "data" key of the dev_dict_ dictionary. This
 * dictionary was retrieved from the camera, so should reflect the
 * current state of the camera.
 */
void MacICACameraControl::scan_images(std::list<file_key_t>&dst)
{
      NSDictionary*use_dict = (NSDictionary*)dev_dict_;

	// The "data" key of the dev_dict_ holds an array of the image
	// items in the camera. Each image item in the array is a
	// dictionary that describes the image.
      NSArray*data_array = [use_dict objectForKey: data_key];
      assert([data_array isKindOfClass:[NSArray class]]);

      for (NSUInteger idx = 0 ; idx < [data_array count] ; idx += 1) {
	    NSDictionary*cur = [data_array objectAtIndex:idx];
	    assert(cur && [cur isKindOfClass:[NSDictionary class]]);

	      // The "ifil" key of the image item contains the file name.
	    NSString*nam = [cur objectForKey: ifil_key];
	    assert(nam && [nam isKindOfClass:[NSString class]]);

	    std::string nam_str = to_string(nam);

	    dst.push_back( file_key_t(idx,nam_str) );
      }
}

/*
 * Get the image item (the ICAObject key) from the device dictionary
 * using the integer key.
 */
ICAObject MacICACameraControl::ica_image_object_from_dev_(long key)
{
      NSDictionary*use_dict = (NSDictionary*)dev_dict_;

	// The images are listed as an array in the dev
	// dictionary. Get a ref to that array, and make sure the key fits.
      NSArray*aref = [use_dict objectForKey:data_key];
      assert(aref && [aref isKindOfClass:[NSArray class]]);

      if ((unsigned)key >= [aref count])
	    return 0;

	// The file we are after is itself another dictionary with the
	// "data" array.
      NSDictionary*cur = [aref objectAtIndex:key];
      assert(cur && [cur isKindOfClass:[NSDictionary class]]);

	// The ICAObject key ("icao") gets for us the image object. It
	// is a NSNumber that I can convert into an ICAObject.
      NSNumber*icao = [cur objectForKey:icao_key];
      assert(icao && [icao isKindOfClass:[NSNumber class]]);

      ICAObject image = [icao longValue];

      return image;
}
