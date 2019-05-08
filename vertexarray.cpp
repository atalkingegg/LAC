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

#include "vertexarray.h"

VertexArray::VertexArray ()
    {
    max = 0;
    n = 0;
    stride = 12;
    pref = VERTEXARRAY_V3C4T2;
    data = NULL;
    }

VertexArray::VertexArray (int pref)
    {
    max = 0;
    n = 0;
    stride = 12;
    this->pref = pref;
    data = NULL;
    }

VertexArray::~VertexArray ()
    {
    if (max > 0)
        {
        free (data);
        }
    }

void VertexArray::setPref (int pref)
    {
    this->pref = pref;
    }

void VertexArray::glBegin (int type)
    {
    n = 0;
    this->type = type;
    }

void VertexArray::glVertex3f (float x, float y, float z)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 0] = x;
    data [stride * n + 1] = y;
    data [stride * n + 2] = z;
    n ++;
    }

void VertexArray::glVertex3fv (float *f)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 0] = f [0];
    data [stride * n + 1] = f [1];
    data [stride * n + 2] = f [2];
    n ++;
    }

void VertexArray::glNormal3f (float x, float y, float z)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 3] = x;
    data [stride * n + 4] = y;
    data [stride * n + 5] = z;
    }

void VertexArray::glColor4ub (int r, int g, int b, int a)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 6] = (float) r / 255.0F;
    data [stride * n + 7] = (float) g / 255.0F;
    data [stride * n + 8] = (float) b / 255.0F;
    data [stride * n + 9] = (float) a / 255.0F;
    }

void VertexArray::glColor3ub (int r, int g, int b)
    {
    glColor4ub (r, g, b, 255);
    }

void VertexArray::glColor4f (float r, float g, float b, float a)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 6] = r;
    data [stride * n + 7] = g;
    data [stride * n + 8] = b;
    data [stride * n + 9] = a;
    }

void VertexArray::glColor3f (float r, float g, float b)
    {
    glColor4f (r, g, b, 255);
    }

void VertexArray::glColor4fv (float *f)
    {
    glColor4f (f [0], f [1], f [2], f [3]);
    }

void VertexArray::glColor3fv (float *f)
    {
    glColor4f (f [0], f [1], f [2], 1);
    }

void VertexArray::glTexCoord2f (float x, float y)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 10] = x;
    data [stride * n + 11] = y;
    }

void VertexArray::glTexCoord2fv (float *f)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 10] = f [0];
    data [stride * n + 11] = f [1];
    }

void VertexArray::glTexCoord2d (float x, float y)
    {
    if (n >= max)
        {
        reallocData ();
        }
    data [stride * n + 10] = x;
    data [stride * n + 11] = y;
    }

void VertexArray::reallocData ()
    {
    while (n >= max)
        {
        if (max == 0)
            {
            max = 32;
            }
        else
            {
            max *= 2;
            }
        data = (float *) realloc (data, max * stride * sizeof (float));
        if (data == NULL)
            {
            display ((char *)"Out of memory", LOG_FATAL);
            }
        }
    }

void VertexArray::glEnd ()
    {
    if (n == 0)
        {
        return;
        }
    glEnableClientState (GL_VERTEX_ARRAY);
    if (pref != VERTEXARRAY_V3C4T2 && pref != VERTEXARRAY_V3C4)
        {
        glEnableClientState (GL_NORMAL_ARRAY);
        }
    else
        {
        glDisableClientState (GL_NORMAL_ARRAY);
        }
    if (pref != VERTEXARRAY_V3N3T2)
        {
        glEnableClientState (GL_COLOR_ARRAY);
        }
    else
        {
        glDisableClientState (GL_COLOR_ARRAY);
        }

    if (pref != VERTEXARRAY_V3N3C4 && pref != VERTEXARRAY_V3C4)
        {
        glEnableClientState (GL_TEXTURE_COORD_ARRAY);
        }
    else
        {
        glDisableClientState (GL_TEXTURE_COORD_ARRAY);
        }
    glVertexPointer (3,	GL_FLOAT,	sizeof (float) * stride, &data [0]);
    glNormalPointer (GL_FLOAT, sizeof (float) * stride, &data [3]);
    glColorPointer (4, GL_FLOAT, sizeof (float) * stride, &data [6]);
    glTexCoordPointer (2, GL_FLOAT, sizeof (float) * stride, &data [10]);
    glDrawArrays (type, 0, n);
    }
#endif

