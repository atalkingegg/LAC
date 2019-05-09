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

//##################################################################################################
// Headers needed by main.cpp
//##################################################################################################
#include <stdio.h> /* sprintf */
#include <SDL2/SDL.h> /* SDL_Window */
#include <SDL2/SDL_mixer.h> /* Mix_Chunk */

#include "common.h" /* pulls in stdint.h, defines Uint32 needed everywhere */
#include "mathtab.h" /* MAXSMOKEELEM */
#include "vertexarray.h" /* VertexArray */
#include "model.h" /* CColor, depends on mathtab.h and vertexarray.h */
#include "object.h" /* CSpaceObj, depends on model.h */
#include "NetworkApi.h" /* LacUdpApiPacket */
#include "pilots.h" /* PilotList */
#include "audio.h" /* SoundSystem, depends on SDL_mixer.h */
#include "main.h" /* ThreeDObjects, depends on pilots.h, audio.h  */
#include "conf.h" /* mouse_reverse */
#include "mission.h" /* self stuff */

//##################################################################################################

//#include <stdio.h>
//#include <string.h>
//#include "aiobject.h"
//#include "common.h"
//#include "conf.h"
//#include "glland.h"
//#include "mission.h"
//#include "main.h"
//#include "mathtab.h"
//#include "NetworkApi.h"

extern bool AirfieldRepairRateNormalForBlueTeam;
extern bool AirfieldRepairRateNormalForRedTeam;
extern bool AirfieldRepairsAcceleratedForBlueTeam;
extern bool AirfieldRepairsAcceleratedForRedTeam ;
extern bool AirfieldRepairsStoppedForBlueTeam;
extern bool AirfieldRepairsStoppedForRedTeam;
extern bool AutoPilotActive;
extern bool ClearSpeedHistoryArrayFlag;
extern bool LandedAtSafeSpeed;
extern bool MissionRunning;
extern bool NetworkPlayerKilled;
extern bool NewSystemMessageNeedsScrolling;
extern bool NoMissionHasYetCommenced;
extern bool PeerPacketReceivedInThisMissionState;
extern bool RadarReflectorBlueHasBeenDestroyedAtLeastOnce;
extern bool RadarReflectorRedHasBeenDestroyedAtLeastOnce;
extern bool WindNoiseOn;

extern char DebugBuf[];
extern char MissionEntryLatch[];
extern char SystemMessageBufferA[];

extern unsigned char AirfieldChosenForLanding;
extern unsigned char AirfieldRequested;
extern unsigned char BattleshipSunkAlertCountBlue;
extern unsigned char BattleshipSunkAlertCountRed;
extern unsigned char DefaultAircraft;
extern unsigned char MaxPlayersInCurrentMission;
extern unsigned char MissionHeadToHead00State;
extern unsigned char MissionIdNegotiationCount;
extern unsigned char MissionOutPacketCount;
extern unsigned char MyNetworkId;
extern unsigned char NetworkApiVersion;
extern unsigned char NetworkOpponent;
extern unsigned char MissionStateNetworkBattle;

extern int AirfieldXCenter;
extern int AirfieldXMax;
extern int AirfieldXMin;
extern int AirfieldYCenter;
extern int AirfieldYMax;
extern int AirfieldYMin;
extern int BattleDamageRiskTimer;
extern int HudLadderBarsOnOff;
extern int HudOnOff;
extern int IffOnOff;
extern int lastDurability;
extern int MissionAircraftDamaged;
extern int MapViewOnOff;
extern int MissionNetworkBattle01PriorAircraft[];
extern int MissionNetworkBattle02PriorAircraft[];
extern int MissionNetworkBattle03PriorAircraft[];
extern int MissionNumber;
extern int NetworkMode; // Mode 0 = PeerToPeer. Mode 1 = ClientServer.
extern int NetworkTransmitTimer;
extern int NetworkTransmitTimerInterval;
extern int PlayerAircraftType;
extern int PlayersOriginalDurability;
extern int PriorStateEndTimer;
extern int RadarOnOff;
extern int RadarWasOn;
extern int RadarZoom; 
extern int StateTransitionTimer;
extern int StaticObjectUpdateTimer;
extern int TrimElevatorSetting;
extern int TrimRudderSetting;

extern unsigned int key_DROPCHAFF;
extern unsigned int key_DROPFLARE;
extern unsigned int key_PRIMARY;
extern unsigned int key_RadarZoomIn;
extern unsigned int key_RadarZoomOut;
extern unsigned int key_SECONDARY;
extern unsigned int key_TARGETNEXT;
extern unsigned int key_TARGETPREVIOUS;
extern unsigned int key_WEAPONSELECT;
extern unsigned int MissedPacketCount;
extern unsigned int NetworkReceiveTimer;
extern unsigned int NetworkReceiveTimerInterval;

extern Uint32 DeltaTime;
extern Uint32 Me163LandingTimer;
extern Uint32 MissionEndingTimer;
extern Uint32 MissionEndingTimer2;
extern Uint32 MissionNetworkBattle01Timer;
extern Uint32 MissionNetworkBattle02Timer;
extern Uint32 MissionNetworkBattle03Timer;
extern Uint32 MissionNetworkBattleRadarTimer;
extern Uint32 NetworkApiPriorTimer[32];

extern float AutoPilotAltitude;
extern float blackout;
extern float CalculatedDamageDueToCurrentRisk;
extern float DamageToClaim;
extern float FuelLevelAtLastLanding;
extern float NetDeltaX;
extern float NetDeltaY;
extern float NetDeltaZ;
extern float PlayersOriginalMaxGamma;
extern float PlayersOriginalRollRate;
extern float PlayersOriginalMaxThrust;
extern float ThrustReadiness;
extern float redout;
extern float ScreenFOVx;
extern float ScreenFOVy;
extern float SeaLevel;

extern float NetworkApiPriorXPosition[];
extern float NetworkApiPriorYPosition[];
extern float NetworkApiPriorZPosition[];

extern Uint32 NetworkApiPriorTimer[32];

extern LacUdpApiPacket InPacket;
extern LacUdpApiPacket OutPacket;

extern int  DiscardAnyInPacketsInQueue();
extern int  GetNetworkApiPacket();
extern int  OpenClientUdpSocket();
extern int  OpenUdpSocketForReceiving();
extern int  OpenUdpSocketForSending();

extern void ConfigureClientUdpSocket();
extern void ConfigureIncomingUdpSocket();
extern void ConfigureOutgoingUdpSocket();
extern void DegradeFlightModelDueToOrdnanceLoad();
extern void event_HudLadderBarsOnOff();
extern void event_IffOnOff();
extern void event_MapViewOnOff();
extern void event_MapZoomIn();
extern void event_RadarOnOff();
extern void event_RadarZoomIn();
extern void event_RadarZoomOut();
extern void event_targetNext ();
extern void event_thrustUp ();
extern void event_ToggleUndercarriage();
extern void event_TrimElevatorDn();
extern void event_TrimElevatorUp();
extern void event_ZoomFovOut();
extern void game_quit ();
extern void LoadServerIpAddress();
extern void LoadVariablesFromNetworkApiPacket();
extern void SendNetworkApiPacket();
extern void UpdateOnlineScoreLogFileWithCalculatedRisks();
extern void UpdateOnlineScoreLogFileWithNewSorties();

bool MissionHeadToHead00RetrieveFirstDamageDescription();
bool MissionNetworkBattle01RetrieveFirstDamageDescription();
bool MissionNetworkBattle02RetrieveFirstDamageDescription();
bool MissionNetworkBattle03RetrieveFirstDamageDescription();

void event_FlapsDN();
void MissionHeadToHead00LoadVariablesFromNetworkApiPacket(int);
void ProcessUdpObjFlightDetails();
void LoadVariablesFromNetworkApiPacket(int);
void RearmRefuelRepair();

void ArmPlayerAtRequestedField()
{
display ((char *)"Function Entry: ArmPlayerAtRequestedField()", LOG_MOST);
sprintf (DebugBuf, "ArmPlayerAtRequestedField() AirfieldRequested = %d\n", AirfieldRequested);
display (DebugBuf, LOG_MOST);
sprintf (DebugBuf, "fplayer->missiles[0] = %d\n", fplayer->missiles[0]);
display (DebugBuf, LOG_MOST);
if (MyNetworkId %2)
   { 
   display ((char *)"ArmPlayerAtRequestedField() RedTeam", LOG_MOST);
   fplayer->party = 1;
   if (AirfieldRequested == 1)
      { 
      display ((char *)"ArmPlayerAtRequestedField() RedTeam Field 1", LOG_MOST);
      display ((char *)"SPAWNING AT RED HQ (29)", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         { 
         fplayer->missiles [0] *= 0.50; // Cut bombload to 50% from this field.
         }
      fplayer->tl->x = 300; 
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested == 2)
      { 
      display ((char *)"ArmPlayerAtRequestedField() RedTeam Field 2", LOG_MOST);
      display ((char *)"SPAWNING AT RED FIELD2", LOG_MOST);
      fplayer->tl->x = 800; 
      fplayer->tl->z = 5.00;
      if (fplayer->missiles [0] >= 6)
         { 
         fplayer->missiles [0] *= 0.75; // Cut bombload to 75% from this field.
         }
      }
   if (AirfieldRequested > 2)
      { 
      display ((char *)"ArmPlayerAtRequestedField() RedTeam Field > 2", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         { 
         AirfieldRequested = 4;
         display ((char *)"SPAWNING AT RED FIELD4", LOG_MOST);
         fplayer->tl->x = 1800;
         fplayer->tl->z = 5.00;
         }
      else
         { 
         AirfieldRequested = 3;
         display ((char *)"SPAWNING AT RED FIELD3", LOG_MOST);
         fplayer->tl->x = 1300;
         fplayer->tl->z = 5.00;
         }
      
      if (fplayer->id == BOMBER_B29)
         { 
         display ((char *)"SPAWNING RedTeam B29 AT RED FIELD5", LOG_MOST);
         fplayer->tl->x = 2300;
         fplayer->tl->z = 5.00;
         }
      }
   }
else
   { 
   display ((char *)"ArmPlayerAtRequestedField() BlueTeam", LOG_MOST);
   fplayer->party = 0;
   if (AirfieldRequested == 1)
      { 
      display ((char *)"ArmPlayerAtRequestedField() BlueTeam Field 1", LOG_MOST);
      display ((char *)"SPAWNING AT BLUE HQ (28)", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         { 
         fplayer->missiles [0] *= 0.50; // Cut bombload to 50% from this field.
         }
      fplayer->tl->x = -400; 
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested == 2)
      { 
      display ((char *)"ArmPlayerAtRequestedField() BlueTeam Field 2", LOG_MOST);
      display ((char *)"SPAWNING AT BLUE FIELD2", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         { 
         fplayer->missiles [0] *= 0.75; // Cut bombload to 75% from this field.
         }
      fplayer->tl->x = -900; 
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested > 2)
      { 
      display ((char *)"ArmPlayerAtRequestedField() BlueTeam Field > 2", LOG_MOST);
      if (fplayer->missiles[0] >= 6)
         { 
         AirfieldRequested = 4;
         display ((char *)"SPAWNING AT BLUE FIELD4", LOG_MOST);
         fplayer->tl->x = -1900;
         fplayer->tl->z = 5.00;
         }
      else
         { 
         AirfieldRequested = 3;
         display ((char *)"SPAWNING AT BLUE FIELD3", LOG_MOST);
         fplayer->tl->x = -1400;
         fplayer->tl->z = 5.00;
         }
      
      if (fplayer->id == BOMBER_B29)
         { 
         display ((char *)"SPAWNING BlueTeam B29 AT BLUE FIELD5", LOG_MOST);
         fplayer->tl->x = -2400;
         fplayer->tl->z = 5.00;
         }
      }
   }
} 

void AutoPilot()
{
if (AutoPilotActive)
   { 
   float AltitudeDifferential = ThreeDObjects[0]->tl->y - AutoPilotAltitude;
   if (fabs(AltitudeDifferential) < 10)
      { 
      TrimElevatorSetting /=2; 
      }
   float GammaDifferential = ThreeDObjects[0]->gamma - 180;
   if (fabs(GammaDifferential) < 1)
      { 
      TrimElevatorSetting /=2; 
      }
   if (AltitudeDifferential < 4)
      { 
      TrimElevatorSetting += (int)(AltitudeDifferential * -20);
      }
   if (AltitudeDifferential > 4)
      { 
      TrimElevatorSetting -= (int)(AltitudeDifferential * 20);
      }
   
   if (TrimElevatorSetting > 2000)
      {
      TrimElevatorSetting = 2000;
      }
   if (TrimElevatorSetting < -2000)
      {
      TrimElevatorSetting = -2000;
      }
   
   if (ThreeDObjects[0]->gamma > 190)
      {
      ThreeDObjects[0]->gamma = 190;
      TrimElevatorSetting /=2; 
      }
   if (ThreeDObjects[0]->gamma < 170)
      {
      ThreeDObjects[0]->gamma = 170;
      TrimElevatorSetting /=2; 
      }
   
   if (ThreeDObjects[0]->theta > 5 )
      {
      ThreeDObjects[0]->theta -=2;
      }
   else if (ThreeDObjects[0]->theta < -5)
      {
      ThreeDObjects[0]->theta +=2;
      }
   if (ThreeDObjects[0]->theta > 2)
      {
      ThreeDObjects[0]->theta -= 0.5;
      }
   else if (ThreeDObjects[0]->theta < -2)
      {
      ThreeDObjects[0]->theta += 0.5;
      }
   if (TrimRudderSetting != 0)
      { 
      ThreeDObjects[0]->theta = TrimRudderSetting * -0.0005;
      }
   }
} 

void CalcDamageRiskFromNearbyOpposition()
{
display ((char *)"CalcDamageRiskFromNearbyOpposition()", LOG_ALL);
static float CalculatedDamageFromAirfields = 0;
static float CalculatedDamageFromAircraft[11] = {0};
unsigned char Mission3dObject; 
float XDisplacementTemp;
float YDisplacementTemp;
float ZDisplacementTemp;
float TotalXYZDisplacement;
float HostileDamageState;
float JinkingStrength = 0;
CalculatedDamageDueToCurrentRisk = 0;
for (Mission3dObject = 1; Mission3dObject<=29; Mission3dObject++)
   { 
   if (ThreeDObjects[Mission3dObject]->active == true && Mission3dObject%2 != MyNetworkId%2)
      { 
      XDisplacementTemp = fabs (fplayer->tl->x - ThreeDObjects[Mission3dObject]->tl->x);
      if (XDisplacementTemp < 90.0)
         { 
         ZDisplacementTemp = fabs (fplayer->tl->z - ThreeDObjects[Mission3dObject]->tl->z);
         if (ZDisplacementTemp < 90.0)
            { 
            YDisplacementTemp = fabs (fplayer->tl->y - ThreeDObjects[Mission3dObject]->tl->y);
            if (YDisplacementTemp < 600)
               { 
               TotalXYZDisplacement = XDisplacementTemp + YDisplacementTemp/2 + ZDisplacementTemp;
               if (TotalXYZDisplacement < 5.0)
                  { 
                  TotalXYZDisplacement = 5.0;
                  }
               if ((Mission3dObject == 28 || Mission3dObject == 29)  && fplayer->tl->y > (SeaLevel-30))
                  { 
                  
                  CalculatedDamageFromAirfields = 4800/TotalXYZDisplacement;
                  if (
                     fplayer->id == BOMBER_B17 ||
                     fplayer->id == BOMBER_B24 ||
                     fplayer->id == BOMBER_G5M ||
                     fplayer->id == BOMBER_HE111 ||
                     fplayer->id == BOMBER_LANCASTER ||
                     fplayer->id == BOMBER_B29
                     )
                     { 
                     CalculatedDamageFromAirfields *= 20; // Big Bombers get hit more often.
                     display ((char *)"CalcDamageRiskFromNearbyOpposition() 20x big target penalty.", LOG_MOST);
                     }
                  else if (
                          fplayer->id == BOMBER_B25 ||
                          fplayer->id == BOMBER_B26 ||
                          fplayer->id == BOMBER_B5N ||
                          fplayer->id == BOMBER_DAUNTLESS ||
                          fplayer->id == BOMBER_DORNIER ||
                          fplayer->id == BOMBER_JU87 ||
                          fplayer->id == BOMBER_JU88 ||
                          fplayer->id == BOMBER_MOSQUITOB ||
                          fplayer->id == BOMBER_SB2C||
                          fplayer->id == BOMBER_TBF
                          )
                     { 
                     CalculatedDamageFromAirfields *=7; // Medium bombers are also easier to hit.
                     display ((char *)"CalcDamageRiskFromNearbyOpposition() 7x Medium bomber penalty.", LOG_MOST);
                     }
                  int volume = (int)(CalculatedDamageFromAirfields * 36) -110;
                  if (volume > 127)
                     {
                     volume = 127;
                     }
                  
                  HostileDamageState = ThreeDObjects[Mission3dObject]->Durability / ThreeDObjects[Mission3dObject]->maxDurability;
                  CalculatedDamageFromAirfields *= HostileDamageState;
                  sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Damage due to airfield proximity and damage state = %f", CalculatedDamageFromAirfields);
                  display (DebugBuf, LOG_MOST);
                  if (HostileDamageState > 0.4)
                     { 
                     sound->setVolume (SOUND_BEEP1, volume); 
                     sound->play (SOUND_BEEP1, false);
                     }
                  else
                     { 
                     CalculatedDamageFromAirfields *= .30; 
                     }

                  if (blackout > 0)
                     { 
                     JinkingStrength = blackout *2;
                     if (JinkingStrength > 10.0)
                        {
                        JinkingStrength = 10.0;
                        CalculatedDamageFromAirfields = 0.0; 
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                        }
                     if (JinkingStrength > 1.0)
                        {
                        CalculatedDamageFromAirfields /= JinkingStrength;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                        }
                     }
                  else if (redout > 0)
                     { 
                     JinkingStrength = redout * 4;
                     if (JinkingStrength > 10.0)
                        {
                        JinkingStrength = 10.0;
                        CalculatedDamageFromAirfields = 0.0; 
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                        }
                     if (JinkingStrength > 1.0)
                        {
                        CalculatedDamageFromAirfields /= JinkingStrength;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                        }
                     }
                  
                  if (myrandom(100) > 50)
                     { 
                     CalculatedDamageFromAirfields = 0.0;
                     display ((char *)"CalcDamageRiskFromNearbyOpposition() myrandom() helped.", LOG_MOST);
                     }
                  
                  if (JinkingStrength >= 10.0)
                     { 
                     CalculatedDamageFromAirfields = 0.0;
                     }
                  
                  if (fplayer->realspeed > .32)
                     {
                     if (myrandom(100) > 20)
                        { 
                        CalculatedDamageFromAirfields = 0;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() Airfield guns missed due to our high speed.", LOG_MOST);
                        }
                     }
                  } 
               
               if (
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B17)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B24)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_JU87)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_G5M)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B25)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B26)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_LANCASTER) ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B29)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B5N)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_DAUNTLESS) ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_DORNIER)   ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_HE111)     ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_JU88)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_SB2C)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_TBF)       ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_D3A)      ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_ME110)    ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_IL2)
                  )
                  { 
                  sound->setVolume (SOUND_BEEP1, 40); 
                  sound->play (SOUND_BEEP1, false);
                  
                  if (XDisplacementTemp < 30 && YDisplacementTemp < 30 && ZDisplacementTemp < 30) 
                     { 
                     CalculatedDamageFromAircraft[Mission3dObject] = 100/TotalXYZDisplacement; // In range
                     int volume = (int)(CalculatedDamageFromAircraft[Mission3dObject] * 36)-110;
                     if (volume > 127)
                        {
                        volume = 127;
                        }
                     sound->setVolume (SOUND_BEEP1, volume); 
                     sound->play (SOUND_BEEP1, false);
                     
                     float PhiDifferential = fabs(fplayer->phi - ThreeDObjects[Mission3dObject]->phi);
                     float GammaDifferential = fabs(fplayer->gamma - ThreeDObjects[Mission3dObject]->gamma);
                     sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Damage due to bomber proximity = %f from bomber %d", CalculatedDamageFromAircraft[Mission3dObject], Mission3dObject);
                     display (DebugBuf, LOG_MOST);
                     if (PhiDifferential < 14.0 && GammaDifferential < 6.0)
                        { 
                        CalculatedDamageFromAircraft[Mission3dObject] *= 2.0; // Serious hits are far more likely in this circumstance!
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() 2x Damage Penalty!", LOG_MOST);
                        }
                     else if (PhiDifferential > 80 || GammaDifferential > 20)
                        { 
                        CalculatedDamageFromAircraft[Mission3dObject] /= 3.0;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() Radical intercept helped.", LOG_MOST);
                        }
                     
                     HostileDamageState = ThreeDObjects[Mission3dObject]->Durability / ThreeDObjects[Mission3dObject]->maxDurability;
                     CalculatedDamageFromAircraft[Mission3dObject] *= HostileDamageState;
                     
                     if (blackout > 0)
                        { 
                        JinkingStrength = blackout * 3;
                        if (JinkingStrength > 10.0)
                           {
                           JinkingStrength = 10.0;
                           CalculatedDamageFromAircraft[Mission3dObject] = 0.0; 
                           }
                        if (JinkingStrength > 1.0)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] /= JinkingStrength;
                           display ((char *)"CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                           }
                        }
                     else if (redout > 0)
                        { 
                        JinkingStrength = redout * 5;
                        if (JinkingStrength > 10.0)
                           {
                           JinkingStrength = 10.0;
                           CalculatedDamageFromAircraft[Mission3dObject] = 0.0; 
                           }
                        if (JinkingStrength > 1.0)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] /= JinkingStrength;
                           display ((char *)"CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                           }
                        }
                     
                     if (myrandom(100) > 50)
                        { 
                        CalculatedDamageFromAircraft[Mission3dObject] = 0.0;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() myrandom() helped.", LOG_MOST);
                        }
                     
                     if (fplayer->realspeed > (ThreeDObjects[Mission3dObject]->realspeed * 1.3) )
                        { 
                        if (myrandom(100) > 20)
                           { 
                           CalculatedDamageFromAircraft[Mission3dObject] = 0;
                           display ((char *)"CalcDamageRiskFromNearbyOpposition() Bomber guns missed due to our high speed.", LOG_MOST);
                           }
                        }
                     
                     if (fplayer->realspeed > (ThreeDObjects[Mission3dObject]->realspeed * 1.5) )
                        { 
                        if (myrandom(100) > 10)
                           { 
                           CalculatedDamageFromAircraft[Mission3dObject] = 0;
                           display ((char *)"CalcDamageRiskFromNearbyOpposition() Bomber guns missed due to our high speed.", LOG_MOST);
                           }
                        }
                     if (ThreeDObjects[Mission3dObject]->tl->y < 25)
                        { 
                        CalculatedDamageFromAircraft[Mission3dObject] = 0;
                        display ((char *)"CalcDamageRiskFromNearbyOpposition() Bomber altitude too low to fire guns.", LOG_MOST);
                        }
                     } 
                  else
                     { 
                     CalculatedDamageFromAircraft[Mission3dObject] = 0.0; // Out of range
                     }
                  
                  float LethalityFactor = ((float)(ThreeDObjects[Mission3dObject]->DefensiveLethality) / 10.0);
                  CalculatedDamageFromAircraft[Mission3dObject] *= LethalityFactor;
                  } // end of section handling bombers in the general vicinity
               } // end of logic checking x, y, and z displacements
            } 
         } 
      } 
   } 
CalculatedDamageDueToCurrentRisk = CalculatedDamageFromAirfields;
if (CalculatedDamageFromAirfields > 0.0)
   {
   sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Airfield damaged us by %f", CalculatedDamageFromAirfields);
   display (DebugBuf, LOG_MOST);
   CalculatedDamageFromAirfields = 0;
   }
for (Mission3dObject = 1; Mission3dObject <=10; Mission3dObject++)
   {
   if (CalculatedDamageFromAircraft[Mission3dObject] > 0.0)
      { 
      CalculatedDamageDueToCurrentRisk += CalculatedDamageFromAircraft[Mission3dObject];
      
      sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Bomber %d damaged us by %f", Mission3dObject, CalculatedDamageFromAircraft[Mission3dObject]);
      display (DebugBuf, LOG_MOST);
      CalculatedDamageFromAircraft[Mission3dObject] = 0; 
      }
   }
if (CalculatedDamageDueToCurrentRisk > 0 && CalculatedDamageDueToCurrentRisk < 0.22)
   { 
   display ((char *)"CalcDamageRiskFromNearbyOpposition() Discarding negligible value of CalculatedDamageDueToCurrentRisk.", LOG_MOST);
   CalculatedDamageDueToCurrentRisk = 0;
   }
if (CalculatedDamageDueToCurrentRisk > 0.0)
   {
   
   for (Mission3dObject = 1; Mission3dObject<=10; Mission3dObject++)
      { 
      if (ThreeDObjects[Mission3dObject]->active == true)
         { 
         if (Mission3dObject%2 == MyNetworkId%2)
            { 
            if ((ThreeDObjects[Mission3dObject]->id >FIGHTER1) && (ThreeDObjects[Mission3dObject]->id <FIGHTER2))
               { 
               float XDisplacementTemp1 = fabs (fplayer->tl->x - ThreeDObjects[Mission3dObject]->tl->x);
               float YDisplacementTemp1 = fabs (fplayer->tl->y - ThreeDObjects[Mission3dObject]->tl->y);
               float ZDisplacementTemp1 = fabs (fplayer->tl->z - ThreeDObjects[Mission3dObject]->tl->z);
               if (XDisplacementTemp1 < 40 && YDisplacementTemp1 < 40 && ZDisplacementTemp1 < 40)
                  { 
                  CalculatedDamageDueToCurrentRisk *= 0.5; // A nearby ally shares our damage risk.
                  display ((char *)"CalcDamageRiskFromNearbyOpposition() Ally sharing helped.", LOG_MOST);
                  }
               else
                  { 
                  ; // Make no adjustment if this aircraft is too far away to share damage risk.
                  }
               }
            }
         }
      }
   }
if (CalculatedDamageDueToCurrentRisk > 0.0)
   {
   sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Total CalculatedDamageDuetoCurrentRisk = %f", CalculatedDamageDueToCurrentRisk);
   display (DebugBuf, LOG_MOST);
   }
} 

void ConfigureOrdnanceForOnlineMissions()
{
int i2;
for (i2 = 0; i2 < missiletypes; i2 ++)
    {
    fplayer->missiles [i2] = 4; 
    }
if (
    fplayer->id == FIGHTER_P38L      ||
    fplayer->id == FIGHTER_A6M2      ||
    fplayer->id == FIGHTER_F4U       ||
    fplayer->id == FIGHTER_F4F       ||
    fplayer->id == FIGHTER_F6F       ||
    fplayer->id == FIGHTER_P47D      ||
    fplayer->id == FIGHTER_P51D      ||
    fplayer->id == FIGHTER_FW190     ||
    fplayer->id == FIGHTER_IL16      ||
    fplayer->id == FIGHTER_FIATG55   ||
    fplayer->id == FIGHTER_ME109G    ||
    fplayer->id == FIGHTER_P39       ||
    fplayer->id == FIGHTER_P40       ||
    fplayer->id == FIGHTER_HURRICANE ||
    fplayer->id == FIGHTER_SPIT9     ||
    fplayer->id == FIGHTER_KI43      ||
    fplayer->id == FIGHTER_LA5       ||
    fplayer->id == FIGHTER_LA7       ||
    fplayer->id == FIGHTER_IL2       ||
    fplayer->id == FIGHTER_MACCIC202 ||
    fplayer->id == FIGHTER_TYPHOON   ||
    fplayer->id == FIGHTER_YAK1      ||
    fplayer->id == FIGHTER_N1K1      ||
    fplayer->id == FIGHTER_YAK9      ||
    fplayer->id == BOMBER_B29        ||
    fplayer->id == FIGHTER_DW520     ||
    fplayer->id == BOMBER_SB2C       ||
    fplayer->id == BOMBER_TBF        ||
    fplayer->id == FIGHTER_ME163     ||
    fplayer->id == FIGHTER_TEMPEST   ||
    fplayer->id == FIGHTER_D3A       ||
    fplayer->id == BOMBER_B5N        ||
    fplayer->id == BOMBER_DAUNTLESS  ||
    fplayer->id == FIGHTER_ME110     ||
    fplayer->id == BOMBER_DORNIER    ||
    fplayer->id == BOMBER_HE111      ||
    fplayer->id == BOMBER_JU88       ||
    fplayer->id == FIGHTER_KI84      ||
    fplayer->id == FIGHTER_KI61      ||
    fplayer->id == BOMBER_JU87       ||
    fplayer->id == BOMBER_B17        ||
    fplayer->id == BOMBER_B24        ||
    fplayer->id == BOMBER_G5M        ||
    fplayer->id == BOMBER_B25        ||
    fplayer->id == BOMBER_B26        ||
    fplayer->id == BOMBER_LANCASTER  ||
    fplayer->id == BOMBER_MOSQUITOB  ||
    fplayer->id == FIGHTER_GENERIC01 ||
    fplayer->id == FIGHTER_A6M5      ||
    fplayer->id == FIGHTER_SPIT5     ||
    fplayer->id == FIGHTER_P51B      ||
    fplayer->id == FIGHTER_P47B      ||
    fplayer->id == FIGHTER_ME109F    ||
    fplayer->id == FIGHTER_P38F

    )
   { 
   for (i2 = 0; i2 < missiletypes; i2 ++)
       { 
       fplayer->missiles [i2] = 0;
       }
   
   for (i2 = 0; i2 < missiletypes; i2 ++)
       {
       if (i2 == 0)
          { 
          if (fplayer->id == FIGHTER_P38L)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_A6M2)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_F4U)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_F4F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F6F)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P47D)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_P51D)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_FW190)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL16)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_FIATG55)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME109G)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P39)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P40)
             {
             fplayer->missiles [i2] = 3;
             }
          if (fplayer->id == FIGHTER_HURRICANE)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_SPIT9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI43)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_N1K1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_YAK9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA5)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_LA7)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_MACCIC202)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_TYPHOON)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_YAK1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 40;
             }
          if (fplayer->id == FIGHTER_DW520)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_SB2C)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == BOMBER_TBF)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_ME163)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TEMPEST)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_D3A)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == BOMBER_B5N)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == BOMBER_DAUNTLESS)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_ME110)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DORNIER)
             {
             fplayer->missiles [i2] = 5;
             }
          if (fplayer->id == BOMBER_HE111)
             {
             fplayer->missiles [i2] = 12;
             }
          if (fplayer->id == BOMBER_JU88)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_KI84)
             {
             fplayer->missiles [i2] = 3;
             }
          if (fplayer->id == FIGHTER_KI61)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_GENERIC01)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_A6M5)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_SPIT5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P51B)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P47B)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_ME109F)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P38F)
             {
             fplayer->missiles [i2] = 1;
             }

          if (fplayer->id == FIGHTER_IL2)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == BOMBER_JU87)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == BOMBER_G5M)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == BOMBER_B25)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == BOMBER_B26)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_B17)
             {
             fplayer->missiles [i2] = 15;
             }
          if (fplayer->id == BOMBER_MOSQUITOB)
             {
             fplayer->ammo = 0; // Mosquito Bomber version carried no guns at all.
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_LANCASTER)
             {
             fplayer->missiles [i2] = 28;
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 40;
             }
           if (fplayer->id == BOMBER_B24)
             {
             fplayer->missiles [i2] = 16;
             }
          }
       if (i2 == 5)
          { 
          if (fplayer->id == FIGHTER_P38L)
             {
             fplayer->missiles [i2] = 10;
             }
          if (fplayer->id == FIGHTER_A6M2)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F4U)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_F4F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F6F)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_P47D)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_P51D)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_FW190)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL16)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_FIATG55)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME109G)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_P39)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P40)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_HURRICANE)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_SPIT9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI43)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_N1K1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_YAK9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA7)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL2)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_JU87)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_G5M)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B25)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_B26)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B17)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B24)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_LANCASTER)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_MOSQUITOB)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_MACCIC202)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TYPHOON)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_YAK1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_DW520)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_SB2C)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_TBF)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_ME163)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TEMPEST)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_D3A)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B5N)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DAUNTLESS)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME110)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DORNIER)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_HE111)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_JU88)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI84)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI61)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_GENERIC01)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_A6M5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_SPIT5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P51B)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_P47B)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_ME109F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P38F)
             {
             fplayer->missiles [i2] = 6;
             }
          }
       }
   }
} 

void DetermineCurrentAirfield()
{
if (fplayer->tl->y > (SeaLevel + 1000))
   { 
   AirfieldChosenForLanding = 0;
   return;
   }
if (fplayer->tl->z >10.0)
   { 
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->z < -10.0)
   { 
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->x < -1500.0)
   { 
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->x > 1500.0)
   { 
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->x > 1100.0)
   { 
   AirfieldChosenForLanding = 6;
   return;
   }
if (fplayer->tl->x > 600.0)
   { 
   AirfieldChosenForLanding = 5;
   return;
   }
if (fplayer->tl->x > 100.0)
   { 
   AirfieldChosenForLanding = 4;
   return;
   }
if (fplayer->tl->x > -500.0)
   { 
   AirfieldChosenForLanding = 1;
   return;
   }
if (fplayer->tl->x > -1000.0)
   { 
   AirfieldChosenForLanding = 2;
   return;
   }
else
   { 
   AirfieldChosenForLanding = 3;
   return;
   }
} 

void LoadVariablesFromNetworkApiPacket(int timer)
{
   
   unsigned char PlayerNumber;
   
   extern LacUdpApiPacket InPacket;
   display ((char *) "LoadVariablesFromNetworkApiPacket()", LOG_NET);
   
   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {
      
      display ((char *)"LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ((char *)"Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      sprintf (SystemMessageBufferA, "NETWORK PROTOCOL CONFLICT WITH PLAYER %d.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      sound->setVolume (SOUND_BEEP1, 80);
      sound->play (SOUND_BEEP1, false);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         { 
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ((char *)"This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         { 
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d from player %d", InPacket.NetworkApiVersion, InPacket.UdpObjPlayerNumber);
         display (DebugBuf, LOG_ERROR);
         display ((char *)"Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1; 
   
   PlayerNumber = InPacket.UdpObjPlayerNumber;
   display ((char *)"LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);
   display ((char *)"Sentient Count =", LOG_NET);
   sprintf (DebugBuf, "%d", ThreeDObjects[PlayerNumber]->Sentient);
   display (DebugBuf, LOG_NET);
   if (PlayerNumber > 10)
      {
      display ((char *)"LoadVariablesFromNetworkApiPacket(): network PlayerNumber ID > 10 error.", LOG_MOST);
      }
   if (ThreeDObjects[PlayerNumber]->Sentient > 0 && ThreeDObjects[PlayerNumber]->Sentient <= 10)
      { 
      ThreeDObjects[PlayerNumber]->Sentient++; 
      if (ThreeDObjects[PlayerNumber]->Sentient == 7) 
         { 
         
         if (InPacket.UdpObjPlayerNumber != 1)
            { 
            ThreeDObjects[PlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 1200);
            }
         if (MissionEntryLatch[PlayerNumber] == 0)
            { 
            if (ThreeDObjects[PlayerNumber]->Durability > 1800)
               { 
               sprintf (SystemMessageBufferA, "STRATEGIC BOMBER # %d CROSSED INTO RADAR RANGE.", PlayerNumber);
               NewSystemMessageNeedsScrolling = true;
               MissionEntryLatch[PlayerNumber] = 1;
               sound->setVolume (SOUND_BEEP1, 20); 
               sound->play (SOUND_BEEP1, false);
               }
            }
         }
      }
   if (ThreeDObjects[PlayerNumber]->Sentient < 2)
      { 
      ThreeDObjects[PlayerNumber]->Sentient = 2; 
      return; // Discard the first packet.
      }
   
   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;
   
   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;
   
   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;
   
   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   NetDeltaX = InPacket.UdpObjXPosition - NetworkApiPriorXPosition[PlayerNumber];
   NetDeltaY = InPacket.UdpObjYPosition - NetworkApiPriorYPosition[PlayerNumber];
   NetDeltaZ = InPacket.UdpObjZPosition - NetworkApiPriorZPosition[PlayerNumber];
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;
   ProcessUdpObjFlightDetails();
   
   NetworkApiPriorXPosition[PlayerNumber] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[PlayerNumber] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[PlayerNumber] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[PlayerNumber]     = timer;
   if (InPacket.UdpObjDamageId == MyNetworkId)
      { // Get here if received damage claim addresses this human player
      ThreeDObjects[0]->Durability -= InPacket.UdpObjDamageAmount; // Damage this player
      sprintf (SystemMessageBufferA, "PLAYER %d DAMAGED YOUR AIRCRAFT.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      }
   else if (InPacket.UdpObjDamageAmount > 0.0)
      { // Get here if received damage claim addresses some other bot or player or mission object
      ThreeDObjects[InPacket.UdpObjDamageId]->Durability -= InPacket.UdpObjDamageAmount; // Damage addressed mission object
      if (InPacket.UdpObjDamageId > 0 && InPacket.UdpObjDamageId <=10)
         { // Get here if the damaged object was an aircraft
         if (InPacket.UdpObjDamageAmount < 1000)
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DAMAGED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         else
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DESTROYED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         NewSystemMessageNeedsScrolling = true;
         }
       else if (InPacket.UdpObjDamageId >= 11)
         { // Get here if the damaged item was NOT an aircraft
         
         if (InPacket.UdpObjDamageAmount < 100000)
            { 
            sprintf  (
                     SystemMessageBufferA,
                     "AIRCRAFT %d DAMAGED OBJECT %d BY %5.0f KILOJOULES.",
                     InPacket.UdpObjPlayerNumber,
                     InPacket.UdpObjDamageId,
                     InPacket.UdpObjDamageAmount
                     );
            if (InPacket.UdpObjDamageId == 22 || InPacket.UdpObjDamageId == 24 || InPacket.UdpObjDamageId == 26 || InPacket.UdpObjDamageId == 28)
               { 
               if (InPacket.UdpObjDamageAmount > 2000)
                  { 
                  float XDistance28 = fabs(fplayer->tl->x - ThreeDObjects[28]->tl->x);
                  float YDistance28 = fabs(fplayer->tl->y - ThreeDObjects[28]->tl->y);
                  float ZDistance28 = fabs(fplayer->tl->z - ThreeDObjects[28]->tl->z);
                  float Distance28 = XDistance28 + YDistance28 + ZDistance28;
                  int Volume28 = 16384 / ((int)Distance28 * Distance28);
                  if (Volume28 > 127)
                     {
                     Volume28 = 127;
                     }
                  sound->setVolume (SOUND_EXPLOSION1, Volume28);
                  sound->play (SOUND_EXPLOSION1, false);
                  }
               }
            if (InPacket.UdpObjDamageId == 23 || InPacket.UdpObjDamageId == 25 || InPacket.UdpObjDamageId == 27 || InPacket.UdpObjDamageId == 29)
               { 
               if (InPacket.UdpObjDamageAmount > 2000)
                  { 
                  float XDistance29 = fabs(fplayer->tl->x - ThreeDObjects[29]->tl->x);
                  float YDistance29 = fabs(fplayer->tl->y - ThreeDObjects[29]->tl->y);
                  float ZDistance29 = fabs(fplayer->tl->z - ThreeDObjects[29]->tl->z);
                  float Distance29 = XDistance29 + YDistance29 + ZDistance29;
                  int Volume29 = 16384 / ((int)Distance29 * Distance29);
                  if (Volume29 > 127)
                     {
                     Volume29 = 127;
                     }
                  sound->setVolume (SOUND_EXPLOSION1, Volume29);
                  sound->play (SOUND_EXPLOSION1, false);
                  }
               }
            display ((char *)"Next message is from LoadVariablesFromNetworkApiPacket()", LOG_MOST);
            display (SystemMessageBufferA, LOG_MOST);
            NewSystemMessageNeedsScrolling = true;
            sprintf (DebugBuf, "LoadVariablesFromNetworkApiPacket() BlueTeam HQ Strength = %f", ThreeDObjects[28]->Durability);
            display (DebugBuf, LOG_MOST);
            sprintf (DebugBuf, "LoadVariablesFromNetworkApiPacket()  RedTeam HQ Strength = %f", ThreeDObjects[29]->Durability);
            display (DebugBuf, LOG_MOST);
            sprintf (DebugBuf, "LoadVariablesFromNetworkApiPacket() BlueTeam RADAR Strength = %f", ThreeDObjects[24]->Durability);
            display (DebugBuf, LOG_MOST);
            sprintf (DebugBuf, "LoadVariablesFromNetworkApiPacket()  RedTeam RADAR Strength = %f", ThreeDObjects[25]->Durability);
            display (DebugBuf, LOG_MOST);
            } 
         else
            { 
            sprintf (DebugBuf, "LoadVariablesFromNetworkApiPacket() InPacket compelled destruction of object %d", InPacket.UdpObjDamageId);
            display (DebugBuf, LOG_MOST);
            ThreeDObjects[InPacket.UdpObjDamageId]->Durability = -4000; 
            if (InPacket.UdpObjDamageId == 22)
               { 
               
               if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.70)  && (BattleshipSunkAlertCountBlue <= 2) )
                  {  
                  ThreeDObjects[28]->Durability = (ThreeDObjects[28]->maxDurability * 0.69);
                  sprintf (SystemMessageBufferA, "MISSION BLUE TEAM BATTLESHIP DESTROYED.");
                  NewSystemMessageNeedsScrolling = true;
                  }
               BattleshipSunkAlertCountBlue++;
               }
            if (InPacket.UdpObjDamageId == 23)
               { 
               
               if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.70)  && (BattleshipSunkAlertCountRed <= 2) )
                  {  
                  ThreeDObjects[29]->Durability = (ThreeDObjects[29]->maxDurability * 0.69);
                  sprintf (SystemMessageBufferA, "MISSION RED TEAM BATTLESHIP DESTROYED.");
                  NewSystemMessageNeedsScrolling = true;
                  }
               BattleshipSunkAlertCountRed++;
               }
            if (InPacket.UdpObjDamageId%2)
               { 
               if (InPacket.UdpObjDamageId == 29)
                  { 
                  sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  if (fplayer->party == 1)
                     { 
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            else
               { 
               if (InPacket.UdpObjDamageId == 28)
                  { 
                  sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  if (fplayer->party == 0)
                     { 
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            } 
         } 
      } 
   if (InPacket.UdpObjDamageAmount < 0.0)
      { // Get here if received damage represents a static update
      
      InPacket.UdpObjDamageAmount *= -1.0; 
      
      if (ThreeDObjects[InPacket.UdpObjDamageId]->Durability > (ThreeDObjects[28]->maxDurability * 0.15))
         { 
         if (InPacket.UdpObjDamageId == 28 && CurrentMissionNumber != MISSION_NETWORKBATTLE02)
            { 
            if (!ThreeDObjects [22]->active)
               { 
               ThreeDObjects[22]->DamageInNetQueue = 299000; 
               display ((char *)"LoadVariablesFromNetworkApiPacket() Informing all that BlueTeam battleship has been destroyed. Object 22 DamageInNetQueue = 299000.", LOG_MOST);
               }
            }
         else if (InPacket.UdpObjDamageId == 29 && CurrentMissionNumber != MISSION_NETWORKBATTLE02)
            { // Get here if this static update indicates RedTeam airfield has strength that cannot be maintained without a battleship
            if (!ThreeDObjects [23]->active)
               { 
               ThreeDObjects[23]->DamageInNetQueue = 299000; 
               display ((char *)"LoadVariablesFromNetworkApiPacket() Informing all that RedTeam battleship has been destroyed. Object 23 DamageInNetQueue = 299000.", LOG_MOST);
               }
            }
         }
      if (InPacket.UdpObjDamageAmount < ThreeDObjects[InPacket.UdpObjDamageId]->Durability)
         { 
         ThreeDObjects[InPacket.UdpObjDamageId]->Durability = InPacket.UdpObjDamageAmount; // Update addressed 3d Object with lower static value.
         }
      }
} 

void ProcessUdpObjFlightDetails()
{
if (InPacket.UdpObjFlightDetails & 4)
   { 
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->SpeedBrake = 1;
   }
else
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->SpeedBrake = 0;
   }
if (InPacket.UdpObjFlightDetails & 3)
   { 
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->FlapsLevel = (InPacket.UdpObjFlightDetails & 3);
   }
else
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->FlapsLevel = 0;
   }
if (InPacket.UdpObjFlightDetails & 8)
   { 
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A FLARE.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   }
if (InPacket.UdpObjFlightDetails & 16)
   { 
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A CHAFF PACKET.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   }
if (InPacket.UdpObjFlightDetails & 32)
   { 
   float XDistance = fabs(InPacket.UdpObjXPosition - fplayer->tl->x);
   float YDistance = fabs (InPacket.UdpObjYPosition - fplayer->tl->y);
   float ZDistance = fabs (InPacket.UdpObjZPosition - fplayer->tl->z);
   float TotalDistance = XDistance + YDistance + ZDistance;
   
   int volume;
   if (TotalDistance > 200)
      { 
      volume = 0;
      }
   else
      { 
      volume = (int) 127 - (int)(TotalDistance/1.8);
      }
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A MISSILE.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (MyNetworkId%2 != InPacket.UdpObjPlayerNumber%2)
      { 
      
      sound->setVolume (SOUND_FIVEBEEPS00, volume);
      sound->play (SOUND_FIVEBEEPS00, false);
      }
   }
} 

void RepairDamagedAirfields()
{
static bool BlueTeamAircraftThreatensAirfield = false;
static bool RedTeamAircraftThreatensAirfield = false;
static bool Siren28 = false;
static bool Siren29 = false;
static float PriorDurability28 = 0.0; 
static float PriorDurability29 = 0.0; 

float XDistance28 = fabs(fplayer->tl->x - ThreeDObjects[28]->tl->x);
float YDistance28 = fabs(fplayer->tl->y - ThreeDObjects[28]->tl->y);
float ZDistance28 = fabs(fplayer->tl->z - ThreeDObjects[28]->tl->z);
float Distance28 = XDistance28 + YDistance28 + ZDistance28;
if (Siren28)
   { 
   
   int Volume28 = 16384 / ((int)Distance28 * Distance28);
   if (Volume28 > 127)
      {
      Volume28 = 127;
      }
   sound->setVolume (SOUND_AIRRAIDSIREN, Volume28);
   
   if (!RedTeamAircraftThreatensAirfield)
      { 
      sound->stop (SOUND_AIRRAIDSIREN); 
      Siren28 = false;
      }
   }

float XDistance29 = fabs(fplayer->tl->x - ThreeDObjects[29]->tl->x);
float YDistance29 = fabs(fplayer->tl->y - ThreeDObjects[20]->tl->y);
float ZDistance29 = fabs(fplayer->tl->z - ThreeDObjects[29]->tl->z);
float Distance29 = XDistance29 + YDistance29 + ZDistance29;
if (Siren29)
   { 
   
   int Volume29 = 16384 / ((int)Distance29 * Distance29);
   if (Volume29 > 127)
      {
      Volume29 = 127;
      }
   sound->setVolume (SOUND_AIRRAIDSIREN, Volume29);
   
   if (!BlueTeamAircraftThreatensAirfield)
      { 
      sound->stop (SOUND_AIRRAIDSIREN); 
      Siren29 = false;
      }
   }
float DurabilityChange28 = PriorDurability28 - ThreeDObjects[28]->Durability;
float DurabilityChange29 = PriorDurability29 - ThreeDObjects[29]->Durability;
if (RedTeamAircraftThreatensAirfield && fabs(DurabilityChange28) > 10)
   { 
   sprintf (DebugBuf, "DebugRjb180530 Airfield 28 sustained %f units of new damage.", DurabilityChange28);
   display (DebugBuf, LOG_MOST);
   if (!Siren28)
      {
      sound->play (SOUND_AIRRAIDSIREN, true);
      Siren28 = true;
      }
   if (DurabilityChange28 > 7000)
      { 
      float XDistance28 = fabs(fplayer->tl->x - ThreeDObjects[28]->tl->x);
      float YDistance28 = fabs(fplayer->tl->y - ThreeDObjects[28]->tl->y);
      float ZDistance28 = fabs(fplayer->tl->z - ThreeDObjects[28]->tl->z);
      float Distance28 = XDistance28 + YDistance28 + ZDistance28;
      int Volume28 = 16384 / ((int)Distance28 * Distance28);
      if (Volume28 > 127)
         {
         Volume28 = 127;
         }
      sound->setVolume (SOUND_EXPLOSION1, Volume28);
      sound->play (SOUND_EXPLOSION1, false);
      }
   }
if (BlueTeamAircraftThreatensAirfield && fabs(DurabilityChange29) > 10)
   { 
   sprintf (DebugBuf, "DebugRjb180530 Airfield 29 sustained %f units of new damage.", DurabilityChange29);
   display (DebugBuf, LOG_MOST);
   if (!Siren29)
      {
      sound->play (SOUND_AIRRAIDSIREN, true);
      Siren29 = true;
      }
   if (DurabilityChange29 > 7000)
      { 
      float XDistance29 = fabs(fplayer->tl->x - ThreeDObjects[29]->tl->x);
      float YDistance29 = fabs(fplayer->tl->y - ThreeDObjects[29]->tl->y);
      float ZDistance29 = fabs(fplayer->tl->z - ThreeDObjects[29]->tl->z);
      float Distance29 = XDistance29 + YDistance29 + ZDistance29;
      int Volume29 = 16384 / ((int)Distance29 * Distance29);
      if (Volume29 > 127)
         {
         Volume29 = 127;
         }
      sound->setVolume (SOUND_EXPLOSION1, Volume29);
      sound->play (SOUND_EXPLOSION1, false);
      }
   }

if (!ThreeDObjects [22]->active)
   { 
   if (ThreeDObjects[28]->Durability > (.70 * ThreeDObjects[28]->maxDurability))
      { 
      if ((MissionNumber == MISSION_NETWORKBATTLE01) || (MissionNumber == MISSION_NETWORKBATTLE03))
         { 
         ThreeDObjects[28]->Durability = (.69 * ThreeDObjects[28]->maxDurability); // Cannot exceed 69% without its battleship.
         }
      }
   }
else
   { 
   if (ThreeDObjects[28]->Durability < (.15 * ThreeDObjects[28]->maxDurability))
      { 
      ThreeDObjects[28]->Durability = (.15 * ThreeDObjects[28]->maxDurability); // Always at least 5% while battleship is alive
      }
   }
if (!ThreeDObjects [23]->active)
   { 
   if (ThreeDObjects[29]->Durability > (.70 * ThreeDObjects[29]->maxDurability))
      { 
      if ((MissionNumber == MISSION_NETWORKBATTLE01) || (MissionNumber == MISSION_NETWORKBATTLE03))
         { 
         ThreeDObjects[29]->Durability = (.69 * ThreeDObjects[29]->maxDurability); // Cannot exceed 69% without its battleship.
         }
      }
   }
else
   { 
   if (ThreeDObjects[29]->Durability < (.15 * ThreeDObjects[28]->maxDurability))
      { 
      ThreeDObjects[29]->Durability = (.15 * ThreeDObjects[29]->maxDurability); // Always at least 5% while battleship is alive
      }
   }

int i;
// Make incremental repairs to any airfield damage. (Airfields are object numbers 28 and 29.)
for (i=28; i<=29; i++)
   {
   if (ThreeDObjects[i]->Durability < ThreeDObjects[i]->maxDurability)
      { 
      
      static float MissionAircraftDistanceX [11];
      static float MissionAircraftDistanceY [11];
      static float MissionAircraftDistanceZ [11];
      static float MissionAircraftDistanceTotal [11];
      
      static float NearestAircraftDistance [2];
      NearestAircraftDistance[0] = 10000.0; 
      NearestAircraftDistance[1] = 10000.0; 
      
      static unsigned int NearestAircraftNumber[2] = {10};
      
      int j;
      for (j=0; j<=10; j++)
         { 
         MissionAircraftDistanceX [j] = fabs(ThreeDObjects[j]->tl->x - ThreeDObjects[i]->tl->x);
         MissionAircraftDistanceY [j] = fabs(ThreeDObjects[j]->tl->y - ThreeDObjects[i]->tl->y);
         MissionAircraftDistanceZ [j] = fabs(ThreeDObjects[j]->tl->z - ThreeDObjects[i]->tl->z);
         MissionAircraftDistanceTotal[j] = (MissionAircraftDistanceX[j] + MissionAircraftDistanceY[j] + MissionAircraftDistanceZ[j]);
         
         fplayer->Sentient = 20;
         ThreeDObjects[0]->Sentient = 20;
         
         if (ThreeDObjects[j]->Sentient < 3)
            { 
            MissionAircraftDistanceTotal[j] = 25000.00; 
            }
         
         if (MissionAircraftDistanceTotal[j] < NearestAircraftDistance[29-i])
            { 
            NearestAircraftDistance[29-i] = MissionAircraftDistanceTotal[j];
            NearestAircraftNumber[29-i] = j; 
            }
         }
      
      if (
         MissionAircraftDistanceTotal[1] < 500 ||
         MissionAircraftDistanceTotal[3] < 500 ||
         MissionAircraftDistanceTotal[5] < 500 ||
         MissionAircraftDistanceTotal[7] < 500 ||
         MissionAircraftDistanceTotal[9] < 500 ||
         (MissionAircraftDistanceTotal[0] < 500 && MyNetworkId%2)
         )
            { 
            RedTeamAircraftThreatensAirfield = true;
            }
      else
            {
            RedTeamAircraftThreatensAirfield = false;
            }
      if (
         MissionAircraftDistanceTotal[2] < 500 ||
         MissionAircraftDistanceTotal[4] < 500 ||
         MissionAircraftDistanceTotal[6] < 500 ||
         MissionAircraftDistanceTotal[8] < 500 ||
         MissionAircraftDistanceTotal[10] < 500 ||
         (MissionAircraftDistanceTotal[0] < 500 && !MyNetworkId%2)
         )
            { 
            BlueTeamAircraftThreatensAirfield = true;
            }
      else
            {
            BlueTeamAircraftThreatensAirfield = false;
            }
      
      if (NearestAircraftNumber[29-i] == 0)
         { 
         NearestAircraftNumber[29-i] = (unsigned int)MyNetworkId;
         }
      
      if (i == 28 || i == 29)
         { 
         ThreeDObjects[i]->Durability += AIRFIELDREPAIRVALUE; 
         }

      if (ThreeDObjects[25]->Durability <= (ThreeDObjects[25]->maxDurability * 0.4))
         { 
         if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.4))
            { 
            ThreeDObjects[29]->Durability = ThreeDObjects[29]->maxDurability * 0.39; 
            
            ThreeDObjects[25]->Durability = (ThreeDObjects[25]->maxDurability * 0.5);
            }
         }
      if (ThreeDObjects[24]->Durability <= (ThreeDObjects[24]->maxDurability * 0.4))
         { 
         if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.4))
            { 
            ThreeDObjects[28]->Durability = ThreeDObjects[28]->maxDurability * 0.39; 
            
            ThreeDObjects[24]->Durability = (ThreeDObjects[24]->maxDurability * 0.5);
            }
         }
      unsigned char NearestRelevantAircraft =  NearestAircraftNumber[29-i];
      if (i%2)
         { 
         if (ThreeDObjects[29]->Durability < (ThreeDObjects[29]->maxDurability * 0.4 ))
            { 
            RadarReflectorRedHasBeenDestroyedAtLeastOnce = true;
            }
         if (NearestAircraftDistance[29-i] < 250)
            { 
            if (ThreeDObjects[29-i]->realspeed > ThreeDObjects[29-i]->StallSpeed)
               { 
               if (NearestAircraftNumber[29-i]%2)
                  { 
                  if ((ThreeDObjects[NearestRelevantAircraft]->Sentient > 4) || NearestRelevantAircraft == MyNetworkId)
                     { 
                     sprintf (SystemMessageBufferA, "REDTEAM HQ AIRFIELD REPAIRS ACCELERATED TO 5X.");
                     NewSystemMessageNeedsScrolling = true;
                     ThreeDObjects[i]->Durability += (4*AIRFIELDREPAIRVALUE);
                     AirfieldRepairsAcceleratedForRedTeam = true;
                     AirfieldRepairsStoppedForRedTeam = false;
                     AirfieldRepairRateNormalForRedTeam = false;
                     }
                  }
               else
                  { 
                  if ((ThreeDObjects[NearestRelevantAircraft]->Sentient > 4) || NearestRelevantAircraft == MyNetworkId)
                     { 
                     sprintf (SystemMessageBufferA, "REDTEAM HQ AIRFIELD REPAIRS STOPPED.");
                     NewSystemMessageNeedsScrolling = true;
                     ThreeDObjects[i]->Durability -= AIRFIELDREPAIRVALUE;
                     AirfieldRepairsAcceleratedForRedTeam = false;
                     AirfieldRepairsStoppedForRedTeam = true;
                     AirfieldRepairRateNormalForRedTeam = false;
                     }
                  }
               }
            } 
         else
            { 
            AirfieldRepairsAcceleratedForRedTeam = false;
            AirfieldRepairsStoppedForRedTeam = false;
            AirfieldRepairRateNormalForRedTeam = true;
            }
         }
      else
         { 
         if (ThreeDObjects[28]->Durability < (ThreeDObjects[28]->maxDurability * 0.4 ))
            { 
            RadarReflectorBlueHasBeenDestroyedAtLeastOnce = true;
            }
         if (NearestAircraftDistance[29-i] < 250)
            { 
            if (ThreeDObjects[29-i]->realspeed > ThreeDObjects[29-i]->StallSpeed)
               { 
               if (NearestAircraftNumber[29-i]%2)
                  { 
                  if ((ThreeDObjects[NearestRelevantAircraft]->Sentient > 4) || NearestRelevantAircraft == MyNetworkId)
                     { 
                     sprintf (SystemMessageBufferA, "BLUETEAM HQ AIRFIELD REPAIRS STOPPED.");
                     NewSystemMessageNeedsScrolling = true;
                     ThreeDObjects[i]->Durability -= AIRFIELDREPAIRVALUE;
                     AirfieldRepairsAcceleratedForBlueTeam = false;
                     AirfieldRepairsStoppedForBlueTeam = true;
                     AirfieldRepairRateNormalForBlueTeam = false;
                     }
                  }
               else
                  { 
                  if ((ThreeDObjects[NearestRelevantAircraft]->Sentient > 4) || NearestRelevantAircraft == MyNetworkId)
                     { 
                     sprintf (SystemMessageBufferA, "BLUETEAM HQ AIRFIELD REPAIRS ACCELERATED TO 5X.");
                     NewSystemMessageNeedsScrolling = true;
                     ThreeDObjects[i]->Durability += (4*AIRFIELDREPAIRVALUE);
                     AirfieldRepairsAcceleratedForBlueTeam = true;
                     AirfieldRepairsStoppedForBlueTeam = false;
                     AirfieldRepairRateNormalForBlueTeam = true;
                     }
                  }
               }
            } 
         else
            { 
            AirfieldRepairsAcceleratedForBlueTeam = false;
            AirfieldRepairsStoppedForBlueTeam = false;
            AirfieldRepairRateNormalForBlueTeam = true;
            }
         }
      } 
   }
PriorDurability28 = ThreeDObjects[28]->Durability;
PriorDurability29 = ThreeDObjects[29]->Durability;
} 

void Mission::draw ()
    {
    }

void Mission::init ()
    {
    }

Mission::Mission ()
    {
    int i;
    timer = 0;

    for (i = 0; i < 3; i ++)
        {
        selfighter [i] = 0;
        }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_HAWK;
    selfighter [2] = FIGHTER_SPIT9;
    selfighters = 2;
    wantfighter = 0;
    selweapons = 3;
    selweapon [0] = BOMB01;
    selweapon [1] = MISSILE_DF1;
    selweapon [2] = MISSILE_AIR2;
    wantweapon = 0;
    textcolor.setColor (255, 255, 0, 180);
    clouds = 0;
    heading = 180;
    state = 0;
    LandedAtSafeSpeed = false;
    }

void Mission::playerInit ()
    {
    int i;
    fplayer = ThreeDObjects [0];
    fplayer->target = NULL;
    fplayer->newinit (selfighter [wantfighter], 1, 0);
    fplayer->ai = false;
    for (i = 0; i < missiletypes; i ++)
        {
        fplayer->missiles [i] = 0;
        }
    for (i = 0; i < missileracks; i ++)
        {
        fplayer->missilerack [i] = -1;
        }
    fplayer->missileCount ();
    } 

int Mission::processtimer (Uint32 dt)
    {
    display ((char *)"Function Entry: Mission::processtimer()", LOG_MOST);
    return 0;
    }

void Mission::start ()
    {
    RadarReflectorBlueHasBeenDestroyedAtLeastOnce = false;
    RadarReflectorRedHasBeenDestroyedAtLeastOnce = false;
    AutoPilotActive = false;
    }

MissionDemo1::MissionDemo1 ()
    {
    id = MISSION_DEMO;
    strncpy (name, "DEMO", 1024);
    }

void MissionDemo1::start ()
    {
    day = 1;
    clouds = 0;
    weather = WEATHER_SUNNY;
    sungamma = 0;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_SEA, NULL);
    fplayer = ThreeDObjects [0];
    ThreeDObjects [0]->tl->x = 1620;
    ThreeDObjects [0]->tl->z = 300;
    ThreeDObjects [0]->newinit (FIGHTER_GENERIC01, 1, 0);
    } 

int MissionDemo1::processtimer (Uint32 dt)
    {
    timer += dt;
    ThreeDObjects [0]->tl->x = 1620;
    ThreeDObjects [0]->tl->z = 300;
    ThreeDObjects [0]->tl->y = 150; 
    fplayer->thrust = 0;
    fplayer->realspeed = 0;
    camera = 3;
    return (id);
    }

void MissionDemo1::draw ()
    {
    sunlight = 1;
    }

MissionTutorial1::MissionTutorial1 ()
    {
    id = MISSION_TUTORIAL;
    strncpy (name, "TUTORIAL1: FLIGHT BASICS", 1024);
    strncpy (briefing, "LEARN TO HANDLE YOUR AIRCRAFT AND THE BASIC CONTROLS.", 1024);
    font1->drawTextScaled(0, 0, -4, briefing, 10);
    heading = 210;
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTutorial1::start ()
    {
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=1; 
    RadarZoom=1; 
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -13.0; 
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    playerInit ();
    fplayer->tl->x = 220;
    fplayer->tl->z = -30;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    int i;
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionTutorial1::processtimer (Uint32 dt)
    {
    timer += dt;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2; 
        sound->stop (SOUND_PLANE1);
        sound->stop (SOUND_PLANE2);
        sound->stop (SOUND_WINDNOISE);
        WindNoiseOn = false;
        }
    if (!ThreeDObjects [1]->active && !ThreeDObjects [2]->active && state == 1)
        {
        return 1;
        }
    return 0;
    }

void MissionTutorial1::draw ()
    {
    char buf [250], buf2 [10];
    int timeroff = 100 * timestep;
    int timerdelay = 300 * timestep;
    int timerlag = 20 * timestep;
    static bool BeepLatch = false;

    if (timer >= 0 && timer <= 30 * timestep)
        {
        font1->drawTextCentered (0, 6, -1, name, &textcolor);
        }
    else if (timer > 50 * timestep && timer <= 180 * timestep)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            if (!BeepLatch)
                {
                BeepLatch = true;
                sound->play (SOUND_BEEP1, false);
                }
            font1->drawTextCentered (0, 11, -2.25, (char *)"JIGGLE YOUR THROTTLE AT THE START OF", &textcolor);
            font1->drawTextCentered (0, 10, -2.25, (char *)"EVERY MISSION TO START FUEL FLOW.", &textcolor);
            }
        }
    else if (timer > 190 * timestep && timer <= 400 * timestep)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0,  9, -2.25, (char *)"LEAVE THE JOYSTICK CENTERED.", &textcolor);
            font1->drawTextCentered (0,  8, -2.25, (char *)"THE FIGHTER SHOULD FLY STRAIGHT ON.", &textcolor);
            font1->drawTextCentered (0,  7, -2.25, (char *)"IF NOT, RECALIBRATE YOUR JOYSTICK", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"THE MENU OPTIONS/CONTROLS LETS YOU", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"DETERMINE THE INPUT DEVICE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"CURRENTLY, THIS IS THE MOUSE", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"THE MENU OPTIONS/CONTROLS LETS YOU", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"DETERMINE THE INPUT DEVICE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"CURRENTLY, THIS IS THE KEYBOARD", &textcolor);
            }
        }
    else if (timer > timeroff + timerdelay && timer <= timeroff + 2 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"MOVING THE JOYSTICK LEFT OR RIGHT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"WILL AFFECT THE AILERON.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THUS YOU WILL ONLY ROLL", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            if (!mouse_reverse && !mouse_relative)
                {
                font1->drawTextCentered (0, 9, -2.25, (char *)"THE MOUSE INTERFACE IS VERY EASY.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, (char *)"ALWAYS POINT TO WHERE YOU WANT TO FLY.", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, (char *)"YOU'LL HAVE TO MOVE THE MOUSE PERMANENTLY", &textcolor);
                }
            else if (mouse_reverse && !mouse_relative)
                {
                font1->drawTextCentered (0, 9, -2.25, (char *)"THE REVERSE MOUSE IS FOR EXPERIENCED GAMERS.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, (char *)"THE STANDARD MOUSE INTERFACE MAY BE", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, (char *)"EASIER TO LEARN", &textcolor);
                }
            else
                {
                font1->drawTextCentered (0, 9, -2.25, (char *)"MOUSE RELATIVE IS A LOT OF WORK.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, (char *)"ONLY THE RELATIVE MOUSE COORDINATES (MOVING)", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, (char *)"WILL HAVE AN EFFECT", &textcolor);
                }
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"DON'T TRY TO FLY WITH JUST THE KEYBOARD. USING A", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"JOYSTICK IS STRONLY RECOMMENDED, BUT IF NECESSARY", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"YOU CAN GET BY WITH MOUSE AND KEYBOARD.", &textcolor);
            }
        }
    else if (timer > timeroff + 2 * timerdelay && timer <= timeroff + 3 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"MOVING UP OR DOWN WILL AFFECT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"THE ELEVATOR.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"YOU'LL START TO FLY A LOOP", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE || controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"MOVING RIGHT OR LEFT WILL AFFECT THE AILERON,", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"MOVING UP OR DOWN WILL AFFECT THE ELEVATOR.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THUS YOU CAN FLY ROLLS AND LOOPS", &textcolor);
            }
        }
    else if (timer > timeroff + 3 * timerdelay && timer <= timeroff + 4 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"IF YOUR JOYSTICK HAS A SO-CALLED \"RUDDER\",", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"YOU MAY ALTER THE FIGHTER'S RUDDER.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"YOU'LL SLIGHTLY FLY TO THE LEFT OR RIGHT", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"MOVING THE MOUSE SLIGHTLY LEFT OR RIGHT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"WILL AFFECT THE RUDDER.", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"TO ALTER THE RUDDER", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"USE THE KEYS PGUP AND PGDN", &textcolor);
            }
        }
    else if (timer > timeroff + 4 * timerdelay && timer <= timeroff + 5 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"YOUR JOYSTICK HAS ALSO A THROTTLE", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"TO SPEEDUP OR DECELERATE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THERE IS NO WAY TO ENTIRELY STOP", &textcolor);
            }
        else
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"USE THE KEYS '1' THROUGH '9' TO ALTER", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"YOUR THROTTLE (SPEED).", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THERE IS NO WAY TO ENTIRELY STOP", &textcolor);
            }
        }
    else if (timer > timeroff + 5 * timerdelay && timer <= timeroff + 7 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"THE BUTTONS ON YOUR JOYSTICK CAN CONTROL ALL", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"OF YOUR WEAPONS AND FLIGHT SURFACES. YOU CAN MAKE", &textcolor);
            font1->drawTextCentered (0, 7,  -2.25, (char *)"ANY BUTTON ACTIVATE ANY CONTROL BY EDITING THE", &textcolor);
            font1->drawTextCentered (0, 6,  -2.25, (char *)"LACCONTROLS.TXT FILE IN THE .LAC FOLDER WITHIN", &textcolor);
            font1->drawTextCentered (0, 5,  -2.25, (char *)"YOUR HOME FOLDER.", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"LEFT MOUSE BUTTON: FIRE CANNON", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"RIGHT MOUSE BUTTON: FIRE MISSILE", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"BUT YOU SHOULD REALLY PREFER THE KEYBOARD", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"NOW, LETS HAVE A LOOK AT SOME IMPORTANT KEYS", &textcolor);
            }
        }
    else if (timer > timeroff + 6 * timerdelay && timer <= timeroff + 8 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"A SUMMARY OF COMMANDS CAN BE FOUND IN THE MENU.", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, (char *)"\"ESC\" WILL SHOW THE MENU", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"AND RETURN BACK TO THE ACTION", &textcolor);
            }
        else
            {
            key2string (key_PRIMARY, buf2);
            sprintf (buf, "%s: FIRE CANNON", buf2);
            font1->drawTextCentered (0, 6, -2.25, buf, &textcolor);
            key2string (key_WEAPONSELECT, buf2);
            sprintf (buf, "%s: CHOOSE MISSILE", buf2);
            font1->drawTextCentered (0, 5, -2.25, buf, &textcolor);
            key2string (key_SECONDARY, buf2);
            sprintf (buf, "%s: FIRE MISSILE", buf2);
            font1->drawTextCentered (0, 4, -2.25, buf, &textcolor);
            }
        }
    else if (timer > timeroff + 7 * timerdelay && timer <= timeroff + 9 * timerdelay - timerlag)
        {
        RadarOnOff=1; 
        RadarZoom=1; 
        IffOnOff=1;
        RadarZoom=3;
        if (!RadarWasOn)
            {
            sound->setVolume (SOUND_BEEP2, 20); 
            sound->play (SOUND_BEEP2, false);
            RadarWasOn=1;
            }
        font1->drawTextCentered (0, 9, -2.25, (char *)"LOOK AT THE RADAR ON THE BOTTOM OF YOUR SCREEN.", &textcolor);
        font1->drawTextCentered (0, 8, -2.25, (char *)"THERE ARE ENEMIES REPRESENTED BY A WHITE DOT (TARGETED)", &textcolor);
        font1->drawTextCentered (0, 7, -2.25, (char *)"AND A YELLOW POINT (NOT TARGETED)", &textcolor);
        }
    else if (timer > timeroff + 8 * timerdelay && timer <= timeroff + 10 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"TARGET AN ADVERSARY WITH A CONFIGURED JOYSTICK BUTTON,", &textcolor);
            key2string (key_TARGETNEXT, buf2);
            sprintf (buf, "HATSWITCH, OR BY PRESSING '%s' ON THE KEYBOARD.", buf2);
            font1->drawTextCentered (0, 8, -2.25, buf, &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THEN APPROACH!", &textcolor);
            }
        else
            {
            font1->drawTextCentered (0, 9, -2.25, (char *)"YOU MAY TARGET AN ENEMY PRESSING", &textcolor);
            key2string (key_TARGETNEXT, buf2);
            sprintf (buf, "OR PRESSING '%s' ON THE KEYBOARD.", buf2);
            font1->drawTextCentered (0, 8, -2.25, buf, &textcolor);
            font1->drawTextCentered (0, 7, -2.25, (char *)"THEN APPROACH!", &textcolor);
            }
        }
    else if (timer > timeroff + 9 * timerdelay && timer <= timeroff + 11 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 9, -2.25, (char *)"SHOOT THE TARGETS TO COMPLETE", &textcolor);
        font1->drawTextCentered (0, 8, -2.25, (char *)"THIS TUTORIAL SUCCESSFULLY", &textcolor);
        }
    if (timer >= timeroff + 7 * timerdelay && state == 0)
        {
        state ++;
        ThreeDObjects [1]->activate ();
        ThreeDObjects [1]->target = ThreeDObjects [0];
        ThreeDObjects [1]->o = &model_figu;
        ThreeDObjects [1]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [1]->tl->x = fplayer->tl->x - 30;
        ThreeDObjects [1]->tl->z = fplayer->tl->z - 30;
        ThreeDObjects [2]->activate ();
        ThreeDObjects [2]->target = ThreeDObjects [0];
        ThreeDObjects [2]->o = &model_figu;
        ThreeDObjects [2]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [2]->tl->x = fplayer->tl->x + 30;
        ThreeDObjects [2]->tl->z = fplayer->tl->z + 30;
        }

    if (timer > timeroff + 10 * timerdelay)
        {
        if (!ThreeDObjects [1]->active && !ThreeDObjects [2]->active)
            {
            font1->drawTextCentered (0, 11, -2.25, (char *)"MISSION COMPLETED SUCCESSFULLY.", &textcolor);
            font1->drawTextCentered (0,  9, -2.25, (char *)"PRESS <ESC> TO RETURN TO MAIN MENU.", &textcolor);

            }
        }
    } 

MissionTutorial2::MissionTutorial2 ()
    {
    id = MISSION_TUTORIAL2;
    strncpy (name, "TUTORIAL2: AIR-TO-GROUND", 1024);
    strncpy (briefing, "PRACTICE USING AIR-TO-GROUND WEAPONS VS ARMORED TARGETS.", 1024);
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_GROUND1;
    wantweapon = 0;
    }

void MissionTutorial2::start ()
    {
    NoMissionHasYetCommenced = false;
    int i;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=0;        
    event_RadarOnOff();  
    IffOnOff=0;          
    RadarZoom=8;         
    event_IffOnOff();    
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_FLATLAND, NULL);
    SeaLevel = -134.0; 
    int px, py;
    
    l->searchPlain (-1, -1, &px, &py);
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5); 
    playerInit ();
    
    fplayer->tl->x = px;
    fplayer->tl->z = py + 150;
    fplayer->phi = 250;
    ConfigureOrdnanceForOnlineMissions();
    
    for (i = 1; i <= 2; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->o = &model_tank1;
        ThreeDObjects [i]->tl->x = px + 300 - i * 4;
        ThreeDObjects [i]->tl->z = py + 300 - i * 4;
        ThreeDObjects [i]->newinit (TANK_GROUND1, 0, 400);
        ThreeDObjects [i]->maxthrust = 0;
        ThreeDObjects [i]->activate();
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    } 

int MissionTutorial2::processtimer (Uint32 dt)
    {
    bool b = false;
    int i;
    timer += dt;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        { 
        return 2;
        }
    for (i = 1; i <= 2; i ++)
        { 
        if (ThreeDObjects [i]->active)
           { 
           ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 0.1; 
           if (ThreeDObjects [i]->party == 0)
                { 
                b = true; 
                }
            }
        }
    if (b)
        { 
        return 0;
        }
    
    return 1;
    } 

void MissionTutorial2::draw ()
    {
    char buf [250], buf2 [10];
    int timeroff = 100 * timestep, timerdelay = 300 * timestep, timerlag = 20 * timestep;

    if (timer >= 0 && timer <= timeroff - 20)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    else if (timer > timeroff && timer <= timeroff + timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, (char *)"FIRSTLY, HAVE A LOOK AT YOUR WEAPONS:", &textcolor);
        key2string (key_WEAPONSELECT, buf2);
        sprintf (buf, "PRESS '%s' TO VIEW CHOICES.", buf2);
        font1->drawTextCentered (0, 6, -2.5, buf, &textcolor);
        }
    else if (timer > timerdelay + timeroff && timer <= timeroff + 2 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, (char *)"USE ROCKETS IF YOUR AIRCRAFT HAS THEM.", &textcolor);
        }
    else if (timer > 2 * timerdelay + timeroff && timer <= timeroff + 3 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, (char *)"BUT BE CAREFUL: ROCKETS", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, (char *)"ARE QUITE 'DUMB' AND WILL FLY STRAIGHT AHEAD.", &textcolor);
        }
    else if (timer > 3 * timerdelay + timeroff && timer <= timeroff + 4 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 6, -2.5, (char *)"HOWEVER, ROCKETS CAUSE A LOT OF DAMAGE", &textcolor);
        }
    else if (timer > 4 * timerdelay + timeroff && timer <= timeroff + 5 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 6, -2.5, (char *)"NOW, BLOW UP THE TWO TANKS", &textcolor);
        }
    } 

MissionTutorial3::MissionTutorial3 ()
    {
    id = MISSION_TUTORIAL3;
    strncpy (name, "TUTORIAL3: FIGHTERS", 1024);
    strncpy (briefing, "HIGH SPEED DOGFIGHT PRACTICE. FIGHT BOTS WITH YOUR GUNS.\nNOTE THAT BOTS ARE NOT AS AGRESSIVE OR AS SMART AS THE\nHUMAN PLAYERS YOU WILL FIND IN THE ONLINE MISSIONS.", 1024);
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    } 

void MissionTutorial3::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    RadarOnOff=0; 
    RadarZoom=1; 
    event_RadarOnOff(); 
    event_RadarZoomIn(); 
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_IffOnOff(); 
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE, NULL);
    SeaLevel = 15.15; 
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5); 
    playerInit ();
    fplayer->tl->x = 20;
    fplayer->tl->z = 70;
    fplayer->phi = 60;
    for (i = 1; i <= 6; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->o = &model_figz;
        ThreeDObjects [i]->tl->x = -i * 120;
        ThreeDObjects [i]->tl->z = -i * 12;
        ThreeDObjects [i]->newinit(FIGHTER_FW190,0,400,100,800); // id, party, stupidity, precision, passivity
        ThreeDObjects [i]->deactivate ();
        }
    ThreeDObjects [1]->activate ();
    state = 0;
    laststate = 0;
    texttimer = 0;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0; 
        fplayer->missiles [1] = 4; 
        fplayer->missiles [2] = 2; 
        fplayer->missiles [3] = 0; 
        fplayer->missiles [4] = 0; 
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0; 
        fplayer->missiles [1] = 0; 
        fplayer->missiles [2] = 0; 
        fplayer->missiles [3] = 0; 
        fplayer->missiles [4] = 0; 
        }
    } 

int MissionTutorial3::processtimer (Uint32 dt)
    {
    bool b = false;
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        }
    for (i = 0; i <= 6; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                {
                b = true;
                }
        }
    if (b)
        { 
        return 0;
        }
    state ++;
    if (state == 1)
        {
        for (i = 2; i <= 3; i ++)
            {
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
            }
        return 0;
        }
    else if (state == 2)
        {
        for (i = 4; i <= 6; i ++)
            {
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
            }
        return 0;
        }
    return 1;
    } 

void MissionTutorial3::draw ()
    {
    char buf [250], buf2 [10];
    int timeroff = 100 * timestep, timerdelay = 300 * timestep, timerlag = 20 * timestep;

    if (laststate != state)
        {
        texttimer = 1;
        laststate = state;
        }
    if (texttimer > 0)
        {
        if (state == 1)
            {
            font1->drawTextCentered (0, 7, -2.5, (char *)"THAT WAS EASY", &textcolor);
            font1->drawTextCentered (0, 6, -2.5, (char *)"BUT CAN YOU HANDLE TWO ADVERSARIES?", &textcolor);
            return;
            }
        if (state == 2)
            {
            font1->drawTextCentered (0, 6, -2.5, (char *)"OK, LET'S TRY THREE OPPONENTS", &textcolor);
            return;
            }
        }
    if (state == 3)
        {
        font1->drawTextCentered (0, 12, -2.5, (char *)"MISSION COMPLETE.", &textcolor);
        font1->drawTextCentered (0, 10, -2.5, (char *)"FOR A MORE SOPHISTICATED MISSION, WE RECOMMEND MISSIONNETWORKBATTLE03.", &textcolor);
        }
    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    if (timer >= timeroff + 6 * timerdelay && timer <= timeroff + 8 * timerdelay - timerlag && state > 0)
        {
        font1->drawTextCentered (0, 15, -2.5, (char *)"SOME TARGETING HINTS:", &textcolor);
        key2string (key_TARGETNEXT, buf2);
        sprintf (buf, "'%s' WILL TARGET THE NEXT RADAR BLIP,", buf2);
        font1->drawTextCentered (0, 13, -2.5, buf, &textcolor);
        key2string (key_TARGETPREVIOUS, buf2);
        sprintf (buf, "'%s' WILL TARGET THE PREVIOUS RADAR BLIP", buf2);
        font1->drawTextCentered (0, 12, -2.5, buf, &textcolor);
        key2string (key_RadarZoomIn, buf2);
        sprintf (buf, "'%s' WILL ZOOM RADAR RANGE IN,", buf2);
        font1->drawTextCentered (0, 10, -2.5, buf, &textcolor);
        key2string (key_RadarZoomOut, buf2);
        sprintf (buf, "'%s' WILL ZOOM RADAR RANGE OUT,", buf2);
        font1->drawTextCentered (0, 9, -2.5, buf, &textcolor);
        }
    if (timer >= timeroff + 1 * timerdelay && state == 0)
        {
        font1->drawTextCentered (0, 16, -2.5, (char *)"NOTE THAT THESE OPPONENTS ARE BOTS. THEY ARE NOT VERY AGRESSIVE AND RARELY SHOOT AT YOU.", &textcolor);
        font1->drawTextCentered (0, 15, -2.5, (char *)"AFTER YOU LEARN THE BASICS WITH THESE TUTORIAL MISSIONS, YOU WILL FIND THAT", &textcolor);
        font1->drawTextCentered (0, 14, -2.5, (char *)"THE ONLINE MISSIONS ARE MUCH MORE FUN, EVEN IF YOU ARE FLYING ALL ALONE.", &textcolor);
        font1->drawTextCentered (0, 13, -2.5, (char *)"(WHENEVER FEWER THAN 10 ONLINE PLAYERS JOIN AN ONLINE MISSION, THE", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, (char *)"REMAINDER ARE REPLACED BY BOTS.)", &textcolor);
        }
    } 

MissionHeadToHead00::MissionHeadToHead00()
    {
    
    id = MISSION_HEADTOHEAD00;
    strncpy (name, "HEAD TO HEAD 00", 1024);
    strncpy (briefing, "SHOOT MINDLESS BOTS OR A SENTIENT NETWORK PEER!\nTHIS MISSION REQUIRES INTERNET ACCESS. EDIT YOUR LACCONFIG.TXT\nFILE TO SPECIFY THE IP ADDRESS OF YOUR SINGLE OPPONENT\nOR LAC SERVER.", 1024);
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_HAWK;
    selfighters = 2;
    selweapons = 1; 
    
    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 3;
    LoadServerIpAddress(); 
    
    NetworkOpponent = 0; 
    if(NetworkMode == 0)
      { 
      if (OpenUdpSocketForReceiving() == 0)
        {
        ConfigureIncomingUdpSocket();
        }
      else
        { 
        }
      if (OpenUdpSocketForSending() == 0)
        {
        ConfigureOutgoingUdpSocket();
        }
      else
        { 
        }
      }
    else if (NetworkMode == 1)
      { 
      if (OpenClientUdpSocket() == 0)
        { 
        ConfigureClientUdpSocket();
        }
      else
        { 
        }
      }
    else
      { 
      }
    } 

void MissionHeadToHead00::start ()
    {
    NoMissionHasYetCommenced = false;
    
    NetworkPlayerKilled = false;
    state = 0;
    laststate = 0;
    texttimer = 0;
    int i;
    int i2;
    day = 1;
    clouds = 2; 
    weather = WEATHER_SUNNY;
    camera = 0;                 
    sungamma = 50;              
    if (l != NULL)              
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915000; 
    
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    playerInit ();
    PlayerAircraftType = fplayer->id;
    fplayer->party = 1;
    
    int r = myrandom (200);        
    r -=100;                       
    float RandomFloat = (float)r;
    fplayer->tl->x = AirfieldXMin;
    fplayer->tl->x += RandomFloat;
    fplayer->tl->z = AirfieldYMin;
    fplayer->tl->z += RandomFloat;
    MissionRunning = false; 
    fplayer->phi = 270;
    fplayer->maxthrust *=1.10;  
    
    HudOnOff = 1;               
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    IffOnOff=0;                 
    MapViewOnOff = 0;           
    RadarOnOff=0;               
    RadarZoom = 1;              
    ScreenFOVx = 1.0;           
    ScreenFOVy = 1.0;           
    
    event_IffOnOff();           
    event_RadarOnOff();         
    event_MapViewOnOff();       
    event_MapZoomIn();          
    event_MapZoomIn();
    event_MapViewOnOff();       
    MaxPlayersInCurrentMission = 2;
    
    for (i = 1; i <= 9; i ++)                          
        {
        ThreeDObjects [i]->target = ThreeDObjects [0];             
        ThreeDObjects [i]->tl->x = -i * 10;                  
        ThreeDObjects [i]->tl->z = -i * 10;                  
        ThreeDObjects [i]->o = &model_figv;                  
        ThreeDObjects [i]->party = 1;                        
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 1200); 
        for (i2 = 0; i2 < missiletypes; i2 ++)
           {
           ThreeDObjects[i]->missiles [i2] = 0; 
           }
        NetworkApiPriorXPosition[i] = -15; 
        NetworkApiPriorZPosition[i] = 45; 
        NetworkApiPriorYPosition[i] = 5;    
        
        if (i > 1)
            { 
            ThreeDObjects [i]->deactivate ();
            }
        }
    ThreeDObjects[1]->thrustDown(); 
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    NetworkApiPriorTimer[MissionHeadToHead00State] = timer; 
    
    state = 1;
    MissionHeadToHead00State = (unsigned char)state;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    ConfigureOrdnanceForOnlineMissions();
    // Ensure aircraft is ready for combat
    ThrustReadiness = (
                        (float)fplayer->DefensiveLethality +
                        fplayer->maxthrust +
                        fplayer->RollRate +
                        fplayer->manoeverability +
                        fplayer->maxDurability +
                        fplayer->maxgamma +
                        fplayer->statLethality +
                        fplayer->StaticDrag +
                        fplayer->SpeedBrakePower +
                        fplayer->MaxFullPowerAltRatio +
                        fplayer->inertia +
                        fplayer->deadweight +
                        fplayer->CompressibilitySpeed +
                        fplayer->CompressibilitySpeedWithSpeedBrakes +
                        fplayer->StallSpeed +
                        fplayer->ServiceCeilingAltitude
                     );
    if (fplayer->WepCapable)
       {
       ThrustReadiness += 200;
       }
    if (ThrustReadiness < 1000)
       {
       display ((char *)"Error. Selected Aircraft ThrustReadiness is insufficient.", LOG_MOST);
       game_quit(); // Airplane performance parameters didn't load properly.
       }
    } 

int MissionHeadToHead00::processtimer (Uint32 dt)
    {
    
    extern int PriorPeerId;
    bool AtLeastOneActiveAiIsStillAlive = false;
    int i;
    int BytesReceived = 0;
    if (!MissionRunning)
       { 
       
       DegradeFlightModelDueToOrdnanceLoad();
       fplayer->tl->y = l->getHeight(AirfieldXMin+1, AirfieldYMin+2);
       if (fplayer->missiles [0] >= 4)
          { 
          fplayer->tl->y += 160;
          fplayer->tl->x = AirfieldXMin-120;
          }
       else
          { 
          fplayer->tl->y += 21;
          }

       fplayer->UndercarriageLevel = 0;
       fplayer->FlapsLevel = 4;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       }
    
    StateTransitionTimer += dt;
    texttimer += dt;
    timer += dt;
    if (StateTransitionTimer <0)
       {
       DiscardAnyInPacketsInQueue();
       }
    
    PriorPeerId = ThreeDObjects[state]->id;
    
    NetworkReceiveTimer += dt;
    if ((NetworkReceiveTimer > NetworkReceiveTimerInterval) && (StateTransitionTimer >=0))
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          { 
          display ((char *)"MissionHeadtoHead00 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          MissedPacketCount = 0;  
          MissionHeadToHead00LoadVariablesFromNetworkApiPacket(timer);
          if (PriorPeerId != ThreeDObjects[state]->id)
             { 
              
              ThreeDObjects[state]->newinit (ThreeDObjects[state]->id, 0, 395);
             }
          if (!PeerPacketReceivedInThisMissionState)
             { 
             PeerPacketReceivedInThisMissionState=true;
             sound->setVolume (SOUND_NEWBANDITONRADARO1, 99);
             sound->play (SOUND_NEWBANDITONRADARO1, false);
             }
          }
       else
          { 
          MissedPacketCount++;
          if (MissedPacketCount > 75 * (200/NetworkReceiveTimerInterval))
             { 
             MissedPacketCount = 0;
             if (PeerPacketReceivedInThisMissionState)
                { 
                PeerPacketReceivedInThisMissionState = false;
                sound->setVolume (SOUND_RADARBANDITDISAPPEARED01, 99);
                sound->play (SOUND_RADARBANDITDISAPPEARED01, false);
                }
             }
          }
       }
    
    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       { 
       NetworkTransmitTimer=0;
       MissionHeadToHead00RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    
    if (!fplayer->active && fplayer->explode >= 2 * timestep)
        { 
        NetworkPlayerKilled = true;
        sound->haltMusic();
        sound->stop (SOUND_PLANE1);
        sound->stop (SOUND_PLANE2);
        sound->stop (SOUND_WINDNOISE);
        WindNoiseOn = false;
        }
    if (!fplayer->active && fplayer->explode >= 100 * timestep)
        { 
        
        display ((char *)"MissionHeadToHead00::processtimer() NetworkPlayerKilled.", LOG_MOST);
        game_quit ();
        }
    
    for (i = 0; i <= 9; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                { 
                AtLeastOneActiveAiIsStillAlive = true;
                }
        }
    if (AtLeastOneActiveAiIsStillAlive)
        { 
        return 0; 
        }
    
    DiscardAnyInPacketsInQueue();
    
    NetworkReceiveTimer= -2500; // Wait awhile before getting next InPacket
    StateTransitionTimer = -2000;
    state ++; 
    NetworkOpponent = false; 
    PeerPacketReceivedInThisMissionState = false; 
    if (state>10)
        {
        state = 10;
        }
    ThreeDObjects [1]->fighterkills = state -1;
    MissionHeadToHead00State = (unsigned char)state; // Advertise our state globally
    
    if (state==2)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 40;
       return 0; 
       }
    if (state==3)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 50;
       return 0; 
       }
    if (state==4)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 60;
       return 0; 
       }
    if (state==5)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 70;
       return 0; 
       }
    if (state==6)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 80;
       return 0; 
       }
    if (state==7)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 90;
       return 0; 
       }
    if (state==8)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 100;
       return 0; 
       }
    if (state==9)
       {
       
       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 110;
       return 0; 
       }
    return 1; 
    } 

void MissionHeadToHead00::draw ()
    {
    int timeroff = 100 * timestep, timerlag = 20 * timestep;
    
    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    } 

void MissionHeadToHead00LoadVariablesFromNetworkApiPacket(int timer)
{
   
   unsigned char PlayerNumber;
   
   extern LacUdpApiPacket InPacket;
   display ((char *) "MissionHeadToHead00LoadVariablesFromNetworkApiPacket()", LOG_NET);
   
   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {
      
      sound->setVolume (SOUND_BEEP1, 80); 
      sound->play (SOUND_BEEP1, false);
      display ((char *)"MissionHeadToHead00LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ((char *)"Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         { 
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ((char *)"This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         { 
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d", LOG_ERROR);
         display (DebugBuf, LOG_ERROR);
         display ((char *)"Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1; 
   
   PlayerNumber = MissionHeadToHead00State;
   display ((char *)"MissionHeadToHead00LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);
   
   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;
   
   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;
   
   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;
   
   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;
   
   NetworkApiPriorXPosition[MissionHeadToHead00State] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[MissionHeadToHead00State] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[MissionHeadToHead00State] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[MissionHeadToHead00State]     = timer;
   fplayer->Durability -= InPacket.UdpObjDamageAmount; // Accept damage from peer
} 

bool MissionHeadToHead00RetrieveFirstDamageDescription()
{
display ((char *)"MissionHeadToHead00RetrieveFirstDamageDescription()", LOG_NET);
MissionAircraftDamaged = 1; 
DamageToClaim = ThreeDObjects[1]->DamageInNetQueue; 
ThreeDObjects[1]->DamageInNetQueue=0; 
return (false);
}

MissionFreeFlightWW2::MissionFreeFlightWW2 ()
    {
    id = MISSION_FREEFLIGHTWW2;
    strncpy (name, "FREE FLIGHT", 1024);
    strncpy (briefing, "FREE FLIGHT OVER THE BLUE OCEAN, WITH NO OPPOSITION", 1024);
    selweapons = 1;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    } 

void MissionFreeFlightWW2::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    RadarOnOff=1;        
    RadarZoom=1;         
    IffOnOff=1;          
    event_RadarOnOff();  
    event_IffOnOff();    
    heading = 220;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915; 
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    playerInit ();
    fplayer->tl->x = 0;
    fplayer->tl->y = 200;
    fplayer->tl->z = 50;
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       event_IffOnOff();    
       }
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0; 
        fplayer->missiles [1] = 4; 
        fplayer->missiles [2] = 2; 
        fplayer->missiles [3] = 0; 
        fplayer->missiles [4] = 0; 
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0; 
        fplayer->missiles [1] = 0; 
        fplayer->missiles [2] = 0; 
        fplayer->missiles [3] = 0; 
        fplayer->missiles [4] = 0; 
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    } 

int MissionFreeFlightWW2::processtimer (Uint32 dt)
    {
    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    return 0;
    } 

void MissionFreeFlightWW2::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    } 

MissionNetworkBattle01::MissionNetworkBattle01 ()
    {
    
    id = MISSION_NETWORKBATTLE03;
    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE01", 1024);
       strncpy (briefing, "REQUIRES INTERNET AND CONFIGURED ACCESS TO A LAC SERVER.\n10 NETWORK PLAYERS IN 2 TEAMS. 1ST TEAM TO DESTROY ENEMY BASE\nWINS. ISLAND TERRAIN. YOUR FUEL LEVEL IS LOW. LAND AND REFUEL\nIMMEDIATELY. BOTS REPLACE ANY OF THE ONLINE PLAYERS NOT PRESENT.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }
    
    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 12;
    
    LoadServerIpAddress(); 
    
    NetworkOpponent = 0; 

    if (NetworkMode == 1)
      { 
      if (OpenClientUdpSocket() == 0)
        { 
        ConfigureClientUdpSocket();
        }
      else
        { 
        }
      }
    else
      {
      display ((char *)"Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    } 

void MissionNetworkBattle01::start ()
    {
    NoMissionHasYetCommenced = false;
    
    MissionStateNetworkBattle = 0;
    NetworkPlayerKilled = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       { 
       MyNetworkId = myrandom(MaxPlayersInCurrentMission); 
       MyNetworkId ++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915; 
    
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    
    int n = 23; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin - 21.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_battleship;
    ThreeDObjects [n]->newinit (STATIC_BATTLESHIP, 0, 400);
    ThreeDObjects [n]->impact = 1.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 12.0;

    n = 25; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 27; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;
    
    n = 22; 
    ThreeDObjects [n]->tl->x = AirfieldXMin - 470.58;
    ThreeDObjects [n]->tl->z = AirfieldYMin -21;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_battleship;
    ThreeDObjects [n]->newinit (STATIC_BATTLESHIP, 0, 400);
    ThreeDObjects [n]->impact = 1.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 12.0;

    n = 24; 
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58; 
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 28; 
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 26; 
    ThreeDObjects [n]->tl->x = AirfieldXMin - 467;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.50;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false; 
    fplayer->phi = 270;
    display ((char *)"MissionNetworkBattle01::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);
    
    HudOnOff = 1;               
    IffOnOff=0;                 
    MapViewOnOff = 0;           
    RadarOnOff=0;               
    RadarZoom = 1;              
    ScreenFOVx = 1.0;           
    ScreenFOVy = 1.0;           
    
    event_IffOnOff();           
    event_RadarOnOff();         
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();         
    event_ZoomFovOut();
    event_MapViewOnOff();       
    event_MapZoomIn();          
    event_MapZoomIn();
    event_MapViewOnOff();       
    MaxPlayersInCurrentMission = 10;
    
    for (i = 1; i <= 10; i ++)
        {
        
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 400);
        // Preserve aircraft type for later comparison to detect changes.
        MissionNetworkBattle01PriorAircraft[i] = FIGHTER_A6M2;
        if (i%2 == 0)
          { 
          ThreeDObjects [i]->party = 0;
          }
        else
          { 
          ThreeDObjects [i]->party = 1;
          }
        ThreeDObjects [i]->target = NULL;
        ThreeDObjects [i]->o = &model_figv;
        
        ThreeDObjects [i]->tl->x = -200 + (myrandom(400) - 200) + (150 * SIN(i * 360 / 11));
        ThreeDObjects [i]->tl->z =  200 + (myrandom(400) - 200) + (150 * COS(i * 360 / 11));
        ThreeDObjects [i]->ammo = 1600;
        ThreeDObjects [i]->Sentient = 0; 
        ThreeDObjects [i]->MyMissionPlayerNumber = i;
        sprintf (DebugBuf, "MissionNetworkBattle01::start() initializing Bot #%d", i);
        display (DebugBuf, LOG_MOST);
        sprintf (
                DebugBuf,
                "MissionNetworkBattle01::start() ThreeDObjects [i]->MyMissionPlayerNumber = %d",
                ThreeDObjects [i]->MyMissionPlayerNumber
                );
        display (DebugBuf, LOG_MOST);
        if (i>1)
           {
           //ThreeDObjects [i]->target = ThreeDObjects [i-1];
           }
        else if (i == 1)
           {
           //ThreeDObjects [i]->target = ThreeDObjects [10];
           }
        for (i2 = 0; i2 < missiletypes; i2 ++)
            {
            ThreeDObjects [i]->missiles [i2] = 0;
            }
        }
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000; 
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    if (MyNetworkId%2)
       { 
       fplayer->target = ThreeDObjects[28]; 
       }
    else
       { 
       fplayer->target = ThreeDObjects[29]; 
       }
    // Ensure aircraft is ready for combat
    ThrustReadiness = (
                   (float)fplayer->DefensiveLethality +
                   fplayer->maxthrust +
                   fplayer->RollRate +
                   fplayer->manoeverability +
                   fplayer->maxDurability +
                   fplayer->maxgamma +
                   fplayer->statLethality +
                   fplayer->StaticDrag +
                   fplayer->SpeedBrakePower +
                   fplayer->MaxFullPowerAltRatio +
                   fplayer->inertia +
                   fplayer->deadweight +
                   fplayer->CompressibilitySpeed +
                   fplayer->CompressibilitySpeedWithSpeedBrakes +
                   fplayer->StallSpeed +
                   fplayer->ServiceCeilingAltitude
                );
    if (fplayer->WepCapable)
       {
       ThrustReadiness += 200;
       }
    if (ThrustReadiness < 1000)
       {
       display ((char *)"Error. Selected Aircraft ThrustReadiness is insufficient.", LOG_MOST);
       game_quit(); // Airplane performance parameters didn't load properly.
       }
    } 

int MissionNetworkBattle01::processtimer (Uint32 dt)
    {
    
    int i;
    int MissionAircraftNumber;
    int BytesReceived = 0;
    MissionNetworkBattle01Timer += DeltaTime; 
    MissionNetworkBattleRadarTimer += DeltaTime; 
    if (MissionNetworkBattleRadarTimer > 100)
       { 
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          { 
          ThreeDObjects[24]->phi += 10; 
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          { 
          ThreeDObjects[25]->phi += 10; 
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot(); 
       }
    if (MissionNetworkBattle01Timer > 1000)
       { 
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --; 
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             { 
             ThreeDObjects[AircraftCount]->Sentient = 0;
             
             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          { 
          sound->setVolume (SOUND_BEEP1, 20); 
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle01Timer = 0;
       RepairDamagedAirfields();
       } 
    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       { 
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk; 
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle01::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             { 
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0; 
       }
    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       { 
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;
       
       if (TeamSwitcher == false)
          { 
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          { 
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       { 
       
       DegradeFlightModelDueToOrdnanceLoad();
       if (fplayer->id != FIGHTER_ME163)
          { 
          fplayer->FuelLevel = 6.0;
          }
       else
          { 
          fplayer->FuelLevel = 15.0;
          }
       if (fplayer->party == 1)
          { 
          fplayer->phi -= 180;
          }
       fplayer->FlapsLevel = 4;
       fplayer->UndercarriageLevel = 1;
       
       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[22]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[22]->tl->y += 0.5;

       ThreeDObjects[23]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[23]->tl->y += 0.5;

       ThreeDObjects[24]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[24]->tl->y += 5.4;
       ThreeDObjects[26]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[26]->tl->y += 5.6;
       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y +=0.90;
       
       ThreeDObjects[25]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[25]->tl->y += 5.4;
       ThreeDObjects[27]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[27]->tl->y += 5.4;
       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.90;

       if (fplayer->missiles [0] >= 4 || fplayer->id == FIGHTER_D3A || fplayer->id == BOMBER_B5N)
          { 
          fplayer->tl->y += 20;
          sprintf (DebugBuf, "MissionNetworkBattle01::processtimer() Heavy bomber launching. AirfieldRequested = %d", AirfieldRequested);
          display (DebugBuf, LOG_MOST);
          if (AirfieldRequested >= 3)
             { 
             display ((char *)"MissionNetworkBattle01::processtimer() Increasing heavy bomber spawn altitude from Field 4", LOG_MOST);
             fplayer->tl->y += 320; // This is a bomber airfield. Give them extra altitude at start. 160 yields 9400 ft.
             fplayer->FuelLevel = 99; // Full fuel when willing to take off from most remote airfield.
             }
          }
       fplayer->UndercarriageLevel = 0;
       fplayer->FlapsLevel = 0;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 150;
          }
       }
    else
       { 
       if (LandedAtSafeSpeed)
          { 
          RearmRefuelRepair();
          }
       }
    timer += dt; 
    
    if (timer > 15000 && MissionStateNetworkBattle == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       { 
       if (MyNetworkId % 2)
          { 
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle == 2 && (timer > PriorStateEndTimer +1100))
       {
       
       if (MyNetworkId%2)
          { 
          if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
             { 
             display ((char *)"MissionNetworkBattle01 state 2: Player's RedTeam HQ is heavily damaged.", LOG_MOST);
             if (fplayer->HistoricPeriod > 1)
                { 
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;
                
                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       { 
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       { 
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                { 
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             { 
             display ((char *)"MissionNetworkBattle01 state 2: Player's RedTeam HQ is NOT heavily damaged.", LOG_MOST);
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       else
          { 
          if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
             { 
             if (fplayer->HistoricPeriod > 1)
                { 
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;
                
                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       { 
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       { 
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                { 
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             { 
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE03, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE01, false);
       MissionStateNetworkBattle = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 13;
       PriorStateEndTimer = timer;
       }
    
    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {
        
        if (ThreeDObjects [0]->explode > 500 * timestep)
           { 
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           { 
           sound->setVolume (SOUND_BEEP1, 20); 
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(10000)>9990))
            { 
            sprintf (DebugBuf, "MissionNetworkBattle01::ProcessTimer() re-spawning aircraft #%d.", MissionAircraftNumber);
            display (DebugBuf, LOG_MOST);
            
            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               GetNetworkApiPacket(); 
               DiscardAnyInPacketsInQueue(); 
               InPacket.UdpObjXPosition -=300; 
               InPacket.UdpObjZPosition -=300; 
               NetworkReceiveTimer= -400; // Wait awhile before getting next InPacket
               // Preserve this aircraft type for later comparison to detect future changes:
               MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
               }
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 400);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability; 
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep; 
            ThreeDObjects [MissionAircraftNumber]->activate (); 
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;
            ThreeDObjects [MissionAircraftNumber]->MyMissionPlayerNumber = MissionAircraftNumber; 
            
            ThreeDObjects [MissionAircraftNumber]->tl->x = 200 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 200 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 260 + (myrandom(80) - 40);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            sprintf (
                    DebugBuf,
                    "MissionNetworkBattle01::processTimer() spawning new aircraft %d at X=%f Y=%f Z=%f",
                    MissionAircraftNumber,
                    ThreeDObjects[MissionAircraftNumber]->tl->x,
                    ThreeDObjects[MissionAircraftNumber]->tl->y,
                    ThreeDObjects[MissionAircraftNumber]->tl->z
                    );
            display (DebugBuf, LOG_MOST);
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0; 
            if (MissionAircraftNumber > 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[MissionAircraftNumber-1];
               }
            else if (MissionAircraftNumber == 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[10];
               }
            } 
        } 
    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       { 
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }
    
    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          { 
          display ((char *)"MissionNetworkBattle01 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             { 
              
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 400);
             }
          
          MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          } 
       }
    
    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       { 
       NetworkTransmitTimer=0;
       MissionNetworkBattle01RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       { 
       MissionEndingTimer -= dt; 
       if (MissionEndingTimer < 1000)
          {
          display ((char *)"MissionNetworkBattle01::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt; 
       if (MissionEndingTimer2 < 1000)
          { 
          display ((char *)"MissionNetworkBattle01::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    } 

void MissionNetworkBattle01::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    } 

bool MissionNetworkBattle01RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ((char *)"MissionNetworkBattle01RetrieveFirstDamageDescription()", LOG_NET);

for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   { 
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
} 

MissionNetworkBattle02::MissionNetworkBattle02 ()
    {
    
    id = MISSION_NETWORKBATTLE02;

    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE02", 1024);
       strncpy (briefing, "REQUIRES INTERNET & CONFIGURED LAC SERVER. 10 NETWORK PLAYERS\nIN 2 TEAMS. 1ST TEAM TO DESTROY ENEMY BASE WINS. DESERT TERRAIN.\nYOU WILL NEED TO LAND AND FUEL UP BEFORE YOU CAN LAUNCH AN\nATTACK. BOTS REPLACE ANY OF THE 10 ONLINE PLAYERS NOT PRESENT.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }
    
    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 12;
    
    LoadServerIpAddress(); 
    
    NetworkOpponent = 0; 

    if (NetworkMode == 1)
      { 
      if (OpenClientUdpSocket() == 0)
        { 
        ConfigureClientUdpSocket();
        }
      else
        { 
        }
      }
    else
      {
      display ((char *)"Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    } 

void MissionNetworkBattle02::start ()
    {
    NoMissionHasYetCommenced = false;
    
    MissionStateNetworkBattle = 0;
    NetworkPlayerKilled = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       { 
       MyNetworkId = myrandom(MaxPlayersInCurrentMission); 
       MyNetworkId++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_DESERT, NULL);
    SeaLevel = 50.0; 
    
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    
    int n = 25; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 24; 
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58; 
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 28; 
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false; 
    fplayer->phi = 270;
    display ((char *)"MissionNetworkBattle02::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);
    
    HudOnOff = 1;               
    IffOnOff=0;                 
    MapViewOnOff = 0;           
    RadarOnOff=0;               
    RadarZoom = 1;              
    ScreenFOVx = 1.0;           
    ScreenFOVy = 1.0;           
    
    event_IffOnOff();           
    event_RadarOnOff();         
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();         
    event_ZoomFovOut();
    event_MapViewOnOff();       
    event_MapZoomIn();          
    event_MapZoomIn();
    event_MapViewOnOff();       
    MaxPlayersInCurrentMission = 10;
    
    for (i = 1; i <= 10; i ++)
        {
        
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 400);
        // Preserve aircraft type for later comparison to detect changes.
        MissionNetworkBattle02PriorAircraft[i] = FIGHTER_A6M2;
        if (i%2 == 0)
          { 
          ThreeDObjects [i]->party = 0;
          }
        else
          { 
          ThreeDObjects [i]->party = 1;
          }
        ThreeDObjects [i]->target = NULL;
        ThreeDObjects [i]->o = &model_figv;
        
        ThreeDObjects [i]->tl->x = -200 + (myrandom(400) - 200) + (150 * SIN(i * 360 / 11));
        ThreeDObjects [i]->tl->z =  200 + (myrandom(400) - 200) + (150 * COS(i * 360 / 11));
        ThreeDObjects [i]->ammo = 1600;
        ThreeDObjects [i]->Sentient = 0; 
        ThreeDObjects [i]->MyMissionPlayerNumber = i;
        sprintf (DebugBuf, "MissionNetworkBattle02::start() initializing Bot #%d", i);
        display (DebugBuf, LOG_MOST);
        sprintf (
                DebugBuf,
                "MissionNetworkBattle02::start() ThreeDObjects [i]->MyMissionPlayerNumber = %d",
                ThreeDObjects [i]->MyMissionPlayerNumber
                );
        display (DebugBuf, LOG_MOST);
        if (i>1)
           {
           ThreeDObjects [i]->target = ThreeDObjects [i-1];
           }
        else if (i == 1)
           {
           ThreeDObjects [i]->target = ThreeDObjects [10];
           }
        for (i2 = 0; i2 < missiletypes; i2 ++)
            {
            ThreeDObjects [i]->missiles [i2] = 0;
            }
        }
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000; 
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    if (MyNetworkId%2)
       { 
       fplayer->target = ThreeDObjects[28]; 
       }
    else
       { 
       fplayer->target = ThreeDObjects[29]; 
       }
    // Ensure aircraft is ready for combat
    ThrustReadiness = (
                   (float)fplayer->DefensiveLethality +
                   fplayer->maxthrust +
                   fplayer->RollRate +
                   fplayer->manoeverability +
                   fplayer->maxDurability +
                   fplayer->maxgamma +
                   fplayer->statLethality +
                   fplayer->StaticDrag +
                   fplayer->SpeedBrakePower +
                   fplayer->MaxFullPowerAltRatio +
                   fplayer->inertia +
                   fplayer->deadweight +
                   fplayer->CompressibilitySpeed +
                   fplayer->CompressibilitySpeedWithSpeedBrakes +
                   fplayer->StallSpeed +
                   fplayer->ServiceCeilingAltitude
                );
    if (fplayer->WepCapable)
       {
       ThrustReadiness += 200;
       }
    if (ThrustReadiness < 1000)
       {
       display ((char *)"Error. Selected Aircraft ThrustReadiness is insufficient.", LOG_MOST);
       game_quit(); // Airplane performance parameters didn't load properly.
       }
    } 

int MissionNetworkBattle02::processtimer (Uint32 dt)
    {
    
    int i;
    int MissionAircraftNumber;
    int BytesReceived = 0;

    MissionNetworkBattleRadarTimer += DeltaTime; 
    if (MissionNetworkBattleRadarTimer > 100)
       { 
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          { 
          ThreeDObjects[24]->phi += 10; 
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          { 
          ThreeDObjects[25]->phi += 10; 
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot(); 
       }

    MissionNetworkBattle02Timer += DeltaTime; 
    if (MissionNetworkBattle02Timer > 1000)
       { 
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --; 
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             { 
             ThreeDObjects[AircraftCount]->Sentient = 0;
             
             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          { 
          sound->setVolume (SOUND_BEEP1, 20); 
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle02Timer = 0;
       RepairDamagedAirfields();
       } 
    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       { 
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk; 
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle02::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             { 
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0; 
       }

    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       { 
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;
       
       if (TeamSwitcher == false)
          { 
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          { 
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       { 
       
       DegradeFlightModelDueToOrdnanceLoad();
       if (AirfieldRequested != 4)
          { 
          fplayer->FuelLevel = 6.0; 
          }
       if (fplayer->party == 1)
          { 
          fplayer->phi -= 180;
          }
       
       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y += 0.9;
       ThreeDObjects[24]->tl->y = ThreeDObjects[28]->tl->y + 2.0; 

       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.9;
       ThreeDObjects[25]->tl->y = ThreeDObjects[29]->tl->y + 2.0; 

       if (fplayer->missiles [0] >= 4 || fplayer->id == FIGHTER_D3A || fplayer->id == BOMBER_B5N)
          { 
          fplayer->tl->y += 20;
          sprintf (DebugBuf, "MissionNetworkBattle02::processtimer() Heavy bomber launching. AirfieldRequested = %d", AirfieldRequested);
          display (DebugBuf, LOG_MOST);
          if (AirfieldRequested >= 3)
             { 
             display ((char *)"MissionNetworkBattle02::processtimer() Increasing heavy bomber spawn altitude from Field 4", LOG_MOST);
             fplayer->tl->y += 320; // This is a heavy bomber airfield. Give them extra altitude at start. 320 yields 9400ft.
             fplayer->FuelLevel = 99; // Full fuel when willing to take off from most remote airfield.
             }
          }
       fplayer->UndercarriageLevel = 0;
       fplayer->FlapsLevel = 0;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 150;
          }
       }
    else
       { 
       if (LandedAtSafeSpeed)
          { 
          RearmRefuelRepair();
          }
       }
    timer += dt; 
    
    if (timer > 15000 && MissionStateNetworkBattle == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       { 
       if (MyNetworkId % 2)
          { 
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle == 2 && (timer > PriorStateEndTimer +1100))
       {
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE02, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE02, false);
       MissionStateNetworkBattle = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 13;
       PriorStateEndTimer = timer;
       }
    
    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {
        
        if (ThreeDObjects [0]->explode > 500 * timestep)
           { 
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           { 
           sound->setVolume (SOUND_BEEP1, 20); 
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(1000)>995))
            { 
            GetNetworkApiPacket(); 
            DiscardAnyInPacketsInQueue(); 
            InPacket.UdpObjXPosition -=300; 
            InPacket.UdpObjZPosition -=300; 
            
            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               GetNetworkApiPacket(); 
               DiscardAnyInPacketsInQueue(); 
               InPacket.UdpObjXPosition -=300; 
               InPacket.UdpObjZPosition -=300; 
               NetworkReceiveTimer= -400; // Wait awhile before getting next InPacket
               // Preserve this aircraft type for later comparison to detect future changes:
               MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
               }
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 400);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability; 
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep; 
            ThreeDObjects [MissionAircraftNumber]->activate (); 
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;
            ThreeDObjects [MissionAircraftNumber]->MyMissionPlayerNumber = MissionAircraftNumber; 
            
            ThreeDObjects [MissionAircraftNumber]->tl->x = 200 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 200 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 260 + (myrandom(80) - 40);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            sprintf (
                    DebugBuf,
                    "MissionNetworkBattle02::processTimer() spawning new aircraft %d at X=%f Y=%f Z=%f",
                    MissionAircraftNumber,
                    ThreeDObjects[MissionAircraftNumber]->tl->x,
                    ThreeDObjects[MissionAircraftNumber]->tl->y,
                    ThreeDObjects[MissionAircraftNumber]->tl->z
                    );
            display (DebugBuf, LOG_MOST);
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0; 
            if (MissionAircraftNumber > 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[MissionAircraftNumber-1];
               }
            else if (MissionAircraftNumber == 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[10];
               }
            } 
        } 
    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       { 
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }
    
    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          { 
          display ((char *)"MissionNetworkBattle02 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             { 
              
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 400);
             }
          
          MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          } 
       }
    
    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       { 
       NetworkTransmitTimer=0;
       MissionNetworkBattle02RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       { 
       MissionEndingTimer -= dt; 
       if (MissionEndingTimer < 1000)
          {
          display ((char *)"MissionNetworkBattle02::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt; 
       if (MissionEndingTimer2 < 1000)
          { 
          display ((char *)"MissionNetworkBattle02::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    } 

void MissionNetworkBattle02::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    } 

bool MissionNetworkBattle02RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ((char *)"MissionNetworkBattle02RetrieveFirstDamageDescription()", LOG_NET);
for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   { 
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
} 

MissionNetworkBattle03::MissionNetworkBattle03 ()
    {
    
    id = MISSION_NETWORKBATTLE03;
    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE03", 1024);
       strncpy (briefing, "REQUIRES INTERNET AND CONFIGURED ACCESS TO A LAC SERVER.\n10 NETWORK PLAYERS IN 2 TEAMS. 1ST TEAM TO DESTROY ENEMY BASE\nWINS. ISLAND TERRAIN. YOU ARE FUELED UP AND READY TO LAUNCH AN\nATTACK. BOTS REPLACE ANY OF THE 10 ONLINE PLAYERS NOT PRESENT.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }
    
    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 12;
    
    LoadServerIpAddress(); 
    
    NetworkOpponent = 0; 

    if (NetworkMode == 1)
      { 
      if (OpenClientUdpSocket() == 0)
        { 
        ConfigureClientUdpSocket();
        }
      else
        { 
        }
      }
    else
      {
      display ((char *)"Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    } 

void MissionNetworkBattle03::start ()
    {
    NoMissionHasYetCommenced = false;
    
    MissionStateNetworkBattle = 0;
    NetworkPlayerKilled = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       { 
       MyNetworkId = myrandom(MaxPlayersInCurrentMission); 
       MyNetworkId ++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915; 
    
    l->flatten (AirfieldXMin+28.0, AirfieldYMin+4, 29, 5); 
    l->flatten (AirfieldXMin+46, AirfieldYMin+5, 7, 6); 
    
    int n = 23; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin - 21.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_battleship;
    ThreeDObjects [n]->newinit (STATIC_BATTLESHIP, 0, 400);
    ThreeDObjects [n]->impact = 1.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 12.0;

    n = 25; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 27; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29; 
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;
    
    n = 22; 
    ThreeDObjects [n]->tl->x = AirfieldXMin - 470.58;
    ThreeDObjects [n]->tl->z = AirfieldYMin -21;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_battleship;
    ThreeDObjects [n]->newinit (STATIC_BATTLESHIP, 0, 400);
    ThreeDObjects [n]->impact = 1.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 12.0;

    n = 24; 
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58; 
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 28; 
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 26; 
    ThreeDObjects [n]->tl->x = AirfieldXMin - 467;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.50;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false; 
    fplayer->phi = 270;
    display ((char *)"MissionNetworkBattle03::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);
    
    HudOnOff = 1;               
    IffOnOff=0;                 
    MapViewOnOff = 0;           
    RadarOnOff=0;               
    RadarZoom = 1;              
    ScreenFOVx = 1.0;           
    ScreenFOVy = 1.0;           
    
    event_IffOnOff();           
    event_RadarOnOff();         
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();         
    event_ZoomFovOut();
    event_MapViewOnOff();       
    event_MapZoomIn();          
    event_MapZoomIn();
    event_MapViewOnOff();       
    MaxPlayersInCurrentMission = 10;
    
    for (i = 1; i <= 10; i ++)
        {
        
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 400);
        // Preserve aircraft type for later comparison to detect changes.
        MissionNetworkBattle01PriorAircraft[i] = FIGHTER_A6M2;
        if (i%2 == 0)
          { 
          ThreeDObjects [i]->party = 0;
          }
        else
          { 
          ThreeDObjects [i]->party = 1;
          }
        ThreeDObjects [i]->target = NULL;
        ThreeDObjects [i]->o = &model_figv;
        
        ThreeDObjects [i]->tl->x = -200 + (myrandom(400) - 200) + (150 * SIN(i * 360 / 11));
        ThreeDObjects [i]->tl->z =  200 + (myrandom(400) - 200) + (150 * COS(i * 360 / 11));
        ThreeDObjects [i]->ammo = 1600;
        ThreeDObjects [i]->Sentient = 0; 
        ThreeDObjects [i]->MyMissionPlayerNumber = i;
        sprintf (DebugBuf, "MissionNetworkBattle03::start() initializing Bot #%d", i);
        display (DebugBuf, LOG_MOST);
        sprintf (
                DebugBuf,
                "MissionNetworkBattle03::start() ThreeDObjects [i]->MyMissionPlayerNumber = %d",
                ThreeDObjects [i]->MyMissionPlayerNumber
                );
        display (DebugBuf, LOG_MOST);
        if (i>1)
           {
           //ThreeDObjects [i]->target = ThreeDObjects [i-1];
           }
        else if (i == 1)
           {
           //ThreeDObjects [i]->target = ThreeDObjects [10];
           }
        for (i2 = 0; i2 < missiletypes; i2 ++)
            {
            ThreeDObjects [i]->missiles [i2] = 0;
            }
        }
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000; 
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    if (MyNetworkId%2)
       { 
       fplayer->target = ThreeDObjects[28]; 
       }
    else
       { 
       fplayer->target = ThreeDObjects[29]; 
       }
    // Ensure aircraft is ready for combat
    ThrustReadiness = (
                   (float)fplayer->DefensiveLethality +
                   fplayer->maxthrust +
                   fplayer->RollRate +
                   fplayer->manoeverability +
                   fplayer->maxDurability +
                   fplayer->maxgamma +
                   fplayer->statLethality +
                   fplayer->StaticDrag +
                   fplayer->SpeedBrakePower +
                   fplayer->MaxFullPowerAltRatio +
                   fplayer->inertia +
                   fplayer->deadweight +
                   fplayer->CompressibilitySpeed +
                   fplayer->CompressibilitySpeedWithSpeedBrakes +
                   fplayer->StallSpeed +
                   fplayer->ServiceCeilingAltitude
                );
    if (fplayer->WepCapable)
       {
       ThrustReadiness += 200;
       }
    if (ThrustReadiness < 1000)
       {
       display ((char *)"Error. Selected Aircraft ThrustReadiness is insufficient.", LOG_MOST);
       game_quit(); // Airplane performance parameters didn't load properly.
       }
    } 

int MissionNetworkBattle03::processtimer (Uint32 dt)
    {
    
    int i;
    int MissionAircraftNumber;
    int BytesReceived = 0;
    MissionNetworkBattle03Timer += DeltaTime; 
    MissionNetworkBattleRadarTimer += DeltaTime; 
    if (MissionNetworkBattleRadarTimer > 100)
       { 
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          { 
          ThreeDObjects[24]->phi += 10; 
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          { 
          ThreeDObjects[25]->phi += 10; 
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot(); 
       }
    if (MissionNetworkBattle03Timer > 1000)
       { 
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --; 
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             { 
             ThreeDObjects[AircraftCount]->Sentient = 0;
             
             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          { 
          sound->setVolume (SOUND_BEEP1, 20); 
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle03Timer = 0;
       RepairDamagedAirfields();
       } 
    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       { 
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk; 
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle03::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             { 
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0; 
       }
    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       { 
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;
       
       if (TeamSwitcher == false)
          { 
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          { 
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             { 
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0; 
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       { 
       
       DegradeFlightModelDueToOrdnanceLoad();
       if (fplayer->id != FIGHTER_ME163)
          { 
          fplayer->FuelLevel = 94.0;
          }
       else
          { 
          fplayer->FuelLevel = 15.0;
          }
       if (fplayer->party == 1)
          { 
          fplayer->phi -= 180;
          }
       fplayer->FlapsLevel = 4;
       fplayer->UndercarriageLevel = 1;
       
       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[22]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[22]->tl->y += 0.5;

       ThreeDObjects[23]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[23]->tl->y += 0.5;

       ThreeDObjects[24]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[24]->tl->y += 5.4;
       ThreeDObjects[26]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[26]->tl->y += 5.6;
       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y +=0.90;
       
       ThreeDObjects[25]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[25]->tl->y += 5.4;
       ThreeDObjects[27]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[27]->tl->y += 5.4;
       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.90;

       if (fplayer->missiles [0] >= 4 || fplayer->id == FIGHTER_D3A || fplayer->id == BOMBER_B5N)
          { 
          fplayer->tl->y += 20;
          sprintf (DebugBuf, "MissionNetworkBattle03::processtimer() Heavy bomber launching. AirfieldRequested = %d", AirfieldRequested);
          display (DebugBuf, LOG_MOST);
          if (AirfieldRequested >= 3)
             { 
             display ((char *)"MissionNetworkBattle03::processtimer() Increasing heavy bomber spawn altitude from Field 4", LOG_MOST);
             fplayer->tl->y += 320; // This is a bomber airfield. Give them extra altitude at start. 160 yields 9400 ft.
             fplayer->FuelLevel = 99; // Full fuel when willing to take off from most remote airfield.
             }
          }
       fplayer->UndercarriageLevel = 0;
       fplayer->FlapsLevel = 0;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 150;
          }
       }
    else
       { 
       if (LandedAtSafeSpeed)
          { 
          RearmRefuelRepair();
          }
       }
    timer += dt; 
    
    if (timer > 15000 && MissionStateNetworkBattle == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       { 
       if (MyNetworkId % 2)
          { 
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle == 2 && (timer > PriorStateEndTimer +1100))
       {
       
       if (MyNetworkId%2)
          { 
          if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
             { 
             display ((char *)"MissionNetworkBattle03 state 2: Player's RedTeam HQ is heavily damaged.", LOG_MOST);
             if (fplayer->HistoricPeriod > 1)
                { 
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;
                
                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       { 
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       { 
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                { 
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             { 
             display ((char *)"MissionNetworkBattle03 state 2: Player's RedTeam HQ is NOT heavily damaged.", LOG_MOST);
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       else
          { 
          if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
             { 
             if (fplayer->HistoricPeriod > 1)
                { 
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;
                
                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       { 
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       { 
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                { 
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             { 
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE03, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE03, false);
       MissionStateNetworkBattle = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle = 13;
       PriorStateEndTimer = timer;
       }
    
    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {
        
        if (ThreeDObjects [0]->explode > 500 * timestep)
           { 
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           { 
           sound->setVolume (SOUND_BEEP1, 20); 
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(10000)>9990))
            { 
            sprintf (DebugBuf, "MissionNetworkBattle03::ProcessTimer() re-spawning aircraft #%d.", MissionAircraftNumber);
            display (DebugBuf, LOG_MOST);
            
            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               GetNetworkApiPacket(); 
               DiscardAnyInPacketsInQueue(); 
               InPacket.UdpObjXPosition -=300; 
               InPacket.UdpObjZPosition -=300; 
               NetworkReceiveTimer= -400; // Wait awhile before getting next InPacket
               // Preserve this aircraft type for later comparison to detect future changes:
               MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
               }
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 400);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability; 
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep; 
            ThreeDObjects [MissionAircraftNumber]->activate (); 
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;
            ThreeDObjects [MissionAircraftNumber]->MyMissionPlayerNumber = MissionAircraftNumber; 
            
            ThreeDObjects [MissionAircraftNumber]->tl->x = 200 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 200 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 260 + (myrandom(80) - 40);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            sprintf (
                    DebugBuf,
                    "MissionNetworkBattle03::processTimer() spawning new aircraft %d at X=%f Y=%f Z=%f",
                    MissionAircraftNumber,
                    ThreeDObjects[MissionAircraftNumber]->tl->x,
                    ThreeDObjects[MissionAircraftNumber]->tl->y,
                    ThreeDObjects[MissionAircraftNumber]->tl->z
                    );
            display (DebugBuf, LOG_MOST);
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0; 
            if (MissionAircraftNumber > 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[MissionAircraftNumber-1];
               }
            else if (MissionAircraftNumber == 1)
               {
               ThreeDObjects [MissionAircraftNumber]->target = ThreeDObjects[10];
               }
            } 
        } 
    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       { 
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }
    
    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          { 
          display ((char *)"MissionNetworkBattle03 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             { 
              
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 400);
             }
          
          MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          } 
       }
    
    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       { 
       NetworkTransmitTimer=0;
       MissionNetworkBattle03RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       { 
       MissionEndingTimer -= dt; 
       if (MissionEndingTimer < 1000)
          {
          display ((char *)"MissionNetworkBattle03::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt; 
       if (MissionEndingTimer2 < 1000)
          { 
          display ((char *)"MissionNetworkBattle03::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    } 

void MissionNetworkBattle03::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    } 

bool MissionNetworkBattle03RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ((char *)"MissionNetworkBattle03RetrieveFirstDamageDescription()", LOG_NET);
for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   { 
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      { 
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue; 
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
} 

void RearmRefuelRepair()
{
if (MyNetworkId % 2)
   { 
   if (fplayer->tl->x < 0.0)
      { 
      sprintf (SystemMessageBufferA, "THIS IS A HOSTILE AIRFIELD!");
      NewSystemMessageNeedsScrolling = true;
      
      if (fplayer->FuelLevel > 6)
         {
         fplayer->FuelLevel = 6;
         }
      if (fplayer->ammo > 30)
         {
         fplayer->ammo = 30;
         }
      return;
      }
   }
else
   { 
   if (fplayer->tl->x > 0.0)
      { 
      sprintf (SystemMessageBufferA, "THIS IS A HOSTILE AIRFIELD!");
      NewSystemMessageNeedsScrolling = true;
      
      if (fplayer->FuelLevel > 6)
         {
         fplayer->FuelLevel = 6;
         }
      if (fplayer->ammo > 30)
         {
         fplayer->ammo = 30;
         }
      return;
      }
   }
FuelLevelAtLastLanding = fplayer->FuelLevel; 
if (fplayer->id != FIGHTER_ME163)
   { 
   fplayer->FuelLevel = 100;
   fplayer->maxthrust = PlayersOriginalMaxThrust;
   fplayer->RollRate = PlayersOriginalRollRate;
   lastDurability = PlayersOriginalDurability;
   if (fplayer->ammo < 1600)
      { 
      fplayer->ammo = 1600; 
      }
   fplayer->Durability = fplayer->maxDurability; 
   ConfigureOrdnanceForOnlineMissions();
   sprintf (SystemMessageBufferA, "REARMED, REFUELED, REPAIRED.");
   NewSystemMessageNeedsScrolling = true;
   }
else
   { 
   if (Me163LandingTimer >= 100)
      {
      Me163LandingTimer -= DeltaTime;
      sprintf (SystemMessageBufferA, "CYCLING ME163 REFUEL CREW....");
      NewSystemMessageNeedsScrolling = true;
      }
   if (Me163LandingTimer < 40000)
      {
      sprintf (SystemMessageBufferA, "MOUNTING TAKEOFF WHEELS....");
      NewSystemMessageNeedsScrolling = true;
      }
   if (Me163LandingTimer < 20000)
      {
      sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #1....");
      NewSystemMessageNeedsScrolling = true;
      }
   if (Me163LandingTimer < 10000)
      {
      sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #2....");
      NewSystemMessageNeedsScrolling = true;
      }
   if (Me163LandingTimer < 1100)
      { 
      sound->setVolume (SOUND_BEEP2, 20); 
      sound->play (SOUND_BEEP2, false);
      fplayer->FuelLevel = 100;
      fplayer->maxthrust = PlayersOriginalMaxThrust;
      fplayer->RollRate = PlayersOriginalRollRate;
      lastDurability = PlayersOriginalDurability;
      fplayer->Durability = fplayer->maxDurability;
      fplayer->ammo = 120;
      Me163LandingTimer = 60000;
      sprintf (SystemMessageBufferA, "REARMED, REFUELED, REPAIRED");
      NewSystemMessageNeedsScrolling = true;
      display (SystemMessageBufferA, LOG_MOST);
      }
   } 
  
  if (MyNetworkId%2)
     { 
     if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
        { 
        DetermineCurrentAirfield();
        if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 4)
           { 
           sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
           NewSystemMessageNeedsScrolling = true;
           fplayer->FuelLevel *= 0.2;
           
           if (fplayer->FuelLevel < FuelLevelAtLastLanding)
              {
              fplayer->FuelLevel = FuelLevelAtLastLanding;
              }
           fplayer->ammo /= 5;
           
           int i2;
           for (i2 = 0; i2 < missiletypes; i2 ++)
               {
               if (i2 == 0)
                  { 
                  fplayer->missiles [i2] /= 3;
                  }
               if (i2 == 5)
                  { 
                  fplayer->missiles [i2] /=3;
                  }
               }
           }
        else
           {
           
           }
        }
     else
        { 
        }
     }
  else
     { 
     if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
        { 
        DetermineCurrentAirfield();
        if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 1)
           { 
           sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
           NewSystemMessageNeedsScrolling = true;
           fplayer->FuelLevel *= 0.2;
           
           if (fplayer->FuelLevel < FuelLevelAtLastLanding)
              {
              fplayer->FuelLevel = FuelLevelAtLastLanding;
              }
           fplayer->ammo /= 5;
           
           int i2;
           for (i2 = 0; i2 < missiletypes; i2 ++)
               {
               if (i2 == 0)
                  { 
                  fplayer->missiles [i2] /= 3;
                  }
               if (i2 == 5)
                  { 
                  fplayer->missiles [i2] /=3;
                  }
               }
           }
        else
           {
           
           }
        }
     else
        { 
        }
     }
}

