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

/* This file includes a collection of GL features. */

#ifndef IS_GL_H
#define IS_GL_H

#include "common.h" // ok
#include "model.h" // ok

class GL
{
  private:
  GLint texnum; // highest used texture number
  int akttex; // current texture number
  CTexture *tex [200]; // support max 200 textures
  GLuint gllistnr, gllightnr; // highest used list number
  int aktlist; // current list number

  public:
  bool alphablending, zbuffer, antialiasing; // features turned on
  int shading;
  float fogcolor [3];
  float foglum;
  float frustum [6] [4]; // the six frustum (cone) planes

  GL ();
  ~GL ();
  void clearScreen ();
  void drawScreen ();
  void rotate (int x, int y, int z);
  GLuint genListSphere (GLint slices, GLint stacks, float radius);
  void setList (GLuint listnr);
  void genList (int *list);
  int genTexture ();
  void enableLinearTexture (int texnum);
  void disableLinearTexture (int texnum);
  CTexture *getTextureTGA (char *fname);
  CTexture *genTextureTGA (char *fname, int quality, int alphatype, int mipmap2, bool alpha);
  float getTexLight ();
  void enableAntiAliasing ();
  void disableAntiAliasing ();
  void enableAlphaBlending ();
  void disableAlphaBlending ();
  void enableTextures (int num);
  void enableLighting ();
  void enableZBuffer ();
  void disableZBuffer ();
  void shadeFlat ();
  void shadeSmooth ();
  void enableFog (float view); // Determines maximum visible distance
  void extractFrustum ();
  bool isPointInFrustum (float x, float y, float z);
  bool isSphereInFrustum (float x, float y, float z, float radius);
  bool isCubeInFrustum (float x, float y, float z, float size);
};

extern GL *gl;

#endif
