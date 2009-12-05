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
# include  "CrunchThread.h"
# include  "CamtoolMain.h"
# include  <QMutexLocker>
# include  "image_math.h"
# include  <iostream>

using namespace std;

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

      bool rc;

	// Try opening the stream as a raw image.
      rc = libraw_.open_image(image_data_);
      if (rc) {
	    rc = libraw_.read_image(&image_preview_);
	    CameraControl::debug_log << TIMESTAMP
				     << ": CrunchThread::crunch_preview_image_"
				     << "libraw_.read_image returned "
				     << (rc?"TRUE":"FALSE") << "."<< endl << flush;
	    libraw_.close_image();
      }

	// Decompress the image. If for some reason the decompress
	// does not work, then ignore it.
      if (rc == false) {
	    rc = image_preview_.loadFromData( image_data_ );
      }

      if (rc == false) {
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
