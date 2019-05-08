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

/* This file handles all input/output directories. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirs.h"
#include "common.h"

/*
* The following line, defining "DATADIR" was added to this file
* for compatibility with the "Codeblocks" Integrated Development
* environment, which does not use the conventional Makefile
* with which the original source code was distributed. In that
* original version, "DATADIR" is defined within the makefile as
* a compiler flag.
*/
#define DATADIR '/usr/local/share'

extern char DebugBuf[500];

char *mystrtok (char *str, int len, char *tok)
    {
    char *str0 = str;
    int z = 0;

    while (*str != tok [0])
        {
        str ++;
        z ++;
        if (z >= len)
            {
            return NULL;
            }
        }
    *str = 0;
    return str0;
    }

void Dirs::append (char *target, char *str)
    {
    if (target [strlen (target) - 1] != '/')
        {
        strcat (target, "/");
        }
    strcat (target, str);
    strcat (target, "/");
    }

Dirs::Dirs (char *arg)
    {
    char *home = getenv ("HOME");
    char *env = getenv ("LAC");
    char *path = getenv ("PATH");
    char myfile [474];
    /*
    * I had to hard-code the file path in the following line to
    * overcome a segmentation error on startup.
    */
    sprintf (myfile, "%s/LAC", "/usr/local/bin");
    bool founddir = false;

    display ((char *)"Dirs::Dirs() myfile = ",LOG_MOST);
    display(myfile, LOG_MOST);

    struct stat mystat;
    if (!stat (myfile, &mystat))
        {
        if (S_ISDIR (mystat.st_mode))
            {
            founddir = true;
            }
        }
    if (!founddir)
        {
        char cwd [4096];
        getcwd (cwd, 4096); // get current working directory
        char mypath [8092];
        strncpy (mypath, cwd, 4096);
        strcat (mypath, ":");
        strcat (mypath, path);
        char *p = mypath;
        int pathlen = strlen (mypath);
        if (arg [0] != '/')
            {
            p = mystrtok (p, (int) (path + pathlen - p), (char *)":");
            while (p + strlen (p) - 1 < mypath + pathlen)
                {
                strncpy (myfile, p, 474);
                if (myfile [strlen (myfile) - 1] != '/')
                    {
                    strcat (myfile, "/");
                    }
                if (*arg == '.' && *(arg+1) == '/')
                    {
                    strcat (myfile, arg + 2);
                    }
                else
                    {
                    strcat (myfile, arg);
                    }
                if (!stat (myfile, &mystat))
                    {
                    if (S_ISREG (mystat.st_mode))
                        {
                        goto found;
                        }
                    }
                p = mystrtok (p + strlen (p) + 1, (int) (path + pathlen - p), (char *)":");
                }
            }

        display ((char *)"Binary file has no context to the data files.", LOG_FATAL);
        exit (EXIT_CONTEXT);
found:
        ;
        int bscount = 0;
        for (p = myfile; p < myfile + strlen (myfile); p ++)
            if (*p == '/')
                {
                bscount ++;
                }
        if (bscount >= 2)
            {
            p = myfile + strlen (myfile);
            while (*p != '/')
                {
                p --;
                }
            p --;
            while (*p != '/')
                {
                p --;
                }
            if (p [1] == '.' && p [2] == '/')
                {
                p --;
                while (*p != '/')
                    {
                    p --;
                    }
                }
            p ++;
            *p = 0;
            }
        else
            {
            display ((char *)"Binary file has no context to the data files.", LOG_FATAL);
            exit (EXIT_CONTEXT);
            }
        } 

    if (home != NULL)
        {
        strcpy (saves, home);
        append (saves, (char *)".LAC");
        if (stat (saves, &mystat))
            {
            mkdir (saves, S_IRWXU);
            }
        }
    else
        {
        if (env != NULL)
            {
            strcpy (saves, env);
            }
        else
            {
            strcpy (saves, "..");
            }
        }
    if (env != NULL)
        {
        display ((char *)"Dirs::Dirs() 160408aa", LOG_MOST);
        strcpy (textures, env);
        strcpy (music, env);
        strcpy (sound, env);
        strcpy (models, env);
        strcpy (maps, env);
        }
    else
        {
        sprintf (DebugBuf, "Found LAC data directory %s ", myfile);
        display (DebugBuf, LOG_MOST);
        strcpy (textures, myfile);
        strcpy (music, myfile);
        strcpy (sound, myfile);
        strcpy (models, myfile);
        strcpy (maps, myfile);
        }
    display ((char *)"Dirs::Dirs() 160408ab", LOG_MOST);
    append (textures, (char *)"textures");
    append (music, (char *)"music");
    append (sound, (char *)"sounds");
    append (models, (char *)"models");
    append (maps, (char *)"maps");
    } // Dirs::Dirs (char *arg)

Dirs::~Dirs () {}

char *Dirs::getSaves (char *name)
    {
    strcpy (dir, saves);
    strcat (dir, name);
    return dir;
    }

char *Dirs::getTextures (char *name)
    {
    strcpy (dir, "../textures/");
    strcat (dir, name);
    display ((char *)"Dirs::getTextures() seeking from ", LOG_MOST);
    display (dir, LOG_MOST);
    return dir;
    }

char *Dirs::getMusic (char *name)
    {
    strcpy (dir, "../music/");
    strcat (dir, name);
    display ((char *)"Dirs::getMusic() seeking from ", LOG_MOST);
    display (dir, LOG_MOST);
    return dir;
    }

char *Dirs::getSounds (char *name)
    {
    strcpy (dir, "../sounds/");
    strcat (dir, name);
    display ((char *)"Dirs::getSounds() seeking from ", LOG_MOST);
    display (dir, LOG_MOST);
    return dir;
    }

char *Dirs::getModels (char *name)
    {
    strcpy (dir, "../models/");
    strcat (dir, name);
    display ((char *)"Dirs::getModels() seeking from ", LOG_MOST);
    display (dir, LOG_MOST);
    return dir;
    }

