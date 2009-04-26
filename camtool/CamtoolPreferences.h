#ifndef __CamtoolPreferences_H
#define __CamtoolPreferences_H

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
# include  "ui_preferences.h"
# include  <QSettings>

class CamtoolPreferences : public QDialog {

      Q_OBJECT

    public:
      CamtoolPreferences(QWidget*parent);
      ~CamtoolPreferences();

    public: // "Time Lapse" settings
      void set_capture_interval(int);
      int  get_capture_interval(void);

      void set_sequence_duration(int);
      int  get_sequence_duration(void);
      
    public: // "Tethered" settings

	// Get the preferred tethered path directory. This method will
	// make sure that the path exists, is a directory, and is writable.
      QString get_tethered_path() const;

	// Get the next name to use for a tethered file. This method
	// will also automatically substitute the file number in the
	// name string and increment the counter.
      QString get_tethered_file();

	// Increment the number to use for the tethered file
	// name. Call this after each capture completes so that the
	// next tethered_file has a different name.
      void step_tethered_number();

    public: // "Preview" settings

	// Remember the state of the preview window, raised(true) or
	// closed(false).
      bool get_preview_raised();
      void set_preview_raised(bool);

    private:
      Ui::PreferencesDialog ui;

      QSettings settings_;

      QString home_path_;

    private:
      void clear_tethered_path_();
      void clear_tethered_name_();

    private slots:
      void select_tethered_slot_();
      void tethered_path_slot_();
      void tethered_name_slot_();
      void tethered_number_slot_(int);
      void image_output_buttons_slot_(QAbstractButton*);

      void select_logfile_slot_(void);
      void logfile_path_slot_(void);
      void debug_buttons_slot_(QAbstractButton*);
};

#endif
