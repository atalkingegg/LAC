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

/* This file includes a vertex array to emulate OpenGL calls. */

#ifndef IS_VERTEXARRAY_H
#define IS_VERTEXARRAY_H

#include "common.h" // ok

#define VERTEXARRAY_V3N3C4T2 1
#define VERTEXARRAY_V3C4T2 2
#define VERTEXARRAY_V3N3T2 3
#define VERTEXARRAY_V3N3C4 4
#define VERTEXARRAY_V3C4 5

/* VertexArray class to emulate OpenGL primitives and pass 3D data in blocks to the hardware! Much faster!
   Use one VertexArray for each #define VERTEXARRAY prototype and each primitive's type (GL_QUADS, GL_TRIANGLES).
   Usage:
     VertexArray quads;
     quads.glBegin(GL_QUADS);
     quads.glVertex3f(...);
     ... ;
     quads.glEnd();
   quads.glEnd() passes the data block to OpenGL, so make sure to call quads.glEnd() ONLY AFTER painting ALL your quads!
   That means, you can add vertices using quads.glVertex() at ANY time WITHOUT glBegin()/glEnd() calls!
   For your other primitives, use different arrays:
     VertexArray quadstrip;
     VertexArray triangles; */
class VertexArray
{
  public:
  int type;    // primitive type in glBegin: GL_QUAD, GL_TRIANGLE...
  int n;       // number of vertices in this array
  int max;     // allocated number of vertices in this array
  float *data; // data block to pass to OpenGL (always float data: position, color, texture coords)
  int stride;  // size of data for one vertex in floats (V3N3C4T2 makes up 12 floats)
  int pref;    // vertex array type as given by the #define VERTEXARRAY prototypes, ONE class instance can only be of ONE type

  VertexArray ();
  VertexArray (int pref);
  ~VertexArray ();
  void setPref (int pref);
  void glBegin (int type);
  void glVertex3f (float x, float y, float z);
  void glVertex3fv (float *f);
  void glNormal3f (float x, float y, float z);
  void glColor4ub (int r, int g, int b, int a);
  void glColor3ub (int r, int g, int b);
  void glColor4f (float r, float g, float b, float a);
  void glColor3f (float r, float g, float b);
  void glColor4fv (float *f);
  void glColor3fv (float *f);
  void glTexCoord2f (float x, float y);
  void glTexCoord2fv (float *f);
  void glTexCoord2d (float x, float y);
  void reallocData ();
  void glEnd ();
};

#endif
