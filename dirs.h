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

/* This file handles als input/output directories. */

#ifndef IS_DIRS_H
#define IS_DIRS_H

class Dirs
{
  public:
  char dir [4096];
  char saves [4096]; // path to saves, UNIX and Windows differ
  char textures [4096]; // path to textures, ...
  char music [4096];
  char sound [4096];
  char models [4096];
  char maps [4096];

  void append (char *target, char *str); // append subdir/file to path using (back-)slashes
  Dirs (char *arg);
  virtual ~Dirs ();
  char *getSaves (char *name); // get dir name containing the saves, ...
  char *getTextures (char *name);
  char *getSounds (char *name);
  char *getMusic (char *name);
  char *getModels (char *name);
  char *getMaps (char *name);
};

extern Dirs *dirs; // paths to directories with game data or saves

#endif
