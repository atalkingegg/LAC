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

#ifndef IS_AIOBJECT_H
#define IS_AIOBJECT_H

// #include "common.h" // ok
// #include "model.h" // ok
// #include "effects.h" // ok

// id values of objects
// non-AI air objects
#define CANNON1 0
#define FLARE1 80
#define CHAFF1 85
#define CANNON2 99
#define FLAPSDN 86
// missiles
#define MISSILE1 100
#define BOMB01 100
#define MISSILE_AIR2 101
#define MISSILE_AIR3 102
#define MISSILE_GROUND1 103
#define MISSILE_GROUND2 104
#define MISSILE_DF1 105
#define MISSILE_FF1 106
#define MISSILE_FF2 107
#define MISSILE_MINE1 180
#define MISSILE2 199
// air units
#define AIR 200
// FIGHTER1 has lowest enumerated value among fighters. Used for boundary checks.
#define FIGHTER1          200
#define FIGHTER_HAWK      201
#define FIGHTER_FIATG55   202
#define FIGHTER_ME109G    203
#define FIGHTER_P40       204
#define FIGHTER_HURRICANE 205
#define FIGHTER_KI43      206
#define FIGHTER_SPIT9     207
#define FIGHTER_IL16      208
#define BOMBER_JU87       209
#define FIGHTER_A6M2      210
#define FIGHTER_F4U       211
#define FIGHTER_F6F       212
#define FIGHTER_P38L      213
#define FIGHTER_P47D      214
#define FIGHTER_P51D      215
#define BOMBER_B17        216
#define FIGHTER_FW190     217
#define FIGHTER_YAK9      218
#define FIGHTER_N1K1      219
#define BOMBER_B24        220
#define FIGHTER_P39       221
#define BOMBER_G5M        222
#define BOMBER_B25        223
#define BOMBER_B26        224
#define FIGHTER_F4F       225
#define FIGHTER_LA5       226
#define FIGHTER_LA7       227
#define FIGHTER_IL2       228
#define FIGHTER_MACCIC202 229
#define BOMBER_LANCASTER  230
#define BOMBER_MOSQUITOB  231
#define FIGHTER_TYPHOON   232
#define FIGHTER_YAK1      233
#define BOMBER_B29        234
#define FIGHTER_DW520     235
#define BOMBER_SB2C       236
#define BOMBER_TBF        237
#define FIGHTER_ME163     238
#define FIGHTER_TEMPEST   239
#define FIGHTER_D3A       240
#define BOMBER_B5N        241
#define BOMBER_DAUNTLESS  242
#define FIGHTER_ME110     243
#define BOMBER_DORNIER    244
#define BOMBER_HE111      245
#define BOMBER_JU88       246
#define FIGHTER_KI84      247
#define FIGHTER_KI61      248

#define FIGHTER_GENERIC01 249
#define FIGHTER_A6M5      250
#define FIGHTER_SPIT5     251
#define FIGHTER_P51B      252
#define FIGHTER_P47B      253
#define FIGHTER_ME109F    254
#define FIGHTER_P38F      255

// FIGHTER2 has highest enumerated value among fighters. Used for boundary checks.
#define FIGHTER2          299

/*
* The next two lines set the boundaries for aircraft that
* emit a trail of smoke, using the enumerations set forth
* immediately above:
*/
#define FIRSTJETFIGHTER 200
#define LASTJETFIGHTER  201

// moving ground units from here
#define MOVING_GROUND 500
#define TANK1 700
#define TANK_AIR1 700
#define TANK_GROUND1 710
#define TANK_TRSAM1 711
#define TANK_PICKUP1 780
#define TANK_TRUCK1 790
#define TANK_TRUCK2 791
#define TANK2 799
// moving water units from here
#define MOVING_WATER 800
#define SHIP1 800
#define SHIP_CRUISER 800
#define SHIP_DESTROYER1 810
#define SHIP2 899
// static ground units from here
#define STATIC_GROUND 1000
#define FLAK1 1000
#define FLAK_AIR1 1000
#define FLARAK_AIR1 1010
#define FLAK2 1099
// passive static units from here
#define STATIC_PASSIVE 10000
#define STATIC_CONTAINER1 10100
#define STATIC_BATTLESHIP 10200
#define STATIC_HALL2 10201
#define STATIC_OILRIG1 10300
#define STATIC_COMPLEX1 10301
#define STATIC_RADAR1 10302
#define STATIC_BASE1 10303
#define STATIC_DEPOT1 10304
#define STATIC_BARRIER1 10400
#define STATIC_AIRFIELD00 10500
#define STATIC_RADARREFLECTOR 10510

/********************************************
*
* class DynamicObj : public CSpaceObj
*
* This data structure contains all of the variables
* that determine the behavior and performance of
* aircraft and other active objects.
*
********************************************/
class DynamicObj : public CSpaceObj
{
  public:
  int id; // object type: FLARAK_AIR1, STATIC_TENT1, FIGHTER_SWALLOW, ...
  bool active; // deactivated means no AI, no collision control and so on
  bool killed; // statistics
  bool LoopedBeyondVertical = false;
  bool WepCapable = true;
  unsigned char HistoricPeriod = 2; 
  unsigned char MyMissionPlayerNumber = 0; 
  unsigned char Sentient = 0; 
  int ttl; // time to live: cannon and missiles will only live a short time, missiles will sink when ttl<=0
  int InertiaTimer = 0;
  int immunity; // immunity means the object cannot collide with others, needed for shooting missiles/cannon
  Uint32 LoopedBeyondVerticalTime = 0;
  Uint32 SpeedBeyondStructuralLimitsTimer = 0;
  float AirDensityDrag = 1.0;
  float FlapDrag = 1.0;
  float impact; // this value will be subtracted from the other objects Durability when colliding
  // Imagine a carthesian coordinate system in the landscape, the y-axis pointing up
  float phi; // angle in x-z plane (polar coordinates)
  float gamma; // orthogonal angle (polar coordinates)
  float GammaDrag = 1.0; // Drag associated with climb angles beyond fplayer->MaxGamma.
  float LoopedBeyondVerticalDrag = 1.0; // Drag asserted after cresting a vertical loop until nose drops level.
  float SpeedBrakeDrag = 1.0;
  float theta; // roll angle of the fighter!
  float thrust; // current thrust, not the speed itself!
  float UndercarriageDrag = 1.0;
  float inertia; // Used in calculation of inertial effects. Relative to P38. 1.0 = same as p38.
  float deadweight; // Used in calculating climb rate.
  float realspeed; // the current speed, we only want to calculate this once per time step
  float forcex, forcez, forcey; // the force vectors (orthogonal, should be clear)
  float braking; // far away from reality: this factorizes the last speed vector with the current conditions (see move method)
  float manoeverability; // how fast a fighter can alter its direction
  float RollRate; // Roll Rate
  float maxthrust; // maximum throttle value
  float rectheta; // roll angle the fighter/object wants to reach
  float recthrust; // throttle the fighter/object wants to reach
  float recheight; // height above ground the fighter wants to reach
  float recgamma; // elevation the fighter wants to reach
  float maxtheta; // a maximum roll angle the object may achieve.
  float maxgamma; // Maximum sustainable climb angle without losing speed.
  float elevatoreffect; // number between 1.0 and -0.5, as diving should be less
  float DragEffect; // Drag from flaps, speedbrake, and undercarriage
  float ruddereffect;
  float rolleffect;
  float recelevatoreffect; // number between 1.0 and -0.5, as diving should be less
  float recruddereffect;
  float recrolleffect;
  float SpeedBeyondStructuralLimitsDrag = 1.0;
  float StaticDrag=1.0;  // Drag when aircraft is flying "clean"
  float BlackoutSensitivity; // Higher = more blackouts.
  float RedoutSensitivity; // Higher = more redouts.
  float FlapSpeed; // Maximum speed for which flaps can be lowered.
  float StallSpeed = 0.11; // Minimum speed for flight at sea level without flaps
  float SpeedBrakePower = 12.0;
  float MaxFullPowerAltRatio = 0.6; // Engines decrease above this fraction of MAXALTITUDE
  float CompressibilitySpeed = 0.38; // Above this speed, controls get stiff or lock up.
  float CompressibilitySpeedWithSpeedBrakes = 0.38; // Some planes enter compressibility at a different speed when speedbrakes are active.
  float DiveSpeedLimitThreshold = 0.35; // Faster than this is not wise.
  float DiveSpeedLimit1 = 0.40; // Faster than this is usually fatal.
  float DiveSpeedStructuralLimit = 0.50; // Faster than this will rip off the wings.
  float SpeedHistoryArray[10] = {0.11}; // This aircraft's speed during 10 prior Inertia periods.
  float InertiallyDampenedPlayerSpeed;
  float ServiceCeilingAltitude = 700; // Absolute maximum altitude this aircraft can achieve
  int FlapsLevelElevatorEffect0=0;
  int FlapsLevelElevatorEffect1=800;
  int FlapsLevelElevatorEffect2=1600;
  int FlapsLevelElevatorEffect3=3200;
  int FlapsLevelElevatorEffect4=6400;
  int party; // usually 0 for enemies, 1 for alliedsCompressibilitySpeedWithSpeedBrakes = 0.38
  int points, fighterkills, shipkills, tankkills, otherkills; // statistics
  int sink; // ships will not explode but sink
  Space *space; // in which space is this object, there is only one ;-)
  DynamicObj *source; // missiles must keep track of the object they have been fired from -> statistics
  int bomber; // act as bomber and prefer ground targets
  char net [100];
  int realism = physics;
  float accx, accy, accz;
  float Durability, maxDurability; // current and initial/maximum Durability
  float DamageInNetQueue; // Damage accumulated on network peer between packets
  void activate ();
  void deactivate ();
  void dinit ();
  DynamicObj ();
  DynamicObj (Space *space2, CModel *o2, float zoom2);
  void thrustUp ();
  void thrustDown ();
  float distance (DynamicObj *target);
  float distanceXZ (DynamicObj *target);
  float ClipDistance; // Objects closer than this are not displayed.
  // check whether the object is exploding or sinking and deactivate if necessary
  void checkExplosion (Uint32 dt);
  // check the objects Durability value and explode/sink if necessary
  void checkDurability ();
  // check whether the object collides on the ground and alter gamma and y-translation
  void crashGround (Uint32 dt);
  // check for collision, simplified model, each model is surrounded by a cube
  // this works pretty well, but we must use more than one model for complex models or scenes
  void collide (DynamicObj *d, Uint32 dt); // d must be the medium (MachineGunBullet, missile)
  void setExplosion (float maxzoom, int len);
  void setBlackSmoke (float maxzoom, int len);
  // return heading difference towards enemy
  int getAngle (DynamicObj *o);
  // return elevation difference towards enemy
  int getAngleH (DynamicObj *o);
  // check for a looping, this is tricky :-)
  bool checkLooping ();
  void move (Uint32 dt);
};

const int missiletypes = 8;
const int missileracks = 6;

class AIObj : public DynamicObj
{
  protected:
  public:
  bool ai; // AI on/off
  bool autofire; // cannon fire on/off
  DynamicObj *target; // targeted object
  int acttype; // object is doing some action (Immelmann, Loop, ... not yet implemented)
  // three intellience characteristics which make up a pilot: 0 = best, 400 = worst
  int intelligence; // valid for every AI object: manoevers, fire rate (tanks), ...
  int aggressivity; // valid for fighters: fly low, stay near and behind enemy
  int precision; // valid for fighters: heading calculation
  // manoevers disable any other AI consideration
  int manoevertheta, manoeverheight, manoeverthrust;
  int idle; // counter how long AI object does the same thing (to change direction)
  int firemissilettl; // minimum time to wait between shooting missiles
  int firecannonttl; // minimum time to wait between shooting cannon
  int FlapsLevel = 0; // From 0 (no flaps) to 4 (max flaps)
  int smokettl; // minimum time to wait between setting smoke elements
  int SpeedBrake = 0; // 0 = off, 1= Speedbrake is active.
  int missiletype; // only relevant for the player, describes type: AAM, AGM, DF
  int missiles [missiletypes]; // number of missiles of each type
  int missilerack [missileracks]; // number of missile racks
  int missilerackn [missileracks]; // number of missile racks
  int UndercarriageLevel = 0; // Landing Gear. 0 = up. 1 = Down.
  int WEPLevel = 0; // War Emergency Power (Afterburner). 0 or 1.
  float aw; // current heading difference to target
  float dtheta, dgamma; // theta/gamma alteration (smooth piloting)
  float disttarget; // current distance to target
  float FuelLevel = 100; // Percent, from 0 to 100
  float CompressibilityLevel = 0.1; // How far into compressibility are we? (Initialize to low level.)
  int flares;
  int chaffs;
  int fireflarettl;
  int firechaffttl;
  int ammo;
  int ttf; // time to fire missile, targeting mechanism
  CSmoke *smoke; // bright smoke behind the object (fighter&missiles)
  Uint32 timer;
  float statLethality; // Lethality of offensive weapons
  int DefensiveLethality; 
  bool dualshot; // one or two cannons?
  int manoeverstate; // changes to realistic manoevers.

  void aiinit (); // initialize variables
  void missileCount ();
  void newinit (int id, int party, int intelligence, int precision, int aggressivity); // init new AI object
  void newinit (int id, int party, int intelligence); // init new AI object (esp. non-fighter)
  AIObj ();
  AIObj (Space *space2, CModel *o2, float zoom2);
  ~AIObj ();
  void initValues (DynamicObj *dobj, float phi); // init values to shoot cannon or missile
  void fireCannon (DynamicObj *MachineGunBullet, float phi);
  void fireCannon (DynamicObj **MachineGunBullet, float phi);
  void fireCannon (DynamicObj **MachineGunBullet);
  void fireMissile2 (int id, AIObj *missile, AIObj *target);
  void fireFlare2 (DynamicObj *flare);
  void fireChaff2 (DynamicObj *chaff);
  int firstMissile (); // select first missile type
  int nextMissile (int from); // select next missile type (cyclic)
  bool haveMissile (int id); // missile of type id left?
  bool haveMissile (); // missile of type missiletype left?
  void decreaseMissile (int id); // decrease missiles by one
  bool fireMissile (int id, AIObj **missile, AIObj *target);
  bool fireMissile (AIObj **missile, AIObj *target);
  bool fireMissile (int id, AIObj **missile);
  bool fireMissile (AIObj **missile);
  bool fireFlare (DynamicObj **flare, AIObj **missile);
  bool fireChaff (DynamicObj **chaff, AIObj **missile);
  bool fireMissileAir (AIObj **missile, AIObj *target);
  bool selectMissileAir (AIObj **missile);
  bool fireMissileAirFF (AIObj **missile, AIObj *target);
  bool selectMissileAirFF (AIObj **missile);
  bool fireMissileGround (AIObj **missile);
  bool selectMissileGround (AIObj **missile);
  void targetNearestGroundEnemy (AIObj **f);
  void targetNearestEnemy (AIObj **f);
  void targetNextEnemy (AIObj **f);
  void targetLockingEnemy (AIObj **f);
  void targetNext (AIObj **f);
  void targetPrevious (AIObj **f);
  void aiAction (Uint32 dt, AIObj **f, AIObj **m, DynamicObj **c, DynamicObj **flare, DynamicObj **chaff); // core AI method
};

#endif
