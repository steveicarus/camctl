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
# include  "CamtoolPreferences.h"
# include  "CamtoolPreview.h"
# include  "image_math.h"
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

void CamtoolMain::display_thumbnail_(CameraControl*camera, long cur_key)
{
	// Get the thumbnail of the new image and decode it into a
	// QImage object.
      char*buf;
      size_t buf_len;
      camera->get_image_thumbnail(cur_key, buf, buf_len);

      QImage image_tmp;
      image_tmp.loadFromData((const uchar*)buf, buf_len);
      delete[]buf;

	// Calculate thumbnail histograms
      QImage red_ihist(THUMB_HIST_WID, THUMB_HIST_HEI, QImage::Format_RGB32);
      QImage gre_ihist(THUMB_HIST_WID, THUMB_HIST_HEI, QImage::Format_RGB32);
      QImage blu_ihist(THUMB_HIST_WID, THUMB_HIST_HEI, QImage::Format_RGB32);

      calculate_histograms(image_tmp, red_ihist, gre_ihist, blu_ihist, true);

      action_thumbnail_hist_red_  ->setPixmap(QPixmap::fromImage(red_ihist));
      action_thumbnail_hist_green_->setPixmap(QPixmap::fromImage(gre_ihist));
      action_thumbnail_hist_blue_ ->setPixmap(QPixmap::fromImage(blu_ihist));

	// If necessary, scale the thumbnail to fit the standard
	// thumbnail window.

      if (image_tmp.width() > THUMB_WID
	  || image_tmp.height() > THUMB_HEI
	  || (image_tmp.width() != THUMB_WID && image_tmp.height() != THUMB_HEI))
	  image_tmp = image_tmp.scaled(THUMB_WID,THUMB_HEI, Qt::KeepAspectRatio);

	// Put the thumbnail into the display.
      action_thumbnail_pixmap_->setPixmap(QPixmap::fromImage(image_tmp));
}

void CamtoolMain::write_tethered_image_(const QString&file_name, const char*data, size_t data_len)
{
	// Get the destination directory for tethered images. If the
	// directory doesn't exist, try to create it in its parent
	// directory. 
      QDir dir (preferences_->get_tethered_path());
      if (! dir.exists()) {
	    QString tmp = dir.dirName();
	    if (!dir.cdUp()) {
		  QMessageBox::information(this, tr("No Directory"),
				   tr("Directory doesn't exist: ") + dir.path());
		  return;
	    }

	    if (!dir.mkdir(tmp)) {
		  QMessageBox::information(this, tr("No Directory"),
				   tr("Unable to make directory: ") + dir.path());
		  return;
	    }
	    dir.cd(tmp);
      }

	// Look for a file name to use for the output image.
      QString path;
      for (;;) {
	      // Get the next file name to use.
	    QString use_name = preferences_->get_tethered_file();

	      // Get the suffix that the camera selected.
	    int suff = file_name.lastIndexOf(QChar('.'));
	    if (suff >= 0)
		  use_name .append( file_name.mid(suff) );

	      // Build the path to the destination file and see if it
	      // doesn't exist already. If it doesn't exist, we're
	      // done. If it does exist, try again.
	    path = dir.filePath(use_name);
	    QFile tmpfile(path);
	    if (! tmpfile.exists())
		  break;
      }

	// Create the file and write the image data.
      FILE*fd = fopen(path.toAscii(), "wb");
      if (fd == 0) {
	    QMessageBox::information(this, tr("Write Error"),
			     tr("Unable to open output file for write: ") + path);
	    return;
      }

      assert(fd);
      fwrite(data, 1, data_len, fd);
      fclose(fd);
}

void CamtoolMain::camera_image_added(CameraControl*camera, const CameraControl::file_key_t&file)
{
      long cur_key = file.first;

      QString file_name (file.second.c_str());
      QVariant file_index ((int)cur_key);

	// Add a file entry into the file list.
      QListWidgetItem*item = new QListWidgetItem(file_name);
      item->setData(Qt::UserRole, file_index);
      ui.images_list->addItem(item);

	// Display a thumbnail of the captured image.
      display_thumbnail_(camera, cur_key);

	// If tethered capture is in progress, or if the preview
	// window is open, then we need the image data.
      char*buf = 0;
      size_t buf_len = 0;
      if (tethered_in_progress_ || preview_window_active()) {
	    bool remove_image_flag = false;
	    if (tethered_in_progress_)
		  remove_image_flag = true;

	    camera->get_image_data(cur_key, buf, buf_len, remove_image_flag);
      }

	// If the preview is enabled, then display the image there.
      if (preview_window_active())
	    preview_->display_preview_image(file_name, buf, buf_len);

	// If we are busy with a tethered capture, then immediately
	// collect the image from the camera and send it to the
	// tethered capture directory.
      if (tethered_in_progress_)
	    write_tethered_image_(file_name, buf, buf_len);

	// Clean up the allocated image buffer, if any.
      if (buf) delete[]buf;
}

void CamtoolMain::camera_image_deleted(CameraControl*, const CameraControl::file_key_t&file)
{
      CameraControl::debug_log << "CamtoolMain:: delete image?" << endl << flush;
      long cur_key = file.first;

      QString file_name (file.second.c_str());
      QVariant file_index ((int)cur_key);

      for (int idx = 0 ; idx < ui.images_list->count() ; idx += 1) {
	    QListWidgetItem*cur = ui.images_list->item(idx);
	    if (cur->data(Qt::UserRole) == file_index) {
		  ui.images_list->removeItemWidget(cur);
		  delete cur;
		  break;
	    }
      }
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
