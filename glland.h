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

/* This file includes a wrapper for land.cpp to display the landscape. */

#ifndef IS_GLLAND_H
#define IS_GLLAND_H

#include <string.h>

#include "model.h" // ok
#include "aiobject.h" // ok
#include "effects.h" // ok
#include "land.h" // ok

#define PARTS 36 // maximum LOD parts

// exact landscape type
#define LANDSCAPE_ALPINE 0
#define LANDSCAPE_ALPINE_NOLAKE 1
#define LANDSCAPE_LOW_ALPINE 2
#define LANDSCAPE_ALPINE_EROSION 5
#define LANDSCAPE_ALPINE_SEA 6
#define LANDSCAPE_ALPINE_ROCKY 7
#define LANDSCAPE_SEA 10
#define LANDSCAPE_FLATLAND 21
#define LANDSCAPE_CANYON 30
#define LANDSCAPE_CANYON_TRENCH 31
#define LANDSCAPE_DESERT 40
#define LANDSCAPE_ARCTIC 50

// number of materials
#define MAXMATERIAL 101

// textures are loaded in main.cpp
extern CTexture *texgrass, *texrocks, *texwater, *textree, *textree2, *textree3, *texcactus1, *texredstone;
extern CTexture *textreeu, *textreeu2, *textreeu3, *textreeu4, *textreeu5, *texcactusu1;
extern CTexture *textree4, *textree5, *texearth, *texsand, *texredsand, *texgravel1;
extern CTexture *texglitter1;

class GLLandscape : public Landscape
{
  private:
  int texturetree1, texturetree2;

  public:
  float lv [3];
  /*
  * Detailed Terrain arrays:
  *
  * The following group of arrays are sized in two dimensions
  * as squares, according to the maximum terrain size. Accordingly,
  * each can be considered as a two-dimensional terrain map containing
  * different kinds of information about each point in the terrain.
  * For example, some contain color intensity information. Others
  * contain a description of the type of geometry (quad or triangle)
  * to be drawn at each point.
  */
  unsigned char r [MAXX + 1] [MAXX + 1]; // red
  unsigned char g [MAXX + 1] [MAXX + 1]; // green
  unsigned char b [MAXX + 1] [MAXX + 1]; // blue
  unsigned char drawrule [MAXX] [MAXX]; // draw triangles or quads?
  unsigned char tex1 [MAXX] [MAXX]; // texture for quad or triangle1
  unsigned char tex2 [MAXX] [MAXX]; // texture for triangle2 if needed
  unsigned char dl [MAXX + 1] [MAXX + 1]; // dynamic light mask (explosions)
  unsigned short hray [MAXX + 1] [MAXX + 1]; // height of lowest sunray
  unsigned short lg [MAXX + 1] [MAXX + 1]; // gaussian filter result
  short nl [MAXX + 2] [MAXX + 2]; // lighting mask, MAXX+1 x MAXX+1 is sufficient, but VC bug?
  /*
  * Coarse Terrain arrays:
  * The following pair of short integer arrays are also sized in two
  * dimensions, but at only one quarter of the resolution of those
  * above. The unsigned short integer stored in each entry indicates
  * the altitudes of the map at the corresponding points.
  */
  unsigned short hcmin [MAXX/4 + 1] [MAXX/4 + 1]; // height mask on a coarser grid: max{h[x+-2][y+-2]}
  unsigned short hcmax [MAXX/4 + 1] [MAXX/4 + 1]; // height mask on a coarser grid: max{h[x+-2][y+-2]}

  int lsticker; // a timer increased every time draw() is called
  int neargridstep; // landscape near grid resolution (1=fine ... 4=coarse)
  int fargridstep; // landscape far grid resolution (1=fine ... 4=coarse)
  int gridstep; // landscape raster block grid resolution (1=fine ... 4=coarse)
  float glittering; // water glittering
  bool hastowns;

  private:
  float mat [MAXMATERIAL] [4]; // materials=colors of landscape IDs
  CTexture *texmap [MAXMATERIAL]; // texture for material
  Space *space; // space of all objects
  CColor treecolor;
  int vmin [PARTS] [PARTS]; // LOD
  int vmax [PARTS] [PARTS]; // LOD
  float vh [PARTS] [PARTS]; // LOD
  bool vis [PARTS] [PARTS]; // LOD

  float lightfac; // light factor to multiply. Higher = brighter.

  void norm (float *c);
  void normalcrossproduct (float *a, float *b, float *c);

  int selectColor (int x, int y);

  public:

  void precalculate (); // precalculate everything (colors, light mask)

  float getMinHeight (float x, float z); // min height of 4x4 grid at landscape point
  float getMaxHeight (float x, float z); // min height of 4x4 grid at landscape point
  float getHeight (float x, float z); // height at landscape point

  float getExactHeight2 (float x, float z); // linear interpolation, coarse grid
  float getExactHeight3 (float x, float z); // linear interpolation, coarse grid
  float getExactHeight4 (float x, float z); // linear interpolation, coarse grid
  float getExactHeight (float x, float z); // linear interpolation

  float getExactLSHeight2 (float x, float z); // linear interpolation, coarse grid
  float getExactLSHeight3 (float x, float z); // linear interpolation, coarse grid
  float getExactLSHeight4 (float x, float z); // linear interpolation, coarse grid
  float getExactLSHeight (float x, float z); // linear interpolation

  float getRayHeight (float x, float z); // height of lowest sun ray

  float getExactRayHeight (float x, float z); // linear interpolation

  void drawTree (float x, float y, float htree, float wtree, int phi); // two in one
  void drawTreeQuad (int x, int y, int phi, bool hq);
  void drawTown (int x, int y);

  int detail [PARTS] [PARTS]; // LOD

  void drawQuadStrip (int x1, int y1, int x2, int y2); // fast quad strip without textures
  void drawQuad (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  void drawTriangle (int x1, int y1, int x2, int y2, int x3, int y3);
  void drawTexturedQuad (int x, int y);
  void drawTexturedTriangle (int x1, int y1, int x2, int y2, int x3, int y3);
  void drawTexturedQuad (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  void drawWaterTexturedQuad (int x, int y);
  void drawTexturedTriangle1 (int x, int y);
  void drawTexturedTriangle2 (int x, int y);

  void viewculling ();
  void draw (int phi, int gamma); // draw ALL

  void calcDynamicLight (CExplosion **explo, DynamicObj **cannon, DynamicObj **missile, DynamicObj **flare); // explosions light up the surface

  void setMaterial (int n, float r, float g, float b, CTexture *tex);

  GLLandscape (Space *space2, int type, int *heightmask);
};

extern GLLandscape *l;

#endif
