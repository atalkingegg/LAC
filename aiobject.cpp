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

/* This file includes all AI objects instancing models. */

//##################################################################################################
// Headers needed by aiobject.cpp
//##################################################################################################

#include <math.h> /* cos */
#include "common.h" /* Uint32 */
#include <stdio.h> /* FILE */
#include <assert.h> /* assert */

#include "mathtab.h" /* MAXSMOKEELEM */
#include "vertexarray.h" /* VertexArray */
#include "model.h" /* CColor, depends on mathtab.h and vertexarray.h */
#include "object.h" /* CSpaceObj, depends on model.h */
#include "effects.h" /* CSmoke, depends on object.h */
#include "aiobject.h" /* AIObj, depends on object.h, effects.h, and mission.h */
#include "mission.h" /* MISSION_HEADTOHEAD00, depends on aiobject.h */
#include "3ds.h" /* CLoad3DS */
#include "pilots.h" /* PilotList */
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h> /* SDL_AudioSpec */
#include "audio.h" /* SoundSystem, depends on SDL_mixer */
#include "main.h" /* ThreeDObjects *** global variable, depends on 3ds.h, pilots.h, audio.h */
#include "NetworkApi.h" /* LacUdpApiPacket */

//#include "aiobject.h"
//#include "glland.h"
//#include "land.h"
//#include "main.h"
//#include "mathtab.h"
//#include "mission.h"
//#include "NetworkApi.h"

extern bool ClearSpeedHistoryArrayFlag;
extern bool GearUpDamageAlreadyInflicted;
extern bool LandedAtSafeSpeed;
extern bool LandingLogged;
extern bool OnTheGround;
extern bool MissileFired;
extern bool NetworkMissionIsActiveWithDataFlow;
extern bool NewSystemMessageNeedsScrolling;
extern bool RadarReflectorBlueHasBeenDestroyedAtLeastOnce;
extern bool RadarReflectorRedHasBeenDestroyedAtLeastOnce;
extern bool RecoveredFromStall;
extern bool TakeoffLogged;

extern char CurrentMissionNumber;

extern char DebugBuf[100];
extern char SystemMessageBufferA[];

extern unsigned char BattleshipSunkAlertCountBlue;
extern unsigned char BattleshipSunkAlertCountRed;
extern unsigned char MissionHeadToHead00State;
extern unsigned char MyNetworkId;
extern unsigned char NetworkOpponent; 
extern unsigned char RecentVictim;

extern int AirfieldXMax;    
extern int AirfieldXMin;    
extern int AirfieldYMax;    
extern int AirfieldYMin;    
extern int IffOnOff;
extern int MissionNumber;
extern int RadarOnOff;

extern Uint32 DeltaTime;
extern Uint32 MissionEndingTimer;
extern Uint32 ViewResetTimer;

extern float BlackoutSensitivity;
extern float CompressibilityRollFactor;
extern float ConfiguredViewDistance;
extern float RecentVictimAltitude;
extern float RecentVictimGamma;
extern float RecentVictimPhi;
extern float RecentVictimTheta;
extern float RecentVictimVelocity;
extern float RecentVictimXPosition;
extern float RecentVictimZPosition;
extern float RegulatedForceX;
extern float RegulatedForceY;
extern float RegulatedForceZ;
extern float SeaLevel;
extern float TrueAirSpeed;

extern LacUdpApiPacket InPacket;

extern FILE *OnlineScoreLogFile;

/*
*
* Function Prototypes:
*/

void CalculateTrueAirspeed();
void event_targetNext();
void RegulateGamma ();
void RegulateXYZForces(float forcex, float forcey, float forcez);
void TestAltitudeAirDensity();
void TestDiveSpeedStructuralLimit();
void TestForExcessGamma();
void TestForFlapEffects();
void TestForLoopBeyondVertical();
void TestForSpeedBrakeDrag();
void TestForUndercarriageEffects();
void TestForWindNoise();

extern void UpdateOnlineScoreLogFileWithCrashes();
extern void UpdateOnlineScoreLogFileWithLandings();
extern void UpdateOnlineScoreLogFileWithTakeoffs();

// core AI method
void AIObj::aiAction (Uint32 dt, AIObj **f, AIObj **m, DynamicObj **c, DynamicObj **flare, DynamicObj **chaff)
    {
    int i;
    // which height???
    float recheight2 = 0.0; // this is the height the object wants to achieve
    timer += dt;
    if (!active && !draw)   // not active, not drawn, then exit
        {
        return;
        }
    if (firecannonttl > 0)
        {
        firecannonttl -= dt;    // time to fire the next cannon
        }
    if (firemissilettl > 0)
        {
        firemissilettl -= dt;    // time to fire the next missile
        }
    if (fireflarettl > 0)
        {
        fireflarettl -= dt;    // time to fire the next flare
        }
    if (firechaffttl > 0)
        {
        firechaffttl -= dt;    // time to fire the next chaff
        }
    if (smokettl > 0)
        {
        smokettl -= dt;        // time to activate next smoke element
        }
    // move object according to our physics
    move (dt);
    float timefac = (float) dt / (float) timestep;
    if (id >= STATIC_PASSIVE)   // no AI for static ground objects (buildings)
        {
        return;
        }
    
    if (haveMissile())
       { 
       if ((missiletype == BOMB01 - MISSILE1) || (missiletype == MISSILE_DF1 - MISSILE1))
          { 
          ttf = 0;
          }
       }
    // set smoke for objects in motion:
    if ((id >= MISSILE1 && id < MISSILE_MINE1) || (id >= FIGHTER1 && id <= FIGHTER2))   // missile or fighter
        {
        float sz = COS(gamma) * COS(phi) * zoom * 1.1; // polar (spherical) coordinates
        float sy = -SIN(gamma) * zoom * 1.1;
        float sx = COS(gamma) * SIN(phi) * zoom * 1.1;
        // some smoke elements per discrete movement
        float fg = sqrt (forcex * forcex + forcey * forcey + forcez * forcez) * 13;
        if (fg >= MAXSMOKEELEM)
            {
            fg = (float) MAXSMOKEELEM - 0.5;
            }
        for (i = 0; i < (int) fg; i ++)
            {
            float fac = (float) i / fg;
            smoke->setSmoke (tl->x - sx - forcex * fac, tl->y - sy - forcey * fac, tl->z - sz - forcez * fac, (int) phi, 39 - i);
            }
        smoke->move (dt, (int) fg + 1);
        smokettl += timestep;
        }
    if (!active)   // not active, then exit
        {
        return;
        }
    if (explode > 0 || sink > 0)   // exploding or sinking, then exit
        {
        thrust = 0;
        return;
        }
    if (NetworkMissionIsActiveWithDataFlow)
        { 
        return; 
        }
    
    // do expensive calculations only once
    float myheight = l->getExactHeight (tl->x, tl->z);
    float targetheight = tl->y;
    
    if (target != NULL)
        {
        targetheight = l->getExactHeight (target->tl->x, target->tl->z);
        disttarget = distance (target);    // distance to target
        }
    else
        { 
        disttarget = 1;
        }
    // get a new target if necessary
    if (id >= MISSILE1 && id <= MISSILE2)
        { 
        if (target == NULL)
            {
            ttl = 0;
            }
        else if (!target->active)
            {
            ttl = 0;
            }
        }
    if (target == NULL)
        {
        if (bomber)
            {
            targetNearestGroundEnemy (f);
            }
        else
            {
            targetNearestEnemy (f);
            }
        }
    if (target != NULL)
        if (!target->active)
            {
            if (bomber)
                {
                targetNearestGroundEnemy (f);
                }
            else
                {
                targetNearestEnemy (f);
                }
            }
    if (id >= FIGHTER1 && id <= FIGHTER2)
        { 
        if (haveMissile () && target != NULL)
            { 
            
            float dgamma = atan ((target->tl->y - tl->y) / disttarget) * 180 / M_PI - (gamma - 180);
            float dphi = getAngle (target);

            if ((missiletype == BOMB01 - MISSILE1) || (missiletype == MISSILE_DF1 - MISSILE1))
                { 
                ttf = 0;
                }
            else if (fabs (dphi) < 50 && fabs (dgamma) < 50 && party != target->party)
                {
                if (disttarget < 75)
                    {
                    if (ttf > 0)
                        {
                        if (missiletype >= 0 && missiletype <= 2)
                            {
                            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                                {
                                float dphi = fabs (phi - target->phi);

                                if (dphi > 270)
                                    {
                                    dphi = 360 - dphi;
                                    }

                                if (dphi < 45)
                                    {
                                    ttf -= 2 * dt;
                                    }
                                else
                                    {
                                    ttf = 50 * timestep;
                                    }
                                }
                            }
                        else if (missiletype == 6 || missiletype == 7)
                            {
                            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                                {
                                ttf -= 2 * dt;
                                }
                            }
                        else
                            {
                            if (target->id > FIGHTER2)
                                {
                                ttf -= 2 * dt;
                                }
                            }
                        }
                    }
                }
            else
                {
                ttf = 50 * timestep;
                }
            } 
        } 
    if (!ai)
        {
        return;
        }
    
    if (
        NetworkOpponent &&
           (
            MissionNumber == MISSION_HEADTOHEAD00    ||
            MissionNumber == MISSION_NETWORKBATTLE01 ||
            MissionNumber == MISSION_NETWORKBATTLE02 ||
            MissionNumber == MISSION_NETWORKBATTLE03
            )
        )
           { 
           manoeverstate = 100; 
           if (MissionNumber == MISSION_HEADTOHEAD00)
              {
              ThreeDObjects[MissionHeadToHead00State]->realspeed = InPacket.UdpObjSpeed;
              ThreeDObjects[MissionHeadToHead00State]->elevatoreffect = InPacket.UdpObjElevator;
              ThreeDObjects[MissionHeadToHead00State]->rolleffect = InPacket.UdpObjAileron;
              ThreeDObjects[MissionHeadToHead00State]->ruddereffect = InPacket.UdpObjRudder;
              ThreeDObjects[MissionHeadToHead00State]->thrust = InPacket.UdpObjThrust;
              }
           } 

    int lsdist = 15;
    float flyx = tl->x + forcex * lsdist, flyz = tl->z + forcez * lsdist;
    int flyxs = l->getCoord ((int) flyx), flyzs = l->getCoord ((int) flyz);
    if ((manoeverheight > 0) & (manoeverstate !=100))
      {
      
      recheight2 = l->getExactHeight (flyx, flyz) + recheight;
      }
    else
      {
      // missiles and non intelligent objects will not change their height due to the surface
      if (
            (
            id >= MISSILE1 &&
            id <= MISSILE2 &&
            target != NULL
            )
          ||
            (
            tl->y - myheight > 8 &&
            target != NULL &&
            tl->y - myheight < 50
            )
          )
          { 
          recheight2 = target->tl->y - 8 * target->thrust * SIN(target->gamma); 
          }
      else if (manoeverstate!=100)
          {
          
          float flyx2 = tl->x + forcex * lsdist * 3, flyz2 = tl->z + forcez * lsdist * 3;
          float flyx3 = tl->x + forcex * lsdist * 8, flyz3 = tl->z + forcez * lsdist * 8;
          float h1 = l->getMaxHeight (flyx, flyz);
          float h2 = l->getMaxHeight (flyx2, flyz2);
          float h3 = l->getMaxHeight (flyx3, flyz3);
          h1 = h1 > h2 ? h1 : h2;
          h1 = h1 > h3 ? h1 : h3;
          recheight2 = recheight + h1;
          }
      }
    // fire flares and chaff
    if (id >= FIGHTER1 && id <= FIGHTER2 && manoeverstate!=100)   // for fighters do the following
        { 
        
        if (manoevertheta <= 0)
            for (i = 0; i < maxmissile; i ++)
                if (m [i]->ttl > 0)
                    if (m [i]->target == this)
                        {
                        if (m [i]->id >= 0 && m [i]->id <= MISSILE_AIR3)
                            {
                            if (fplayer->elevatoreffect >= 0.5)
                                {
                                fireFlare (flare, m);
                                fireflarettl += intelligence / 20 * timestep;
                                }
                            manoevertheta = 35 * timestep;
                            }
                        else
                            {
                            if (fplayer->elevatoreffect >= 0.5)
                                {
                                fireChaff (chaff, m);
                                firechaffttl += intelligence / 20 * timestep;
                                }
                            manoevertheta = 35 * timestep;
                            }
                        }
        }
    // manoevers (may use the height information)
    if (tl->y > 200)
       { 
       manoeverstate = 11; 
       }
    if (manoeverstate && active && draw)
        {
        if (manoeverstate == 0)
           {
           if (fabs (tl->y) < 45) 
              { 
              manoeverstate = 1;
              }
           }
        if (manoeverstate == 1)   
            {
            recelevatoreffect = 0.05; 
            
            if (fabs (theta) > 10)
                {
                recrolleffect = -1;
                }
            else
                {
                recrolleffect = 0;
                manoeverstate = 2; 
                }
            }
        if (manoeverstate == 2) 
            {
            if (fabs (theta) <= 150)
                { 
                recrolleffect = 0;
                recelevatoreffect = 1.0; 
                }
            else
                {
                manoeverstate = 3; 
                }
            }
        if (manoeverstate == 3) 
            {
            if (gamma < 170 || gamma > 190)
                {
                recrolleffect = 0;
                recelevatoreffect = 1.0; 
                }
            else
                {
                manoeverstate = 4;
                }
            }
        if (manoeverstate == 4) 
            {
            if (fabs (theta) > 20)
                { 
                recrolleffect = 1;
                recelevatoreffect = 0.05; 
                }
            else
                {
                manoeverstate = 10;
                }
            }
        if (manoeverstate == 10)   // Gradual climb vertical
            {
            recrolleffect = 0;
            recelevatoreffect = .10; 
            if (gamma > 260 || gamma < 90)
                {
                manoeverstate = 11;
                }
            }
        if (manoeverstate == 11) 
            {
            recrolleffect = 0;
            recelevatoreffect = 0;
            if (fabs (tl->y) > 100)
               {
               manoeverstate = 12;
               }
            else
               {
               manoeverstate = 10; 
               }
            }
        if (manoeverstate == 12) 
            {
            if (tl->y > 190)
               { 
               recelevatoreffect = -1.0; 
               }
            recelevatoreffect = -0.4; 
            if (gamma > 170 && gamma < 190)
                { 
                recelevatoreffect = -0.15;
                }
            if (fabs (theta) > 30)
                { 
                recrolleffect = 1;
                }
            if (fabs (tl->y - myheight) < 30)
                {
                manoeverstate = 20;
                }
            }
        if (manoeverstate == 20)   // Roll
            {
            recelevatoreffect = 0.55;
            recrolleffect = 1;
            if (theta > 80 && theta < 90)
                {
                manoeverstate = 21;
                }
            }
        if (manoeverstate == 21)
            {
            if (theta > -10 && theta < 10)
                {
                recelevatoreffect = 0.05;
                rectheta = 0.0;
                manoeverstate = 0;
                }
            }
        float pulljoystick = 0.005;
        float nocorrection = 0.1;
        if ( manoeverstate != 100)
           {
           if (recrolleffect > rolleffect + nocorrection)
             {
             rolleffect += pulljoystick * timestep;
             }
           else if (recrolleffect < rolleffect - nocorrection)
             {
             rolleffect -= pulljoystick * timestep;
             }
           if (recelevatoreffect > elevatoreffect + nocorrection)
               {
               elevatoreffect += pulljoystick * timestep;
               }
           else if (recelevatoreffect < elevatoreffect - nocorrection)
               {
               elevatoreffect -= pulljoystick * timestep;
               }
           }
        return;
        }
    if ( manoeverstate != 100)
       {
       // calculate the recommended height, recheight2 depends on it
       if (manoeverheight > 0)
           {
           manoeverheight -= dt;
           }
       if (manoeverheight <= 0)
           {
           if (!(id >= FIGHTER1 && id <= FIGHTER2) && target != NULL)   // no fighter, has target (missile, mine)
               {
               recheight = target->tl->y - targetheight;
               }
           else if (id == BOMBER_B17)     // transporters have to stay higher
               {
               recheight = 40;
               manoeverheight = 1;
               }
           else if (id >= FIGHTER1 && id <= FIGHTER2 && target != NULL)     // fighter, has target
               {
               if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                   {
                   recheight = target->tl->y - targetheight;    // target is a fighter
                   }
               else
                   {
                   recheight = target->tl->y - targetheight + 5;    // target is no fighter
                   }
               if (!l->isWater (l->f [flyxs] [flyzs]))   // not flying above water
                   {
                   if (recheight < 3.5 + 0.01 * aggressivity)
                       {
                       recheight = 3.5 + 0.01 * aggressivity;
                       }
                   }
               float minh = 5.5 + 0.01 * aggressivity; // minimum height
               if (l->type == LAND_CANYON)
                   {
                   minh = 6.5 + 0.01 * aggressivity;    // stay higher in canyons
                   }
               if (fabs (tl->y - myheight) < minh)
                   {
                   recheight = 9 + 0.015 * aggressivity;
                   if (fabs (tl->y - myheight) < minh)
                       {
                       manoeverstate = 1;
                       }
                   else
                       {
                       manoeverheight = 5 * timestep; // fly manoever to gain height
                       }
                   }
               if (disttarget < 50 && fabs (tl->y - myheight) > 25)
                   {
                   recheight = 8 + 0.025 * aggressivity;
                   manoeverheight = 12 * timestep;
                   }
               }
           }
        }
    if (ttl <= 0 && id >= MISSILE1 && id <= MISSILE2 && id != MISSILE_MINE1)
        {
        recheight = -100;
        recheight2 = -100;
        recgamma = 90;
        }
    else if (ai)
        {
        if (target != NULL && ((id >= MISSILE1 && id <= MISSILE2) || (id >= FIGHTER1 && id <= FIGHTER2 && manoeverheight <= 0)))   // is AGM
            {
            float dgamma = 0;

            if (disttarget <= -0.00001 || disttarget >= 0.00001)   // no division by zero
                {
                dgamma = atan ((target->tl->y - tl->y) / disttarget) * 180 / M_PI - (gamma - 180);
                }
            recgamma = gamma + dgamma; // get recommended elevation to target
            }
        else
            {
            recgamma = (int) ((recheight2 - tl->y) * 10 - gamma + 360);
            }
        }
    // do a smooth roll
    float deltatheta;
    deltatheta = rectheta - theta;
    if (fabs (dtheta) > 30)
        {
        dtheta = 0;
        }
    float myRollRate = fabs (deltatheta) / 5.0F * RollRate;
    if (myRollRate > RollRate)
        {
        myRollRate = RollRate;
        }
    float RollRate2 = myRollRate;
    if (RollRate2 >= -0.00001 && RollRate2 <= 0.00001)
        {
        RollRate2 = 0.00001;
        }
    if (deltatheta > 0 && dtheta < 0)
        {
        dtheta += myRollRate * timefac;
        }
    else if (deltatheta < 0 && dtheta > 0)
        {
        dtheta -= myRollRate * timefac;
        }
    else if (deltatheta > 0)
        {
        float estimatedtheta = dtheta * (dtheta + RollRate2 * 5 / timefac) / 2 / RollRate2;
        if (deltatheta > estimatedtheta)
            {
            dtheta += myRollRate * timefac;
            }
        else if (deltatheta < estimatedtheta)
            {
            dtheta -= myRollRate * timefac;
            }
        }
    else
        {
        float estimatedtheta = -dtheta * (dtheta - RollRate2 * 5 / timefac) / 2 / RollRate2;
        if (deltatheta < estimatedtheta)
            {
            dtheta -= myRollRate * timefac;
            }
        else if (deltatheta > estimatedtheta)
            {
            dtheta += myRollRate * timefac;
            }
        }
    if (dtheta > (RollRate * (1.0 + realspeed)) * timefac * 5.0F)
        {
        dtheta = (RollRate * (1.0 + realspeed)) * timefac * 5.0F;
        }
    theta += dtheta;
    // height changes
    float RollRate1 = RollRate / 5;
    if (RollRate1 >= -0.00001 && RollRate1 <= 0.00001)
        {
        RollRate1 = 0.00001;
        }
    if (theta > maxtheta)
        {
        theta = maxtheta;    // restrict roll angle
        }
    else if (theta < -maxtheta)
        {
        theta = -maxtheta;
        }
    float deltagamma = recgamma - gamma;
    if (deltagamma > 0 && dgamma < 0)
        {
        dgamma += RollRate1 * timefac;
        }
    else if (deltagamma < 0 && dgamma > 0)
        {
        dgamma -= RollRate1 * timefac;
        }
    else if (deltagamma > 0)
        {
        float estimatedgamma = dgamma * (dgamma + RollRate1 * 2) / RollRate1;
        if (id == 200)
            {
            id = id;
            }
        if (deltagamma > estimatedgamma + 2)
            {
            dgamma += RollRate1 * timefac;
            }
        else if (deltagamma < estimatedgamma - 2)
            {
            dgamma -= RollRate1 * timefac;
            }
        }
    else if (deltagamma < 0)
        {
        float estimatedgamma = -dgamma * (dgamma + RollRate1 * 2) / RollRate1;
        if (id == 200)
            {
            id = id;
            }
        if (deltagamma < estimatedgamma - 2)
            {
            dgamma -= RollRate1 * timefac;
            }
        else if (deltagamma > estimatedgamma + 2)
            {
            dgamma += RollRate1 * timefac;
            }
        }
    if (dgamma > manoeverability * (3.33 + 15.0 * realspeed) * timefac)
        {
        dgamma = manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        }
    gamma += dgamma;
    if (gamma > 180 + maxgamma)
        {
        gamma = 180 + maxgamma;
        }
    else if (gamma < 180 - maxgamma)
        {
        gamma = 180 - maxgamma;
        }
    if (id >= MISSILE1 && id <= MISSILE2)
        {
        if (target == NULL)
            {
            ttl = 0;
            return;
            }
        else if (target->active == false)
            {
            ttl = 0;
            return;
            }
        }
    if (target == NULL)
        {
        return;
        }
    // fighter's targeting mechanism for missiles
    if (id >= FIGHTER1 && id <= FIGHTER2)   // for fighters do the following
        {
        if (ai)
            {
            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                {
                if (!selectMissileAirFF (m))
                    {
                    selectMissileAir (m);
                    }
                }
            else
                {
                selectMissileGround (m);
                }
            }
        }
    if (!ai || target == NULL)   
        {
        return;
        }
    int firerate;
    firerate = 3;
    float dx2, dz2, ex, ez;
    float dx = target->tl->x - tl->x, dz = target->tl->z - tl->z; // current distances
    if ((id >= FIGHTER1 && id <= FIGHTER2) || (id >= MISSILE1 && id <= MISSILE2) || (id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
        {
        float t = 10.0 * disttarget; // generous time to new position
        if (t > 60)
            {
            t = 60;    // higher values will not make sense
            }
        t *= (float) (400 - precision) / 400;
        int tt = (int) target->theta;
        if (tt < 0)
            {
            tt += 360;
            }
        float newphi = t * SIN(tt) * 5.0 * target->manoeverability; // new angle of target after time t
        if (newphi > 90)
            {
            newphi = 90;
            }
        else if (newphi < -90)
            {
            newphi = -90;
            }
        newphi += (float) target->phi;
        if (newphi >= 360)
            {
            newphi -= 360;
            }
        if (newphi < 0)
            {
            newphi += 360;
            }
        if ((id >= FIGHTER1 && id <= FIGHTER2) || (id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
            {
            ex = target->tl->x - SIN(newphi) * t * target->realspeed * 0.25; // estimated target position x
            ez = target->tl->z - COS(newphi) * t * target->realspeed * 0.25; // estimated target position z
            }
        else
            {
            ex = target->tl->x - SIN(newphi) * t * target->realspeed * 0.05; // estimated target position x
            ez = target->tl->z - COS(newphi) * t * target->realspeed * 0.05; // estimated target position z
            }
        dx2 = ex - tl->x;
        dz2 = ez - tl->z; // estimated distances
        }
    else
        {
        dx2 = dx;
        dz2 = dz;
        }
    float a, w = phi;
    if (dz2 > -0.0001 && dz2 < 0.0001)
        {
        dz2 = 0.0001;
        }
    // get heading to target
    a = atan (dx2 / dz2) * 180 / M_PI;
    if (dz2 > 0)
        {
        if (dx2 > 0)
            {
            a -= 180.0F;
            }
        else
            {
            a += 180.0F;
            }
        }
    aw = a - w; // aw=0: target in front, aw=+/-180: target at back
    if (aw < -180)
        {
        aw += 360;
        }
    if (aw > 180)
        {
        aw -= 360;
        }
    if (manoevertheta > 0)
        {
        manoevertheta -= dt;
        }
    if (manoeverthrust > 0)
        {
        manoeverthrust -= dt;
        }
    // heading calculations
    
    if (id >= FIGHTER1 && id < FIGHTER2)   // for aircraft do the following
        {
        if (!acttype && disttarget <= 1000 && manoevertheta <= 0)   // no special action, near distance, no roll manoever
            {
            if (aw > 0)   // positive angle
                {
                if (aw > 140 && disttarget > 50)
                    {
                    manoeverstate = 1;// Immelman
                    }
                else if (aw > 160.0F + 0.05 * intelligence && disttarget < 8 + 0.01 * intelligence)     // target very near at the back
                    { // Immelman
                    manoeverstate = 1;
                    }
                else if (aw > 160 && disttarget < 25)     // target is at the back
                    {
                    if (fabs (tl->y - myheight) > 7 && gamma >= 175 + intelligence / 100)   // high enough over ground
                        { // roll
                        manoeverstate = 20;
                        }
                    else
                        {
                        rectheta = -90;
                        if (manoevertheta <= 0)
                            { // Turn
                            manoevertheta = timestep * (100 + myrandom ((400 - intelligence) / 8)); // turn hard left or right
                            }
                        if (manoeverthrust <= 0)
                            {
                            recthrust = maxthrust / (1.05F + (float) intelligence * 0.0015);    // fly faster
                            }
                        if (intelligence < 280 && manoeverheight <= 0)
                            { // Height change
                            recheight = 5;
                            manoeverheight = timestep * (20 - intelligence / 50);
                            } // stay low
                        }
                    }
                else if (aw < 40 && disttarget > 60)
                    {
                    rectheta = 00;
                    }
                else if (aw < 20 && disttarget > 30)
                    {
                    rectheta = 00;
                    }
                else     // otherwise fly to target direction
                    {
                    int maw = aw > 90 ? 90 : (int) aw;
                    int maw2 = 90 - maw;
                    rectheta = 90 - maw2 * intelligence / 400;
                    if (maw < 30)
                        {
                        rectheta /= 2;
                        }
                    if (maw < 5)
                        {
                        rectheta = 0;
                        if (target->id >= FIGHTER1 && target->id <= FIGHTER2 && disttarget < 20)
                            {
                            ((AIObj *) target)->manoevertheta = timestep * (50 - intelligence / 10);
                            }
                        }
                    }
                }
            else     // same for negative angle
                {
                if (aw < -140 && disttarget > 50)
                    { // Immelmann
                    manoeverstate = 1;
                    }
                else if (aw < -160.0F - 0.05 * intelligence && disttarget < 4 + 0.01 * intelligence)     // target very near at the back
                    { // Immelmann
                    manoeverstate = 1;
                    }
                else if (aw < -160 && disttarget < 25)
                    {
                    if (fabs (tl->y - myheight) > 7 && gamma >= 175 + intelligence / 100)   // high enough over ground
                        { // roll
                        manoeverstate = 20; // roll
                        }
                    else
                        {
                        rectheta = 90;
                        if (manoevertheta <= 0)
                            { // Turn
                            manoevertheta = timestep * (100 + myrandom ((400 - intelligence) / 8));
                            }
                        if (manoeverthrust <= 0)
                            {
                            recthrust = maxthrust / (1.05F + (float) intelligence * 0.0015);
                            }
                        if (intelligence < 280 && manoeverheight <= 0)
                            { // Height change
                            recheight = 5;
                            manoeverheight = timestep * (20 - intelligence / 50);
                            }
                        }
                    }
                else if (aw > -40 && disttarget > 60)
                    {
                    rectheta = 0;
                    }
                else if (aw > -20 && disttarget > 30)
                    {
                    rectheta = 0;
                    }
                else
                    {
                    int maw = aw < -90 ? -90 : (int) aw;
                    int maw2 = -90 - maw;
                    rectheta = -90 - maw2 * intelligence / 400;
                    if (maw > -30)
                        {
                        rectheta /= 2;
                        }
                    if (maw > -5)
                        {
                        rectheta = 0;
                        if (target->id >= FIGHTER1 && target->id <= FIGHTER2 && disttarget < 20)
                            {
                            ((AIObj *) target)->manoevertheta = timestep * (50 - intelligence / 10);
                            }
                        }
                    }
                }
            }
        }
    else if (id >= MISSILE1 && id <= MISSILE2)     // for missiles do the following
        {
        if (fabs (aw) < 50 && disttarget > 50)   // target in front and minimum distance, then no roll
            {
            rectheta = 0;
            }
        else     // otherwise chase target
            {
            if (aw < -90 || aw > 90)
                {
                rectheta = 0;
                }
            else if (aw > 0)
                {
                rectheta = aw > 90 ? 90 : aw;
                }
            else
                {
                rectheta = aw < -90 ? -90 : aw;
                }
            }
        }
    else if (id >= FLAK1 && id <= FLAK2)     // ground-air-cannon
        {
        recthrust = 0;
        thrust = 0;
        if (aw > 5)
            {
            rectheta = maxtheta;
            }
        else if (aw < -5)
            {
            rectheta = -maxtheta;
            }
        else
            {
            rectheta = 0;
            }
        }
    else if (id >= TANK1 && id <= TANK2)     // tanks
        {
        recthrust = maxthrust;
        thrust = maxthrust; // always at maximum thrust
        if (aw > 5)
            {
            rectheta = maxtheta;
            }
        else if (aw < -5)
            {
            rectheta = -maxtheta;
            }
        else
            {
            rectheta = 0;
            }
        if (firecannonttl <= 0)
            {
            if (id == TANK_AIR1)
                if (fabs (rectheta - theta) < 2 && fabs (aw) < 20 && disttarget < 40 && target->tl->y > tl->y + 2)
                    {
                    fireCannon (c);
                    firecannonttl += firerate * timestep;
                    }
            if (id == TANK_GROUND1)
                if (fabs (rectheta - theta) < 2 && fabs (aw) < 20 && disttarget < 35 && target->tl->y <= tl->y + 1 && target->tl->y >= tl->y - 1)
                    {
                    fireCannon (c);
                    firecannonttl += firerate * timestep;
                    }
            }
        }
    // thrust and manoever calculations
    if (id >= FIGHTER1 && id <= FIGHTER2)   // fighters
        {
        if (disttarget > 5 + aggressivity / 12)   // 2.5 seems to be best, but fighters become far too strong
            {
            if (disttarget < 50 && fabs (aw) > 30 && manoeverthrust <= 0)
                {
                recthrust = maxthrust / (1.0F + (float) intelligence * 0.0025);
                }
            else
                {
                thrustUp ();    // otherwise fly faster
                }
            }
        else if (manoeverthrust <= 0)
            {
            if (recthrust > target->thrust)   // adopt thrust of target
                {
                thrustDown ();
                }
            else
                {
                thrustUp ();
                }
            }
        if (disttarget > 50 && fabs (aw) < 20)   // high distance and target in front, then fly straight
            {
            rectheta = 0;
            }
        if (manoeverthrust <= 0)
            if (disttarget < 25 && fabs (aw) > 160 && target->id >= TANK1)   // avoid collisions
                {
                manoeverthrust = 25 * timestep;
                recthrust = maxthrust;
                manoevertheta = 25 * timestep;
                rectheta = 0;
                manoeverheight = 25 * timestep;
                recheight = 10;
                }
        // fire cannon?
        float agr = 4.0 - (float) aggressivity / 100;
        if (firecannonttl <= 0)
            {
            if (fabs (rectheta - theta) < agr && fabs (aw) < 20 + agr * 4 && disttarget < 30)
                {
                fireCannon (c);
                }
            else if (disttarget < 2 + agr && fabs (aw) < 20 + agr * 4)
                {
                fireCannon (c);
                }
            }
        // fire missile?
        if (firemissilettl <= 0)
            {
            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                {
                int z1 = 0;

                if (disttarget < 15 && fabs (aw) < 20)
                    {
                    for (i = 0; i < maxfighter; i ++)
                        {
                        if (target == f [i]->target && party == f [i]->party)
                            {
                            z1 ++;
                            }
                        }
                    if (z1 >= 3)
                        {
                        manoevertheta = 15 * timestep;
                        firemissilettl = 10 * timestep;
                        if (myrandom (2))
                            {
                            rectheta = 90;
                            }
                        else
                            {
                            rectheta = -90;
                            }
                        }
                    }
                if (firemissilettl <= 0)
                    {
                    if (fabs (rectheta - theta) < agr / 2 && fabs (aw) < agr && disttarget < 45)
                        {
                        fireMissile (m, (AIObj *) target);
                        firemissilettl += aggressivity * timestep;
                        }
                    }
                }
            else     // ground target
                {
                if (fabs (rectheta - theta) < 5 + agr * 4 && fabs (aw) < 5 + agr * 4 && disttarget < 50)
                    if (!(l->lsticker & 7))
                        {
                        fireMissileGround (m);
                        firemissilettl += aggressivity / 2 * timestep;
                        }
                }
            }
        }
    if ((id >= FLAK1 && id <= FLAK2) || id == SHIP_CRUISER || id == SHIP_DESTROYER1 || id == TANK_TRSAM1)
        {
        if (firecannonttl <= 0)
            for (int i = 0; i < maxfighter; i ++)
                if (f [i]->active)
                    if (party != f [i]->party)
                        {
                        disttarget = distance (f [i]); // distance to target
                        ex = f [i]->tl->x; // estimated target position x
                        ez = f [i]->tl->z; // estimated target position z
                        dx2 = ex - tl->x;
                        dz2 = ez - tl->z; // estimated distances
                        w = (int) phi;
                        if (dz2 > -0.0001 && dz2 < 0.0001)
                            {
                            dz2 = 0.0001;
                            }
                        a = (atan (dx2 / dz2) * 180 / M_PI);
                        if (dz2 > 0)
                            {
                            if (dx2 > 0)
                                {
                                a -= 180;
                                }
                            else
                                {
                                a += 180;
                                }
                            }
                        aw = a - w;
                        if (aw < -180)
                            {
                            aw += 360;
                            }

                        if (aw > 180)
                            {
                            aw -= 360;
                            }
                        if (id == FLAK_AIR1)
                            if (f [i]->tl->y > tl->y + 2)
                                {
                                if (fabs (aw) <= 20 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                firecannonttl = firerate * timestep;
                                }
                        if (id == SHIP_DESTROYER1)
                            if (f [i]->tl->y > tl->y + 2)
                                {
                                if (aw >= 0 && aw < 40 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }
                                if (aw >= -40 && aw < 0 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }
                                if (aw >= 120 && aw < 160 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }
                                if (aw >= -160 && aw < -120 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }
                                firecannonttl = firerate * timestep;
                                }
                        if (firemissilettl <= 0)
                            if (id == FLARAK_AIR1)
                                if (fabs (aw) < 25 && disttarget < 45) // + aggressive
                                    if (f [i]->tl->y > tl->y + 2)
                                        {
                                        ttf = 0;
                                        fireMissileAirFF (m, f [i]);
                                        firemissilettl += (20 + firerate * 10) * timestep;
                                        }
                        if (id == TANK_TRSAM1)
                            {
                            if (firemissilettl <= 0)
                                if (aw >= -30 && aw < 30 && disttarget < 60)   // + aggressive
                                    {
                                    ttf = 0;
                                    fireMissileAirFF (m, f [i]);
                                    firemissilettl += aggressivity / 5 * timestep;
                                    missiles [6] ++; // unlimited ammo
                                    }
                            }
                        if (id == SHIP_CRUISER)
                            {
                            if (firemissilettl <= 0)
                                if (aw >= -30 && aw < 30 && disttarget < 60)   // + aggressive
                                    {
                                    ttf = 0;
                                    fireMissileAirFF (m, f [i]);
                                    firemissilettl += aggressivity / 5 * timestep;
                                    missiles [6] ++; // unlimited ammo
                                    }
                            if (firecannonttl <= 0)
                                {
                                fireCannon (c, phi + aw);
                                }
                            }
                        }
        }
    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        if (rectheta > 90 - precision / 5)
            {
            rectheta = 90 - precision / 5;
            }
        else if (rectheta < -90 + precision / 5)
            {
            rectheta = -90 + precision / 5;
            }
        }
    } 

void AIObj::aiinit ()
    {
    int i;
    acttype = 0;
    dualshot = false;
    intelligence = 100;
    aggressivity = 100;
    precision = 100;
    Durability = 0.01F;
    ai = true;
    active = true;
    draw = true;
    target = NULL;
    dtheta = 0;
    dgamma = 0;
    id = MISSILE1;
    impact = 30;
    manoevertheta = 0;
    manoeverheight = 0;
    manoeverthrust = 0;
    idle = 0;
    smokettl = 0;
    firecannonttl = 0;
    firemissilettl = 0;
    fireflarettl = 0;
    firechaffttl = 0;
    flares = 0;
    aw = 0;
    source = NULL;
    points = 0;
    elevatoreffect = 0;
    ruddereffect = 0;
    gamma = 180;
    recgamma = 180;
    dgamma = 0;
    theta = 0;
    maxgamma = 70;
    maxtheta = 90;
    missiletype = 0;
    autofire = false;
    ttl = -1;
    ttf = 30 * timestep;
    for (i = 0; i < missiletypes; i ++)
        {
        missiles [i] = 0;
        }
    for (i = 0; i < missileracks; i ++)
        {
        missilerack [i] = -1;
        missilerackn [i] = 0;
        }
    bomber = 0;
    timer = 0;
    ammo = -1;
    manoeverstate = 11;
    rectheta = 90;
    } 

AIObj::AIObj ()
    {
    o = NULL;
    zoom = 1.0;
    aiinit ();
    smoke = new CSmoke (0);
    }

AIObj::~AIObj ()
    {
    delete smoke;
    }

AIObj::AIObj (Space *space2, CModel *o2, float zoom2)
    {
    this->space = space2;
    o = o2;
    zoom = zoom2;
    aiinit ();
    smoke = new CSmoke (0);
    space->addObject (this);
    }

void AIObj::decreaseMissile (int id)
    {
    char buf [STDSIZE];
    int i;
    id -= MISSILE1;
    if (id < 0 || id >= missiletypes)
        {
        sprintf (buf, "Wrong missile ID in %s, line %d", __FILE__, __LINE__);
        display (buf, LOG_ERROR);
        }
    missiles [id] --;
    int ptrrack = 0, maxrack = 0;
    for (i = 0; i < missileracks; i ++)
        if (missilerack [i] == id)
            if (missilerackn [i] > maxrack)
                {
                ptrrack = i;
                maxrack = missilerackn [i];
                }
    if (maxrack > 0)
        {
        missilerackn [ptrrack] --;
        refscale [ptrrack * 3 + 2 - missilerackn [ptrrack]] = 0;
        }
    }

void AIObj::fireCannon (DynamicObj *MachineGunBullet, float phi)
    {
    if (firecannonttl > 0)
        {
        return;
        }

    if (ammo == 0)
        {
        return;
        }
    ammo --;
    MachineGunBullet->thrust = 0;
    MachineGunBullet->recthrust = MachineGunBullet->thrust;
    MachineGunBullet->manoeverability = 0.0;
    MachineGunBullet->maxthrust = 0;
    if (target != NULL && ai)
        {
        if (target->active)
            {
            // exact calculation to hit enemy (non-static turret!)
            if (id >= FIGHTER1 && id <= FIGHTER2)
                {
                MachineGunBullet->gamma = gamma;
                }
            else
                {
                MachineGunBullet->gamma = 180.0 + atan ((target->tl->y - tl->y) / distance (target)) * 180.0 / M_PI;
                }
            }
        }
    else
        {
        MachineGunBullet->gamma = gamma;    
        }
    MachineGunBullet->party = party;
    MachineGunBullet->ttl = 80 * timestep;
    MachineGunBullet->Durability = 1;
    MachineGunBullet->immunity = (int) (zoom * 3) * timestep; 
    MachineGunBullet->source = this;
    MachineGunBullet->phi = phi;
    MachineGunBullet->theta = theta;
    initValues (MachineGunBullet, phi);
    float fac = 0.7F;
    MachineGunBullet->forcex += COS(MachineGunBullet->gamma) * SIN(MachineGunBullet->phi) * fac;
    MachineGunBullet->forcey -= SIN(MachineGunBullet->gamma) * fac;
    MachineGunBullet->forcez += COS(MachineGunBullet->gamma) * COS(MachineGunBullet->phi) * fac;
    MachineGunBullet->activate ();
    firecannonttl += 45;
    if (day)
        {
        if (dualshot)
            {
            MachineGunBullet->o = &model_cannon1b;
            }
        else
            {
            MachineGunBullet->o = &model_cannon1;
            }
        }
    else
        {
        if (dualshot)
            {
            MachineGunBullet->o = &model_cannon2b;
            }
        else
            {
            MachineGunBullet->o = &model_cannon2;
            }
        }
    }

void AIObj::fireCannon (DynamicObj **MachineGunBullet, float phi)
    {
    int i;
    if (firecannonttl > 0)
        {
        return;
        }
    if (ammo == 0)
        {
        return;
        }
    for (i = 0; i < maxMachineGunBullet; i ++)
        {
        if (!MachineGunBullet [i]->active)
            {
            break;
            }
        }
    if (i < maxMachineGunBullet)
        {
        fireCannon (MachineGunBullet [i], phi);
        }
    }

void AIObj::fireCannon (DynamicObj **MachineGunBullet)
    {
    if (firecannonttl > 0)
        {
        return;
        }
    if (ammo == 0)
        {
        return;
        }
    fireCannon (MachineGunBullet, phi);
    }

bool AIObj::fireChaff (DynamicObj **chaff, AIObj **missile)
    {
    int i, i2;
    if (chaffs <= 0)
        {
        return false;
        }
    if (firechaffttl > 0)
        {
        return false;
        }
    for (i = 0; i < maxchaff; i ++)
        {
        if (chaff [i]->ttl <= 0)
            {
            break;
            }
        }
    if (i < maxchaff)
        {
        fireChaff2 (chaff [i]);
        chaffs --;
        firechaffttl = 8 * timestep;
        for (i2 = 0; i2 < maxmissile; i2 ++)
            {
            if (missile [i2]->ttl > 0)
                {
                if (missile [i2]->id > MISSILE_AIR3) // only radar seeking missiles
                    if (missile [i2]->target == this)   // only change target if angle is good
                        {
                        bool hit = false;
                        if (myrandom ((int) (fabs (elevatoreffect) * 90 + 20)) > 50)
                            {
                            hit = true;
                            }
                        if (hit)
                            {
                            if (DebugLevel == LOG_ALL)
                                {
                                display ((char*)"AIObj::fireChaff() Missile to chaff", LOG_ALL);
                                }

                            missile [i2]->target = chaff [i];
                            }
                        }
                }
            }
        return true;
        }
    return false;
    }

void AIObj::fireChaff2 (DynamicObj *chaff)
    {
    char buf [STDSIZE];
    if (debug == LOG_ALL)
        {
        sprintf (buf, "Chaff: party=%d", party);
        display (buf, LOG_ALL);
        }
    chaff->dinit ();
    chaff->thrust = 0;
    chaff->realspeed = 0;
    chaff->recthrust = 0;
    chaff->manoeverability = 0.0;
    chaff->maxthrust = 1.0;
    chaff->gamma = 0;
    chaff->party = party;
    chaff->ttl = 80 * timestep;
    chaff->Durability = 1;
    chaff->immunity = (int) (zoom * 12) * timestep;
    chaff->source = this;
    chaff->phi = phi;
    chaff->id = CHAFF1;
    initValues (chaff, phi);
    chaff->activate ();
    chaff->explode = 0;
    chaff->zoom = 0.12F;
    } 

bool AIObj::fireFlare (DynamicObj **flare, AIObj **missile)
    {
    int i, i2;
    if (flares <= 0)
        {
        return false;
        }
    if (fireflarettl > 0)
        {
        return false;
        }
    for (i = 0; i < maxflare; i ++)
        {
        if (flare [i]->ttl <= 0)
            {
            break;
            }
        }
    if (i < maxflare)
        {
        fireFlare2 (flare [i]);
        flares --;
        fireflarettl = 8 * timestep;
        for (i2 = 0; i2 < maxmissile; i2 ++)
            {
            if (missile [i2]->ttl > 0)
                {
                if (missile [i2]->id >= MISSILE_AIR2 && missile [i2]->id <= MISSILE_AIR3) // only heat seeking missiles
                    if (missile [i2]->target == this)   // only change target if angle is good
                        {
                        bool hit = false;
                        if (myrandom ((int) (fabs (elevatoreffect) * 90 + 20)) > 50)
                            {
                            hit = true;
                            }
                        if (hit)
                            {
                            if (DebugLevel == LOG_ALL)
                                {
                                display ((char*)"AIObj::fireFlare() Missile to flare", LOG_ALL);
                                }
                            missile [i2]->target = flare [i];
                            }
                        }
                }
            }
        return true;
        }
    return false;
    }

void AIObj::fireFlare2 (DynamicObj *flare)
    {
    char buf [STDSIZE];
    if (DebugLevel == LOG_ALL)
        {
        sprintf (buf, "Flare: party=%d", party);
        display (buf, DebugLevel);
        }
    flare->dinit ();
    flare->thrust = 0;
    flare->realspeed = 0;
    flare->recthrust = 0;
    flare->manoeverability = 0.0;
    flare->maxthrust = 1.0;
    flare->gamma = 0;
    flare->party = party;
    flare->ttl = 80 * timestep;
    flare->Durability = 1;
    flare->immunity = (int) (zoom * 12) * timestep;
    flare->source = this;
    flare->phi = phi;
    flare->id = FLARE1;
    initValues (flare, phi);
    flare->activate ();
    flare->explode = 0;
    }

bool AIObj::fireMissile (int id, AIObj **missile, AIObj *target)
    {
    int i;
    if (!haveMissile (id))
        {
        return false;
        }
    if (ttf > 0)
        {
        return false;
        }
    for (i = 0; i < maxmissile; i ++)
        {
        if (missile [i]->ttl <= 0)
            {
            break;
            }
        }
    if (i < maxmissile)
        {
        fireMissile2 (id, missile [i], target);
        decreaseMissile (id);
        firemissilettl = 20 * timestep;
        return true;
        }
    return false;
    }

bool AIObj::fireMissile (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }
    return fireMissile (missiletype + MISSILE1, missile, (AIObj *) target);
    }

bool AIObj::fireMissile (int id, AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }
    return fireMissile (id, missile, (AIObj *) target);
    }

bool AIObj::fireMissile (AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }
    return fireMissile (missiletype + MISSILE1, missile);
    }
void AIObj::fireMissile2 (int id, AIObj *missile, AIObj *target)
    { // Get here when pilot launches an air-to-ground rocket
    sprintf (DebugBuf, "Rocket: party=%d, id=%d", party, id);
    display (DebugBuf, LOG_MOST);
    ttf = 50 * timestep;
    missile->dinit ();
    missile->aiinit ();
    missile->newinit (id, party, 0);
    initValues (missile, phi);
    missile->id = id;
    missile->explode = 0;
    missile->thrust = thrust + 0.001;
    missile->recthrust = missile->maxthrust;
    missile->gamma = gamma;
    missile->target = target;
    missile->recgamma = gamma;
    missile->Durability = 1;
    missile->party = party;
    missile->immunity = (45 + (int) (zoom * 6.0)) * timestep;
    missile->dtheta = 0;
    missile->dgamma = 0;
    missile->source = this;
    if (missile->id == MISSILE_DF1)
       { 
       sprintf (DebugBuf, "AiObj::FireMissile2() gamma = %f", gamma);
       display (DebugBuf, LOG_MOST);
       display ((char*)"AIObj::fireMissile2() dropped MISSILE_DF1", LOG_MOST);
       missile->gamma += (((gamma-180)/6) -15); 
       missile->tl->y -=0.002; // This is the "drop distance" that rockets fall down before their motors ignite.
       }
    if (missile->id == BOMB01)
       { 
       display ((char*)"AIObj::fireMissile2() dropped BOMB02", LOG_MOST);
       missile->immunity = (60 + (int) (zoom * 6.0)) * timestep; 
       missile->gamma -=10; 
       missile->recgamma -=10;
       if (missile->gamma <0)
          {
          missile->gamma = 0;
          missile->recgamma = 0;
          }
       }
    if (missile->id > BOMB01 && missile->id < FIGHTER1)
       { 
       MissileFired = true; 
       }
    display ((char*)"AIObj::fireMissile2() missile->gamma =", LOG_MOST);
    sprintf (DebugBuf, "%f", missile->gamma);
    display (DebugBuf, LOG_MOST);
    missile->activate ();
    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        missile->manoeverheight = 30 * timestep;
        missile->recheight = missile->tl->y - l->getHeight (missile->tl->x, missile->tl->z) - 4;
        }
    } 

bool AIObj::fireMissileAir (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }
    if (target->id >= MOVING_GROUND)
        {
        return false;
        }
    if (haveMissile (MISSILE_AIR3))
        {
        return fireMissile (MISSILE_AIR3, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_AIR2))
        {
        return fireMissile (MISSILE_AIR2, missile, (AIObj *) target);
        }
    else if (haveMissile (BOMB01))
        {
        return fireMissile (BOMB01, missile, (AIObj *) target);
        }
    return false;
    }

bool AIObj::fireMissileAirFF (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }
    if (target->id >= MOVING_GROUND)
        {
        return false;
        }
    if (haveMissile (MISSILE_FF2))
        {
        return fireMissile (MISSILE_FF2, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_FF1))
        {
        return fireMissile (MISSILE_FF1, missile, (AIObj *) target);
        }
    return false;
    }

bool AIObj::fireMissileGround (AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }
    if (target->id < MOVING_GROUND)
        {
        return false;
        }
    if (haveMissile (MISSILE_GROUND2))
        {
        return fireMissile (MISSILE_GROUND2, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_GROUND1))
        {
        return fireMissile (MISSILE_GROUND1, missile, (AIObj *) target);
        }
    return false;
    }

int AIObj::firstMissile ()
    {
    int i = 0;
    while (!missiles [i])
        {
        i ++;
        if (i >= missiletypes)
            {
            return 0;
            }
        }

    ttf = 50 * timestep;
    return i;
    }

bool AIObj::haveMissile (int id)
    {
    char buf [STDSIZE];
    id -= MISSILE1;
    if (id < 0 || id >= missiletypes)
        {
        sprintf (buf, "Wrong missile ID in %s, line %d", __FILE__, __LINE__);
        display (buf, LOG_ERROR);
        }
    if (missiles [id] > 0)
        {
        return true;
        }
    return false;
    }

bool AIObj::haveMissile () // due to missiletype
    {
    if (missiles [missiletype] > 0)
        {
        return true;
        }
    return false;
    }

void AIObj::initValues (DynamicObj *dobj, float phi)
    {
    float fac = zoom / 8;
    if (dobj->id == FLARE1 || dobj->id == CHAFF1)
        {
        fac = -fac;
        }
    // use the exact polar coordinates because of gamma and theta
    float cgamma = gamma;
    dobj->tl->x = tl->x + COS(cgamma) * SIN(phi) * fac;
    dobj->tl->y = tl->y - SIN(cgamma) * fac;
    if ((id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
        {
        dobj->tl->y += fac;
        }
    dobj->tl->z = tl->z + COS(cgamma) * COS(phi) * fac;
    dobj->phi = phi;
    dobj->rectheta = dobj->theta;
    dobj->forcex = forcex;
    dobj->forcey = forcey;
    dobj->forcez = forcez;
    dobj->rot->setAngles ((short) (90 + dobj->gamma - 180), (short) dobj->theta + 180, (short) -dobj->phi);
    }

void AIObj::missileCount ()
    {
    if (id < FIGHTER1 || id > FIGHTER2)
        {
        return;
        }
    int i;
    for (i = 0; i < missiletypes; i ++)
        {
        missiles [i] = 0;
        }
    for (i = 0; i < missileracks; i ++)
        {
        if (missilerackn [i] > 0)
            {
            missiles [missilerack [i]] += missilerackn [i];
            }
        }
    }

void AIObj::newinit (int id, int party, int intelligence, int precision, int aggressivity)
    {
    int i;
    ai = true;
    this->id = id;
    this->party = party;
    manoeverstate = 2;
    activate ();
    for (i = 0; i < missileracks; i ++)
        {
        missilerackn [i] = 0;
        }
    ammo = -1;
    bomber = 0;
    dualshot = false;
    float cubefac = 0.6F; // fighter
    float cubefac1 = 0.7F; // tanks and sams
    float missilethrustbase = 6.5F;
    o = getModel (id);
    o->cubex = zoom;
    o->cubey = zoom;
    o->cubez = zoom;
    if (id == STATIC_AIRFIELD00)
       {
       o->cubex *= 90.0;
       o->cubey *= 4.0;
       o->cubez *= 28.0; //  was 28.0
       maxDurability = 1530000;  //  Can survive 17 hits by 500 lb bombs.
       Durability = 1530000;
       }
    if (id == CANNON1)
       {
       statLethality = 2;
       impact = 2;
       }
    if (id == STATIC_RADARREFLECTOR)
       {
       statLethality = 0;
       impact = 10;
       maxDurability = 65000;
       Durability = 65000;
       o->cubex *= 6.0;
       o->cubey *= 1.0;
       o->cubez *= 5.0; //  was 8.0
       }
    if (id == FIGHTER_P38L)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.68; 
        RollRate = 0.70;
        manoeverability = 0.1685;
        maxDurability = 1020;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 28.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 100.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778; 
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=4000;
        FlapsLevelElevatorEffect4=11000;
        SpeedBrakePower=1.01;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 3.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.400; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.369565217391; 
        DiveSpeedLimit1 = 0.42; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.504; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_FIATG55)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.58; 
        RollRate = 0.65;
        manoeverability = 0.1541;
        maxDurability = 840;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 23.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.8093200;
        ammo = 1600;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.4; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.34; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.34; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.371; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.140;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_A6M2)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.48; 
        RollRate = 0.50;
        manoeverability = 0.1987;
        maxDurability = 420;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 27.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.8827200;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.51; 
        inertia = 2.0; 
        deadweight = 0.07;
        CompressibilitySpeed = 0.280; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.280; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.306324110672; //@A6M2 0.306324110672 yields 310MPH@SeaLevel.
        DiveSpeedLimit1 = 0.290; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.370; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1500;
        } 
    if (id == FIGHTER_KI43)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.75; 
        RollRate = 0.50;
        manoeverability = 0.2000;
        maxDurability = 360;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 29; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.9320360;
        ammo = 1600;
        impact = 280;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.51; 
        inertia = 1.6712; 
        deadweight = 0.07;
        CompressibilitySpeed = 0.20; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.20; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.291501976285; // Ki43: Faster than begins to regulate overspeed. 0.291501976285 ~= 295 MPH
        DiveSpeedLimit1 = 0.23; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.31; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.110;
        ServiceCeilingAltitude = 1500;
        } 
    if (id == FIGHTER_IL16)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.4; 
        RollRate = 0.50;
        manoeverability = 0.2016;
        maxDurability = 600;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 21; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3054760;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.51; 
        inertia = 2.2; 
        deadweight = 0.07;
        CompressibilitySpeed = 0.21; 
        CompressibilitySpeedWithSpeedBrakes = 0.21; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.2421; 
        DiveSpeedLimit1 = 0.24; // Faster than this induces extra turbulence. IL16: 263
        DiveSpeedStructuralLimit = 0.3063; // Faster than this will induce severe airframe damage. IL16: 310MPH
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1500;
        } 
    if (id == FIGHTER_F4F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.48; 
        RollRate = 0.55;
        manoeverability = 0.1872;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 19.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0815520;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.7; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.27; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.287549407115; 
        DiveSpeedLimit1 = 0.3010; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.40; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_F6F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.842; 
        RollRate = 0.57;
        manoeverability = 0.1857;
        maxDurability = 1080;
        zoom = 0.36;
        maxtheta = 90.0;
        maxgamma = 27; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.1130000;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.7; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.27; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.3468; 
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_F4U)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.90; 
        RollRate = 0.60;
        manoeverability = 0.1332;
        maxDurability = 1100;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.1130000;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 109.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.67;
        inertia = 2.557; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.386363636364; 
        DiveSpeedLimit1 = 0.379; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.49; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_P47D)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.84; 
        RollRate = 0.72;
        manoeverability = 0.1267;
        maxDurability = 2400;
        zoom = 0.36;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.4;
        ammo = 1600;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.685; 
        inertia = 3.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.35; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.388339920949; 
        DiveSpeedLimit1 = 0.43; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_FW190)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.6; 
        RollRate = 0.70;
        manoeverability = 0.128;
        maxDurability = 1140;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 22; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.7829126;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.67;
        inertia = 2.55; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.363636363636; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_P51D)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.67; 
        RollRate = 0.55;
        manoeverability = 0.1440;
        maxDurability = 1020;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.1130000;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.67;
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.397233201581; 
        DiveSpeedLimit1 = 0.3873; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.39; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_HAWK)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 3;
        DefensiveLethality = 0;
        maxthrust = 3.5; 
        RollRate = 0.70;
        manoeverability = 0.1440;
        maxDurability = 1020;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 28; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 6.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.5;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 3.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.56; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.56; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.6916; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.71; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.88; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.165;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == BOMBER_B17)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 1;
        DefensiveLethality = 39;
        maxthrust = 1.70; 
        RollRate = 0.28;
        manoeverability = 0.1125;
        maxDurability = 16800;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 22.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.0;
        ammo = 2450;
        impact = 5000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = .69;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.249011857708; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.35; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.110;
        ServiceCeilingAltitude = 1360;
        } 
    else if (id == BOMBER_B24)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 2;
        DefensiveLethality = 30;
        maxthrust = 1.69; 
        RollRate = 0.26;
        manoeverability = 0.1080;
        maxDurability = 15600;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 22.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.0;
        ammo = 2450;
        impact = 4000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = .69;
        inertia = 4.2; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.247035573123; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.35; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1360;
        } 
    else if (id == FIGHTER_SPIT9)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.66; 
        RollRate = 0.58;
        manoeverability = 0.1699;
        maxDurability = 840;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 27.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.1685400;
        ammo = 2450;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1700;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.35; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.370553359684; 
        DiveSpeedLimit1 = 0.3806; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.46; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_ME109G)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.69; 
        RollRate = 0.55;
        manoeverability = 0.1512;
        maxDurability = 780;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 500;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68;
        inertia = 1.8; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.28; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.28; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.353754940711; 
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_HURRICANE)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.54; 
        RollRate = 0.50;
        manoeverability = 0.1863;
        maxDurability = 900;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 22.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2400000;
        ammo = 1600;
        impact = 700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 105.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.296442687747; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_P40)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.35; 
        RollRate = 0.50;
        manoeverability = 0.1449;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.1130000;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.8; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.3162055336; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.49; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_P39)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.64; 
        RollRate = 0.55;
        manoeverability = 0.1418;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 18.000000;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 103.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.48; 
        inertia = 2.9; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.334980237154; 
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_YAK9)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.56; 
        RollRate = 0.55;
        manoeverability = 0.1282;
        maxDurability = 750;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0970872;
        ammo = 1600;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.60; 
        inertia = 2.4; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.28; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.28; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.369565217391; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    else if (id == FIGHTER_N1K1)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.9; 
        RollRate = 0.55;
        manoeverability = 0.1483;
        maxDurability = 900;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 29.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.8205800;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.26; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.26; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.363636363636; 
        DiveSpeedLimit1 = 0.375; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.46; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == BOMBER_JU87)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.44; 
        RollRate = 0.33;
        manoeverability = 0.1183;
        maxDurability = 4000;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 14; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.6000000;
        ammo = 2450;
        impact = 1300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 120.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1770;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=2.70;
        MaxFullPowerAltRatio = 0.48;
        inertia = 3.8904; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.212450592885; // Ju87: 0.212450592885=215MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.34; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.110;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_G5M)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 10;
        maxthrust = 1.56; 
        RollRate = 0.32;
        manoeverability = 0.1161;
        maxDurability = 1202;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 2500;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .160;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.17; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.17; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.22233201581; 
        DiveSpeedLimit1 = 0.20; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.34; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_B25)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 12;
        maxthrust = 1.43; 
        RollRate = 0.33;
        manoeverability = 0.1246;
        maxDurability = 4400;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 18; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 28.320000;
        ammo = 2450;
        impact = 2000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.21; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.21; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.227272727273; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.38; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1200;
        } 
    if (id == BOMBER_B26)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 2;
        DefensiveLethality = 12;
        maxthrust = 1.60; 
        RollRate = 0.35;
        manoeverability = 0.1252;
        maxDurability = 3600;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 22; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 11.410640;
        ammo = 2450;
        impact = 2000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.24; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.24; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.255928853755; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.39; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1200;
        } 
    if (id == FIGHTER_LA5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.74; 
        RollRate = 0.60;
        manoeverability = 0.1433;
        maxDurability = 1650;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 25.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.6171650;
        ammo = 2450;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .1770;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.60;
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.358695652174; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_LA7)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.77; 
        RollRate = 0.62;
        manoeverability = 0.1400;
        maxDurability = 1650;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 23.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.9220000;
        ammo = 2450;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1770;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.60;
        inertia = 2.40; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.366600790514; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.131;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_IL2)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 2;
        DefensiveLethality = 3;
        maxthrust = 1.4; 
        RollRate = 0.40;
        manoeverability = 0.125;
        maxDurability = 7000;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 1300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=2.10;
        MaxFullPowerAltRatio = 0.45;
        inertia = 2.7; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.210; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.210; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.21442687747; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.131;
        ServiceCeilingAltitude = 1280;
        } 
    if (id == FIGHTER_MACCIC202)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.57; 
        RollRate = 0.55;
        manoeverability = 0.1555;
        maxDurability = 1620;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.9009708;
        ammo = 2450;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1700;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.0;
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.328063241107; 
        DiveSpeedLimit1 = 0.33; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == BOMBER_LANCASTER)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 7;
        maxthrust = 1.74; 
        RollRate = 0.30;
        manoeverability = 0.0990;
        maxDurability = 12000;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 13; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.0;
        ammo = 2450;
        impact = 5000;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .140;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 5.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.220; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.220; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.217391304348; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1200;
        } 
    if (id == BOMBER_MOSQUITOB)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.58; 
        RollRate = 0.38;
        manoeverability = 0.1166;
        maxDurability = 2620;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 20.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.0;
        ammo = 2450;
        impact = 1700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0; 
        ClipDistance = 0.03;
        FlapSpeed = .177;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 4.1; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.34; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.34; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.370553359684; 
        DiveSpeedLimit1 = 0.41; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_TYPHOON)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.5; 
        RollRate = 0.55;
        manoeverability = 0.1404;
        maxDurability = 1620;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 18; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.4800000;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1770;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.32; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.32; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.367588932806; 
        DiveSpeedLimit1 = 0.39; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_YAK1)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.6; 
        RollRate = 0.55;
        manoeverability = 0.1389;
        maxDurability = 740;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 23.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0970872;
        ammo = 1600;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .177;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.60; 
        inertia = 2.1; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.324110671937; 
        DiveSpeedLimit1 = 0.32; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == BOMBER_B29)
        {
        o->cubex = zoom * cubefac * 3;
        o->cubey = zoom * cubefac * 2;
        o->cubez = zoom * cubefac * 3;
        HistoricPeriod = 2;
        DefensiveLethality = 30;
        maxthrust = 1.6; 
        RollRate = 0.28;
        manoeverability = 0.0900;
        maxDurability = 17100;
        zoom = 1.25;
        maxtheta = 90.0;
        maxgamma = 9.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 2450;
        impact = 7000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = .69;
        inertia = 5.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.313241106719; 
        DiveSpeedLimit1 = 0.33; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.38; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1400;
        } 
    if (id == FIGHTER_DW520)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.56; 
        RollRate = 0.58;
        manoeverability = 0.1584;
        maxDurability = 1380;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 21.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.0;
        ammo = 2450;
        impact = 500;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.303359683794; 
        DiveSpeedLimit1 = 0.32; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.40; // Faster than this will induce severe airframe damage.
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == BOMBER_SB2C)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 2;
        DefensiveLethality = 6;
        maxthrust = 1.7; 
        RollRate = 0.33;
        manoeverability = 0.1260;
        maxDurability = 5000;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 18.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.0;
        ammo = 2450;
        impact = 1400;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=2.60;
        MaxFullPowerAltRatio = 0.48;
        inertia = 3.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.251976284585; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_TBF)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 7;
        maxthrust = 1.85; 
        RollRate = 0.33;
        manoeverability = 0.1278;
        maxDurability = 6000;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 20.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.6;
        ammo = 2450;
        impact = 1300;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=2.50;
        MaxFullPowerAltRatio = 0.48;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.23; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.23; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.232213438735; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.39; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == FIGHTER_ME163)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 3.80; 
        RollRate = 0.65;
        manoeverability = 0.1620;
        maxDurability = 1500; 
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 55; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 26.56;
        ammo = 120;
        impact = 400;
        BlackoutSensitivity = 24.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 95.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 1.00; 
        inertia = 1.7; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.44; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.44; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.533596837945; 
        DiveSpeedLimit1 = 0.55; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.66; // Faster than this will induce severe airframe damage.
        WepCapable = false;
        StallSpeed = 0.140;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_TEMPEST)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.85; 
        RollRate = 0.50;
        manoeverability = 0.1397;
        maxDurability = 900;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 29; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 5.4679600;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1770;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.3;  
        deadweight = 0.13;
        CompressibilitySpeed = 0.32; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.32; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.387351778656; 
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.46; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.140;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_D3A)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 1;
        maxthrust = 1.5;  
        RollRate = 0.40;
        manoeverability = 0.1181;
        maxDurability = 2600;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 17.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.9009708;
        ammo = 2450;
        impact = 1300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.90;
        MaxFullPowerAltRatio = 0.66;
        inertia = 3.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.21; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.21;
        SeaLevelSpeedLimitThreshold = 0.2243083004; 
        DiveSpeedLimit1 = 0.27; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_B5N)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 1;
        maxthrust = 1.4; 
        RollRate = 0.33;
        manoeverability = 0.1422;
        maxDurability = 3300;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 16.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.9009708;
        ammo = 2450;
        impact = 1400;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower = 1.90;
        MaxFullPowerAltRatio = 0.48;
        inertia = 4.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.200; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.200; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.192687747036; 
        DiveSpeedLimit1 = 0.210; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.260; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_DAUNTLESS)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.5; 
        RollRate = 0.40;
        manoeverability = 0.1382;
        maxDurability = 4000;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 19.0 ;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0753126;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower = 2.0;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.75; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.225; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.225; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.212450592885; 
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.40; // Faster than this will induce severe airframe damage.
        WepCapable = false;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_ME110)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 2;
        maxthrust = 1.8; 
        RollRate = 0.47;
        manoeverability = 0.1195;
        maxDurability = 960;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 17.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 5.4679600;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 3.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.326086956522; 
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == BOMBER_DORNIER)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.55; 
        RollRate = 0.33;
        manoeverability = 0.1170;
        maxDurability = 3900;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 18; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 3.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.20; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.20; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.212450592885; 
        DiveSpeedLimit1 = 0.24; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.32; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_HE111)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 7;
        maxthrust = 1.6; 
        RollRate = 0.33;
        manoeverability = 0.1139;
        maxDurability = 4400;
        zoom = 0.75;
        maxtheta = 90.0;
        maxgamma = 11; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.30;
        ammo = 2450;
        impact = 2000;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 4.7; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.23023715415; 
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.300; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == BOMBER_JU88)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 2.1; 
        RollRate = 0.33;
        manoeverability = 0.1224;
        maxDurability = 2500;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 8; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .180;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.61;
        inertia = 4.2; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.27371541502; 
        DiveSpeedLimit1 = 0.30; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.125;
        ServiceCeilingAltitude = 1026;
        } 
    if (id == FIGHTER_KI84)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.99; 
        RollRate = 0.60;
        manoeverability = 0.1656;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 29; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2400000;
        ammo = 2450;
        impact = 700;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.2; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.340; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.340; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.3814229249; 
        DiveSpeedLimit1 = 0.378; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_KI61)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.55; 
        RollRate = 0.55;
        manoeverability = 0.1454;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2400000;
        ammo = 2450;
        impact = 800;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.324110671937; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_GENERIC01)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.55; 
        RollRate = 0.55;
        manoeverability = 0.1454;
        maxDurability = 960;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 20; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2400000;
        ammo = 2450;
        impact = 800;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.324110671937; 
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_A6M5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.50; 
        RollRate = 0.50;
        manoeverability = 0.195;
        maxDurability = 420;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 28.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.8827200;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.51; 
        inertia = 1.0; 
        deadweight = 0.07;
        CompressibilitySpeed = 0.280; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.280; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.311264822134; // A6M5: 0.311264822134 yields 315MPH.
        DiveSpeedLimit1 = 0.290; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.370; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1500;
        } 
    if (id == FIGHTER_SPIT5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.60; 
        RollRate = 0.58;
        manoeverability = 0.172;
        maxDurability = 840;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 27.5; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.1685400;
        ammo = 2450;
        impact = 600;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1700;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 2.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.35; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.370553359684; 
        DiveSpeedLimit1 = 0.3806; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.46; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_P51B)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.60; 
        RollRate = 0.55;
        manoeverability = 0.150;
        maxDurability = 1020;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 19; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.1130000;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.67;
        inertia = 2.3; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.3814229249; 
        DiveSpeedLimit1 = 0.3873; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.39; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_P47B)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.70; 
        RollRate = 0.72;
        manoeverability = 0.1267;
        maxDurability = 2400;
        zoom = 0.36;
        maxtheta = 90.0;
        maxgamma = 16; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.4;
        ammo = 1600;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.685; 
        inertia = 3.0; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.35; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.357707509881; 
        DiveSpeedLimit1 = 0.43; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.135;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_ME109F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.64; 
        RollRate = 0.55;
        manoeverability = 0.1512;
        maxDurability = 780;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 26; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 500;
        BlackoutSensitivity = 22.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0; 
        ClipDistance = 0.03;
        FlapSpeed = .170;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68;
        inertia = 1.8; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.28; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.28; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.340909090909; 
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.130;
        ServiceCeilingAltitude = 1760;
        } 
    if (id == FIGHTER_P38F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.60; 
        RollRate = 0.70;
        manoeverability = 0.175;
        maxDurability = 1020;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 26.0; 
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 100.0; 
        ClipDistance = 0.03;
        FlapSpeed = .1778; 
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=4000;
        FlapsLevelElevatorEffect4=11000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.68; 
        inertia = 3.5; 
        deadweight = 0.13;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.30; // Replaces above setting when SpeedBrakes are active.
        SeaLevelSpeedLimitThreshold = 0.363636363636; 
        DiveSpeedLimit1 = 0.42; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.504; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.120;
        ServiceCeilingAltitude = 1760;
        } 
    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        recthrust = maxthrust;
        Durability = maxDurability;
        thrust = recthrust = maxthrust;
        smoke->type = 1;
        impact = 2;
        forcez = recthrust;
        }
    if (id == FLAK_AIR1)
        {
        maxthrust = 0;
        thrust = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 12.0;
        Durability = maxDurability = 240;
        zoom = 0.35;
        }
    if (id == FLARAK_AIR1)
        {
        maxthrust = 0;
        thrust = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 6.0;
        Durability = maxDurability = 210;
        zoom = 0.3;
        missiles [6] = 100;
        }
    if (id >= FLAK1 && id <= FLAK2)
        {
        o->cubex = zoom * cubefac1;
        o->cubey = zoom * cubefac1;
        o->cubez = zoom * cubefac1;
        }
    if (id == TANK_AIR1)
        {
        maxthrust = 0.04;
        thrust = 0;
        gamma = 180;
        theta = 0;
        phi = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 8.0;
        Durability = maxDurability = 600;
        zoom = 0.35;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.45;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_GROUND1)
        {
        maxthrust = 0.04;
        thrust = 0;
        gamma = 180;
        theta = 0;
        phi = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 8.0;
        Durability = maxDurability = 600;
        zoom = 0.4;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_PICKUP1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 90;
        zoom = 0.25;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.55;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_TRUCK1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 60;
        zoom = 0.45;
        o->cubex = zoom * 0.6;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.6;
        }
    else if (id == TANK_TRUCK2)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 120;
        zoom = 0.4;
        o->cubex = zoom * 0.6;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.6;
        }
    else if (id == TANK_TRSAM1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 150;
        zoom = 0.35;
        missiles [6] = 200;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.6;
        o->cubez = zoom * 0.7;
        }
    if (id >= TANK1 && id <= TANK2)
        {
        }
    if (id == SHIP_CRUISER)
        {
        zoom = 5.0;
        maxthrust = 0.05;
        thrust = 0.05;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 4.0; //  was 4.0
        impact = 200;
        Durability = maxDurability = 30000;
        missiles [6] = 200;
        o->cubex = zoom * 0.35;
        o->cubey = zoom * 0.1;
        o->cubez = zoom * 0.35;
        }
    else if (id == SHIP_DESTROYER1)
        {
        zoom = 2.5;
        maxthrust = 0.05;
        thrust = 0.05;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 6.0; //  was 6.0
        impact = 300;
        Durability = maxDurability = 15000;
        o->cubex = zoom * 0.4;
        o->cubey = zoom * 0.12;
        o->cubez = zoom * 0.4;
        }
    if (id == BOMB01)
        {
        intelligence = 0;
        maxthrust = 0.01 * missilethrustbase;
        RollRate = 0.0;
        manoeverability = 0.0;
        ai = true;
        StaticDrag = 8;
        ttl = 2000 * timestep;
        impact = 30000;
        }
    else if (id == MISSILE_AIR2)
        {
        intelligence = 100;
        maxthrust = 0.5 * missilethrustbase;
        RollRate = 3.5; //  old 3.5
        manoeverability = 4.5;
        StaticDrag = 8;
        ttl = 320 * timestep;
        impact = 2100;
        }
    else if (id == MISSILE_AIR3)
        {
        intelligence = 100;
        maxthrust = 0.65 * missilethrustbase;
        RollRate = 4.5;
        manoeverability = 5.5; //  was 2.0
        StaticDrag = 8;
        ttl = 340 * timestep;
        impact = 2200;
        }
    else if (id == MISSILE_GROUND1)
        {
        intelligence = 10;
        maxthrust = 0.60 * missilethrustbase;
        RollRate = 1.2;
        manoeverability = 1.0; //  was 1.0
        ai = true;
        StaticDrag = 8;
        ttl = 300 * timestep;
        impact = 3000;
        }
    else if (id == MISSILE_GROUND2)
        {
        intelligence = 10;
        maxthrust = 0.75 * missilethrustbase;
        RollRate = 1.5;
        manoeverability = 1.0; //  was 1.0
        ai = true;
        StaticDrag = 8;
        ttl = 400 * timestep;
        impact = 4000;
        }
    else if (id == MISSILE_DF1)
        {
        intelligence = 1;
        maxthrust = 0.80 * missilethrustbase;
        RollRate = 0.0;
        manoeverability = 0.0;
        ai = true;
        StaticDrag = 8;
        ttl = 350 * timestep;
        impact = 8000;
        }
    else if (id == MISSILE_FF1)
        {
        intelligence = 100;
        maxthrust = 0.85 * missilethrustbase;
        RollRate = 2.0;
        manoeverability = 5.0;
        StaticDrag = 10;
        ttl = 300 * timestep;
        impact = 2100;
        }
    else if (id == MISSILE_FF2)
        {
        intelligence = 100;
        maxthrust = 0.90 * missilethrustbase;
        RollRate = 3.0;
        manoeverability = 6.0; //  was 2.0
        StaticDrag = 10;
        ttl = 320 * timestep;
        impact = 2200;
        }
    else if (id == MISSILE_MINE1)
        {
        intelligence = 0;
        maxthrust = 0.1;
        RollRate = 1.0;
        manoeverability = 1.0; //  was 1.0
        ai = true;
        StaticDrag = 8;
        ttl = -1;
        impact = 500;
        zoom = 0.3;
        }
    if (id >= MISSILE1 && id <= MISSILE2)
        {
        o->cubex = zoom;
        o->cubey = zoom;
        o->cubez = zoom;
        }
    if (id >= STATIC_PASSIVE)
        {
        intelligence = 0;
        maxthrust = 0;
        RollRate = 0;
        manoeverability = 0;
        impact = 5;
        maxtheta = 0;
        maxgamma = 0;
        }
    if (id == STATIC_CONTAINER1)
        {
        Durability = maxDurability = 90;
        zoom = 1.0;
        impact = 20;
        o->cubex = zoom * 0.4;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.9;
        }
    if (id == STATIC_BATTLESHIP)
        {
        Durability = maxDurability = 120000;
        zoom = 12.0;
        impact = 2000;
        o->cubex = zoom * 0.20;  //  Width of parked battleship's hit bubble
        o->cubey = zoom * 0.16; //  Altitude of parked battleship's hit bubble
        o->cubez = zoom * 1.05; //  Length of parked battleship's hit bubble
        }
    if (id == STATIC_HALL2)
        {
        Durability = maxDurability = 900;
        zoom = 2.5;
        impact = 20;
        o->cubex = zoom;
        o->cubey = zoom * 0.45;
        o->cubez = zoom;
        }
    if (id == STATIC_OILRIG1)
        {
        Durability = maxDurability = 1400;
        zoom = 3.5;
        impact = 20;
        o->cubex = zoom * 0.95;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.95;
        }
    if (id == STATIC_COMPLEX1)
        {
        Durability = maxDurability = 5000;
        zoom = 2.0;
        impact = 20;
        o->cubex = zoom * 0.75;
        o->cubey = zoom * 0.6;
        o->cubez = zoom * 0.75;
        }
    if (id == STATIC_RADAR1)
        {
        Durability = maxDurability = 500;
        zoom = 1.3;
        impact = 20;
        o->cubex = zoom * 0.5;
        o->cubey = zoom * 0.7;
        o->cubez = zoom * 0.5;
        }
    if (id == STATIC_BASE1)
        {
        Durability = maxDurability = 5500;
        zoom = 4.0;
        impact = 20;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.7;
        }
    if (id == STATIC_DEPOT1)
        {
        Durability = maxDurability = 3000;
        zoom = 1.5;
        impact = 20;
        o->cubex = zoom;
        o->cubey = zoom * 0.5;
        o->cubez = zoom;
        }
    if (id == STATIC_BARRIER1)
        {
        Durability = maxDurability = 1000;
        zoom = 12.0;
        impact = 2000;
        o->cubex = 0.8;
        o->cubey = 11;
        o->cubez = 11;
        }
    this->intelligence = intelligence;
    this->precision = precision;
    this->aggressivity = aggressivity;
    missileCount ();
    } //  end AIObj::newinit ()



void AIObj::newinit (int id, int party, int intelligence)
    {
    newinit (id, party, intelligence, intelligence, intelligence);
    }

int AIObj::nextMissile (int from)
    {
    int i = from + 1;
    if (i >= missiletypes)
        {
        i = 0;
        }
    while (!missiles [i])
        {
        i ++;
        if (i >= missiletypes)
            {
            i = 0;
            }
        if (i == from)
            {
            break;
            }
        }
    ttf = 50 * timestep;
    return i;
    }

bool AIObj::selectMissileAir (AIObj **missile)
    {
    bool sel = false;
    if (haveMissile (MISSILE_AIR3))
        {
        missiletype = MISSILE_AIR3 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_AIR2))
        {
        missiletype = MISSILE_AIR2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (BOMB01))
        {
        missiletype = BOMB01 - MISSILE1;
        sel = true;
        }
    return sel;
    }

bool AIObj::selectMissileAirFF (AIObj **missile)
    {
    bool sel = false;
    if (haveMissile (MISSILE_FF2))
        {
        missiletype = MISSILE_FF2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_FF1))
        {
        missiletype = MISSILE_FF1 - MISSILE1;
        sel = true;
        }
    return sel;
    }

bool AIObj::selectMissileGround (AIObj **missile)
    {
    bool sel = false;
    if (haveMissile (MISSILE_GROUND2))
        {
        missiletype = MISSILE_GROUND2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_GROUND1))
        {
        missiletype = MISSILE_GROUND1 - MISSILE1;
        sel = true;
        }
    return sel;
    }

void AIObj::targetNearestEnemy (AIObj **f)
    {
    int i;
    float d = 1E12; 
    ttf = 50 * timestep;
    for (i = 0; i < maxfighter; i ++)
        { 
        if (this != f [i] && party != f [i]->party && f [i]->active)
            { 
            float phi = getAngle (f [i]); 
            float d2 = distance (f [i]) * (60 + fabs (phi)); // prefer enemies in front

            if (d2 < d)
                {
                d = d2;
                target = f [i];
                }
            }
        }
    if (!ai && target)
        if (distance (target) > 400)
            {
            target = NULL;
            }
    }

void AIObj::targetNearestGroundEnemy (AIObj **f)
    {
    int i;
    float d = 1E12; 
    ttf = 50 * timestep;
    for (i = 0; i < maxfighter; i ++)
        {
        if (this != f [i] && party != f [i]->party && f [i]->active)
            {
            float phi = getAngle (f [i]);
            float d2 = distance (f [i]) * (60 + fabs (phi)); // prefer enemies in front
            if (bomber)
                if (f [i]->id < MOVING_GROUND)
                    {
                    d2 += 1E10;    // only use this target if no ground targets exist
                    }
            if (d2 < d)
                {
                d = d2;
                target = f [i];
                }
            }
        }
    if (target)
        if (distance (target) > 400)
            {
            target = NULL;
            }
    }

void AIObj::targetNext (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;
    if (target == ThreeDObjects[0])
        {
        
        target = f [0]; // Special case. Select NOTHING.
        return; // and keep it that way!
        }
    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }
    int z = 0;
    do
        {
        i ++;
        if (i >= maxfighter)
            {
            i = 0;
            }
        if (f [i] == this)
            {
            i ++;
            z ++;
            }
        if (i >= maxfighter)
            {
            i = 0;
            }
        }
    while ((!f [i]->active || distance (f [i]) > 11000) && z <= 1); 
    target = f [i];
    if (z > 1 && !ai)
        {
        target = NULL;
        }
    } 

void AIObj::targetPrevious (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;
    if (target == NULL)
        {
        target = f [0];
        }
    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }
    int z = 0;
    do
        {
        i --;
        if (i < 0)
            {
            i = maxfighter - 1;
            }
        if (f [i] == this)
            {
            i --;
            z ++;
            }
        if (i < 0)
            {
            i = maxfighter - 1;
            }
        }
    while ((!f [i]->active || distance (f [i]) > 11000) && z <= 1);
    target = f [i];
    if (z > 1 && !ai)
        {
        target = NULL;
        }
    } 

void DynamicObj::activate ()
    {
    active = true;
    draw = true;
    }

// check the objects Durability value for damage. Explode/sink if necessary
void DynamicObj::checkDurability ()
{
        if (fplayer->target == nullptr)
        {
             sprintf (DebugBuf, "CRITICAL ERROR: Called DynamicObj::checkDurability when fplayer->target is NULL");
             display (DebugBuf, LOG_ERROR);
             return;
        }


    if (Durability <= 0)
        {
        Durability = 0;

	// CRITICAL TODO: fix crash here,
	// assert(fplayer != nullptr);
	// Seems to be mostly this one.
	// assert(fplayer->target != nullptr);

        while (!fplayer->target->active)
           {
           display ((char*)"DynamicObj::checkDurability() an inactive, selected target has been destroyed.", LOG_MOST);
           fplayer->target = ThreeDObjects[0];
           sprintf(DebugBuf, "WARNING: at aiobject.cpp:DynamicObj:checkDurability:5354 fplayer->target is now %p",
		(void*) &ThreeDObjects[0]);
           fplayer->targetNext ((AIObj **) ThreeDObjects);
           }
        if (explode <= 0)
            {
            if (id >= MISSILE1 && id <= MISSILE2)
                {
                explode = 1;
                active = false;
                }
            if (id >= FIGHTER1 && id <= FIGHTER2)
                {
                explode = 1;
                active = false;
                }
            if (id >= TANK1 && id <= TANK2)
                {
                explode = 1;
                active = false;
                }
            if (id >= FLAK1 && id <= FLAK2)
                {
                explode = 1;
                active = false;
                }
            if (id >= STATIC_PASSIVE)
                { 
                if (
                   CurrentMissionNumber != MISSION_NETWORKBATTLE01
                   && CurrentMissionNumber != MISSION_NETWORKBATTLE02
                   && CurrentMissionNumber != MISSION_NETWORKBATTLE03
                   )
                   {
                   explode = 1;
                   active = false;
                   }
                else
                   { 
                   static bool latch = false;
                   if (!latch)
                      {
                      
                      sound->setVolume (SOUND_EXPLOSION1, 120);
                      sound->play (SOUND_EXPLOSION1, false);
                      latch = true;
                      }
                   if (!ThreeDObjects [22]->active && (BattleshipSunkAlertCountBlue <= 2))
                      { 
                      
                      BattleshipSunkAlertCountBlue++; 
                      if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.70))
                         {
                         ThreeDObjects[28]->Durability = (ThreeDObjects[28]->maxDurability * 0.69);
                         }
                      sprintf (SystemMessageBufferA, "BLUE TEAM BATTLESHIP DESTROYED.");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   if (!ThreeDObjects [23]->active  && (BattleshipSunkAlertCountRed <= 2))
                      { 
                      
                      BattleshipSunkAlertCountRed++; 
                      if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.70))
                         {
                         ThreeDObjects[29]->Durability = (ThreeDObjects[29]->maxDurability * 0.69);
                         }
                      sprintf (SystemMessageBufferA, "RED TEAM BATTLESHIP DESTROYED.");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   if (ThreeDObjects[28]->Durability <= 400)
                      { 
                      ThreeDObjects[24]->Durability = 0; //  Destroy associated RADARReflector too.
                      if (ThreeDObjects[28]->Durability <= 0)
                         { //  Get here when BlueTeam HQ is completely destroyed.
                         if (fplayer->target == ThreeDObjects[28])
                            { //  Get here if we were targeting the airfield about to be destroyed
                            fplayer->targetNext ((AIObj **) ThreeDObjects); //  Don't let this continue
                            display ((char*)"DynamicObj::checkDurability() advancing target beyond destroyed airfield 28.", LOG_MOST);
                            }
                         sound->haltMusic();
                         display ((char*)"DynamicObj::checkDurability Playing mission-end music...", LOG_MOST);
                         if (MyNetworkId%2)
                            { //  Get here if player is on RedTeam and won the battle
                            sound->setVolume (SOUND_MISSIONENDINGINSTRUMENTAL, 127);
                            sound->play (SOUND_MISSIONENDINGINSTRUMENTAL, false);
                            }
                         else
                            { //  Get here if player is on BlueTeam and lost the battle
                            sound->setVolume (SOUND_DEFEAT00, 127);
                            sound->play (SOUND_DEFEAT00, false);
                            }
                         sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                         NewSystemMessageNeedsScrolling = true;
                         MissionEndingTimer = 13000; //  Signal end of mission in 13 seconds.
                         ThreeDObjects[28]->DamageInNetQueue = 300000; // Force other logic to propogate destruct message to all players
                         }
                      }
                   else if (ThreeDObjects[29]->Durability <= 400)
                      { 
                      ThreeDObjects[25]->Durability = 0; //  Destroy associated RADARReflector too.
                      if (ThreeDObjects[29]->Durability <= 0)
                         { //  Get here when RedTeam HQ is completely destroyed.
                         if (fplayer->target == ThreeDObjects[29])
                            { //  Get here if we were targeting the airfield about to be destroyed
                            fplayer->targetNext ((AIObj **) ThreeDObjects); //  Don't let this continue
                            display ((char*)"DynamicObj::checkDurability() advancing target beyond destroyed airfield 29.", LOG_MOST);
                            }
                         sound->haltMusic();
                         display ((char*)"DynamicObj::checkDurability Playing mission-end music...", LOG_MOST);
                         if (!MyNetworkId%2)
                            { //  Get here if player is on BlueTeam and won the battle
                            sound->setVolume (SOUND_DEFEAT00, 127);
                            sound->play (SOUND_DEFEAT00, false);
                            }
                         else
                            { //  Get here if player is on RedTeam and lost the battle
                            sound->setVolume (SOUND_MISSIONENDINGINSTRUMENTAL, 127);
                            sound->play (SOUND_MISSIONENDINGINSTRUMENTAL, false);
                            }
                         sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                         NewSystemMessageNeedsScrolling = true;
                         MissionEndingTimer = 13000; //  Signal end of mission in 13 seconds.
                         ThreeDObjects[29]->DamageInNetQueue = 300000; // Force other logic to propogate destruct message to all players
                         }
                      }
                   else if (ThreeDObjects[24]->Durability <= 0)
                      {
                      sprintf (SystemMessageBufferA, "BLUETEAM RADAR DESTROYED");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   else if (ThreeDObjects[25]->Durability <= 0)
                      {
                      sprintf (SystemMessageBufferA, "REDTEAM RADAR DESTROYED");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   NewSystemMessageNeedsScrolling = true;
                   explode = 1;
                   active = false;
                   }
                }
            }
        if (sink <= 0)
            if (id >= SHIP1 && id <= SHIP2)
                {
                sink = 1;
                }
        }
} 

// check whether the object is exploding or sinking and deactivate if necessary
void DynamicObj::checkExplosion (Uint32 dt) 
    {
    if (explode > 0)
        {
        if (explode == 1)
            {
            ttl = -1;
            if (
               id == STATIC_CONTAINER1 ||
               id == STATIC_RADAR1 ||
               id == STATIC_COMPLEX1 ||
               id == STATIC_DEPOT1 ||
               id == TANK1 ||
               id == STATIC_AIRFIELD00
               )
                {
                
                setExplosion (2.0, 100 * timestep);
                setBlackSmoke (14.0, 600 * timestep);
                }
            else if (id == STATIC_OILRIG1)
                {
                setExplosion (3.0, 40 * timestep);
                setBlackSmoke (5.5, 80 * timestep);
                }
            else
                { 
                float zoom2 = zoom * 2;

                if (zoom2 > 2)
                    {
                    zoom2 = 2;
                    }
                setExplosion (zoom2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                }
            }
        if (id >= STATIC_GROUND || (id >= MOVING_GROUND && id <= MOVING_WATER))
            {
            if (explode >= 25 * timestep && ttl == -1)
                {
                setExplosion (zoom * 2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                ttl = -2;
                }
            if (explode >= 30 * timestep && ttl == -2)
                {
                setExplosion (zoom * 2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                ttl = -3;
                }
            }
        if (explode >= 35 * timestep)
            {
            deactivate ();
            ttl = -1;
            explode += dt;
            setBlackSmoke (18.0, 600 * timestep);
            if (id == STATIC_RADARREFLECTOR)
               {
               
               if (tl->x > 0)
                  { 
                  if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.4))
                     {
                     ThreeDObjects[29]->Durability = ThreeDObjects[29]->maxDurability * 0.39;
                     RadarReflectorRedHasBeenDestroyedAtLeastOnce = true;
                     }
                  }
               else
                  { 
                  if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.4))
                     {
                     ThreeDObjects[28]->Durability = ThreeDObjects[28]->maxDurability * 0.39;
                     RadarReflectorBlueHasBeenDestroyedAtLeastOnce = true;
                     }
                  }
               }
            if (((id != STATIC_AIRFIELD00) && (id != STATIC_RADARREFLECTOR) && (id >= STATIC_GROUND))|| ((id >= MOVING_GROUND) && (id <= MOVING_WATER)))
                {
                explode = 0;
                draw = true;
                id = STATIC_PASSIVE;
                Durability = 100000; 
                o = &model_rubble1;  
                zoom *= 0.7F;
                if (zoom > 1)
                    {
                    zoom = 1;
                    }
                
                tl->y = l->getExactHeight (tl->x, tl->z) + zoom / 4;
                }
            }
        else
            {
            explode += dt;
            }
        } 
    if (sink)
        {
        sink += dt;
        if (sink > 100 * timestep)
            {
            deactivate ();
            ttl = -1;
            }
        }
    } 

// check for a looping, this is tricky :-)
bool DynamicObj::checkLooping ()
    {
    if (gamma > 270)
        {
        gamma = 540 - gamma;
        theta += 180;
        phi += 180;
        rectheta += 180;
        if (theta >= 360)
            {
            theta -= 360;
            }

        if (rectheta >= 360)
            {
            rectheta -= 360;
            }

        if (phi >= 360)
            {
            phi -= 360;
            }
        return true; 
        }
    else if (gamma < 90)
        {
        gamma = 180 - gamma;
        theta += 180;
        phi += 180;
        rectheta += 180;
        if (theta >= 360)
            {
            theta -= 360;
            }
        if (rectheta >= 360)
            {
            rectheta -= 360;
            }
        if (phi >= 360)
            {
            phi -= 360;
            }
        return true; 
        }
    return false; 
    } // end DynamicObj::checkLooping ()

// check for collision, simplified model, each model is surrounded by a cube
// this works pretty well, but we must use more than one model for complex models or scenes
void DynamicObj::collide (DynamicObj *d, Uint32 dt) // d must be the medium (MachineGunBullet, missile)
    {
    
    if (immunity > 0 || d->immunity > 0)
        {
        return;    
        }
    if (explode > 0 || sink > 0)
        {
        return;
        }
    bool collide = false;
    if (    tl->x + o->cubex >= d->tl->x - d->o->cubex && tl->x - o->cubex <= d->tl->x + d->o->cubex &&
            tl->y + o->cubey >= d->tl->y - d->o->cubey && tl->y - o->cubey <= d->tl->y + d->o->cubey &&
            tl->z + o->cubez >= d->tl->z - d->o->cubez && tl->z - o->cubez <= d->tl->z + d->o->cubez)
        {
        collide = true;
        }
    if (collide)
        { 
        if (id == STATIC_BATTLESHIP)
           { 
           if (d == (DynamicObj *) fplayer  )
              { 
              display ((char*)"DynamicObj::collide() player collided with a Battleship", LOG_MOST);
              fplayer->Durability -= 12;
              }
           }
        if (id == STATIC_AIRFIELD00)
           { 
           if (d->id > FIGHTER1 && d->id < FIGHTER2)
              { 
              if (
                 tl->x + 1 >= d->tl->x - 1 && tl->x - 1 <= d->tl->x + 1 &&
                 tl->y + 1 >= d->tl->y - 1 && tl->y - 1 <= d->tl->y + 1 &&
                 tl->z + 1 >= d->tl->z - 1 && tl->z - 1 <= d->tl->z + 1
                 )
                    { 
                    if (d == (DynamicObj *) fplayer)
                       { 
                       fplayer->Durability -= 4.0F; 
                       }
                    }
              else
                    { 
                    return; 
                    }
              }
           else if (d->id == BOMB01)
              { 
              d->Durability -= 30000.0; 
              DamageInNetQueue += 30000.0; 
              display ((char*)"DynamicObj::collide() bomb hit. DamageInNetQueue += 30000.0", LOG_MOST);
              }
           else if (d->id == MISSILE_DF1)
              { 
              d->Durability -= 8000; 
              DamageInNetQueue += 8000.0; 
              display ((char*)"DynamicObj::collide() missile hit. DamageInNetQueue += 8000.0", LOG_MOST);
              }
           } 
        if (this == (DynamicObj *) fplayer && game == GAME_PLAY && realism && d->id >= AIR && d->id < MOVING_GROUND)
            { // Get here if the player has collided with another aircraft
            Durability = -1.0F; // Damage the player
            d->Durability = -1.0F; // Damage the other aircraft with which we collided
            }
        if (id < STATIC_PASSIVE || (id >= STATIC_PASSIVE && d->id >= MISSILE1 && d->id <= MISSILE2))
            { 
            Durability -= (float) d->impact; 
            sprintf (DebugBuf, "BlueTeam Battleship status = %f, RedTeam Battleship status = %f.", ThreeDObjects[22]->Durability, ThreeDObjects[23]->Durability);
            display (DebugBuf, LOG_MOST);
            
            if (
               CurrentMissionNumber ==  MISSION_NETWORKBATTLE01 ||
               CurrentMissionNumber ==  MISSION_NETWORKBATTLE02 ||
               CurrentMissionNumber == MISSION_NETWORKBATTLE03
               )
               { 
               d->Durability -= 500; 
               }
            DamageInNetQueue += (float)d->impact;
            sprintf (DebugBuf, "DynamicObj::collide() impact. DamageInNetQueue += d->impact or %f", d->impact);
            display (DebugBuf, LOG_MOST);
            }
        if (d->id == CANNON1)
           {
           Durability -= fplayer->statLethality;
           }
        if (d->id == CANNON2)
           {
           Durability -= fplayer->statLethality;
           }
        if (d->source->party == party)
           { // Somebody is attempting fratricide
           if (d->source == (DynamicObj *) fplayer)
              { // player is shooting his own team
              if (id == STATIC_AIRFIELD00)
                 { 
                 //sprintf (SystemMessageBufferA, "YOU ARE SHOOTING YOUR OWN AIRFIELD!");
                 //NewSystemMessageNeedsScrolling = true;
                 //fplayer->Durability -= 0.02;
                 }
              }
           }
        if ((d->id != BOMB01) && (d->id != MISSILE_DF1))
           { 
           DamageInNetQueue += fplayer->statLethality;
           }
        if (d->source != NULL && active)   // only for missiles/cannons
            { 
            if (Durability <= 0)
                { 
                if (active && draw && !killed)
                   {
                   if (d->source->id >= FIGHTER1 && d->source->id <= FIGHTER2)
                       { 
                       killed = true;
                       DamageInNetQueue += 500000; 
                       display ((char*)"DynamicObj::collide() Fighter killed something. DamageInNetQueue += 500000.0", LOG_MOST);
                       if (id >= FIGHTER1 && id <= FIGHTER2)
                           { 
                           Sentient = 0; 
                           d->source->fighterkills ++;
                           fplayer->targetNext ((AIObj **) ThreeDObjects); 
                           
                           RecentVictim = id;
                           RecentVictimAltitude = tl->y;
                           RecentVictimXPosition = tl->x;
                           RecentVictimZPosition = tl->z;
                           RecentVictimGamma = gamma;
                           RecentVictimPhi = phi;
                           RecentVictimTheta = theta;
                           RecentVictimVelocity = realspeed * 0.8; 
                           }
                       else if (id >= SHIP1 && id <= SHIP2)
                           {
                           d->source->shipkills ++;
                           }
                       else if ((id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
                           {
                           d->source->tankkills ++;
                           }
                       if (id == STATIC_DEPOT1)
                           { 
                           if (tl->x > 0)
                              { 
                              display ((char*)"DynamicObj::collide() RedTeam Bunker Destroyed.", LOG_MOST);
                              
                              ThreeDObjects[29]->Durability -= 2400;
                              }
                       else
                              { 
                              display ((char*)"DynamicObj::collide() BlueTeam Bunker Destroyed.", LOG_MOST);
                              
                              ThreeDObjects[28]->Durability -= 2400;
                              }
                           }
                       }
                   }
                }
            }

        setExplosion (0.2, 20 * timestep);
        setBlackSmoke (0.5, 30 * timestep);
        }
    } 

// check whether the object collides on the ground and alter gamma and y-translation
void DynamicObj::crashGround (Uint32 dt)
    {
    static bool NoseWheelDamageInflicted; 
    static bool TireSqueakSounded; 
    static bool LandingDamageSounded; 
    static bool OceanCrashSounded; 
    static bool BellyScrapeSounded; 
    static bool WheelRolloutSounded; 
    if (id >= MOVING_GROUND)
        { 
        return;
        }
    float TerrainHeightHere = l->getExactHeight (tl->x, tl->z);
    float height = tl->y - TerrainHeightHere;
    if (id >= CANNON1 && id <= CANNON2)
        { 
        if (tl->y < TerrainHeightHere)
           {
           setExplosion (3.0, 30 * timestep); 
           deactivate ();
           }
        }
    if (height < zoom)
        { 
        if (this == (DynamicObj *) fplayer)
           { 
           tl->y -= (height - zoom); 
           }
        else
           { 
           if (Sentient > 3)
              { 
              tl->y -= (height - zoom*0.3); 
              }
           else
              { 
              if (this->id !=0)
                 { 
                 sprintf (
                         DebugBuf,
                         "crashGround(): Bot #%d destroyed due to impact with ground. Type =%d.",
                         this->MyMissionPlayerNumber,
                         this->id
                         );
                 display (DebugBuf, LOG_MOST);
                 setExplosion (3.0, 30 * timestep);
                 deactivate();
                 float Xdistance;
                 float Zdistance;
                 float XZdistance;
                 Xdistance = (fabs)(fplayer->tl->x - tl->x);
                 Zdistance = (fabs)(fplayer->tl->z - tl->z);
                 XZdistance = (Xdistance + Zdistance) * 1.5; 
                 if (XZdistance > 126)
                    {
                    XZdistance = 126;
                    }
                 sound->setVolume (SOUND_EXPLOSION1, (127 - (int)XZdistance));
                 sound->play (SOUND_EXPLOSION1, false);
                 } 
              }
           }
        if (realspeed < StallSpeed * 0.5)
           { 
           if (gamma > 180)
              { 
              gamma -= (.005 * timestep); 
              }
           if (gamma < 180)
              {
              gamma = 180;
              }
           theta = 0; 
           }
        if (Durability > 0)
            {
            if (id >= MISSILE1 && id <= MISSILE2)
                {
                setExplosion (1.2, 30 * timestep);
                setBlackSmoke (1.2, 30 * timestep);
                }
            }
        float decfac = 200.0F; 
        if (realism && this == (DynamicObj *) fplayer && game == GAME_PLAY)
            { 
            OnTheGround = true;
            if (fplayer->realspeed < (fplayer->StallSpeed * 1.0))
               { 
               if (gamma < 174.0 && !NoseWheelDamageInflicted)
                  { 
                  Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 7); 
                  NoseWheelDamageInflicted = true;
                  }
               float height2 = tl->y - l->getExactHeight (tl->x+1, tl->z);
               float height3 = tl->y - l->getExactHeight (tl->x-1, tl->z);
               float height4 = tl->y - l->getExactHeight (tl->x, tl->z+1);
               float height5 = tl->y - l->getExactHeight (tl->x, tl->z-1);
               float diff1 = (fabs)(height2 - height3);
               float diff2 = (fabs)(height3 - height4);
               float diff3 = (fabs)(height4 - height5);
               if (diff1 < 0.4 && diff2 < 0.4 && diff3 < 0.4)
                  { 
                  
                  if (fabs(l->getExactHeight(tl->x, tl->z) + SeaLevel) < 0.1)
                     { 
                     if (!OceanCrashSounded)
                        {
                        sound->setVolume (SOUND_CRASHOCEAN00, 90);
                        sound->play (SOUND_CRASHOCEAN00, false);
                        Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 15.0); 
                        if (fplayer->UndercarriageLevel)
                           { 
                           Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 9.0); 
                           }
                        OceanCrashSounded = true;
                        }
                     }
                  else if (fplayer->UndercarriageLevel)
                     { 
                     float PlayerAlt;
                     float RunwayAlt;
                     PlayerAlt = fabs(l->getExactHeight(tl->x, tl->z));
                     RunwayAlt = fabs(l->getExactHeight(AirfieldXMin+1, AirfieldYMin+2));
                     if (PlayerAlt == RunwayAlt && (fabs(fplayer->theta)) < 5.0)
                        { 
                        Durability -= 0; 
                        if (fplayer->thrust < 0.01)
                           {
                           
                           ClearSpeedHistoryArrayFlag = true;
                           fplayer->realspeed = 0.0;
                           fplayer->thrust = 0.00;
                           fplayer->recthrust = 0.00; 
                           }
                        if (TrueAirSpeed < 50.00 && (
                                                    fplayer->id == BOMBER_B17       ||
                                                    fplayer->id == BOMBER_B24       ||
                                                    fplayer->id == BOMBER_JU87      ||
                                                    fplayer->id == BOMBER_G5M       ||
                                                    fplayer->id == BOMBER_B25       ||
                                                    fplayer->id == BOMBER_B26       ||
                                                    fplayer->id == BOMBER_B29       ||
                                                    fplayer->id == BOMBER_JU88      ||
                                                    fplayer->id == BOMBER_DORNIER   ||
                                                    fplayer->id == BOMBER_HE111     ||
                                                    fplayer->id == BOMBER_LANCASTER
                                                    )
                           )
                           { 
                           LandedAtSafeSpeed = true; 
                           UpdateOnlineScoreLogFileWithLandings();
                           
                           if (TakeoffLogged == true)
                              {
                              TakeoffLogged = false;
                              }
                           }
                        else if (TrueAirSpeed < 3.5)
                           { 
                           LandedAtSafeSpeed = true; 
                           UpdateOnlineScoreLogFileWithLandings();
                           
                           if (TakeoffLogged == true)
                              {
                              TakeoffLogged = false;
                              }
                           }
                        if (!TireSqueakSounded)
                           {
                           sound->setVolume (SOUND_TIRESQUEAK1, 90);
                           sound->play (SOUND_TIRESQUEAK1, false);
                           TireSqueakSounded = true;
                           }
                        }
                     if ((fabs(fplayer->theta)) > 10.0 && (fplayer->realspeed > 0.03 && !LandedAtSafeSpeed))
                        { 
                        Durability -= 15;
                        fplayer->theta = 0;
                        sound->setVolume (SOUND_BELLYSCRAPE00, 90);
                        sound->play (SOUND_BELLYSCRAPE00, false);
                        }
                     else
                        { 
                        if (!WheelRolloutSounded)
                           {
                           sound->setVolume (SOUND_WHEELROLLOUT00, 90);
                           sound->play (SOUND_WHEELROLLOUT00, false);
                           WheelRolloutSounded = true;
                           }
                        }
                     }
                  else
                     { 
                     if (!BellyScrapeSounded)
                        {
                        sound->setVolume (SOUND_BELLYSCRAPE00, 90);
                        sound->play (SOUND_BELLYSCRAPE00, false);
                        BellyScrapeSounded = true;
                        }
                     if (!GearUpDamageAlreadyInflicted)
                        {
                        Durability -= fplayer->realspeed * decfac * dt / timestep;; 
                        GearUpDamageAlreadyInflicted = true; 
                        }
                     if (fplayer->gamma > 2)
                        { 
                        fplayer->gamma -=0.5;
                        }
                     
                     }
                  }
               else
                  { 
                  
                  Durability -= fplayer->realspeed * decfac * dt / timestep;
                  if (!LandingDamageSounded)
                     {
                     sound->setVolume (SOUND_DAMAGE00, 90);
                     sound->play (SOUND_DAMAGE00, false);
                     LandingDamageSounded = true;
                     }
                  }
               }
            else
               { 
               
               if (!LandedAtSafeSpeed)
                  { 
                  Durability -= fplayer->realspeed * decfac * dt / timestep;
                  if (!LandingDamageSounded)
                     {
                     sound->setVolume (SOUND_DAMAGE00, 90);
                     sound->play (SOUND_DAMAGE00, false);
                     LandingDamageSounded = true;
                     }
                  }
               if (Durability < 0.5)
                  { 
                  setExplosion (0.2, 25 * timestep);
                  setBlackSmoke (0.5, 25 * timestep);
                  UpdateOnlineScoreLogFileWithCrashes(); 
                  }
               }
            }
        else
            { 
            
            if (id == BOMB01)
               { 
               Durability -= decfac * dt / timestep;
               }
            if (realspeed > 0.33)
               { 
               if (id != BOMB01)
                  { 
                  Durability -= decfac * dt / timestep;
                  }
                  if (Durability < 0 && (id >= FIGHTER1 && id <= FIGHTER2))
                   {
                   setExplosion (0.2, 25 * timestep);
                   setBlackSmoke (0.5, 25 * timestep);
                   active = false;
                   }
               }
            }
        }
    else
       { 
       if (this == (DynamicObj *) fplayer && height > zoom + 1)
          { 
          OnTheGround = false;
          if (LandedAtSafeSpeed)
             { 
             UpdateOnlineScoreLogFileWithTakeoffs();
             }
          LandedAtSafeSpeed = false; 
          
          LandingLogged = false; 
          GearUpDamageAlreadyInflicted = false;
          NoseWheelDamageInflicted = false;
          TireSqueakSounded = false;
          LandingDamageSounded = false;
          OceanCrashSounded = false;
          BellyScrapeSounded = false;
          WheelRolloutSounded = false;
          }
       }
    } 

void DynamicObj::deactivate ()
    {
    active = false;
    draw = false;
    }

void DynamicObj::dinit ()
    {
    rot->a = 90;
    phi = 0;
    theta = 0;
    gamma = 180;
    rectheta = 0;
    tl->z = 0;
    tl->x = 0;
    forcex = 0;
    forcez = 0;
    forcey = 0;
    maxthrust = 0.3;
    braking = 0;
    manoeverability = 0.5;
    thrust = maxthrust;
    recthrust = thrust;
    recheight = 5.0;
    ttl = -1;
    Durability = 0.01F;
    maxDurability = 0.01F;
    immunity = 0;
    recgamma = 180;
    id = CANNON1;
    impact = 7;
    source = NULL;
    points = 0;
    party = 0;
    elevatoreffect = 0;
    ruddereffect = 0;
    rolleffect = 0;
    maxgamma = 70;
    maxtheta = 90;
    gamma = 180;
    theta = 0;
    explode = 0;
    sink = 0;
    RollRate = 1.0;
    fighterkills = 0;
    shipkills = 0;
    tankkills = 0;
    otherkills = 0;
    killed = false;
    realism = physics;
    accx = accy = accz = 0;
    DragEffect=1.0;
    } 

float DynamicObj::distance (DynamicObj *target)
    {
    float dx = target->tl->x - tl->x;
    float dz = target->tl->z - tl->z;
    float dy = target->tl->y - tl->y;
    return sqrt (dx * dx + dz * dz + dy * dy);
    }

float DynamicObj::distanceXZ (DynamicObj *target)
    {
    float dx = target->tl->x - tl->x;
    float dz = target->tl->z - tl->z;
    return sqrt (dx * dx + dz * dz);
    }

DynamicObj::DynamicObj ()
    {
    dinit ();
    }

DynamicObj::DynamicObj (Space *space2, CModel *o2, float zoom2)
    {
    this->space = space2;
    o = o2;
    zoom = zoom2;
    dinit ();
    space->addObject (this);
    }

// return heading difference towards enemy
int DynamicObj::getAngle (DynamicObj *o)
    {
    float dx2 = o->tl->x - tl->x, dz2 = o->tl->z - tl->z;
    int a, w = (int) phi;
    if (dz2 > -0.0001 && dz2 < 0.0001)
        {
        dz2 = 0.0001;
        }
    a = (int) (atan (dx2 / dz2) * 180 / M_PI);
    if (dz2 > 0)
        {
        if (dx2 > 0)
            {
            a -= 180;
            }
        else
            {
            a += 180;
            }
        }
    int aw = a - w;

    if (aw < -180)
        {
        aw += 360;
        }
    if (aw > 180)
        {
        aw -= 360;
        }
    return aw;
    }

// return elevation difference towards enemy
int DynamicObj::getAngleH (DynamicObj *o)
    {
    float disttarget = distance (o);
    return (int) (atan ((o->tl->y - tl->y) / disttarget) * 180 / M_PI - (gamma - 180));
    }

void DynamicObj::move (Uint32 dt)
    {
    float DegreesToRadiansFactor = 0.01745333;
    float SpeedHistoryAccumulator = 0;
    unsigned char HistoryArrayPointer;
    if (dt <= 0)
        {
        return;
        }
    if (OnTheGround)
       {
       CalculateTrueAirspeed();
       }
    
    if (ViewResetTimer >0)
       { 
       ViewResetTimer += dt; 
       if (ViewResetTimer > 2000)
          { 
          view = ConfiguredViewDistance; 
          ViewResetTimer = 0;
          }
       }
    if (realspeed <= 0)
        {
        realspeed = 1.0F;
        }
    float brakepower = 1.0F;
    float timefac = (float) dt / (float) timestep;
    checkExplosion (dt); // check if this object is exploding
    if (sink > 0)   // only ships (they will not explode)
        {
        tl->y -= 0.02 * timefac; // sink down
        gamma = recgamma = 180.0 + 0.5 * (float) sink / timestep; // change angle when sinking
        return; // and exit move()
        }
    if (!active && !draw)
        {
        return;    // exit if not active
        }
    if (id == BOMB01)
       { 
       recgamma -= dt/15; 
       gamma -= dt/15;
       if (recgamma < 109)
          {
          recgamma = 109;
          }
       if (gamma < 109)
          {
          gamma = 109;
          }
       }
    if (id >= STATIC_PASSIVE)   // only buildings, static objects
        {
        // set the correct angles to diplay
        rot->setAngles ((short) (90 + gamma - 180), (short) theta + 180, (short) -phi);
        checkDurability ();
        return; // and exit this function
        }
    if (id == FLARE1)   // only flares
        {
        tl->y -= 0.04 * timefac; // fall down (gravity, constant)
        zoom = 0.12F + 0.03F * sin ((float) ttl / (float) timestep / 15); // blink (high frequency)
        phi = camphi; // angles to viewer (player)
        theta = 0;
        gamma = camgamma;
        }
    if (id == CHAFF1)   // only chaff
        {
        tl->y -= 0.04 * timefac; // fall down (gravity, constant)
        zoom = 0.12F + 0.01F * (80 * timestep - ttl) / timestep; // spread out
        phi = camphi; // angles to viewer (player)
        theta = 0;
        gamma = camgamma;
        }
    (void) checkLooping ();
    // The core of directional alterations and force calculations:
    int vz = 1;
    if (gamma < 90 || gamma > 270)
        {
        vz = -1;
        }
    
    if (maxthrust + thrust <= -0.00001 || maxthrust + thrust >= 0.00001)
        { 
        
        phi += vz * SIN(theta) * elevatoreffect * manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        gamma += COS(theta) * elevatoreffect * manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        phi += -vz * COS(theta) * ruddereffect * manoeverability * (0.66 + 3.0 * realspeed) * timefac;
        gamma += SIN(theta) * ruddereffect * manoeverability * (0.66 + 3.0 * realspeed) * timefac;
        // change roll due to roll ;-)
        if (rolleffect)
            {
            theta += rolleffect * (RollRate * (1.0 + realspeed)) * timefac * 5.0F;
            rectheta = theta;
            }
        }
    if (phi < 0)
        {
        phi += 360.0;    // validate heading
        }
    else if (phi >= 360.0)
        {
        phi -= 360.0;
        }
    if (theta < -180 && rectheta < -180)
        {
        rectheta += 360;
        theta += 360;
        }
    else if (theta >= 180 && rectheta >= 180)
        {
        rectheta -= 360;
        theta -= 360;
        }
    if (recthrust > maxthrust)   // check maximum throttle
        {
        recthrust = maxthrust;
        }
    float throttlechange = maxthrust / 100 * timefac; 
    if (recthrust > thrust + throttlechange)   
        { 
        thrust += (throttlechange * 2); 
        }
    else if (recthrust < thrust - throttlechange )
        { 
        thrust -= (throttlechange / 2);  
        }
    
    if ( party == 0 && NetworkOpponent && (MissionNumber == MISSION_HEADTOHEAD00))
       { 
       
       ThreeDObjects[MissionHeadToHead00State]->realspeed = InPacket.UdpObjSpeed;
       ThreeDObjects[MissionHeadToHead00State]->thrust = InPacket.UdpObjThrust;
       ThreeDObjects[MissionHeadToHead00State]->recthrust = InPacket.UdpObjThrust;
       }
    // PHYSICS (simplified model)
    CVector3 vaxis, uaxis, utemp, utemp2, utemp3;
    float gammaup, phiup, thetaup, RealSpeedTemp2;
    bool stop;
    float gravityforce;
    if (id <= CANNON2)
        {  
        tl->x += forcex * timefac; // add our vector to the translation
        tl->z += forcez * timefac;
        tl->y += forcey * timefac;
        goto cannondone; // jump down to decrease ttl and test collision
        }
    
    vaxis.set (COS(gamma) * SIN(phi), SIN(gamma), COS(gamma) * COS(phi));
    if (realism)
        {
        
        gammaup = gamma + 90;
        thetaup = -theta;
        phiup = phi;
        uaxis.set (COS(gammaup) * SIN(phiup), SIN(gammaup), COS(gammaup) * COS(phiup)); // upward axis (theta = 0)
        
        utemp.take (&uaxis);
        utemp.mul (COS(thetaup));
        utemp2.take (&vaxis);
        utemp2.mul ((1 - COS(thetaup)) * uaxis.dotproduct (&vaxis));
        utemp3.take (&uaxis);
        utemp3.crossproduct (&vaxis);
        utemp3.mul (SIN(thetaup));
        utemp.add (&utemp2);
        utemp.add (&utemp3);
        uaxis.take (&utemp);
        
        braking = (fabs (ruddereffect * 20.0) + fabs (elevatoreffect * 35.0) + fabs (rolleffect * 18.0) + fplayer->DragEffect)/200;
        brakepower = pow (0.93 - braking, timefac);
        
        accx *= brakepower;
        accy *= brakepower;
        accz *= brakepower;

        accz += thrust * vaxis.z * 0.3 * timefac;
        accx += thrust * vaxis.x * 0.3 * timefac;
        accy -= thrust * vaxis.y * 0.1 * timefac;
        
        accz += thrust * uaxis.z * 0.067 * timefac;
        accx += thrust * uaxis.x * 0.067 * timefac;
        accy -= thrust * uaxis.y * 0.067 * timefac * cos((fplayer->theta) * DegreesToRadiansFactor);
        
        accy -= 0.015 * timefac;
        
        accy -= fplayer->deadweight * timefac;
        
        if (id == BOMB01)
           { 
           
           accy += sin((gamma - 180)* DegreesToRadiansFactor) * ((fplayer->InertiallyDampenedPlayerSpeed)/ 0.4 );
           }
        else
           { 
           accy += sin((gamma - 180)* DegreesToRadiansFactor) * ((fplayer->InertiallyDampenedPlayerSpeed)/ 0.8 );
           }
        
        accx -= sin(fplayer->phi * DegreesToRadiansFactor) * ((fplayer->realspeed));
        accz -= cos(fplayer->phi * DegreesToRadiansFactor) * ((fplayer->realspeed));
        float stepfac = 0.24;
        tl->x += accx * timefac * stepfac;
        tl->z += accz * timefac * stepfac;
        tl->y += accy * timefac * stepfac;
        float scalef = 1.1;
        forcex = accx * stepfac * scalef;
        forcey = accy * stepfac * scalef;
        forcez = accz * stepfac * scalef;
        
        gravityforce = sqrt (realspeed) * vaxis.y * 1.10 * timefac; 
        forcez += gravityforce * vaxis.z;
        forcex += gravityforce * vaxis.x;
        forcey += gravityforce * vaxis.y;
        
        forcey -= gravityforce * vaxis.y * 2.2;
        }
    stop = false;
    if (id >= TANK1 && id <= TANK2)   // tanks cannot climb steep faces
        {
        float newy = l->getExactHeight (tl->x + forcex, tl->z + forcez) + zoom / 2;

        if (fabs (newy - tl->y) > 0.05)
            {
            stop = true;
            }
        else if (fabs (newy - tl->y) > 2)
            {
            stop = false;
            }
        }
    //
    //  InertiaEffects
    //
    
    InertiaTimer += DeltaTime;
    
    unsigned static char InertiaPhase;
    
    if (InertiaTimer < (1000 * inertia))
       {
       InertiaPhase = 0; 
       }
    else if (InertiaTimer < (2000 * inertia))
       {
       InertiaPhase = 1;
       }
    else if (InertiaTimer < (3000 * inertia))
       {
       InertiaPhase = 2;
       }
    else if (InertiaTimer < (4000 * inertia))
       {
       InertiaPhase = 3;
       }
    else if (InertiaTimer < (5000 * inertia))
       {
       InertiaPhase = 4;
       }
    else if (InertiaTimer < (6000 * inertia))
       {
       InertiaPhase = 5;
       }
    else if (InertiaTimer < (7000 * inertia))
       {
       InertiaPhase = 6;
       }
    else if (InertiaTimer < (8000 * inertia))
       {
       InertiaPhase = 7;
       }
    else if (InertiaTimer < (9000 * inertia))
       {
       InertiaPhase = 8;
       }
    else if (InertiaTimer < (10000 * inertia))
       {
       InertiaPhase = 9; // Last of 10 phases is phase "9".
       }
    
    SpeedHistoryArray[InertiaPhase] = (fplayer->realspeed);
    
    if (RecoveredFromStall)
       {
       for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
          {
          SpeedHistoryArray[HistoryArrayPointer] = fplayer->StallSpeed;
          RecoveredFromStall = false;
          }
       }
    
    if (ClearSpeedHistoryArrayFlag)
       {
       for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
          {
          SpeedHistoryArray[HistoryArrayPointer] = 0.0;
          ClearSpeedHistoryArrayFlag = false;
          }
       }
    
    for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
        {
        SpeedHistoryAccumulator += SpeedHistoryArray[HistoryArrayPointer];
        }
    
    InertiallyDampenedPlayerSpeed =  SpeedHistoryAccumulator / 10; 
    
    if (InertiaTimer > (int)(10000.0 * inertia))
        {
        InertiaTimer = 0;
        }

    if (!fplayer->ai)
       {
       TestAltitudeAirDensity();
       forcex /= AirDensityDrag;
       forcey /= AirDensityDrag;
       forcez /= AirDensityDrag;
       
       TestForExcessGamma();
       forcex /= GammaDrag;
       forcey /= GammaDrag;
       forcez /= GammaDrag;
       
       TestForLoopBeyondVertical();
       forcex /= LoopedBeyondVerticalDrag;
       forcey /= LoopedBeyondVerticalDrag;
       forcez /= LoopedBeyondVerticalDrag;
       TestDiveSpeedStructuralLimit();
       forcex /= SpeedBeyondStructuralLimitsDrag;
       forcey /= SpeedBeyondStructuralLimitsDrag;
       forcez /= SpeedBeyondStructuralLimitsDrag;
       TestForFlapEffects();
       forcex /= FlapDrag;
       forcey /= FlapDrag;
       forcez /= FlapDrag;
       TestForUndercarriageEffects();
       forcex /= UndercarriageDrag;
       forcey /= UndercarriageDrag;
       forcez /= UndercarriageDrag;
       TestForSpeedBrakeDrag();
       forcex /= SpeedBrakeDrag;
       forcey /= SpeedBrakeDrag;
       forcez /= SpeedBrakeDrag;
       RegulateXYZForces(forcex, forcey, forcez);
       forcex = RegulatedForceX;
       forcey = RegulatedForceY;
       forcez = RegulatedForceZ;
       RegulateGamma();
       
       }
    RealSpeedTemp2 = ((forcex * forcex) + (forcez * forcez) + (forcey * forcey));
    if (RealSpeedTemp2 == 0)
      {
      RealSpeedTemp2 = 0.001;
      }
    realspeed = sqrt (RealSpeedTemp2);
    if (!fplayer->ai)
        {
        
        realspeed = (realspeed + InertiallyDampenedPlayerSpeed)/2;
        TestForWindNoise();
        }
    // objects moving on the ground should always change their elevation due to the surface
    if (id >= TANK1 && id <= TANK2 && thrust > 0 && !stop)
        {
        float newy = l->getExactHeight (tl->x, tl->z) + zoom / 2;
        float dy = newy - tl->y + forcey;
        float dx = fabs (forcex) + fabs (forcez);
        float gamma2 = 0;
        if (fabs (dx) > 0.0001)
            {
            gamma2 = atan (dy / dx);
            }
        gamma = 180.0 + 180.0 / M_PI * gamma2;
        tl->y = newy;
        }
        // set angles to correctly display the object
        rot->setAngles ((short) (90 + gamma - 180), (short) theta + 180, (short) -phi);
cannondone:
    ;
    if (ttl > 0)
        {
        ttl -= dt; 
        if (ttl <= 0)
            {
            ttl = -1;
            if (id >= MISSILE1 && id <= MISSILE2)
                {
                recheight = -10;    // missiles drop
                }
            else
                {
                deactivate ();    // cannon shots vanish
                }
            }
        }

    checkDurability (); // check Durability issues
    crashGround (dt); // check ground collision
    if (immunity > 0)
        {
        immunity -= dt;    
        }
    } 

void DynamicObj::setBlackSmoke (float maxzoom, int len)
    {
    int i;
    for (i = 0; i < maxblacksmoke; i ++) // search a free blacksmoke instance
        if (blacksmoke [i]->ttl <= 0)
            {
            break;
            }
    if (i >= maxblacksmoke)
        {
        i = 0;
        }
    blacksmoke [i]->setBlackSmoke (tl->x, tl->y, tl->z, phi, maxzoom, len);
    } 

void DynamicObj::setExplosion (float maxzoom, int len)
    {
    int i;
    for (i = 0; i < maxexplosion; i ++) // search a free explosion instance
        if (explosion [i]->ttl <= 0)
            {
            break;
            }
    if (i >= maxexplosion)
        {
        i = 0;
        }
    if (id >= CANNON1 && id <= CANNON2)
       { 
       
       explosion [i]->setExplosion (tl->x, tl->y+1.0, tl->z, forcex, forcey, forcez, maxzoom/2, len);
       }
    else if (id == MISSILE_DF1)
       { 
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, tl->y * 0.4, len);
       }
    else if (id == BOMB01)
       { 
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, tl->y * 0.9, len);
       }
    else if (id == STATIC_AIRFIELD00)
       {
       ;
       }
    else
       {
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, maxzoom, len);
       }
    } 

void DynamicObj::thrustDown ()
    {
    recthrust -= maxthrust / 12;
    if (recthrust < maxthrust / 2)
        {
        recthrust = maxthrust / 2;
        }
    }

void DynamicObj::thrustUp ()
    {
    recthrust += maxthrust / 12;
    if (recthrust > maxthrust)
        {
        recthrust = maxthrust;
        }
    }
