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

/*
*  This file is a "leftover" from the old gl-117 source code.
*  Most of its functions have been eliminated or stripped
*  down to the smallest possible component that isn't required
*  by the few remaining pilot-related components from the old
*  sim. Eventually everything here ought to be eliminated.
*  None of the logic within this entire source code file
*  is actually visible to the players, but a few underlying
*  entanglements still prevent its complete removal....
*/

#ifndef IS_PILOTS_H

#include <stdio.h>
#include <string.h>
#include "dirs.h"
#include "pilots.h"
#include "mission.h"

void Pilot::load ()
    {
    }

Pilot::Pilot (char *name)
    {
    strcpy (this->name, name);
    load ();
    }

Pilot::~Pilot ()
    {
    }

void PilotList::load (char *fname)
    {
    aktpilots = 0;
    aktpilot = aktpilots;
    pilot [aktpilot] = new Pilot ((char *)"");
    aktpilots ++;
    }

PilotList::PilotList (char *fname)
    {
    int i;
    for (i = 0; i < maxpilots; i ++)
        {
        pilot [i] = NULL;
        }
    load (fname);
    }

#endif
