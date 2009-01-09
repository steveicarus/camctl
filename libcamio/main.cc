/*
 * Copyright (c) 2008 Stephen Williams (steve@icarus.com)
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

# include  "CameraControl.h"
# include  <iostream>
# include  <iomanip>
# include  <valarray>
# include  <assert.h>

using namespace std;

static CameraControl* current_camera = 0;

void cmd_battery(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      int val = current_camera->battery_level();
      if (val < 0) {
	    cout << "No battery info" << endl;
      } else {
	    cout << "Battery: " << val << "%" << endl;
      }
}

void cmd_camera(int argc, char**argv)
{
      if (CameraControl::camera_list.empty()) {
	    cerr << "No cameras?" << endl;
	    return;
      }

      valarray<CameraControl*>selection_array(CameraControl::camera_list.size());
      int idx = 0;
      for (list<CameraControl*>::iterator cur = CameraControl::camera_list.begin()
		 ; cur != CameraControl::camera_list.end() ; cur ++ ) {

	    assert(idx < selection_array.size());
	    selection_array[idx] = *cur;
	    cout << idx << ": "
		 << (*cur)->camera_make() << " " << (*cur)->camera_model()
		 << " (" << (*cur)->control_class() << ")" << endl;
	    idx += 1;
      }

      printf("Select Camera [0]: ");
      fflush(stdout);

      int sel = -1;
      char buf[16];
      while (sel < 0 && fgets(buf, sizeof buf, stdin)) {
	    char*cp = buf + strspn(buf, " \r\n");
	    if (*cp == 0) {
		  sel = 0;
	    } else {
		  sel = strtoul(cp, 0, 10);
	    }

	    if (sel >= selection_array.size()) {
		  sel = -1;
		  printf("Select Camera [0]: ");
		  fflush(stdout);
	    }
      }

      current_camera = CameraControl::camera_list.front();
}

static void cmd_capture(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      current_camera->capture_image();
}

static void cmd_dump(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      if (argc < 2)
	    current_camera->debug_dump(cout, "default");
      else
	    current_camera->debug_dump(cout, argv[1]);
}

static void cmd_get_image(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      if (argc < 2) {
	    cerr << "Which image?" << endl;
	    return;
      }

      long key = strtol(argv[1],0,0);
      char*buf;
      size_t buf_len;
      current_camera->get_image_data(key, buf, buf_len);

      if (buf == 0) {
	    cerr << "Key not valid." << endl;
	    return;
      }

      FILE*fd = fopen("tmp.jpg", "wb");
      assert(fd);
      fwrite(buf, 1, buf_len, fd);
      fclose(fd);

      delete[]buf;
}

static void cmd_ls(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      const list<CameraControl::file_key_t>& files = current_camera->image_list();
      typedef list<CameraControl::file_key_t>::const_iterator list_iter_t;
      for (list_iter_t cur = files.begin() ; cur != files.end() ; cur++) {
	    cout << cur->second
		 << "  (key=" << hex << cur->first << dec << ")" << endl;
      }
}

static void cmd_mode(int argc, char**argv)
{
      if (current_camera == 0) {
	    cerr << "Select a camera first." << endl;
	    return;
      }

      string mode = current_camera->exposure_program_mode();
      cout << "Exposure Program Mode: " << mode << endl;
}

static void cmd_help(int, char**);

static struct cmd_table_s {
      const char*name;
      void (*fun) (int argc, char**argv);
      const char*short_help;
} cmd_table[] = {
      { "battery",  cmd_battery,  "Get Battery level" },
      { "camera",   cmd_camera,   "Select the camera to use" },
      { "capture",  cmd_capture,  "Capture image (take picture)" },
      { "dump",     cmd_dump,     "Debug dump" },
      { "get_image",cmd_get_image,"Get image from camera and save to file"},
      { "ls",       cmd_ls,       "List files on the camera" },
      { "mode",     cmd_mode,     "Get camera program mode" },
      { "help",     cmd_help,     "Help!" },
      { 0, 0 }
};

static void cmd_help(int, char**)
{
      for (int idx = 0 ; cmd_table[idx].name ; idx += 1) {
	    cout << setw(10) << cmd_table[idx].name
		 << " - " << cmd_table[idx].short_help << endl;
      }
}

int main(int argc, char*argv[])
{
      CameraControl::camera_inventory();

      printf(">");
      fflush(stdout);

      char line_buffer[2048];
      while ( fgets(line_buffer, sizeof line_buffer, stdin) ) {
	    char*linev[1024];
	    int linec = 0;

	    char*cp = line_buffer + strspn(line_buffer, " \r\n");
	    while (*cp != 0) {

		  linev[linec++] = cp;
		  cp += strcspn(cp, " \r\n");
		  if (*cp != 0) {
			*cp++ = 0;
			cp += strspn(cp, " \r\n");
		  }
	    }


	    linev[linec] = 0;
	    if (linec > 0) {
		  int idx;
		  for (idx = 0 ; cmd_table[idx].name ; idx += 1) {
			if (strcmp(cmd_table[idx].name, linev[0]) == 0)
			      break;
		  }

		  if (cmd_table[idx].name)
			cmd_table[idx].fun (linec, linev);
		  else
			printf("Invalid command: %s\n", linev[0]);
	    }
	    printf(">"); fflush(stdout);
      }

      return 0;
}
