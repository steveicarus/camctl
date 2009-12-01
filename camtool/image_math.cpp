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

# include  "image_math.h"
# include  <qapplication.h>
# include  <QImage>

static void normalize(const int HIST_WID, const int HIST_HEI,
		      unsigned long*red_hist,
		      unsigned long*gre_hist,
		      unsigned long*blu_hist)
{
	// Normalize each histogram by finding the maximum histogram
	// bar and scaling all the bars so that the maximum bar goes
	// right to the top of the graph.

      unsigned long norm_red = 0;
      unsigned long norm_gre = 0;
      unsigned long norm_blu = 0;
      for (int idx = 0 ; idx < HIST_WID ; idx += 1) {
	    if (red_hist[idx] > norm_red) norm_red = red_hist[idx];
	    if (gre_hist[idx] > norm_gre) norm_gre = gre_hist[idx];
	    if (blu_hist[idx] > norm_blu) norm_blu = blu_hist[idx];
      }
      for (int idx = 0 ; idx < HIST_WID ; idx += 1) {
	    if (norm_red > 0)
		  red_hist[idx] = red_hist[idx] * HIST_HEI / norm_red;
	    if (norm_gre > 0)
		  gre_hist[idx] = gre_hist[idx] * HIST_HEI / norm_gre;
	    if (norm_blu > 0)
		  blu_hist[idx] = blu_hist[idx] * HIST_HEI / norm_blu;
      }
}

void calculate_histograms(const QImage&ref, QImage&red, QImage&green, QImage&blue,
			  bool skip_saturated_black)
{
      const int HIST_WID = red.width();
      const int HIST_HEI = red.height();

	// The dimensions of the target image must be non-nil, and the
	// width must be <= 256.
      assert(HIST_WID > 0 && HIST_HEI > 0 && HIST_WID <= 256);

	// The histogram targets must all be the same size.
      assert(red.width()   == HIST_WID);
      assert(green.width() == HIST_WID);
      assert(blue.width()  == HIST_WID);
      assert(red.height()   == HIST_HEI);
      assert(green.height() == HIST_HEI);
      assert(blue.height()  == HIST_HEI);

	// Clear the histogram counts
      unsigned long red_hist[256];
      unsigned long gre_hist[256];
      unsigned long blu_hist[256];
      for (int idx = 0 ; idx < HIST_WID ; idx += 1) {
	    red_hist[idx] = 0;
	    gre_hist[idx] = 0;
	    blu_hist[idx] = 0;
      }

	// Accumulate level counts for the image.
      for (int y = 0 ; y < ref.height() ; y += 1) {
	    for (int x = 0 ; x < ref.width() ; x += 1) {
		  QRgb pix = ref.pixel(x,y);
		  if (skip_saturated_black && pix == qRgb(0,0,0))
			continue;
		  red_hist[ qRed(pix)   / (256/HIST_WID) ] += 1;
		  gre_hist[ qGreen(pix) / (256/HIST_WID) ] += 1;
		  blu_hist[ qBlue(pix)  / (256/HIST_WID) ] += 1;
	    }
      }

	// Normalize each histogram by finding the maximum histogram
	// bar and scaling all the bars so that the maximum bar goes
	// right to the top of the graph.
      normalize(HIST_WID, HIST_HEI, red_hist, gre_hist, blu_hist);

	// Draw the target histograms.
      QRgb pix_black = qRgb(0,0,0);
      QRgb pix_red = qRgb(255,0,0);
      QRgb pix_gre = qRgb(0,255,0);
      QRgb pix_blu = qRgb(0,0,255);

      for (int y = 0 ; y < HIST_HEI ; y += 1) {
	    for (int x = 0 ; x < HIST_WID ; x += 1) {
		  unsigned long thresh = HIST_HEI - y - 1;
		  red  .setPixel(x,y,red_hist[x] >= thresh? pix_red : pix_black);
		  green.setPixel(x,y,gre_hist[x] >= thresh? pix_gre : pix_black);
		  blue .setPixel(x,y,blu_hist[x] >= thresh? pix_blu : pix_black);
	    }
      }
}
