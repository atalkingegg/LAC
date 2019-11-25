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

#ifndef IS_COMMON_H
#define IS_COMMON_H
//##################################################################################################
// Headers needed only by common.h
//##################################################################################################
#include <SDL2/SDL.h> /* SDL_BUTTON_LEFT */
#include <math.h> /* M_PI */

// #include "config.h" // defines HAVE_SDL, HAVE_SDL_MIXER
// #undef USE_GLUT
// #define HAVE_SDL_MIXER

// #include <GL/glut.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>
// #include <SDL2/SDL_thread.h>
// #include <SDL2/SDL_mixer.h>
// #include <GL/glu.h>

#undef int32
#undef Uint32
#undef Uint16
#undef NULL
#include <stdint.h>
typedef int32_t int32;
typedef uint32_t Uint32;
typedef uint16_t Uint16;
#define NULL nullptr

#define MUMBLEROOTCHANNEL 0
#define MUMBLECURRENTMISSIONCHANNEL 1
#define MUMBLECURRENTMISSIONBLUESUBCHANNEL 2
#define MUMBLECURRENTMISSIONREDSUBCHANNEL 3
#define MUMBLENONMISSIONCHANNEL 4

#define AIRFIELDREPAIRVALUE 300 //Amount of Airfield damage repaired per repair interval when not influenced by nearby aircraft.

#define ZOOM 256
#define GLOBALSCALE 1
#define VERSIONSTRING "LINUX AIR COMBAT SDL2-191125A"
#define MAXALTITUDE 1320 //1320 =43660ft. Any attempt to go higher is completely prevented.

// data types
#define LINEWIDTH(x) (float) x * height / 600.0F

// game states
#define GAME_INIT 0
#define GAME_PLAY 1
#define GAME_PAUSE 2
#define GAME_MENU 3
#define GAME_CREDITS 4
#define GAME_CREATE 5
#define GAME_MISSION 7
#define GAME_STATS 8
#define GAME_FAME 9
#define GAME_FIGHTER 10
#define GAME_FINISH 11
#define GAME_QUIT 12

// weather
#define WEATHER_SUNNY 0
#define WEATHER_THUNDERSTORM 1
#define WEATHER_CLOUDY 2

// controls
#define CONTROLS_KEYBOARD 0
#define CONTROLS_MOUSE 1
#define CONTROLS_JOYSTICK 2

// view range limits.
#define VIEW_MIN 30
#define VIEW_MAX 210

extern bool MultiPlayer, isserver;
extern bool SunBlinding; // viewer gazing into the sun?

extern int antialiasing;
extern int brightness; // brightness correction
extern int camera; // camera type (0=cockpit, 1=chase, 2=backward, ...)
extern int clouds;
extern int contrast; // contrast/gamma correction (not yet configurable)
extern int controls; // see CONTROLS-constants
extern int day;
extern int debug; // debug mode (1=counter measures)
extern int DebugLevel;
extern int difficulty; // 0=easy, 1=normal, 2=hard
extern int dithering;
extern int dynamiclighting;
extern int fullscreen;
extern int game; // see GAME-constants
extern int lighting; // 1=on, 0=off (obsolete)
extern int mode; // always off=0, 1 would mean to calculate ROAM (obsolete)!
extern int mousex, mousey; // current mouse coordinates
extern int physics; // 0=action, 1=realistic
extern int quality;
extern int showcollision;
extern int specialeffects;
extern int volumemusic;
extern int volumesound;
extern int wantwidth, wantheight, wantfullscreen; // requested values for next restart
extern int weather; // 0: sunny, 1: thunderstorm, 2: no atmosphere (moon)
extern int width, height, bpp;

extern float blackout, redout; // high force
extern float camphi, camgamma, camtheta; // phi: heading, theta: roll, gamma: elevation
extern float camx, camy, camz; // camera position
extern float sungamma; // angle of sunrays dropping on horizon
extern float sunlight; // dynamically adjustable light (0=black...1=default...brighter)
extern float view;
extern float view_x, view_y; // angles for joystick-controlled view

extern void display (char *str, int level);
extern void display_exit ();
extern void error_outofmemory (); // display "out of memory" error and exit
extern void key2string (int key, char *buf);
extern void joystick2string (int key, char *buf);

extern float getView (); // return view dependant on weather

// maximum constants for objects
const int maxexplosion = 30;
const int maxfighter = 30;
const int maxjaxis = 10; // Maximum number of axis on any joystick
const int maxjoysticks = 10; // Maximum number of joysticks
const int maxMachineGunBullet = 150;
const int maxmissile = 30;
const int maxstar = 30;
const int maxgroundobj = 10;
const int maxblacksmoke = 50;
const int maxflare = 30;
const int maxchaff = 30;
const int timestep = 34; // (higher values mean slower gameplay)

// sine and cosine table (obsolete)
//#define SIN(x) sine[(int)x]
//#define COS(x) cosi[(int)x]

// From cockpit in forward view, help calculate best pilot position:
#define AngleAheadToBestViewpoint 15.0F
#define PilotPositionFactor 1.0F

// exact sine, cosine functions
#define SIN(x) sin(x * M_PI / 180.0)
#define COS(x) cos(x * M_PI / 180.0)

// define common GLUT/SDL keys

  #define MOUSE_BUTTON_LEFT SDL_BUTTON_LEFT
  #define MOUSE_BUTTON_MIDDLE SDL_BUTTON_MIDDLE
  #define MOUSE_BUTTON_RIGHT SDL_BUTTON_RIGHT
  #define MOUSE_UP SDL_RELEASED
  #define MOUSE_DOWN SDL_PRESSED
  #define KEY_F1 SDLK_F1
  #define KEY_F2 SDLK_F2
  #define KEY_F3 SDLK_F3
  #define KEY_F4 SDLK_F4
  #define KEY_F5 SDLK_F5
  #define KEY_F6 SDLK_F6
  #define KEY_F7 SDLK_F7
  #define KEY_F8 SDLK_F8
  #define KEY_F9 SDLK_F9
  #define KEY_F10 SDLK_F10
  #define KEY_UP SDLK_UP
  #define KEY_DOWN SDLK_DOWN
  #define KEY_LEFT SDLK_LEFT
  #define KEY_RIGHT SDLK_RIGHT
  #define KEY_PGUP SDLK_PAGEUP
  #define KEY_PGDOWN SDLK_PAGEDOWN
  #define KEY_LALT SDLK_LALT
  #define KEY_RALT SDLK_RALT
  #define KEY_LCTRL SDLK_LCTRL
  #define KEY_RCTRL SDLK_RCTRL
  #define KEY_LSHIFT SDLK_LSHIFT
  #define KEY_RSHIFT SDLK_RSHIFT
  #define KEY_CAPSLOCK SDLK_CAPSLOCK
  #define KEY_DELETE SDLK_DELETE
  #define KEY_INSERT SDLK_INSERT
  #define KEY_HOME SDLK_HOME
  #define KEY_END SDLK_END
  #define KEY_NUM1 SDLK_KP1
  #define KEY_NUM2 SDLK_KP2
  #define KEY_NUM3 SDLK_KP3
  #define KEY_NUM4 SDLK_KP4
  #define KEY_NUM5 SDLK_KP5
  #define KEY_NUM6 SDLK_KP6
  #define KEY_NUM7 SDLK_KP7
  #define KEY_NUM8 SDLK_KP8
  #define KEY_NUM9 SDLK_KP9
  #define KEY_NUM0 SDLK_KP0

#define KEY_BACKSPACE 8
#define KEY_TAB 9
#define KEY_ENTER 13
#define KEY_SPACE 32

// log/debug level, look at common.cpp
#define LOG_NONE 0 // do not log anything
#define LOG_FATAL 1 // only show fatal (LAC will exit) errors (stderr)
#define LOG_ERROR 2 // show errors too (stderr)
#define LOG_MOST 3 // show warnings too (stdout)
#define LOG_NET 4 // show important messages too (stdout)
#define LOG_ALL 5 // log everything (stdout)
#define STDSIZE 256 // typical temporary buffer size
#define PATHSIZE 4096 // maximum path/directory string size

// exit codes
#define EXIT_NORMAL 0
#define EXIT_LOADFILE 1
#define EXIT_INIT 2
#define EXIT_COMMAND 3
#define EXIT_ALLOC 4
#define EXIT_CONTEXT 5

#endif
