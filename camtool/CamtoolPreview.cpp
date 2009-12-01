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
# include  "CamtoolPreview.h"
# include  "CamtoolMain.h"
# include  <QMutexLocker>
# include  "image_math.h"
# include  <iostream>

using namespace std;


CamtoolPreview::CamtoolPreview(CamtoolMain*parent)
: QDialog(parent, Qt::WindowMinimizeButtonHint), main_window_(parent)
{
      ui.setupUi(this);

      connect(ui.preview_buttons,
	      SIGNAL(clicked(QAbstractButton*)),
	      SLOT(preview_buttons_slot_(QAbstractButton*)));

      connect(ui.zoom_check,
	      SIGNAL(stateChanged(int)),
	      SLOT(zoom_check_slot_(int)));

	// The Cruncher thread sends results back to me by sending
	// signals. Connect those signals to slots in this class.
      connect(&cruncher_,
	      SIGNAL(display_preview_image(QImage*)),
	      SLOT(display_preview_image_slot_(QImage*)));
      connect(&cruncher_,
	      SIGNAL(display_rgb_hist_image(QImage*,QImage*,QImage*)),
	      SLOT(display_rgb_hist_image_slot_(QImage*,QImage*,QImage*)));

      preview_scene_ = new QGraphicsScene;
      preview_pixmap_ = new QGraphicsPixmapItem;
      preview_scene_->addItem(preview_pixmap_);
      ui.preview_image->setScene(preview_scene_);

      charts_scene_ = new QGraphicsScene;
      charts_red_hist_ = new QGraphicsPixmapItem;
      charts_green_hist_ = new QGraphicsPixmapItem;
      charts_blue_hist_ = new QGraphicsPixmapItem;

      charts_scene_->setBackgroundBrush(QBrush(QColor(0,0,0)));
      charts_scene_->addItem(charts_red_hist_);
      charts_scene_->addItem(charts_green_hist_);
      charts_scene_->addItem(charts_blue_hist_);

      charts_red_hist_  ->setPos(0, 0);
      charts_green_hist_->setPos(0, 1*(CrunchThread::CHART_HEI+10));
      charts_blue_hist_ ->setPos(0, 2*(CrunchThread::CHART_HEI+10));

      ui.preview_charts->setScene(charts_scene_);

      cruncher_.start();
}

CamtoolPreview::~CamtoolPreview()
{
      cruncher_.clean_up();
      cruncher_.wait();

      delete preview_pixmap_;
      delete preview_scene_;

      delete charts_red_hist_;
      delete charts_green_hist_;
      delete charts_blue_hist_;
      delete charts_scene_;
}

CrunchThread::CrunchThread()
: image_hist_red_(CHART_WID, CHART_HEI, QImage::Format_RGB32),
  image_hist_gre_(CHART_WID, CHART_HEI, QImage::Format_RGB32),
  image_hist_blu_(CHART_WID, CHART_HEI, QImage::Format_RGB32)
{
      image_preview_busy_ = false;
      thread_quit_ = false;
}

CrunchThread::~CrunchThread()
{
}

/*
 * The crunch thread is a simple loop that waits for a task from the
 * client, processed that task, and waits some more. The client
 * signals the task by loading the task parameters and setting a flag.
 */
void CrunchThread::run()
{
      mutex_.lock();
      for (;;) {
	      // The user gave me an image to process?
	    if (image_preview_busy_) {
		  crunch_preview_image_();
		  image_preview_busy_ = false;
		  wait_.wakeAll();
		  continue;
	    }

	      // The user is asking me to quit?
	    if (thread_quit_)
		  break;

	      // Nothing to do. Wait.
	    wait_.wait(&mutex_);
      }
      mutex_.unlock();
}

/*
 * The user uses this method to arrange for the thread to exit.
 */
void CrunchThread::clean_up()
{
      QMutexLocker lock_image(&mutex_);
      thread_quit_ = true;
      wait_.wakeAll();
}

void CrunchThread::crunch_preview_image_()
{
      CameraControl::debug_log << TIMESTAMP
	    << ": CrunchThread::crunch_preview_image_ load image data."
	    << endl << flush;

	// Decompress the image. If for some reason the decompress
	// does not work, then ignore it.
      bool load_rc = image_preview_.loadFromData( image_data_ );
      if (load_rc == false) {
	    CameraControl::debug_log << TIMESTAMP
		  << ": CrunchThread::crunch_preview_image_ failed to load image data."
		  << endl << flush;
	    return;
      }

	// Send the image to the preview display...
      emit display_preview_image(&image_preview_);

      calculate_histograms(image_preview_,
			   image_hist_red_,
			   image_hist_gre_,
			   image_hist_blu_);

      emit display_rgb_hist_image(&image_hist_red_,
				  &image_hist_gre_,
				  &image_hist_blu_);
}

/*
 * Receive the preview data from the main application. Here we take
 * the data and convert it into a form that the cruncher thread can
 * use. When the data/data_len bytes are no longer directly needed,
 * then this method hands the task off to the background thread and
 * returns to the caller.
 */
void CrunchThread::process_preview_data(const QString&file_name,
					const QByteArray&image_data)
{
      QMutexLocker lock_image(&mutex_);

	// Wait for any previous image to finish.
      while (image_preview_busy_) wait_.wait(&mutex_);

	// Load this new image data into the image_tmp_ and start the
	// thread processing.
      image_data_ = image_data;
      image_preview_busy_ = true;
      wait_.wakeAll();
}

/*
 * The main thread calls this method when it has a preview image to
 * process. Pass the data directly back to the cruncher thread. The
 * "process_preview_data" method will return when it no longer needs
 * the data pointer.
 */
void CamtoolPreview::display_preview_image(const QString&file_name,
					   const QByteArray&image_data)
{
      if (file_name.endsWith(".NEF")) {
	    CameraControl::debug_log << TIMESTAMP
		      << ": CamtoolPreview::display_preview_image: "
		      << "Skip preview display of .NEF image."
		      << endl << flush;
	    return;
      }

      CameraControl::debug_log << TIMESTAMP
			       << ": CamtoolPreview::display_preview_image: "
			       << "Process preview display of "
			       << file_name.toStdString()
			       << "." << endl << flush;
      cruncher_.process_preview_data(file_name, image_data);
}

void CamtoolPreview::display_preview_image_slot_(QImage*pix)
{
      CameraControl::debug_log << TIMESTAMP
			       << ": CamtoolPreview::display_preview_image_slot_"
			       << endl << flush;

      preview_pixmap_->setPixmap(QPixmap::fromImage(*pix));
      preview_pixmap_->update();

	// If the "Zoom 1:1" button is checked, then set the display
	// to 1-to-1. Otherwise, fit the image into the view.
      if (ui.zoom_check->isChecked()) {
	    ui.preview_image->setMatrix(QMatrix());
      } else {
	    ui.preview_image->fitInView(preview_pixmap_,
					Qt::KeepAspectRatioByExpanding);
      }
}

void CamtoolPreview::display_rgb_hist_image_slot_(QImage*red, QImage*gre, QImage*blu)
{
      charts_red_hist_  ->setPixmap(QPixmap::fromImage(*red));
      charts_green_hist_->setPixmap(QPixmap::fromImage(*gre));
      charts_blue_hist_ ->setPixmap(QPixmap::fromImage(*blu));
      charts_red_hist_  ->update();
      charts_green_hist_->update();
      charts_blue_hist_ ->update();
}

void CamtoolPreview::closeEvent(QCloseEvent*event)
{
      main_window_->close_preview_window();
      QDialog::closeEvent(event);
}

void CamtoolPreview::preview_buttons_slot_(QAbstractButton*button)
{
      switch (ui.preview_buttons->standardButton(button)) {

	  case QDialogButtonBox::Close:
	    main_window_->close_preview_window();
	    break;

	  default:
	    break;
      }

}

void CamtoolPreview::zoom_check_slot_(int state)
{
      if (state) {
	    ui.preview_image->setMatrix(QMatrix());
      } else {
	    ui.preview_image->fitInView(preview_pixmap_,
					Qt::KeepAspectRatioByExpanding);
      }
}
