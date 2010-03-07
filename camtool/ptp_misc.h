#ifndef _ptp_misc_H
#define _ptp_misc_H
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

# include  <inttypes.h>
# include  <string>

/*
 * Return the string description of the opcode. If it is a vendor
 * specific code, then use the vendorid to look it up.
 */
extern std::string ptp_opcode_string(uint16_t code, uint32_t vendorid);

/*
 * Return the string description of the event code.
 */
extern std::string ptp_event_string(uint16_t code, uint32_t vendorid);

/*
 * Return the string description of the property code
 */
extern std::string ptp_property_string(uint16_t code, uint32_t vendorid);

/*
 * Return the string description of the property value.
 */
extern std::string ptp_property_uint8_string(uint16_t code, uint8_t value,
					     uint32_t vendorid);
extern std::string ptp_property_uint16_string(uint16_t code, uint16_t value,
					       uint32_t vendorid);
extern std::string ptp_property_uint32_string(uint16_t code, uint32_t value,
					       uint32_t vendorid);

#endif
