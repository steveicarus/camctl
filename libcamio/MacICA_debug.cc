
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

/*
 * Little convenience function to get a string from a CFStringRef.
 */
static string get_string(CFStringRef val)
{
      char buf[128];
      CFIndex slen = CFStringGetLength(val);
      Boolean rc;
      rc = CFStringGetCString(val, buf, sizeof buf, kCFStringEncodingASCII);
      assert(rc);
      buf[slen] = 0;
      return buf;
}

/*
 * Given a CFTypeRef, briefly dump the referenced value. Numbers are
 * displayed as numbers, strings as quoted strings, etc.
 */
static ostream& dump_value(ostream&out, CFTypeRef ref)
{
      assert(ref);

      CFIndex ref_type = CFGetTypeID(ref);
      if (ref_type == CFNumberGetTypeID()) {
	    int value;
	    CFNumberGetValue((CFNumberRef)ref, kCFNumberIntType, &value);
	    out << setw(10) << value << " (" << hex << value << dec << ")";

      } else if (ref_type == CFStringGetTypeID()) {
	    char buf[1024];
	    CFIndex len = CFStringGetLength((CFStringRef) ref);
	    assert(len < sizeof buf);
	    CFStringGetCString((CFStringRef)ref, buf, sizeof buf, kCFStringEncodingASCII);
	    buf[len] = 0;
	    out << "\"" << buf << "\"";

      } else {
	    CFStringRef type_str = CFCopyTypeIDDescription(ref_type);
	    char buf[128];
	    CFIndex len = CFStringGetLength(type_str);
	    assert(len < sizeof buf);
	    CFStringGetCString(type_str, buf, sizeof buf, kCFStringEncodingASCII);
	    buf[len] = 0;
	    out << "<typeid=" << buf << ">";
	    CFRelease(type_str);
      }

      return out;
}

void MacICACameraControl::debug_dump_default_(std::ostream&out) const
{
      CFIndex dict_size = CFDictionaryGetCount(dev_dict_);

      CFStringRef*keys = new CFStringRef[dict_size];
      CFTypeRef*values = new CFTypeRef[dict_size];

      CFDictionaryGetKeysAndValues(dev_dict_, (const void**)keys, (const void**)values);

      for (int idx = 0 ; idx < dict_size ; idx += 1) {
	    string key_str = get_string(keys[idx]);
	    out << setw(28) << key_str << "  value: ";
	    dump_value(out, values[idx]) << endl;
      }

      delete[]keys;
      delete[]values;
}

void MacICACameraControl::debug_dump_capabilities_(std::ostream&out) const
{
      CFTypeRef val = (CFTypeRef) CFDictionaryGetValue(dev_dict_, CFSTR("capa"));
      if (val == 0) {
	    out << "No device properties key?" << endl;
	    return;
      }

      CFStringRef desc = CFCopyDescription(val);
      size_t len = CFStringGetLength(desc);
      char*bbuf = new char[len+1];
      CFStringGetCString(desc, bbuf, len+1, kCFStringEncodingASCII);
      bbuf[len] = 0;
      out << bbuf << endl;
      CFRelease(desc);
      delete[]bbuf;
}

void MacICACameraControl::debug_dump_data_(std::ostream&out) const
{
      CFTypeRef val = (CFTypeRef) CFDictionaryGetValue(dev_dict_, CFSTR("data"));
      if (val == 0) {
	    out << "No data key?" << endl;
	    return;
      }

      CFStringRef desc = CFCopyDescription(val);
      size_t len = CFStringGetLength(desc);
      char*bbuf = new char[len+1];
      CFStringGetCString(desc, bbuf, len+1, kCFStringEncodingASCII);
      bbuf[len] = 0;
      out << bbuf << endl;
      CFRelease(desc);
      delete[]bbuf;
}

void MacICACameraControl::debug_dump_device_(std::ostream&out) const
{
      CFStringRef desc = CFCopyDescription(dev_prop_dict_);
      size_t len = CFStringGetLength(desc);
      char*bbuf = new char[len+1];
      CFStringGetCString(desc, bbuf, len+1, kCFStringEncodingASCII);
      bbuf[len] = 0;
      out << bbuf << endl;
      CFRelease(desc);
      delete[]bbuf;
}

void MacICACameraControl::debug_dump(std::ostream&out, const std::string&detail)const
{
      out << "**** MacICACameraControl dump " << detail << " ****" << endl;
      if (detail == "capabilities") {
	    debug_dump_capabilities_(out);
	    return;
      }

      if (detail == "data") {
	    debug_dump_data_(out);
	    return;
      }

      if (detail == "device") {
	    debug_dump_device_(out);
	    return;
      }

      debug_dump_default_(out);
}
