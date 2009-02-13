
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
# include  <ImageCapture/ImageCapture.h>
# include  <iostream>
# include  <assert.h>

using namespace std;

MacICABlacklist::MacICABlacklist(ICAObject dev)
: MacICACameraControl(dev)
{
}

MacICABlacklist::~MacICABlacklist()
{
}

CameraControl::capture_resp_t MacICABlacklist::capture_image(void)
{
      return CAP_NOT_SUPPORTED;
}
