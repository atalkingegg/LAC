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

/* This file includes global definitions. */

//##################################################################################################
// Headers needed only by common.cpp
//##################################################################################################

#include <SDL2/SDL.h> /* KEY_F1 */

#include <stdio.h> /* FILE */
#include "common.h" /* LOG_NONE */
#include <string.h> /* strlen */
#include <stdlib.h> /* exit */
#include "dirs.h" /* dirs */
//#include <ctype.h>

extern char DebugBuf[100];

void display_stream (FILE *stream, char *str, int level)
    {
    if (level == LOG_NONE)
        {
        fprintf (stream, "%s\n", str);
        }
    else if (level == LOG_FATAL)
        {
        fprintf (stream, "Fatal: %s\n", str);
        }
    else if (level == LOG_ERROR)
        {
        fprintf (stream, "Error: %s\n", str);
        }
    else if (level == LOG_NET)
        {
        fprintf (stream, "Net:  %s\n", str);
        }
    else if (level == LOG_MOST)
        {
        fprintf (stream, "Info: %s\n", str);
        }
    else
        {
        fprintf (stream, "Debug: %s\n", str);
        }
    fflush (stream);
    }

FILE *display_out = NULL;

void display_exit ()
    {
    if (display_out)
        {
        fclose (display_out);
        }
    display_out = NULL;
    }

// display log/debug message
void display (char *str, int level)
    {
    int len = strlen (str);
    if (!len)
        {
        return;
        }
    if (level <= DebugLevel)
        {
        FILE *stream = stdout;
        if (level == LOG_FATAL || level == LOG_ERROR || level == LOG_NET)
            {
            stream = stderr;
            }

        display_stream (stream, str, level);
        if (!display_out)
            {
            if (dirs)
                {
                if ((display_out = fopen (dirs->getSaves ((char *)"logfile.txt"), "wt")) != NULL)
                    {
                    display_stream (display_out, str, level);
                    }
                }
            }
        else
            {
            display_stream (display_out, str, level);
            }
        }
    }

// display "out of memory" error and exit
void error_outofmemory ()
    {
    display ((char *)"Out of memory", LOG_FATAL);
    exit (EXIT_ALLOC);
    }

void key2string (int key, char *buf)
    {
    buf [0] = '\0';
    sprintf (DebugBuf, "key2string() key = %d", key);
    display (DebugBuf, LOG_ALL);
    // try one byte keys
    if (key > 32 && key <= 'z')
        {
        sprintf (buf, "%c", key);
        }
    else if (key == KEY_SPACE)
        {
        strncpy (buf, "SPACE", 6);
        }
    else if (key == KEY_TAB)
        {
        strncpy (buf, "TAB", 4);
        }
    else if (key == KEY_ENTER)
        {
        strncpy (buf, "ENTER", 6);
        }
    else if (key == KEY_BACKSPACE)
        {
        strncpy (buf, "BSPACE", 7);
        }
    else if (key == 12)
       { 
       strncpy (buf, "NUM *", 6);
       }
    else if (key == 127)
       { 
       strncpy (buf, "DELETE", 7);
       }
    else if (key == 1)
       { 
       strncpy (buf, "NUM1", 5);
       }
    else if (key == 2)
       { 
       strncpy (buf, "NUM2", 5);
       }
    else if (key == 3)
       { 
       strncpy (buf, "NUM3", 5);
       }
    else if (key == 4)
       { 
       strncpy (buf, "NUM4", 5);
       }
    else if (key == 5)
       { 
       strncpy (buf, "NUM5", 5);
       }
    else if (key == 6)
       { 
       strncpy (buf, "NUM6", 5);
       }
    else if (key == 7)
       { 
       strncpy (buf, "NUM7", 5);
       }
    else if (key == 8)
       { 
       strncpy (buf, "NUM8", 5);
       }
    else if (key == 9)
       { 
       strncpy (buf, "NUM9", 5);
       }
    else if (key == 0)
       { 
       strncpy (buf, "NUM0", 5);
       }
    else if (key == 10)
       { 
       strncpy (buf, "NUM DEL", 8);
       }
    else if (key == 11)
       { 
       strncpy (buf, "NUM/", 5);
       }
    else if (key == 13)
       { 
       strncpy (buf, "NUM-", 5);
       }
    else if (key == 14)
       { 
       strncpy (buf, "NUM+", 5);
       }
    else if (key == 15)
       { 
       strncpy (buf, "NUM ENTER", 10);
       }
    if (key < 256)
        {
        return;
        }
    // try special keys
    key -= 256;

    if (key == KEY_F1)
        {
        strncpy (buf, "F1", 3);
        }
    else if (key == KEY_F2)
        {
        strncpy (buf, "F2", 3);
        }
    else if (key == KEY_F3)
        {
        strncpy (buf, "F3", 3);
        }
    else if (key == KEY_F4)
        {
        strncpy (buf, "F4", 3);
        }
    else if (key == KEY_F5)
        {
        strncpy (buf, "F5", 3);
        }
    else if (key == KEY_F6)
        {
        strncpy (buf, "F6", 3);
        }
    else if (key == KEY_F7)
        {
        strncpy (buf, "F7", 3);
        }
    else if (key == KEY_F8)
        {
        strncpy (buf, "F8", 3);
        }
    else if (key == KEY_F9)
        {
        strncpy (buf, "F9", 3);
        }
    else if (key == KEY_F10)
        {
        strncpy (buf, "F10", 4);
        }
    else if (key == KEY_F10)
        {
        strncpy (buf, "F11", 4);
        }
    else if (key == KEY_F10)
        {
        strncpy (buf, "F12", 4);
        }
    else if (key == KEY_UP)
        {
        strncpy (buf, "UP", 3);
        }
    else if (key == KEY_DOWN)
        {
        strncpy (buf, "DOWN", 5);
        }
    else if (key == KEY_LEFT)
        {
        strncpy (buf, "LEFT", 5);
        }
    else if (key == KEY_RIGHT)
        {
        strncpy (buf, "RIGHT", 6);
        }
    else if (key == KEY_PGUP)
        {
        strncpy (buf, "PGUP", 5);
        }
    else if (key == KEY_PGDOWN)
        {
        strncpy (buf, "PGDOWN", 7);
        }
    else if (key == KEY_LALT)
        {
        strncpy (buf, "LALT", 5);
        }
    else if (key == KEY_RALT)
        {
        strncpy (buf, "RALT", 5);
        }
    else if (key == KEY_LCTRL)
        {
        strncpy (buf, "LCTRL", 6);
        }
    else if (key == KEY_RCTRL)
        {
        strncpy (buf, "RCTRL", 6);
        }
    else if (key == KEY_LSHIFT)
        {
        strncpy (buf, "LSHIFT", 7);
        }
    else if (key == KEY_RSHIFT)
        {
        strncpy (buf, "RSHIFT", 7);
        }
    else if (key == KEY_CAPSLOCK)
        {
        strncpy (buf, "CAPS", 5);
        }
    else if (key == KEY_DELETE)
        {
        strncpy (buf, "DEL", 4);
        }
    else if (key == KEY_HOME)
        {
        strncpy (buf, "HOME", 5);
        }
    else if (key == KEY_INSERT)
        {
        strncpy (buf, "INS", 4);
        }
    else if (key == KEY_END)
        {
        strncpy (buf, "END", 4);
        }
    else if (key == 101)
        {
        strncpy (buf, "PGUP", 5);
        display ((char *)"key2string() NUM5", LOG_ALL);
        }
    }

void joystick2string (int button, char *buf)
    {
    buf [0] = '\0';
    char joystick = 'A' + button / 1000;
    int b = button % 1000;
    // try one byte keys
    if (b >= 0 && b < 100)
        {
        sprintf (buf, "%c BTN%d", joystick, b);
        }
    else if (b >= 100)
        {
        sprintf (buf, "%c HAT%d", joystick, b - 100);
        }
    }
