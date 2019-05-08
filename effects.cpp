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

#include <math.h>

#include "effects.h"
#include "glland.h"
#include "gl.h"
#include "common.h"

CTexture *texsmoke, *texsmoke2, *texsmoke3;

CSmoke::CSmoke (int type)
    {
    for (int i = 0; i < MAXSMOKEELEM; i ++)
        {
        time [i] = 0;
        zoom [i] = 0.1;
        phi [i] = 0;
        }

    last = 0;
    this->type = type;
    }

CSmoke::~CSmoke ()
    {
    }

void CSmoke::setSmoke (float x, float y, float z, int myphi, int mytime)
    {
    last ++;

    if (last >= MAXSMOKEELEM)
        {
        last = 0;
        }

    v [last].x = x;
    v [last].y = y;
    v [last].z = z;
    time [last] = mytime;
    phi [last] = myphi;
    }

void CSmoke::move (Uint32 dt, int dec)
    {
    for (int i = 0; i < MAXSMOKEELEM; i ++)
        if (time [i] > 0)
            {
            time [i] -= dec;
            if (time [i] < 0)
                {
                time [i] = 0;
                }
            }
    }

void CSmoke::drawElem (int n)
    {
    if (n < 0 || n >= MAXSMOKEELEM)
        {
        return;
        }
    float myzoom = smokezoom [time [n]];
    glPushMatrix ();
    glTranslatef (v [n].x, v [n].y, v [n].z);
    glRotatef (camphi, 0.0, 1.0, 0.0);
    glRotatef (-camgamma, 1.0, 0.0, 0.0);
    glScalef (myzoom, myzoom, myzoom);
    glBegin (GL_QUADS);
    glColor4ub (255, 255, 255, time [n] * 5 + 25);
    glTexCoord2f (0, 0);
    glVertex3f (1, 1, 0);
    glTexCoord2f (1, 0);
    glVertex3f (1, -1, 0);
    glTexCoord2f (1, 1);
    glVertex3f (-1, -1, 0);
    glTexCoord2f (0, 1);
    glVertex3f (-1, 1, 0);
    glEnd ();
    glPopMatrix ();
    }

void CSmoke::drawElemHQ (int n)
    {
    if (n < 0 || n >= MAXSMOKEELEM)
        {
        return;
        }

    glBegin (GL_QUADS);
    glColor4ub (255, 255, 255, time [n] * 10 + 55);
    float myzoom = smokezoom [time [n]];
    glTexCoord2f (0, 0);
    glVertex3f (v [n].x - myzoom, v [n].y + myzoom, v [n].z);
    glTexCoord2f (1, 0);
    glVertex3f (v [n].x + myzoom, v [n].y + myzoom, v [n].z);
    glTexCoord2f (1, 1);
    glVertex3f (v [n].x + myzoom, v [n].y - myzoom, v [n].z);
    glTexCoord2f (0, 1);
    glVertex3f (v [n].x - myzoom, v [n].y - myzoom, v [n].z);
    glTexCoord2f (0, 0);
    glVertex3f (v [n].x, v [n].y + myzoom, v [n].z - myzoom);
    glTexCoord2f (1, 0);
    glVertex3f (v [n].x, v [n].y + myzoom, v [n].z + myzoom);
    glTexCoord2f (1, 1);
    glVertex3f (v [n].x, v [n].y - myzoom, v [n].z + myzoom);
    glTexCoord2f (0, 1);
    glVertex3f (v [n].x, v [n].y - myzoom, v [n].z - myzoom);
    glEnd ();
    }

void CSmoke::draw ()
    {
    int i;
    int smoketype = 0;

    if (type == 0)
        {
        smoketype = texsmoke->textureID;
        }
    else if (type == 1)
        {
        smoketype = texsmoke2->textureID;
        }

    if (antialiasing)
        {
        gl->enableLinearTexture (smoketype);
        }
    else
        {
        gl->disableLinearTexture (smoketype);
        }

    gl->enableAlphaBlending ();
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.02);
    gl->enableTextures (smoketype);

    // draw smoke elements in the best order
    for (i = last; i >= 0; i --)
        {
        if (time [i] > 0)
            {
            drawElem (i);
            }
        }

    for (i = MAXSMOKEELEM - 1; i > last; i --)
        {
        if (time [i] > 0)
            {
            drawElem (i);
            }
        }

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_ALPHA_TEST);
    gl->disableAlphaBlending ();
    }

Flash::Flash ()
    {
    phi = 0;
    }

Flash::~Flash ()
    {
    }

void Flash::set (float x, float y, float z, int phi)
    {
    int i;
    this->phi = phi;
    w [0].x = x;
    w [0].y = y;
    w [0].z = z;

    for (i = 1; i < 10; i ++)
        {
        w [i].y = y + 6.0 * i;
        w [i].x = w [i - 1].x + 0.1 * myrandom (60) - 3.0;
        w [i].z = w [i - 1].z + 0.1 * myrandom (60) - 3.0;
        }
    }

void Flash::draw ()
    {
    int i;
    glDisable (GL_TEXTURE_2D);
    glBegin (GL_QUAD_STRIP);
    glColor3ub (255, 255, 255);
    float myzoom = 0.3;

    for (i = 0; i < 10; i ++)
        {
        glVertex3f (w [i].x - myzoom * cosi [phi], w [i].y, w [i].z + myzoom * sine [phi]);
        glVertex3f (w [i].x + myzoom * cosi [phi], w [i].y, w [i].z - myzoom * sine [phi]);
        }

    glEnd ();
    }

void Flash::drawHQ ()
    {
    int i;
    glDisable (GL_TEXTURE_2D);
    gl->enableAlphaBlending ();
    glBegin (GL_QUADS);
    float myzoom = 0.4;
    float alphawidth = 0.7;

    for (i = 0; i < 9; i ++)
        {
        glColor4ub (255, 255, 255, 0);
        glVertex3f (w [i].x - myzoom * cosi [phi], w [i].y, w [i].z + myzoom * sine [phi]);
        glColor4ub (255, 255, 255, 255);
        glVertex3f (w [i].x - myzoom * alphawidth * cosi [phi], w [i].y, w [i].z + myzoom * alphawidth * sine [phi]);
        glColor4ub (255, 255, 255, 255);
        glVertex3f (w [i + 1].x - myzoom * alphawidth * cosi [phi], w [i + 1].y, w [i + 1].z + myzoom * alphawidth * sine [phi]);
        glColor4ub (255, 255, 255, 0);
        glVertex3f (w [i + 1].x - myzoom * cosi [phi], w [i + 1].y, w [i + 1].z + myzoom * sine [phi]);
        glColor4ub (255, 255, 255, 255);
        glVertex3f (w [i].x - myzoom * alphawidth * cosi [phi], w [i].y, w [i].z + myzoom * alphawidth * sine [phi]);
        glVertex3f (w [i].x + myzoom * alphawidth * cosi [phi], w [i].y, w [i].z - myzoom * alphawidth * sine [phi]);
        glVertex3f (w [i + 1].x + myzoom * alphawidth * cosi [phi], w [i + 1].y, w [i + 1].z - myzoom * alphawidth * sine [phi]);
        glVertex3f (w [i + 1].x - myzoom * alphawidth * cosi [phi], w [i + 1].y, w [i + 1].z + myzoom * alphawidth * sine [phi]);
        glColor4ub (255, 255, 255, 255);
        glVertex3f (w [i].x + myzoom * alphawidth * cosi [phi], w [i].y, w [i].z - myzoom * alphawidth * sine [phi]);
        glColor4ub (255, 255, 255, 0);
        glVertex3f (w [i].x + myzoom * cosi [phi], w [i].y, w [i].z - myzoom * sine [phi]);
        glColor4ub (255, 255, 255, 0);
        glVertex3f (w [i + 1].x + myzoom * cosi [phi], w [i + 1].y, w [i + 1].z - myzoom * sine [phi]);
        glColor4ub (255, 255, 255, 255);
        glVertex3f (w [i + 1].x + myzoom * alphawidth * cosi [phi], w [i + 1].y, w [i + 1].z - myzoom * alphawidth * sine [phi]);
        }

    glEnd ();
    gl->disableAlphaBlending ();
    }

CExplosion::CExplosion (Space *space, CModel *sphere)
    {
    this->space = space;
    o = sphere;
    ttl = 0;
    space->addObject (this);
    draw = false;
    zoom = 0.1;
    drawlight = false;
    }

void CExplosion::setExplosion (float x, float y, float z, float vx, float vy, float vz, float maxzoom, int len)
    {
    tl->x = x;
    tl->y = y;
    tl->z = z;
    this->maxzoom = maxzoom;
    ttl = len;
    maxlen = len;
    draw = true;
    v.set (vx, vy, vz);
    }

void CExplosion::move (Uint32 dt)
    {
    if (ttl > 0)
        {
        float timefac = (float) dt / (float) timestep;
        zoom = sine [ttl * 180 / maxlen] * maxzoom;
        ttl -= dt;
        tl->y += 0.01 * timefac;

        if (ttl <= 0)
            {
            ttl = 0;
            draw = false;
            }

        float brakepower = pow (0.98, timefac);
        v.mul (brakepower);
        tl->x += v.x * timefac;
        tl->y += v.y * timefac;
        tl->z += v.z * timefac;
        }
    }

CBlackSmoke::CBlackSmoke (Space *space)
    {
    this->space = space;
    ttl = 0;
    space->addObject (this);
    draw = false;
    zoom = 0.1;
    drawlight = false;
    alpha = -1;
    }

void CBlackSmoke::setBlackSmoke (float x, float y, float z, float myphi, float maxzoom, int len)
    {
    this->myphi = myphi;
    tl->x = x;
    tl->y = y;
    tl->z = z;
    this->maxzoom = maxzoom;
    ttl = len;
    maxlen = len;
    draw = true;
    zoom = 0;
    }

void CBlackSmoke::move (Uint32 dt)
    {
    if (ttl > 0)
        {
        zoom = maxzoom * (maxlen - ttl) / maxlen;
        tl->y += 0.04 * dt / timestep;
        ttl -= dt;

        if (ttl <= 0)
            {
            ttl = 0;
            draw = false;
            }
        }
    }

void CBlackSmoke::drawGL (CVector3 *z1, CVector3 *z2, CVector3 *tl, float alpha2, float lum2, bool drawlight2, bool istextured2)
    {
    if (ttl <= 0 || !specialeffects)
        {
        return;
        }

    if (draw == 2 || gl->isSphereInFrustum (tl->x + this->tl->x, tl->y + this->tl->y, tl->z + this->tl->z, this->zoom))
        {
        glDepthMask (GL_FALSE);

        if (antialiasing)
            {
            gl->enableLinearTexture (texsmoke3->textureID);
            }
        else
            {
            gl->disableLinearTexture (texsmoke3->textureID);
            }

        gl->enableAlphaBlending ();
        glEnable (GL_ALPHA_TEST);
        glAlphaFunc (GL_GEQUAL, 0.02);
        gl->enableTextures (texsmoke3->textureID);
        glBegin (GL_QUADS);
        int myalpha = 255 - (maxlen - ttl) * 255 / maxlen;

        if (myalpha > 255)
            {
            myalpha = 255;
            }

        glColor4ub (0, 0, 0, myalpha);
        float myzoom = zoom;
        float cosphi = COS(camphi), sinphi = SIN(camphi);
        glTexCoord2f (0, 0);
        glVertex3f (this->tl->x - myzoom * cosphi, this->tl->y + myzoom, this->tl->z + myzoom * sinphi);
        glTexCoord2f (1, 0);
        glVertex3f (this->tl->x + myzoom * cosphi, this->tl->y + myzoom, this->tl->z - myzoom * sinphi);
        glTexCoord2f (1, 1);
        glVertex3f (this->tl->x + myzoom * cosphi, this->tl->y - myzoom, this->tl->z - myzoom * sinphi);
        glTexCoord2f (0, 1);
        glVertex3f (this->tl->x - myzoom * cosphi, this->tl->y - myzoom, this->tl->z + myzoom * sinphi);
        glEnd ();
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_ALPHA_TEST);
        gl->disableAlphaBlending ();
        glDepthMask (GL_TRUE);
        }
    }

Star::Star (int phi, int gamma, float size)
    {
    this->phi = phi;
    this->gamma = gamma;
    this->size = size;
    }

void Star::draw ()
    {
    glPointSize (LINEWIDTH(size));
    glBegin (GL_POINTS);
    glColor3ub (255, 255, 255);
    glVertex3f (0, 0, 0);
    glEnd ();
    }

/* Each font is stored in one bitmap, width=height=2^n!
   Use GIMP or equiv and put the ASCII letters of your favourite font
   there, ascending order, start whereever you want. Write as many
   letters as possible per line, each separated by at least one whitespace.
   Look at the font*.tga files for examples. The max letter height is fixed. */

bool Font::isPixel (int x, int y)
    {
    y = texture->height - y;
    int index = (y * texture->width + x) * 4;

    if (texture->data [index] > 50 && texture->data [index + 1] > 50 && texture->data [index + 2] > 50)
        {
        return true;
        }

    return false;
    }

void Font::extractLetters (int height, char start, int num)
    {
    int i;
    int mode = 0;
    bool modeon = 0;
    this->start = start;
    this->height = height;
    int x = 0, y = 0;
    int xs = 0, xe = 0;
    n = 0;

    for (;;)
        {
        mode ++;

        for (i = y + 2; i <= y + height - 2; i += 2)
            {
            if (isPixel (x, i))
                {
                mode = 0;
                }
            }

        if (mode == 0)
            {
            if (!modeon)
                {
                xs = x;
                modeon = true;
                }
            }

        if (mode >= 5)
            {
            if (modeon)
                {
                xe = x;
                modeon = false;
                letterx [n] = xs - 2;
                lettery [n] = y;
                letterw [n] = xe - xs + 1;
                n ++;

                if (n >= num)
                    {
                    return;
                    }
                }
            }

        x ++;

        if (x >= texture->width)
            {
            if (modeon)
                {
                xe = x;
                modeon = false;
                letterx [n] = xs - 2;
                lettery [n] = y;
                letterw [n] = xe - xs + 3;
                n ++;

                if (n >= num)
                    {
                    return;
                    }
                }

            y += height;
            x = 0;
            }

        if (y >= texture->height)
            {
            return;
            }
        }
    }

Font::Font (char *filename, int height, char start, int num)
    {
    texture = gl->genTextureTGA (filename, 0, 1, 0, true);
    extractLetters (height, start, num);
    zoom = 0.1F;
    stdcol = new CColor (255, 255, 255, 220);
    highlightcol = new CColor (255, 255, 0, 255);
    }

Font::~Font ()
    {
    delete stdcol;
    delete highlightcol;
    }

void Font::drawText (float x, float y, float z, char *str, CColor *c, bool centered, int highlight, CColor *highlightcol)
    {
    int len = strlen (str);
    glDisable (GL_LIGHTING);
    glDisable (GL_DEPTH_TEST);
    gl->enableTextures (texture->textureID);
    gl->enableAlphaBlending ();
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.1);
    glBegin (GL_QUADS);
    glColor4ub (c->c [0], c->c [1], c->c [2], c->c [3]);
    float xz = x * zoom, yz = y * zoom;

    if (centered)
        {
        float xw = 0;

        for (int i = 0; i < len; i ++)
            {
            if (str [i] >= start && str [i] < start + n)
                {
                int c = (int) (str [i] - start);
                xw += zoom * letterw [c] / height;
                }
            else if (str [i] == '\t')
                {
                int xzint = (int) (xw * 10.0);
                xzint -= xzint & 3; // modulo 4
                xzint += 4;
                xw = (float) xzint * zoom;
                }
            else
                {
                xw += 0.5 * zoom;
                }
            }

        xz -= xw / 2;
        }

    for (int i = 0; i < len; i ++)
        {
        if (str [i] >= start && str [i] < start + n)
            {
            if (i != highlight)
                {
                glColor4ub (c->c [0], c->c [1], c->c [2], c->c [3]);
                }
            else
                {
                glColor4ub (highlightcol->c [0], highlightcol->c [1], highlightcol->c [2], highlightcol->c [3]);
                }

            int c = (int) (str [i] - start);
            float tx = (float) letterx [c] / texture->width;
            float ty = 1.0 - (float) lettery [c] / texture->height;
            float tx2 = (float) tx + (float) letterw [c] / texture->width;
            float ty2 = (float) ty - (float) height / texture->height;
            float xi = zoom * letterw [c] / height;
            float yi = zoom;
            glTexCoord2f (tx, ty2);
            glVertex3f (xz, yz, z);
            glTexCoord2f (tx, ty);
            glVertex3f (xz, yz + yi, z);
            glTexCoord2f (tx2, ty);
            glVertex3f (xz + xi, yz + yi, z);
            glTexCoord2f (tx2, ty2);
            glVertex3f (xz + xi, yz, z);
            xz += xi;
            }
        else if (str [i] == '\t')
            {
            int xzint = (int) (xz * 10.0);
            xzint -= xzint & 3; // modulo 4
            xzint += 4;
            xz = (float) xzint * zoom;
            }
        else if (str [i] == '\n')
            {
            yz -= zoom;
            xz = x * zoom;
            }
        else
            {
            xz += 0.5 * zoom;
            }
        }

    glEnd ();
    glDisable (GL_ALPHA_TEST);
    gl->disableAlphaBlending ();
    glDisable (GL_TEXTURE_2D);
    }

void Font::drawText (float x, float y, float z, char *str, CColor *c, int highlight, CColor *highlightcol)
    {
    drawText (x, y, z, str, c, false, highlight, highlightcol);
    }

void Font::drawText (float x, float y, float z, char *str, CColor *c, int highlight)
    {
    drawText (x, y, z, str, c, false, highlight, highlightcol);
    }

void Font::drawText (float x, float y, float z, char *str, int highlight, CColor *highlightcol)
    {
    drawText (x, y, z, str, stdcol, false, highlight, highlightcol);
    }

void Font::drawText (float x, float y, float z, char *str, int highlight)
    {
    drawText (x, y, z, str, stdcol, false, highlight, highlightcol);
    }

void Font::drawText (float x, float y, float z, char *str, CColor *c)
    {
    drawText (x, y, z, str, c, false, -1, highlightcol);
    }

void Font::drawText (float x, float y, float z, char *str)
    {
    drawText (x, y, z, str, stdcol, false, -1, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str, CColor *c, int highlight, CColor *highlightcol)
    {
    drawText (x, y, z, str, c, true, highlight, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str, CColor *c, int highlight)
    {
    drawText (x, y, z, str, c, true, highlight, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str, CColor *c)
    {
    drawText (x, y, z, str, c, true, -1, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str, int highlight, CColor *highlightcol)
    {
    drawText (x, y, z, str, stdcol, true, highlight, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str, int highlight)
    {
    drawText (x, y, z, str, stdcol, true, highlight, highlightcol);
    }

void Font::drawTextCentered (float x, float y, float z, char *str)
    {
    drawText (x, y, z, str, stdcol, true, -1, highlightcol);
    }

void Font::drawTextRotated (float x, float y, float z, char *str, CColor *color, int timer)
    {
    int len = strlen (str);
    glDisable (GL_LIGHTING);
    glDisable (GL_DEPTH_TEST);
    gl->enableTextures (texture->textureID);
    gl->enableAlphaBlending ();
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.1);
    float xz = x * 0.1, yz = y * 0.1;

    for (int i = 0; i < len; i ++)
        {
        if (str [i] >= start && str [i] <= start + n)
            {
            int c = (int) (str [i] - start);
            float tx = (float) letterx [c] / texture->width;
            float ty = 1.0 - (float) lettery [c] / texture->height;
            float tx2 = (float) tx + (float) letterw [c] / texture->width;
            float ty2 = (float) ty - (float) height / texture->height;
            float xi = 0.1 * letterw [c] / height;
            float yi = 0.1;
            glPushMatrix ();
            glTranslatef (xz + xi / 2, yz + yi / 2, z);
            glRotatef ((float) timer / timestep + i * 20, 1, 0, 0);
            glBegin (GL_QUADS);
            glColor4ub (color->c [0], color->c [1], color->c [2], color->c [3]);
            glTexCoord2f (tx, ty2);
            glVertex3f (-xi / 2, -yi / 2, 0);
            glTexCoord2f (tx, ty);
            glVertex3f (-xi / 2, yi / 2, 0);
            glTexCoord2f (tx2, ty);
            glVertex3f (xi / 2, yi / 2, 0);
            glTexCoord2f (tx2, ty2);
            glVertex3f (xi / 2, -yi / 2, 0);
            glEnd ();
            glPopMatrix ();
            xz += xi;
            }
        else if (str [i] == '\t')
            {
            int xzint = (int) (xz * 10.0);
            xzint -= xzint & 3; // modulo 4
            xzint += 4;
            xz = (float) xzint / 10.0;
            }
        else
            {
            xz += 0.05;
            }
        }

    glDisable (GL_ALPHA_TEST);
    gl->disableAlphaBlending ();
    glDisable (GL_TEXTURE_2D);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer, int highlight, CColor *highlightcol)
    {
    int i;
    int len = strlen (str);
    glDisable (GL_LIGHTING);
    glDisable (GL_DEPTH_TEST);
    gl->enableTextures (texture->textureID);
    gl->enableAlphaBlending ();
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.1);
    float xw = 0;

    for (i = 0; i < len; i ++)
        {
        if (str [i] >= start && str [i] <= start + n)
            {
            int c = (int) (str [i] - start);
            xw += 0.1 * letterw [c] / height;
            }
        else if (str [i] == '\t')
            {
            int xzint = (int) (xw * 10.0);
            xzint -= xzint & 3; // modulo 4
            xzint += 4;
            xw = (float) xzint / 10.0;
            }
        else
            {
            xw += 0.05;
            }
        }

    float xz = x * 0.1, yz = y * 0.1;
    glPushMatrix ();
    glTranslatef (xz + xw / 2, yz, z);
    xz = 0;
    glScalef (1.0 + 0.08 * sine [abs (timer * 8 / timestep % 360)], 1, 1);

    for (i = 0; i < len; i ++)
        {
        if (str [i] >= start && str [i] <= start + n)
            {
            int c = (int) (str [i] - start);
            float tx = (float) letterx [c] / texture->width;
            float ty = 1.0 - (float) lettery [c] / texture->height;
            float tx2 = (float) tx + (float) letterw [c] / texture->width;
            float ty2 = (float) ty - (float) height / texture->height;
            float xi = 0.1 * letterw [c] / height;
            float yi = 0.1;

            if (i != highlight)
                {
                glColor4ub (color->c [0], color->c [1], color->c [2], color->c [3]);
                }
            else
                {
                glColor4ub (highlightcol->c [0], highlightcol->c [1], highlightcol->c [2], highlightcol->c [3]);
                }

            glPushMatrix ();
            glTranslatef (xz + xi / 2 - xw / 2, yi / 2, 0);
            glBegin (GL_QUADS);
            glTexCoord2f (tx, ty2);
            glVertex3f (-xi / 2, -yi / 2, 0);
            glTexCoord2f (tx, ty);
            glVertex3f (-xi / 2, yi / 2, 0);
            glTexCoord2f (tx2, ty);
            glVertex3f (xi / 2, yi / 2, 0);
            glTexCoord2f (tx2, ty2);
            glVertex3f (xi / 2, -yi / 2, 0);
            glEnd ();
            glPopMatrix ();
            xz += xi;
            }
        else if (str [i] == '\t')
            {
            int xzint = (int) (xz * 10.0);
            xzint -= xzint & 3; // modulo 4
            xzint += 4;
            xz = (float) xzint / 10.0;
            }
        else
            {
            xz += 0.05;
            }
        }

    glPopMatrix ();
    glDisable (GL_ALPHA_TEST);
    gl->disableAlphaBlending ();
    glDisable (GL_TEXTURE_2D);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer, int highlight)
    {
    drawTextScaled (x, y, z, str, color, timer, highlight, highlightcol);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, CColor *color, int timer)
    {
    drawTextScaled (x, y, z, str, color, timer, -1, highlightcol);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, int timer, int highlight, CColor *highlightcol)
    {
    drawTextScaled (x, y, z, str, stdcol, timer, highlight, highlightcol);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, int timer, int highlight)
    {
    drawTextScaled (x, y, z, str, stdcol, timer, highlight, highlightcol);
    }

void Font::drawTextScaled (float x, float y, float z, char *str, int timer)
    {
    drawTextScaled (x, y, z, str, stdcol, timer, -1, highlightcol);
    }

HighClouds::HighClouds (int theta)
    {
    init (theta);
    }

HighClouds::~HighClouds ()
    {
    delete sphere;
    }

void HighClouds::init (int theta)
    {
    sphere = new CSpherePart (1, 9, theta);
    CObject *co = sphere->object [0];
    co->hasTexture = true;
    co->material = new CMaterial ();

    for (int i2 = 0; i2 < co->numVertices; i2 ++)
        {
        co->vertex [i2].tex.x = co->vertex [i2].vector.x * 5;
        co->vertex [i2].tex.y = co->vertex [i2].vector.y * 5;
        }

    co->hasTexture = true;
    sphere->displaylist = false;
    o = sphere;
    rot->b = 90;
    draw = 2;
    drawlight = false;
    zoom = 300;
    }

void HighClouds::setTexture (CTexture *texture)
    {
    sphere->object [0]->material->texture = texture;
    }

void HighClouds::drawGL (CVector3 *tl, CVector3 *textl)
    {
    int j;
    CObject *cm = o->object [0];

    for (int i2 = 0; i2 < cm->numVertices; i2 ++)
        {
        cm->vertex [i2].tex.x = cm->vertex [i2].vector.x * 4 + textl->x / zoom;
        cm->vertex [i2].tex.y = cm->vertex [i2].vector.y * 4 - textl->z / zoom;
        }

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.015);
    glDisable (GL_DEPTH_TEST);
    glPushMatrix ();
    glTranslatef (tl->x, tl->y, tl->z);
    glRotatef (90, 0, -1, 0);
    glRotatef (270, 1, 0, 0);
    glRotatef (90, 0, 0, 1);
    glScalef (zoom, zoom, zoom);
    glShadeModel (GL_SMOOTH);
    glEnable (GL_TEXTURE_2D);
    glColor3ub (255, 255, 255);
    glBindTexture (GL_TEXTURE_2D, cm->material->texture->textureID);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin (GL_TRIANGLES);

    for (j = 0; j < cm->numTriangles; j++)
        {
        CVertex *v = cm->triangle [j].v [0];

        for (int whichVertex = 0; whichVertex < 3; whichVertex ++)
            {
            v = cm->triangle [j].v [whichVertex];
            glNormal3f (v->normal.x, v->normal.y, v->normal.z);
            glTexCoord2f (v->tex.x, v->tex.y);
            glVertex3f(v->vector.x, v->vector.y, v->vector.z);
            }
        }

    glEnd();

    glBegin(GL_QUADS);

    for (j = 0; j < cm->numQuads; j++)
        {
        CVertex *v = cm->quad [j].v [0];

        for (int whichVertex = 0; whichVertex < 4; whichVertex ++)
            {
            v = cm->quad [j].v [whichVertex];
            glNormal3f (v->normal.x, v->normal.y, v->normal.z);
            glTexCoord2f(v->tex.x, v->tex.y);
            glVertex3f(v->vector.x, v->vector.y, v->vector.z);
            }
        }

    glEnd();
    glEnable (GL_DEPTH_TEST);
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_BLEND);
    glDisable (GL_TEXTURE_2D);
    glPopMatrix ();
    }

#endif
