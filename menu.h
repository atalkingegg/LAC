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

#ifndef IS_MENU_H
#define IS_MENU_H

#include "model.h" // ok

extern int menu_id;

extern CColor menu_colwhite;
extern CColor menu_colgrey;
extern CColor menu_colyellow;

class Component
{
  public:
  unsigned char colback [4];
  float x, y, w, h;
  int id;
  bool highlight;
  bool active;
  bool visible;
  void (*func) (Component *, int button); // button=0...3 for mouse, 0...255 for one byte keys, 256... for special+256
  Component *parent;
  Component ();
  virtual void setActive (bool value);
  virtual void setVisible (bool value);
  virtual void setHighlight (bool value);
  bool isActive ();
  bool isVisible ();
  bool isHighlight ();
  void setBounds (float x, float y, float w, float h);
  void setFunction (void (*func) (Component *, int button));
  void execFunction (int button);
  virtual void draw ();
  virtual void eventMouse (int x, int y, int button);
  virtual void triggerMouse (int id, int button);
  virtual bool contains (int id);
  virtual void eventKey (unsigned char key);
  virtual void eventSpecial (int key);
  virtual void eventJoystick (int button);
};

class Container : public Component
{
  public:
  int numcomponents;
  Component *components [100];
  Container ();
  int getNumberOfComponents ();
  void add (Component *component);
  void draw ();
  void eventMouse (int x, int y, int button);
  void triggerMouse (int id, int button);
  void setVisible (bool value);
  void setActive (bool value);
  void setHighlight (bool value);
  bool contains (int id);
  void eventKey (unsigned char key);
  void eventSpecial (int key);
  void eventJoystick (int button);
};

class Label : public Component
{
  public:
  char text [64];
  bool transparent;
  Label ();
  Label (char *text);
  void setTransparent (bool transparent);
  void setText (char *text);
  void draw ();
  void triggerMouse (int id, int button);
};

class Button : public Component
{
  public:
  char text [64];
  Button ();
  Button (char *text);
  void draw ();
  void triggerMouse (int id, int button);
};

class TextField : public Component
{
  public:
  char text [8192];
  float fonth;
  TextField ();
  TextField (char *text, float fontheight);
  void setText (char *text);
  void draw ();
  void triggerMouse (int id, int button);
};

class EditKey : public Component
{
  public:
  char text [64];
  char key;
  int special;
  bool specialActive; // key or special valid?
  EditKey ();
  EditKey (char *text);
  void setText (char *text);
  void setKey (int key); // please add 0xFF to two byte keys to distinguish from one byte keys
  int getKey (); // returns special+256 for two byte keys ('+' is special=13, ENTER is key=13)
  void draw ();
  void triggerMouse (int id, int button);
  void eventKey (unsigned char key);
  void eventSpecial (int key);
};

class EditJoystick : public Component
{
  public:
  char text [64];
  int button;
  EditJoystick ();
  EditJoystick (char *text);
  void setText (char *text);
  void setButton (int key); // button 0..99, hat 100..
  int getButton (); // returns button 0..99, hat 100..
  void draw ();
  void triggerMouse (int id, int button);
  void eventJoystick (int button);
};

class EditField : public Component
{
  public:
  char text [64];
  int timer;
  int ptr;
  int maxlen;
  EditField ();
  EditField (int maxlen);
  void setText (char *text);
  void draw ();
  void triggerMouse (int id, int button);
  void eventKey (unsigned char key);
};

#endif
