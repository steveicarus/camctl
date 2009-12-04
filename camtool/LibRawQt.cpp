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

# include  "LibRawQt.h"
# include  <QImage>

LibRawQt::LibRawQt()
{
}

LibRawQt::~LibRawQt()
{
}

bool LibRawQt::open_image(const QByteArray&src)
{
      data_ = src;
      int rc = open_buffer(data_.data(), data_.length());
      if (rc < 0) {
	    recycle();
	    data_.clear();
	    return false;
      }
      return true;
}

void LibRawQt::close_image(void)
{
      recycle();
      data_.clear();
}

bool LibRawQt::read_image(QImage*image)
{
      int rc;

      rc = unpack();
      assert(rc == 0);

      imgdata.params.document_mode = 0;
      imgdata.params.use_camera_wb = 1;
      imgdata.params.output_bps = 8;
      imgdata.params.filtering_mode = LIBRAW_FILTERING_AUTOMATIC;
      imgdata.params.no_auto_bright = 1;
      rc = dcraw_process();
      assert(rc == 0);

      libraw_processed_image_t*img = dcraw_make_mem_image(&rc);
      assert(img->type == LIBRAW_IMAGE_BITMAP);
      assert(img->colors == 3);
      assert(img->bits == 8);

      {
	    QImage tmp ((const uchar*)img->data, img->width, img->height,
			img->width * img->colors * (img->bits/8),
			QImage::Format_RGB888);
	    *image = tmp.copy();
      }
      ::free(img);

      return true;
}
