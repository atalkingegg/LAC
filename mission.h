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

/* This file contains all mission data and definitions (really trivial). */

#ifndef IS_MISSION_H
#define IS_MISSION_H

#include "model.h" // ok
#include "aiobject.h" // ok

#define MISSION_DEMO 0
#define MISSION_TUTORIAL 8
#define MISSION_TUTORIAL3 10
#define MISSION_TUTORIAL2 12
#define MISSION_TUTORIAL4 14
#define MISSION_NETWORKBATTLE01 20
#define MISSION_NETWORKBATTLE02 21
#define MISSION_NETWORKBATTLE03 23
#define MISSION_FREEFLIGHTWW2 30
#define MISSION_HEADTOHEAD00 32

class Mission
{
  protected:
  void autoLFBriefing ();

  public:
  int timer; // mission timer
  int maxtime; // maximum time to get a time bonus
  char briefing [1024]; // briefing text
  char name [1024]; // mission title
  int id; // mission id
  int selfighter [3]; // fighter to choose
  int selfighters, wantfighter; // selected fighter
  int selweapon [3]; // weapon pack to choose
  int selweapons, wantweapon; // selected weapon pack
  int alliedpilot [10]; // id of allied pilots
  CColor textcolor; // standard text color to blend in during flight
  int difficulty; // difficulty level for THIS mission
  int heading; // define northern direction (the sun is at 0 degree)
  int state; // different mission states (individual meaning)
  int physics = 1; // action=0 or simulation=1

  Mission ();
  virtual ~Mission () {}
  void playerInit ();
  void alliedInit (int fighterid, int pilotid, AIObj *aiobj);
  void init ();
  virtual void start (); // custom definitions for a mission
  virtual int processtimer (Uint32 dt); // custom definitions controlled by the timer, mission success/failure
  virtual void draw (); // custom definitions that have to be drawn
  void invertZ (); // invert Z coordinate of all fighters and objects
};

class MissionDemo1 : public Mission
{
  public:
  MissionDemo1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTutorial1 : public Mission
{
  public:
  MissionTutorial1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTutorial2 : public Mission
{
  public:
  MissionTutorial2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTutorial3 : public Mission
{
  public:
  int laststate, texttimer;
  MissionTutorial3 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTutorial4 : public Mission
{
  public:
  MissionTutorial4 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionDogfight1 : public Mission
{
  public:
  int laststate, texttimer;
  MissionDogfight1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionHeadToHead00 : public Mission
{
  public:
  int laststate, texttimer;
  MissionHeadToHead00 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTurkeyShoot : public Mission
{
  public:
  int laststate, texttimer;
  MissionTurkeyShoot ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionEveryManForHimself : public Mission
{
  public:
  int laststate, texttimer;
  MissionEveryManForHimself ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTeamsMissilesAndGuns : public Mission
{
  public:
  int laststate, texttimer;
  MissionTeamsMissilesAndGuns ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionNetworkBattle01 : public Mission
{
  public:
  int laststate, texttimer;
  MissionNetworkBattle01 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionNetworkBattle02 : public Mission
{
  public:
  int laststate, texttimer;
  MissionNetworkBattle02 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionNetworkBattle03 : public Mission
{
  public:
  int laststate, texttimer;
  MissionNetworkBattle03 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionFreeFlightWW2 : public Mission
{
  public:
  int laststate, texttimer;
  MissionFreeFlightWW2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionJetbait1 : public Mission
{
  public:
  int laststate, texttimer;
  MissionJetbait1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTeamBase1 : public Mission
{
  public:
  int laststate, texttimer;
  int team1x, team1y, team2x, team2y;
  MissionTeamBase1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionWaves1 : public Mission
{
  public:
  int laststate, texttimer;
  MissionWaves1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTest1 : public Mission
{
  public:
  MissionTest1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTest2 : public Mission
{
  public:
  MissionTest2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTransport : public Mission
{
  public:
  MissionTransport ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionConvoy : public Mission
{
  public:
  MissionConvoy ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionDogfight2 : public Mission
{
  public:
  MissionDogfight2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionAirBattle : public Mission
{
  public:
  MissionAirBattle ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionGround1 : public Mission
{
  public:
  MissionGround1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionScout : public Mission
{
  public:
  MissionScout ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionBase : public Mission
{
  public:
  MissionBase ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionDepot : public Mission
{
  public:
  MissionDepot ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionDefend1 : public Mission
{
  public:
  MissionDefend1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionDogfight3 : public Mission
{
  public:
  MissionDogfight3 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTank1 : public Mission
{
  public:
  MissionTank1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionConvoy2 : public Mission
{
  public:
  MissionConvoy2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionShip1 : public Mission
{
  public:
  MissionShip1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionShip2 : public Mission
{
  public:
  MissionShip2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionShip3 : public Mission
{
  public:
  MissionShip3 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionCanyon1 : public Mission
{
  public:
  MissionCanyon1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionCanyon2 : public Mission
{
  public:
  MissionCanyon2 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionCanyon3 : public Mission
{
  public:
  MissionCanyon3 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMoonDefense1 : public Mission
{
  public:
  MissionMoonDefense1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMoonBattle : public Mission
{
  public:
  MissionMoonBattle ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMoonDogfight1 : public Mission
{
  public:
  MissionMoonDogfight1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionTunnel1 : public Mission
{
  public:
  MissionTunnel1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMoonBase1 : public Mission
{
  public:
  MissionMoonBase1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMoon1 : public Mission
{
  public:
  MissionMoon1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

class MissionMultiDogfight1 : public Mission
{
  public:
  MissionMultiDogfight1 ();
  virtual void start ();
  virtual int processtimer (Uint32 dt);
  virtual void draw ();
};

extern Mission *mission;
extern Mission *missionnew;

#endif
