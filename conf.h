/*
    LAC (Linux Air combat)
    Copyright 2015 by Robert J. Bosen. Major portions of
    this code were derived from "gl-117", by Thomas A. Drexl and
    other contributors, who are mentioned in the "Credits" menu.

    This file is part of LAC.

    LAC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    LAC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LAC; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* This file contains all configuration parsing code. */

#ifndef IS_CONF_H
#define IS_CONF_H

// interface definitions
extern unsigned int key_firecannon, key_firemissile, key_dropchaff, key_dropflare;
extern unsigned int key_selectmissile, key_thrustup, key_thrustdown;
extern unsigned int key_targetnearest, key_targetnext, key_targetprevious, key_targetlocking;

extern int joystick_firecannon, joystick_firemissile, joystick_dropchaff, joystick_dropflare;
extern int joystick_selectmissile, joystick_thrustup, joystick_thrustdown;
extern int joystick_ZoomFOVin, joystick_ZoomFOVout, joystick_targetnext, joystick_targetprevious, joystick_targetlocking, joystick_ToggleExtIntView;
extern int joystick_aileron, joystick_elevator, joystick_rudder, joystick_throttle;
extern int joystick_view_x, joystick_view_y;

extern unsigned char mouse_firecannon, mouse_firemissile;
extern unsigned char mouse_selectmissile;
extern unsigned int mouse_sensitivity;
extern bool mouse_reverse;
extern bool mouse_relative;
extern int mouse_autorudder;

// the abstract config file as text file in memory
class ConfigFile
{
  public:
  char buf [32000]; // max file size
  int length;
  FILE *out;
  ConfigFile ();
  ConfigFile (char *fname);
  char *skipwhite (char *str);
  char *skipnum (char *str);
  char *skipalphanum (char *str);
  char *getString (char *dest, char *str);
  int getValue (char *str);
  int openOutput (char *fname);
  int write (char *str1, int n);
  int write (char *str1, char c);
  void writeText (char *str);
  void close ();
};

// special config files of Lac: LacConfig.txt and LacControls.txt
extern void save_config ();
extern void save_saveconfig ();
extern int load_config ();
extern int load_saveconfig ();
extern void save_configInterface ();
extern int load_configInterface ();

#endif
