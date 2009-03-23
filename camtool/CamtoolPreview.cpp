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
      charts_green_hist_->setPos(0, 1*(CHART_HEI+10));
      charts_blue_hist_ ->setPos(0, 2*(CHART_HEI+10));

      ui.preview_charts->setScene(charts_scene_);
}

CamtoolPreview::~CamtoolPreview()
{
      delete preview_pixmap_;
      delete preview_scene_;

      delete charts_red_hist_;
      delete charts_green_hist_;
      delete charts_blue_hist_;
      delete charts_scene_;
}

void CamtoolPreview::display_preview_image(const QString&file_name,
					   const char*data, size_t data_len)
{
      QImage image_tmp;
      image_tmp.loadFromData( (const uchar*)data, data_len );

	// Load the image into the display.
      preview_pixmap_->setPixmap(QPixmap::fromImage(image_tmp));

      QImage red_hist (CHART_WID, CHART_HEI, QImage::Format_RGB32);
      QImage gre_hist (CHART_WID, CHART_HEI, QImage::Format_RGB32);
      QImage blu_hist (CHART_WID, CHART_HEI, QImage::Format_RGB32);

      calculate_histograms(image_tmp, red_hist, gre_hist, blu_hist);

      charts_red_hist_  ->setPixmap(QPixmap::fromImage(red_hist));
      charts_green_hist_->setPixmap(QPixmap::fromImage(gre_hist));
      charts_blue_hist_ ->setPixmap(QPixmap::fromImage(blu_hist));
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
