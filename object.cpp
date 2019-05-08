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

/* This file includes simple objects that create models. */

#ifndef IS_OBJECT_H

#include "object.h"
#include "gl.h"

CSpaceObj::CSpaceObj ()
    {
    tl = new CVector3 ();
    rot = new CRotation ();
    zoom = 1;
    alpha = 1;
    draw = 1;
    lum = 1;
    drawlight = true;
    explode = 0;
    numRefModels = 0;
    refmodel = NULL;
    o = NULL;
    }

CSpaceObj::CSpaceObj (CModel *o, float zoom)
    {
    this->o = o;
    this->zoom = zoom;
    tl = new CVector3 ();
    rot = new CRotation ();
    alpha = 1;
    draw = 1;
    lum = 1;
    drawlight = true;
    explode = 0;
    numRefModels = 0;
    refmodel = NULL;
    }

CSpaceObj::~CSpaceObj ()
    {
    int i;

    if (refmodel)
        {
        for (i = 0; i < numRefModels; i ++)
            {
            delete refmodel [i];
            }

        delete refmodel;
        delete reftl;
        delete refrot;
        delete refscale;
        }
    delete tl;
    delete rot;
    }

void CSpaceObj::addRefModel (CModel *model, CVector3 *tl, CRotation *rot, float scale)
    {
    if (refmodel == NULL)
        {
        refmodel = new CModel *[50];
        reftl = new CVector3 [50];
        refrot = new CRotation [50];
        refscale = new float [50];
        }
    refmodel [numRefModels] = model;
    reftl [numRefModels].take (tl);
    refrot [numRefModels].take (rot);
    refscale [numRefModels] = scale;
    numRefModels ++;
    }

void CSpaceObj::translate (CVector3 *v)
    {
    tl->x = v->x;
    tl->y = v->y;
    tl->z = v->z;
    }

void CSpaceObj::translate (float x, float y, float z)
    {
    tl->x = x;
    tl->y = y;
    tl->z = z;
    }

void CSpaceObj::rotate (short a, short b, short c)
    {
    rot->setAngles (a, b, c);
    }

void CSpaceObj::rotateOn (short a, short b, short c)
    {
    rot->addAngles (a, b, c);
    }

void CSpaceObj::drawGL (CVector3 *z1, CVector3 *z2, CVector3 *tl, float alpha2, float lum2, bool drawlight2, bool istextured2)
    {
    
    int i;
    CVector3 tl1;
    tl1.x = tl->x + this->tl->x;
    tl1.y = tl->y + this->tl->y;
    tl1.z = tl->z + this->tl->z;
    if (draw == 0)
        {
        return;
        }
    if (draw == 2 || gl->isSphereInFrustum (tl1.x, tl1.y, tl1.z, this->zoom * 1.5))
        {
        if (drawlight && drawlight2)
            {
            glEnable (GL_LIGHTING);
            
            o->draw (tl, this->tl, this->rot, this->zoom, lum * lum2, explode);
            if (refmodel != NULL)
                {
                glPushMatrix ();
                glTranslatef (tl1.x, tl1.y - 0.001 * explode * explode / timestep / timestep, tl1.z);
                glRotatef (rot->c+90, 0, -1, 0);
                glRotatef (-rot->a+90, 0, 0, 1);
                glRotatef (rot->b+180, 1, 0, 0);
                glScalef (zoom, zoom, zoom);
                if (o->refpoint)
                    {
                    for (i = 0; i < numRefModels; i ++)
                        {
                        if (refscale [i] > 0.001)
                            {
                            refmodel [i]->draw (&o->refpoint [i / 3], &reftl [i], &refrot [i], refscale [i], lum * lum2, explode);
                            }
                        }
                    }
                glPopMatrix ();
                }
            }
        else
            {
            glDisable (GL_LIGHTING);
            if (istextured2)
                {
                o->draw2 (tl, this->tl, this->rot, this->zoom, explode);
                }
            else
                {
                if (drawlight2)
                    {
                    o->draw3 (tl, this->tl, this->rot, this->zoom, lum * lum2, explode);
                    }
                else
                    {
                    o->draw3 (tl, this->tl, this->rot, this->zoom, explode);
                    }
                }
            }
        }
    } 

Space::Space ()
    {
    no = 0;
    alpha = 1;
    lum = 1;
    drawlight = true;
    tl = new CVector3 ();
    z1 = new CVector3 ();
    z2 = new CVector3 ();
    }

Space::Space (CVector3 *v, CVector3 *w)
    {
    no = 0;
    alpha = 1;
    lum = 1;
    drawlight = true;
    tl = new CVector3 ();
    z1 = new CVector3 (v);
    z2 = new CVector3 (w);
    }

Space::~Space ()
    {
    delete tl;
    delete z1;
    delete z2;
    }

void Space::init ()
    {
    no = 0;
    }

bool Space::addObject (CSpaceObj *o2)
    {
    if (no < 0 || no >= maxobjperspace)
        {
        return false;
        }
    o [no] = o2;
    no ++;
    return true;
    }

bool Space::removeObject (CSpaceObj *o2)
    {
    int i, i2;
    for (i = 0; i < no; i ++)
        {
        if (o [i] == o2)
            {
            for (i2 = i + 1; i2 < no; i2 ++)
                {
                o [i2 - 1] = o [i2];
                }
            no --;
            return true;
            }
        }
    return false;
    }

void Space::translate (CVector3 *v)
    {
    tl->x = v->x;
    tl->y = v->y;
    tl->z = v->z;
    }

void Space::drawGL ()
    {
    int i;
    if (drawlight)
        {
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o != NULL)
                    {
                    if (o [i]->draw && !o [i]->o->alpha)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, true, true);
                        }
                    }
                }
            }
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o != NULL)
                    {
                    if (o [i]->draw && o [i]->o->alpha)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, true, true);
                        }
                    }
                }
            }
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o == NULL)
                    {
                    if (o [i]->draw)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, true, true);
                        }
                    }
                }
            }
        }
    else     /* not yet implemented */
        {
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o != NULL)
                    {
                    if (o [i]->draw && !o [i]->o->alpha)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, false, false);
                        }
                    }
                }
            }
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o != NULL)
                    {
                    if (o [i]->draw && o [i]->o->alpha)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, false, false);
                        }
                    }
                }
            }
        for (i = 0; i < no; i ++)
            {
            if (o [i] != NULL)
                {
                if (o [i]->o == NULL)
                    {
                    if (o [i]->draw)
                        {
                        o [i]->drawGL (z1, z2, tl, alpha, lum, false, false);
                        }
                    }
                }
            }
        }
    } 

#endif
