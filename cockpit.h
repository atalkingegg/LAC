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

/* This file includes the cockpit rendering methods. */

#ifndef IS_COCKPIT_H
#define IS_COCKPIT_H

#include "common.h" // ok
#include "model.h" // ok

class Cockpit
{
  private:
  void cockpitvertex (float phi, float gamma);

  public:
  int flarewarning, chaffwarning;
  Uint32 dt;

  Cockpit ();

  void drawBlip (int shape, float x, float y, float z, unsigned char r, unsigned char g, unsigned char b);

  void setColor (int alpha);
  void setColor (CColor *color, int alpha);
  void drawChassis ();
  void drawCounter ();
  void drawTargeter ();
  void drawCross ();
  void drawHeading ();
  void drawTargetedElement ();
  void drawWeapon ();
  void drawRadar ();
  void drawRelativeHeightBar();
  void drawThrustBar();
};

#endif

