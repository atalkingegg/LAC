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

/* This file includes a 3ds loader. Thanks to the great NeHe Tutorials! */

#ifndef IS_3DS_H
#define IS_3DS_H

#include "model.h" // ok

const int debug3ds = 0; // set to 1 to get all Chunk IDs to stdout

// abstract binary file representation in memory
class BinaryFile
{
  public:
  FILE *in; // file struct
  Uint32 size; // file size
  Uint32 filepointer; // pointer to char in memory
  unsigned char *data; // file data in memory
  unsigned char ret [11]; // big endian swapping buffer

  // data type conversation
  BinaryFile (char *filename);
  ~BinaryFile ();
  int readFloat (float *f);
  int readFloat (float *f, int n);
  int readUInt32 (Uint32 *i);
  int readUInt16 (Uint16 *i);
  int readString (char *ptr, int ptrmax, int n);
  int readString (char *ptr, int n);
  int readString (char *ptr);
  int skip (int n);
};

// Primary Chunk (19789)
#define PRIMARY       0x4D4D

// Main Chunks (15677, 2, 45056)
#define VERSION       0x0002
#define MAINSCALE     0x0100
#define OBJECTINFO    0x3D3D
#define EDITKEYFRAME  0xB000

// Sub defines of OBJECTINFO (45055, 16384)
#define MATERIAL	  0xAFFF
#define OBJECT		  0x4000

// Sub defines of MATERIAL
#define MAT_NAME       0xA000
#define MAT_DIFFUSE    0xA020
#define MAT_MAP        0xA200
#define MAT_MAPFILE    0xA300
#define MAT_USCALE     0xA354
#define MAT_VSCALE     0xA356
#define MAT_UOFFSET    0xA358
#define MAT_VOFFSET    0xA35A
#define MAT_ROTATION   0xA35C

// Read new object
#define OBJECT_MESH   0x4100

// Sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110
#define OBJECT_FACES		0x4120
#define OBJECT_MATERIAL		0x4130
#define OBJECT_UV			0x4140
#define OBJECT_MESHMATRIX			0x4160

// Structure for the 3DS indicies
struct Indices {
  Uint16 a, b, c, bVisible;
};

// Chunk info
struct Chunk
{
  Uint16 ID;
  Uint32 length;
  Uint32 bytesRead;
};

// CLoad3DS handles all of the loading code (see NEHE's OpenGL tutorials)
class CLoad3DS
{
  public:
  BinaryFile *file;
  CLoad3DS ();
  bool Import3DS (CModel *model, char *filename);

  // developer specific loading routines
  private:
  int GetString (char *);
  void ReadChunk (Chunk *);
  void ProcessNextChunk (CModel *model, Chunk *);
  void ProcessNextObjectChunk (CModel *model, CObject *object, Chunk *);
  void ProcessNextMaterialChunk (CModel *model, Chunk *);
  void ReadColorChunk (CMaterial *material, Chunk *pChunk);
  void ReadVertices (CObject *object, Chunk *);
  void ReadVertexIndices (CObject *object, Chunk *);
  void ReadUVCoordinates (CObject *object, Chunk *);
  void ReadMeshMatrix (CObject *object, Chunk *);
  void ReadUScale (CMaterial *material, Chunk *);
  void ReadVScale (CMaterial *material, Chunk *);
  void ReadUOffset (CMaterial *material, Chunk *);
  void ReadVOffset (CMaterial *material, Chunk *);
  void ReadUVRotation (CMaterial *material, Chunk *);
  void ReadObjectMaterial (CModel *model, CObject *object, Chunk *previousChunk);
  void Compile (CModel *model);
  void ComputeNormals (CModel *model);
  void ComputeColors (CModel *model);
  void LoadTextures (CModel *model);
  void Normalize (CModel *model);
  void CleanUp ();
  FILE *filepointer;
  Chunk *currentChunk;
  Chunk *tempChunk;
};

#endif

