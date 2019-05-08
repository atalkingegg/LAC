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

/* This file includes a collection of functions and precalculated tables. */

#ifndef IS_MATHTAB_H

#include <math.h>
#include "mathtab.h"
#include "common.h"

int randptr = 0;

int randommaster [64] [64];

float PI;

float sine [360];
float cosi [360];
float smokezoom [MAXSMOKEELEM];

float dist (float dx, float dy)
    {
    return (float) sqrt (dx*dx + dy*dy);
    }

// return random number, but prefer extremely high and low values
int extremerandom (int n)
    {
    int ret;
    if (n == 0)
        {
        return 0;
        }
    ret = rand () % n;
    randptr ++;
    if (randptr >= 64)
        {
        randptr = 0;
        }
    ret = randommaster [randptr] [randptr] % n;
    if ((ret % 5) <= 4)
        {
        if (ret > n/2 && ret < 3*n/4)
            {
            return ret + n/4;
            }
        else if (ret < n/2 && ret > n/4)
            {
            return ret - n/4;
            }
        }
    return ret;
    }

void mathtab_init ()
    {
    int i, i2;
    PI = (float) (atan (1.0) * 4.0);

    for (i = 0; i < 360; i ++)
        {
        sine [i] = (float) sin ((float) i / 180.0 * PI);
        cosi [i] = (float) cos ((float) i / 180.0 * PI);
        }
    for (i = 0; i < 63; i ++)
        for (i2 = 0; i2 < 63; i2 ++)
            {
            randommaster [i] [i2] = (i * i2 * 2000) % 32678;
            }
    for (i = 0; i < MAXSMOKEELEM; i ++)
        {
        smokezoom [i] = (2.0 - (1.9 * i / MAXSMOKEELEM)) * 0.1;
        }
    }

// return random number
int myrandom (int n)
    {
    if (n == 0)
        {
        return 0;
        }
        return rand () % n;
    }

int myrandom (int n, int x, int y)
    {
    int ret;
    if (n == 0)
        {
        return 0;
        }
        ret = rand () % n;
    if ((ret % 5) <= 4)
        {
        if (ret > n/2 && ret < 3*n/4)
            {
            return ret + n/4;
            }
        else if (ret < n/2 && ret > n/4)
            {
            return ret - n/4;
            }
        }
    return ret;
    }
#endif
