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

/* This file includes all special effects. */

#ifndef IS_EFFECTS_H
#define IS_EFFECTS_H

#include "model.h" // ok
#include "object.h" // ok
#include "mathtab.h" // ok

extern CTexture *texsmoke, *texsmoke2, *texsmoke3;

// Smoke of missiles and fighters
class CSmoke
{
  public:
  CVector3 v [MAXSMOKEELEM];
  float zoom [MAXSMOKEELEM];
  int time [MAXSMOKEELEM];
  int phi [MAXSMOKEELEM]; // player's view angle (for low quality smoke), obsolete since v0.8.5
  int last;
  int type;

  CSmoke (int type);
  virtual ~CSmoke ();
  void setSmoke (float x, float y, float z, int myphi, int mytime);
  void move (Uint32 dt, int dec);
  void drawElem (int n);
  void drawElemHQ (int n);
  void draw ();
};

// Flash of a thunderstorm
class Flash
{
  public:
  int time;
  int phi; // player's view angle
  CVector3 w [10];
  Flash ();
  virtual ~Flash ();
  void set (float x, float y, float z, int phi);
  void draw ();
  void drawHQ ();
};

// Explosion balls
class CExplosion : public CSpaceObj
{
  public:
  Space *space;
  int ttl, maxlen;
  float maxzoom;
  CVector3 v;

  CExplosion (Space *space, CModel *sphere);
  virtual ~CExplosion () {}
  void setExplosion (float x, float y, float z, float vx, float vy, float vz, float maxzoom, int len);
  void move (Uint32 dt);
  // drawGL() of CSpaceObj
};

// Dark smoke after explosions
class CBlackSmoke : public CSpaceObj
{
  public:
  Space *space;
  int ttl, maxlen;
  float maxzoom;
  float myphi; // player's view angle, obsolete since v0.8.5

  CBlackSmoke (Space *space);
  virtual ~CBlackSmoke () {}
  void setBlackSmoke (float x, float y, float z, float myphi, float maxzoom, int len);
  void move (Uint32 dt);
  virtual void drawGL (CVector3 *z1, CVector3 *z2, CVector3 *tl, float alpha2, float lum2, bool drawlight2, bool istextured2);
};

// Stars at night
class Star
{
  public:
  int phi, gamma; // polar (sphere) coords
  float size;

  Star (int phi, int gamma, float size);
  virtual ~Star () {}
  void draw ();
};

// Font class to create fonts out of bitmaps
class Font
{
  public:
  CTexture *texture; // the font texture
  char start; // starting character
  int height; // fixed line height in the texture (should be maximum letter height)
  int n; // number of letters
  float zoom; // zoom factor for output
  short letterx [256]; // starting x coordinate in the texture
  short lettery [256]; // starting y coordinate in the texture
  short letterw [256]; // width in the texture
  CColor *stdcol;
  CColor *highlightcol;

  Font (char *filename, int height, char start, int num); // new Font
  virtual ~Font ();

  bool isPixel (int x, int y); // is pixel set in the texture?
  void extractLetters (int height, char start, int num); // extract the letters

  // some custom OpenGL output methods
  void drawText (float x, float y, float z, char *str, CColor *c, bool centered, int highlight, CColor *highlightcol);
  void drawText (float x, float y, float z, char *str, CColor *c, int highlight, CColor *highlightcol);
  void drawText (float x, float y, float z, char *str, CColor *c, int highlight);
  void drawText (float x, float y, float z, char *str, CColor *c);
  void drawText (float x, float y, float z, char *str, int highlight, CColor *highlightcol);
  void drawText (float x, float y, float z, char *str, int highlight);
  void drawText (float x, float y, float z, char *str);
  void drawTextCentered (float x, float y, float z, char *str, CColor *c, int highlight, CColor *highlightcol);
  void drawTextCentered (float x, float y, float z, char *str, CColor *c, int highlight);
  void drawTextCentered (float x, float y, float z, char *str, CColor *c);
  void drawTextCentered (float x, float y, float z, char *str, int highlight, CColor *highlightcol);
  void drawTextCentered (float x, float y, float z, char *str, int highlight);
  void drawTextCentered (float x, float y, float z, char *str);
  void drawTextRotated (float x, float y, float z, char *str, CColor *color, int timer);
  void drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer, int highlight, CColor *highlightcol);
  void drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer, int highlight);
  void drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer);
  void drawTextScaled (float x, float y, float z, char *str, int timer, int highlight, CColor *highlightcol);
  void drawTextScaled (float x, float y, float z, char *str, int timer, int highlight);
  void drawTextScaled (float x, float y, float z, char *str, int timer);
};

class HighClouds : public CSpaceObj
{
  public:
  CSpherePart *sphere;

  HighClouds (int theta);
  virtual ~HighClouds ();
  void init (int theta);
  void setTexture (CTexture *texture);
  void drawGL (CVector3 *tl, CVector3 *textl);
};

#endif
