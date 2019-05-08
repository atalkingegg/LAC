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

/* This file includes a wavefront obj loader. */

#ifndef IS_OBJLOAD_H
#define IS_OBJLOAD_H

#define int32 int

#include "model.h" // ok

class CFile
{
  public:
  FILE *in;
  int32 size;
  int32 filepointer;
  char *data;

  CFile (char *filename);
  ~CFile ();
  float readFloat (int32 offset);
  float readFloat ();
  int readInteger (int32 offset);
  int readInteger ();
  char *readWord (int32 offset);
  char *readWord ();
  char *getChar ();
  void nextWhite ();
  void skipWhite ();
};

// CLoad3DS handles all of the loading code (see NEHE's OpenGL tutorials)
class CLoadOBJ
{
  public:
  CLoadOBJ ();
  bool ImportOBJ (CModel *model, char *filename);

  private:
  CFile *file;
  void ComputeNormals (CModel *model);
  void ComputeColors (CModel *model);
  void Normalize (CModel *model);
  void LoadObject (CModel *model);
};

#endif
