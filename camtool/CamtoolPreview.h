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
# include  "ui_preview.h"

class CamtoolMain;
class QString;

class CamtoolPreview : public QDialog {

      Q_OBJECT

    public:
      CamtoolPreview(CamtoolMain*parent);
      ~CamtoolPreview();

      void display_preview_image(const QString&file_name, const char*data, size_t data_len);

	// Override this event to add the ability to let the main
	// window know that this is closed.
      virtual void closeEvent(QCloseEvent*event);

    private slots:
      void preview_buttons_slot_(QAbstractButton*);

    private:
      QGraphicsScene* preview_scene_;
      QGraphicsPixmapItem*preview_pixmap_;

      enum {CHART_WID = 128, CHART_HEI = 50};
      QGraphicsScene*     charts_scene_;
      QGraphicsPixmapItem*charts_red_hist_;
      QGraphicsPixmapItem*charts_green_hist_;
      QGraphicsPixmapItem*charts_blue_hist_;

    private:
      CamtoolMain*main_window_;
      Ui::PreviewWindow ui;
};

#endif
