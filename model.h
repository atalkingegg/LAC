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

/* This file includes the memory representation of any 3D model. */

#ifndef IS_MODEL_H
#define IS_MODEL_H

#include "common.h" // ok
#include "vertexarray.h" // ok

extern double pitab; // pi=atan(1)
extern float sintab [360], costab [360]; // table for sine, cosine functions (obsolete, use COS(), SIN() instead)

/* Currently models are normalized to the (-1,-1,-1)-(1,1,1) cube and static!
   The model represents the "class" description of a static model's geometry and colors.
   It is "instanciated" using a DynamicObj which has a reference to the model,
   scaling, rotation, translation, physical attributes...
   A model consists of several objects.
   Objects consist of vertices, quads, triangles, and one material. */

// CColor stores color information
class CColor
{
  public:
  unsigned char c [4]; // color information as vector, 32 bpp (floats would be faster for the vertex arrays)
  CColor ();
  CColor (CColor *col);
  CColor (short cr, short cg, short cb);
  CColor (short cr, short cg, short cb, short ca);
  ~CColor ();
  void setColor (CColor *col);
  void setColor (short cr, short cg, short cb, short ca);
  void setColor (short cr, short cg, short cb);
  bool isEqual (CColor *col); // compare colors
  void take (CColor *col); // copy data from col
};

// CTexture loads and stores a texture to memory
// To use a texture, it must be loaded and added to the OpenGL texture list using
//   gl->genTextureTGA();
// instead of this class!
class CTexture
{
  public:
  unsigned char *data; // texture color data as array, 32 bpp (RGBA? => see tga loader)
  char name [256]; // texture file name
  bool mipmap; // mipmapping on/off depending on gluBuildMipmaps
  int textureID; // texture ID of native GL code
  int width, height;
  // average values to substitute textured quads by colors for LOD:
  float texlight; // average of texture's overall brightness
  float texred; // average of texture's red
  float texgreen; // average of texture's green
  float texblue; // average of texture's blue
  int quality; // texture quality of native GL code
  bool alpha; // alpha blending necessary
  CTexture ();
  ~CTexture ();
  int loadFromTGA (char *fname, int quality, int alphatype, int mipmap); // called via gl->genTextureTGA()
  void getColor (CColor *c, int x, int y); // color of a special pixel
};

// CVector3 stores the components of a 3D vector (x,y,z)
class CVector3
{
  public:
  float x, y, z; // coordinates, float on most systems faster than double
  CVector3 ();
  CVector3 (float x, float y, float z);
  CVector3 (CVector3 *v);
  void set (float x, float y, float z);
  void neg ();
  void add (CVector3 *v);
  void sub (CVector3 *v);
  void mul (float fac);
  void crossproduct (CVector3 *v);
  float dotproduct (CVector3 *v);
  float length ();
  void norm ();
  bool isEqual (CVector3 *v); // exactly equal in memory (no sense for comparisons)
  bool isEqual (CVector3 *v, float tol); // numerically equal, use a tolerance like 1E-8
  void take (CVector3 *v); // copy data from v
};

// CVector2 stores the components of a 2D vector, for texture purpose
class CVector2
{
  public:
  float x, y; // coordinates
  void take (CVector2 *v); // copy data from v
  bool isEqual (CVector2 *v); // exactly equal in memory (no sense for comparisons)
  bool isEqual (CVector2 *v, float tol); // numerically equal, use a tolerance like 1E-8
};

// CVertex represents a vertex which may take inforamtion about color, location, normal, texture, number of surrounding triangles
class CVertex
{
  public:
  int triangles; // number of triangles this vertex belongs to
  CColor color; // color, used when displayed without textures
  CVector3 vector; // coordinates
  CVector3 normal; // normal vector interpolated of all surrounding triangles
  CVector2 tex; // 2D texture coordinates
  CVertex ();
  CVertex (CVertex *v);
  void addNormal (CVector3 *n); // the normal vector of a vertex can be calculated as average of all adjacent plane normals
  void addColor (CColor *c); // the color of a vertex can be calculated as average of all adjacent plane colors
  void take (CVertex *v); // copy data from v
};

// CRotation stores one (x,y,z)-rotation
class CRotation
{
  private:
  float rot [3] [3]; // rotation matrix
  public:
  short a, b, c; // rotation angles for each plane of the carthesian cosy
  CRotation ();
  ~CRotation ();
  void setAngles (short a, short b, short c);
  void addAngles (short a, short b, short c);
  void calcRotation ();
  float rotateX (CVector3 *v);
  float rotateY (CVector3 *v);
  float rotateZ (CVector3 *v);
  float getsintab (int a);
  float getcostabntab (int a);
  void take (CRotation *r);
};

// CTriangle stores references to the vertices of the triangle/face
class CTriangle
{
  public:
  CVertex *v [3]; // references to the three vertices
  void getNormal (CVector3 *n);
  void setVertices (CVertex *a, CVertex *b, CVertex *c);
};

// CQuad stores references to the vertices of the quad/square/face
class CQuad
{
  public:
  CVertex *v [4]; // references to the four vertices
  void getNormal (CVector3 *n);
  void setVertices (CVertex *a, CVertex *b, CVertex *c, CVertex *d);
};

// CMaterial stores the name, filename, color, and texture of a material
class CMaterial
{
  public:
  char name [255]; // unique name
  char filename [255]; // unique file name
  CColor color; // uniform color
  CTexture *texture; // reference to a texture (or NULL if there is no texture)
  float utile; // tiling coordinates
  float vtile;
  float uoffset; // texture offsets (the importer must calculate u/v due to offsets)
  float voffset;
  float uscale; // texture scaling (the importer must calculate u/v due to scaling)
  float vscale;
  float wrot; // rotation in degree (the importer must calculate u/v due to wrot)
  CMaterial ();
};

// CObject stores the material, vertices, and faces (triangles, quads) of an object
class CObject
{
  public:
  Uint16 numVertices;
  Uint16 numTriangles;
  Uint16 numQuads;
  Uint16 numTexVertex;
  CMaterial *material; // an object has one unique material
  bool hasTexture; // an object can have one unique texture
  char name [255]; // unique object name
  CVertex *vertex; // vertex list
  CTriangle *triangle; // triangle list
  CQuad *quad; // quad list
  CObject ();
  ~CObject ();
  int addVertex (CVertex *w); // used to construct objects
  void setColor (CColor *col);
};

// CModel stores the materials and objects of a model, the data structure is optimized for 3DS files
class CModel
{
  private:
  // all private members are only used "temporarily" at runtime!
  int rotcol; // very special for flickering light sources, e.g. the engine's bright yellow color is rotated
  float light_ambient [4]; // special light source attributes
  float light_diffuse [4];
  float light_ambient2 [4];
  float light_diffuse2 [4];
  CVector3 tlnull;
  CRotation rotnull;
  VertexArray *va; // using a vertex array means more memory, but better performance

  public:
  char name [20]; // unique model name like "GL-117"
  int shading; // shading can be set to FLAT (0) or SMOOTH/GOURAUD (1)
  Uint16 numObjects; // number of objects (which have a unique material)
  Uint16 numMaterials; // number of materials (should be the same as the number of objects)
  bool displaylist; // enable using a display list
  CMaterial *material [100]; // materials, at most 100 (these are only pointers)
  CObject *object [100]; // objects, at most 100 (these are only pointers)
  bool nolight; // do not use light?
  bool alpha; // use alpha blending?
  int numRefpoints; // reference points for missiles
  CVector3 *refpoint; // obsolete
  float scale, scalex, scaley, scalez; // overall scaling of original 3DS coords
  // Note: the model is stored to (-1,-1,-1)-(1,1,1) in RAM, the SpaceObj tells about the scaling of a model
  float cubex, cubey, cubez; // surrounding cube (or radius of sphere) for simplified collision detection
  int list1, list2, list3; // display lists already generated for each type of draw() method

  CModel ();
  void setName (char *name);
  void addMaterial (CMaterial *material);
  void addObject (CObject *object);
  void addRefPoint (CVector3 *tl);
  ~CModel ();
  void setColor (CColor *col);
  void drawVertexNormals (CObject *cm, float zoom);
  int rotateColor (int n);
  void scaleTexture (float fx, float fy);
  // the drawing methods take the following parameters:
  // tl+tl2=translation, rot=rotation, lum=luminance (default 1.0), explode=radial translation (default 0)
  // draw everything
  void draw (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, float lum, int explode);
  // draw without GL lighting
  void draw2 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, int explode);
  // draw without textures
  void draw3 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, int explode);
  // draw without textures, different luminance
  void draw3 (CVector3 *tl, CVector3 *tl2, CRotation *rot, float zoom, float lum, int explode);
};

// CSphere represents an ellipsoid according to the CModel data structure
class CSphere : public CModel
{
  private:
  int random (int n);
  public:
  int segments; // segments on lateral angle, doubled for longitudinal angle
  float radius;
  float dx, dy, dz; // ellipsoid measures in the carthesian cosy
  CSphere ();
  CSphere (float radius, int segments, float dx, float dy, float dz);
  ~CSphere ();
  void init (float radius, int segments);
  void init (float radius, int segments, float dx, float dy, float dz, int randomized);
  void invertNormals (); // point outside/inside
  void setNorthPoleColor (CColor *c, float w); // see setPoleColor, phi=0, theta=90
  void setSouthPoleColor (CColor *c, float w);
  void setPoleColor (int phi, int theta, CColor *c, float w); // shade color over any pole
};

// Part of an ellipsoid
class CSpherePart : public CModel
{
  public:
  int segments; // segments on lateral angle, doubled for longitudinal angle
  float radius;
  CSpherePart ();
  CSpherePart (float radius, int segments, float phi);
  ~CSpherePart ();
  void init (float radius, int segments);
  void init (float radius, int segments, float phi);
  void setNorthPoleColor (CColor *c, float w); // see setPoleColor, phi=0, theta=90
  void setSouthPoleColor (CColor *c, float w);
  void setPoleColor (int phi, int theta, CColor *c, float w); // shade color over any pole
};

#endif
