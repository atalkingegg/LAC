/*
    LAC (Linux Air combat)
    Copyright 2015 by Robert J. Bosen.

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

#include "main.h"
#include "glland.h"
#include <ctype.h>

// AIObj pointer global variables:
extern AIObj *fplayer;

// Other global variables:
extern char CurrentMissionNumber;

extern unsigned char MissionOutPacketCount;
extern unsigned char MyNetworkId;
extern unsigned char Realm;

/*
*
* Define the UDP Packet format used for our Network API data exchanges:
*
*/
struct LacUdpApiPacket
{
unsigned char NetworkApiVersion;    // Originally 00. Incremented each time packet format changed.
unsigned char UdpObjCommand;        // For future expansion.
unsigned char UdpObjMissionId;      // Mission to which this packet pertains. (Helps servers route packets)
unsigned char UdpObjRealm;          // For future expansion.
unsigned char UdpObjTerrainOrMorse; // Terrain number as defined in glland.h. Defaults to LANDSCAPE_SEA
unsigned char UdpObjPlayerNumber;   // Identifies current player ID within mission. Typically a small integer between 0 and 10.
unsigned char UdpObjDamageId;       // Identifies aircraft within current mission that was damaged by amount in UdpObjDamageAmount field.
unsigned char UdpObjFlightDetails;  // Bit-coded with settings for flaps, brakes, GunCamera, flare, chaff/ECM, gun, missile.
Uint16        UdpObjVehicle;        // Indicates the type of aircraft flown by UdpObj. Defaults to SelectedAircraft as used in main.cpp.
float  UdpObjXPosition;             // x position in xyz space within Terrain. Defaults to fplayer->tl->x
float  UdpObjYPosition;             // y position in xyz xpace within Terrain. Defaults to fplayer->tl->y
float  UdpObjZPosition;             // z position in xyz space within Terrain. Defaults to fplayer->tl->z
float  UdpObjGamma;                 // Climb/Descend angle of aircraft flown by UdpObj. Defaults to fplayer->gamma
float  UdpObjPhi;                   // Compass heading of aircraft flown by UdpObj. Defaults to fplayer->phi
float  UdpObjTheta;                 // Roll angle of aircraft flown by UdpObj. Defaults to fplayer->theta
float  UdpObjSpeed;                 // Velocity of aircraft flown by UdpObj. Defaults to fplayer->realspeed
float  UdpObjThrust;                // Throttle setting of aircraft flown by UdpObj. Defaults to fplayer->thrust
float  UdpObjElevator;              // Elevator setting of aircraft flown by UdpObj. Defaults to fplayer->elevatoreffect
float  UdpObjAileron;               // Aileron setting of aircraft flown by UdpObj. Defaults to fplayer->rolleffect
float  UdpObjRudder;                // Rudder setting of aircraft flown by UdpObj. Defaults to fplayer->ruddereffect
float  UdpObjDamageAmount;          // Amount of damage % applied in claim. Defaults to 0.
Uint16 UdpObjHash;                  // Packet signature.
};

struct Admin012Packet
{
unsigned char NetworkApiVersion;                      
unsigned char UdpObjCommand = 12;                     
unsigned char UdpObjMissionOutPacketCount = MissionOutPacketCount;
unsigned char UdpObjMyNetworkId = MyNetworkId;
unsigned char UdpObjRealm = Realm;
char          UdpObjCurrentMissionNumber = CurrentMissionNumber;
char          UdpObjLacVersion[32];   
char          UdpObjCommunityHandle[16] = {"00000000000000"}; 
};

struct Admin013Packet
{
unsigned char NetworkApiVersion = 8;
unsigned char UdpObjCommand = 13;
unsigned char UdpObjMissionId = 0;
unsigned char UdpObjRealm = 0;

unsigned char UdpObjMissionOutPacketCount01 = 0;
char          UdpObjLacVersion01[32] = {"Lac00p00"};
char          UdpObjCommunityHandle01[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount02 = 0;
char          UdpObjLacVersion02[32] = {"Lac00p00"};
char          UdpObjCommunityHandle02[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount03 = 0;
char          UdpObjLacVersion03[32] = {"Lac00p00"};
char          UdpObjCommunityHandle03[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount04 = 0;
char          UdpObjLacVersion04[32] = {"Lac00p00"};
char          UdpObjCommunityHandle04[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount05 = 0;
char          UdpObjLacVersion05[32] = {"Lac00p00"};
char          UdpObjCommunityHandle05[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount06 = 0;
char          UdpObjLacVersion06[32] = {"Lac00p00"};
char          UdpObjCommunityHandle06[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount07 = 0;
char          UdpObjLacVersion07[32] = {"Lac00p00"};
char          UdpObjCommunityHandle07[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount08 = 0;
char          UdpObjLacVersion08[32] = {"Lac00p00"};
char          UdpObjCommunityHandle08[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount09 = 0;
char          UdpObjLacVersion09[32] = {"Lac00p00"};
char          UdpObjCommunityHandle09[16] = {"00000000000000"};

unsigned char UdpObjMissionOutPacketCount10 = 0;
char          UdpObjLacVersion010[32] = {"Lac00p00"};
char          UdpObjCommunityHandle10[16] = {"00000000000000"};
}; 

struct Admin014Packet
{
unsigned char NetworkApiVersion;      
unsigned char UdpObjCommand = 14;     
unsigned char UdpObjRealm = Realm;
unsigned char UdpObjMissionId;
unsigned char DestinationPlayerNumber;
char          MessageText[80];
}; 

