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


void CamtoolMain::resync_camera_images_(void)
{
      assert(selected_camera_);
      std::list<CameraControl::file_key_t> files = selected_camera_->image_list();

      ui.images_list->clear();

      for (list<CameraControl::file_key_t>::iterator cur = files.begin()
		 ; cur != files.end() ; cur ++) {

	    long cur_key = cur->first;
	    CameraControl::debug_log << "  " << cur_key << " " << cur->second << endl;

	    QString file_name (cur->second.c_str());
	    QVariant file_index ((int)cur_key);

	    QListWidgetItem*item = new QListWidgetItem(file_name);
	    item->setData(Qt::UserRole, file_index);
	    ui.images_list->addItem(item);
      }
}

void CamtoolMain::camera_image_added(CameraControl*camera, const CameraControl::file_key_t&file)
{
      long cur_key = file.first;

      QString file_name (file.second.c_str());
      QVariant file_index ((int)cur_key);

      QListWidgetItem*item = new QListWidgetItem(file_name);
      item->setData(Qt::UserRole, file_index);
      ui.images_list->addItem(item);

	// Get the thumbnail of the new image and write it into the
	// image thumbnail display.
      char*buf;
      size_t buf_len;
      camera->get_image_thumbnail(cur_key, buf, buf_len);

      QPixmap pix_tmp;
      pix_tmp.loadFromData((const uchar*)buf, buf_len);
      action_thumbnail_pixmap_->setPixmap(pix_tmp);

      delete[]buf;
}

void CamtoolMain::camera_image_deleted(CameraControl*, const CameraControl::file_key_t&)
{
      CameraControl::debug_log << "CamtoolMain:: delete image?" << endl << flush;
}

void CamtoolMain::images_list_slot_(QListWidgetItem*item)
{
      if (selected_camera_ == 0)
	    return;

      QString file_name = item->text();
      QVariant file_index = item->data(Qt::UserRole);

      QString path = QFileDialog::getSaveFileName(0, tr("Save Image"), file_name);
      if (! path.isEmpty()) {
	    QMessageBox::StandardButton resp
		  = QMessageBox::question(this, tr("Delete From Camera"),
					  tr("Delete this image from the camera"
					     " after download?"),
					  QMessageBox::Yes|QMessageBox::No,
					  QMessageBox::No);
	    bool delete_flag = resp==QMessageBox::Yes;
	    char*buf;
	    size_t buf_len;
	    selected_camera_->get_image_data(file_index.toInt(), buf, buf_len,
					     delete_flag);

	    FILE*fd = fopen(path.toAscii(), "wb");
	    assert(fd);
	    fwrite(buf, 1, buf_len, fd);
	    fclose(fd);
	    delete[]buf;
      }
}

void CamtoolMain::images_refresh_slot_(void)
{
      if (selected_camera_ == 0)
	    return;

      resync_camera_images_();
}
