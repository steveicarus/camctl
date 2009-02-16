
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
# include  <QTreeWidgetItem>
# include  <iostream>
# include  <iomanip>
# include  <assert.h>

using namespace std;

static void describe_in_tree(QTreeWidgetItem*root, CFDictionaryRef dict);
static void describe_in_tree(QTreeWidgetItem*root, CFArrayRef val);

static void set_item_value(QTreeWidgetItem*item, CFTypeRef val)
{
      Boolean rc;
      CFTypeID val_type = CFGetTypeID(val);

      if (val_type == CFStringGetTypeID()) {
	    char buf[1024];
	    CFIndex slen;

	    CFStringRef val_ref = (CFStringRef)val;
	    slen = CFStringGetLength(val_ref);
	    assert(slen < (CFIndex)sizeof buf);
	    rc = CFStringGetCString(val_ref, buf, sizeof buf, kCFStringEncodingASCII);
	    assert(rc);
	    buf[slen] = 0;

	    item->setText(1, QString(buf));

      } else if (val_type == CFNumberGetTypeID()) {

	    CFNumberRef val_ref = (CFNumberRef) val;
	    switch (CFNumberGetType(val_ref)) {
		case kCFNumberSInt32Type: {
		      SInt32 tmp;
		      rc = CFNumberGetValue(val_ref, kCFNumberSInt32Type, &tmp);
		      item->setText(1, QString("0x%1 (%2)")
				    .arg(tmp,8,16, QLatin1Char('0'))
				    .arg(tmp,0,10));
		      break;
		}
		case kCFNumberSInt64Type: {
		      SInt64 tmp;
		      rc = CFNumberGetValue(val_ref, kCFNumberSInt64Type, &tmp);
		      item->setText(1, QString("0x%1 (%2)")
				    .arg(tmp,16,16,QLatin1Char('0'))
				    .arg(tmp,0,10));
		      break;
		}
		default:
		  item->setText(1, QString("Number type=%1").arg(CFNumberGetType(val_ref)));
		  break;
	    }

      } else if (val_type == CFDictionaryGetTypeID()) {
	    item->setText(1, "");
	    CFDictionaryRef val_ref = (CFDictionaryRef) val;
	    describe_in_tree(item, val_ref);
	    
      } else if (val_type == CFArrayGetTypeID()) {
	    item->setText(1, "");
	    CFArrayRef val_ref = (CFArrayRef) val;
	    describe_in_tree(item, val_ref);
	    
      } else {
	    item->setText(1, QString("TypeID=%1").arg(val_type));
      }
}

static void describe_in_tree(QTreeWidgetItem*root, CFDictionaryRef dict)
{

	// Now scan the dev_dict dictionary to get all the key/value
	// pairs for the device.
      CFIndex dict_size = CFDictionaryGetCount(dict);
      CFStringRef*keys = new CFStringRef[dict_size];
      CFTypeRef*values = new CFTypeRef[dict_size];
      CFDictionaryGetKeysAndValues(dict, (const void**)keys, (const void**)values);

      for (int idx = 0 ; idx < dict_size ; idx += 1) {
	    char buf[1024];
	    Boolean rc;

	    CFIndex slen = CFStringGetLength(keys[idx]);
	    assert(slen < (CFIndex)sizeof buf);
	    rc = CFStringGetCString(keys[idx], buf, sizeof buf, kCFStringEncodingASCII);
	    assert(rc);
	    buf[slen] = 0;

	    QTreeWidgetItem*item = new QTreeWidgetItem;
	    item->setText(0, buf);

	    set_item_value(item, values[idx]);

	    root->addChild(item);
      }

      delete[]keys;
      delete[]values;
}

static void describe_in_tree(QTreeWidgetItem*root, CFArrayRef val)
{
      CFIndex array_size = CFArrayGetCount(val);

      for (CFIndex idx = 0 ; idx < array_size ; idx += 1) {
	    CFTypeRef cur = (CFTypeRef) CFArrayGetValueAtIndex(val, idx);
	    QTreeWidgetItem*item = new QTreeWidgetItem;
	    item->setText(0, QString("%1").arg(idx));
	    set_item_value(item, cur);
	    root->addChild(item);
      }
}

QTreeWidgetItem* MacICACameraControl::describe_camera(void)
{
      QTreeWidgetItem*root = new QTreeWidgetItem;
      root->setText(0, "MacICACameraControl");
      root->setFirstColumnSpanned(true);

      describe_in_tree(root, dev_dict_);
      root->addChild(CameraControl::describe_camera());

      return root;
}

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
ostream& MacICACameraControl::dump_value(ostream&out, CFTypeRef ref)
{
      assert(ref);

      CFTypeID ref_type = CFGetTypeID(ref);
      if (ref_type == CFNumberGetTypeID()) {
	    int value;
	    CFNumberGetValue((CFNumberRef)ref, kCFNumberIntType, &value);
	    out << setw(10) << value << " (" << hex << value << dec << ")";

      } else if (ref_type == CFStringGetTypeID()) {
	    char buf[1024];
	    CFIndex len = CFStringGetLength((CFStringRef) ref);
	    assert(len < (CFIndex)sizeof buf);
	    CFStringGetCString((CFStringRef)ref, buf, sizeof buf, kCFStringEncodingASCII);
	    buf[len] = 0;
	    out << "\"" << buf << "\"";

      } else if (ref_type == CFDictionaryGetTypeID()) {
	    CFDictionaryRef dref = (CFDictionaryRef)ref;
	    CFIndex dict_size = CFDictionaryGetCount(dref);
	    out << "**** Dictionary w/ "
		<< dec << dict_size
		<< " entries ****" << endl;

	    CFStringRef*keys = new CFStringRef[dict_size];
	    CFTypeRef*values = new CFTypeRef[dict_size];

	    CFDictionaryGetKeysAndValues(dref, (const void**)keys,
					 (const void**)values);

	    for (int idx = 0 ; idx < dict_size ; idx += 1) {
		  string key_str = get_string(keys[idx]);
		  out << setw(36) << key_str << "  value: ";
		  dump_value(out, values[idx]) << endl;
	    }

	    delete[]keys;
	    delete[]values;

      } else {
	    CFStringRef type_str = CFCopyTypeIDDescription(ref_type);
	    char buf[128];
	    CFIndex len = CFStringGetLength(type_str);
	    assert(len < (CFIndex)sizeof buf);
	    CFStringGetCString(type_str, buf, sizeof buf, kCFStringEncodingASCII);
	    buf[len] = 0;
	    out << "<typeid=" << buf << ">";
	    CFRelease(type_str);

	    type_str = CFCopyDescription(ref);
	    len = CFStringGetLength(type_str);
	    char*msg = new char[len+1];
	    CFStringGetCString(type_str, msg, len+1, kCFStringEncodingASCII);
	    msg[len] = 0;
	    out << msg << endl;
	    delete[]msg;
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

void MacICACameraControl::debug_dump(std::ostream&out, const std::string&detail)const
{
      out << "**** MacICACameraControl dump " << detail << " ****" << endl;
      debug_dump_default_(out);
}
