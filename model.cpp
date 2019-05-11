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

/* This file includes the memory representation of any 3D model.
   For a detailed description of the data structure look at model.h */

//##################################################################################################
// Headers needed only by model.cpp
//##################################################################################################
#include "model.h" /* CColor */
#include <string.h> /* memset */
#include "loader_tga.h" /* tga_load */
#include <math.h> /* sqrt, M_PI */
#include <stdlib.h> /* exit */
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>  /* GL_LINES */
#include "gl.h" /* gl, depends on SDL_opengl.h */
#include "main.h" /* MissionActive */

float sintab [360], costab [360];

CColor::CColor ()
    {
    memset (c, 255, 4 * sizeof (unsigned char));
    }

CColor::CColor (CColor *col)
    {
    memcpy (c, col->c, 4 * sizeof (unsigned char));
    }

CColor::CColor (short cr, short cg, short cb)
    {
    c [0] = cr;
    c [1] = cg;
    c [2] = cb;
    c [3] = 255;
    }

CColor::CColor (short cr, short cg, short cb, short ca)
    {
    c [0] = cr;
    c [1] = cg;
    c [2] = cb;
    c [3] = ca;
    }

CColor::~CColor () {}

void CColor::setColor (CColor *col)
    {
    memcpy (c, col->c, 4 * sizeof (unsigned char));
    }

void CColor::setColor (short cr, short cg, short cb, short ca)
    {
    c [0] = cr;
    c [1] = cg;
    c [2] = cb;
    c [3] = ca;
    }

void CColor::setColor (short cr, short cg, short cb)
    {
    c [0] = cr;
    c [1] = cg;
    c [2] = cb;
    c [3] = 255;
    }

bool CColor::isEqual (CColor *col)
    {
    return memcmp (c, col->c, 4 * sizeof (unsigned char)) == 0;
    }

void CColor::take (CColor *col)
    {
    memcpy (c, col->c, 4 * sizeof (unsigned char));
    }

CTexture::CTexture ()
    {
    texlight = 1.0F;
    width = 0;
    height = 0;
    textureID = -1;
    data = NULL;
    texred = 1.0F;
    texgreen = 1.0F;
    texblue = 1.0F;
    mipmap = true;
    quality = 0;
    }

CTexture::~CTexture ()
    {
    if (data != NULL)
        {
        delete data;
        }
    }

int CTexture::loadFromTGA (char *fname, int quality, int alphatype, int mipmap) // only 24 bit TGA
    {
    int i, i2;
    data = tga_load (fname, &width, &height); // global 32 bpp texture buffer

    if (!data)
        {
        return 0;
        }
    long texl = 0;
    long texr = 0;
    long texg = 0;
    long texb = 0;

    for (i = 0; i < height; i ++)
        {
        for (i2 = 0; i2 < width; i2 ++)
            {
            int n2 = (i * width + i2)*4;
            texl += (int) data [n2+2] + data [n2+1] + data [n2];
            texr += (int) data [n2];
            texg += (int) data [n2 + 1];
            texb += (int) data [n2 + 2];
            if (alphatype == 0)   // alpha=255 or 0
                {
                if (data [n2+0] + data [n2+1] + data [n2+2] < 30)
                    {
                    data [n2+3] = 0;
                    }
                else
                    {
                    data [n2+3] = 255;
                    }
                }
            else if (alphatype == 1)     // alpha=maxcolor
                {
                int max = (data [n2+0] > data [n2+1] ? data [n2+0] : data [n2+1]);
                max = (max > data [n2+2] ? max : data [n2+2]);
                data [n2+3] = max;
                }
            else if (alphatype == 2)     // alpha=red
                {
                data [n2+3] = data [n2];
                }
            else if (alphatype == 3)     // alpha=midcolor*6 or 0
                {
                int test = (data [n2] + data [n2 + 1] + data [n2 + 2]) * 2;

                if (test > 255)
                    {
                    test = 255;
                    }
                else if (test < 30)
                    {
                    test = 0;
                    }
                data [n2+3] = (unsigned char) test;
                }
            else if (alphatype == 4)     // alpha=red, color=white
                {
                data [n2+3] = data [n2+0];
                data [n2+0] = 255;
                data [n2+1] = 255;
                data [n2+2] = 255;
                }
            else if (alphatype == 5)     // alpha=red*2, color=black
                {
                int test = data [n2+0] * 2;
                if (test > 255)
                    {
                    test = 255;
                    }
                data [n2+3] = test;
                data [n2+0] = 0;
                data [n2+1] = 0;
                data [n2+2] = 0;
                }
            else if (alphatype == 6)     // alpha=red, color=black
                {
                data [n2+3] = data [n2+0];
                data [n2+0] = 0;
                data [n2+1] = 0;
                data [n2+2] = 0;
                }
            }
        }
    texlight = (float) texl / width / height / 3 / 256; // average brightness
    texred = (float) texr / width / height / 256; // average red
    texgreen = (float) texg / width / height / 256; // average green
    texblue = (float) texb / width / height / 256; // average blue
    strcpy (name, fname);
    this->quality = quality;
    this->mipmap = (mipmap != 0);
    return 1;
    }

void CTexture::getColor (CColor *c, int x, int y)
    {
    if (x < 0)
        {
        x = (int) -x % width;
        }
    if (y < 0)
        {
        y = (int) -y % height;
        }
    if (x >= width)
        {
        x = (int) x % width;
        }
    if (y >= height)
        {
        y = (int) y % height;
        }
    int offs = y * width + x;
    offs <<= 2;
    c->c [0] = data [offs];
    c->c [1] = data [offs + 1];
    c->c [2] = data [offs + 2];
    c->c [3] = data [offs + 3];
    }

CVector3::CVector3 ()
    {
    x = y = z = 0;
    }

CVector3::CVector3 (float x, float y, float z)
    {
    this->x = x;
    this->y = y;
    this->z = z;
    }

CVector3::CVector3 (CVector3 *v)
    {
    x = v->x;
    y = v->y;
    z = v->z;
    }

void CVector3::set (float x, float y, float z)
    {
    this->x = x;
    this->y = y;
    this->z = z;
    }

void CVector3::neg ()
    {
    x = -x;
    y = -y;
    z = -z;
    }

void CVector3::add (CVector3 *v)
    {
    x += v->x;
    y += v->y;
    z += v->z;
    }

void CVector3::sub (CVector3 *v)
    {
    x -= v->x;
    y -= v->y;
    z -= v->z;
    }

void CVector3::mul (float fac)
    {
    x *= fac;
    y *= fac;
    z *= fac;
    }

void CVector3::crossproduct (CVector3 *v)
    {
    float nx = y * v->z - z * v->y;
    float ny = z * v->x - x * v->z;
    float nz = x * v->y - y * v->x;
    x = nx;
    y = ny;
    z = nz;
    }

float CVector3::dotproduct (CVector3 *v)
    {
    return x * v->x + y * v->y + z * v->z;
    }

float CVector3::length ()
    {
    return (float) sqrt (x * x + y * y + z * z);
    }

void CVector3::norm ()
    {
    float d = sqrt (x * x + y * y + z * z);

    if (d == 0)
        {
        d = 1E-10;
        }
    x /= d;
    y /= d;
    z /= d;
    }

bool CVector3::isEqual (CVector3 *v)
    {
    return x == v->x && y == v->y && z == v->z;
    }

bool CVector3::isEqual (CVector3 *v, float tol)
    {
    return x >= v->x - tol && x <= v->x + tol &&
           y >= v->y - tol && y <= v->y + tol &&
           z >= v->z - tol && z <= v->z + tol;
    }

void CVector3::take (CVector3 *v)
    {
    x = v->x;
    y = v->y;
    z = v->z;
    }

bool CVector2::isEqual (CVector2 *v)
    {
    return x == v->x && y == v->y;
    }

bool CVector2::isEqual (CVector2 *v, float tol)
    {
    return x >= v->x - tol && x <= v->x + tol &&
           y >= v->y - tol && y <= v->y + tol;
    }

void CVector2::take (CVector2 *v)
    {
    x = v->x;
    y = v->y;
    }

CVertex::CVertex ()
    {
    triangles = 0;
    }

CVertex::CVertex (CVertex *v)
    {
    take (v);
    }

void CVertex::addNormal (CVector3 *n)
    {
    triangles ++;
    normal.x = (normal.x * (triangles - 1) + n->x) / (float) triangles;
    normal.y = (normal.y * (triangles - 1) + n->y) / (float) triangles;
    normal.z = (normal.z * (triangles - 1) + n->z) / (float) triangles;
    }

void CVertex::addColor (CColor *c)
    {
    triangles ++;

    for (int i = 0; i < 4; i ++)
        {
        color.c [i] = (unsigned char) (((float) color.c [i] * (triangles - 1) + c->c [i]) / (float) triangles);
        }
    }

void CVertex::take (CVertex *v)
    {
    vector.take (&v->vector);
    normal.take (&v->normal);
    color.take (&v->color);
    triangles = v->triangles;
    }

CRotation::CRotation ()
    {
    a = b = c = 0;
    calcRotation ();

    for (int i = 0; i < 360; i ++)
        {
        sintab [i] = sin (M_PI / 180 * i);
        costab [i] = cos (M_PI / 180 * i);
        }
    }

CRotation::~CRotation () {}

void CRotation::setAngles (short a, short b, short c)
    {
    a %= 360;

    if (a < 0)
        {
        a += 360;
        }
    b %= 360;
    if (b < 0)
        {
        b += 360;
        }
    c %= 360;
    if (c < 0)
        {
        c += 360;
        }
    this->a = a;
    this->b = b;
    this->c = c;
    }

void CRotation::addAngles (short a, short b, short c)
    {
    this->a += a;
    this->b += b;
    this->c += c;
    this->a %= 360;
    if (this->a < 0)
        {
        this->a += 360;
        }
    this->b %= 360;
    if (this->b < 0)
        {
        this->b += 360;
        }
    this->c %= 360;
    if (this->c < 0)
        {
        this->c += 360;
        }
    }

void CRotation::calcRotation ()
    {
    rot [0] [0] = costab [c] * costab [b];
    rot [0] [1] = sintab [a] * sintab [b] * costab [c] - sintab [c] * costab [a];
    rot [0] [2] = sintab [a] * sintab [c] + costab [a] * sintab [b] * costab [c];
    rot [1] [0] = sintab [c] * costab [b];
    rot [1] [1] = costab [c] * costab [a] + sintab [a] * sintab [b] * sintab [c];
    rot [1] [2] = costab [a] * sintab [b] * sintab [c] - sintab [a] * costab [c];
    rot [2] [0] = -sintab [b];
    rot [2] [1] = sintab [a] * costab [b];
    rot [2] [2] = costab [a] * costab [b];
    }

float CRotation::rotateX (CVector3 *v)
    {
    return v->x * rot [0] [0] + v->y * rot [0] [1] + v->z * rot [0] [2];
    }

float CRotation::rotateY (CVector3 *v)
    {
    return v->x * rot [1] [0] + v->y * rot [1] [1] + v->z * rot [1] [2];
    }

float CRotation::rotateZ (CVector3 *v)
    {
    return v->x * rot [2] [0] + v->y * rot [2] [1] + v->z * rot [2] [2];
    }

float CRotation::getsintab (int a)
    {
    if (a >= 0 && a < 360)
        {
        return sintab [a];
        }
    return 0;
    }

float CRotation::getcostabntab (int a)
    {
    if (a >= 0 && a < 360)
        {
        return costab [a];
        }
    return 0;
    }

void CRotation::take (CRotation *r)
    {
    a = r->a;
    b = r->b;
    c = r->c;
    }

void CTriangle::getNormal (CVector3 *n)
    {
    CVector3 dummy;
    n->take (&v [1]->vector);
    n->sub (&v [0]->vector);
    dummy.take (&v [2]->vector);
    dummy.sub (&v [0]->vector);
    n->crossproduct (&dummy);
    }

void CTriangle::setVertices (CVertex *a, CVertex *b, CVertex *c)
    {
    int i;
    CVector3 dummy;
    v [0] = a;
    v [1] = b;
    v [2] = c;
    getNormal (&dummy);
    dummy.norm ();
    if (dummy.z > 0)
        {
        dummy.neg ();
        }
    for (i = 0; i < 3; i ++)
        {
        v [i]->addNormal (&dummy);
        }
    }

void CQuad::getNormal (CVector3 *n)
    {
    CVector3 dummy;
    n->take (&v [1]->vector);
    n->sub (&v [0]->vector);
    dummy.take (&v [3]->vector);
    dummy.sub (&v [0]->vector);
    n->crossproduct (&dummy);
    }

void CQuad::setVertices (CVertex *a, CVertex *b, CVertex *c, CVertex *d)
    {
    int i;
    CVector3 dummy;
    v [0] = a;
    v [1] = b;
    v [2] = c;
    v [3] = d;
    getNormal (&dummy);
    dummy.norm ();
    if (dummy.z > 0)
        {
        dummy.neg ();
        }
    for (i = 0; i < 4; i ++)
        {
        v [i]->addNormal (&dummy);
        }
    }

CMaterial::CMaterial ()
    {
    uscale = 1;
    vscale = 1;
    uoffset = 0;
    voffset = 0;
    wrot = 0;
    char tmp [255] = {0};
    strncpy (filename, tmp, 255);
    strncpy (name, tmp, 255);
    }

CObject::CObject ()
    {
    numVertices = 0;
    numTriangles = 0;
    numQuads = 0;
    numTexVertex = 0;
    material = NULL;
    hasTexture = false;
    }

CObject::~CObject ()
    {
    }

int CObject::addVertex (CVertex *w)
    {
    int i;

    for (i = 0; i < numVertices; i ++)
        if (w->vector.isEqual (&vertex [i].vector, 1e-3F) && w->color.isEqual (&vertex [i].color))
            {
            break;
            }
    if (i == numVertices)
        {
        vertex [numVertices ++].take (w);
        }
    return i;
    }

void CObject::setColor (CColor *col)
    {
    int i;

    for (i = 0; i < numVertices; i ++)
        {
        memcpy (vertex [i].color.c, col, 4 * sizeof (unsigned char));
        }
    }

CModel::CModel ()
    {
    numObjects = 0;
    numMaterials = 0;
    shading = 0;
    displaylist = true;
    list1 = -1;
    list2 = -1;
    list3 = -1;
    scale = 1.0F;
    name [0] = '0';
    nolight = false;
    alpha = false;
    light_ambient [0] = 0.3;
    light_ambient [1] = 0.3;
    light_ambient [2] = 0.3;
    light_ambient [3] = 1;
    light_diffuse [0] = 0.9;
    light_diffuse [1] = 0.9;
    light_diffuse [2] = 0.9;
    light_diffuse [3] = 1;
    light_ambient2 [0] = 0.2;
    light_ambient2 [1] = 0.2;
    light_ambient2 [2] = 0.2;
    light_ambient2 [3] = 1;
    light_diffuse2 [0] = 0.1;
    light_diffuse2 [1] = 0.1;
    light_diffuse2 [2] = 0.1;
    light_diffuse [3] = 1;
    numRefpoints = 0;
    refpoint = NULL;
    va = new VertexArray (VERTEXARRAY_V3N3C4T2);
    }

void CModel::setName (char *name)
    {
    strcpy (this->name, name);
    }

void CModel::addMaterial (CMaterial *material)
    {
    this->material [numMaterials] = new CMaterial;

    if (this->material [numMaterials] == NULL)
        {
        exit (100);
        }
    if (material != NULL)
        {
        memcpy (this->material [numMaterials], material, sizeof (CMaterial));
        }
    numMaterials ++;
    }

void CModel::addObject (CObject *object)
    {
    this->object [numObjects] = new CObject;

    if (this->object [numObjects] == NULL)
        {
        exit (101);
        }
    if (object != NULL)
        {
        memcpy (this->object [numObjects], object, sizeof (CObject));
        }
    numObjects ++;
    rotcol = 0;
    }

void CModel::addRefPoint (CVector3 *tl)
    {
    int i, i2;

    if (refpoint == NULL)
        {
        refpoint = new CVector3 [10];
        }
    for (i = 0; i < numRefpoints; i ++)
        {
        if (tl->z < refpoint [i].z)
            {
            for (i2 = numRefpoints; i2 > i; i2 --)
                {
                refpoint [i2].take (&refpoint [i2 - 1]);
                }
            refpoint [i].take (tl);
            goto AddRef1;
            }
        }
    refpoint [numRefpoints].take (tl);
AddRef1:
    numRefpoints ++;
    }

CModel::~CModel ()
    {
    int i;

    for (i = 0; i < numMaterials; i ++)
        {
        delete material [i];
        }
    for (i = 0; i < numObjects; i ++)
        {
        delete object [i];
        }
    if (refpoint)
        {
        delete refpoint;
        }
    }

void CModel::setColor (CColor *col)
    {
    int i;

    for (i = 0; i < numObjects; i++)
        {
        object [i]->setColor (col);
        }
    }

void CModel::drawVertexNormals (CObject *cm, float zoom)
    {
    glColor3ub (255, 0, 0);
    glBegin (GL_LINES);

    for (int j = 0; j < cm->numVertices; j++)
        if (cm->vertex [j].triangles > 0)
            {
            glVertex3f (cm->vertex [j].vector.x*zoom, cm->vertex [j].vector.y*zoom, cm->vertex [j].vector.z*zoom);
            glVertex3f ((cm->vertex [j].vector.x + cm->vertex [j].normal.x / 5)*zoom, (cm->vertex [j].vector.y + cm->vertex [j].normal.y / 5)*zoom, (cm->vertex [j].vector.z + cm->vertex [j].normal.z / 5)*zoom);
            }
    glEnd ();
    }

int CModel::rotateColor (int n)
    {
    if (n == 0)
        {
        return 0;
        }
    rotcol ++;
    if (rotcol > n)
        {
        rotcol = 0;
        }
    return rotcol;
    }

void CModel::scaleTexture (float fx, float fy)
    {
    int i;

    for (i = 0; i < numObjects; i ++)
        {
        CObject *o = object [i];
        int i2;
        for (i2 = 0; i2 < o->numVertices; i2 ++)
            {
            o->vertex [i2].tex.x *= fx;
            o->vertex [i2].tex.y *= fy;
            }
        }
    }

void CModel::draw (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, float lum, int explode)
    {
    
    if (MissionActive && ((abs(tl2->x - camx) > 100) || (abs(tl2->y - camy) > 100) || (abs(tl2->z - camz) > 100)))
       { 
       return;
       }
    
    if (nolight)   // if model wants to be rendered without light, call draw2
        {
        glDisable (GL_LIGHTING);
        draw2 (tl, tl2, rot, zoom, explode);
        glEnable (GL_LIGHTING);
        return;
        }
    if (tl == NULL)
        {
        tl = &tlnull;
        }
    if (tl2 == NULL)
        {
        tl2 = &tlnull;
        }
    if (rot == NULL)
        {
        rot = &rotnull;
        }
    int i, j;
    CObject *cm;
    float la [4] = { 0.2, 0.2, 0.2, 1.0};
    if (lum >= 1)
        {
        float addl = lum;

        if (addl >= 5)
            {
            addl = 5;
            }
        la [0] = 0.2 * addl;
        la [1] = 0.2 * addl;
        la [2] = 0.2 * addl;
        }
    glLightfv (GL_LIGHT0, GL_AMBIENT, la);
    float ld [4] = { 0.7, 0.7, 0.7, 1.0};
    if (lum != 1)
        {
        ld [0] *= lum;
        ld [1] *= lum;
        ld [2] *= lum;
        if (ld [0] > 1.0)
            {
            ld [0] = 1.0;
            }
        if (ld [1] > 1.0)
            {
            ld [1] = 1.0;
            }
        if (ld [2] > 1.0)
            {
            ld [2] = 1.0;
            }
        }
    glLightfv (GL_LIGHT0, GL_DIFFUSE, ld);
    for (i = 0; i < numObjects; i ++)
        {
        if (numObjects <= 0)
            {
            break;
            }
        cm = object [i];
        if (cm->hasTexture)
            {
            if (antialiasing)
                {
                gl->enableLinearTexture (cm->material->texture->textureID);
                }
            else
                {
                gl->disableLinearTexture (cm->material->texture->textureID);
                }
            }
        }
    zoom *= scale;
    glPushMatrix ();
    glTranslatef (tl->x + tl2->x, tl->y + tl2->y - 0.002 * explode * explode / timestep / timestep, tl->z + tl2->z);
    float explodefac = (float) explode / 10 / timestep;
    if (showcollision)
        {
        glPushMatrix ();
        glScalef (cubex, cubey, cubez);
        glColor3ub (255, 0, 0);
        glBegin (GL_LINE_STRIP);
        glVertex3f (1, 1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (1, 1, 1);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex3f (-1, 1, 1);
        glVertex3f (-1, 1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (-1, -1, 1);
        glVertex3f (-1, 1, 1);
        glEnd ();
        glBegin (GL_LINES);
        glVertex3f (1, 1, 1);
        glVertex3f (-1, 1, 1);
        glVertex3f (1, -1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (-1, -1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (-1, 1, -1);
        glEnd ();
        glPopMatrix ();
        }
    glRotatef (rot->c+90, 0, -1, 0);
    glRotatef (-rot->a+90, 0, 0, 1);
    glRotatef (rot->b+180, 1, 0, 0);
    glScalef (zoom, zoom, zoom);
    if (shading == 1)
        {
        glShadeModel (GL_FLAT);
        }
    else
        {
        glShadeModel (GL_SMOOTH);
        }
    if (alpha)
        {
        glEnable (GL_BLEND);
        glEnable (GL_ALPHA_TEST);
        glAlphaFunc (GL_GEQUAL, 0.2);
        }
    
    for (i = 0; i < numObjects; i ++) // was numObjects
        { 
        if (numObjects <= 0)
            {
            break;
            }
        cm = object [i];
        if (cm->hasTexture)
            {
            glEnable (GL_TEXTURE_2D);
            glColor4f (1, 1, 1, 1);
            glBindTexture (GL_TEXTURE_2D, cm->material->texture->textureID);
            }
        else
            {
            glDisable (GL_TEXTURE_2D);
            glColor4f (1, 1, 1, 1);
            }
        if (cm->material != NULL)
            {
            if (cm->material->color.c [0] > 190 && cm->material->color.c [1] > 190 && cm->material->color.c [2] < 20)
                {
                glDisable (GL_LIGHTING);
                }
            else
                {
                glEnable (GL_LIGHTING);
                }
            }
        CVector3 shift;
        va->glBegin (GL_TRIANGLES);
        for (j = 0; j < cm->numTriangles; j++)
            {
            CVertex *v = cm->triangle [j].v [0];
            if (explode > 0)
                {
                shift.x = v->normal.x * explodefac;
                shift.y = v->normal.y * explodefac;
                shift.z = v->normal.z * explodefac;
                }
            for (int whichVertex = 0; whichVertex < 3; whichVertex ++)
                { 
                v = cm->triangle [j].v [whichVertex];
                va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                if (cm->hasTexture)
                    {
                    if (cm->vertex)
                        {
                        va->glTexCoord2f (v->tex.x, v->tex.y);
                        va->glColor4f (1, 1, 1, 1);
                        }
                    }
                else
                    {
                    if (numMaterials/* && cm->material->textureID >= 0*/)
                        {
                        unsigned char *color = cm->material->color.c; /*material[cm->material->textureID]->color.c;*/
                        if (color [0] > 190 && color [1] > 190 && color [2] < 20)
                            {
                            rotateColor (30);
                            va->glColor4ub (color [0] + rotcol, color [1] + rotcol, color [2] + rotcol * 3, 255);
                            }
                        else
                            {
                            va->glColor4ub (color [0], color [1], color [2], color [3]);
                            }
                        }
                    }
                va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                }
            }
        va->glEnd ();
        va->glBegin (GL_QUADS);
        for (j = 0; j < cm->numQuads; j++)
            {
            CVertex *v = cm->quad [j].v [0];
            if (explode > 0)
                {
                shift.x = v->normal.x * explodefac;
                shift.y = v->normal.y * explodefac;
                shift.z = v->normal.z * explodefac;
                }
            for (int whichVertex = 0; whichVertex < 4; whichVertex++)
                {
                v = cm->quad [j].v [whichVertex];
                va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                if (cm->hasTexture)
                    {
                    if (cm->vertex)
                        {
                        va->glTexCoord2f (v->tex.x, v->tex.y);
                        va->glColor4f (1, 1, 1, 1);
                        }
                    }
                else
                    {
                    if (numMaterials && cm->material->texture->textureID >= 0)
                        {
                        unsigned char *pColor = material [cm->material->texture->textureID]->color.c;
                        va->glColor4ub (pColor [0], pColor [1], pColor [2], pColor [3]);
                        }
                    }
                va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                }
            }
        va->glEnd ();
        }
    if (alpha)
        {
        glDisable (GL_BLEND);
        glDisable (GL_ALPHA_TEST);
        }
    glPopMatrix ();
    }

void CModel::draw2 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, int explode)
    {
    int i, j;
    CObject *cm;
    for (i = 0; i < numObjects; i ++)
        {
        if (numObjects <= 0)
            {
            break;
            }
        cm = object [i];
        if (cm->hasTexture)
            {
            if (antialiasing)
                {
                gl->enableLinearTexture (cm->material->texture->textureID);
                }
            else
                {
                gl->disableLinearTexture (cm->material->texture->textureID);
                }
            }
        }
    zoom *= scale;
    glPushMatrix ();
    glTranslatef (tl->x + tl2->x, tl->y + tl2->y - 0.002 * explode * explode / timestep / timestep, tl->z + tl2->z);
    float explodefac = (float) explode / 10 / timestep;
    if (showcollision)
        {
        glPushMatrix ();
        glScalef (cubex, cubey, cubez);
        glColor3ub (255, 0, 0);
        glBegin (GL_LINE_STRIP);
        glVertex3f (1, 1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (1, 1, 1);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex3f (-1, 1, 1);
        glVertex3f (-1, 1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (-1, -1, 1);
        glVertex3f (-1, 1, 1);
        glEnd ();
        glBegin (GL_LINES);
        glVertex3f (1, 1, 1);
        glVertex3f (-1, 1, 1);
        glVertex3f (1, -1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (-1, -1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (-1, 1, -1);
        glEnd ();
        glPopMatrix ();
        }
    glRotatef (rot->c+90, 0, -1, 0);
    glRotatef (-rot->a+90, 0, 0, 1);
    glRotatef (rot->b+180, 1, 0, 0);
    glScalef (zoom, zoom, zoom);
    bool listgen = false;
    if (list2 == -1 && explode <= 0 && displaylist)
        {
        listgen = true;
        gl->genList (&list2);
        }
    if (listgen || explode > 0 || !displaylist)
        {
        if (shading == 1)
            {
            glShadeModel (GL_FLAT);
            }
        else
            {
            glShadeModel (GL_SMOOTH);
            }
        if (alpha)
            {
            glEnable (GL_BLEND);
            glEnable (GL_ALPHA_TEST);
            glAlphaFunc (GL_GEQUAL, 0.01);
            }
        for (i = 0; i < numObjects; i++)
            {
            if (numObjects <= 0)
                {
                break;
                }
            cm = object [i];
            if (cm->hasTexture)
                {
                glEnable (GL_TEXTURE_2D);
                glColor4ub (255, 255, 255, 255);
                glBindTexture (GL_TEXTURE_2D, cm->material->texture->textureID);
                }
            else
                {
                glDisable (GL_TEXTURE_2D);
                glColor4ub (255, 255, 255, 255);
                }
            CVector3 shift;
            va->glBegin (GL_TRIANGLES);
            for (j = 0; j < cm->numTriangles; j++)
                {
                CVertex *v = cm->triangle [j].v [0];
                if (explode > 0)
                    {
                    shift.x = v->normal.x * explodefac;
                    shift.y = v->normal.y * explodefac;
                    shift.z = v->normal.z * explodefac;
                    }
                for (int whichVertex = 0; whichVertex < 3; whichVertex ++)
                    {
                    v = cm->triangle [j].v [whichVertex];
                    va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                    if (cm->hasTexture)
                        {
                        if (cm->vertex)
                            {
                            va->glTexCoord2f (v->tex.x, v->tex.y);
                            va->glColor4f (1, 1, 1, 1);
                            }
                        }
                    else
                        {
                        unsigned char *pColor = v->color.c;
                        va->glColor4ub (pColor [0], pColor [1], pColor [2], pColor [3]);
                        }
                    va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                    }
                }
            va->glEnd ();
            va->glBegin (GL_QUADS);
            for (j = 0; j < cm->numQuads; j++)
                {
                CVertex *v = cm->quad [j].v [0];
                if (explode > 0)
                    {
                    shift.x = v->normal.x * explodefac;
                    shift.y = v->normal.y * explodefac;
                    shift.z = v->normal.z * explodefac;
                    }
                for (int whichVertex = 0; whichVertex < 4; whichVertex ++)
                    {
                    v = cm->quad [j].v [whichVertex];
                    va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                    if (cm->hasTexture)
                        {
                        if (cm->vertex)
                            {
                            va->glTexCoord2f (v->tex.x, v->tex.y);
                            va->glColor4f (1, 1, 1, 1);
                            }
                        }
                    else
                        {
                        unsigned char *pColor = v->color.c;
                        va->glColor4ub (pColor [0], pColor [1], pColor [2], pColor [3]);
                        }
                    va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                    }
                }
            va->glEnd ();
            }
        if (alpha)
            {
            glDisable (GL_BLEND);
            glDisable (GL_ALPHA_TEST);
            }
        if (listgen)
            {
            glEndList ();
            }
        }
    else
        {
        glCallList (list2);
        }
    glPopMatrix ();
    }

void CModel::draw3 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, float lum, int explode)
    {
    int i, j;
    CObject *cm;
    zoom *= scale;
    glPushMatrix ();
    glTranslatef (tl->x + tl2->x, tl->y + tl2->y - 0.002 * explode * explode / timestep / timestep, tl->z + tl2->z);
    float explodefac = (float) explode / 10 / timestep;
    if (showcollision)
        {
        glPushMatrix ();
        glScalef (cubex, cubey, cubez);
        glColor3ub (255, 0, 0);
        glBegin (GL_LINE_STRIP);
        glVertex3f (1, 1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (1, 1, 1);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex3f (-1, 1, 1);
        glVertex3f (-1, 1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (-1, -1, 1);
        glVertex3f (-1, 1, 1);
        glEnd ();
        glBegin (GL_LINES);
        glVertex3f (1, 1, 1);
        glVertex3f (-1, 1, 1);
        glVertex3f (1, -1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (-1, -1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (-1, 1, -1);
        glEnd ();
        glPopMatrix ();
        }
    glRotatef (rot->c+90, 0, -1, 0);
    glRotatef (-rot->a+90, 0, 0, 1);
    glRotatef (rot->b+180, 1, 0, 0);
    glScalef (zoom, zoom, zoom);
    if (alpha)
        {
        glEnable (GL_BLEND);
        glEnable (GL_ALPHA_TEST);
        glAlphaFunc (GL_GEQUAL, 0.2);
        }
    for (i = 0; i < numObjects; i++)
        {
        if (numObjects <= 0)
            {
            break;
            }
        cm = object [i];
        glDisable (GL_TEXTURE_2D);
        glColor3ub (255, 255, 255);
        CVector3 shift;
        va->glBegin (GL_TRIANGLES);
        for (j = 0; j < cm->numTriangles; j++)
            {
            CVertex *v = cm->triangle [j].v [0];
            if (explode > 0)
                {
                shift.x = v->normal.x * explodefac;
                shift.y = v->normal.y * explodefac;
                shift.z = v->normal.z * explodefac;
                }
            for (int whichVertex = 0; whichVertex < 3; whichVertex ++)
                {
                v = cm->triangle [j].v [whichVertex];
                unsigned char *pColor = v->color.c;
                float red = lum * pColor [0] / 256;
                float green = lum * pColor [1] / 256;
                float blue = lum * pColor [2] / 256;
                if (red >= 1.0)
                    {
                    red = 1.0;
                    }
                if (green >= 1.0)
                    {
                    green = 1.0;
                    }
                if (blue >= 1.0)
                    {
                    blue = 1.0;
                    }
                va->glColor3f (red, green, blue);
                va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                }
            }
        va->glEnd ();
        va->glBegin (GL_QUADS);
        for (j = 0; j < cm->numQuads; j++)
            {
            CVertex *v = cm->quad [j].v [0];
            if (explode > 0)
                {
                shift.x = v->normal.x * explodefac;
                shift.y = v->normal.y * explodefac;
                shift.z = v->normal.z * explodefac;
                }
            for (int whichVertex = 0; whichVertex < 4; whichVertex ++)
                {
                v = cm->quad [j].v [whichVertex];
                unsigned char *pColor = v->color.c;
                float red = lum * pColor [0] / 256;
                float green = lum * pColor [1] / 256;
                float blue = lum * pColor [2] / 256;
                if (red >= 1.0)
                    {
                    red = 1.0;
                    }
                if (green >= 1.0)
                    {
                    green = 1.0;
                    }
                if (blue >= 1.0)
                    {
                    blue = 1.0;
                    }
                va->glColor3f (red, green, blue);
                va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                }
            }
        va->glEnd ();
        }
    if (alpha)
        {
        glDisable (GL_BLEND);
        glDisable (GL_ALPHA_TEST);
        }
    glPopMatrix ();
    }

void CModel::draw3 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, int explode)
    {
    int i, j;
    CObject *cm;
    zoom *= scale;
    glPushMatrix ();
    glTranslatef (tl->x + tl2->x, tl->y + tl2->y - 0.002 * explode * explode / timestep / timestep, tl->z + tl2->z);
    float explodefac = (float) explode / 10 / timestep;

    if (showcollision)
        {
        glPushMatrix ();
        glScalef (cubex, cubey, cubez);
        glColor3ub (255, 0, 0);
        glBegin (GL_LINE_STRIP);
        glVertex3f (1, 1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (1, 1, 1);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex3f (-1, 1, 1);
        glVertex3f (-1, 1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (-1, -1, 1);
        glVertex3f (-1, 1, 1);
        glEnd ();
        glBegin (GL_LINES);
        glVertex3f (1, 1, 1);
        glVertex3f (-1, 1, 1);
        glVertex3f (1, -1, -1);
        glVertex3f (-1, -1, -1);
        glVertex3f (1, -1, 1);
        glVertex3f (-1, -1, 1);
        glVertex3f (1, 1, -1);
        glVertex3f (-1, 1, -1);
        glEnd ();
        glPopMatrix ();
        }
    glRotatef (rot->c+90, 0, -1, 0);
    glRotatef (-rot->a+90, 0, 0, 1);
    glRotatef (rot->b+180, 1, 0, 0);
    glScalef (zoom, zoom, zoom);
    bool listgen = false;
    if (list3 == -1 && explode <= 0 && displaylist)
        {
        listgen = true;
        gl->genList (&list3);
        }
    if (listgen || explode > 0 || !displaylist)
        {
        if (shading == 1)
            {
            glShadeModel (GL_FLAT);
            }
        else
            {
            glShadeModel (GL_SMOOTH);
            }
        if (alpha)
            {
            glEnable (GL_BLEND);
            glEnable (GL_ALPHA_TEST);
            glAlphaFunc (GL_GEQUAL, 0.2);
            }
        for (i = 0; i < numObjects; i++)
            {
            if (numObjects <= 0)
                {
                break;
                }
            cm = object [i];
            glDisable (GL_TEXTURE_2D);
            glColor3ub (255, 255, 255);
            CVector3 shift;
            va->glBegin (GL_TRIANGLES);
            for (j = 0; j < cm->numTriangles; j++)
                {
                CVertex *v = cm->triangle [j].v [0];
                if (explode > 0)
                    {
                    shift.x = v->normal.x * explodefac;
                    shift.y = v->normal.y * explodefac;
                    shift.z = v->normal.z * explodefac;
                    }
                for (int whichVertex = 0; whichVertex < 3; whichVertex ++)
                    {
                    v = cm->triangle [j].v [whichVertex];
                    va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                    if (cm->hasTexture && false)
                        {
                        if (cm->vertex)
                            {
                            va->glTexCoord2f (v->tex.x, v->tex.y);
                            va->glColor4f (1, 1, 1, 1);
                            }
                        }
                    else
                        {
                        unsigned char *pColor = v->color.c;
                        va->glColor3ub (pColor[0], pColor[1], pColor[2]);
                        }

                    va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                    }
                }
            va->glEnd ();
            va->glBegin (GL_QUADS);
            for (j = 0; j < cm->numQuads; j++)
                {
                CVertex *v = cm->quad [j].v [0];
                if (explode > 0)
                    {
                    shift.x = v->normal.x * explodefac;
                    shift.y = v->normal.y * explodefac;
                    shift.z = v->normal.z * explodefac;
                    }
                for (int whichVertex = 0; whichVertex < 4; whichVertex ++)
                    {
                    v = cm->quad [j].v [whichVertex];
                    va->glNormal3f (v->normal.x, v->normal.y, v->normal.z);
                    if (cm->hasTexture && false)
                        {
                        if (cm->vertex)
                            {
                            va->glTexCoord2f (v->tex.x, v->tex.y);
                            va->glColor4f (1, 1, 1, 1);
                            }
                        }
                    else
                        {
                        unsigned char *pColor = v->color.c;
                        va->glColor3ub (pColor[0], pColor[1], pColor[2]);
                        }
                    va->glVertex3f (v->vector.x + shift.x, v->vector.y + shift.y, v->vector.z + shift.z);
                    }
                }
            va->glEnd ();
            }
        if (alpha)
            {
            glDisable (GL_BLEND);
            glDisable (GL_ALPHA_TEST);
            }
        if (listgen)
            {
            glEndList ();
            }
        }
    else
        {
        glCallList (list3);
        }
    glPopMatrix ();
    }

CSphere::CSphere () {}

CSphere::CSphere (float radius, int segments, float dx, float dy, float dz)
    {
    init (radius, segments, dx, dy, dz, 0);
    }

CSphere::~CSphere () {}

int CSphere::random (int n)
    {
    if (n == 0)
        {
        return 0;
        }

    return rand () % n;
    }

void CSphere::init (float radius, int segments)
    {
    init (radius, segments, 1, 1, 1, 0);
    }

void CSphere::init (float radius, int segments, float dx, float dy, float dz, int randomized)
    {
    CObject *co = new CObject;

    if (co == NULL)
        {
        exit (100);
        }
    co->vertex = new CVertex [segments * segments * 2 + 2];
    if (co->vertex == NULL)
        {
        exit (100);
        }
    co->triangle = new CTriangle [segments * 4];
    if (co->triangle == NULL)
        {
        exit (100);
        }
    co->quad = new CQuad [segments * segments * 2];
    if (co->quad == NULL)
        {
        exit (100);
        }
    this->radius = radius;
    this->segments = segments;
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    int p [4];
    float step = 180.0 / segments;
    CRotation *rot = new CRotation ();
    if (rot == NULL)
        {
        exit (100);
        }
    CVertex *w = new CVertex ();
    if (w == NULL)
        {
        exit (100);
        }
    for (float i = 0; i < 180; i += step)
        {
        for (float i2 = 0; i2 < 360; i2 += step)
            {
            int a = ((int) i) % 360, b = ((int) i2) % 360;
            float si = rot->getsintab (a), ci = rot->getcostabntab (a);
            float si2 = rot->getsintab (b), ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            p [0] = co->addVertex (w);
            a = ((int) (i + step)) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            if (a < 179 || i2 == 0)
                {
                p [1] = co->addVertex (w);
                }
            b = ((int) (i2 + step)) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            if (a < 179)
                {
                p [2] = co->addVertex (w);
                }
            a = ((int) i) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            p [3] = co->addVertex (w);
            if (i == 0 || i >= 180 - step - 1)
                {
                if (!random (randomized))
                    {
                    if (i == 0)
                        {
                        co->triangle [co->numTriangles ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [2]]);
                        }
                    else
                        {
                        co->triangle [co->numTriangles ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [3]]);
                        }
                    }
                }
            else
                {
                if (!random (randomized))
                    {
                    co->quad [co->numQuads ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [2]], &co->vertex [p [3]]);
                    }
                }
            }
        }
    delete rot;
    delete w;
    addObject (co);
    setColor (new CColor (128, 128, 128, 255));
    for (int i2 = 0; i2 < object [0]->numVertices / 2; i2 ++)
        {
        object [0]->vertex [i2].normal.neg ();
        }
    }

void CSphere::invertNormals ()
    {
    for (int i = 0; i < object [0]->numVertices; i ++)
        {
        object [0]->vertex [i].normal.neg ();
        }
    }

void CSphere::setNorthPoleColor (CColor *c, float w)
    {
    int i, i2;

    for (i = 0; i < 4; i ++)
        {
        object [0]->vertex [0].color.c [i] = c->c [i];
        }
    int num = (int) (w * segments * segments / 2);
    num /= (segments * 2);
    num *= (segments * 2);
    for (i = 1; i <= num; i ++)
        {
        float weight = 1.0F - (float) ((int) ((i - 1) / (segments * 2)) * segments * 2) / (float) num;
        for (i2 = 0; i2 < 4; i2 ++)
            {
            object [0]->vertex [i].color.c [i2] = (short) ((1.0F - weight) * object [0]->vertex [i].color.c [i2] + weight * c->c [i2]);
            }
        }
    }

void CSphere::setSouthPoleColor (CColor *c, float w)
    {
    int i, i2;
    int max = (segments - 1) * segments * 2 + 1;

    for (i = 0; i < 4; i ++)
        {
        object [0]->vertex [max].color.c [i] = c->c [i];
        }
    int num = (int) (w * segments * segments / 2);
    num /= (segments * 2);
    num *= (segments * 2);
    for (i = 1; i <= num; i ++)
        {
        float weight = 1.0F - (float) ((int) ((i - 1) / (segments * 2)) * segments * 2) / (float) num;

        for (i2 = 0; i2 < 4; i2 ++)
            {
            object [0]->vertex [max - i].color.c [i] = (short) ((1.0F - weight) * object [0]->vertex [max - i].color.c [i2] + weight * c->c [i2]);
            }
        }
    }

void CSphere::setPoleColor (int phi, int theta, CColor *c, float w)
    {
    int i, i2;

    for (i = 0; i < object [0]->numVertices; i ++)
        {
        int phi2 = ((i - 1) % (segments * 2)) * 360 / (segments * 2);
        int theta2 = ((i - 1) / (segments * 2) + 1) * 360 / (segments * 2);
        if (i == 0)
            {
            theta2 = 0;
            phi2 = 0;
            }
        int dphi = phi - phi2;
        if (dphi < -180)
            {
            dphi += 360;
            }
        else if (dphi > 180)
            {
            dphi -= 360;
            }
        int dtheta = theta - theta2;
        if (dtheta < -180)
            {
            dtheta += 360;
            }
        else if (dtheta > 180)
            {
            dtheta -= 360;
            }
        float alpha = sqrt ((float) (dphi*dphi+dtheta*dtheta));
        if (alpha < 180 * w)
            {
            float weight = 1.0 - alpha / 180.0 / w;

            for (i2 = 0; i2 < 4; i2 ++)
                {
                object [0]->vertex [i].color.c [i2] = (short) ((1.0F - weight) * object [0]->vertex [i].color.c [i2] + weight * c->c [i2]);
                }
            }
        }
    }

CSpherePart::CSpherePart () {}

CSpherePart::CSpherePart (float radius, int segments, float phi)
    {
    init (radius, segments, phi);
    }

CSpherePart::~CSpherePart () {}

void CSpherePart::init (float radius, int segments)
    {
    init (radius, segments, 10);
    }

void CSpherePart::init (float radius, int segments, float phi)
    {
    CObject *co = new CObject;
    co->vertex = new CVertex [segments * 4 + 1];
    co->triangle = new CTriangle [segments];
    co->quad = new CQuad [segments * 3];
    this->radius = radius;
    this->segments = segments;
    float dx = 1, dy = 1, dz = 1;
    int p [4];
    float step = 360.0 / segments;
    float step2 = phi / 4;
    CRotation *rot = new CRotation ();
    CVertex *w = new CVertex ();

    for (float i = 0; i < phi; i += step2)
        for (float i2 = 0; i2 < 360; i2 += step)
            {
            int a = ((int) i) % 360, b = ((int) i2) % 360;
            float si = rot->getsintab (a), ci = rot->getcostabntab (a);
            float si2 = rot->getsintab (b), ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            p [0] = co->addVertex (w);
            a = ((int) (i + step2)) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            if (a < 179 || i2 == 0)
                {
                p [1] = co->addVertex (w);
                }
            b = ((int) (i2 + step)) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            if (a < 179)
                {
                p [2] = co->addVertex (w);
                }
            a = ((int) i) % 360;
            si = rot->getsintab (a);
            ci = rot->getcostabntab (a);
            si2 = rot->getsintab (b);
            ci2 = rot->getcostabntab (b);
            w->vector.x = radius * si * ci2 * dx;
            w->vector.y = radius * si * si2 * dy;
            w->vector.z = radius * ci * dz;
            p [3] = co->addVertex (w);
            if (i == 0 || i >= 180 - step2 - 0.2)
                {
                if (i == 0)
                    {
                    co->triangle [co->numTriangles ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [2]]);
                    }
                else
                    {
                    co->triangle [co->numTriangles ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [3]]);
                    }
                }
            else
                {
                co->quad [co->numQuads ++].setVertices (&co->vertex [p [0]], &co->vertex [p [1]], &co->vertex [p [2]], &co->vertex [p [3]]);
                }
            }
    delete rot;
    delete w;
    addObject (co);
    setColor (new CColor (128, 128, 128, 255));
    }

void CSpherePart::setNorthPoleColor (CColor *c, float w)
    {
    int i, i2;

    for (i = 0; i < 4; i ++)
        {
        object [0]->vertex [0].color.c [i] = c->c [i];
        }
    int num = (int) (w * segments * 4 / 2);
    num /= (segments * 2);
    num *= (segments * 2);
    for (i = 1; i <= num; i ++)
        {
        float weight = 1.0F - (float) ((int) ((i - 1) / (segments * 2)) * 4 * 2) / (float) num;
        for (i2 = 0; i2 < 4; i2 ++)
            {
            object [0]->vertex [i].color.c [i2] = (short) ((1.0F - weight) * object [0]->vertex [i].color.c [i2] + weight * c->c [i2]);
            }
        }
    }

void CSpherePart::setSouthPoleColor (CColor *c, float w)
    {
    }

void CSpherePart::setPoleColor (int phi, int theta, CColor *c, float w)
    {
    }
