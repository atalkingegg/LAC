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

/* This file handles all pilots data. */

#ifndef IS_PILOTS_H
#define IS_PILOTS_H

#define SAVEVERSION "LAC01p13"

const int maxpilotdata = 100;

#define PILOT_WASHINGTON 0
#define PILOT_ADAMS 1
#define PILOT_JEFFERSON 2
#define PILOT_MADISON 3
#define PILOT_MONROE 4
#define PILOT_JQADAMS 5
#define PILOT_JACKSON 6
#define PILOT_VANBUREN 7
#define PILOT_HARRISON 8
#define PILOT_PLAYER 9

class Pilot
{
  public:
  char name [16];
  int mission_state [maxpilotdata]; // success/failure
  int mission_time [maxpilotdata]; // time spent on the mission
  int mission_fighterkills [maxpilotdata];
  int mission_shipkills [maxpilotdata];
  int mission_tankkills [maxpilotdata];
  int mission_otherkills [maxpilotdata];
  int mission_Durability [maxpilotdata]; // Durability left
  int mission_points [maxpilotdata]; // extra points for hitting a target
  int mission_score [maxpilotdata]; // overall score (calculated)
  int ranking; // current ranking (calculated due to all scores)

  void load ();
  void save ();
  char *getRank ();
  char *getShortRank ();
  Pilot (char *name);
  ~Pilot ();
};

const int maxpilots = 5;

class PilotList
{
  public:
  int aktpilots, aktpilot;
  Pilot *pilot [maxpilots];

  void load (char *fname);
  void save (char *fname);
  PilotList (char *fname);
  ~PilotList ();
  void rm ();
  void add (char *name);
};

#endif
