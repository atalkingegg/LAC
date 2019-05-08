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

/* This file includes a very general GUI. */

extern char DebugBuf[100];

#ifndef IS_MENU_H

#include "menu.h"
#include "main.h"

int menu_id = 0;
int menu_z = -2;

CColor menu_colgrey (220, 220, 220, 200);
CColor menu_colwhite (255, 255, 255, 200);
CColor menu_colyellow (255, 255, 0, 200);

extern unsigned int key_LookBK;
extern unsigned int key_LookDN;
extern unsigned int key_LookFD;
extern unsigned int key_LookLT;
extern unsigned int key_LookRT;
extern unsigned int key_LookUP;

Component::Component ()
    {
    
    menu_id ++;
    id = menu_id;
    func = NULL;
    highlight = false;
    active = true;
    visible = true;
    parent = NULL;
    colback [0] = 0;
    colback [1] = 0;
    colback [2] = 255;
    colback [3] = 120;
    }

void Component::setBounds (float x, float y, float w, float h)
    {
    
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    }

void Component::draw ()
    {
    }

void Component::setFunction (void (*func) (Component *, int button))
    {
    
    this->func = func;
    }

void Component::execFunction (int button)
    {
    
    if (func)
        {
        func (this, button);
        }
    }

void Component::eventMouse (int x, int y, int button)
    {
    
    }

void Component::eventKey (unsigned char key)
    {
    }

void Component::eventJoystick (int button)
    {
    }

void Component::eventSpecial (int key)
    {
    }

void Component::triggerMouse (int id, int button)
    {
    }

void Component::setActive (bool value)
    {
    active = value;
    }

void Component::setVisible (bool value)
    {
    visible = value;
    }

void Component::setHighlight (bool value)
    {
    highlight = value;
    }

bool Component::isActive ()
    {
    return active;
    }

bool Component::isVisible ()
    {
    return visible;
    }

bool Component::isHighlight ()
    {
    return highlight;
    }

bool Component::contains (int id)
    {
    if (id == this->id)
        {
        return true;
        }

    return false;
    }

Container::Container ()
    {
    numcomponents = 0;
    }

void Container::add (Component *component)
    {
    component->parent = this;
    components [numcomponents] = component;
    numcomponents ++;
    }

void Container::draw ()
    {
    int i;

    if (active && visible)
        {
        for (i = 0; i < numcomponents; i ++)
            {
            components [i]->draw ();
            }
        }
    }

int Container::getNumberOfComponents ()
    {
    return numcomponents;
    }

void Container::eventMouse (int x, int y, int button)
    {
    
    int i, i2;
//  glutSwapBuffers ();
    GLuint selectBuff [20 * 4];
    GLint hits, viewport [4];
    // Select buffer parameters
    glSelectBuffer (20 * 4, selectBuff);
    glGetIntegerv (GL_VIEWPORT, viewport);
    // Enter to selection mode
    glEnable (GL_DEPTH_TEST);
    glDisable (GL_LIGHTING);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    // Set-up pick matrix
    gluPickMatrix (x, viewport [3] - y, 1, 1, viewport);
    // Set our perspective transformation matrix
    gluPerspective (80.0, 1.0, 0.25 * GLOBALSCALE, view * GLOBALSCALE);
//  gluPerspective (60.0, 1.0, 0.2, 200.0);
    glMatrixMode (GL_MODELVIEW);
    // Render all scene and fill selection buffer
    glRenderMode (GL_SELECT);
    glInitNames ();
    glPushName (0);
    draw ();
    // Get hits and go back to normal rendering
    hits = glRenderMode (GL_RENDER);
    // Get first model in selection buffer stack
    GLuint *ptr = selectBuff;
    int mypicks = 0;
    int pick [100];

    if (hits > 0)
        {
        int i;

        for (i = 0; i < hits; i ++)
            {
            int names = *ptr;
            ptr ++;
            ptr ++;
            ptr ++;
            pick [mypicks] = *ptr;
            mypicks ++;
            ptr += names;

            if (mypicks >= 20)
                {
                break;
                }
            }
        }
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->setHighlight (false);
        for (i2 = 0; i2 < hits; i2 ++)
            {
            if (components [i]->contains (pick [i2]))
                {
                components [i]->triggerMouse (pick [i2], button);
                }
            }
        }
    }

void Container::triggerMouse (int id, int button)
    {
    int i;
    for (i = 0; i < numcomponents; i ++)
        {
        if (components [i]->contains (id))
            {
            components [i]->triggerMouse (id, button);
            }
        }
    }

void Container::setVisible (bool value)
    {
    int i;
    visible = value;
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->setVisible (value);
        }
    }

void Container::setActive (bool value)
    {
    int i;
    active = value;
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->setActive (value);
        }
    }

void Container::setHighlight (bool value)
    {
    int i;
    highlight = value;
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->setHighlight (value);
        }
    }

bool Container::contains (int id)
    {
    int i;
    bool ret = false;
    for (i = 0; i < numcomponents; i ++)
        {
        ret |= components [i]->contains (id);
        }
    return ret;
    }

void Container::eventKey (unsigned char key)
{
int i;
for (i = 0; i < numcomponents; i ++)
    {
    components [i]->eventKey (key);
    }
if (key == key_LookBK)
   {
   
   if (game == GAME_FIGHTER || game == GAME_QUIT || game == GAME_MISSION)
      {
      mousey += 8;
      }
   else
      {
      mousey += 1;
      }
   
   }
if (key == key_LookLT)
   {
   if (game == GAME_FIGHTER || game == GAME_QUIT || game == GAME_MISSION)
      {
      mousex -= 8;
      }
   else
      {
      mousex -= 1;
      }
   
   }
if (key == key_LookRT)
   {
   if (game == GAME_FIGHTER || game == GAME_QUIT || game == GAME_MISSION)
      {
      mousex += 8;
      }
   else
      {
      mousex += 1;
      }
   
   }
if (key == key_LookFD)
   {
   
   if (game == GAME_FIGHTER || game == GAME_QUIT || game == GAME_MISSION)
      {
      mousey -= 8;
      }
   else
      {
      mousey -= 1;
      }
   }
if (key != key_LookFD && key != key_LookBK && key != key_LookLT && key != key_LookRT)
   { 
   eventMouse(mousex, mousey, 1);
   }
}

void Container::eventSpecial (int key)
    {
    int i;
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->eventSpecial (key);
        }
    }

void Container::eventJoystick (int button)
    {
    int i;
    for (i = 0; i < numcomponents; i ++)
        {
        components [i]->eventJoystick (button);
        }
    }

Label::Label ()
    {
    }

Label::Label (char *text)
    {
    strcpy (this->text, text);
    transparent = false;
    colback [0] = 140;
    colback [1] = 140;
    colback [2] = 140;
    colback [3] = 180;
    }

void Label::setText (char *text)
    {
    strcpy (this->text, text);
    }

void Label::setTransparent (bool transparent)
    {
    this->transparent = transparent;
    }

void Label::draw ()
    {
    if (!visible)
        {
        return;
        }
    CColor *col = &menu_colwhite;
    if (!transparent)
        {
        glLoadName (id);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin (GL_QUADS);
        glColor4ub (colback [0], colback [1], colback [2], colback [3]);
        glVertex3f (0.1 * x, 0.1 * y, menu_z);
        glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
        glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
        glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
        glEnd ();
        glDisable (GL_BLEND);
        }
    font1->zoom = 0.1 * (h - 0.1);
    float yf = (y - h - 0.01) / (h - 0.1);
    float xf = (x + 0.25) / (h - 0.1);
    font1->drawText (xf, yf, menu_z, text, col);
    font1->zoom = 0.1;
    }

void Label::triggerMouse (int id, int button)
    {
    }

Button::Button ()
    {
    }

Button::Button (char *text)
    {
    strcpy (this->text, text);
    }

void Button::draw ()
    {
    if (!visible)
        {
        return;
        }
    CColor *col_active = &menu_colwhite;
    CColor *col_deactive = &menu_colgrey;
    CColor *col_highlight = &menu_colyellow;
    glLoadName (id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_QUADS);
    if (!highlight)
        {
        glColor4ub (colback [0], colback [1], colback [2], colback [3]);
        }
    else
        {
        glColor4ub (255, 255, 0, colback [3]);
        }
    glVertex3f (0.1 * x, 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
    glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
    glEnd ();
    glDisable (GL_BLEND);
    font1->zoom = 0.1 * (h - 0.1);
    float yf = (y - h - 0.01) / (h - 0.1);
    float xf = (x + 0.25) / (h - 0.1);
    if (active && !highlight)
        {
        font1->drawText (xf, yf, menu_z, text, col_active);
        }
    else if (!active)
        {
        font1->drawText (xf, yf, menu_z, text, col_deactive);
        }
    else
        {
        font1->drawText (xf, yf, menu_z, text, col_highlight);
        }
    font1->zoom = 0.1;
    }

void Button::triggerMouse (int id, int button)
    {
    
    if (!active)
        {
        return;
        }
    highlight = true;
    if (button)
        {
        execFunction (button);
        }
    }

TextField::TextField ()
    {
    }

TextField::TextField (char *text, float fontheight)
    {
    strcpy (this->text, text);
    fonth = fontheight;
    colback [0] = 140;
    colback [1] = 140;
    colback [2] = 140;
    colback [3] = 180;
    }

void TextField::setText (char *text)
    {
    strcpy (this->text, text);
    }

void TextField::draw ()
    {
    if (!visible)
        {
        return;
        }
    CColor *col = &menu_colwhite;
    glLoadName (id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_QUADS);
    glColor4ub (colback [0], colback [1], colback [2], colback [3]);
    glVertex3f (0.1 * x, 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
    glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
    glEnd ();
    glDisable (GL_BLEND);
    font1->zoom = 0.1 * (fonth - 0.1);
    float yf = (y - fonth - 0.1) / (fonth - 0.1);
    float xf = (x + 0.25) / (fonth - 0.1);
    font1->drawText (xf, yf, menu_z, text, col);
    font1->zoom = 0.1;
    }

void TextField::triggerMouse (int id, int button)
    {
    }

EditKey::EditKey ()
    {
    
    key = 'A';
    }

EditKey::EditKey (char *text)
    {
    key = 'A';
    strcpy (this->text, text);
    }

void EditKey::setText (char *text)
    {
    
    strcpy (this->text, text);
    }

void EditKey::setKey (int key)
    {
    if (key >= 256)
        {
        this->special = key - 256;
        specialActive = true;
        }
    else
        {
        this->key = key;
        specialActive = false;
        }
    }

int EditKey::getKey ()
    {
    
    if (specialActive)
        {
        return special + 256;
        }
    else
        {
        return key;
        }
    }

void EditKey::draw ()
    {
    if (!visible)
        {
        return;
        }
    char buf [64];
    CColor *col = &menu_colwhite;
    glLoadName (id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_QUADS);
    if (!highlight)
        {
        glColor4ub (colback [0], colback [1], colback [2], colback [3]);
        }
    else
        {
        glColor4ub (255, 255, 0, colback [3]);
        }
    glVertex3f (0.1 * x, 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
    glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
    glEnd ();
    glDisable (GL_BLEND);
    font1->zoom = 0.1 * (h - 0.1);
    float yf = (y - h - 0.01) / (h - 0.1);
    float xf = (x + 0.25) / (h - 0.1);
    font1->drawText (xf, yf, menu_z, text, col);
    if (specialActive)
        {
        key2string (special + 256, buf);
        }
    else
        {
        key2string (key, buf);
        }
    font1->drawText (xf + (w - 2) / h, yf, menu_z, buf, col);
    font1->zoom = 0.1;
    }

void EditKey::triggerMouse (int id, int button)
    {
    if (!active)
        {
        return;
        }
    highlight = true;
    }

void EditKey::eventKey (unsigned char key)
    {
    char buf [256];
    if (!active)
        {
        return;
        }
    
    if (highlight)
        {
        
        key2string (key, buf);

        if (buf [0] != '\0')
            {
            this->key = (int) key;
            specialActive = false;
            execFunction (key);
            }
        }
    else
       {
       
       }
    }

void EditKey::eventSpecial (int key)
    {
    char buf [256];
    if (!active)
        {
        return;
        }
    
    if (highlight)
        {
        key2string (key + 256, buf);
        if (buf [0] != '\0')
            {
            this->special = key;
            specialActive = true;
            execFunction (special + 256);
            }
        }
    }

EditJoystick::EditJoystick ()
    {
    button = 0;
    }

EditJoystick::EditJoystick (char *text)
    {
    button = 0;
    strcpy (this->text, text);
    }

void EditJoystick::setText (char *text)
    {
    strcpy (this->text, text);
    }

void EditJoystick::setButton (int button)
    {
    this->button = button;
    }

int EditJoystick::getButton ()
    {
    return button;
    }

void EditJoystick::draw ()
    {
    if (!visible)
        {
        return;
        }
    char buf [64];
    CColor *col = &menu_colwhite;
    glLoadName (id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_QUADS);
    if (!highlight)
        {
        glColor4ub (colback [0], colback [1], colback [2], colback [3]);
        }
    else
        {
        glColor4ub (255, 255, 0, colback [3]);
        }
    glVertex3f (0.1 * x, 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
    glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
    glEnd ();
    glDisable (GL_BLEND);
    font1->zoom = 0.1 * (h - 0.1);
    float yf = (y - h - 0.01) / (h - 0.1);
    float xf = (x + 0.25) / (h - 0.1);
    font1->drawText (xf, yf, menu_z, text, col);
    joystick2string (button, buf);
    font1->drawText (xf + (w - 2) / h, yf, menu_z, buf, col);
    font1->zoom = 0.1;
    }

void EditJoystick::triggerMouse (int id, int button)
    {
    if (!active)
        {
        return;
        }
    highlight = true;
    }

void EditJoystick::eventJoystick (int button)
    {
    char buf [256];
    if (!active)
        {
        return;
        }
    if (highlight)
        {
        joystick2string (button, buf);
        if (buf [0] != '\0')
            {
            this->button = (int) button;
            execFunction (button);
            }
        }
    }

EditField::EditField ()
    {
    memset (text, 0, 64);
    ptr = 0;
    timer = SDL_GetTicks ();
    }

EditField::EditField (int maxlen)
    {
    memset (this->text, 0, 64);
    ptr = 0;
    if (maxlen > 62)
        {
        maxlen = 62;
        }
    else if (maxlen < 0)
        {
        maxlen = 0;
        }
    this->maxlen = maxlen;
    timer = SDL_GetTicks ();
    }

void EditField::setText (char *text)
    {
    if (strlen (text) > (unsigned int) maxlen)
        {
        return;
        }
    memset (this->text, 0, 64);
    strcpy (this->text, text);
    ptr = strlen (this->text);
    }

void EditField::draw ()
    {
    char str [64];
    int akttime;
    if (!visible)
        {
        return;
        }
    CColor *col = &menu_colgrey;
    strcpy (str, text);
    if (highlight)
        {
        akttime = SDL_GetTicks ();
        if (((akttime - timer) & 0xff) > 128)
            {
            str [ptr] = 'I';
            str [ptr + 1] = 0;
            }
        }
    glLoadName (id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin (GL_QUADS);
    if (!highlight)
        {
        glColor4ub (colback [0], colback [1], colback [2], colback [3]);
        }
    else
        {
        glColor4ub (255, 255, 0, colback [3]);
        }
    glVertex3f (0.1 * x, 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * y, menu_z);
    glVertex3f (0.1 * (x + w), 0.1 * (y - h), menu_z);
    glVertex3f (0.1 * x, 0.1 * (y - h), menu_z);
    glEnd ();
    glDisable (GL_BLEND);
    font1->zoom = 0.1 * (h - 0.1);
    float yf = (y - h - 0.01) / (h - 0.1);
    float xf = (x + 0.25) / (h - 0.1);
    font1->drawText (xf, yf, menu_z, str, col);
    font1->zoom = 0.1;
    }

void EditField::triggerMouse (int id, int button)
    {
    if (!active)
        {
        return;
        }
    highlight = true;
    }

void EditField::eventKey (unsigned char key)
    {
    if (!highlight)
        {
        return;
        }
    if (ptr >= 62)
        {
        return;
        }
    if (key >= 32)
        {
        text [ptr] = key;
        ptr ++;
        text [ptr] = 0;
        }
    else if (key == 8 && ptr > 0)
        {
        ptr --;
        text [ptr] = 0;
        }
    }
#endif
