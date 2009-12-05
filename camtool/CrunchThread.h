#ifndef __CamtoolPreview_H
#define __CamtoolPreview_H

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
# include  <QThread>
# include  <QMutex>
# include  <QWaitCondition>
# include  <QImage>
# include  "LibRawQt.h"

class QString;

 /*
  * Create a thread to do background image processing and other
  * number crunching. The CamtoolPreview class uses an instance of
  * this class as a thread, and collects the results by receiving the
  * signals that this class emits.
  */
class CrunchThread : public QThread {

      Q_OBJECT

    public:
      CrunchThread();
      ~CrunchThread();

	// These constants are the dimensions of the generated
	// histogram charts.
      enum {CHART_WID = 128, CHART_HEI = 50};

	// The client uses this method to hand an image to this
	// instance. This image is compressed/formatted. The cruncher
	// instance will process this data into an internal form, so
	// the caller need to preserve the data on return.
      void process_preview_data(const QString&file_name,
				const QByteArray&image_data);

      void clean_up();

    signals:
	// The cruncher thread sends these signals to hand over
	// completed pixmaps...

	// ... Full image pixmap
      void display_preview_image(QImage*pix);
	// ... RGB hitograms
      void display_rgb_hist_image(QImage*r, QImage*g, QImage*b);

    private:
      void run();
      void crunch_preview_image_(void);

    private:
      QMutex mutex_;
      QWaitCondition wait_;

      QByteArray image_data_;
      QImage image_preview_;
      QImage image_hist_red_;
      QImage image_hist_gre_;
      QImage image_hist_blu_;
      bool image_preview_busy_;
      bool thread_quit_;

      LibRawQt libraw_;
};

#endif
