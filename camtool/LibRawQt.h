#ifndef __LibRawQt_H
#define __LibRawQt_H

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

# include  <libraw/libraw.h>
# include  <QByteArray>

class QImage;

class LibRawQt : private LibRaw {

    public:
      explicit LibRawQt();
      ~LibRawQt();

	// Open the bytearray as the datastream source for the next
	// image. This implicitly closes the previous image.
      bool open_image(const QByteArray&src);
      void close_image(void);

      bool read_image(QImage*image);

    private:
      QByteArray data_;
};

#endif
