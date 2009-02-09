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

# include  <qapplication.h>
# include  "CamtoolMain.h"
# include  <QFileDialog>
# include  <QMessageBox>
# include  <iostream>

using namespace std;


void CamtoolMain::camera_images(CameraControl*camera)
{
      std::list<CameraControl::file_key_t> files = camera->image_list();

      ui.images_list->clear();

      for (list<CameraControl::file_key_t>::iterator cur = files.begin()
		 ; cur != files.end() ; cur ++) {

	    CameraControl::debug_log << "  " << cur->first << " " << cur->second << endl;
	    ui.images_list->addItem(cur->second.c_str());
      }
}
