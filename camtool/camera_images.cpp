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

      long last_key = -1;
      for (list<CameraControl::file_key_t>::iterator cur = files.begin()
		 ; cur != files.end() ; cur ++) {

	    last_key = cur->first;
	    CameraControl::debug_log << "  " << last_key << " " << cur->second << endl;

	    QString file_name (cur->second.c_str());
	    QVariant file_index ((int)last_key);

	    QListWidgetItem*item = new QListWidgetItem(file_name);
	    item->setData(Qt::UserRole, file_index);
	    ui.images_list->addItem(item);
      }
#if 0
      char*buf;
      size_t buf_len;
      camera->get_image_thumbnail(last_key, buf, buf_len);
      QString thumb_path = QFileDialog::getSaveFileName(0, QString("(debug) Where to write a thumbnail?"));
      FILE*thumb_fd = fopen(thumb_path.toAscii(), "wb");
      assert(thumb_fd);
      fwrite(buf, 1, buf_len, thumb_fd);
      fclose(thumb_fd);
      delete[]buf;
#endif
}

void CamtoolMain::images_list_slot_(QListWidgetItem*item)
{
      if (selected_camera_ == 0)
	    return;

      QString file_name = item->text();
      QVariant file_index = item->data(Qt::UserRole);

      char*buf;
      size_t buf_len;
      selected_camera_->get_image_data(file_index.toInt(), buf, buf_len);

      QString path = QFileDialog::getSaveFileName(0, tr("Save Image"), file_name);
      FILE*fd = fopen(path.toAscii(), "wb");
      assert(fd);
      fwrite(buf, 1, buf_len, fd);
      fclose(fd);
      delete[]buf;
}
