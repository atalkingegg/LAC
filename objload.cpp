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

/* This file includes a wavefront obj loader. */

/*
*
* A "Wavefront" file is a geometry definition file format first
* developed by Wavefront Technologies for its Advanced Visualizer
* animation package.
*
* The file format is open and has been adopted by other 3D graphics
* application vendors. For the most part it is a universally accepted
* format.
*
* "Wavefront" files use a simple data-format that represents 3D geometry
* alone — namely, the position of each vertex, the UV position of each
* texture coordinate vertex, vertex normals, and the faces that make
* each polygon defined as a list of vertices, and texture vertices.
* Vertices are stored in a counter-clockwise order by default, making
* explicit declaration of face normals unnecessary. The coordinates
* have no units, but the files can contain scale information in a human
* readable comment line.
*
*/

#ifndef IS_OBJLOAD_H

#include <stdio.h>
#include <math.h>
#include "objload.h"

CFile::CFile (char *filename)
    {
    char buf [STDSIZE];
    in = fopen (filename, "rb");
    if (in == NULL)
        {
        sprintf (buf, "Cannot open file %s", filename);
        display (buf, LOG_FATAL);
        exit (EXIT_LOADFILE);
        }
    fseek (in, 0, SEEK_END);
    size = ftell (in);
    fseek (in, 0, SEEK_SET);
    data = new char [size];
    int32 z = 0;
    while (!feof (in))
        {
        fread (&data [z], 4096, sizeof (char), in);
        z += 4096;
        }
    fclose (in);
    filepointer = 0;
    }

CFile::~CFile ()
    {
    delete data;
    }

float CFile::readFloat (int32 offset)
    {
    int32 i = offset;
    while (data [i] == ' ' || data [i] == '\t' || data [i] == 0)
        {
        i ++;
        }
    offset = i;
    while ((data [i] >= '0' && data [i] <= '9') || data [i] == '.' || data [i] == '-')
        {
        i ++;
        }
    data [i] = 0;
    filepointer = i + 1;
    char *p1;
    char **p2 = &p1;
    return (float) strtod (&data [offset], p2);
    }

float CFile::readFloat ()
    {
    return readFloat (filepointer);
    }

int CFile::readInteger (int32 offset)
    {
    int32 i = offset;
    while (data [i] == ' ' || data [i] == '\t' || data [i] == 0)
        {
        i ++;
        }
    offset = i;
    while ((data [i] >= '0' && data [i] <= '9') || data [i] == '-')
        {
        i ++;
        }
    data [i] = 0;
    filepointer = i + 1;
    return (int) atoi (&data [offset]);
    }

int CFile::readInteger ()
    {
    return readInteger (filepointer);
    }

char *CFile::readWord (int32 offset)
    {
    int32 i = offset;
    while (data [i] == ' ' || data [i] == '\t' || data [i] == 0)
        {
        i ++;
        }
    offset = i;
    while (data [i] > ' ' && data [i] <= 'z')
        {
        i ++;
        }
    data [i] = 0;
    filepointer = i + 1;
    return &data [offset];
    }

char *CFile::readWord ()
    {
    return readWord (filepointer);
    }

char *CFile::getChar ()
    {
    return &data [filepointer];
    }

void CFile::nextWhite ()
    {
    int32 i = filepointer;
    while (data [i] != ' ' && data [i] != '\t' && data [i] != '\n' && i < size)
        {
        i ++;
        }
    filepointer = i;
    }

void CFile::skipWhite ()
    {
    int32 i = filepointer;
    while (data [i] == ' ' || data [i] == '\t')
        {
        i ++;
        }
    filepointer = i;
    }

CLoadOBJ::CLoadOBJ ()
    {
    }

void CLoadOBJ::ComputeColors (CModel *model)
    {
    int i, i2;
    if (model->numObjects <= 0)
        {
        return;
        }
    CVector3 n;
    CVector3 light (1.0, 1.0, 1.0);
    light.norm ();
    for (i = 0; i < model->numObjects; i ++)
        {
        CObject *object = (model->object [i]);
        for (i2 = 0; i2 < object->numVertices; i2 ++)
            {
            n.take (&object->vertex [i2].normal);
            int lum = (int) (255.0 - 255.0 * acos (n.dotproduct (&light)));
            object->vertex [i2].color.c [0] = lum;
            object->vertex [i2].color.c [1] = lum;
            object->vertex [i2].color.c [2] = lum;
            }
        }
    }

void CLoadOBJ::ComputeNormals (CModel *model)
    {
    int i, i2, i3;
    CVector3 n;

    if (model->numObjects <= 0)
        {
        return;
        }
    for (i = 0; i < model->numObjects; i ++)
        {
        CObject *object = (model->object [i]);
        for (i2 = 0; i2 < object->numTriangles; i2 ++)
            {
            object->triangle [i2].getNormal (&n);
            if (n.x == 0 && n.y == 0 && n.z == 0)
                {
                n.z = 1;
                }
            for (i3 = 0; i3 < 3; i3 ++)
                {
                object->triangle [i2].v [i3]->addNormal (&n);
                }
            }
        for (i2 = 0; i2 < object->numQuads; i2 ++)
            {
            object->quad [i2].getNormal (&n);
            for (i3 = 0; i3 < 4; i3 ++)
                {
                object->quad [i2].v [i3]->addNormal (&n);
                }
            }
        for (i2 = 0; i2 < object->numTriangles; i2 ++)
            {
            for (i3 = 0; i3 < 3; i3 ++)
                {
                object->triangle [i2].v [i3]->normal.norm ();
                }
            }
        for (i2 = 0; i2 < object->numQuads; i2 ++)
            {
            for (i3 = 0; i3 < 4; i3 ++)
                {
                object->quad [i2].v [i3]->normal.norm ();
                }
            }
        }
    }

void CLoadOBJ::Normalize (CModel *model)
    {
    int i, i2;
    float minx = 1E10, miny = 1E10, minz = 1E10;
    float maxx = -1E10, maxy = -1E10, maxz = -1E10;
    for (i = 0; i < model->numObjects; i ++)
        {
        CObject *object = (model->object [i]);
        for (i2 = 0; i2 < object->numVertices; i2 ++)
            {
            CVertex *v = &object->vertex [i2];
            if (v->vector.x > maxx)
                {
                maxx = v->vector.x;
                }
            if (v->vector.y > maxy)
                {
                maxy = v->vector.y;
                }
            if (v->vector.z > maxz)
                {
                maxz = v->vector.z;
                }
            if (v->vector.x < minx)
                {
                minx = v->vector.x;
                }
            if (v->vector.y < miny)
                {
                miny = v->vector.y;
                }
            if (v->vector.z < minz)
                {
                minz = v->vector.z;
                }
            }
        }
    float tlx = (float) (maxx + minx) / 2.0F;
    float tly = (float) (maxy + miny) / 2.0F;
    float tlz = (float) (maxz + minz) / 2.0F;
    float scx = (float) (maxx - minx) * 0.5F;
    float scy = (float) (maxy - miny) * 0.5F;
    float scz = (float) (maxz - minz) * 0.5F;
    float sc = scx > scy ? scx : scy;
    sc = scz > sc ? scz : sc;
    for (i = 0; i < model->numObjects; i ++)
        {
        CObject *object = (model->object [i]);
        for (i2 = 0; i2 < object->numVertices; i2 ++)
            {
            CVertex *v = &object->vertex [i2];
            v->vector.x -= tlx;
            v->vector.x /= sc;
            v->vector.y -= tly;
            v->vector.y /= sc;
            v->vector.z -= tlz;
            v->vector.z /= sc;
            }
        }
    }

bool CLoadOBJ::ImportOBJ (CModel *model, char *filename)
    {
    file = new CFile (filename);
    int32 i = 0;
    int i2;
    int vertices = 0, faces = 0;
    // precalculate number of vertices and number of triangles
    while (i < file->size - 1)
        {
        // one vertex
        if (file->data [i] == 'v' && (file->data [i + 1] == ' ' || file->data [i + 1] == '\t'))
            {
            vertices ++;
            }
        // read number of vertices in this face
        if (file->data [i] == 'f' && (file->data [i + 1] == ' ' || file->data [i + 1] == '\t'))
            {
            file->filepointer = i + 1;
            file->skipWhite ();
            char *p;
            int z = 0;
            do
                {
                (void) file->readInteger ();
                file->nextWhite ();
                file->skipWhite ();
                p = file->getChar ();
                z ++;
                }
            while (*p >= '0' && *p <= '9');
            faces += (z - 2);
            }
        i ++;
        }
    // create new object
    model->object [model->numObjects] = new CObject;
    CObject *object = model->object [model->numObjects];
    model->numObjects ++;
    object->numVertices = vertices;
    object->numTriangles = faces;
    object->numQuads = 0;
    // create array of vertices and faces (triangles, quads)
    object->vertex = new CVertex [object->numVertices];
    object->triangle = new CTriangle [object->numTriangles];
    object->quad = new CQuad [object->numQuads];
    int vn = 0, tn = 0;
    i = 0;
    while (i < file->size - 1)
        { 
        if (file->data [i] == 'v' && (file->data [i + 1] == ' ' || file->data [i + 1] == '\t'))
            {
            object->vertex [vn].vector.x = file->readFloat (i + 2);
            object->vertex [vn].vector.y = file->readFloat ();
            object->vertex [vn].vector.z = file->readFloat ();
            i = file->filepointer - 2;
            vn ++;
            }
        // read face
        if (file->data [i] == 'f' && (file->data [i + 1] == ' ' || file->data [i + 1] == '\t'))
            {
            file->filepointer = i + 1;
            file->skipWhite ();
            char *p;
            int z = 0;
            int a [100];
            // read vertex indices
            do
                {
                a [z] = file->readInteger ();
                file->nextWhite ();
                file->skipWhite ();
                p = file->getChar ();
                z ++;
                }
            while (*p >= '0' && *p <= '9' && z < 100);
                {
                // create triangles (triangulation)
                for (i2 = 2; i2 < z; i2 ++)
                    {
                    object->triangle [tn].v [0] = &object->vertex [a [0] - 1];
                    object->triangle [tn].v [1] = &object->vertex [a [i2 - 1] - 1];
                    object->triangle [tn].v [2] = &object->vertex [a [i2] - 1];
                    tn ++;
                    }
                }
            }
        i ++;
        }
    delete file;
    ComputeNormals (model);
    ComputeColors (model);
    Normalize (model);
    return true;
    }
#endif
