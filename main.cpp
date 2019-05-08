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

#include "cockpit.h"
#include "common.h"
#include "conf.h"
#include "dirs.h"
#include "gl.h"
#include "glland.h"
#include "land.h"
#include "main.h"
#include "mathtab.h"
#include "menu.h"
#include "mission.h"
#include "NetworkApi.h"

//
// This simulator uses a lot of global variables.
// Throughout LAC, the vast majority of these global
// variable definitions can be found in one of three
// locations follows:
//
// 1 of 3: Most global variables are defined here,
// at the top of "main.cpp".
//
// 2 of 3: global variables relating to keyboard,
// joystick, or other reconfigurable items are defined
// in a block of similar code at the top of "conf.cpp".
//
// 3 of 3: global variables related to network code
// are defined at the top of "NetworkApi.cpp".
//
// Within those three files, global variables are stored
// in alphanumeric order by variable type.
//
// LAC's extensive use of global variables results from
// its need to code for speed at any price: global
// variables can be accessed immediately and arbitrarily,
// from anywhere in the program logic and at any time.
// Furthermore, this kind of direct, global access is
// faster than the usual c++ methods (which rely upon
// pushing values onto the stack in the proper sequence,
// and then popping them back off of the stack in the
// reverse sequence).
//
// However, with so many global variables, it is essential
// that they be given clear, globally unique names for
// reference, and that they be well organized so that they
// can be easily found by programmers. Accordingly,
// All of LAC's global variables are sorted by
// data type (whether a standard c++ data type
// or some new type built up from the fundamental
// data types for use by this specific project),
// and all are then further sorted into strict
// alphanumeric order within those groups.
//
// In general, pointers to variables of a specific
// data type are listed immediately after variables
// of the same data type, and arrays of such variables
// also follow variables of the same data type.
//
// LAC's global variable names tend to be long, and
// have been carefully chosen for clarity. In general,
// the intended use of every global variable is made
// clear by careful consideration of its name.
//
// This arrangement should help programmers to more
// easily locate and use global variables appropriately,
// and with maximum speed where it's needed.
//

// global const variables:
const int maxfx = 256;
const int maxfy = 64;

// global boolean variables:
bool AirfieldRepairRateNormalForBlueTeam = false;
bool AirfieldRepairRateNormalForRedTeam = false;
bool AirfieldRepairsAcceleratedForBlueTeam = false;
bool AirfieldRepairsAcceleratedForRedTeam = false;
bool AirfieldRepairsStoppedForBlueTeam = false;
bool AirfieldRepairsStoppedForRedTeam = false;
bool AutoPilotActive = false;
bool CapsLockActive = false; 
bool ChaffPacketFired = false; 
bool ClearSpeedHistoryArrayFlag = false; 
bool ClimbAngleExceedsMaxSustainable = false;
bool ConfigInit = false; 
bool EnterPressed = false; 
bool FirstStart = false;
bool FlareFired = false; 
bool GearUpDamageAlreadyInflicted = false;
bool GunCameraActive = false; 
bool GunCameraStartSegmentFlag = false; 
bool GunCameraStopSegmentFlag = false;  
bool initing = true;
bool InPacketContainsMumbleBroadcast  = false;
bool InPacketContainsMumbleNarrowcast = false;
bool JoystickFireButton = false;
bool LandedAtSafeSpeed = false; 
bool LandingLogged = false; 
bool OnTheGround = false;
bool MissileFired = false; 
bool MissionActive = false;
bool MissionRunning; 
bool MumbleBroadcastRequested = false; 
bool MumbleChannelExitRequested = false;
bool MumbleChannelMainRequested = false;
bool MumbleCurrentMissionBlueSubChannelRequested = false; 
bool MumbleCurrentMissionChannelRequested = false; 
bool MumbleCurrentMissionRedSubChannelRequested  = false; 
bool MumbleTunedToCurrentMissionChannel = false; 
bool MumbleTunedToMyBlueMissionAndSubchannel = false; 
bool MumbleTunedToMyRedMissionAndSubchannel  = false; 
bool MumbleTransmissionRequested = false; 
bool NetworkMissionIsActiveWithDataFlow = false; 
bool NetworkPlayerKilled = false; 
bool NewSystemMessageNeedsScrolling = false; 
bool NoMissionHasYetCommenced = true; 
bool PeerPacketReceivedInThisMissionState = false; 
bool RadarReflectorBlueHasBeenDestroyedAtLeastOnce; 
bool RadarReflectorRedHasBeenDestroyedAtLeastOnce; 
bool RadarTargetColorLogged = false; 
bool RecoveredFromStall = false;  
bool ShotDownByBomberOrAirfieldGunners = false; 
bool SpaceBarPressed = false; 
bool SpeedBrakeRequested = false;
bool StartCannon = false;
bool SunBlinding = false;
bool TakeoffLogged = false; 
bool TargetSelectNotYetInitialized = true;
bool UdpSetupCompleted = false;
bool WindNoiseOn = false;

// global boolean arrays:
bool MorseSquelches[10] = {false}; 

// global unsigned char variables:
unsigned char AirfieldChosenForLanding = 0;     
unsigned char AirfieldRequested = 1;            
unsigned char BattleshipSunkAlertCountBlue = 0; 
unsigned char BattleshipSunkAlertCountRed = 0;  
unsigned char DefaultAircraft = FIGHTER_P38L;   
unsigned char MaxPlayersInCurrentMission = 10;  
unsigned char MissionHeadToHead00State = 0;     
unsigned char MissionIdNegotiationCount = 0;    
unsigned char MissionOutPacketCount = 0;        
unsigned char MissionStateNetworkBattle;      
unsigned char MorseCharacterIn = 0;             
unsigned char MumbleChannelPresumed = 0;        
unsigned char MyNetworkId = 1;                  
unsigned char NetworkApiVersion = 9;            
unsigned char NetworkOpponent = 0;              
unsigned char NewestInPacketId = 0;             
unsigned char NewestMumblePacketId = 0;         
unsigned char PreviousMorseSender = 1;          
unsigned char Realm = 0;                        
unsigned char RecentVictim = 0;                 
unsigned char SelectedMissionTarget = 0;        
unsigned char ServerIpByte1of4 = 192;           
unsigned char ServerIpByte2of4 = 168;
unsigned char ServerIpByte3of4 = 1;
unsigned char ServerIpByte4of4 = 2;
unsigned char ServerMessageRecipient = 255;     

// global char array variables (single-dimensional):
char CommsMacro01String1[64] = "CHANNEL-M1-ALL";
char CommsMacro02String1[64] = "CHANNEL-M1-BLUE";
char CommsMacro03String1[64] = "CHANNEL-M1-RED";
char CommsMacro04String1[64] = "CHANNEL-M2-ALL";
char CommsMacro05String1[64] = "CHANNEL-M2-BLUE";
char CommsMacro06String1[64] = "CHANNEL-M2-RED";
char CommsMacro07String1[64] = "CHANNEL-M3-ALL";
char CommsMacro08String1[64] = "CHANNEL-M3-BLUE";
char CommsMacro09String1[64] = "CHANNEL-M3-RED";
char CommsMacro10String1[64] = "CHANNEL-ROOT";
char CommsMacro11String1[64] = "PTT-NARROWCASTING";
char CommsMacro12String1[64] = "PTT-BROADCASTING";
char CommsMacro13String1[64] = "RADIO-VOLUME-INCREASED-5-PERCENT";
char CommsMacro14String1[64] = "RADIO-VOLUME-DECREASED-5-PERCENT";
char CommsMacro15String1[64] = "RADIO-MIC-TOGGLED";
char CommsMacro16String1[64] = "RADIO-SOUND-TOGGLED";
char CommunityHandle [20] = "00000000000000"; 
char ControlsMenu0Names [37] [37]; 
char CurrentMissionNumber; 
char DebugBuf[1100]; 
char MainMenuNames [7] [50]; 
char MissionEntryLatch [32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
char MorseRadioMessageBuffer [64] = "";
char NetworkPassword [32] = "password"; 
char NetworkPlayerMumbleChannel[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
char OptionsMenuNames [5] [25]; 
char RemoteDnsIpAddress[100] = "LacServer2.LinuxAirCombat.com"; 
char RemoteDnsIpAddressHost[32] = "LacServer2";
char RemoteDnsIpAddressSecondLevelDomain[32] = "LinuxAirCombat";
char RemoteDnsIpAddressTopLevelDomain[32] = "com";
char ServerIpAddressAscii[16] = "000.000.000.000";
char ServerMessageBuffer[81] = "";
char submenu1names [20] [50]; 
char SystemMessageBuffer0 [64] = "____________________________________________________";
char SystemMessageBuffer1 [64] = "____________________________________________________";
char SystemMessageBuffer2 [64] = "____________________________________________________";
char SystemMessageBufferA [64] = "____________________________________________________";
char TargetVocalize1State = 0;
char TargetVocalize2State = 0;

char *FileSystemHomeDir = getenv("HOME"); 
char *FileSystemDefaultHeightMapFileName = (char*)"/.LAC/DefaultHeightMap.LAC";
char *FileSystemGunCamHistoryFileName = (char*)"/.LAC/GunCamHistory.LAC";
char *FileSystemLastTerrainFileName = (char*)"/.LAC/LastTerrain.LAC";
char *FileSystemOnlineLineScoreLogFileName = (char*)"/.LAC/OnlineScoreLog.LAC";
char  FileSystemDefaultHeightMapFilePath [512]; 
char  FileSystemGunCamHistoryFilePath [512];    
char  FileSystemLastTerrainFilePath [512];      
char  FileSystemOnlineScoreLogFilePath [512];   

// global char array variables (two-dimensional):
char PlayerIdStrings[10][9] =
{ 
    {"PLAYER01"},
    {"PLAYER02"},
    {"PLAYER03"},
    {"PLAYER04"},
    {"PLAYER05"},
    {"PLAYER06"},
    {"PLAYER07"},
    {"PLAYER08"},
    {"PLAYER09"},
    {"PLAYER10"},
};

// global unsigned char array variables:
unsigned char MorseCharacterOut[16] = {0};            
unsigned char firetex [maxfx * maxfy * 4];

// global integer variables:
int AirfieldXMax = 219;    
int AirfieldXMin = 161;    
int AirfieldYMax = 10;     
int AirfieldYMin = 3;      
int BattleDamageRiskTimer = 0;
int brightness = 0;
int camera = 0;
int clouds = 0;
int contrast = 10; 
int CreditsTimer;
int day = 1;
int debug = 1;
int DebugLevel = LOG_MOST;
int DoneFiringCannon=0;
int FinishTimer;
int flash = 0;
int FlapsLevelElevatorEffectNow=0;
int FlightIsInverted=0; 
int frames = 0;
int game = GAME_INIT;
int HudLadderBarsOnOff = 0; 
int HudOnOff=1;
int IffOnOff = 0; 
int InertiaTimer = 0; 
int initsynchrotimer = 0;
int inittimer = 0;
int joybutton = 0;
int JoystickButton = -1;
int joysticks;
int keyb_elev = 0;
int keyb_roll = 0;
int keyb_rudder = 0;
int LacTimer = 0;
int lastDurability = 0;
int MapEastWestScroll = 0; 
int MapNorthSouthScroll = 0; 
int MapViewOnOff = 0; 
int MapZoomFactor = 0; 
int MenuTimer = 0;
int MissionAircraftDamaged = 0; 
int MissionEnding = 0;
int MissionMenuTimer;
int MissionNumber = 0; 
int missionstate = 0;
int MouseControlFlag = 0; 
int mousex;
int mousey;
int MorsePanelHighlightTimer = 0; 
int NetworkTransmitTimer = 0; 
int NetworkTransmitTimerInterval = 200; 
int PlayersOriginalDurability; 
int PriorPeerId=0; 
int PriorStateEndTimer = 0; 
int PlayerAircraftType;
int PlayerEngineType=0; 
int RadarOnOff = 1; 
int RadarWasOn=0; 
int RadarZoom = 4; 
int sdldisplay = true;
int sdlreshape = true;
int SelectedAircraft = 1;
int ServerMessageTimer = 0; 
int showcollision = 0;
int SkewViewBack = 20;
int SkewViewLeft = 0;
int SkewViewRight = 10;
int SkewViewUp = 10;
int SkipCount = 8;  
int starttime;
int StateTransitionTimer = 0;
int StaticObjectUpdateTimer = 3900; 
int TargetVocalizeTimer1 = 0;
int TargetVocalizeTimer2 = 0;
int time1 = 0;
int time2 = 0;
int TrainingStartId;
int TrimElevatorSetting = 0;
int TrimRudderSetting = 0;
int vibration = 0;
int weather = WEATHER_SUNNY;
int WindNoiseLevel;

// global int16_t variables:
int16_t PriorServerMessageChecksum;
int16_t ServerMessageChecksumBuffer;

// global Unsigned integer variables:
unsigned int MissedPacketCount = 0;    
unsigned int MyInPortNumber = 51100;   
unsigned int NetworkReceiveTimer = 0;  
unsigned int NetworkReceiveTimerInterval = 100; 
unsigned int PeerInPortNumber = 51100; 

// global Unsigned 16-bit integer variables:
Uint16 OutGoingPacketSignature;    

// global Unsigned 32-bit integer variables:
Uint32 DeltaTime;
Uint32 LastTime = 0;
Uint32 Me163LandingTimer = 60000;           
Uint32 MissionEndingTimer = 0;              
Uint32 MissionEndingTimer2 = 0;             
Uint32 MissionNetworkBattle01Timer = 0;     
Uint32 MissionNetworkBattle02Timer = 0;     
Uint32 MissionNetworkBattle03Timer = 0;     
Uint32 MissionNetworkBattleRadarTimer = 0;  
Uint32 NetworkApiPriorTimer[32];            
Uint32 SpeedBrakeTimer;
Uint32 StallTimer;
Uint32 ViewResetTimer;                  

// global Long integer variables:
long CountdownTimer = 100; 

// global integer array variables:
int heat [maxfy] [maxfx];
int heat2 [maxfy] [maxfx];
int jaxis [maxjaxis * maxjoysticks];
int MissionNetworkBattle01PriorAircraft[10];
int MissionNetworkBattle02PriorAircraft[10];
int MissionNetworkBattle03PriorAircraft[10];
int MissionMumbleTimer[32]; 
int MissionPacketTimer[32]; 
int sdljoystickaxes [maxjoysticks];

// NOTE: DO NOT USE IN PRODUCTION, this should be dynamicly generated
// only ones matching display should ever be used
// Note: Xres, Yres, BPP (why?), fullscreen (why?)
// also, original table for CRTs, should now pull info from SDL display info.

#define NUM_SCREEN_RES 13
int resolution [NUM_SCREEN_RES] [4] =
    {
        {  640,   480,  32,  0 },
        {  800,   600,  32,  0 },
        {  1024,  600,  32,  0 },
        {  1024,  768,  32,  0 },
        {  1280,  800,  32,  0 },
        {  1280, 1024,  32,  0 },
        {  1440,  900,  32,  0 },
        {  1680, 1050,  32,  0 },
        {  1920, 1080,  32,  0 },
        {  1920, 1200,  32,  0 },
        {  2048, 1280,  32,  0 },
        {  2560, 1600,  32,  0 },
        {  2880, 1800,  32,  0 },
    };

// global Floating-point variables:
float AirDensityAtThisAltitude;
float AutoPilotAltitude = 0;
float blackout = 0;
float BlackoutSensitivity;
float CalculatedDamageDueToCurrentRisk = 0; 
float camphi = 0, camgamma = 0, camtheta = 0; 
float CamPhiTemp2   = 0; 
float camx = 0, camy = 0, camz = 0; 
float ChaseViewDistance = 0;
float cloudfog = 100; // Maximum view distance before fog obscures everything. 1 = about 400 meters.
float CompressibilityElevatorFactor = 1.0;
float CompressibilityRollFactor = 1.0;
float ConfiguredViewDistance; 
float DamageToClaim = 0.0; 
float dtheta = 0, dgamma = 0;
float fps = 30;
float FuelLevelAtLastLanding = 0;
float GammaDelta = 0;
float HitBubbleRadius = 0.125; 
float lastfps = -1;
float LoopedBeyondVerticalDrag = 1.0;
float MaxGammaPenaltyPerWeapon = 0.0; 
float MaxFullPowerAltitude;
float nearclippingplane = 0.026; 
float redout = 0;
float PlayersOriginalRollRate = 0; 
float PlayersOriginalMaxGamma = 0;  
float PlayersOriginalMaxThrust = 0; 
float PriorGamma = 0;
float PriorThetaNormalized = 5;
float RedoutSensitivity;
float RecentVictimAltitude = 0;  
float RecentVictimGamma = 0;     
float RecentVictimPhi = 0;       
float RecentVictimTheta = 0;     
float RecentVictimVelocity = 0;  
float RecentVictimXPosition = 0; 
float RecentVictimZPosition = 0; 
float ScreenFOVx = 1.0; 
float ScreenFOVy = 1.0; 
float ScreenFOVz = 1.0; 
float SeaLevel = -13.0; 
float StallRatio;       
float sungamma = 45.0;
float sunlight = 1.0;
float sunlight_dest = 1.0;
float ThetaDelta = 0;
float ThetaNormalized = 5;
float ThetaTemp3=0; 
float ThrustReadiness = 0; 
float TrueAirSpeed = 500; 
float Turbulence = 0; 
float TurbulenceFactor = 1.0; 
float view_x = 0; 
float view_y = 0; 
float VisibleAngle = 80.0F;

// global Floating-point array variables:
float NetDeltaX;
float NetDeltaY;
float NetDeltaZ;
float NetworkApiPriorXPosition[32]; 
float NetworkApiPriorYPosition[32]; 
float NetworkApiPriorZPosition[32]; 
float RegulatedForceX; 
float RegulatedForceY; 
float RegulatedForceZ; 

// global AIObj pointer variables:
AIObj *fplayer; 
AIObj *ThreeDObjects [maxfighter]; 
AIObj *missile [maxmissile]; 

// global Button pointer variables:
Button *mainbutton [10];
Button *optbutton [10];

// global CBlackSmoke pointer variables:
CBlackSmoke *blacksmoke [maxblacksmoke];

//global CColor variables:
CColor colorblue (50, 50, 255, 255);
CColor colorgreen (100, 255, 100, 255);
CColor colorred (255, 0, 0, 255);
CColor colorwhite (255, 255, 255, 255);
CColor coloryellow (255, 255, 0, 200);

// global CExplosion pointer variables:
CExplosion *explosion [maxexplosion];

//global CLoad3DS variables:
CLoad3DS g_Load3ds;

// global Cockpit pointer variables;
Cockpit *cockpit;

// global CModel variables:
CModel model_Airfield00;
CModel model_bomb01;
CModel model_cannon1;
CModel model_cannon1b;
CModel model_cannon2;
CModel model_cannon2b;
CModel model_chaff1;
CModel model_depot1;
CModel model_fig;
CModel model_figa;
CModel model_figb;
CModel model_figc;
CModel model_figi;
CModel model_figu;
CModel model_figv;
CModel model_figw;
CModel model_figx;
CModel model_figy;
CModel model_figz;
CModel model_figAA;
CModel model_figAB;
CModel model_figAC;
CModel model_figAD;
CModel model_figAE;
CModel model_figAF;
CModel model_figAG;
CModel model_figAH;
CModel model_figAI;
CModel model_figAJ;
CModel model_figAK;
CModel model_figAL;
CModel model_figAM;
CModel model_figAN;
CModel model_figAO;
CModel model_figAP;
CModel model_figAQ;
CModel model_figAR;
CModel model_figAS;
CModel model_figAT;
CModel model_figAU;
CModel model_figAV;
CModel model_figAW;
CModel model_figAX;
CModel model_figAY;
CModel model_figAZ;
CModel model_figBA;
CModel model_figBB;
CModel model_figBC;
CModel model_figBD;
CModel model_figBE;
CModel model_figBF;
CModel model_figBG;
CModel model_figBH;
CModel model_figBI;
CModel model_figBJ;
CModel model_figBK;
CModel model_figBL;
CModel model_figBM;
CModel model_figBN;
CModel model_figBO;
CModel model_figBP;
CModel model_figBQ;
CModel model_figBR;
CModel model_figBS;
CModel model_figBT;
CModel model_figBU;
CModel model_figBV;
CModel model_figBW;
CModel model_figBX;
CModel model_figBY;
CModel model_figBZ;
CModel model_figCA;
CModel model_figCB;
CModel model_figCC;
CModel model_figCD;
CModel model_figCE;
CModel model_figCF;
CModel model_flak1;
CModel model_flapsdn;
CModel model_flapsup;
CModel model_flare1;
CModel model_battleship;
CModel model_hall2;
CModel model_mine1;
CModel model_missile2;
CModel model_missile3;
CModel model_missile4;
CModel model_missile5;
CModel model_missile6;
CModel model_missile7;
CModel model_missile8;
CModel model_radar;
CModel model_RadarReflector;
CModel model_rubble1;
CModel model_tank1;
CModel model_tank2;
CModel model_tent1;

// global CModel pointer variables:
CModel *explsphere;
CModel *obj;
CModel *objmissile;

// global Component pointer variables:
Component *currentoptmenu = NULL;
Component *currentsubmenu = NULL;

// global Container variables:
Container allmenus;
Container fightermenu;
Container MissionMenu;
Container quitmenu;
Container statsmenu;

// global Container pointer variables:
Container *mainmenu;

// global Container array pointer variables:
Container *controlsmenu [5];
Container *optmenu [5];
Container *submenu [10];

// global CRotation pointer variables:
CRotation rot;
CRotation rot2;
CRotation *rotmissile;
CRotation *rotnull;

// global CSpaceObj pointer variables:
CSpaceObj *sphere;

// global CSphere pointer variables:
CSphere *objsphere;

// global CTexture pointer variables:
CTexture *texclouds1;
CTexture *texclouds2;
CTexture *texclouds3;
CTexture *texcross;
CTexture *texcross2;
CTexture *texflare1;
CTexture *texflare2;
CTexture *texflare3;
CTexture *texflare4;
CTexture *texmoon = NULL;
CTexture *texradar1;
CTexture *texradar2;
CTexture *texsun;
CTexture *textitle;

// global CVector variables:
CVector3 tl;
CVector3 tl2;

// global CVector3 pointer variables:
CVector3 *clip1;
CVector3 *clip2;
CVector3 *tlinf;
CVector3 *tlnull;
CVector3 *tlminf;

// global Dirs pointer variables:
Dirs *dirs;

// global DynamicObj pointer variables:
DynamicObj *chaff [maxchaff];
DynamicObj *flare [maxflare];
DynamicObj *groundobj [maxgroundobj];
DynamicObj *MachineGunBullet [maxMachineGunBullet];

// global FILE pointer variables:
FILE *GunCamHistoryFile;
FILE *OnlineScoreLogFile;

// global Flash pointer variables:
Flash *flash1;

// global Font pointer variables:
Font *font1;
Font *font2;

// NOTE: SDL2 window handles, GL contexts, and screen bounds
// TODO: convert these to dynamic display objects
#define MAXDISPLAYS 3
SDL_GLContext glcontext[MAXDISPLAYS];
SDL_Window *win[MAXDISPLAYS];
SDL_Rect bounds[MAXDISPLAYS];

// global GL pointer variables:
GL *gl;

// global GLenum variables:
GLenum polygonMode = GL_FILL;

// global GLfloat array variables:
GLfloat light_position0[] = { 0.0, 0.5, -1.0, 0 };

// global GLLandscape pointer variables:
GLLandscape *l = NULL;

// global HighClouds pointer variables:
HighClouds *highclouds;
HighClouds *highclouds2;

// global Mission pointer variables:
Mission *mission = 0;
Mission *missionnew = 0;

// global PilotList pointer variables:
PilotList *pilots;

// global SDL_Joystick pointer variables:
SDL_Joystick *sdljoystick [maxjoysticks];

// global SoundSystem pointer variables:
SoundSystem *sound = NULL;

// global Space pointer variables:
Space *space;

// global Star pointer array variables:
Star *star [maxstar];

// External Variables:
extern int joystick_DROPCHAFF;
extern int joystick_DROPFLARE;
extern int joystick_FIREPRIMARY;
extern int joystick_FIRESECONDARY;
extern int joystick_FlapsDN;
extern int joystick_FlapsUP;
extern int joystick_HudLadderBarsOnOff;
extern int joystick_HudOnOff;
extern int joystick_IffOnOff;
extern int joystick_LookBK;
extern int joystick_LookDN;
extern int joystick_LookFD;
extern int joystick_LookLT;
extern int joystick_LookRT;
extern int joystick_LookUP;
extern int joystick_MapScrollEast;
extern int joystick_MapScrollNorth;
extern int joystick_MapScrollSouth;
extern int joystick_MapScrollWest;
extern int joystick_MapViewOnOff;
extern int joystick_MapZoomIn;
extern int joystick_MapZoomOut;
extern int joystick_RadarOnOff;
extern int joystick_RadarZoomIn;
extern int joystick_RadarZoomOut;
extern int joystick_SpeedBrake;
extern int joystick_TargetNext;
extern int joystick_TargetPrev;
extern int joystick_TargetVocalize;
extern int joystick_ToggleExtIntView;
extern int joystick_TrimElevatorDn;
extern int joystick_TrimElevatorUp;
extern int joystick_TrimRudderLeft;
extern int joystick_TrimRudderRight;
extern int joystick_Undercarriage;
extern int joystick_WEAPONSELECT;
extern int joystick_WEPorAfterburner;
extern int joystick_ZoomFOVin;
extern int joystick_ZoomFOVout;

extern unsigned int key_AutoPilot;
extern unsigned int key_CommsMacro01;
extern unsigned int key_CommsMacro02;
extern unsigned int key_CommsMacro03;
extern unsigned int key_CommsMacro04;
extern unsigned int key_CommsMacro05;
extern unsigned int key_CommsMacro06;
extern unsigned int key_CommsMacro07;
extern unsigned int key_CommsMacro08;
extern unsigned int key_CommsMacro09;
extern unsigned int key_CommsMacro10;
extern unsigned int key_CommsMacro11;
extern unsigned int key_CommsMacro12;
extern unsigned int key_CommsMacro13;
extern unsigned int key_CommsMacro14;
extern unsigned int key_CommsMacro15;
extern unsigned int key_CommsMacro16;
extern unsigned int key_DROPCHAFF;
extern unsigned int key_DROPFLARE;
extern unsigned int key_FlapsDn;
extern unsigned int key_FlapsUp;
extern unsigned int key_GunCameraToggle;
extern unsigned int key_HudOnOff;
extern unsigned int key_HudLadderBarsOnOff;
extern unsigned int key_IffOnOff;
extern unsigned int key_LookBK;
extern unsigned int key_LookDN;
extern unsigned int key_LookFD;
extern unsigned int key_LookLT;
extern unsigned int key_LookRT;
extern unsigned int key_LookUP;
extern unsigned int key_MapScrollEast;
extern unsigned int key_MapScrollNorth;
extern unsigned int key_MapScrollSouth;
extern unsigned int key_MapScrollWest;
extern unsigned int key_MapViewOnOff;
extern unsigned int key_MapZoomIn;
extern unsigned int key_MapZoomOut;
extern unsigned int key_MumblePtt;
extern unsigned int key_MumbleWhisper;
extern unsigned int key_PRIMARY;
extern unsigned int key_RadarOnOff;
extern unsigned int key_RadarZoomIn;
extern unsigned int key_RadarZoomOut;
extern unsigned int key_SECONDARY;
extern unsigned int key_SpeedBrake;
extern unsigned int key_TARGETNEXT;
extern unsigned int key_TARGETPREVIOUS;
extern unsigned int key_TARGETVOCALIZE;
extern unsigned int key_THRUSTDOWN;
extern unsigned int key_THRUSTUP;
extern unsigned int key_ToggleViewIntExt;
extern unsigned int key_TrimElevatorUp;
extern unsigned int key_TrimElevatorDn;
extern unsigned int key_TrimRudderLeft;
extern unsigned int key_TrimRudderRight;
extern unsigned int key_Undercarriage;
extern unsigned int key_WEAPONSELECT;
extern unsigned int key_WepAfterburner;
extern unsigned int key_ZoomFovIn;
extern unsigned int key_ZoomFovOut;

// Extern LacUdpApiPacket Variables;
extern Admin013Packet Admin013OutPacket;
extern LacUdpApiPacket InPacket;
extern LacUdpApiPacket OutPacket;

// Function Prototypes:
void AileronSettings (int x, int y);
void CompressibilityEffects();
void CompressibilityTurbulenceEffects();
void DisplayGame();
void DragEffects();
void ElevatorSettings(int x, int y);
void event_FlapsDN();
void event_FlapsUP();
void event_ToggleSpeedBrake();
void event_ToggleUndercarriage();
void event_ToggleWEP();
void event_TrimElevatorDn();
void event_TrimElevatorUp();
void event_TrimRudderLeft();
void event_TrimRudderRight();
void ExcessGamma();
void InertiaEffects();
void mission_display();
void RudderSettings(int rudder);
void SendAdmin013Packet();
void SendNetworkApiPacket();
void SpeedBrakeEffects();
void StallEffects();
void switch_fighter();
void TargetVocalizeProcess1 ();
void TargetVocalizeProcess2 ();
void ThrottleSettings(int throttle);
void TransmitCharacterViaMorseRadio(int);
void UpdateOnlineScoreLogFileWithDefeats();
void UpdateOnlineScoreLogFileWithLandings();
void UpdateOnlineScoreLogFileWithNewSession();
void UpdateOnlineScoreLogFileWithNewSorties();
void UpdateOnlineScoreLogFileWithVictories();
void VocalizeBlueHqStatus();
void VocalizeRedHqStatus();
void ZeroSpeedCorrection();

/****************************************************************************
	LOCAL FUNCTIONS
****************************************************************************/
static void LacJoystickHatFunc(int);
int game_levelInit (void);
void MouseMission (int button, int state, int x, int y);
void MouseQuit (int button, int state, int x, int y);
void checkargs (int argc, char **argv);
void config_test (int argc, char **argv);
void createMenu (void);
int setScreen (int w, int h, int b, int f);
void LacFirstInit (void);
void LacReshapeFunc (int width, int height);
void playRandomMusic (void);
void sdlMainLoop (void);

/****************************************************************************
  LAC ENTRY POINT
****************************************************************************/

int main (int argc, char **argv)
{

    char buf [STDSIZE]; // temp buffer
    int i;

    checkargs (argc, argv); // process command line parameters
    dirs = new Dirs (argv [0]); // get data directory (DATADIR, defined via autoconf)
    sprintf (buf, "Startup %s, %s ... ", argv [0], VERSIONSTRING);
    display (buf, LOG_MOST);
    
    size_t size1 = strlen(FileSystemHomeDir) + strlen(FileSystemLastTerrainFileName) + 1;
    snprintf(FileSystemLastTerrainFilePath, size1, "%s%s", FileSystemHomeDir, FileSystemLastTerrainFileName);
    display ((char*)"GLLandscape::GLLandscape() path to LastTerrain.LAC file =", LOG_MOST);
    display (FileSystemLastTerrainFilePath, LOG_MOST);

    size_t size2 = strlen(FileSystemHomeDir) + strlen(FileSystemDefaultHeightMapFileName) + 1;
    snprintf(FileSystemDefaultHeightMapFilePath, size2, "%s%s", FileSystemHomeDir, FileSystemDefaultHeightMapFileName);
    display ((char*)"GLLandscape::GLLandscape() path to DefaultHeightMap.LAC file =", LOG_MOST);
    display (FileSystemDefaultHeightMapFilePath, LOG_MOST);

    size_t size3 = strlen(FileSystemHomeDir) + strlen(FileSystemOnlineLineScoreLogFileName) + 1;
    snprintf(FileSystemOnlineScoreLogFilePath, size3, "%s%s", FileSystemHomeDir, FileSystemOnlineLineScoreLogFileName);
    display ((char*)"GLLandscape::GLLandscape() path to OnlineScoreLog.LAC file =", LOG_MOST);
    display (FileSystemOnlineScoreLogFilePath, LOG_MOST);

    size_t size4 = strlen(FileSystemHomeDir) + strlen(FileSystemGunCamHistoryFileName) + 1;
    snprintf(FileSystemGunCamHistoryFilePath, size4, "%s%s", FileSystemHomeDir, FileSystemGunCamHistoryFileName);
    display ((char*)"GLLandscape::GLLandscape() path to GunCamHistory.LAC file =", LOG_MOST);
    display (FileSystemGunCamHistoryFilePath, LOG_MOST);

    if (!load_config ())   
        {
        
        config_test (argc, argv); // do screen test
        FirstStart = true; 
        }
    save_config (); 
    load_configInterface (); 
    save_configInterface (); 
    
   GunCamHistoryFile = fopen (FileSystemGunCamHistoryFilePath, "a"); 
   if (GunCamHistoryFile == NULL)
      { 
      display ((char*)"main() failed to open GunCamHistory.LAC", LOG_MOST);
      }
   else
      {
      display ((char*)"main() succesfully opened GunCamHistory.LAC", LOG_MOST);
      }
    
   OnlineScoreLogFile = fopen (FileSystemOnlineScoreLogFilePath, "a"); 
   if (OnlineScoreLogFile == NULL)
      { 
      display ((char*)"main() failed to open OnlineScoreLogFile.LAC", LOG_MOST);
      }
   else
      {
      display ((char*)"main() succesfully opened OnlineScoreLogFile.LAC", LOG_MOST);
      }
    UpdateOnlineScoreLogFileWithNewSession();
    
    display ((char *)"joystick_FIREPRIMARY=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_FIREPRIMARY); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_FIRESECONDARY=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_FIRESECONDARY); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_DROPFLARE=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_DROPFLARE); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_DROPCHAFF=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_DROPCHAFF); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_WEAPONSELECT=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_WEAPONSELECT); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_ZoomFOVin=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_ZoomFOVin); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_ZoomFOVout=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_ZoomFOVout); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TargetNext=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TargetNext); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TargetPrev=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TargetPrev); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TargetVocalize=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TargetVocalize); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_ToggleExtIntView=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_ToggleExtIntView); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_FlapsUP=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_FlapsUP); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_FlapsDN=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_FlapsDN); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_WEPorAfterburner=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_WEPorAfterburner); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_SpeedBrake=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_SpeedBrake); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_Undercarriage=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_Undercarriage); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TrimRudderLeft=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TrimRudderLeft); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TrimRudderRight=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TrimRudderRight); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TrimElevatorUP=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TrimElevatorUp); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_TrimElevatorDn=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_TrimElevatorDn); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookBK=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookBK); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookDN=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookDN); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookFD=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookFD); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookLT=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookLT); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookRT=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookRT); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_LookUP=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_LookUP); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_RadarZoomIn=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_RadarZoomIn); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_RadarZoomOut=", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_RadarZoomOut); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_RadarOnOff", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_RadarOnOff); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_HudOnOff", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_HudOnOff); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_IffOnOff", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_IffOnOff); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_HudLadderBarsOnOff", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_HudLadderBarsOnOff); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapViewOnOff", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapViewOnOff); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapZoomIn", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapZoomIn); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapZoomOut", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapZoomOut); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapScrollEast", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapScrollEast); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapScrollWest", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapScrollWest); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapScrollNorth", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapScrollNorth); 
    display (DebugBuf, LOG_MOST);  
    display ((char *)"joystick_MapScrollSouth", LOG_MOST);
    sprintf (DebugBuf, "%d", joystick_MapScrollSouth); 
    display (DebugBuf, LOG_MOST);  
    pilots = new PilotList (dirs->getSaves ((char *)"pilots")); 
    // display ((char *)"Using SDL and GLUT", LOG_MOST);
    display ((char *)"Using SDL2 and OpenGL", LOG_MOST);

    if (!ConfigInit)
        if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
            {
            sprintf (buf, "Couldn't initialize SDL: %s", SDL_GetError ());
            display (buf, LOG_FATAL);
            exit (EXIT_INIT);
            }
    atexit (SDL_Quit);

    if (!ConfigInit)
        {
        if (!setScreen (width, height, bpp, fullscreen))
            {
            load_saveconfig ();
            if (!setScreen (width, height, bpp, fullscreen))
                {
                sprintf (buf, "No working display mode %dx%d found.", width, height);
                display (buf, LOG_FATAL);
                exit (EXIT_INIT);
                }
            }
        }

// TODO: use SDL2
//    SDL_WM_SetCaption ("LINUX Air Combat", "Linux Air Combat"); 

    display ((char *)"Creating sound system", LOG_MOST);
    sound = new SoundSystem (); 
    sound->volumesound = volumesound;
    sound->volumemusic = volumemusic;
    sound->setVolume (); 
    sound->setVolumeMusic (); 
    sound->setVolume (SOUND_AIRRAIDSIREN, 3);
    sound->play (SOUND_AIRRAIDSIREN, false);
    sound->setVolume (SOUND_LACSTARTUP00, 126);
    sound->play (SOUND_LACSTARTUP00, false);
    LacFirstInit ();
    LacReshapeFunc (width, height);
    display ((char *)"Querying joystick", LOG_MOST);
    joysticks = SDL_NumJoysticks ();
    memset (jaxis, 0, maxjaxis * maxjoysticks * sizeof (int));
    if (joysticks > 0)
        {
        for (i = 0; i < joysticks; i ++)
            {
            SDL_JoystickEventState (SDL_ENABLE);
            sdljoystick [i] = SDL_JoystickOpen (i);
            sdljoystickaxes [i] = SDL_JoystickNumAxes (sdljoystick [i]);
// TODO: use SDL2
//	    sprintf (buf, "Joystick \"%s\" detected as joystick %d", SDL_JoystickName (i), i);
            display (buf, LOG_MOST);
            sprintf (buf, "Axis count = %d", sdljoystickaxes[i]);
            display (buf, LOG_MOST);
            controls = CONTROLS_JOYSTICK;
            }
        }
    else
        {
        display ((char *)"No joystick found", LOG_MOST);

        if (controls == CONTROLS_JOYSTICK)   
            {
            controls = CONTROLS_MOUSE;
            }
        }
    if (MouseControlFlag)
        {
        
        controls = CONTROLS_MOUSE;
        }

// TODO: not in SDL2
//    SDL_EnableUNICODE (1);
//    SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    createMenu ();
    display ((char *)"Entering SDL main loop (GLUT emulation)", LOG_MOST);
    
    sdlMainLoop (); 
    return 0; 
} 

void adjustBrightness (void)
{
    
    if (brightness < 0)
        {
        glColor4ub (0, 0, 0, -brightness);
        float xf = 2.0, yf = 1.5, zf = 1.0;
        glDisable (GL_DEPTH_TEST);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin (GL_QUADS);
        glVertex3f (-xf, -yf, -zf);
        glVertex3f (-xf, yf, -zf);
        glVertex3f (xf, yf, -zf);
        glVertex3f (xf, -yf, -zf);
        glEnd ();
        glDisable (GL_BLEND);
        }
    else if (brightness > 0)
        {
        glColor4ub (255, 255, 255, brightness);
        float xf = 2.0, yf = 1.5, zf = 1.0;
        glDisable (GL_DEPTH_TEST);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin (GL_QUADS);
        glVertex3f (-xf, -yf, -zf);
        glVertex3f (-xf, yf, -zf);
        glVertex3f (xf, yf, -zf);
        glVertex3f (xf, -yf, -zf);
        glEnd ();
        glDisable (GL_BLEND);
        }
} 

void callbackAntialiasing (Component *comp, int key)
    {
    display ((char*)"Function Entry callbackAntialiasing()", LOG_MOST);
    void event_setAntialiasing ();
    char buf [5];
    antialiasing = (antialiasing == 0 ? 1 : 0);

    if (antialiasing)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }
    ((Label *) optmenu [0]->components [7])->setText (buf);
    event_setAntialiasing ();
    } 

void callbackBrightness (Component *comp, int key)
    {
    display ((char*)"Function Entry callbackBrightness()", LOG_MOST);
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        brightness += 10;

        if (brightness > 50)
            {
            brightness = -50;
            }
        }
    else
        {
        brightness -= 10;

        if (brightness < -50)
            {
            brightness = 50;
            }
        }

    sprintf (buf, "%d%%", brightness);
    ((Label *) optmenu [0]->components [13])->setText (buf);
    } 

void callbackControls (Component *comp, int key)
    {
    display ((char*)"Function Entry callbackControls()", LOG_MOST);
    void textControls (char *buf);
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        { 
        fplayer->rolleffect = 0;
        fplayer->ruddereffect = 0;
        fplayer->elevatoreffect = 0;
        keyb_elev = 0;
        keyb_roll = 0;
        keyb_rudder = 0;
        controls ++;
        if (controls > 2)
            {
            controls = 0;
            }

        if (controls == CONTROLS_JOYSTICK && !joysticks)
            {
            controls = CONTROLS_KEYBOARD;
            }
        }
    textControls (buf);
    ((Label *) optmenu [2]->components [5])->setText (buf);
    allmenus.components [11]->setVisible (false);
    allmenus.components [12]->setVisible (false);
    allmenus.components [13]->setVisible (false);
    if (controls == CONTROLS_KEYBOARD)
        {
        allmenus.components [11]->setVisible (true);
        }
    else if (controls == CONTROLS_JOYSTICK)
        {
        allmenus.components [13]->setVisible (true);
        }
    else
        {
        allmenus.components [12]->setVisible (true);
        }
    controls = CONTROLS_JOYSTICK; 
    textControls (buf);
    } 

void callbackCredits (Component *comp, int key)
    {
    display ((char*)"Function Entry callbackCredits()", LOG_MOST);
    void switch_credits ();
    switch_credits ();
    } 

void callbackDifficulty (Component *comp, int key)
    {
    display ((char*)"Function Entry callbackDificulty()", LOG_MOST);
    char buf [256];
    if (key == MOUSE_BUTTON_LEFT)
        {
        difficulty = 2;
        // only difficulty level "2" is now supported.
        if (difficulty != 2)
            {
            difficulty = 2;
            }
        }
    else
        {
        difficulty = 2;
        if (difficulty !=2)
            {
            difficulty = 2;
            }
        }
        sprintf (buf, "%s", "REALISTIC");

    ((Label *) optmenu [2]->components [1])->setText (buf);
    } 

void callbackDithering (Component *comp, int key)
    {
    char buf [5];
    dithering = (dithering == 0 ? 1 : 0);

    if (dithering)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    ((Label *) optmenu [0]->components [5])->setText (buf);
    } 

void callbackDynamicLighting (Component *comp, int key)
    {
    char buf [5];
    dynamiclighting = (dynamiclighting == 0 ? 1 : 0);

    if (dynamiclighting)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    ((Label *) optmenu [0]->components [11])->setText (buf);
    } 

void callbackFighter (Component *comp, int key)
    { 
    display ((char*)"Function Entry: callbackFighter()", LOG_MOST);
    
    switch_fighter ();
    } 

void callbackFighterNext (Component *comp, int key)
    {
    if (key == MOUSE_BUTTON_LEFT)
        {
        SelectedAircraft ++;
        sprintf (DebugBuf, "callbackFighterNext() mission->name = %s", mission->name);
        display (DebugBuf, LOG_MOST);
        display ((char*)"callbackFighterNext() decrementing SelectedAircraft. Now", LOG_MOST);
        sprintf (DebugBuf, "%d", SelectedAircraft);
        display (DebugBuf, LOG_MOST);
        
    if (SelectedAircraft > 55)
            {
            SelectedAircraft = 1;
            }
        }
    } 

void callbackFighterPrev (Component *comp, int key)
    {
    if (key == MOUSE_BUTTON_LEFT)
        {
        SelectedAircraft --;
        sprintf (DebugBuf, "callbackFighterPrev() mission->name = %s", mission->name);
        display (DebugBuf, LOG_MOST);
        display ((char*)"callbackFighterPrev() decrementing SelectedAircraft. Now", LOG_MOST);
        sprintf (DebugBuf, "%d", SelectedAircraft);
        display (DebugBuf, LOG_MOST);
        if (SelectedAircraft < 1)
            {
            SelectedAircraft = 55;
            }
        }
    } 

void callbackFullscreen (Component *comp, int key)
    {
    if (wantfullscreen)
        {
        wantfullscreen = 0;
        }
    else
        {
        wantfullscreen = 1;
        }

    if (wantfullscreen)
        {
        ((Label *) optmenu [0]->components [18])->setText ((char *)"YES");
        }
    else
        {
        ((Label *) optmenu [0]->components [18])->setText ((char *)"NO");
        }
    } 

void callbackJoystickAxis (Component *comp, int key)
    {
    int i;
    int buttonnum = -1;
    display ((char*)"callbackJoystickAxis()", LOG_MOST);
    
    for (i = 0; i < 12; i ++)
        {
        if (comp == controlsmenu [2]->components [i])
            {
            buttonnum = i;
            }
        }
    if (buttonnum == -1)
        {
        return;
        }

    Label *la = (Label *) controlsmenu [2]->components [buttonnum + 1];
    int joynum = 0;
    int joyaxis = 0;

    if (la->text [1] == '/')
        {
        joynum = -1;
        }
    else
        {
        joynum = la->text [0] - 'A';
        joyaxis = la->text [2] - '0';
        }

    if (key == MOUSE_BUTTON_LEFT)
        {
        if (joynum == -1)
            {
            joynum = 0;
            joyaxis = 0;
            }
        else if (joynum >= joysticks)
            {
            joynum = -1;
            }
        else
            {
            joyaxis ++;

            if (joyaxis >= sdljoystickaxes [joynum])
                {
                joyaxis = 0;
                joynum ++;

                if (joynum >= joysticks)
                    {
                    joynum = -1;
                    }
                }
            }
        }
    else
        {
        if (joynum == -1)
            {
            joynum = joysticks - 1;
            joyaxis = sdljoystickaxes [joynum] - 1;
            }
        else if (joynum >= joysticks)
            {
            joynum = -1;
            }
        else
            {
            joyaxis --;

            if (joyaxis < 0)
                {
                joynum --;

                if (joynum < 0)
                    {
                    joynum = -1;
                    joyaxis = 0;
                    }
                else
                    {
                    joyaxis = sdljoystickaxes [joynum] - 1;
                    }
                }
            }
        }

    int *joysetting = NULL;

    if (buttonnum == 0)
        {
        joysetting = &joystick_aileron;
        }
    else if (buttonnum == 2)
        {
        joysetting = &joystick_elevator;
        }
    else if (buttonnum == 4)
        {
        joysetting = &joystick_throttle;
        }
    else if (buttonnum == 6)
        {
        joysetting = &joystick_rudder;
        }
    else if (buttonnum == 8)
        {
        joysetting = &joystick_view_x;
        }
    else if (buttonnum == 10)
        {
        joysetting = &joystick_view_y;
        }

    *joysetting = joynum * 1000 + joyaxis;
    char latext [4];

    if (joynum >= 0)
        {
        latext [0] = 'A' + joynum;
        latext [1] = 32;
        latext [2] = '0' + joyaxis;
        latext [3] = 0;
        la->setText (latext);
        }
    else
        {
        la->setText ((char *)"N/A");
        }
    } 

void callbackKeyboard (Component *comp, int key)
    {
    display ((char*)"callbackKeyboard()", LOG_MOST);
    int i;

    for (i = 5; i < 42; i ++) 
        if (comp == controlsmenu [0]->components [i])
            {
            int z = 5;

            if      (i == z ++)
                {
                key_PRIMARY = key;
                display ((char*)"callbackKeyboard() mapping key_PRIMARY", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_SECONDARY = key;
                display ((char*)"callbackKeyboard() mapping key_SECONDARY", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_DROPFLARE = key;
                display ((char*)"callbackKeyboard() mapping key_DROPFLARE", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_DROPCHAFF = key;
                display ((char*)"callbackKeyboard() mapping key_DROPCHAFF", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_WEAPONSELECT = key;
                display ((char*)"callbackKeyboard() mapping key_WEAPONSELECT", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_TARGETNEXT = key;
                display ((char*)"callbackKeyboard() mapping key_TARGETNEXT", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_TARGETPREVIOUS = key;
                display ((char*)"callbackKeyboard() mapping key_TARGETPREVIOUS", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_ToggleViewIntExt = key;
                display ((char*)"callbackKeyboard() mapping key_ToggleViewIntExt", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_FlapsUp = key;
                display ((char*)"callbackKeyboard() mapping key_FlapsUp", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_FlapsDn = key;
                display ((char*)"callbackKeyboard() mapping key_FlapsDn", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_GunCameraToggle = key;
                display ((char*)"callbackKeyboard() mapping key_GunCameraToggle", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_WepAfterburner = key;
                display ((char*)"callbackKeyboard() mapping key_WepAfterburner", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_SpeedBrake = key;
                display ((char*)"callbackKeyboard() mapping key_SpeedBrake", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_Undercarriage = key;
                display ((char*)"callbackKeyboard() mapping key_Undercarriage", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_RadarZoomIn = key;
                display ((char*)"callbackKeyboard() mapping key_RadarZoomIn", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_RadarZoomOut = key;
                display ((char*)"callbackKeyboard() mapping key_RadarZoomOut", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_HudOnOff = key;
                display ((char*)"callbackKeyboard() mapping key_HudOnOff", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_IffOnOff = key;
                display ((char*)"callbackKeyboard() mapping key_IffOnOff", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_HudLadderBarsOnOff = key;
                display ((char*)"callbackKeyboard() mapping key_HudLadderBarsOnOff", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapViewOnOff = key;
                display ((char*)"callbackKeyboard() mapping key_MapViewOnOff", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapZoomIn = key;
                display ((char*)"callbackKeyboard() mapping key_MapZoomIn", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapZoomOut = key;
                display ((char*)"callbackKeyboard() mapping key_MapZoomOut", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapScrollEast = key;
                display ((char*)"callbackKeyboard() mapping key_MapScrollEast", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapScrollWest = key;
                display ((char*)"callbackKeyboard() mapping key_MapScrollWest", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapScrollNorth = key;
                display ((char*)"callbackKeyboard() mapping key_MapScrollNorth", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MapScrollSouth = key;
                display ((char*)"callbackKeyboard() mapping key_MapScrollSouth", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_TARGETVOCALIZE = key;
                display ((char*)"callbackKeyboard() mapping key_TARGETVOCALIZE", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MumblePtt = key;
                display ((char*)"callbackKeyboard() mapping key_MumblePtt", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_MumbleWhisper = key;
                display ((char*)"callbackKeyboard() mapping key_MumbleWhisper", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_ZoomFovIn = key;
                display ((char*)"callbackKeyboard() key_ZoomFovIn", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_ZoomFovOut = key;
                display ((char*)"callbackKeyboard() key_ZoomFovOut", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            else if (i == z ++)
                {
                key_RadarOnOff = key;
                display ((char*)"callbackKeyboard() key_RadarOnOff", LOG_MOST);
                sprintf (DebugBuf, "Key = %d", key);
                display (DebugBuf, LOG_MOST);
                }
            }
    } 

void callbackMainMenu (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMainMenu()", LOG_MOST);
    int i;

    for (i = 0; i < 6; i ++)
        if (comp == mainbutton [i])
            {
            allmenus.setVisible (false);
            allmenus.visible = true;
            allmenus.components [0]->setVisible (true);
            allmenus.components [i + 1]->setVisible (true);
            currentsubmenu = allmenus.components [i + 1];

            if (MissionActive)
                {
                mainbutton [6]->setVisible (true);
                }
            else
                {
                mainbutton [6]->setVisible (false);
                }

            if (i == 3)
                {
                if (currentoptmenu)
                    {
                    currentoptmenu->setVisible (true);
                    }

                if (currentoptmenu == allmenus.components [10])
                    {
                    if (controls == CONTROLS_KEYBOARD)
                        {
                        allmenus.components [11]->setVisible (true);
                        }
                    else if (controls == CONTROLS_JOYSTICK)
                        {
                        allmenus.components [13]->setVisible (true);
                        }
                    else
                        {
                        allmenus.components [12]->setVisible (true);
                        }
                    }
                }

            return;
            }

    for (i = 0; i < 3; i ++)
        if (comp == optbutton [i])
            {
            allmenus.components [8]->setVisible (false);
            allmenus.components [9]->setVisible (false);
            allmenus.components [10]->setVisible (false);
            allmenus.components [i + 1 + 7]->setVisible (true);

            if (i + 1 + 7 == 10)
                {
                if (controls == CONTROLS_KEYBOARD)
                    {
                    allmenus.components [11]->setVisible (true);
                    }
                else if (controls == CONTROLS_JOYSTICK)
                    {
                    allmenus.components [13]->setVisible (true);
                    }
                else
                    {
                    allmenus.components [12]->setVisible (true);
                    }
                }
            else
                {
                allmenus.components [11]->setVisible (false);
                allmenus.components [12]->setVisible (false);
                allmenus.components [13]->setVisible (false);
                }

            currentoptmenu = allmenus.components [i + 1 + 7];
            return;
            }
    } 

void callbackMouseAutorudder (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMouseAutoRudder()", LOG_MOST);
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        mouse_autorudder += 10;

        if (mouse_autorudder > 100)
            {
            mouse_autorudder = 0;
            }
        }
    else
        {
        mouse_autorudder -= 10;

        if (mouse_autorudder < 0)
            {
            mouse_autorudder = 100;
            }
        }

    sprintf (buf, "%d%%", mouse_autorudder);
    ((Label *) controlsmenu [1]->components [7])->setText (buf);
    } 

void callbackMouseDefaults (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMouseDefaults()", LOG_MOST);
    char buf [256];
    mouse_sensitivity = 100;
    mouse_reverse = false;
    mouse_relative = false;
    mouse_autorudder = 30;
    sprintf (buf, "%d%%", mouse_autorudder);
    ((Label *) controlsmenu [1]->components [7])->setText (buf);

    if (mouse_relative)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }

    ((Label *) controlsmenu [1]->components [5])->setText (buf);

    if (mouse_reverse)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }

    ((Label *) controlsmenu [1]->components [3])->setText (buf);
    sprintf (buf, "%d%%", mouse_sensitivity);
    ((Label *) controlsmenu [1]->components [1])->setText (buf);
    } 

void callbackMouseRelative (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMouseRelative()", LOG_MOST);
    char buf [256];
    mouse_relative = !mouse_relative;

    if (mouse_relative)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }

    ((Label *) controlsmenu [1]->components [5])->setText (buf);
    } 

void callbackMouseReverse (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMouseReverse()", LOG_MOST);
    char buf [256];
    mouse_reverse = !mouse_reverse;

    if (mouse_reverse)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }

    ((Label *) controlsmenu [1]->components [3])->setText (buf);
    } 

void callbackMouseSensitivity (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMouseSensitivity()", LOG_MOST);
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        mouse_sensitivity += 10;

        if (mouse_sensitivity > 250)
            {
            mouse_sensitivity = 70;
            }
        }
    else
        {
        mouse_sensitivity -= 10;

        if (mouse_sensitivity < 70)
            {
            mouse_sensitivity = 250;
            }
        }

    sprintf (buf, "%d%%", mouse_sensitivity);
    ((Label *) controlsmenu [1]->components [1])->setText (buf);
    } 

void callbackMusic (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackMusic()", LOG_MOST);
    void playRandomMusic ();
    void ReshapeMenu ();
    char buf [256];

    if (sound->audio)
        {
        if (key == MOUSE_BUTTON_LEFT)
            {
            sound->volumemusic += 10;

            if (sound->volumemusic > 100)
                {
                sound->volumemusic = 0;
                sound->haltMusic ();
                }

            volumemusic = sound->volumemusic;
            sound->setVolumeMusic ();
            ReshapeMenu ();
            }
        else
            {
            sound->volumemusic -= 10;

            if (sound->volumemusic < 0)
                {
                sound->volumemusic = 100;
                }

            volumemusic = sound->volumemusic;
            sound->setVolumeMusic ();
            ReshapeMenu ();
            }

        if (sound->volumemusic != 0 && !sound->musicplaying)
            {
            playRandomMusic ();
            }

        sprintf (buf, "%d%%", volumemusic);
        ((Label *) optmenu [1]->components [3])->setText (buf);
        }
    } 

void callbackPhysics (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackPhysics()", LOG_MOST);
    char buf [256];
    physics = 1;
    sprintf (buf, "%s", "SIMULATOR");
    ((Label *) optmenu [2]->components [3])->setText (buf);
    } 

void callbackQuality (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackQuality()", LOG_MOST);
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        quality ++; 

        if (quality > 5)
            {
            quality = 0;
            }
        }
    else
        {
        quality --;

        if (quality < 0)
            {
            quality = 5;
            }
        }

    sprintf (buf, "%d", quality);
    ((Label *) optmenu [0]->components [1])->setText (buf); 
    } 

void callbackQuit (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackQuit()", LOG_MOST);
    void switch_quit ();
    switch_quit ();
    } 

void callbackQuitNow (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackQuitNow()", LOG_MOST);
    void game_quit ();
    game_quit ();
    } 

void callbackResolution (Component *comp, int key)
{
    display ((char*)"Function entry: callbackResolution()", LOG_MOST);
// NOTE: now tied this to resolution array
// Also, non-fullscreen window resolutions don't need to be subset of display resolutions
// could be resizeable if code handles it properly

    int found = 0;
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        for (int i = 0; i < NUM_SCREEN_RES; i++)
            if (wantwidth == resolution[i][0])
                {
                found = i + 1;
                }
        }
    else
        {
        for (int i = 0; i < NUM_SCREEN_RES; i++)
            if (wantwidth == resolution[i][0])
                {
                found = i - 1;
                }
        }

    if (found < 0)
        {
        found = NUM_SCREEN_RES - 1;
        }
    else if (found >= NUM_SCREEN_RES)
        {
        found = 0;
        }

    wantwidth = resolution[found][0]; //  resx [found];
    wantheight = resolution[found][1]; // resy [found];
    sprintf (buf, "%d*%d", wantwidth, wantheight);
    ((Label *) optmenu [0]->components [16])->setText (buf);
} 

void callbackReturn (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackReturn()", LOG_MOST);
    void switch_game ();
    switch_game ();
    } 

void callbackSound (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackSound()", LOG_MOST);
    void ReshapeMenu ();
    void setPlaneVolume ();
    char buf [256];

    if (sound->audio)
        {
        if (key == MOUSE_BUTTON_LEFT)
            {
            sound->volumesound += 10;

            if (sound->volumesound > 100)
                {
                sound->volumesound = 0;
                }

            volumesound = sound->volumesound;
            sound->setVolume ();
            setPlaneVolume ();
            sound->play (SOUND_CLICK1, false);
            ReshapeMenu ();
            }
        else
            {
            sound->volumesound -= 10;

            if (sound->volumesound < 0)
                {
                sound->volumesound = 100;
                }

            volumesound = sound->volumesound;
            sound->setVolume ();
            setPlaneVolume ();
            sound->play (SOUND_CLICK1, false);
            ReshapeMenu ();
            }

        sprintf (buf, "%d%%", volumesound);
        ((Label *) optmenu [1]->components [1])->setText (buf);
        }
    } 

void callbackSpecialEffects (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackSpecialEffects()", LOG_MOST);
    char buf [5];
    specialeffects = (specialeffects == 0 ? 1 : 0);

    if (specialeffects)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    ((Label *) optmenu [0]->components [9])->setText (buf);
    } 

void callbackSwitchMainMenu (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackSwitchMainMenu()", LOG_MOST);
    void switch_menu ();
    switch_menu ();
    } 

void callbackSwitchStartMission (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackSwitchStartMission()", LOG_MOST);
    void pleaseWait ();
    void switch_game ();
    void switch_menu ();
    pleaseWait ();
    if (!game_levelInit())
        {
        switch_menu ();
        return;
        }
    switch_game ();
    
    fplayer->FuelLevel = 100;
    if (MissionNumber == 1)   
        {
        sound->loadMusic (MUSIC_TUTORIAL01);
        sound->playMusic (1); 
        }
    else if (MissionNumber == 2)     
        {
        sound->loadMusic (MUSIC_TUTORIAL02);
        sound->playMusic (1); 
        }
    else if (MissionNumber == 3)     
        {
        
        sound->loadMusic (MUSIC_TUTORIAL03); 
        sound->playMusic (1); 
        }
    else if (MissionNumber == 12)
        { 
        display ((char*)"Loading music file for MissionHeadToHead00.", LOG_MOST);
        sound->loadMusic (MUSIC_HEADTOHEAD);
        sound->playMusic (1); 
        }
    else if (MissionNumber == 20)     
        {
        sound->loadMusic (MUSIC_MISSIONNETWORKBATTLE01);
        sound->playMusic (1); 
        }
    else if (MissionNumber == 21)     
        {
        sound->loadMusic (MUSIC_MISSIONNETWORKBATTLE02);
        sound->playMusic (1); 
        }
    else if (MissionNumber == 23)     
        {
        sound->loadMusic (MUSIC_MISSIONNETWORKBATTLE03);
        sound->playMusic (1); 
        }
    else
        {
        sound->loadMusic (MUSIC_MUSIC02ANDFURBALLAA);
        sound->playMusic (1);
        }
    MissionActive = true;
    } 

void callbackTraining (Component *comp, int key)
    { 
    display ((char*)"Function Entry callbackTraining()", LOG_MOST);
    int getTrainingIdFromValue (int n);
    void switch_mission (int missionid);
    allmenus.setVisible (false);
    switch_mission (getTrainingIdFromValue (comp->id - TrainingStartId));
    } 

void callbackView (Component *comp, int key)
    {
    display ((char*)"Function entry: callbackView()", LOG_MOST);
    void ReshapeMenu ();
    char buf [256];

    if (key == MOUSE_BUTTON_LEFT)
        {
        view += 10;

        if (view > VIEW_MAX)
            {
            view = VIEW_MIN;
            }

        ReshapeMenu ();
        }
    else
        {
        view -= 10;

        if (view < VIEW_MIN)
            {
            view = VIEW_MAX;
            }

        ReshapeMenu ();
        }

    sprintf (buf, "%d", (int) view);
    ((Label *) optmenu [0]->components [3])->setText (buf); 
    } 

void checkargs (int argc, char **argv)
    {
    void viewParameters ();
    char buf [STDSIZE]; 
    int i; 
    MouseControlFlag = 0; 

    for (i = 1; i < argc; i ++)   
        {
        if (argv [i] [1] == 'd')   
            {
            char *ptr = &argv [i] [2];
            DebugLevel = atoi (ptr);

            if (DebugLevel < LOG_NONE || DebugLevel > LOG_ALL)   
                {
                display ((char *)"Invalid debug level", LOG_FATAL);
                viewParameters ();
                exit (EXIT_COMMAND);
                }
            else
                {
                sprintf (buf, "Entering debug level %d", DebugLevel);
                display (buf, LOG_MOST);
                }
            }
        else if (argv [i] [1] == 'v')     // display version string
            {
            display ((char *)VERSIONSTRING, LOG_NONE);
            exit (EXIT_NORMAL);
            }
        else if (argv [i] [1] == 'h')     // display startup help screen
            {
            viewParameters ();
            exit (EXIT_NORMAL);
            }
        else if (argv [i] [1] == 'm')
            {
            
            display ((char *)"MouseControl acknowledged.", LOG_MOST);
            MouseControlFlag = 1;
            }
        else
            {
            display ((char *)"Invalid command line parameter", LOG_FATAL);
            viewParameters ();
            exit (EXIT_COMMAND);
            }
        }
    } 

CModel *getModel (int id)
    {
    if (id == FIGHTER_P38L)
        {
        return &model_fig;
        }
    else if (id == FIGHTER_HAWK)
        {
        return &model_figb;
        }
    else if (id == FIGHTER_P51D)
        {
        return &model_figc;
        }
    else if (id == FIGHTER_SPIT9)
        {
        return &model_figi;
        }
    else if (id == FIGHTER_A6M2)
        {
        return &model_figv;
        }
    else if (id == FIGHTER_P47D)
        {
        return &model_figw;
        }
    else if (id == FIGHTER_F4F)
        {
        return &model_figAO;
        }
    else if (id == FIGHTER_F6F)
        {
        return &model_figx;
        }
    else if (id == FIGHTER_F4U)
        {
        return &model_figy;
        }
    else if (id == FIGHTER_FW190)
        {
        return &model_figz;
        }
    else if (id == BOMBER_B17)
        {
        return &model_figu;
        }
    else if (id == BOMBER_JU87)
        {
        return &model_figAA;
        }
    else if (id == BOMBER_G5M)
        {
        return &model_figAL;
        }
    else if (id == BOMBER_B25)
        {
        return &model_figAM;
        }
    else if (id == BOMBER_B26)
        {
        return &model_figAN;
        }
    else if (id == FIGHTER_IL16)
        {
        return &model_figAB;
        }
    else if (id == FIGHTER_FIATG55)
        {
        return &model_figAC;
        }
    else if (id == FIGHTER_ME109G)
        {
        return &model_figAD;
        }
    else if (id == FIGHTER_P39)
        {
        return &model_figAK;
        }
    else if (id == BOMBER_G5M)
        {
        return &model_figAL;
        }
    else if (id == BOMBER_B25)
        {
        return &model_figAM;
        }
    else if (id == FIGHTER_P40)
        {
        return &model_figAE;
        }
    else if (id == FIGHTER_HURRICANE)
        {
        return &model_figAF;
        }
    else if (id == FIGHTER_KI43)
        {
        return &model_figAG;
        }
    else if (id == FIGHTER_YAK9)
        {
        return &model_figAH;
        }
    else if (id == FIGHTER_N1K1)
        {
        return &model_figAI;
        }
    else if (id == BOMBER_B24)
        {
        return &model_figAJ;
        }
    else if (id == FIGHTER_LA5)
        {
        return &model_figAP;
        }
    else if (id == FIGHTER_LA7)
        {
        return &model_figAQ;
        }
    else if (id == FIGHTER_IL2)
        {
        return &model_figAR;
        }
    else if (id == FIGHTER_MACCIC202)
        {
        return &model_figAS;
        }
    else if (id == BOMBER_LANCASTER)
        {
        return &model_figAT;
        }
    else if (id == BOMBER_MOSQUITOB)
        {
        return &model_figAU;
        }
    else if (id == FIGHTER_TYPHOON)
        {
        return &model_figAV;
        }
    else if (id == FIGHTER_YAK1)
        {
        return &model_figAW;
        }
    else if (id == BOMBER_B29)
        {
        return &model_figAX;
        }
    else if (id == FIGHTER_DW520)
        {
        return &model_figAY;
        }
    else if (id == BOMBER_SB2C)
        {
        return &model_figAZ;
        }
    else if (id == BOMBER_TBF)
        {
        return &model_figBA;
        }
    else if (id == FIGHTER_ME163)
        {
        return &model_figBB;
        }
    else if (id == FIGHTER_TEMPEST)
        {
        return &model_figBC;
        }
    else if (id == FIGHTER_D3A)
        {
        return &model_figBD;
        }
    else if (id == BOMBER_B5N)
        {
        return &model_figBE;
        }
    else if (id == BOMBER_DAUNTLESS)
        {
        return &model_figBF;
        }
    else if (id == FIGHTER_ME110)
        {
        return &model_figBG;
        }
    else if (id == BOMBER_DORNIER)
        {
        return &model_figBH;
        }
    else if (id == BOMBER_HE111)
        {
        return &model_figBI;
        }
    else if (id == BOMBER_JU88)
        {
        return &model_figBJ;
        }
    else if (id == FIGHTER_KI84)
        {
        return &model_figBK;
        }
    else if (id == FIGHTER_KI61)
        {
        return &model_figBL;
        }
    else if (id == FIGHTER_GENERIC01)
        {
        return &model_figBM;
        }
    else if (id == FIGHTER_A6M5)
        {
        return &model_figBN;
        }
    else if (id == FIGHTER_SPIT5)
        {
        return &model_figBO;
        }
    else if (id == FIGHTER_P51B)
        {
        return &model_figBP;
        }
    else if (id == FIGHTER_P47B)
        {
        return &model_figBQ;
        }
    else if (id == FIGHTER_ME109F)
        {
        return &model_figBR;
        }
    else if (id == FIGHTER_P38F)
        {
        return &model_figBS;
        }
    else if (id == BOMB01)
        {
        return &model_bomb01;
        }
    else if (id == MISSILE_AIR2)
        {
        return &model_missile2;
        }
    else if (id == MISSILE_AIR3)
        {
        return &model_missile3;
        }
    else if (id == MISSILE_GROUND1)
        {
        return &model_missile4;
        }
    else if (id == MISSILE_GROUND2)
        {
        return &model_missile5;
        }
    else if (id == MISSILE_DF1)
        {
        return &model_missile6;
        }
    else if (id == MISSILE_FF1)
        {
        return &model_missile7;
        }
    else if (id == MISSILE_FF2)
        {
        return &model_missile8;
        }
    else if (id == TANK_AIR1)
        {
        return &model_tank1;
        }
    else if (id == TANK_GROUND1)
        {
        return &model_tank2;
        }
    else if (id == FLAK_AIR1)
        {
        return &model_flak1;
        }
    else if (id == STATIC_BATTLESHIP)
        {
        return &model_battleship;
        }
    else if (id == STATIC_HALL2)
        {
        return &model_hall2;
        }
    else if (id == STATIC_AIRFIELD00)
        {
        return &model_Airfield00;
        }
    else if (id == STATIC_RADARREFLECTOR)
        {
        return &model_RadarReflector;
        }
    else if (id == STATIC_RADAR1)
        {
        return &model_radar;
        }
    else if (id == STATIC_DEPOT1)
        {
        return &model_depot1;
        }
    return &model_figBS; 
    } 

// test screen settings automatically
void config_test (int argc, char **argv)
    {
    display ((char *)"No configuration file found. Testing...", LOG_MOST);
    int bppi [4];
    char buf [STDSIZE];
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
        {
        sprintf (buf, "Couldn't initialize SDL: %s", SDL_GetError ());
        display (buf, LOG_FATAL);
        exit (EXIT_INIT);
        }
    ConfigInit = true;
    int valids = -1; // valid screen mode? (-1 = no mode)
    int n = 0;
  
    while (n < NUM_SCREEN_RES)
        {
        if (setScreen (resolution [n] [0], resolution [n] [1], resolution [n] [2], resolution [n] [3]))
            {
            bppi [n] = bpp; 
            valids = n;
            break; 
            }
        else
            {
            bppi [n] = -1;
            }
        n ++;
        }
    if (valids == -1)
        { 
        display ((char *)"No working display modes found. Your computer may not be compatible. Try editing the ./LAC/LacConfig.txt file yourself.", LOG_FATAL);
        display ((char *)"Online help is available at AskMisterWizard.com", LOG_FATAL);
        exit (EXIT_INIT);
        }
    
    quality = 0;
    view = 80;
    width = resolution [valids] [0];
    height = resolution [valids] [1];
    bpp = bppi [valids];
    fullscreen = 0;
    wantwidth = width; 
    wantheight = height;
    wantfullscreen = fullscreen;
    } 

void createMenu ()
    {
    display ((char*)"Function entry: createMenu()", LOG_MOST);
    void setJoystickAxisString (int joysetting, char *joystr);
    void textControls (char *buf);
    void textMouseButton (char *buf, int button);
    char buf [512];
    char buf2 [256];
    int i;
    int z = 0;
    float xf = -2;
    float xfstep = 3; 
    float yf = 12;
    float yfstep = 0.8; 
    float xsubmenu = -1;
    float ysubmenu = 12;
    Button *button;
    Label *label;
    TextField *textfield;
    EditKey *editkey;
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register main menu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    strncpy (MainMenuNames [0], "MainMenuNames[0]", 17);
    strncpy (MainMenuNames [1], "FLY A MISSION", 14);
    strncpy (MainMenuNames [2], "", 2);
    strncpy (MainMenuNames [3], "SETUP OPTIONS", 14);
    strncpy (MainMenuNames [4], "AUTHORS", 8);
    strncpy (MainMenuNames [5], "EXIT", 5);
    strncpy (MainMenuNames [6], "RETURN TO FLIGHT", 17);
    mainmenu = new Container ();
    allmenus.add (mainmenu);

    for (i = 0; i < 7; i ++)
        {
        submenu [i] = new Container ();
        allmenus.add (submenu [i]);
        }

    for (i = 1; i < 2; i ++)
        { 
        mainbutton [i] = new Button (MainMenuNames [i]);
        mainbutton [i]->setBounds (-12, yf, 10, yfstep - 0.1);
        yf -= yfstep;
        mainbutton [i]->setFunction (callbackMainMenu);
        mainmenu->add (mainbutton [i]);
        }
    
    for (i = 3; i < 7; i ++)
        { 
        mainbutton [i] = new Button (MainMenuNames [i]);
        mainbutton [i]->setBounds (-12, yf, 10, yfstep - 0.1);
        yf -= yfstep;
        mainbutton [i]->setFunction (callbackMainMenu);
        mainmenu->add (mainbutton [i]);
        }

    currentsubmenu = submenu [0];
    mainbutton [4]->setFunction (callbackCredits);
    mainbutton [5]->setFunction (callbackQuit);
    mainbutton [6]->setVisible (false);
    mainbutton [6]->setFunction (callbackReturn);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register training submenu "submenu1names[]"
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    z = 0;
    strncpy (submenu1names [z ++], "TUTORIAL1: FLIGHT BASICS", 25);
    strncpy (submenu1names [z ++], "TUTORIAL2: AIR-TO-GROUND", 25);
    strncpy (submenu1names [z ++], "TUTORIAL3: FIGHTERS", 20);
    strncpy (submenu1names [z ++], "TUTORIAL4: FREE FLIGHT", 24);
    strncpy (submenu1names [z ++], "NETWORK HEAD TO HEAD 00", 24);
    strncpy (submenu1names [z ++], "NETWORK BATTLE 01", 18);
    strncpy (submenu1names [z ++], "NETWORK BATTLE 02", 18);
    strncpy (submenu1names [z ++], "NETWORK BATTLE 03", 18);
    xf = xsubmenu;
    yf = ysubmenu - 2;
    xfstep = 12;
    yfstep = 0.8;
    
    for (i = 0; i < z; i ++)
        { 
        
        button = new Button (submenu1names [i]);
        button->setBounds (xf, yf, xfstep, yfstep - 0.1);
        button->setFunction (callbackTraining);
        yf -= yfstep;
        
        if (i == 3)
            {
            yf -= yfstep;
            }
        submenu [1]->add (button); 
        }
    
    TrainingStartId = submenu [1]->components [0]->id;
    yf -= yfstep;
    xf = xsubmenu;
    yf = ysubmenu;
    yfstep = 1;
    button = new Button ((char *)"DEFAULT AIRCRAFT");
    button->setFunction (callbackFighter); 
    button->setBounds (xf, yf, xfstep - 0.2, yfstep - 0.1);
    xf += xfstep;
    submenu [1]->add (button);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register options submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xf = xsubmenu;
    yf = 12;
    xfstep = 4.6;
    strncpy (OptionsMenuNames [0], "VIDEO", 6);
    strncpy (OptionsMenuNames [1], "AUDIO", 6);
    strncpy (OptionsMenuNames [2], "GAME", 5);
    for (i = 0; i < 3; i ++)
        { 
        optmenu [i] = new Container ();
        allmenus.add (optmenu [i]);
        }
    for (i = 0; i < 3; i ++)
        { 
        optbutton [i] = new Button (OptionsMenuNames [i]);
        optbutton [i]->setBounds (xf, yf, xfstep - 0.2, 1);
        xf += xfstep;
        optbutton [i]->setFunction (callbackMainMenu);
        submenu [3]->add (optbutton [i]);
        }

    currentoptmenu = optmenu [0];
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register video options submenu with 9 buttons and a "NEED RESTART" label
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    float xftab = 3;
    xf = xsubmenu;
    yf = 10;
    xfstep = 13.6;
    yfstep = 1;
    button = new Button ((char *)"QUALITY");
    button->setFunction (callbackQuality);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);
    sprintf (buf, "%d", quality);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"VIEW");
    button->setFunction (callbackView);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);
    sprintf (buf, "%d", (int) view);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"DITHERING");
    button->setFunction (callbackDithering);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);

    if (dithering)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"ANTIALIASING");
    button->setFunction (callbackAntialiasing);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);

    if (antialiasing)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"SPECIAL EFFECTS");
    button->setFunction (callbackSpecialEffects);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);

    if (specialeffects)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"DYN LIGHTING");
    button->setFunction (callbackDynamicLighting);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);

    if (dynamiclighting)
        {
        strncpy (buf, "ON", 3);
        }
    else
        {
        strncpy (buf, "OFF", 4);
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"BRIGHTNESS");
    button->setFunction (callbackBrightness);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);
    sprintf (buf, "%d%%", brightness);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    yf -= yfstep / 2;
    sprintf (buf, "NEED RESTART:");
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf, yf, 2, yfstep - 0.3);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"RESOLUTION");
    button->setFunction (callbackResolution);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);
    sprintf (buf, "%d*%d", wantwidth, wantheight);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab * 2, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"FULLSCREEN");
    button->setFunction (callbackFullscreen);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [0]->add (button);

    if (wantfullscreen)
        {
        sprintf (buf, "YES");
        }
    else
        {
        sprintf (buf, "NO");
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [0]->add (label);
    yf -= yfstep;
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register sound options submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    yf = 10;
    xfstep = 13.6;
    yfstep = 1;
    button = new Button ((char *)"SOUND VOLUME");
    button->setFunction (callbackSound);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [1]->add (button);

    if (sound->audio)
        {
        sprintf (buf, "%d%%", volumesound);
        }
    else
        {
        sprintf (buf, "N/A");
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [1]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"MUSIC VOLUME");
    button->setFunction (callbackMusic);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [1]->add (button);

    if (sound->audio)
        {
        sprintf (buf, "%d%%", volumemusic);
        }
    else
        {
        sprintf (buf, "N/A");
        }

    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [1]->add (label);
    yf -= yfstep;

    if (!sound->audio)
        {
        optmenu [1]->components [0]->setActive (false);
        optmenu [1]->components [1]->setActive (false);
        optmenu [1]->components [2]->setActive (false);
        optmenu [1]->components [3]->setActive (false);
        }

    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register controls options submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xftab = 6.8;
    xf = xsubmenu;
    yf = 10;
    xfstep = 13.5;
    yfstep = 1;
    button = new Button ((char *)"DIFFICULTY");
    button->setFunction (callbackDifficulty);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [2]->add (button);

    if (difficulty == 0)
        {
        sprintf (buf, "%s", "INFANTILE");
        }
    else if (difficulty == 1)
        {
        sprintf (buf, "%s", "RELAXED");
        }
    else if (difficulty == 2)
        {
        sprintf (buf, "%s", "REALISTIC");
        }
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"PRECISION");
    button->setFunction (callbackPhysics);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [2]->add (button);
    sprintf (buf, "%s", "SIMULATOR");
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"CONTROLS");
    button->setFunction (callbackControls);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    optmenu [2]->add (button);
    textControls (buf);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    optmenu [2]->add (label);
    yf -= yfstep;
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register keyboard control submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xf = xsubmenu -11;
    yf = ysubmenu - 6;
    xfstep = 10;
    yfstep = 0.7; 
    for (i = 0; i < 3; i ++)
        {
        controlsmenu [i] = new Container ();
        allmenus.add (controlsmenu [i]);
        }
    label = new Label ((char *)"REMAP KEYBOARD KEYS");
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [0]->add (label);
    yf -= yfstep;
    
    controlsmenu [0]->add (label);
    yf -= yfstep;
    
    controlsmenu [0]->add (label);
    yf -= yfstep;
    
    controlsmenu [0]->add (label);
    yf -= yfstep;
    
    controlsmenu [0]->add (label);
    yf -= yfstep;
    yf = yf + (5 * yfstep); 
    z = 0;
    display ((char*)"Register keyboard control submenu", LOG_MOST);
    strncpy (ControlsMenu0Names [z ++], "FIRE PRIMARY", 13);
    strncpy (ControlsMenu0Names [z ++], "FIRE SECONDARY", 15);
    strncpy (ControlsMenu0Names [z ++], "DROP FLARE", 11);
    strncpy (ControlsMenu0Names [z ++], "DROP CHAFF", 11);
    strncpy (ControlsMenu0Names [z ++], "WEAPON SELECT", 14);
    strncpy (ControlsMenu0Names [z ++], "TARGET NEXT", 12);
    strncpy (ControlsMenu0Names [z ++], "TARGET PREVIOUS", 16);
    strncpy (ControlsMenu0Names [z ++], "TOGGLE EXT VIEW", 16);
    strncpy (ControlsMenu0Names [z ++], "FLAPS UP", 9);
    strncpy (ControlsMenu0Names [z ++], "FLAPS DOWN", 11);
    strncpy (ControlsMenu0Names [z ++], "GUN CAM TOGGLE", 15);
    strncpy (ControlsMenu0Names [z ++], "WEP/AFTERBURNER", 16);
    strncpy (ControlsMenu0Names [z ++], "SPEED BRAKE", 12);
    strncpy (ControlsMenu0Names [z ++], "UNDERCARRIAGE", 14);
    strncpy (ControlsMenu0Names [z ++], "RADAR ZOOM IN", 14);
    strncpy (ControlsMenu0Names [z ++], "RADAR ZOOM OUT", 15);
    strncpy (ControlsMenu0Names [z ++], "HUD ON/OFF", 11);
    strncpy (ControlsMenu0Names [z ++], "IFF ON/OFF", 11);
    strncpy (ControlsMenu0Names [z ++], "HUD LADDER ON/OFF", 18);
    strncpy (ControlsMenu0Names [z ++], "MAP VIEW ON/OFF", 16);
    strncpy (ControlsMenu0Names [z ++], "MAP ZOOM IN", 12);
    strncpy (ControlsMenu0Names [z ++], "MAP ZOOM OUT", 13);
    strncpy (ControlsMenu0Names [z ++], "MAP SCROLL EAST", 16);
    strncpy (ControlsMenu0Names [z ++], "MAP SCROLL WEST", 16);
    strncpy (ControlsMenu0Names [z ++], "MAP SCROLL NORTH", 17);
    strncpy (ControlsMenu0Names [z ++], "MAP SCROLL SOUTH", 17);
    strncpy (ControlsMenu0Names [z ++], "TARGET VOCALIZE", 16);
    strncpy (ControlsMenu0Names [z ++], "MUMBLE PTT", 11);
    strncpy (ControlsMenu0Names [z ++], "MUMBLE WHISPER", 15);
    strncpy (ControlsMenu0Names [z ++], "ZOOM FOV IN", 12);
    strncpy (ControlsMenu0Names [z ++], "ZOOM FOV OUT", 13);
    strncpy (ControlsMenu0Names [z ++], "RADAR ON/OFF", 13);
    strncpy (ControlsMenu0Names [z ++], "LOOK FWD", 9);
    strncpy (ControlsMenu0Names [z ++], "LOOK BACK", 10);
    strncpy (ControlsMenu0Names [z ++], "LOOK LEFT", 10);
    strncpy (ControlsMenu0Names [z ++], "LOOK RIGHT", 11);
    strncpy (ControlsMenu0Names [z ++], "LOOK UP", 8);
    
    int keys [38]; 
    keys [0]  = key_PRIMARY;
    keys [1]  = key_SECONDARY;
    keys [2]  = key_DROPFLARE;
    keys [3]  = key_DROPCHAFF;
    keys [4]  = key_WEAPONSELECT;
    keys [5]  = key_TARGETNEXT;
    keys [6]  = key_TARGETPREVIOUS;
    keys [7]  = key_ToggleViewIntExt;
    keys [8]  = key_FlapsUp;
    keys [9]  = key_FlapsDn;
    keys [10] = key_GunCameraToggle;
    keys [11] = key_WepAfterburner;
    keys [12] = key_SpeedBrake;
    keys [13] = key_Undercarriage;
    keys [14] = key_RadarZoomIn;
    keys [15] = key_RadarZoomOut;
    keys [16] = key_HudOnOff;
    keys [17] = key_IffOnOff;
    keys [18] = key_HudLadderBarsOnOff;
    keys [19] = key_MapViewOnOff;
    keys [20] = key_MapZoomIn;
    keys [21] = key_MapZoomOut;
    keys [22] = key_MapScrollEast;
    keys [23] = key_MapScrollWest;
    keys [24] = key_MapScrollNorth;
    keys [25] = key_MapScrollSouth;
    keys [26] = key_TARGETVOCALIZE;
    keys [27] = key_MumblePtt;
    keys [28] = key_MumbleWhisper;
    keys [29] = key_ZoomFovIn;
    keys [30] = key_ZoomFovOut;
    keys [31] = key_RadarOnOff;

    yf -=0.5; 

    for (i = 0; i < 25; i ++)
        { 
        editkey = new EditKey (ControlsMenu0Names [i]);
        editkey->setKey (keys [i]);
        editkey->setFunction (callbackKeyboard);
        editkey->setBounds (xf, yf, xfstep, yfstep - 0.1);
        controlsmenu [0]->add (editkey);
        yf -= yfstep;
        }
    yf =6; 
    xf = -1;

    for (i = 25; i < 31; i ++)   
        {
        editkey = new EditKey (ControlsMenu0Names [i]);
        editkey->setKey (keys [i]);
        editkey->setFunction (callbackKeyboard);
        editkey->setBounds (xf, yf, xfstep, yfstep - 0.1);
        controlsmenu [0]->add (editkey);
        yf -= yfstep;
        }
    yf -= 1;
    yfstep = 1.0;
    button = new Button ((char *)"LOAD DEFAULTS");
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [0]->add (button);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register mouse control submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xftab = 3.5;
    xf = xsubmenu + 1;
    yf = ysubmenu - 6;
    xfstep = 12.5;
    yfstep = 0.8;
    button = new Button ((char *)"SENSITIVITY");
    button->setFunction (callbackMouseSensitivity);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (button);
    sprintf (buf, "%d%%", (int) mouse_sensitivity);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"REVERSE");
    button->setFunction (callbackMouseReverse);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (button);
    if (mouse_reverse)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"RELATIVE");
    button->setFunction (callbackMouseRelative);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (button);
    if (mouse_relative)
        {
        sprintf (buf, "ON");
        }
    else
        {
        sprintf (buf, "OFF");
        }
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"AUTORUDDER");
    button->setFunction (callbackMouseAutorudder);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (button);
    sprintf (buf, "%d%%", mouse_autorudder);
    label = new Label (buf);
    label->setTransparent (true);
    label->setBounds (xf + xfstep - xftab, yf, 2, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    yf -= 0.5;
    textMouseButton (buf2, mouse_firecannon);
    sprintf (buf, "FIRE CANNON:            \t %s", buf2);
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    textMouseButton (buf2, mouse_firemissile);
    sprintf (buf, "FIRE MISSILE:            \t %s", buf2);
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    textMouseButton (buf2, mouse_selectmissile);
    sprintf (buf, "SELECT MISSILE:      \t %s", buf2);
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (label);
    yf -= yfstep;
    yf -= 1;
    yfstep = 1.0;
    button = new Button ((char *)"LOAD DEFAULTS");
    button->setFunction (callbackMouseDefaults);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [1]->add (button);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register joystick control submenu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    char joystr [4];
    xf = xsubmenu + 1;
    yf = ysubmenu - 6;
    xfstep = 12.5;
    yfstep = 0.8;
    button = new Button ((char *)"JOYSTICK AILERON");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_aileron, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"JOYSTICK ELEVATOR");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_elevator, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"JOYSTICK/THROTTLE");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_throttle, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"JOYSTICK/RUDDER");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_rudder, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"JOYSTICK VIEW X");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_view_x, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    button = new Button ((char *)"JOYSTICK VIEW Y");
    button->setFunction (callbackJoystickAxis);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    controlsmenu [2]->add (button);
    setJoystickAxisString (joystick_view_y, joystr);
    label = new Label (joystr);
    label->setBounds (xf + xfstep - xftab, yf, xftab, yfstep - 0.1);
    label->setTransparent (true);
    controlsmenu [2]->add (label);
    yf -= yfstep;
    
    yf -= 1;
    xfstep = 26.5;
    yfstep = 1.0;
    label = new Label ((char *)"");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"TO MAP JOYSTICK AXES, CLICK ON THE");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"JOYSTICK AXIS BUTTONS ABOVE.");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"TO MAP JOYSTICK BUTTONS, USE YOUR");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"FAVORITE TEXT EDITOR TO EDIT THE");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"LACCONTROLS.TXT FILE IN THE .LAC");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"FOLDER WITHIN YOUR HOME DIRECTORY.");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"TO MAP KEYBOARD KEYS, CLICK THE");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"CONTROLS BUTTON ABOVE, WHERE IT SAYS");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"NEXT PAGE.");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    yf-=yfstep;
    label = new Label ((char *)"");
    label->setBounds (xf-14, yf, xfstep, yfstep);
    controlsmenu [2]->add (label);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register fighter menu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xf = -14; 
    yf = 12;
    xfstep = 25; 
    yfstep = 1.2;
    sprintf (buf, "      SELECT YOUR AIRCRAFT:");
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    fightermenu.add (label);
    yf -= yfstep + 0.25;
    yfstep = 1.0;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    fightermenu.add (label);
    yf -= yfstep + 0.25;
    yfstep = 1.0;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    fightermenu.add (label);
    yf -= yfstep + 0.25;
    yfstep = 1.0;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    fightermenu.add (label);
    yf -= yfstep + 0.25;
    yfstep = 1.1;
    textfield = new TextField ((char *)"", yfstep - 0.1);
    textfield->setBounds (xf, yf, xfstep, (yfstep - 0.1) * 5 + 0.15);
    fightermenu.add (textfield);
    yf -= yfstep * 5 + 0.05;
    button = new Button ((char *)"<");
    button->setBounds (xf, yf, 1.5, 1.5);
    button->setFunction (callbackFighterPrev);
    fightermenu.add (button);
    button = new Button ((char *)">");
    button->setBounds (-xf -1.5, yf, 1.5, 1.5);
    button->setFunction (callbackFighterNext);
    fightermenu.add (button);
    textfield = new TextField ((char *)"    PREV/NEXT AIRCRAFT   ", yfstep - 0.1);
    textfield->setBounds (xf + 1.5, yf, 17, 1.5);  
    fightermenu.add (textfield);
    yf -= 13;
    sprintf (buf, "               BACK TO MAIN MENU");
    button = new Button (buf);
    button->setBounds (xf, yf, xfstep, yfstep - 0.1);
    button->setFunction (callbackSwitchMainMenu);
    fightermenu.add (button);
    yf -= yfstep + 0.25;
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register mission menu
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xf = -12.5;
    yf = 11;
    xfstep = 25;
    yfstep = 1.3;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, yfstep - 0.1);
    MissionMenu.add (label);
    yf -= yfstep + 0.15;
    yfstep = 1.0;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, 3.5);
    MissionMenu.add (label);
    yf -= 3.75;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep / 2 - 0.1, 5.7);
    MissionMenu.add (label);
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf + xfstep / 2 + 0.1, yf, xfstep / 2 - 0.1, 5.7);
    MissionMenu.add (label);
    yf -= 5.95;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep / 2 - 0.1, 6.8);
    MissionMenu.add (label);
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf + xfstep / 2 + 0.1, yf, xfstep / 2 - 0.1, 6.8);
    MissionMenu.add (label);
    yf -= 7.15;
    yfstep = 1.0;
    button = new Button ((char *)" BACK TO MAIN MENU");
    button->setBounds (xf, yf, xfstep / 2 - 0.1, yfstep - 0.1);
    button->setFunction (callbackSwitchMainMenu);
    MissionMenu.add (button);
    button = new Button ((char *)"    START MISSION");
    button->setBounds (xf + xfstep / 2 + 0.1, yf, xfstep / 2 - 0.1, yfstep - 0.1);
    button->setFunction (callbackSwitchStartMission);
    MissionMenu.add (button);
    /*
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Register quit dialog
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
    xf = -6;
    yf = 2;
    xfstep = 12;
    yfstep = 1.2;
    buf [0] = 0;
    label = new Label (buf);
    label->setBounds (xf, yf, xfstep, 3);
    quitmenu.add (label);
    yf -= 3.25;
    yfstep = 1.0;
    xfstep = 12;
    button = new Button ((char *)"    YES");
    button->setBounds (xf, yf, xfstep / 2 - 0.1, yfstep - 0.1);
    button->setFunction (callbackQuitNow);
    quitmenu.add (button);
    button = new Button ((char *)"     NO");
    button->setBounds (xf + xfstep / 2 + 0.1, yf, xfstep / 2 - 0.1, yfstep - 0.1);
    button->setFunction (callbackSwitchMainMenu);
    quitmenu.add (button);
    fightermenu.setVisible (false);
    MissionMenu.setVisible (false);
    quitmenu.setVisible (false);
    allmenus.setVisible (false);
    allmenus.visible = true;
    allmenus.components [0]->setVisible (true);
    } 

void createMission (int missionid)
    {
    mission = new MissionDemo1 (); 
    CurrentMissionNumber = (char)missionid;
    display (DebugBuf, LOG_MOST);
    if (missionnew != NULL)
        {
        delete missionnew;
        }
    if (missionid == MISSION_DEMO)
        {
        missionnew = new MissionDemo1 ();
        }
    else if (missionid == MISSION_TUTORIAL)
        {
        
        MissionNumber = 1;
        missionnew = new MissionTutorial1 ();
        }
    else if (missionid == MISSION_TUTORIAL2)
        {
        
        MissionNumber = 2;
        missionnew = new MissionTutorial2 ();
        }
    else if (missionid == MISSION_TUTORIAL3)
        {
        
        MissionNumber = 3;
        missionnew = new MissionTutorial3 ();
        }
    else if (missionid == MISSION_FREEFLIGHTWW2)
        {
        
        MissionNumber = 4;
        missionnew = new MissionFreeFlightWW2 ();
        }
    else if (missionid == MISSION_HEADTOHEAD00)
        {
        MissionNumber = 12;
        missionnew = new MissionHeadToHead00 ();
        }
    else if (missionid == MISSION_NETWORKBATTLE01)
        {
        MissionNumber = 13;
        display ((char*)"Setting missionnew to MissionNetworkBattle01", LOG_MOST);
        missionnew = new MissionNetworkBattle01 ();
        sound->haltMusic ();
        }
    else if (missionid == MISSION_NETWORKBATTLE02)
        {
        MissionNumber = 14;
        display ((char*)"Setting missionnew to MissionNetworkBattle02", LOG_MOST);
        missionnew = new MissionNetworkBattle02 ();
        sound->haltMusic ();
        }
    else if (missionid == MISSION_NETWORKBATTLE03)
        {
        MissionNumber = 15;
        display ((char*)"Setting missionnew to MissionNetworkBattle03", LOG_MOST);
        missionnew = new MissionNetworkBattle03 ();
        sound->haltMusic ();
        }
    } 

void credits_mouse (int button, int state, int x, int y)
    {
    display ((char*)"Function entry: credits_mouse()", LOG_MOST);
    void switch_menu ();
    switch_menu ();
    } 

void DisplayCredits ()
    {
    float yt = 0, zf = -2.4, ydist = 1.5;
    glPushMatrix ();
    glTranslatef (0, -3.5 + 0.014 * (float) CreditsTimer / (timestep*2.0), 0);
    CColor *col = &colorwhite;
    CColor *col2 = &coloryellow;
    float fontzoom = 1;
    font1->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"LINUX AIR COMBAT WAS WRITTEN BY", col2);
    font1->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"ROBERT BOSEN, USING A FRAMEWORK", col2);
    font1->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"DERIVING FROM GL-117 BY TOM DREXL.", col2);
    font1->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"NO MICROSOFT OR APPLE PRODUCTS WERE USED", col);
    font1->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"IN THE PRODUCTION OF THIS SOFTWARE.", col);
    font1->zoom = 0.08;
    font2->zoom = 0.06;
    fontzoom = 0.8;
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"NETWORK DEVELOPMENT SUPPORT: ADAM BOSEN", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"IL2 AUTOGUNNER CODE: HYRUM BOSEN", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"GRAPHICS SUPPORT: STEFANO PERIS, BLAKE WILLIAMS, TY WILLIAMS", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"MAKEFILE SUPPORT: MARCO DIEGO AURELIO MESQUITA", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"ALPHA TESTERS: TY WILLIAMS, BLAKE WILLIAMS, HYRUM BOSEN", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"PUBLIC-DOMAIN MUSIC CLIPS: ALEXANDER NAKARADA", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"DEVELOPMENT OF LINUX AIR COMBAT WAS GIVEN A", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"TREMENDOUS HEAD START BY AN AMAZING SET OF FREE", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"SOURCE CODE, OPERATING SYSTEMS, LANGUAGES, AND", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"TOOLS. IN ADDITION TO THE LINUX OPERATING SYSTEM", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"BY LINUS TORVALDS AND GNU C AND CLIBS BY RICHARD", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"STALLMAN AND THE GNU PROJECT, SPECIAL THANKS GO TO MANY", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"OTHER PREDECESSORS IN THE GL-117 PROJECT,", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"INCLUDING:", col);
    font2->drawTextCentered (0, (yt -= 4 * ydist) / fontzoom, zf, (char *)"TECHNICAL UNIVERSITY OF MUNICH, THOMAS A. DREXL, PIOTER", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"PAWLOW, WOLFGANG HOMMEL, LOURENS VEEN, ARNE REINERS", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"AND COUNTLESS OTHERS WHO CREATED THE UNDERLYING LINUX", col);
    font2->drawTextCentered (0, (yt -= 2 * ydist) / fontzoom, zf, (char *)"GRAPHIC AND MEDIA LIBRARIES OPENGL, GLUT, SDL, ETC.", col);
    font2->zoom = 0.1;
    font2->zoom = 0.1;
    glPopMatrix ();
    } 

void DisplayFighter ()
    {
    char *getModelName (int id);
    void drawMouseCursor ();
    allmenus.setVisible (false);
    fightermenu.setVisible (true); 
    fightermenu.draw (); 
    AIObj ffighter;
    char buf [256];
    CVector3 vec;
    CVector3 tl;
    tl.y = -0.08; 
    tl.z = -0.3; 
    CRotation rot;
    rot.a = 300;
    rot.b = 0;
    rot.c = (5 + MissionMenuTimer / timestep) % 360;
    CModel *model = NULL;
    int id = 0;
    model = &model_figBT;
    id = FIGHTER_P38F;
    if (SelectedAircraft == 1)
        { 
        model = &model_figAY;
        id = FIGHTER_DW520;
        }
    else if (SelectedAircraft == 2)
        { 
        model = &model_figBH;
        id = BOMBER_DORNIER;
        }
    else if (SelectedAircraft == 3)
        { 
        model = &model_figz;
        id = FIGHTER_FW190;
        }
    else if (SelectedAircraft == 4)
        { 
        model = &model_figBI;
        id = BOMBER_HE111;
        }
    else if (SelectedAircraft == 5)
        { 
        model = &model_figAA;
        id = BOMBER_JU87;
        }
    else if (SelectedAircraft == 6)
        { 
        model = &model_figBJ;
        id = BOMBER_JU88;
        }
    else if (SelectedAircraft == 7)
        { 
        model = &model_figBR;
        id = FIGHTER_ME109F;
        }
    else if (SelectedAircraft == 8)
        { 
        model = &model_figAD;
        id = FIGHTER_ME109G;
        }
    else if (SelectedAircraft == 9)
        { 
        model = &model_figBG;
        id = FIGHTER_ME110;
        }
    else if (SelectedAircraft == 10)
        { 
        model = &model_figBB;
        id = FIGHTER_ME163;
        }
    else if (SelectedAircraft == 11)
        { 
        model = &model_figAC;
        id = FIGHTER_FIATG55;
        }
    else if (SelectedAircraft == 12)
        { 
        model = &model_figAS;
        id = FIGHTER_MACCIC202;
        }
    else if (SelectedAircraft == 13)
        { 
        model = &model_figBD;
        id = FIGHTER_D3A;
        }
    else if (SelectedAircraft == 14)
        { 
        model = &model_figAI;
        id = FIGHTER_N1K1;
        }
    else if (SelectedAircraft == 15)
        { 
        model = &model_figBL;
        id = FIGHTER_KI61;
        }
    else if (SelectedAircraft == 16)
        { 
        model = &model_figv;
        id = FIGHTER_A6M2;
        }
    else if (SelectedAircraft == 17)
        { 
        model = &model_figBN;
        id = FIGHTER_A6M5;
        }
    else if (SelectedAircraft == 18)
        { 
        model = &model_figAL;
        id = BOMBER_G5M;
        }
    else if (SelectedAircraft == 19)
        { 
        model = &model_figBE;
        id = BOMBER_B5N;
        }
    else if (SelectedAircraft == 20)
        { 
        model = &model_figAG;
        id = FIGHTER_KI43;
        }
    else if (SelectedAircraft == 21)
        { 
        model = &model_figBK;
        id = FIGHTER_KI84;
        }
    else if (SelectedAircraft == 22)
        { 
        model = &model_figAT;
        id = BOMBER_LANCASTER;
        }
    else if (SelectedAircraft == 23)
        { 
        model = &model_figAU;
        id = BOMBER_MOSQUITOB;
        }
    else if (SelectedAircraft == 24)
        { 
        model = &model_figAF;
        id = FIGHTER_HURRICANE;
        }
    else if (SelectedAircraft == 25)
        { 
        model = &model_figBC;
        id = FIGHTER_TEMPEST;
        }
    else if (SelectedAircraft == 26)
        { 
        model = &model_figAV;
        id = FIGHTER_TYPHOON;
        }
    else if (SelectedAircraft == 27)
        { 
        model = &model_figBO;
        id = FIGHTER_SPIT5;
        }
    else if (SelectedAircraft == 28)
        { 
        model = &model_figi;
        id = FIGHTER_SPIT9;
        }
    else if (SelectedAircraft == 29)
        { 
        model = &model_figAK;
        id = FIGHTER_P39;
        }
    else if (SelectedAircraft == 30)
        { 
        model = &model_figu;
        id = BOMBER_B17;
        }
    else if (SelectedAircraft == 31)
        { 
        model = &model_figAX;
        id = BOMBER_B29;
        }
    else if (SelectedAircraft == 32)
        { 
        model = &model_figAJ;
        id = BOMBER_B24;
        }
    else if (SelectedAircraft == 33)
        { 
        model = &model_figAE;
        id = FIGHTER_P40;
        }
    else if (SelectedAircraft == 34)
        { 
        model = &model_figAZ;
        id = BOMBER_SB2C;
        }
    else if (SelectedAircraft == 35)
        { 
        model = &model_figBF;
        id = BOMBER_DAUNTLESS;
        }
    else if (SelectedAircraft == 36)
        { 
        model = &model_figAO;
        id = FIGHTER_F4F;
        }
    else if (SelectedAircraft == 37)
        { 
        model = &model_figx;
        id = FIGHTER_F6F;
        }
    else if (SelectedAircraft == 38)
        { 
        model = &model_figBA;
        id = BOMBER_TBF;
        }
    else if (SelectedAircraft == 39)
        { 
        model = &model_figBS;
        id = FIGHTER_P38F;
        }
    else if (SelectedAircraft == 40)
        { 
        model = &model_fig;
        id = FIGHTER_P38L;
        }
    else if (SelectedAircraft == 41)
        { 
        model = &model_figAN;
        id = BOMBER_B26;
        }
    else if (SelectedAircraft == 42)
        { 
        model = &model_figAM;
        id = BOMBER_B25;
        }
    else if (SelectedAircraft == 43)
        { 
        model = &model_figBP;
        id = FIGHTER_P51B;
        }
    else if (SelectedAircraft == 44)
        { 
        model = &model_figc;
        id = FIGHTER_P51D;
        }
    else if (SelectedAircraft == 45)
        { 
        model = &model_figBQ;
        id = FIGHTER_P47B;
        }
    else if (SelectedAircraft == 46)
        { 
        model = &model_figw;
        id = FIGHTER_P47D;
        }
    else if (SelectedAircraft == 47)
        { 
        model = &model_figy;
        id = FIGHTER_F4U;
        }
    else if (SelectedAircraft == 48)
        { 
        model = &model_figAR;
        id = FIGHTER_IL2;
        }
    else if (SelectedAircraft == 49)
        { 
        model = &model_figAP;
        id = FIGHTER_LA5;
        }
    else if (SelectedAircraft == 50)
        { 
        model = &model_figAQ;
        id = FIGHTER_LA7;
        }
    else if (SelectedAircraft == 51)
        { 
        model = &model_figAB;
        id = FIGHTER_IL16;
        }
    else if (SelectedAircraft == 52)
        { 
        model = &model_figAW;
        id = FIGHTER_YAK1;
        }
    else if (SelectedAircraft == 53)
        { 
        model = &model_figAH;
        id = FIGHTER_YAK9;
        }
    else if (SelectedAircraft == 54)
        { 
        model = &model_figb;
        id = FIGHTER_HAWK;
        }
    else if (SelectedAircraft == 55)
        { 
        model = &model_figBM;
        id = FIGHTER_GENERIC01;
        }
    PlayerAircraftType = id;
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    
    if (NoMissionHasYetCommenced)
       { 
       model->draw (&vec, &tl, &rot, 0.11, 0.5, 0);
       }
    else
       { 
       }
    glDisable (GL_LIGHTING);
    glDisable (GL_DEPTH_TEST);
    ffighter.o = model;
    ffighter.newinit (id, 1, 0);
    float fontzoom = 0.7;
    float textx = -13.75; 
    font1->zoom = 0.07;
    font1->drawText (textx / fontzoom, 9.7 / fontzoom, -2, getModelName (id), &menu_colwhite);
    float yf = 8.5;
    if (
        ffighter.id == FIGHTER_HAWK
        || ffighter.id == FIGHTER_A6M2
        || ffighter.id == FIGHTER_P51D
        || ffighter.id == FIGHTER_F6F
        || ffighter.id == FIGHTER_F4U
        || ffighter.id == FIGHTER_FW190
        || ffighter.id == FIGHTER_IL16
        || ffighter.id == FIGHTER_SPIT9
        || ffighter.id == FIGHTER_FIATG55
        || ffighter.id == FIGHTER_ME109G
        || ffighter.id == FIGHTER_P39
        || ffighter.id == FIGHTER_P40
        || ffighter.id == FIGHTER_HURRICANE
        || ffighter.id == FIGHTER_KI43
        || ffighter.id == FIGHTER_YAK9
        || ffighter.id == FIGHTER_N1K1
        || ffighter.id == FIGHTER_F4F
        || ffighter.id == FIGHTER_LA5
        || ffighter.id == FIGHTER_LA7
        || ffighter.id == FIGHTER_MACCIC202
        || ffighter.id == FIGHTER_YAK1
        || ffighter.id == FIGHTER_DW520
        || ffighter.id == FIGHTER_ME163
        || ffighter.id == FIGHTER_D3A
        || ffighter.id == FIGHTER_ME110
        || ffighter.id == FIGHTER_KI84
        || ffighter.id == FIGHTER_KI61
        || ffighter.id == FIGHTER_GENERIC01
        || ffighter.id == FIGHTER_A6M5
        || ffighter.id == FIGHTER_SPIT5
        || ffighter.id == FIGHTER_P51B
        || ffighter.id == FIGHTER_P47B
        || ffighter.id == FIGHTER_ME109F
        || ffighter.id == FIGHTER_P38F
       )
        {
        strncpy (buf, "HISTORIC ROLE: FIGHTER", 23);
        }
    if (ffighter.id == BOMBER_JU87 ||
        ffighter.id == FIGHTER_D3A ||
        ffighter.id == BOMBER_SB2C ||
        ffighter.id == BOMBER_TBF
        )
        {
        strncpy (buf, "HISTORIC ROLE: DIVE-BOMBER", 27);
        }
    if (ffighter.id == BOMBER_B25        ||
        ffighter.id == BOMBER_B26        ||
        ffighter.id == BOMBER_MOSQUITOB  ||
        ffighter.id == FIGHTER_IL2       ||
        ffighter.id == BOMBER_B5N        ||
        ffighter.id == BOMBER_DAUNTLESS  ||
        ffighter.id == BOMBER_DORNIER    ||
        ffighter.id == BOMBER_JU88
        )
        {
        strncpy (buf, "HISTORIC ROLE: TACTICAL/MEDIUM BOMBER", 38);
        }
    if (ffighter.id == BOMBER_B17       ||
        ffighter.id == BOMBER_B24       ||
        ffighter.id == BOMBER_B29       ||
        ffighter.id == BOMBER_G5M       ||
        ffighter.id == BOMBER_LANCASTER ||
        ffighter.id == BOMBER_HE111
        )
        {
        strncpy (buf, "HISTORIC ROLE: STRATEGIC/HEAVY BOMBER", 38);
        }
    if (ffighter.id == FIGHTER_TYPHOON  ||
        ffighter.id == FIGHTER_TEMPEST  ||
        ffighter.id == FIGHTER_P47D     ||
        ffighter.id == FIGHTER_P38L
        )
        {
        strncpy (buf, "HISTORIC ROLE: FIGHTER/BOMBER", 30);
        }
    if (ffighter.id == FIGHTER_ME163)
       {
       strncpy (buf, "HISTORIC ROLE: SHORT-RANGE INTERCEPTOR", 39);
       }

    font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
    yf -= 0.3; 
    if (ffighter.id == FIGHTER_P38L)
       {
       yf -= 1.0;
       strncpy (buf, "SPEED: 414   CLIMB: 4040 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT:  4X50CAL MG AND 1X20MM CANNON,\nALL GUNS CENTER-MOUNTED FOR EASIER AIMING.\n10XROCKETS, 2X500LB BOMBS. USE DIVE FLAPS TO\nMAINTAIN CONTROL AT HIGH SPEED.", 164);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P38L;
       }
    else if (ffighter.id == FIGHTER_P51D)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 442   CLIMB: 3200 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL, 6XROCKETS, 2X500LB BOMBS", 44);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P51D;
       }
    else if (ffighter.id == FIGHTER_A6M2)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 346   CLIMB: 3100 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: EXCELLENT", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: POOR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X30CAL, 1X500LB BOMB.\nNOTE: THE PERFORMANCE OF THIS AIRCRAFT\nDOMINATED THE EARLY PACIFIC WAR AND SHOCKED\nTHE AMERICANS AT PEARL HARBOR AND MANILLA.", 170);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_A6M2;
       }
    else if (ffighter.id == FIGHTER_HAWK)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 740   CLIMB: 13000 FT/MIN", 33);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: GOOD.\nNOTE: THIS SIMULATED PLANE IS NOT HISTORICALLY\nBASED AND IS NOT WELL RESEARCHED. IT SHOULD\nNOT BE USED IN ONLINE COMPETITION.", 200);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_HAWK;
       }
    else if (ffighter.id == FIGHTER_SPIT9)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 415   CLIMB: 3860 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 28);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 4X30CAL", 26);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_SPIT9;
       }
    else if (ffighter.id == FIGHTER_ME109G)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 398   CLIMB: 3345 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: FAIR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X30MM (CENTERLINE), 2X50CAL, \n2XROCKETS, 1X500LB BOMB", 66);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_ME109G;
       }
    else if (ffighter.id == FIGHTER_P39)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 379   CLIMB: 3600 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X37MM FIRING THRU NOSE, 4X50CAL, \n1XBOMB. NOTE: THE P39'S PLANNED SUPERCHARGER\nWAS NEVER INSTALLED SO PERFORMANCE ABOVE \n15000 FEET IS POOR.", 155);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P39;
       }
    else if (ffighter.id == FIGHTER_P40)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 360   CLIMB: 2100 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL, 3X500LB BOMBS", 33);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P40;
       }
    else if (ffighter.id == FIGHTER_HURRICANE)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 340   CLIMB: 2780 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 4X20MM, 2X500LB BOMBS", 32);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_HURRICANE;
       }
    else if (ffighter.id == FIGHTER_KI43)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 333   CLIMB: 3900 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: EXCELLENT", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: POOR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X50CAL, 2X500LB BOMBS", 33);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_KI43;
       }
    else if (ffighter.id == BOMBER_B17)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 292   CLIMB: 2300 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 13X50CAL, 15X500LB BOMBS", 35);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B17;
       }
    else if (ffighter.id == FIGHTER_P47D)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 433   CLIMB: 3180 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 8X50CAL, 8XROCKETS, 2X500LB BOMBS.", 45);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P47D;
       }
    else if (ffighter.id == FIGHTER_F4F)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 331   CLIMB: 2200 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: EXCELLENT", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL", 18);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_F4F;
       }
    else if (ffighter.id == FIGHTER_F6F)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 391   CLIMB: 3500", 25);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL, 6XROCKETS, 1X500LB BOMB", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_F6F;
       }
    else if (ffighter.id == FIGHTER_F4U)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 431   CLIMB: 3760 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL, 8XROCKETS, 1X500LB BOMB", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_F4U;
       }
    else if (ffighter.id == FIGHTER_FW190)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 408   CLIMB: 2953 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 4X20MM, 2X50CAL", 26);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_FW190;
       }
    else if (ffighter.id == BOMBER_JU87)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 255   CLIMB: 900 FPM LOADED", 35);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "  EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X50CAL REAR-FACING, \n4X500LB BOMBS", 55);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_JU87;
       }
    else if (ffighter.id == BOMBER_G5M)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 265   CLIMB: 1800 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: POOR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM (TAIL GUN), 4X30CAL, \n4X500LB BOMBS", 54);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_G5M;
       }
    else if (ffighter.id == BOMBER_B25)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 270  CLIMB: 1400 FPM UNLOADED ", 38);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "  EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "  LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "  POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 18X50CAL, 1X 57MM, 8XROCKETS,\n6X500LB BOMBS", 55);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B25;
       }
    else if (ffighter.id == BOMBER_B26)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 299   CLIMB: 2300 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 11x50CAL, 8X500LB BOMBS.\nNOTE: THIS AIRCRAFT'S SHORT WINGS\nMAKE IT DIFFICULT TO CONTROL AT LOW SPEED.\nLANDINGS ARE TRICKY!", 136);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B26;
       }
    else if (ffighter.id == FIGHTER_IL16)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 275   CLIMB: 2500 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: EXCELLENT", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X50CAL, 6XROCKETS", 37);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_IL16;
       }
    else if (ffighter.id == FIGHTER_FIATG55)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 417   CLIMB: 3300 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT:  1X20MM (CENTER MOUNT), 4X50CAL", 42);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_FIATG55;
       }
    else if (ffighter.id == FIGHTER_YAK9)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 417   CLIMB: 3280 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: FAIR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM, 2X50 CAL", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_YAK9;
       }
    else if (ffighter.id == FIGHTER_N1K1)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 408   CLIMB: 4000 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM AND 2X50CAL", 29);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_N1K1;
       }
    else if (ffighter.id == BOMBER_B24)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 290   CLIMB: 2250 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 10X50CAL, 16X500LB", 29);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B24;
       }
    else if (ffighter.id == FIGHTER_LA5)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 403   CLIMB: 3280 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X500LB BOMBS", 32);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_LA5;
       }
    else if (ffighter.id == FIGHTER_LA7)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 411   CLIMB: 3095 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 3X20MM, 1X500LB BOMB", 31);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_LA7;
       }
    else if (ffighter.id == FIGHTER_IL2)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 257   CLIMB: 2050 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X23MM, 2X30CAL, 1X50CAL (REAR), \n2X500LB BOMBS, 8XROCKETS", 70);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_IL2;
       }
    else if (ffighter.id == FIGHTER_MACCIC202)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 372   CLIMB: 3563 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X50CAL, 2X30CAL, 1X500LB BOMB", 41);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_MACCIC202;
       }
    else if (ffighter.id == BOMBER_LANCASTER)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 260   CLIMB: 1100 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 8X30CAL, 28X500LB BOMBS", 34);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_LANCASTER;
       }
    else if (ffighter.id == BOMBER_MOSQUITOB)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 415   CLIMB: 2850 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD (FOR A BOMBER)", 37);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: THIS IS THE BOMBER VERSION. IT\nCARRIES 8X500LB BOMBS, BUT NO GUNS,\nRELYING ON ITS HIGH SPEED FOR DEFENSE.", 118);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_MOSQUITOB;
       }
    else if (ffighter.id == FIGHTER_TYPHOON)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 412   CLIMB: 274O FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: AVERAGE", 25);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 4X20MM, 8XROCKETS, 2X500LB BOMBS", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_TYPHOON;
       }
    else if (ffighter.id == FIGHTER_YAK1)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 368   CLIMB: 3038 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: FAIR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM, 2X50CAL", 26);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_YAK1;
       }
    else if (ffighter.id == BOMBER_B29)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 357   CLIMB: 900 FT/MIN", 31);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 40X500LB, 10X50CAL IN REMOTE-\nCONTROL TURRETS, 2X50CAL (TAIL GUNNER)", 80);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B29;
       }
    else if (ffighter.id == FIGHTER_DW520)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 347   CLIMB: 2820 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM, 4X30CAL", 26);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_DW520;
       }
    else if (ffighter.id == BOMBER_SB2C)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 295   CLIMB: 1800 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 4X500LB BOMBS, 2X20MM IN WINGS,\n2X50CAL (REAR GUNNER),\n8X ROCKETS", 78);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_SB2C;
       }
    else if (ffighter.id == BOMBER_TBF)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 275   CLIMB: 2060 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: POOR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X50CAL IN WINGS,\n2X50CAL DORSAL TURRET,\n1X30CAL REAR VENTRAL,\n4X500LB, 8XROCKETS", 95);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_TBF;
       }
    else if (ffighter.id == FIGHTER_ME163)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 600  CLIMB: 16000 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY POOR DUE TO VOLATILE FUEL", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X30MM\nNOTE: THIS ROCKET-POWERED AIRCRAFT\nHOLDS FUEL FOR ONLY 7.5 MINUTES THRUST\nAT FULL POWER.", 109);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_ME163;
       }
    else if (ffighter.id == FIGHTER_TEMPEST)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 432   CLIMB: 4700 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 4X20MM, 2X500LB BOMBS,\n8XROCKETS", 44);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_TEMPEST;
       }
    else if (ffighter.id == FIGHTER_D3A)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 267   CLIMB: 1869 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X30CAL FORWARD-FIRING\n1X30CAL REAR-FIRING\n2X500LB BOMBS", 69);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_D3A;
       }
    else if (ffighter.id == BOMBER_B5N)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 235   CLIMB: 1283 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X30CAL IN WINGS,\n1X30CAL (REAR GUNNER), 2X500LB BOMBS", 66);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_B5N;
       }
    else if (ffighter.id == BOMBER_DAUNTLESS)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 255   CLIMB: 1700 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: VERY GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X50CAL CENTER-MOUNT FWD,\n2X50CAL REAR GUNNER, 4X500LB BOMBS", 72);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_DAUNTLESS;
       }
    else if (ffighter.id == FIGHTER_ME110)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 370   CLIMB: 2500 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: AVERAGE", 25);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM CANNONS, 4X30CAL MG,\n2X30CAL (REAR GUNNER)", 61);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_ME110;
       }
    else if (ffighter.id == BOMBER_DORNIER)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 255   CLIMB: 1600 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X30CAL IN CUPOLA, 5X500LB BOMBS", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_DORNIER;
       }
    else if (ffighter.id == BOMBER_HE111)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 273   CLIMB: 800 FT/MIN", 31);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 7X30CAL MG, \n12X500LB BOMBS", 39);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_HE111;
       }
    else if (ffighter.id == BOMBER_JU88)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 317   CLIMB: 770 FT/MIN", 31);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X30CAL MG IN CUPOLA,\n6X500LB BOMBS", 47);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = BOMBER_JU88;
       }
    else if (ffighter.id == FIGHTER_KI84)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 426   CLIMB: 4300 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: AVERAGE", 25);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X50CAL,\n3X500LB BOMBS", 42);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_KI84;
       }
    else if (ffighter.id == FIGHTER_KI61)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 368   CLIMB: 2990 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X50CAL,\n1X500LB BOMB", 41);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_KI61;
       }
    else if (ffighter.id == FIGHTER_GENERIC01)
       {
       yf -= 1;
       strncpy (buf, "SPEED: AVERAGE   CLIMB: AVERAGE", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "MANEUVERABILITY AVERAGE", 24);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "LETHALITY: AVERAGE", 19);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_GENERIC01;
       }
    else if (ffighter.id == FIGHTER_A6M5)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 355   CLIMB: 3275 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2",15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: EXCELLENT", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: POOR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X20MM, 2X30CAL, 1X500LB BOMB", 40);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_A6M5;
       }
    else if (ffighter.id == FIGHTER_SPIT5)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 371   CLIMB: 3250 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: AVERAGE", 20);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 2X 20MM, 4X30CAL", 27);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_SPIT5;
       }
    else if (ffighter.id == FIGHTER_P51B)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 426   CLIMB: 3450 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 6X50CAL, 4XROCKETS, 1X500LB BOMB", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P51B;
       }
    else if (ffighter.id == FIGHTER_P47B)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 402   CLIMB: 2100 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: FAIR", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: EXCELLENT", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 8X50CAL, 6XROCKETS, 1X500LB BOMB", 43);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P47B;
       }
    else if (ffighter.id == FIGHTER_ME109F)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 385   CLIMB: 3100 FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1.3;
       strncpy (buf, "MANEUVERABILITY: GOOD", 22);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: FAIR", 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM (CENTERLINE), 2X50CAL, \n1X500LB BOMB", 55);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_ME109F;
       }
    else if (ffighter.id == FIGHTER_P38F)
       {
       yf -= 1;
       strncpy (buf, "SPEED: 408   CLIMB:  3300FT/MIN", 32);
       if (ffighter.HistoricPeriod == 1)
           {
           strncat (buf, "     EARLY WW2", 15);
           }
       else if (ffighter.HistoricPeriod == 2)
           {
           strncat (buf, "     LATE WW2", 14);
           }
       else if (ffighter.HistoricPeriod == 3)
           {
           strncat (buf, "     POST WW2", 14);
           }
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "MANEUVERABILITY: VERY GOOD AT SPEEDS < 400.", 44);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "DURABILITY: GOOD" , 17);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       yf -= 1;
       strncpy (buf, "ARMAMENT: 1X20MM CANNON, 4X50CAL. ALL GUNS\nCENTER-MOUNTED. 1X500LB BOMB, 6XROCKETS. \nNOTE: THIS EARLY P38 HAS NO DIVE FLAPS.\nUSE EXTREME CAUTION IN HIGH-SPEED DIVES!", 169);
       font1->drawText (textx / fontzoom, yf / fontzoom, -2, buf, &menu_colwhite);
       DefaultAircraft = FIGHTER_P38F;
       }
    else
       { 
       display ((char*)"DisplayFighter() cannot provide details for undefined aircraft.", LOG_MOST);
       DefaultAircraft = FIGHTER_P38L;
       }
    font1->zoom = 0.1;
    drawMouseCursor ();
    } 

void DisplayGame ()
    {
    void drawMouseCursor ();
    int i;
    double sunx = 0, suny = 0, sunz;

    bool sunvisible = false;
    float pseudoview = getView ();
    float mycamtheta = camtheta, mycamphi = camphi + view_x , mycamgamma = camgamma + view_y;
    if (dithering)
        {
        glEnable (GL_DITHER);
        }
    else
        {
        glDisable (GL_DITHER);
        }
    if (vibration > 0)
        {
        if (camera == 0)
            {
            
            float sinvib = sin ((float) vibration / timestep);
            mycamphi += 0.2 * vibration * sinvib / timestep;
            mycamgamma += 0.2 * vibration * sinvib / timestep;
            }
        }
    // calculate light factor
    if (camera == 0 && SunBlinding && day && weather == WEATHER_SUNNY)
        {
        float np = fplayer->phi - 180;

        if (np >= 180)
            {
            np -= 360;
            }
        float sunfactor = fabs (np) + fabs (fplayer->gamma - 180 - sungamma);
        if (sunfactor < 50)
            {
            sunfactor = (50 - sunfactor) / 10;

            if (sunfactor < 1.0)
                {
                sunfactor = 1.0;
                }
            }
        else
            {
            sunfactor = 1.0;
            }

        sunlight_dest = sunfactor;

        if (game == GAME_PLAY)
            {
            pseudoview /= sunfactor;
            }
        }
    else
        {
        if (day)
            {
            sunlight_dest = 1.0;
            }
        else
            {
            sunlight_dest = 0.75;
            }

        if (flash > 0)
            {
            sunlight_dest = (float) flash / timestep;
            }
        }
    
    if (dynamiclighting)
        {
        for (i = 0; i < maxfighter; i ++)
            {
            if (ThreeDObjects [i]->draw)
                if (ThreeDObjects [i]->explode > 0)
                    if (ThreeDObjects [i] != fplayer)
                        {
                        float dgamma = fabs (fplayer->getAngleH (ThreeDObjects [i]));
                        float dphi = fabs (fplayer->getAngle (ThreeDObjects [i]));
                        if (dphi < 45 && dgamma < 45)
                            {
                            float ddist = fplayer->distance (ThreeDObjects [i]);
                            if (ddist < 40)
                                {
                                ddist /= 15;
                                ddist ++;
                                dphi /= 25;
                                dphi ++;
                                dgamma /= 25;
                                dgamma ++;
                                if (ThreeDObjects [i]->explode < 8 * timestep)
                                    {
                                    sunlight_dest = (float) ThreeDObjects [i]->explode / timestep * 4 / ddist / dphi / dgamma;
                                    }
                                else if (ThreeDObjects [i]->explode < 16 * timestep)
                                    {
                                    sunlight_dest = (16.0 - ThreeDObjects [i]->explode / timestep) * 4 / ddist / dphi / dgamma;
                                    }
                                }
                            }
                        }
            }
        }

    if (game != GAME_PLAY && sunlight > 0.9F)
        {
        sunlight = 0.9F;
        }
    // start rendering
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glScalef (ScreenFOVx, ScreenFOVy, ScreenFOVz); 
    glShadeModel (GL_SMOOTH);
    glPushMatrix ();
    glRotatef (-mycamtheta, 0.0, 0.0, 1.0);
    glRotatef (mycamgamma, 1.0, 0.0, 0.0);
    glRotatef (-mycamphi, 0.0, 1.0, 0.0);
    // draw sky
    glDisable (GL_FOG);
    glDisable (GL_DEPTH_TEST);
    float mylight = sunlight;
    if (!day)
        {
        mylight /= 0.75;
        }
    if (mylight > 1.0 && day)
        {
        mylight = mylight / 5.0 + 0.8;
        }
    else if (mylight > 1.0 && !day)
        {
        mylight = mylight / 5.0 + 0.8;
        }
    gl->foglum = mylight;
    
    sphere->drawGL (tlminf, tlinf, tlnull, space->alpha, mylight, true, false);
    if (weather == WEATHER_SUNNY || weather == WEATHER_CLOUDY)
        {
        if (!day)
            {
            glPointSize (LINEWIDTH(1.0F));
            int stars = maxstar;
            if (weather != WEATHER_CLOUDY)
                {
                stars = maxstar / 2;
                }
            for (i = 0; i < stars; i ++)
                {
                glPushMatrix ();
                glRotatef (star [i]->phi, 0.0, 1.0, 0.0);
                glRotatef (star [i]->gamma, 1.0, 0.0, 0.0);
                glTranslatef (0, 0, -10);
                star [i]->draw ();
                glPopMatrix ();
                }
            }
        }
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_FOG); // To disable fog altogether, eliminate this line and the other glEnable (GL_FOG) line in gl.cpp
    if (quality >= 2 && clouds > 0 && view !=9)   
        {
        cloudfog = pseudoview + 40;
        if (cloudfog > 150)
            {
            cloudfog = 150;
            }
        gl->enableFog (cloudfog);
        
        highclouds->zoom = 390;
        
        float ch2 = -370 - fplayer->tl->y / 10.0; 
        CVector3 tlsphere2 (0, ch2, 0);
        highclouds->drawGL (&tlsphere2, fplayer->tl);
        glDisable (GL_FOG);
        }
    // draw sun or moon (or earth)
    if (weather == WEATHER_SUNNY || weather == WEATHER_CLOUDY)
        {
        glRotatef (180, 0.0, 1.0, 0.0);
        if (camera == 0)
            {
            glRotatef (sungamma, 1.0, 0.0, 0.0);
            }
        else
            {
            glRotatef (mycamgamma + sungamma, 1.0, 0.0, 0.0);
            }
        float zf = -11;
        if (day)
            {
            zf = -10;
            }
        glTranslatef (0, 0, zf);
        gl->extractFrustum ();
        if  (
            specialeffects &&
                (
                gl->isPointInFrustum (-1, 1, 0) ||
                gl->isPointInFrustum (-1, -1, 0) ||
                gl->isPointInFrustum (1, -1, 0) ||
                gl->isPointInFrustum (1, 1, 0)
                )
            )
            {
            glDisable (GL_DEPTH_TEST);

            if (day)
                {
                gl->enableTextures (texsun->textureID);
                }
            else
                {
                gl->enableTextures (texearth->textureID);
                }

            if (day && l->type != 1)
                {
                glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                }

            gl->enableAlphaBlending ();
            glEnable (GL_ALPHA_TEST);
            float alphamax = 0.1;

            if (day)
                {
                alphamax = 0.1;
                }

            glAlphaFunc (GL_GEQUAL, alphamax);
            glBegin (GL_QUADS);

            if (day)
                {
                float gm = (40.0 - sungamma) / 80.0;

                if (gm < 0)
                    {
                    gm = 0;
                    }

                if (gm > 0.5)
                    {
                    gm = 0.5;
                    }

                glColor4f (1.0, 1.0 - gm/2, 0.95 - gm/2, 1.0); 
                }
            else
                {
                glColor4f (1.0, 1.0, 1.0, 1.0);
                }

            glTexCoord2d (0, 1);
            glVertex3f (-1, 1, 0);
            glTexCoord2d (1, 1);
            glVertex3f (1, 1, 0);
            glTexCoord2d (1, 0);
            glVertex3f (1, -1, 0);
            glTexCoord2d (0, 0);
            glVertex3f (-1, -1, 0);
            glEnd ();
            
            double proj[16];
            double modl[16];
            int vp[4];
            glGetDoublev( GL_PROJECTION_MATRIX, proj );
            glGetDoublev( GL_MODELVIEW_MATRIX, modl );
            glGetIntegerv( GL_VIEWPORT, vp );
            gluProject (0, 0, 0, modl, proj, vp, &sunx, &suny, &sunz);

            if ((sunx>=vp[0])&&(suny>=vp[1])&&(sunx<(vp[0]+vp[2]))&&(suny<(vp[1]+vp[3])))
                {
                sunvisible = true;
                }

            glDisable (GL_ALPHA_TEST);
            glEnable (GL_DEPTH_TEST);
            glDisable (GL_TEXTURE_2D);
            gl->disableAlphaBlending ();
            }
        }
    glPopMatrix ();
    glPushMatrix ();
    glRotatef (-mycamtheta, 0.0, 0.0, 1.0);  
    glRotatef (mycamgamma, 1.0, 0.0, 0.0);
    glRotatef (-mycamphi, 0.0, 1.0, 0.0);
    glDisable (GL_FOG);
    glScalef (1.0, 1.0, 1.0);
    glTranslatef (-camx, -camy, -camz);
    if (camera != 50 && camera !=9)
        {
        
        gl->enableFog (pseudoview); 
        }
    // draw terrain
    l->calcDynamicLight (explosion, MachineGunBullet, (DynamicObj **) missile, flare);
    glEnable (GL_CULL_FACE);
    glCullFace (GL_FRONT);
    l->draw ((int) mycamphi, (int) (-mycamgamma + 180.0));
    glDisable (GL_CULL_FACE);
    // draw objects
    
    gl->extractFrustum ();
    if (camera != 50)
        {
        space->lum = sunlight;
        float dayfac = 1.0;
        if (!day)
            {
            dayfac = 0.5;
            }
        if (weather == WEATHER_SUNNY || weather == WEATHER_CLOUDY)
            {
            for (i = 0; i < space->no; i ++)
                {
                if (space->o [i]->tl->y < l->getExactRayHeight (space->o [i]->tl->x, space->o [i]->tl->z))
                    {
                    space->o [i]->lum = 0.5 * dayfac;
                    }
                else
                    {
                    space->o [i]->lum = 1.0 * dayfac;
                    }
                }
            }
        else
            {
            for (i = 0; i < space->no; i ++)
                {
                space->o [i]->lum = dayfac;
                }
            }
        if (flash > 7 * timestep)
            {
            if (antialiasing)
                {
                flash1->draw ();
                }
            else
                {
                flash1->drawHQ ();
                }
            }
        glEnable( GL_LIGHTING);
        glEnable( GL_LIGHT0 );
        glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        space->drawGL ();
        glDisable (GL_LIGHTING);
        glDepthMask (GL_FALSE);
        
        for (i = 0; i < space->no; i ++)
            {
            AIObj *dobj = (AIObj *) space->o [i];
            if (dobj->id > BOMB01)
                { 
                if (dobj->draw && dobj->drawlight && dobj->active)
                    { 
                    if (dobj->smoke)
                        {
                        if ((dobj->id <= MISSILE2) || (dobj->id == FIGHTER_HAWK || dobj->id == FIGHTER_ME163))
                            { 
                            if (dobj->ttl > 0)
                                {
                                dobj->smoke->draw ();
                                }
                            }
                        }
                    }
                }
            }
        glDepthMask (GL_TRUE);
       }
    glDisable (GL_TEXTURE_2D);
    glDisable (GL_LIGHTING);
    if (camera == 0)
        {
        glDisable (GL_DEPTH_TEST);
        if (antialiasing)
            {
            glEnable (GL_LINE_SMOOTH);
            }
        cockpit->drawTargeter ();
        if (antialiasing)
            {
            glDisable (GL_LINE_SMOOTH);
            }
        
        }
    glPopMatrix ();
   // SunBlinding test
    if (sunvisible)
        {
        GLfloat zbuf[1];
        glReadPixels((int)sunx,(int)suny,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,zbuf);
        SunBlinding=(zbuf[0]<1)?false:true;
        }
    else
        {
        SunBlinding=false;
        }
    // draw cockpit
    if (camera == 0)
        {
        if (antialiasing)
            {
            glEnable (GL_LINE_SMOOTH);
            }

        glPushMatrix ();
        glRotatef (view_y, 1, 0, 0);
        glRotatef (-view_x, 0, 1, 0);
        cockpit->drawCross ();
        cockpit->drawHeading ();
        glPopMatrix ();
        if (antialiasing)
            {
            glDisable (GL_LINE_SMOOTH);
            }
        cockpit->drawChassis();
        cockpit->drawRadar ();
        cockpit->drawWeapon ();
        cockpit->drawCounter ();
        cockpit->drawThrustBar ();
        }
    if (camera == 5)
        {
        cockpit->drawChassis();
        cockpit->drawRadar();
        }
    // draw blackout/redout (blending)
    if (camera!=1)   
        {
        int black = (int) blackout;
        if (black > 255)
            {
            black = 255;
            }
        int red = (int) redout;
        if (red > 255)
            {
            red = 255;
            }
        float xf = 2.0, yf = 1.5, zf = 1.0 * ScreenFOVx;
        if (black > 0)
            {
            glColor4ub (0, 0, 0, black);
            }
        else if (red > 0)
            {
            glColor4ub (255, 0, 0, red);
            }
        if (black > 0 || red > 0)
            {
            glDisable (GL_DEPTH_TEST);
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin (GL_QUADS);
            glVertex3f (-xf, -yf, -zf);
            glVertex3f (-xf, yf, -zf);
            glVertex3f (xf, yf, -zf);
            glVertex3f (xf, -yf, -zf);
            glEnd ();
            gl->disableAlphaBlending ();
            glDisable (GL_BLEND);
            glEnable (GL_DEPTH_TEST);
            }
        }
    if (brightness)
        {
        adjustBrightness ();
        glEnable (GL_DEPTH_TEST);
        }
    
    mission->draw ();
    glPushMatrix ();
    glDisable (GL_DEPTH_TEST);
    glPopMatrix ();
    char buf [150];
    if (camera !=9)
       { // For all views except map view, we want to display FPS here.
       sprintf (buf, "FPS: %d", (int) fps);
       font2->drawText (-30 / ScreenFOVx, 22 / ScreenFOVx, -3.0, buf, &colorgreen);
       }
    else
       { // For Map view, we want to display XZ position here instead.
       sprintf (buf, "POSITION= %8.2f, %8.2f", fplayer->tl->x, fplayer->tl->z);
       font2->drawText (-30 / ScreenFOVx, 21 / ScreenFOVx, -3.0, buf, &colorgreen);
       int SectorX;
       int SectorZ;
       SectorX = (int)(fplayer->tl->x/1000);
       if (fplayer->tl->x <0)
          {
          SectorX--;
          }
       SectorZ = (int)(fplayer->tl->z / 1000);
       if (fplayer->tl->z <0)
          {
          SectorZ--;
          }
       sprintf (buf, "SECTOR= %i, %i", SectorX, SectorZ);
       font2->drawText (-30 / ScreenFOVx, 22 / ScreenFOVx, -3.0, buf, &colorgreen);
       
       float CCIPoffsetX = 0.0;
       float CCIPoffsetY = 0.0;
       float DegreesToRadiansFactor = -0.01745333;
       sprintf (buf, "O"); // White "O" marks player position. Green "O" marks CCIP.
       if (MapEastWestScroll == 0  && MapNorthSouthScroll == 0 && camera == 9)
          {
          font2->drawText (CCIPoffsetX / ScreenFOVx, (CCIPoffsetY - 0.30) / ScreenFOVx, -3.2, buf, &colorwhite);
          }
       if (
              (fplayer->missiletype == 0) &&
              (fplayer->missiles [fplayer->missiletype] && MapZoomFactor == 0) &&
              (MapEastWestScroll == 0)  &&
              (MapNorthSouthScroll == 0)
          )
          { 
          
          if (view < 200)
             { 
             ConfiguredViewDistance = view;
             view = 200;
             ViewResetTimer = 1; 
             }
          
          CCIPoffsetX = sin(fplayer->phi * DegreesToRadiansFactor) * fplayer->realspeed * ScreenFOVx; 
          CCIPoffsetX *= fplayer->tl->y; 
          CCIPoffsetX /= 3.0; 
          CCIPoffsetY = cos(fplayer->phi * DegreesToRadiansFactor) * fplayer->realspeed * ScreenFOVy; 
          CCIPoffsetY *= fplayer->tl->y; 
          CCIPoffsetY /= 3.0; 
          font2->drawText (CCIPoffsetX / ScreenFOVx, (CCIPoffsetY - 0.30) / ScreenFOVx, -3.2, buf, &colorgreen); 
          if (MapEastWestScroll == 0 && MapNorthSouthScroll == 0 && camera == 9)
             {
             font2->drawText (0.01 / ScreenFOVx, -0.30 / ScreenFOVx, -3.2, buf, &colorwhite); 
             sprintf (buf, "BOMB SITE MODE ACTIVE. WHITE PIPPER = YOUR AIRCRAFT. GREEN PIPPER = PREDICTED IMPACT POINT.");
             font2->drawTextCentered (1 / ScreenFOVx, -17 / ScreenFOVx, -2.5, buf, &colorgreen);
             sprintf (buf, "FOR BEST ACCURACY FLY LEVEL & STEADY AT BETWEEN 8,000 AND 12,000 FEET, AT 240MPH.");
             font2->drawTextCentered (1 / ScreenFOVx, -18 / ScreenFOVx, -2.5, buf, &colorgreen);
             sprintf (buf, "AT 190MPH BOMBS FALL SHORT BY 1/2 RUNWAY LENGTH. AT 330MPH BOMBS GO LONG BY 1/2 RUNWAY LENGTH.");
             font2->drawTextCentered (1 / ScreenFOVx, -19 / ScreenFOVx, -2.5, buf, &colorgreen);
             sprintf (buf, "AT HIGHER ALTITUDES, BOMBS WILL FALL A LITTLE SHORT. AT LOWER ALTITUDES THEY WILL FLY A LITTLE BEYOND PREDICTED POINT.");
             font2->drawTextCentered (1 / ScreenFOVx, -20 / ScreenFOVx, -2.5, buf, &colorgreen);
             }
          }
       else if ((fplayer->missiletype == 0) && (fplayer->missiles [fplayer->missiletype]))
          { 
          sprintf (buf, "BOMB SITE MODE INACTIVE. (IT IS AVAILABLE WHEN BOMBS ARE ARMED AND MAP IS ZOOMED ALL THE WAY OUT AND CENTERED.)");
          font2->drawTextCentered (1 / ScreenFOVx, -23 / ScreenFOVx, -3.0, buf, &colorwhite);
          }
       }
    bool write = false;
    if (FirstStart)
        {
        int akttime;
        akttime = SDL_GetTicks ();

        if (akttime - starttime < 20000)
            if ((akttime - starttime) / 300 % 3)
                {
                font1->drawTextCentered (0, 0, -1.8, (char *)"PLEASE WAIT WHILE", &colorred);
                font1->drawTextCentered (0, -1, -1.8, (char *)"ADJUSTING QUALITY", &colorred);
                write = true;
                }
        }
    if (fps >= 5 && fps <= 20 && !write)
        {
        font1->drawTextCentered (0, -8, -2, (char *)"FPS TOO LOW", &colorred);
        font1->drawTextCentered (0, -9, -2, (char *)"TURN DOWN VIEW OR QUALITY", &colorred);
        }
    if (fps < 5 && !write)
        {
        font1->drawTextCentered (0, -8, -2, (char *)"FPS FAR TOO LOW", &colorred);
        font1->drawTextCentered (0, -9, -2, (char *)"SEE \"README\" OR \"FAQ\" FILES", &colorred);
        font1->drawTextCentered (0, -10, -2, (char *)"HIT \"ESC\" AND 'Y' TO EXIT THE GAME", &colorred);
        }
    if (controls == CONTROLS_MOUSE && !mouse_relative)
        {
        drawMouseCursor ();
        }
    } 

void DisplayMenu ()
    {
    void drawMouseCursor ();
    int i;
    glInitNames ();
    glPushName (0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    int MenuTimernorm = MenuTimer * 5 / timestep;
    if (MenuTimernorm != 0)
        {
        MenuTimernorm %= 360;
        }
    if (MenuTimernorm < 0)
        {
        MenuTimernorm *= -1;
        }
    CColor color2 (255, 255, (int) (255.0 * cosi [MenuTimernorm]), 255);
    if (allmenus.components [3]->isVisible ())
        {
        ((Container *) allmenus.components [3])->components [0]->setVisible (true);
        for (i = 1; i < 25; i ++)
            {
            ((Container *) allmenus.components [3])->components [i]->setVisible (false);
            }
        }
    allmenus.draw ();
    drawMouseCursor ();
    } 

void DisplayQuit ()
    {
    void drawMouseCursor ();
    allmenus.setVisible (false);
    quitmenu.setVisible (true);
    quitmenu.draw ();
    font1->drawTextCentered (0, 0, -2, (char *)"REALLY QUIT?", &menu_colwhite);
    drawMouseCursor ();
    }

void drawMouseCursor ()
    {
    glPushMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    gluOrtho2D (0, width, 0, height);
    glScalef (1, -1, 1);
    glTranslatef (0, -height, 0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gl->enableAlphaBlending ();
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GEQUAL, 0.0); 
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    gl->enableTextures (texcross2->textureID);
    int crossradius = width / 64;
    glTranslatef (mousex, mousey, 0);
    glBegin (GL_QUADS);
    glColor4ub (255, 255, 255, 255);
    glTexCoord2i (0, 0);
    glVertex2d (-crossradius, -crossradius);
    glTexCoord2i (0, 1);
    glVertex2d (-crossradius, crossradius);
    glTexCoord2i (1, 1);
    glVertex2d (crossradius, crossradius);
    glTexCoord2i (1, 0);
    glVertex2d (crossradius, -crossradius);
    glEnd ();
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_TEXTURE_2D);
    gl->disableAlphaBlending ();
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    glPopMatrix ();
    } 

void event_AutoPilot ()
   {
   display ((char*)"event_AutoPilot", LOG_MOST);
   if (!AutoPilotActive)
      {
      AutoPilotActive = true;
      AutoPilotAltitude = fplayer->tl->y;
      sprintf (SystemMessageBufferA, "AUTOPILOT ON");
      NewSystemMessageNeedsScrolling = true;
      sound->play (SOUND_BEEP2, false);
      }
   else
      {
      AutoPilotActive = false;
      sprintf (SystemMessageBufferA, "AUTOPILOT OFF");
      NewSystemMessageNeedsScrolling = true;
      sound->play (SOUND_BEEP1, false);
      TrimElevatorSetting = 0;
      }
   }

void event_CommsMacro01 ()
   {
   display ((char*)"event_CommsMacro01", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro01String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro01String1[0], "CHANNEL-M1-ALL"))
      { 
      display ((char*)"event_CommsMacro01 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 1;
      }
   else
      { 
      display ((char*)"event_CommsMacro01 NOT setting MumbleChannelPresumed because CommsMacro01 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionBlueSubChannelRequested = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionChannelRequested = false;
      MumbleTunedToCurrentMissionChannel = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionChannelRequested = true;
      MumbleTunedToCurrentMissionChannel = true;
      }
   }

void event_CommsMacro02 ()
   {
   display ((char*)"event_CommsMacro02", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro02String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro02String1[0], "CHANNEL-M1-BLUE"))
      { 
      display ((char*)"event_CommsMacro02 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 2;
      }
   else
      { 
      display ((char*)"event_CommsMacro02 NOT setting MumbleChannelPresumed because CommsMacro02 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionBlueSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionBlueSubChannelRequested = true;
      if (MyNetworkId%2 == 0)
         { 
         if (ThreeDObjects[29]->Durability >= ThreeDObjects[29]->maxDurability)
            {
            display ((char*)"event_CommsMacro02() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeRedHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro03 ()
   {
   display ((char*)"event_CommsMacro03", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro03String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro03String1[0], "CHANNEL-M1-RED"))
      { 
      display ((char*)"event_CommsMacro03 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 3;
      }
   else
      { 
      display ((char*)"event_CommsMacro03 NOT setting MumbleChannelPresumed because CommsMacro03 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionBlueSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionRedSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionRedSubChannelRequested = true;
      if (MyNetworkId%2)
         { 
         if (ThreeDObjects[28]->Durability >= ThreeDObjects[28]->maxDurability)
            {
            display ((char*)"event_CommsMacro03() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeBlueHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro04 ()
   {
   display ((char*)"event_CommsMacro04", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro04String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro04String1[0], "CHANNEL-M2-ALL"))
      { 
      display ((char*)"event_CommsMacro04 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 4;
      }
   else
      { 
      display ((char*)"event_CommsMacro04 NOT setting MumbleChannelPresumed because CommsMacro04 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionBlueSubChannelRequested = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionChannelRequested = false;
      MumbleTunedToCurrentMissionChannel = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionChannelRequested = true;
      MumbleTunedToCurrentMissionChannel = true;
      }
   }

void event_CommsMacro05 ()
   {
   display ((char*)"event_CommsMacro05", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro05String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro05String1[0], "CHANNEL-M2-BLUE"))
      { 
      display ((char*)"event_CommsMacro05 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 5;
      }
   else
      { 
      display ((char*)"event_CommsMacro05 NOT setting MumbleChannelPresumed because CommsMacro05 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionBlueSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionBlueSubChannelRequested = true;
      if (MyNetworkId%2 == 0)
         { 
         if (ThreeDObjects[29]->Durability >= ThreeDObjects[29]->maxDurability)
            {
            display ((char*)"event_CommsMacro05() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeRedHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro06 ()
   {
   display ((char*)"event_CommsMacro06", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro06String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro06String1[0], "CHANNEL-M2-RED"))
      { 
      display ((char*)"event_CommsMacro06 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 6;
      }
   else
      { 
      display ((char*)"event_CommsMacro06 NOT setting MumbleChannelPresumed because CommsMacro06 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionBlueSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionRedSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionRedSubChannelRequested = true;
      if (MyNetworkId%2)
         { 
         if (ThreeDObjects[28]->Durability >= ThreeDObjects[28]->maxDurability)
            {
            display ((char*)"event_CommsMacro06() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeBlueHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro07 ()
   {
   display ((char*)"event_CommsMacro07", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro07String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro07String1[0], "CHANNEL-M3-ALL"))
      { 
      display ((char*)"event_CommsMacro07 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 7;
      }
   else
      { 
      display ((char*)"event_CommsMacro07 NOT setting MumbleChannelPresumed because CommsMacro07 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionBlueSubChannelRequested = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   display ((char*)"event_CommsMacro07() CurrentMissionNumber = ", LOG_MOST);
   sprintf (DebugBuf, "%d", CurrentMissionNumber);
   display (DebugBuf, LOG_MOST);
   display ((char*)"event_CommsMacro07() CurrentMissionNumber = ", LOG_MOST);
   sprintf (DebugBuf, "%d", CurrentMissionNumber);
   display (DebugBuf, LOG_MOST);
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02)
      { 
      MumbleChannelExitRequested = true;
      display ((char*)"event_CommsMacro07() MumbleChannelExitRequested = true", LOG_MOST);
      MumbleCurrentMissionChannelRequested = false;
      MumbleTunedToCurrentMissionChannel = false;
      display ((char*)"event_CommsMacro07() MumbleCurrentMissionChannelRequested = false", LOG_MOST);
      }
   else
      { 
      MumbleChannelExitRequested = false;
      display ((char*)"event_CommsMacro07() MumbleChannelExitRequested = false", LOG_MOST);
      MumbleCurrentMissionChannelRequested = true;
      MumbleTunedToCurrentMissionChannel = true;
      display ((char*)"event_CommsMacro07() MumbleCurrentMissionChannelRequested = true", LOG_MOST);
      }
   }

void event_CommsMacro08 ()
   {
   display ((char*)"event_CommsMacro08", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro08String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro08String1[0], "CHANNEL-M3-BLUE"))
      { 
      display ((char*)"event_CommsMacro08 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 8;
      }
   else
      { 
      display ((char*)"event_CommsMacro08 NOT setting MumbleChannelPresumed because CommsMacro08 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionBlueSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionBlueSubChannelRequested = true;
      if (MyNetworkId%2 == 0)
         { 
         if (ThreeDObjects[29]->Durability >= ThreeDObjects[29]->maxDurability)
            {
            display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeRedHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForRedTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro09 ()
   {
   display ((char*)"event_CommsMacro09", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro09String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   if (!strcmp (&CommsMacro09String1[0], "CHANNEL-M3-RED"))
      { 
      display ((char*)"event_CommsMacro09 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 9;
      }
   else
      { 
      display ((char*)"event_CommsMacro09 NOT setting MumbleChannelPresumed because CommsMacro09 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionBlueSubChannelRequested = false;
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02)
      { 
      MumbleChannelExitRequested = true;
      MumbleCurrentMissionRedSubChannelRequested = false;
      }
   else
      { 
      MumbleChannelExitRequested = false;
      MumbleCurrentMissionRedSubChannelRequested = true;
      if (MyNetworkId%2)
         { 
         if (ThreeDObjects[28]->Durability >= ThreeDObjects[28]->maxDurability)
            {
            display ((char*)"event_CommsMacro09() Radio announcing Enemy HQ Airfield Status 100%", LOG_MOST);
            sound->setVolume (SOUND_ENEMYHQSTATUS100A, 127);
            sound->play (SOUND_ENEMYHQSTATUS100A, false);
            }
         else
            { 
            if (myrandom(100) > 20)
               { 
               VocalizeBlueHqStatus();
               }
            else
               { 
               if (AirfieldRepairsAcceleratedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs accelerated", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY, false);
                  }
               else if (AirfieldRepairsStoppedForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs stopped", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSSTOPPED, false);
                  }
               else if (AirfieldRepairRateNormalForBlueTeam)
                  { 
                  display ((char*)"event_CommsMacro08() Radio announcing Enemy HQ Airfield repairs proceeding normally.", LOG_MOST);
                  sound->play (SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY, false);
                  }
               }
            }
         }
      }
   }

void event_CommsMacro10 ()
   {
   display ((char*)"event_CommsMacro10", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro10String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   MumbleChannelMainRequested = true;
   if (!strcmp (&CommsMacro10String1[0], "CHANNEL-ROOT"))
      { 
      display ((char*)"event_CommsMacro10 setting MumbleChannelPresumed.", LOG_MOST);
      MumbleChannelPresumed = 10;
      }
   else
      { 
      display ((char*)"event_CommsMacro10 NOT setting MumbleChannelPresumed because CommsMacro10 has been customized.", LOG_MOST);
      return;
      }
   
   MumbleChannelExitRequested = false;
   MumbleCurrentMissionChannelRequested = false;
   MumbleTunedToCurrentMissionChannel = false;
   MumbleCurrentMissionBlueSubChannelRequested = false;
   MumbleCurrentMissionRedSubChannelRequested = false;
   }

void event_CommsMacro11 ()
   {
   display ((char*)"event_CommsMacro11", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro11String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_CommsMacro12 ()
   {
   display ((char*)"event_CommsMacro012", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro12String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_CommsMacro13 ()
   {
   display ((char*)"event_CommsMacro13", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro13String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_CommsMacro14 ()
   {
   display ((char*)"event_CommsMacro14", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro14String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_CommsMacro15 ()
   {
   display ((char*)"event_CommsMacro15", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro15String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_CommsMacro16 ()
   {
   display ((char*)"event_CommsMacro16", LOG_MOST);
   sprintf (SystemMessageBufferA, "%s", CommsMacro16String1);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20); 
   sound->play (SOUND_BEEP1, false);
   
   MumbleChannelExitRequested = false;
   }

void event_fireCannon ()
    {
    if (!fplayer->active)
        {
        return;
        }
    if (fplayer->firecannonttl > 0)
        {
        return;
        }
    if (fplayer->ammo == 0)
        {
        return;
        }
    fplayer->autofire = true;
    } 

void event_fireChaff ()
    {
    display ((char *)"event_fireChaff(). 150807AA", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    if (fplayer->firechaffttl > 0)
        {
        return;
        }
    if (fplayer->fireChaff (chaff, missile))
        {
        sound->play (SOUND_CHAFF1, false);
        ChaffPacketFired = true;
        }
    } 

void event_fireFlare ()
    {
    display ((char *)"event_fireFlare(). 150807AB", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    if (fplayer->fireflarettl > 0)
        {
        return;
        }
    if (fplayer->fireFlare (flare, missile))
        {
        sound->play (SOUND_CHAFF1, false);
        FlareFired = true; 
        }
    } 

void event_fireMissile ()
    {
    display ((char *)"event_fireMissile(). 150807AC", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    if (fplayer->firemissilettl > 0)
        {
        return;
        }
    if (fplayer->fireMissile (fplayer->missiletype + BOMB01, missile))
        {
        sound->play (SOUND_BOMB01, false);
        fplayer->maxgamma += MaxGammaPenaltyPerWeapon; 
        sprintf (DebugBuf, "fplayer->maxgamma now = %f", fplayer->maxgamma);
        display (DebugBuf, LOG_MOST);
        fplayer->maxthrust += 0.01; 
        if (fplayer->maxthrust > PlayersOriginalMaxThrust)
           { 
           fplayer->maxthrust = PlayersOriginalMaxThrust;
           }
        sprintf (DebugBuf, "fplayer->maxthrust now = %f", fplayer->maxthrust);
        display (DebugBuf, LOG_MOST);
        }
    } 

void event_FlapsDN()
    {
    //
    
    //
    if  (fplayer->InertiallyDampenedPlayerSpeed < fplayer->FlapSpeed)
        {
        
        sprintf (DebugBuf, "event_FlapsDN() InertiallyDampenedPlayerSpeed=%f (slow enough)", fplayer->InertiallyDampenedPlayerSpeed);
        display (DebugBuf, LOG_MOST);
        fplayer->FlapsLevel++;
        if ( fplayer->FlapsLevel > 4)
            {
            fplayer->FlapsLevel=4;
            }
        else
            {
            display ((char *)"event_FlapsDN() 150808AA", LOG_MOST);
            sound->setVolume (SOUND_FLAPSDN, 99); 
            sound->play (SOUND_FLAPSDN, false);
            switch (fplayer->FlapsLevel)
                {
                case 1:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect1;
                    break;
                    }
                case 2:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect2;
                    break;
                    }
                case 3:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect3;
                    break;
                    }
                case 4:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect4;
                    break;
                    }
                default:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
                    }
                }
            }
        }
    else
        {
        
        sound->play (SOUND_CLICK1, false);
        FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
        }
    } 

void event_FlapsUP()
    {
    if (fplayer->FlapsLevel<=0)
        {
        
        fplayer->FlapsLevel=0;
        FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
        }
    else
        {
        
        fplayer->FlapsLevel--;
        fplayer->elevatoreffect *=0.85;
        sound->setVolume (SOUND_FLAPSUP, 88); 
        sound->play (SOUND_FLAPSUP, false);
        switch (fplayer->FlapsLevel)
            {
            case 1:
                {
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect1;
                break;
                }
            case 2:
                {
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect2;
                break;
                }
            case 3:
                {
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect3;
                break;
                }
            case 4:
                {
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect4;
                break;
                }
            default:
                {
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
                }
            }
        }
    sound->play (SOUND_CLICK1, false);
    } 

void event_GunCameraToggle()
   {
   sprintf (DebugBuf, "event_GunCameraToggle().");
   display (DebugBuf, LOG_MOST);
   sound->play (SOUND_CLICK1, false);
   if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
      {
      if (GunCameraActive)
         { 
         GunCameraActive = false;
         GunCameraStopSegmentFlag = true; 
         sound->setVolume (SOUND_BEEP1, 20); 
         sound->play (SOUND_BEEP1, false);
         display ((char*)"event_GunCameraToggle() Switching Gun Camera off.", LOG_MOST);
         sprintf (SystemMessageBufferA, "GUN CAMERA SERVICE REQUEST ENDED. ");
         NewSystemMessageNeedsScrolling = true;
         fwrite (SystemMessageBufferA, strlen(SystemMessageBufferA), 1, GunCamHistoryFile);
         time_t now = time(NULL);
         struct tm *CurrentTimeStruct = localtime(&now);
         strftime(DebugBuf, sizeof(DebugBuf)-1, "%d %m %Y %H:%M", CurrentTimeStruct);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "\n");
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   Ending altitude in feet: %5.0f.\n", (50.0 * (fplayer->tl->y + SeaLevel )) - 17.0);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   VictoryCount=%d.\n", fplayer->fighterkills);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   BlueHqAirfieldStatus=%5.0f \n", (ThreeDObjects[28]->Durability / ThreeDObjects[28]->maxDurability) * 100.0 );
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   RedHqAirfieldStatus=%5.0f \n", (ThreeDObjects[29]->Durability / ThreeDObjects[29]->maxDurability) * 100.0 );
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   FUEL=%4.0f\n", fplayer->FuelLevel);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   My Aircraft Damage State= %3.0f\n", 100 * (1-(fplayer->Durability / fplayer->maxDurability)));
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "\n\n");
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         }
      else
         { 
         GunCameraActive = true;
         GunCameraStartSegmentFlag = true; 
         sound->setVolume (SOUND_BEEP2, 20); 
         sound->play (SOUND_BEEP2, false);
         display ((char*)"event_GunCameraToggle() Switching Gun Camera on.", LOG_MOST);
         sprintf (SystemMessageBufferA, "GUN CAMERA SERVICE REQUEST STARTED. ");
         NewSystemMessageNeedsScrolling = true;
         fwrite (SystemMessageBufferA, strlen(SystemMessageBufferA), 1, GunCamHistoryFile);
         time_t now = time(NULL);
         struct tm *CurrentTimeStruct = gmtime(&now);
         strftime(DebugBuf, sizeof(DebugBuf)-1, "%d %m %Y %H:%M GMT (DD MM YYYY HH:MM)", CurrentTimeStruct);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "\n");
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   Realm=%d.\n", Realm);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   Mission=%d.\n", CurrentMissionNumber);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   MyAircraft=%d.\n", PlayerAircraftType);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   StartingAltitudeMark=%f.\n", fplayer->tl->y);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   Starting altitude in feet: %5.0f.\n", (50.0 * (fplayer->tl->y + SeaLevel )) - 17.0);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   VictoryCount=%d.\n", fplayer->fighterkills);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   BlueHqAirfieldStatus=%5.0f \n", (ThreeDObjects[28]->Durability / ThreeDObjects[28]->maxDurability) * 100.0 );
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   RedHqAirfieldStatus=%5.0f \n", (ThreeDObjects[29]->Durability / ThreeDObjects[29]->maxDurability) * 100.0 );
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   FUEL=%4.0f\n", fplayer->FuelLevel);
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         sprintf (DebugBuf, "   My Aircraft Damage State= %3.0f\n", 100 * (1-(fplayer->Durability / fplayer->maxDurability)));
         fwrite (DebugBuf, strlen(DebugBuf), 1, GunCamHistoryFile);
         }
      }
   else
      { 
      sprintf (SystemMessageBufferA, "GUN CAMERA NOT AVAILABLE IN THIS MISSION.");
      NewSystemMessageNeedsScrolling = true;
      }
   }

void event_HudLadderBarsOnOff()
    {
    display ((char *)"event_HudLadderBarsOnOff().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    if (HudLadderBarsOnOff)
        { 
        HudLadderBarsOnOff = 0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    else
        { 
        if (HudOnOff)
          { 
          HudLadderBarsOnOff = 1;
          sound->setVolume (SOUND_BEEP2, 20); 
          sound->play (SOUND_BEEP2, false);
          }
         else
         { 
            HudLadderBarsOnOff = 0; 
         }
        }
    }

void event_HudOnOff ()
    {
    display ((char *)"event_HudOnOff().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    if (HudOnOff)
        {
        HudOnOff=0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    else
        {
        HudOnOff=1;
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    } 

void event_IffOnOff()
    {
    display ((char *)"event_IffOnOff().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    if (RadarZoom == 8)
        {
        sound->play (SOUND_CLICK1, false);
        return;
        }
    if (IffOnOff)
        {
        IffOnOff=0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    else
        {
        IffOnOff=1;
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_LookBK()
    {
    display ((char *)"event_LookBK().", LOG_MOST);
    camera = 2;
    }

void event_LookDN()
    {
       display ((char *)"event_LookDN().", LOG_MOST);
       if (camera == 1)
       { 
       view_y ++;
       }
    else
       {
       camera = 8;
       }
    }

void event_LookFD()
    {
    display ((char *)"event_LookFD().", LOG_MOST);
    camera = 0;
    view_x = 0;
    view_y = 0;
    }

void event_LookLT()
    {
    display ((char *)"event_LookLT().", LOG_MOST);
       if (camera == 1)
       { 
       view_x ++;
       }
    else
       {
       camera = 6;
       }
    }

void event_LookRT()
    {
    display ((char *)"event_LookRT().", LOG_MOST);
       if (camera == 1)
       { 
       view_x --;
       }
    else
       {
       camera = 7;
       }
    }

void event_LookUP()
    {
    display ((char *)"event_LookUP().", LOG_MOST);
       if (camera == 1)
       { 
       view_y --;
       }
    else
       {
       camera = 5;
       }
    }

void event_MapScrollEast()
    {
    display ((char *)"event_MapScrollEast().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapEastWestScroll-=10;
    if (MapEastWestScroll < -250)
        {
        MapEastWestScroll = -250;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    if (MapEastWestScroll == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_MapScrollNorth()
    {
    display ((char *)"event_MapScrollNorth().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapNorthSouthScroll-=10;
    if (MapNorthSouthScroll == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    if (MapNorthSouthScroll < -250)
        {
        MapNorthSouthScroll = -250;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    }

void event_MapScrollSouth()
    {
    display ((char *)"event_MapScrollSouth().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapNorthSouthScroll+=10;
    if (MapNorthSouthScroll == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    if (MapNorthSouthScroll > 250)
        {
        MapNorthSouthScroll = 250;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    }

void event_MapScrollWest()
    {
    display ((char *)"event_MapScrollWest().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapEastWestScroll+=10;
    if (MapEastWestScroll == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    if (MapEastWestScroll > 250)
        {
        MapEastWestScroll = 250;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    }

void event_MapViewOnOff()
    {
    display ((char *)"event_MapViewOnOff().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapNorthSouthScroll = 0;
    MapEastWestScroll = 0;
    if (MapViewOnOff)
        {
        MapViewOnOff = 0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        camera = 0;
        }
    else
        {
        MapViewOnOff = 1;
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        camera = 9;
        }
    }

void event_MapZoomIn()
    {
    sound->play (SOUND_CLICK1, false);
    MapZoomFactor-=10;
    if (MapZoomFactor < -130)
        {
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        MapZoomFactor = -130;
        }
    if (MapZoomFactor == -60)
       {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
       }
    sprintf (DebugBuf, "event_MapZoomIn() MapZoomFactor = %d", MapZoomFactor);
    display (DebugBuf, LOG_MOST);
    }

void event_MapZoomOut()
    {
    display ((char *)"event_MapZoomOut().", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    MapZoomFactor+=10;
    if (MapZoomFactor >= 0)
        {
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        MapZoomFactor = 0;
        }
    if (MapZoomFactor == -60)
       {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
       }
    sprintf (DebugBuf, "event_MapZoomOut() MapZoomFactor = %d", MapZoomFactor);
    display (DebugBuf, LOG_MOST);
    }

void event_MumblePtt ()
    {
    
    display ((char *)"event_MumblePtt().", LOG_MOST);
    MumbleTransmissionRequested = true;
    }

void event_MumbleWhisper ()
    {
    
    display ((char *)"event_MumbleWhisper().", LOG_MOST);
    MumbleBroadcastRequested = true;
    }

void event_RadarOnOff ()
    {
    display ((char *)"event_RadarOnOff(). 15080829AA", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    TargetSelectNotYetInitialized = true; 
    if (RadarOnOff)
        {
        RadarOnOff=0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    else
        {
        RadarOnOff=1;
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        fplayer->target = ThreeDObjects[0];
        }
    } 

void event_RadarZoomIn ()
    {
    display ((char *)"event_RadarZoomIn(). 150829AA", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    RadarZoom++;
    if (RadarZoom>7)
        {
        RadarZoom=7;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    } 

void event_RadarZoomOut ()
    {
    display ((char *)"event_RadarZoomOut(). 15080829AA", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    RadarZoom--;
    if (RadarZoom<0)
        {
        RadarZoom=0;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    
    if (MyNetworkId%2)
       { 
       if (RadarReflectorRedHasBeenDestroyedAtLeastOnce)
          { 
          if (RadarZoom <1)
             { 
             RadarZoom = 1; 
             sprintf (SystemMessageBufferA, "HQ RADAR TOO DAMAGED FOR THAT RANGE.");
             NewSystemMessageNeedsScrolling = true;
             sound->setVolume (SOUND_BEEP1, 20); 
             sound->play (SOUND_BEEP1, false);
             }
          }
       }
    else
       { 
       if (RadarReflectorBlueHasBeenDestroyedAtLeastOnce)
          { 
          if (RadarZoom <1)
             { 
             RadarZoom = 1; 
             sprintf (SystemMessageBufferA, "HQ RADAR TOO DAMAGED FOR THAT RANGE.");
             NewSystemMessageNeedsScrolling = true;
             sound->setVolume (SOUND_BEEP1, 20); 
             sound->play (SOUND_BEEP1, false);
             }

          }
       }
    } 

void event_selectMissile ()
    {
    int OldMissileType = 0;
    display ((char *)"event_selectMissile(). 150807AD", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    sound->play (SOUND_CLICK1, false);  
    OldMissileType = fplayer->missiletype;
    fplayer->missiletype = fplayer->nextMissile (fplayer->missiletype);
    if (fplayer->missiles [fplayer->missiletype] <= 0)
        {
        
        return;
        }
    
    if(OldMissileType == fplayer->missiletype)
        {
        
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    else
        {
        
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    sound->play (SOUND_CLICK1, false); 
    } 

void event_setAntialiasing ()
    {
    if (antialiasing)
        {
        gl->enableLinearTexture (font1->texture->textureID);
        gl->enableLinearTexture (font2->texture->textureID);
        if (texmoon != NULL)
            {
            gl->enableLinearTexture (texmoon->textureID);
            gl->enableLinearTexture (texsun->textureID);
            gl->enableLinearTexture (texearth->textureID);
            }
        }
    else
        {
        gl->disableLinearTexture (font1->texture->textureID);
        gl->disableLinearTexture (font2->texture->textureID);

        if (texmoon != NULL)
            {
            gl->disableLinearTexture (texmoon->textureID);
            gl->disableLinearTexture (texsun->textureID);
            gl->disableLinearTexture (texearth->textureID);
            }
        }
    } 

void event_stopCannon ()
    {
    fplayer->autofire = false;
    sound->stop (SOUND_CANNON1);
    } 

void event_targetNext ()
    {
    display ((char *)"event_targetNext(). 150807AF", LOG_MOST);
    DynamicObj* InitialTarget = fplayer->target; 
    DynamicObj* FinalTarget   = 0;
    
    if (!fplayer->active)
        {
        return;
        }
    fplayer->targetNext ((AIObj **) ThreeDObjects);
    FinalTarget = fplayer->target;
    if (InitialTarget == FinalTarget)
       { 
       sound->play (SOUND_BEEP1, false);
       }
    else
       { 
       sound->play (SOUND_BEEP2, false);
       
       }
       if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
          { 
          if (ThreeDObjects[MyNetworkId] == fplayer->target)
             { 
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             FinalTarget = fplayer->target;
             }
          
          if (fplayer->target == ThreeDObjects[24])
             { 
             display ((char*)"event_targetNext() attempted to target object 24. Skipping ahead.", LOG_MOST);
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             }
          if (fplayer->target == ThreeDObjects[25])
             { 
             display ((char*)"event_targetNext() attempted to target object 25. Skipping ahead.", LOG_MOST);
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             }
          if (fplayer->target == ThreeDObjects[26])
             { 
             display ((char*)"event_targetNext() attempted to target object 26. Skipping ahead.", LOG_MOST);
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             }
          else if (fplayer->target == ThreeDObjects[27])
             { 
             display ((char*)"event_targetNext() attempted to target object 27. Skipping ahead.", LOG_MOST);
             fplayer->targetNext ((AIObj **) ThreeDObjects); 
             }
          
          if (fplayer->target == ThreeDObjects[1])
             {
             SelectedMissionTarget = 1;
             }
          else if (fplayer->target == ThreeDObjects[2])
             {
             SelectedMissionTarget = 2;
             }
          else if (fplayer->target == ThreeDObjects[3])
             {
             SelectedMissionTarget = 3;
             }
          else if (fplayer->target == ThreeDObjects[4])
             {
             SelectedMissionTarget = 4;
             }
          else if (fplayer->target == ThreeDObjects[5])
             {
             SelectedMissionTarget = 5;
             }
          else if (fplayer->target == ThreeDObjects[6])
             {
             SelectedMissionTarget = 6;
             }
          else if (fplayer->target == ThreeDObjects[7])
             {
             SelectedMissionTarget = 7;
             }
          else if (fplayer->target == ThreeDObjects[8])
             {
             SelectedMissionTarget = 8;
             }
          else if (fplayer->target == ThreeDObjects[9])
             {
             SelectedMissionTarget = 9;
             }
          else if (fplayer->target == ThreeDObjects[10])
             {
             SelectedMissionTarget = 10;
             }
          else if (fplayer->target == ThreeDObjects[22])
             {
             SelectedMissionTarget = 22;
             }
          else if (fplayer->target == ThreeDObjects[23])
             {
             SelectedMissionTarget = 23;
             }
          else if (fplayer->target == ThreeDObjects[28])
             {
             SelectedMissionTarget = 28;
             }
          else if (fplayer->target == ThreeDObjects[29])
             {
             SelectedMissionTarget = 29;
             }
          
          Admin013OutPacket.UdpObjCommand = (char)13;
          Admin013OutPacket.UdpObjRealm = Realm;
          Admin013OutPacket.UdpObjMissionId = CurrentMissionNumber;
          
          SendAdmin013Packet();
          }
       int MissionAircraft;
       for (MissionAircraft = 0; MissionAircraft < maxfighter; MissionAircraft++)
          { 
          if (ThreeDObjects[MissionAircraft] == fplayer->target)
             { 
             switch (MissionAircraft %2)
                {
                case 0:
                    { 
                    sound->setVolume (SOUND_BLUE, 150);
                    sound->play (SOUND_BLUE, false);
                    break;
                    }
                case 1:
                    { 
                    sound->setVolume (SOUND_RED, 150);
                    sound->play (SOUND_RED, false);
                    break;
                    }
                default:
                   {
                    break;
                   }
                }
             TargetVocalizeTimer2 = 25; 
             TargetVocalize2State = true;
             }
          }
    
    RadarTargetColorLogged = false;
    } 

void event_targetPrevious ()
    {
    display ((char *)"event_targetPrevious(). 150807AE", LOG_MOST);
    DynamicObj* InitialTarget = fplayer->target; 
    DynamicObj* FinalTarget   = 0;
    
    if (!fplayer->active)
         {
         return;
         }
    fplayer->targetPrevious ((AIObj **) ThreeDObjects);
    FinalTarget = fplayer->target;
    if (InitialTarget == FinalTarget)
       { 
       sound->play (SOUND_BEEP1, false);
       }
    else
       { 
       sound->play (SOUND_BEEP2, false);
       
       int MissionAircraft;
       if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
          { 
          if (ThreeDObjects[MyNetworkId] == fplayer->target)
             { 
             fplayer->targetPrevious ((AIObj **) ThreeDObjects); 
             FinalTarget = fplayer->target;
             }
          
          if (fplayer->target == ThreeDObjects[27])
             { 
             fplayer->targetPrevious ((AIObj **) ThreeDObjects); 
             }
          if (fplayer->target == ThreeDObjects[26])
             { 
             fplayer->targetPrevious ((AIObj **) ThreeDObjects); 
             }
          if (fplayer->target == ThreeDObjects[25])
             { 
             fplayer->targetPrevious ((AIObj **) ThreeDObjects); 
             }
          if (fplayer->target == ThreeDObjects[24])
             { 
             fplayer->targetPrevious ((AIObj **) ThreeDObjects); 
             }
          
          if (fplayer->target == ThreeDObjects[1])
             {
             SelectedMissionTarget = 1;
             }
          else if (fplayer->target == ThreeDObjects[2])
             {
             SelectedMissionTarget = 2;
             }
          else if (fplayer->target == ThreeDObjects[3])
             {
             SelectedMissionTarget = 3;
             }
          else if (fplayer->target == ThreeDObjects[4])
             {
             SelectedMissionTarget = 4;
             }
          else if (fplayer->target == ThreeDObjects[5])
             {
             SelectedMissionTarget = 5;
             }
          else if (fplayer->target == ThreeDObjects[6])
             {
             SelectedMissionTarget = 6;
             }
          else if (fplayer->target == ThreeDObjects[7])
             {
             SelectedMissionTarget = 7;
             }
          else if (fplayer->target == ThreeDObjects[8])
             {
             SelectedMissionTarget = 8;
             }
          else if (fplayer->target == ThreeDObjects[9])
             {
             SelectedMissionTarget = 9;
             }
          else if (fplayer->target == ThreeDObjects[10])
             {
             SelectedMissionTarget = 10;
             }
          else if (fplayer->target == ThreeDObjects[22])
             {
             SelectedMissionTarget = 22;
             }
          else if (fplayer->target == ThreeDObjects[23])
             {
             SelectedMissionTarget = 23;
             }
          else if (fplayer->target == ThreeDObjects[28])
             {
             SelectedMissionTarget = 28;
             }
          else if (fplayer->target == ThreeDObjects[29])
             {
             SelectedMissionTarget = 29;
             }
          
          Admin013OutPacket.UdpObjCommand = (char)13;
          Admin013OutPacket.UdpObjRealm = Realm;
          Admin013OutPacket.UdpObjMissionId = CurrentMissionNumber;
          
          SendAdmin013Packet();
          }
       for (MissionAircraft = 0; MissionAircraft < maxfighter; MissionAircraft++)
          { 
          if (ThreeDObjects[MissionAircraft] == fplayer->target)
             { 
             switch (MissionAircraft %2)
                {
                case 0:
                    { 
                    sound->setVolume (SOUND_BLUE, 150);
                    sound->play (SOUND_BLUE, false);
                    break;
                    }
                case 1:
                    { 
                    sound->setVolume (SOUND_RED, 150);
                    sound->play (SOUND_RED, false);
                    break;
                    }
                default:
                   {
                    break;
                   }
                }
             TargetVocalizeTimer2 = 25; 
             TargetVocalize2State = true;
             }
          }
       }
    
    RadarTargetColorLogged = false;
    } 

void event_targetVocalize ()
   {
   extern char TargetVocalize1State;
   display ((char *)"event_targetVocalize() Function Entry.", LOG_MOST);
   if (TargetVocalize1State < 7)
      {
      TargetVocalize1State++; 
      }
   else
      {
      TargetVocalize1State = 0;
      }
   }

void event_thrustDown ()
    {
    display ((char *)"event_thrustDown(). 150807AG", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    fplayer->thrustDown ();
    fplayer->thrustDown ();
    if (OnTheGround)
       {
       fplayer->thrust = 0.0;
       fplayer->recthrust = 0.0;
       }
    sound->play (SOUND_CLICK1, false);
    } 

void event_thrustUp ()
    {
    display ((char *)"event_thrustUp(). 150807AH", LOG_MOST);
    if (!fplayer->active)
        {
        return;
        }
    fplayer->thrustUp ();
    fplayer->thrustUp ();
    sound->play (SOUND_CLICK1, false);
    } 

void event_ToggleSpeedBrake()
    {
    display ((char *)"event_ToggleSpeedBrake(). 150806EA", LOG_MOST);
    if (fplayer->SpeedBrakePower == 1.00)
       { 
       return; 
       }
    if (fplayer->SpeedBrake)
        { 
        SpeedBrakeRequested = 0;
        fplayer->SpeedBrake=0;
        SpeedBrakeTimer = 0;
        sound->setVolume (SOUND_SPEEDBRAKE1, 50);
        sound->play (SOUND_SPEEDBRAKE1, false);
        }
    else if (fplayer->SpeedBrake == 0 )
        { 
        SpeedBrakeRequested = 1;
        if (SpeedBrakeTimer > 50 && SpeedBrakeTimer < 2000)
           { 
           return;
           }
        sound->setVolume (SOUND_SPEEDBRAKE, 50);
        sound->play (SOUND_SPEEDBRAKE, false);
        }
    sound->play (SOUND_CLICK1, false);
    } 

void event_ToggleUndercarriage()
    {
    display ((char *)"event_ToggleUndercarriage.", LOG_MOST);
    
    if (fplayer->InertiallyDampenedPlayerSpeed < (fplayer->FlapSpeed * 0.75))
        {
        
        if (fplayer->UndercarriageLevel)
            {
            
            fplayer->UndercarriageLevel--;
            sound->setVolume (SOUND_UNDERCARRIAGE, 88); 
            sound->play (SOUND_UNDERCARRIAGE, false);
            }
        else
            {
            
            fplayer->UndercarriageLevel++;
            sound->setVolume (SOUND_UNDERCARRIAGE, 88); 
            sound->play (SOUND_UNDERCARRIAGE, false);
            }
        }
    sound->play (SOUND_CLICK1, false);
    }

void event_ToggleViewIntExt()
    {
    display ((char *)"event_ToggleViewIntExt(). 150807KA", LOG_MOST);
    if (camera==0)
        {
        camera = 1;
        }
    else
        {
        camera = 0;
        }
    }

void event_ToggleWEP()
    {
    void setPlaneVolume();
    display ((char *)"event_ToggleWEP(). 150806EC", LOG_MOST);
    if (!fplayer->WepCapable)
       { 
       sound->play (SOUND_CLICK1, false);
       fplayer->WEPLevel = 0;
       return;
       }
    if ( fplayer->WEPLevel)
        { 
        fplayer->WEPLevel--;
        fplayer->maxthrust -= 0.1;
        }
    else if ((fplayer->thrust / fplayer->maxthrust)>0.9)
        { 
        fplayer->WEPLevel++;
        fplayer->maxthrust += 0.1;
        }
    sound->play (SOUND_CLICK1, false);
    setPlaneVolume (); 
    }

void event_TrimElevatorDn()
    {
    display ((char *)"event_TrimElevatorDn(). 150806EC", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    TrimElevatorSetting -=200;
    if (TrimElevatorSetting < -6000)
        {
        TrimElevatorSetting = -6000;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    if (TrimElevatorSetting == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_TrimElevatorUp()
    {
    display ((char *)"event_TrimElevatorUp(). 150806EC", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    TrimElevatorSetting +=200;
    if (TrimElevatorSetting > 6000)
        {
        TrimElevatorSetting = 6000;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    if (TrimElevatorSetting == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_TrimRudderLeft()
    {
    display ((char *)"event_TrimRudderLeft(). 150806EC", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    TrimRudderSetting -= 4000;
    if (TrimRudderSetting < -32000)
        {
        TrimRudderSetting = -32000;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    if (TrimRudderSetting == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_TrimRudderRight()
    {
    display ((char *)"event_TrimRudderRight(). 150806EC", LOG_MOST);
    sound->play (SOUND_CLICK1, false);
    TrimRudderSetting += 4000;
    if (TrimRudderSetting > 32000)
        {
        TrimRudderSetting = 32000;
        sound->setVolume (SOUND_BEEP1, 20); 
        sound->play (SOUND_BEEP1, false);
        }
    if (TrimRudderSetting == 0)
        {
        sound->setVolume (SOUND_BEEP2, 20); 
        sound->play (SOUND_BEEP2, false);
        }
    }

void event_ZoomFovIn()
   {
   
   ScreenFOVx += 0.1; 
   ScreenFOVy+=0.1; 
   if(ScreenFOVy> 1.6)
       {
       ScreenFOVy = 1.6;
       sound->setVolume (SOUND_BEEP1, 20); 
       sound->play (SOUND_BEEP1, false);
       }
   if(ScreenFOVx > 1.6)
       {
       ScreenFOVx = 1.6;
       }
   if (ScreenFOVx > 0.91 && ScreenFOVx < 1.09)
       {
       ScreenFOVx = 1.0;
       ScreenFOVy = 1.0;
       sound->setVolume (SOUND_BEEP2, 20); 
       sound->play (SOUND_BEEP2, false);
       }
   joybutton=0;
   DoneFiringCannon++;
}

void event_ZoomFovOut()
{
   
   ScreenFOVx -= 0.1; 
   ScreenFOVy-=0.1; 
   if(ScreenFOVy< 0.5)
       {
       ScreenFOVy = 0.5;
       sound->setVolume (SOUND_BEEP1, 20); 
       sound->play (SOUND_BEEP1, false);
       }
   if(ScreenFOVx < 0.5)
       {
       ScreenFOVx = 0.5;
       }
   if (ScreenFOVx > 0.91 && ScreenFOVx < 1.09)
       {
       ScreenFOVx = 1.0;
       ScreenFOVy = 1.0;
       sound->setVolume (SOUND_BEEP2, 20); 
       sound->play (SOUND_BEEP2, false);
       }
   joybutton=0;
   DoneFiringCannon++;
}

void frame ()
    {
    frames ++;
    time2 = SDL_GetTicks ();

    if (time2 - time1 > 1000)
        {
        if (time2 - time1 < 1500)
            {
            fps = (float) frames * 1000.0 / (time2 - time1);
            }

        time1 = time2;
        frames = 0;
        }
    } 

void game_joystickaxis (int x, int y, int rudder, int throttle,  int JoyHatX, int JoyHatY)
    {
    static int PriorJoyHatX = 0;
    static int PriorJoyHatY = 0;
    if (fplayer->ai)
        {
        return;    
        }
    AileronSettings(x, y);
    ElevatorSettings(x, y);
    RudderSettings(rudder);
    ThrottleSettings(throttle);
    
    if (JoyHatX != PriorJoyHatX)
       { 
       if (JoyHatX > 8000)
          { 
          display ((char*)"game_joystickaxis() Hat Right", LOG_MOST);
          LacJoystickHatFunc (2);
          }
       else if (JoyHatX < -8000)
          {
          display ((char*)"game_joystickaxis() Hat Left", LOG_MOST);
          LacJoystickHatFunc (8);
          }
       }
    if (JoyHatY != PriorJoyHatY)
       { 
       if (JoyHatY > 8000)
          { 
          display ((char*)"game_joystickaxis() Hat Down", LOG_MOST);
          LacJoystickHatFunc (4);
          }
       else if (JoyHatY < -8000)
          {
          display ((char*)"game_joystickaxis() Hat Up", LOG_MOST);
          LacJoystickHatFunc (1);
          }
       }
    PriorJoyHatX = JoyHatX;
    PriorJoyHatY = JoyHatY;
    } 

void game_JoystickButton (int button)
    {
    extern int joystick_ZoomFOVin;
    extern int joystick_ZoomFOVout;
    extern int joystick_TargetNext;
    extern int joystick_TargetPrev;
    sprintf (DebugBuf, (char *)"game_JoystickButton() %d", button);
    display (DebugBuf, LOG_MOST);  
    joybutton=button;
    if (!fplayer->active)
        {
        return;
        }
    if (button == joystick_LookBK)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookBK();
        }
    if (button == joystick_LookDN)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookDN();
        }
    if (button == joystick_LookFD)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookFD();
        }
    if (button == joystick_LookLT)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookLT();
        }
    if (button == joystick_LookRT)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookRT();
        }
    if (button == joystick_LookUP)
        {
        sound->play (SOUND_CLICK1, false);
        event_LookUP();
        }
    if (button == joystick_FIRESECONDARY)
        {
        sound->play (SOUND_CLICK1, false);
        event_fireMissile ();
        }
    if (button == joystick_DROPFLARE)
        {
        sound->play (SOUND_CLICK1, false);
        event_fireFlare ();
        }
    if (button == joystick_DROPCHAFF)
        {
        sound->play (SOUND_CLICK1, false);
        event_fireChaff ();
        }
    if (button == joystick_WEAPONSELECT)
        {
        event_selectMissile ();
        }
    if (button == joystick_ZoomFOVin)
        {
        event_ZoomFovIn();
        }
    if (button == joystick_ZoomFOVout)
        {
        event_ZoomFovOut();
        }
    if (button == joystick_TargetNext)
        {
        event_targetNext ();
        joybutton=0;
        DoneFiringCannon++;
        }
    if (button == joystick_TargetVocalize)
        {
        event_targetVocalize();
        joybutton=0;
        DoneFiringCannon++;
        }
    if (button == joystick_TargetPrev)
        {
        event_targetPrevious ();
        joybutton=0;
        DoneFiringCannon++;
        }
    if (button == joystick_ToggleExtIntView)
        {
        sound->play (SOUND_CLICK1, false);
        event_ToggleViewIntExt();
        }
    if (button == joystick_FlapsDN)
        {
        event_FlapsDN();
        }
    if (button == joystick_FlapsUP)
        {
        event_FlapsUP();
        }
    if (button == joystick_SpeedBrake)
        {
        event_ToggleSpeedBrake();
        }
    if (button == joystick_Undercarriage)
        {
        event_ToggleUndercarriage();
        }
    if (button == joystick_WEPorAfterburner)
        {
        event_ToggleWEP();
        }
    if (button == joystick_TrimElevatorDn)
        {
        event_TrimElevatorDn();
        }
    if (button == joystick_TrimElevatorUp)
        {
        event_TrimElevatorUp();
        }
    if (button == joystick_TrimRudderLeft)
        {
        event_TrimRudderLeft();
        }
    if (button == joystick_TrimRudderRight)
        {
        event_TrimRudderRight();
        }
    if (!DoneFiringCannon && joybutton == joystick_FIREPRIMARY && fplayer->ammo)
        {
        fplayer->fireCannon (MachineGunBullet);
        if (!StartCannon)
            {
            StartCannon = true;
            }
        sound->setVolume (SOUND_CANNON1, 120);
        sound->play (SOUND_CANNON1, 1);
        }
    } 

void game_joystickhat (int normhat)
    {
    game_JoystickButton (normhat);
    } 

void game_mouserelmotion (int xrel, int yrel)
    {
    if (controls != CONTROLS_MOUSE || !mouse_relative)
        {
        return;
        }
    if(xrel > 70)
       {
       xrel =  70;
       }
    if(xrel < -70)
       {
       xrel = -70;
       }
    if(yrel > 100)
       {
       yrel = 100;
       }
    if(yrel < -100)
       {
       xrel = -100;
       }
    float xr = (float) xrel / width, yr = (float) yrel / height;
    float roll = (float) -xr * mouse_sensitivity/4;
    if (roll > 50.0F)
        {
        roll = 50.0F;
        }
    else if (roll < -50.0F)
        {
        roll = -50.0F;
        }
    fplayer->rolleffect = roll;
    fplayer->elevatoreffect = (float) yr * fabs (yr) * 30 * mouse_sensitivity;
    if (mouse_sensitivity < 100)
        {
        
        fplayer->elevatoreffect *=0.2F;
        }
    else if (mouse_sensitivity >200)
        {
        
        fplayer->elevatoreffect *=2.0F;
        }
    if (!mouse_reverse)
        {
        fplayer->elevatoreffect *= -1;
        }
    if (fplayer->elevatoreffect > 200.0f)
        {
        fplayer->elevatoreffect = 200.0f;
        }
    else if (fplayer->elevatoreffect < -200.0f)
        {
        fplayer->elevatoreffect = -200.0f;
        }
    } 

void game_mousemotion (int x, int y)
    {
    if (controls != CONTROLS_MOUSE || mouse_relative)
        {
        return;
        }
    float mx = width / 2, my = height / 2;
    float dx = x - mx, dy = my - y;
    dx *= mouse_sensitivity / 70.0F;
    dy *= mouse_sensitivity / 70.0F;
    if (mouse_reverse)
        {
        dy *= -1;
        }
    // mouse interface code added by Lourens Veen
    float nx = dx / width; // normalised x-coordinate, -1 at lefthand
    // side of the screen, 1 at righthand side
    if (mouse_autorudder)
        {
        if (fabs(nx) < (1.0f/3.0f))
            if (nx < 0.0f)
                {
                fplayer->ruddereffect = 729.0f*nx*(nx+1.0f/3.0f)*(nx+1.0f/3.0f)/4.0f;
                }
            else
                {
                fplayer->ruddereffect = 729.0f*nx*(nx-1.0f/3.0f)*(nx-1.0f/3.0f)/4.0f;
                }
            else
                {
                fplayer->ruddereffect = 0.0f;
                }
        }
    else
        {
        fplayer->ruddereffect = 0;
        }
    const float roll_deadarea = (float) mouse_autorudder / 1000.0F; 
    if (fabs(nx) > roll_deadarea)
        {
        if (nx > 0.0f)
            {
            nx -= roll_deadarea;
            }
        else
            {
            nx += roll_deadarea;
            }
        if (nx > 0)
            {
            fplayer->rolleffect = -(exp(log(nx) * 1.3f)) * 3.0f;
            }
        else
            {
            fplayer->rolleffect = (exp(log(-nx) * 1.3f)) * 3.0f;
            }
        if (fplayer->rolleffect < -1.0F)
            {
            fplayer->rolleffect = -1.0F;
            }
        if (fplayer->rolleffect > 1.0F)
            {
            fplayer->rolleffect = 1.0F;
            }
        }
    else
        {
        fplayer->rolleffect = 0.0f;
        }
    fplayer->elevatoreffect = dy / height * 2.5;
    if (fplayer->elevatoreffect > 1.0f)
        {
        fplayer->elevatoreffect = 1.0f;
        }
    else if (fplayer->elevatoreffect < -0.5f)
        {
        fplayer->elevatoreffect = -0.5f;
        }
    sdldisplay = true;
    } 

int game_levelInit (void)
    {
    void setLightSource (int gamma);
    int i;
    display ((char *)"game_levelInit() beginning.", LOG_MOST);
    
    for (i = 0; i < maxfighter; i ++)
        {
        ThreeDObjects [i]->dinit ();
        ThreeDObjects [i]->aiinit ();
        ThreeDObjects [i]->explode = 0;
        ThreeDObjects [i]->tl->x = 0;
        ThreeDObjects [i]->tl->z = -i * 10;
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->zoom = 0.35;
        ThreeDObjects [i]->deactivate ();
        ThreeDObjects [i]->recheight = 15;
        ThreeDObjects [i]->recthrust = 0.2;
        ThreeDObjects [i]->rectheta = 0;
        ThreeDObjects [i]->o = &model_fig;
        }
    for (i = 0; i < maxgroundobj; i ++)
        {
        groundobj [i]->dinit ();
        groundobj [i]->zoom = 1;
        groundobj [i]->id = STATIC_PASSIVE;
        groundobj [i]->deactivate ();
        }
    Mission *missionold = mission;
    mission = missionnew;
    missionnew = NULL;
    mission->difficulty = difficulty;
    mission->start ();
    if (missionold != NULL)
        {
        delete missionold;
        missionold = NULL;
        }
    initing = true;
    flash = 0;
    if (clouds == 0)
        {
        highclouds->setTexture (NULL);
        }
    else if (clouds == 1)
        {
        highclouds->setTexture (texclouds1);
        }
    else if (clouds == 2)
        {
        highclouds->setTexture (texclouds2);
        }
    else if (clouds == 3)
        {
        highclouds->setTexture (texclouds3);
        }
    if (clouds == 0)
        {
        highclouds2->setTexture (NULL);
        }
    else if (clouds == 1)
        {
        highclouds2->setTexture (texclouds2);
        }
    else if (clouds == 2)
        {
        highclouds2->setTexture (NULL);
        }
    else if (clouds == 3)
        {
        highclouds2->setTexture (NULL);
        }
    
    for (i = 0; i < maxfighter; i ++)
        {
        if (ThreeDObjects [i]->id >= FLAK1 && ThreeDObjects [i]->id <= FLAK2)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom;
            }
        else if (ThreeDObjects [i]->id == SHIP_DESTROYER1)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom / 4;
            }
        else if (ThreeDObjects [i]->id == SHIP_CRUISER)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom / 4;
            }
        else if (ThreeDObjects [i]->id >= TANK1 && ThreeDObjects [i]->id <= TANK2)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom * 0.55;
            }
        else if (ThreeDObjects [i]->id == STATIC_BATTLESHIP)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom / 3;
            }
        else if (ThreeDObjects [i]->id == STATIC_HALL2)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom / 3;
            }
        else if (ThreeDObjects [i]->id == STATIC_AIRFIELD00)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + ThreeDObjects [i]->zoom / 3;
            }
        else if (ThreeDObjects [i]->id == STATIC_RADAR1)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 0.7 * ThreeDObjects [i]->zoom;
            }
        else if (ThreeDObjects [i]->id == STATIC_BASE1)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 0.5 * ThreeDObjects [i]->zoom;
            }
        else if (ThreeDObjects [i]->id == STATIC_DEPOT1)
            {
            ThreeDObjects [i]->tl->y = l->getExactHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 0.5 * ThreeDObjects [i]->zoom;
            }
        else if (ThreeDObjects [i]->id == MISSILE_MINE1)
            {
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 5 + myrandom (20);
            }
        else
            {
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 20;
            }
        }
    for (i = 0; i < maxMachineGunBullet; i ++)
        {
        MachineGunBullet [i]->deactivate ();

        if (day)
            {
            MachineGunBullet [i]->o = &model_cannon1;
            MachineGunBullet [i]->zoom = 0.08;
            MachineGunBullet [i]->drawlight = false;
            }
        else
            {
            MachineGunBullet [i]->o = &model_cannon2;
            MachineGunBullet [i]->zoom = 0.1;
            }
        }
    for (i = 0; i < maxmissile; i ++)
        {
        missile [i]->aiinit ();
        missile [i]->deactivate ();
        }
    for (i = 0; i < maxflare; i ++)
        {
        flare [i]->dinit ();
        flare [i]->deactivate ();
        }
    for (i = 0; i < maxchaff; i ++)
        {
        chaff [i]->dinit ();
        chaff [i]->deactivate ();
        }
    for (i = 0; i < maxstar; i ++)
        {
        star [i]->phi = myrandom (360);
        star [i]->gamma = myrandom (85);
        star [i]->size = 0.6 + 0.15 * myrandom (8);
        }
    CColor skycolor;
    if (day && weather == WEATHER_SUNNY)
        {
        if (sungamma < 35)
            {
            skycolor.setColor ((unsigned short) (127 + 70 - 2 * sungamma), 127, 127);
            objsphere->setColor (&skycolor);
            }
        else
            {
            skycolor.setColor (127, 127, 127 + 5);
            objsphere->setColor (&skycolor);
            }
        if (sungamma < 35)
            {
            gl->fogcolor [0] = (float) (127 + 70 - 2 * sungamma) / 256.0;
            gl->fogcolor [1] = 0.5;
            gl->fogcolor [2] = 0.5;
            }
        else
            {
            gl->fogcolor [0] = 0.5;
            gl->fogcolor [1] = 0.5;
            gl->fogcolor [2] = 0.52;
            }
        skycolor.setColor (50, 200, 255);
        objsphere->setNorthPoleColor (&skycolor, 1.8);
        if (sungamma < 35)
            {
            skycolor.setColor ((unsigned short) (180 + 70 - 2 * sungamma), 180, 180);
            objsphere->setPoleColor (270, (int) (90 - sungamma), &skycolor, 0.3);
            }
        else
            {
            skycolor.setColor (200, 200, 200);
            objsphere->setPoleColor (270, (int) (90 - sungamma), &skycolor, 0.3);
            }
        }
    else if (!day && weather == WEATHER_SUNNY)
        {
        skycolor.setColor (64, 64, 64);
        objsphere->setColor (&skycolor);
        gl->fogcolor [0] = 0.25;
        gl->fogcolor [1] = 0.25;
        gl->fogcolor [2] = 0.25;
        if (l->type)
            {
            skycolor.setColor (0, 0, 170);
            objsphere->setNorthPoleColor (&skycolor, 1.8);
            skycolor.setColor (64, 64, 64);
            objsphere->setPoleColor (270, (int) (90 - sungamma), &skycolor, 0.3);
            }
        else
            {
            skycolor.setColor (0, 0, 0);
            objsphere->setNorthPoleColor (&skycolor, 1.8);
            }
        }
    else if (day && weather == WEATHER_THUNDERSTORM)
        {
        skycolor.setColor (102, 102, 102);
        objsphere->setColor (&skycolor);
        gl->fogcolor [0] = 0.4;
        gl->fogcolor [1] = 0.4;
        gl->fogcolor [2] = 0.4;
        skycolor.setColor (102, 102, 102);
        objsphere->setNorthPoleColor (&skycolor, 1.8);
        }
    else if (!day && weather == WEATHER_THUNDERSTORM)
        {
        skycolor.setColor (40, 40, 40);
        objsphere->setColor (&skycolor);
        gl->fogcolor [0] = 0.16;
        gl->fogcolor [1] = 0.16;
        gl->fogcolor [2] = 0.16;
        skycolor.setColor (40, 40, 40);
        objsphere->setNorthPoleColor (&skycolor, 1.8);
        }
    else if (weather == WEATHER_CLOUDY)
        {
        skycolor.setColor (20, 20, 20);
        objsphere->setColor (&skycolor);
        gl->fogcolor [0] = 0.08;
        gl->fogcolor [1] = 0.08;
        gl->fogcolor [2] = 0.08;
        }
    glDeleteLists (objsphere->list1, 1);
    glDeleteLists (objsphere->list2, 1);
    glDeleteLists (objsphere->list3, 1);
    objsphere->list1 = -1;
    objsphere->list2 = -1;
    objsphere->list3 = -1;
    setLightSource ((int) sungamma);
    glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable (GL_LIGHT0);                
    float light_ambient [4] = {0.2, 0.2, 0.2, 1.0};
    float light_diffuse [4] = {1.0, 1.0, 1.0, 1.0};
    float light_specular [4] = {1.0, 1.0, 1.0, 1.0};
    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable (GL_LIGHTING);                // Turn on lighting
    glEnable (GL_COLOR_MATERIAL);            // Allow color
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    fplayer->missiletype = fplayer->firstMissile ();
    initing = false;
    lastDurability = (int) fplayer->Durability;
    fps = 30;
    redout = 0;
    blackout = 0;
    fplayer->realism = physics;
    return 1;
    } 

void game_mouse (int button, int state, int x, int y)
    {
    if (state == MOUSE_DOWN && fplayer->active)
        {
        
        if (button == mouse_firecannon)
            {
            
            fplayer->autofire = !fplayer->autofire;

            if (!fplayer->autofire)
                {
                
                sound->stop (SOUND_CANNON1);
                }
            }
        else if (button == mouse_firemissile)
            {
            event_fireMissile ();
            }
        else if (button == mouse_selectmissile)
            {
            event_selectMissile ();
            }
        }
    } 

void game_quit ()
    {
    int i;
    volumesound = sound->volumesound;
    volumemusic = sound->volumemusic;
    save_config ();
    save_configInterface ();
    for (i = 0; i < maxMachineGunBullet; i ++)
        {
        delete (MachineGunBullet [i]);
        }
    for (i = 0; i < maxmissile; i ++)
        {
        delete (missile [i]);
        }
    for (i = 0; i < maxflare; i ++)
        {
        delete (flare [i]);
        }
    for (i = 0; i < maxchaff; i ++)
        {
        delete (chaff [i]);
        }
    for (i = 0; i < maxexplosion; i ++)
        {
        delete (explosion [i]);
        }
    for (i = 0; i < maxstar; i ++)
        {
        delete (star [i]);
        }
    for (i = 0; i < maxgroundobj; i ++)
        {
        delete (groundobj [i]);
        }
    for (i = 0; i < maxblacksmoke; i ++)
        {
        delete (blacksmoke [i]);
        }
    delete tlinf;
    delete tlminf;
    delete tlnull;
    delete explsphere;
    delete objsphere;
    delete sphere;
    delete flash1;
    delete cockpit;
    delete font1;
    delete font2;
    delete space;
    delete dirs;
    delete gl;
    delete sound;
    display ((char*)"\n\n\n\n\n\n\n\n\n\n\n\n\n\n", LOG_MOST);
    display ((char*)"LINUX AIR COMBAT HAS COMPLETED NORMALLY.\n", LOG_MOST);
    display ((char*)"A complete log of all diagnostic messages from this session is available", LOG_MOST);
    display ((char*)"in a file named logfile.txt which you can find in the hidden .LAC folder", LOG_MOST);
    display ((char*)"beneath your home folder. This can be very handy as you configure for best", LOG_MOST);
    display ((char*)"performance with your joystick, keyboard, mouse, network, and display.\n", LOG_MOST);
    display ((char*)"Online help is available at AskMisterWizard.com\n\n", LOG_MOST);
    if (strcmp(CommunityHandle, "00000000000000") == 0 )
       { 
       display ((char*)"Your CommunityHandle has not yet been set, so other LAC players will never", LOG_MOST);
       display ((char*)"know your name. You should choose a name for yourself and then use a simple", LOG_MOST);
       display ((char*)"text editor to write it into the LacConfig.txt file that you will find in", LOG_MOST);
       display ((char*)"the hidden .LAC folder within your home directory. If you search for", LOG_MOST);
       display ((char*)"CommunityHandle within that file you will find detailed instructions.\n", LOG_MOST);
       }
    exit (EXIT_NORMAL);
    }

void game_view ()
{
	frame();
//	SDL_GL_SwapBuffers ();
	SDL_GL_SwapWindow(win[0]);

} 

int getJoystickAxisIndex (int n)
    {
    int idx = (n / 1000) * 10 + (n % 1000);
    if (idx < 0)
        {
        idx = 0;
        }
    if (idx >= maxjaxis)
        {
        idx = 0;
        }
    return idx;
    } 

char *getModelName (int id)
    {
    if (id == FIGHTER_P38L)
        {
        return (char *)"USA LOCKHEED P38L LIGHTNING";
        }
    else if (id == FIGHTER_P51D)
        {
        return (char *)"USA NORTH AMERICAN P51D MUSTANG";
        }
    else if (id == FIGHTER_A6M2)
        {
        return (char *)"JAPAN MITSUBISHI A6M2 ZERO";
        }
    else if (id == FIGHTER_HAWK)
        {
        return (char *)"WHIMSICAL HAWK";
        }
    else if (id == FIGHTER_SPIT9)
        {
        return (char *)"UK SUPERMARINE SPITFIRE MK9";
        }
    else if (id == FIGHTER_HURRICANE)
        {
        return (char *)"UK HAWKER HURRICANE";
        }
    else if (id == FIGHTER_KI43)
        {
        return (char *)"JAPAN NAKAJIMA KI43 OSCAR";
        }
    else if (id == BOMBER_B17)
        {
        return (char *)"USA BOEING B17G FLYING FORTRESS";
        }
    else if (id == FIGHTER_F4F)
        {
        return (char *)"USA GRUMMAN F4F WILDCAT";
        }
    else if (id == FIGHTER_F6F)
        {
        return (char *)"USA GRUMMAN F6F HELLCAT";
        }
    else if (id == FIGHTER_ME109G)
        {
        return (char *)"GERMANY MESSERSCHMITT ME109G";
        }
    else if (id == FIGHTER_P39)
        {
        return (char *)"USA BELL P39 AIRACOBRA";
        }
    else if (id == FIGHTER_P40)
        {
        return (char *)"USA CURTISS P40 WARHAWK";
        }
    else if (id == FIGHTER_F4U)
        {
        return (char *)"USA VOUGHT F4U CORSAIR";
        }
    else if (id == FIGHTER_P47D)
        {
        return (char *)"USA REPUBLIC P47D THUNDERBOLT";
        }
    else if (id == FIGHTER_FW190)
        {
        return (char *)"GERMANY FOCKE WULF FW-190 BUTCHERBIRD";
        }
    else if (id == BOMBER_JU87)
        {
        return (char *)"GERMANY JUNKERS JU-87 STUKA";
        }
    else if (id == BOMBER_G5M)
        {
        return (char *)"JAPAN MITSUBISHI G4M BETTY";
        }
    else if (id == BOMBER_B25)
        {
        return (char *)"USA NORTH AMERICAN B25 MITCHELL";
        }
    else if (id == BOMBER_B26)
        {
        return (char *)"USA MARTIN B26 MARAUDER";
        }
    else if (id == FIGHTER_IL16)
        {
        return (char *)"USSR POLYKARPOV I-16";
        }
    else if (id == FIGHTER_FIATG55)
        {
        return (char *)"ITALY FIAT G55 CENTAURO";
        }
    else if (id == FIGHTER_YAK9)
        {
        return (char *)"USSR YAKOVLEV YAK9";
        }
    else if (id == FIGHTER_N1K1)
        {
        return (char *)"JAPAN KAWANISHI N1K1 SHIDEN";
        }
    else if (id == BOMBER_B24)
        {
        return (char *)"USA CONSOLODATED B24 LIBERATOR";
        }
    else if (id == FIGHTER_LA5)
        {
        return (char *)"USSR LAVOCHKIN LA5";
        }
    else if (id == FIGHTER_LA7)
        {
        return (char *)"USSR LAVOCHKIN LA7";
        }
    else if (id == FIGHTER_IL2)
        {
        return (char *)"USSR ILYUSHIN IL2 STURMOVIK";
        }
    else if (id == FIGHTER_MACCIC202)
        {
        return (char *)"ITALY MACCI C202 FOLGORE";
        }
    else if (id == BOMBER_LANCASTER)
        {
        return (char *)"UK AVRO LANCASTER";
        }
    else if (id == BOMBER_MOSQUITOB)
        {
        return (char *)"UK DE HAVILAND MOSQUITO B";
        }
    else if (id == FIGHTER_TYPHOON)
        {
        return (char *)"UK HAWKER TYPHOON";
        }
    else if (id == FIGHTER_YAK1)
        {
        return (char *)"USSR YAKOVLEV YAK1";
        }
    else if (id == BOMBER_B29)
        {
        return (char *)"USA BOEING B29 SUPERFORTRESS";
        }
    else if (id == FIGHTER_DW520)
        {
        return (char *)"FRENCH DEWOITINE 520";
        }
    else if (id == BOMBER_SB2C)
        {
        return (char *)"USA CURTISS SB2C HELLDIVER";
        }
    else if (id == BOMBER_TBF)
        {
        return (char *)"USA GRUMMAN TBF AVENGER";
        }
    else if (id == FIGHTER_ME163)
        {
        return (char *)"GERMANY ME163 KOMET";
        }
    else if (id == FIGHTER_TEMPEST)
        {
        return (char *)"UK HAWKER TEMPEST";
        }
    else if (id == FIGHTER_D3A)
        {
        return (char *)"JAPAN AICHI D3A VAL";
        }
    else if (id == BOMBER_B5N)
        {
        return (char *)"JAPAN NAKAJIMA B5N KATE";
        }
    else if (id == BOMBER_DAUNTLESS)
        {
        return (char *)"USA DOUGLAS DAUNTLESS";
        }
    else if (id == FIGHTER_ME110)
        {
        return (char *)"GERMANY MESSERSCHMIDT ME110";
        }
    else if (id == BOMBER_DORNIER)
        {
        return (char *)"GERMANY DORNIER 17";
        }
    else if (id == BOMBER_HE111)
        {
        return (char *)"GERMANY HEINKEL HE111";
        }
    else if (id == BOMBER_JU88)
        {
        return (char *)"GERMANY JUNKERS JU88";
        }
    else if (id == FIGHTER_KI84)
        {
        return (char *)"JAPAN NAKAJIMA KI84";
        }
    else if (id == FIGHTER_KI61)
        {
        return (char *)"JAPAN KAWASAKI KI61";
        }
    else if (id == FIGHTER_GENERIC01)
        {
        return (char *)"UNDEFINED GENERIC01";
        }
    else if (id == FIGHTER_A6M5)
        {
        return (char *)"JAPAN MITSUBISHI A6M5 ZERO";
        }
    else if (id == FIGHTER_SPIT5)
        {
        return (char *)"UK SUPERMARINE SPITFIRE MK5";
        }
    else if (id == FIGHTER_P51B)
        {
        return (char *)"USA NORTH AMERICAN P51B MUSTANG";
        }
    else if (id == FIGHTER_P47B)
        {
        return (char *)"USA REPUBLIC P47B THUNDERBOLT";
        }
    else if (id == FIGHTER_ME109F)
        {
        return (char *)"GERMANY MESSERSCHMIDT ME109F";
        }
    else if (id == FIGHTER_P38F)
        {
        return (char *)"USA LOCKHEED P38F LIGHTNING";
        }
    else if (id == BOMB01)
        {
        return (char *)"BOMBS";
        }
    else if (id == MISSILE_GROUND1)
        {
        return (char *)"AIR TO GROUND WEAPON PACK";
        }
    else if (id == MISSILE_DF1)
        {
        return (char *)"ROCKETS";
        }
    else if (id == MISSILE_AIR2)
        {
        return (char *)"AIR-AIR MISSILE PACK";
        }
    return (char *)"NO INFO";
    } 

int getTrainingIdFromValue (int n)
    {
    sprintf (DebugBuf, "getTrainingIdFromValue() input = %d", n);
    display (DebugBuf, LOG_MOST);
    if (n == 0)
       {
       return MISSION_TUTORIAL;
       }
    if (n == 1)
       {
       return MISSION_TUTORIAL2;
       }
    if (n == 2)
       {
       return MISSION_TUTORIAL3;
       }
    if (n == 3)
       {
       return MISSION_FREEFLIGHTWW2;
       }
    if (n == 4)
       {
       return MISSION_HEADTOHEAD00;
       }
    if (n == 5)
       {
       return MISSION_NETWORKBATTLE01;
       }
    if (n == 6)
       {
       return MISSION_NETWORKBATTLE02;
       }
    if (n == 7)
       {
       return MISSION_NETWORKBATTLE03;
       }
    return (100);
    } 

float getView ()
    {
    if (weather == WEATHER_THUNDERSTORM && view > 40.0)
        {
        return 40.0;
        }
    return view;
    } 

void init_display ()
    { 
    CVector3 vec;
    CColor color (255, 160, 160, 255);
    CColor red (255, 0, 0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glPushMatrix ();
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    float LacTimerRatio = 1.0 - ((float)(LacTimer - 6900)/3000.0);
    
    glPushMatrix ();
    
    float ThetaRotation1 = -20 + LacTimer/22;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation1;
    rot.c = 00;
    float ZDepth1 = -80 +LacTimer/40;
    glTranslatef (-4.0, 0.15, ZDepth1); 
    model_figv.draw (&vec, &tl, &rot, 1.0, 3.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation2 = -20 + (LacTimer-1600)/26;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation2;
    rot.c = 00;
    float ZDepth2 = -80 + (LacTimer-1600)/40;
    glTranslatef (-3, 0.2, ZDepth2); 
    model_figx.draw (&vec, &tl, &rot, 1.0, 3.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation3 = -20 + (LacTimer-3200)/25;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation3;
    rot.c = 00;
    float ZDepth3 = -80 +(LacTimer-3200)/40;
    glTranslatef (-1, 0.0, ZDepth3); 
    model_figAQ.draw (&vec, &tl, &rot, 1.0, 3.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation4 = -20 + (LacTimer-4800)/24;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation4;
    rot.c = 10;
    float ZDepth4 = -80 +(LacTimer-4800)/40;
    glTranslatef (-1, 0.2, ZDepth4); 
    model_figc.draw (&vec, &tl, &rot, 1.0, 3.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation5 = -20 + (LacTimer-6400)/23;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation5;
    rot.c = 00;
    float ZDepth5 = -80 +(LacTimer-6400)/40;
    glTranslatef ((10.0 - (float)((float)LacTimer/800.0)), (8.0-(float)((float)LacTimer/1200.0)), ZDepth5); 
    model_fig.draw (&vec, &tl, &rot, 1.0, 1.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation6 = -20 + (LacTimer-6400)/22;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation6;
    rot.c = 00;
    float ZDepth6 = -80 +(LacTimer-6400)/40;
    glTranslatef ((12.0 - (float)((float)LacTimer/800.0)), (6.0-(float)((float)LacTimer/1500.0)), ZDepth6); 
    model_fig.draw (&vec, &tl, &rot, 1.0, 3.0, 0); 
    glPopMatrix ();
    
    glPushMatrix ();
    
    float ThetaRotation7 = -20 + (LacTimer-6400)/21;
    rot.a = 270; 
    rot.b = 50 - ThetaRotation7;
    rot.c = 20;
    float ZDepth7 = -80 +(LacTimer-6400)/40;
    glTranslatef ((14.0 - (float)((float)LacTimer/800.0)), (4.0-(float)((float)LacTimer/1500.0)), ZDepth7); 
    model_fig.draw (&vec, &tl, &rot, 1.0, 0.0, 0); 
    glPopMatrix ();
    if (LacTimer > 3700 && LacTimer < 3900)
       {
       sound->setVolume(SOUND_CANNON1, 10);
       sound->play (SOUND_CANNON1, false);
       }
    if (LacTimer > 5600 && LacTimer < 5900)
       {
       sound->setVolume(SOUND_CANNON1, 20);
       sound->play (SOUND_CANNON1, false);
       }
    if (LacTimer > 7700 && LacTimer < 7900)
       {
       sound->setVolume(SOUND_CANNON1, 30);
       sound->play (SOUND_CANNON1, false);
       }
    if (LacTimer > 9500 && LacTimer < 9600)
       {
       sound->setVolume(SOUND_CANNON1, 70);
       sound->play (SOUND_CANNON1, false);
       }
    if (LacTimer > 9750 && LacTimer < 9990)
       {
       sound->setVolume(SOUND_CANNON1, 40);
       sound->play (SOUND_CANNON1, false);
       }
    if (LacTimer > 11750 && LacTimer < 11990)
       {
       sound->setVolume(SOUND_CANNON1, 5);
       sound->play (SOUND_CANNON1, false);
       }
    glDisable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    if (LacTimer > 300)
        {
        float xf = 1.3F, yf = 0.65F, zf = 1.4F;
        int col = (LacTimer - 300) / 2;
        if (col < 0 || col > 255)
            {
            col = 255;
            }
        glColor3ub (col, col, col);
        glPushMatrix ();
        glTranslatef (0, 0.5F, 0);
        gl->enableTextures (textitle->textureID);
        if (antialiasing)
            {
            gl->enableLinearTexture (textitle->textureID);
            }
        else
            {
            gl->disableLinearTexture (textitle->textureID);
            }
        glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBegin (GL_QUADS);
        if (LacTimer > 7900)
           {
           if (LacTimerRatio > 0)
              {
              glTexCoord2d (0, 1);
              glVertex3f (-xf, yf, -zf);
              glTexCoord2d (1, 1);
              glVertex3f ( xf, yf, -zf);
              glTexCoord2d (1, 0);
              glVertex3f ( xf, -yf, -zf -LacTimerRatio * 0.05 * (sin((float)((float)(LacTimer)/100.0))));
              glTexCoord2d (0, 0);
              glVertex3f (-xf, -yf, -zf -LacTimerRatio * 0.05 * (sin((float)((float)(LacTimer)/100.0))));
              }
           }
        if (LacTimer < 7900)
           {
           glTexCoord2d (0, 1);
           glVertex3f (-xf, yf, -zf);
           glTexCoord2d (1, 1);
           glVertex3f ( xf, yf, -zf);
           glTexCoord2d (1, 0);
           glVertex3f ( xf, -yf, -zf);
           glTexCoord2d (0, 0);
           glVertex3f (-xf, -yf, -zf);
           }
        if (LacTimer > 9900)
           {
           float LacTimerRatio = 1.0 - ((float)(LacTimer - 9900)/3000.0);
           if (LacTimerRatio > 0)
              {
              glTexCoord2d (0, 1);
              glVertex3f (-xf, yf, -zf);
              glTexCoord2d (1, 1);
              glVertex3f ( xf, yf, -zf);
              glTexCoord2d (1, 0);
              glVertex3f ( xf, -yf, -zf -LacTimerRatio * (sin((float)((float)(LacTimer)/100.0))));
              glTexCoord2d (0, 0);
              glVertex3f (-xf, -yf, -zf -LacTimerRatio * (sin((float)((float)(LacTimer)/100.0))));
              }
           else
              {
              glTexCoord2d (0, 1);
              glVertex3f (-xf, yf, -zf);
              glTexCoord2d (1, 1);
              glVertex3f ( xf, yf, -zf);
              glTexCoord2d (1, 0);
              glVertex3f ( xf, -yf, -zf);
              glTexCoord2d (0, 0);
              glVertex3f (-xf, -yf, -zf);
              }
           }
        glEnd ();
        glPopMatrix ();
        glDisable (GL_BLEND);
        }
    glDisable (GL_LIGHTING);
    glPopMatrix ();
    glDisable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glDisable (GL_BLEND);
    font1->zoom = 0.18;
    if (LacTimer < 9900)
       {
       font1->drawTextCentered(0, 12.5, -3, (char *)VERSIONSTRING, &red);
       font1->zoom = 0.08;
       font1->drawTextCentered(0, 26,-3,(char *)"                    BY ROBERT BOSEN.");
       }
    else
       {
       font1->drawTextCentered(0, (15.5 - ((float)(LacTimer - 9900)/50.0)), -3 - LacTimerRatio * 4.0 * sin((float)LacTimer/200), (char *)VERSIONSTRING, &red);
       font1->zoom = 0.08;
       font1->drawTextCentered(0, (26 - ((float)(LacTimer - 9900)/30.0)), -3 - LacTimerRatio * 4.0 * sin((float)LacTimer/220),(char *)"                    BY ROBERT BOSEN.");
       }
    font1->zoom = 0.08;
    font1->drawTextCentered(0,-20,-3,(char *)"DERIVED FROM THOMAS A. DREXL'S GL-117");
    font1->drawTextCentered(0,-28,-4,(char *)"NO COMMERCIAL PRODUCTS WERE USED IN THE DEVELOPMENT OF THIS SOFTWARE.");
    font1->zoom = 0.1;
    } 

/****************************************************************************
  DATA INITIALIZATION
****************************************************************************/

// initialize game data
void InitLac ()
    {
    display ((char*)"Function entry: InitLac()", LOG_MOST);
    void pleaseWait ();
    void ReshapeMenu ();
    int i, i2;
    
    tlinf = new CVector3 (1E10, 1E10, 1E10);
    tlminf = new CVector3 (-1E10, -1E10, -1E10);
    tlnull = new CVector3 (0, 0, 0);
    rotnull = new CRotation ();
    rotmissile = new CRotation ();
    rotmissile->a = 90;
    rotmissile->c = 270;
    
    for (i = 0; i < maxgroundobj; i ++)
        {
        groundobj [i] = new DynamicObj (space, &model_tent1, 3);
        }
    explsphere = new CSphere ();
    ((CSphere *) explsphere)->init (1, 9);
    CColor explcolor (255, 255, 1);
    explsphere->setColor (&explcolor);
    explsphere->alpha = true;
    for (i = 0; i < explsphere->object [0]->numVertices; i ++)
        {
        explsphere->object [0]->vertex [i].color.setColor (myrandom (100) + 155, myrandom (100) + 100, 0, myrandom (3) / 2 * 255);
        }
    for (i = 0; i < maxexplosion; i ++)
        {
        explosion [i] = new CExplosion (space, explsphere);
        }
    for (i = 0; i < maxblacksmoke; i ++)
        {
        blacksmoke [i] = new CBlackSmoke (space);
        }
    for (i = 0; i < maxfighter; i ++)
        {
        ThreeDObjects [i] = new AIObj (space, &model_fig, 0.4);
        for (i2 = 0; i2 < 12; i2 ++)
            {
            ThreeDObjects [i]->addRefModel (&model_bomb01, tlnull, rotmissile, 0.2);
            }
        }
    highclouds = new HighClouds (25);
    highclouds->setTexture (texclouds3);
    highclouds2 = new HighClouds (25);
    highclouds2->setTexture (texclouds3);
    objsphere = new CSphere (1, 9, 1, 1, 1);
    sphere = new CSpaceObj (objsphere, 10.0);
    sphere->rot->a = 90;
    sphere->rot->b = 90;
    sphere->rot->c = 270;
    sphere->draw = 2;
    sphere->drawlight = false;
    flash1 = new Flash ();
    for (i = 0; i < maxMachineGunBullet; i ++)
        {
        MachineGunBullet [i] = new DynamicObj (space, &model_cannon1, 0.07);
        }
    for (i = 0; i < maxmissile; i ++)
        {
        missile [i] = new AIObj (space, &model_bomb01, 0.1);
        }
    for (i = 0; i < maxflare; i ++)
        {
        flare [i] = new DynamicObj (space, &model_flare1, 0.1);
        }
    for (i = 0; i < maxchaff; i ++)
        {
        chaff [i] = new DynamicObj (space, &model_chaff1, 0.1);
        }
    for (i = 0; i < maxstar; i ++)
        {
        star [i] = new Star (myrandom (360), myrandom (85), 0.4 + 0.1 * myrandom (8));
        }
    cockpit = new Cockpit ();
    pleaseWait ();
    createMission (MISSION_DEMO); 
    game_levelInit ();
    ReshapeMenu ();
    } 

void init_mouse (int button, int state, int x, int y)
    {
    void KeyInit (int key, int x, int y);
    KeyInit (32, x, y);
    }

void InitReshape ()
    {
    // use whole window
    glViewport (0, 0, (GLint) width, (GLint) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    
    gluPerspective (VisibleAngle, 1.0, nearclippingplane, 80);
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);
    } 

void init_timer (Uint32 dt)
    {
    void KeyInit (int key, int x, int y);
    void proceedFire ();
    LacTimer += dt;
    initsynchrotimer += dt;
    if (initsynchrotimer > 20)
        {
        initsynchrotimer -= 20;
        dt = 20;
        }
    else
        {
        return;
        }
    int r = myrandom (100);
    if (r == 50)
        {
        r = myrandom (100);    // do not optimize this: random number generator initialization
        }
    
    if (inittimer >= 8000)
        {
        KeyInit (27, 0, 0); 
        }
    inittimer ++;
    sdldisplay = true;
    } 

void KeyCredits (unsigned char key, int x, int y)
    {
    display ((char*)"Function Entry: KeyCredits()", LOG_MOST);
    void switch_menu ();
    if (key == 27)
        {
        switch_menu ();
        }
    } 

void KeyFighter (unsigned char key, int x, int y)
    {
    display ((char*)"Function Entry: KeyFighter()", LOG_MOST);
    void switch_menu ();
    if (key == 27)
        {
        switch_menu ();
        }
    } 

void KeyFinish (unsigned char key, int x, int y)
    {
    display ((char*)"Function Entry: KeyFinish()", LOG_MOST);
    if (key == 27)
        {
        void switch_menu ();
        switch_menu ();
        }
    } 

void KeyGame (unsigned int key, int x, int y)
    {
    display ((char*)"Function Entry: KeyGame()", LOG_MOST);
    void switch_menu ();
    void ReshapeGame ();
    unsigned int hikey = key;
    unsigned int lokey = key;
    sprintf (DebugBuf, "KeyGame() received %d", key);
    display (DebugBuf, LOG_MOST);
    
    NetworkMissionIsActiveWithDataFlow = false;
    if (camera == 50 && game == GAME_PAUSE)
        {
        camera = 0;
        game = GAME_PLAY;
        return;
        }
    if (key == 557)
       { 
       if (!CapsLockActive)
          {
          CapsLockActive = true;
          display ((char*)"KeyGame() setting CapsLockActive true.", LOG_MOST);
          }
       }
    if (key == 27)
        {
        switch_menu ();
        }
    if (CapsLockActive)
       { 
       
       if (key == 8)
          { 
          if (MorseSquelches[PreviousMorseSender -1] == false)
             { 
             MorseSquelches[PreviousMorseSender -1] = true; 
             sprintf (SystemMessageBufferA, "MORSE: SQUELCHED PLAYER %d", PreviousMorseSender);
             NewSystemMessageNeedsScrolling = true;
             }
          else
             { 
             
             unsigned char i;
             for (i=0; i<=9; i++)
                {
                MorseSquelches[i] = false;
                }
             sprintf (SystemMessageBufferA, "MORSE: UNSQUELCHED ALL PLAYERS");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       if (key == 92)
          { 
          key = '?';
          }
       if (key >= 32 && key <= 192)
          { 
          
          TransmitCharacterViaMorseRadio (key);
          return; 
          }
       }
    if (hikey == key_AutoPilot || lokey == key_AutoPilot)
        {
        event_AutoPilot ();
        }
    else if (hikey == key_SECONDARY || lokey == key_SECONDARY)
        {
        event_fireMissile ();
        }
    else if (hikey == key_DROPFLARE || lokey == key_DROPFLARE)
        {
        sound->play (SOUND_CLICK1, false);
        event_fireFlare ();
        }
    else if (hikey == key_DROPCHAFF || lokey == key_DROPCHAFF)
        {
        sound->play (SOUND_CLICK1, false);
        event_fireChaff ();
        }
    else if (hikey == key_PRIMARY || lokey == key_PRIMARY)
        {
        event_fireCannon ();
        }
    else if (hikey == key_WEAPONSELECT || lokey == key_WEAPONSELECT)
        {
        event_selectMissile ();
        }
    else if (hikey == key_TARGETNEXT || lokey == key_TARGETNEXT)
        {
        event_targetNext ();
        }
    else if (hikey == key_TARGETPREVIOUS || lokey == key_TARGETPREVIOUS)
        {
        event_targetPrevious ();
        }
    else if (hikey == key_TARGETVOCALIZE || lokey == key_TARGETVOCALIZE)
        {
        event_targetVocalize();
        }
    else if (hikey == key_THRUSTDOWN || lokey == key_THRUSTDOWN)
        {
        event_thrustDown();
        }
    else if (hikey == key_THRUSTUP || lokey == key_THRUSTUP)
        {
        event_thrustUp();
        }
    else if (hikey == key_ToggleViewIntExt || lokey == key_ToggleViewIntExt)
        {
        sound->play (SOUND_CLICK1, false);
        event_ToggleViewIntExt();
        }
    else if (hikey == key_ZoomFovIn || lokey == key_ZoomFovIn)
        {
        sound->play (SOUND_CLICK1, false);
        event_ZoomFovIn();
        }
    else if  (hikey == key_ZoomFovOut || lokey == key_ZoomFovOut)
        {
        sound->play (SOUND_CLICK1, false);
        event_ZoomFovOut();
        }
    else if (hikey == key_FlapsDn || lokey == key_FlapsDn)
        {
        event_FlapsDN();
        }
    else if (hikey == key_FlapsUp || lokey == key_FlapsUp)
        {
        event_FlapsUP();
        }
    else if (hikey == key_GunCameraToggle || lokey == key_GunCameraToggle)
        {
        event_GunCameraToggle();
        }
    else if (hikey == key_SpeedBrake || lokey == key_SpeedBrake)
        {
        event_ToggleSpeedBrake();
        }
    else if (hikey == key_Undercarriage || lokey == key_Undercarriage)
        {
        event_ToggleUndercarriage();
        }
    else if (hikey == key_WepAfterburner || lokey == key_WepAfterburner)
        {
        event_ToggleWEP();
        }
    else if (hikey == key_TrimElevatorDn || lokey == key_TrimElevatorDn)
        {
        event_TrimElevatorDn();
        }
    else if (hikey == key_TrimElevatorUp || lokey == key_TrimElevatorUp)
        {
        event_TrimElevatorUp();
        }
    else if (hikey == key_TrimRudderLeft || lokey == key_TrimRudderLeft)
        {
        event_TrimRudderLeft();
        }
    else if (hikey == key_TrimRudderRight || lokey == key_TrimRudderRight)
        {
        event_TrimRudderRight();
        }
    else if (hikey == key_PRIMARY || lokey == key_PRIMARY)
        {
        fplayer->autofire = false;
        }
    else if (hikey == key_RadarZoomIn || lokey == key_RadarZoomIn)
        {
        event_RadarZoomIn();
        }
    else if (hikey == key_RadarZoomOut || lokey == key_RadarZoomOut)
        {
        event_RadarZoomOut();
        }
    else if (hikey == key_RadarOnOff || lokey == key_RadarOnOff)
        {
        event_RadarOnOff();
        }
    else if (hikey == key_HudOnOff || lokey == key_HudOnOff)
        {
        event_HudOnOff();
        }
    else if (hikey == key_IffOnOff || lokey == key_IffOnOff)
        {
        event_IffOnOff();
        }
    else if (hikey == key_HudLadderBarsOnOff || lokey == key_HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    else if (hikey == key_MapViewOnOff || lokey == key_MapViewOnOff)
        {
        event_MapViewOnOff();
        }
    else if (hikey == key_MapZoomIn || lokey == key_MapZoomIn)
        {
        event_MapZoomIn();
        }
    else if (hikey == key_MapZoomOut || lokey == key_MapZoomOut)
        {
        event_MapZoomOut();
        }
    else if (hikey == key_MapScrollEast || lokey == key_MapScrollEast)
        {
        event_MapScrollEast();
        }
    else if (hikey == key_MapScrollWest || lokey == key_MapScrollWest)
        {
        event_MapScrollWest();
        }
    else if (hikey == key_MapScrollNorth || lokey == key_MapScrollNorth)
        {
        event_MapScrollNorth();
        }
    else if (hikey == key_MapScrollSouth || lokey == key_MapScrollSouth)
        {
        event_MapScrollSouth();
        }
    else if (hikey == key_MumblePtt || lokey == key_MumblePtt)
        {
        event_MumblePtt();
        }
    else if (hikey == key_MumbleWhisper || lokey == key_MumbleWhisper)
        {
        event_MumbleWhisper();
        }
    else if (hikey == key_CommsMacro01 || lokey == key_CommsMacro01)
        {
        event_CommsMacro01();
        }
    else if (hikey == key_CommsMacro02 || lokey == key_CommsMacro02)
        {
        event_CommsMacro02();
        }
    else if (hikey == key_CommsMacro03 || lokey == key_CommsMacro03)
        {
        event_CommsMacro03();
        }
    else if (hikey == key_CommsMacro04 || lokey == key_CommsMacro04)
        {
        event_CommsMacro04();
        }
    else if (hikey == key_CommsMacro05 || lokey == key_CommsMacro05)
        {
        event_CommsMacro05();
        }
    else if (hikey == key_CommsMacro06 || lokey == key_CommsMacro06)
        {
        event_CommsMacro06();
        }
    else if (hikey == key_CommsMacro07 || lokey == key_CommsMacro07)
        {
        event_CommsMacro07();
        }
    else if (hikey == key_CommsMacro08 || lokey == key_CommsMacro08)
        {
        event_CommsMacro08();
        }
    else if (hikey == key_CommsMacro09 || lokey == key_CommsMacro09)
        {
        event_CommsMacro09();
        }
    else if (hikey == key_CommsMacro10 || lokey == key_CommsMacro10)
        {
        event_CommsMacro10();
        }
    else if (hikey == key_CommsMacro11 || lokey == key_CommsMacro11)
        {
        event_CommsMacro11();
        }
    else if (hikey == key_CommsMacro12 || lokey == key_CommsMacro12)
        {
        event_CommsMacro12();
        }
    else if (hikey == key_CommsMacro13 || lokey == key_CommsMacro13)
        {
        event_CommsMacro13();
        }
    else if (hikey == key_CommsMacro14 || lokey == key_CommsMacro14)
        {
        event_CommsMacro14();
        }
    else if (hikey == key_CommsMacro15 || lokey == key_CommsMacro15)
        {
        event_CommsMacro15();
        }
    else if (hikey == key_CommsMacro16 || lokey == key_CommsMacro16)
        {
        event_CommsMacro16();
        }
    
    display ((char *)"Key Pressed 150729ab", LOG_ALL); 
    sprintf (DebugBuf, "%d", key); 
    display (DebugBuf, LOG_ALL);  
    
    if (key == key_LookBK)
        {
        event_LookBK();
        }
    if (key == key_LookDN)
        {
        event_LookDN();
        }
    if (key == key_LookFD)
        {
        event_LookFD();
        }
    if (key == key_LookLT)
        {
        event_LookLT();
        }
    if (key == key_LookRT)
        {
        event_LookRT();
        }
    if (key == key_LookUP)
        {
        event_LookUP();
        }
    sdldisplay = true;
    } 

void KeyInit (int key, int x, int y)
    {
    display ((char*)"Function Entry: KeyInit()", LOG_MOST);
    void InitLac ();
    void switch_menu ();
    gl->clearScreen (); 
    InitLac ();
    switch_menu ();
    fplayer->ai = true;
    camera = 5;
    starttime = SDL_GetTicks ();
    } 

void KeyMenu (unsigned char key, int x, int y)
    {
    display ((char*)"Function Entry: KeyMenu()", LOG_MOST);
    void switch_game ();
    void switch_quit();
    if (key == 27)   
        {
        if (MissionActive)
            {
            switch_game ();
            }
        else
            {
            switch_quit ();
            }
        }
    } 

void KeyMission (unsigned char key, int x, int y)
    {
    void switch_menu ();
    void pleaseWait ();
    void switch_game ();
    display ((char*)"Function Entry: KeyMission()", LOG_MOST);
    if (key == 27)
        {
        switch_menu ();
        }
    else if (key == 13)
        {
        pleaseWait ();
        if (!game_levelInit ())
            {
            switch_menu ();
            return;
            }
        switch_game ();
        MissionActive = true;
        }
    } 

void KeyStats (unsigned char key, int x, int y)
    {
    if (key == 27)
        {
        void switch_menu ();
        void pleaseWait ();
        pleaseWait ();
        display ((char *)"ESC key detected.", LOG_MOST);
        MissionActive = false;
        //createMission (MISSION_DEMO);
        game_levelInit ();
        sound->haltMusic ();
        switch_menu ();
        }
    } 

void KeyQuit (unsigned char key, int x, int y)
    {
    void switch_menu();
    void game_quit ();
    if (key == 'y' || key == 'Y')
        {
        game_quit ();
        }
    else
        {
        switch_menu ();
        }
    } 

void KeyupGame (unsigned int key, int x, int y)
    {
    display ((char *)"KeyupGame()", LOG_MOST); 
    unsigned int hikey = (unsigned int) key;
    unsigned int lokey = (unsigned int) key;
    if (hikey == key_PRIMARY || lokey == key_PRIMARY)
        {
        event_stopCannon ();
        }
    sprintf (DebugBuf, "%d", key); 
    display (DebugBuf, LOG_MOST);  
    if (key == 557)
      { 
      if (CapsLockActive)
         {
         CapsLockActive = false;
         display ((char*)"KeyupGame() setting CapsLockActive false.", LOG_MOST);
         }
      }
    if (key > 512)
       {
       key -= 512;
       }
    if (key == key_LookBK || key == key_LookDN || key == key_LookLT || key == key_LookRT || key == key_LookUP )
       { 
       event_LookFD();
       camera = 0; 
       }
    switch (key - 256)   
        {
        case KEY_UP:
            keyb_elev = 0;
            break;
        case KEY_DOWN:
            keyb_elev = 0;
            break;
        case KEY_LEFT:
            keyb_roll = 0;
            keyb_rudder = 0;
            break;
        case KEY_RIGHT:
            keyb_roll = 0;
            keyb_rudder = 0;
            break;
        case KEY_LALT:
            keyb_rudder = 0;
            break;
        case KEY_LCTRL:
            keyb_rudder = 0;
            break;
        }
    sdldisplay = true;
    } 

static void LacDisplayFunc ()
    {
    // Abstract Event Branch
    void pause_display ();
    void mission_display ();
    if (game == GAME_INIT)
        {
        init_display ();
        game_view ();
        }
    else if (game == GAME_PLAY)
        {
        DisplayGame ();
        game_view ();
        }
    else if (game == GAME_PAUSE)
        {
        DisplayGame ();
        pause_display ();
        game_view ();
        }
    else if (game == GAME_MENU)
        {
        if (camera == 50)
            {
            camera = 0;
            }
        DisplayGame ();
        DisplayMenu ();
        game_view ();
        }
    else if (game == GAME_CREDITS)
        {
        if (camera == 50)
            {
            camera = 0;
            }
        DisplayGame ();
        DisplayCredits ();
        game_view ();
        }
    else if (game == GAME_MISSION)
        {
        if (camera == 50)
            {
            camera = 0;
            }
        DisplayGame ();
        mission_display ();
        game_view ();
        }
    else if (game == GAME_STATS)
        {
        if (camera == 50)
            {
            camera = 0;
            }
        game_view ();
        pause_display ();
        }
    else if (game == GAME_FIGHTER)
        {
        DisplayGame ();
        DisplayFighter ();
        game_view ();
        }
    else if (game == GAME_FINISH)
        {
        if (camera == 50)
            {
            camera = 0;
            }
        DisplayGame ();
        game_view ();
        }
    else if (game == GAME_QUIT)
        {
        DisplayGame ();
        DisplayQuit ();
        game_view ();
        }
    } 

void LacFirstInit ()
    {
    
    void setMissiles (CModel *model);
    void InitReshape ();
    void setLightSource (int gamma);
    int i;
    display ((char *)"LacFirstInit() 150808AA Creating calculation tables", LOG_MOST);
    mathtab_init ();
    display ((char *)"LacFirstInit() 150808AB Creating advanced OpenGL methods", LOG_MOST);
    gl = new GL ();
    
    display ((char *)"Loading textures", LOG_MOST);
    texgrass = gl->genTextureTGA (dirs->getTextures ((char *)"grass1.tga"), 0, 0, 1, false);
    texrocks = gl->genTextureTGA (dirs->getTextures ((char *)"rocks1.tga"), 0, 0, 1, false);
    texwater = gl->genTextureTGA (dirs->getTextures ((char *)"water1.tga"), 0, 0, 1, false);
    texsand = gl->genTextureTGA (dirs->getTextures ((char *)"sand1.tga"), 0, 0, 1, false);
    texredsand = gl->genTextureTGA (dirs->getTextures ((char *)"redsand1.tga"), 0, 0, 1, false);
    texredstone = gl->genTextureTGA (dirs->getTextures ((char *)"redstone2.tga"), 0, 0, 1, false);
    texgravel1 = gl->genTextureTGA (dirs->getTextures ((char *)"gravel1.tga"), 0, 0, 1, false);
    texglitter1 = gl->genTextureTGA (dirs->getTextures ((char *)"glitter.tga"), 0, -1, 0, true);
    textree = gl->genTextureTGA (dirs->getTextures ((char *)"tree2.tga"), 0, -1, 1, true);
    textreeu = gl->genTextureTGA (dirs->getTextures ((char *)"treeu2.tga"), 0, -1, 1, true);
    textree2 = gl->genTextureTGA (dirs->getTextures ((char *)"tree1.tga"), 0, -1, 1, true);
    textreeu2 = gl->genTextureTGA (dirs->getTextures ((char *)"treeu1.tga"), 0, -1, 1, true);
    textree3 = gl->genTextureTGA (dirs->getTextures ((char *)"tree3.tga"), 0, 3, 1, true);
    textreeu3 = gl->genTextureTGA (dirs->getTextures ((char *)"treeu3.tga"), 0, 3, 1, true);
    textree4 = gl->genTextureTGA (dirs->getTextures ((char *)"tree4.tga"), 0, 3, 1, true);
    textreeu4 = gl->genTextureTGA (dirs->getTextures ((char *)"treeu4.tga"), 0, 3, 1, true);
    textree5 = gl->genTextureTGA (dirs->getTextures ((char *)"tree5.tga"), 0, -1, 1, true);
    textreeu5 = gl->genTextureTGA (dirs->getTextures ((char *)"treeu5.tga"), 0, -1, 1, true);
    texcactus1 = gl->genTextureTGA (dirs->getTextures ((char *)"cactus1.tga"), 0, 3, 1, true);
    texcactusu1 = gl->genTextureTGA (dirs->getTextures ((char *)"cactusu1.tga"), 0, 3, 1, true);
    texsmoke = gl->genTextureTGA (dirs->getTextures ((char *)"smoke1.tga"), 0, -1, 1, true);
    texsmoke2 = gl->genTextureTGA (dirs->getTextures ((char *)"smoke2.tga"), 0, -1, 1, true);
    texsmoke3 = gl->genTextureTGA (dirs->getTextures ((char *)"smoke3.tga"), 0, 5, 1, true);
    texsun = gl->genTextureTGA (dirs->getTextures ((char *)"sun2.tga"), 1, -1, 0, true);
    texmoon = gl->genTextureTGA (dirs->getTextures ((char *)"moon1.tga"), 1, 2, 0, true);
    texearth = gl->genTextureTGA (dirs->getTextures ((char *)"earth.tga"), 1, 0, 0, true);
    texflare1 = gl->genTextureTGA (dirs->getTextures ((char *)"flare1.tga"), 1, -1, 0, true);
    texflare2 = gl->genTextureTGA (dirs->getTextures ((char *)"flare2.tga"), 1, -1, 0, true);
    texflare3 = gl->genTextureTGA (dirs->getTextures ((char *)"flare3.tga"), 1, -1, 0, true);
    texflare4 = gl->genTextureTGA (dirs->getTextures ((char *)"flare4.tga"), 1, -1, 0, true);
    texcross = gl->genTextureTGA (dirs->getTextures ((char *)"cross.tga"), 0, -1, 1, true);
    texcross2 = gl->genTextureTGA (dirs->getTextures ((char *)"cross2.tga"), 0, -1, 1, true);
    texclouds1 = gl->genTextureTGA (dirs->getTextures ((char *)"clouds1.tga"), 0, -1, 1, true);
    texclouds2 = gl->genTextureTGA (dirs->getTextures ((char *)"clouds2.tga"), 0, 4, 1, true);
    texclouds3 = gl->genTextureTGA (dirs->getTextures ((char *)"clouds3.tga"), 0, 6, 1, true);
    texradar1 = gl->genTextureTGA (dirs->getTextures ((char *)"radar2.tga"), 0, -1, 0, true);
    texradar2 = gl->genTextureTGA (dirs->getTextures ((char *)"radar1.tga"), 0, -1, 0, true);
    display ((char *)"Loading Fonts", LOG_MOST);
    font1 = new Font (dirs->getTextures ((char *)"font1.tga"), 32, '!', 64);
    font2 = new Font (dirs->getTextures ((char *)"font2.tga"), 32, '!', 64);
    display ((char *)"Loading 3ds models:", LOG_MOST);
    display ((char *)" * p38Laa.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_fig, dirs->getModels ((char *)"p38Laa.3ds"));
    model_fig.setName ((char *)"P38");
    display ((char *)" * gl-14c.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figb, dirs->getModels ((char *)"gl-14c.3ds"));
    model_figb.setName ((char *)"HAWK");
    display ((char *)" * gl-50.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figi, dirs->getModels ((char *)"spitfire.3ds"));
    model_figi.setName ((char *)"SPITFIRE");
    display ((char *)" * b17aa.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figu, dirs->getModels ((char *)"b17aa.3ds"));
    model_figu.setName ((char *)"B17");
    display ((char *)" * a6m2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figv, dirs->getModels ((char *)"a6m2.3ds"));
    model_figv.setName ((char *)"A6M2");
    display ((char *)" * P51.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figc, dirs->getModels ((char *)"P51a00.3ds"));
    model_figc.setName ((char *)"P51D");
    display ((char *)" * P47.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figw, dirs->getModels ((char *)"P47aa.3ds"));
    model_figw.setName ((char *)"P47D");
    display ((char *)" * F4F.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAO, dirs->getModels ((char *)"F4F.3ds"));
    model_figAO.setName ((char *)"F4F");
    display ((char *)" * F6F.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figx, dirs->getModels ((char *)"F6F.3ds"));
    model_figx.setName ((char *)"F6F");
    display ((char *)" * F4U.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figy, dirs->getModels ((char *)"F4U.3ds"));
    model_figy.setName ((char *)"F4U");
    display ((char *)" * FW190.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figz, dirs->getModels ((char *)"FW190.3ds"));
    model_figz.setName ((char *)"FW190");
    display ((char *)" * JU87.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAA, dirs->getModels ((char *)"JU87.3ds"));
    model_figAA.setName ((char *)"jU87");
    display ((char *)" * IL16.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAB, dirs->getModels ((char *)"IL16.3ds"));
    model_figAB.setName ((char *)"IL16");
    display ((char *)" * FIATG55.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAC, dirs->getModels ((char *)"FIATG55.3ds"));
    model_figAC.setName ((char *)"FIATG55");
    display ((char *)" * ME109.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAD, dirs->getModels ((char *)"ME109.3ds"));
    model_figAD.setName ((char *)"ME109");
    display ((char *)" * P40.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAE, dirs->getModels ((char *)"P40.3ds"));
    model_figAE.setName ((char *)"P40");
    display ((char *)" * HURRICANE.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAF, dirs->getModels ((char *)"HURRICANE.3ds"));
    model_figAF.setName ((char *)"HURRICANE");
    display ((char *)" * KI43.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAG, dirs->getModels ((char *)"KI43.3ds"));
    model_figAG.setName ((char *)"KI43");
    display ((char *)" * Yak9.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAH, dirs->getModels ((char *)"Yak9.3ds"));
    model_figAH.setName ((char *)"YAK9");
    display ((char *)" * N1K1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAI, dirs->getModels ((char *)"N1K1.3ds"));
    model_figAI.setName ((char *)"N1K1");
    display ((char *)" * b24.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAJ, dirs->getModels ((char *)"b24.3ds"));
    model_figAJ.setName ((char *)"B24");
    display ((char *)" * P39.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAK, dirs->getModels ((char *)"P39.3ds"));
    model_figAK.setName ((char *)"P38");
    display ((char *)" * G4m.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAL, dirs->getModels ((char *)"G4m.3ds"));
    model_figAL.setName ((char *)"G4M");
    display ((char *)" * B25.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAM, dirs->getModels ((char *)"B25.3ds"));
    model_figAM.setName ((char *)"B25");
    display ((char *)" * B26.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAN, dirs->getModels ((char *)"B26.3ds"));
    model_figAN.setName ((char *)"B26");
    display ((char *)" * La5.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAP, dirs->getModels ((char *)"La5.3ds"));
    model_figAP.setName ((char *)"LA5");
    display ((char *)" * La7.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAQ, dirs->getModels ((char *)"La7.3ds"));
    model_figAQ.setName ((char *)"LA7");
    display ((char *)" * IL2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAR, dirs->getModels ((char *)"IL2.3ds"));
    model_figAR.setName ((char *)"IL2");
    display ((char *)" * MacciC202.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAS, dirs->getModels ((char *)"MacciC202.3ds"));
    model_figAS.setName ((char *)"MACCI");
    display ((char *)" * Lancaster.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAT, dirs->getModels ((char *)"Lancaster.3ds"));
    model_figAT.setName ((char *)"LANCASTER");
    display ((char *)" * MosquitoB.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAU, dirs->getModels ((char *)"MosquitoB.3ds"));
    model_figAU.setName ((char *)"MOSQUIOB");
    display ((char *)" * Typhoon.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAV, dirs->getModels ((char *)"Typhoon.3ds"));
    model_figAV.setName ((char *)"Typhoon");
    display ((char *)" * Yak1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAW, dirs->getModels ((char *)"Yak1.3ds"));
    model_figAW.setName ((char *)"YAK1");
    display ((char *)" * B29.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAX, dirs->getModels ((char *)"B29.3ds"));
    model_figAX.setName ((char *)"B29");
    display ((char *)" * DW520.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAY, dirs->getModels ((char *)"DW520.3ds"));
    model_figAY.setName ((char *)"DW520");
    display ((char *)" * SB2C.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figAZ, dirs->getModels ((char *)"SB2C.3ds"));
    model_figAZ.setName ((char *)"SB2C");
    display ((char *)" * TBF.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBA, dirs->getModels ((char *)"TBF.3ds"));
    model_figBA.setName ((char *)"TBF");
    display ((char *)" * ME163.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBB, dirs->getModels ((char *)"ME163.3ds"));
    model_figBB.setName ((char *)"ME163");
    display ((char *)" * TEMPEST.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBC, dirs->getModels ((char *)"TEMPEST.3ds"));
    model_figBC.setName ((char *)"TEMPEST");
    display ((char *)" * D3A.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBD, dirs->getModels ((char *)"D3A.3ds"));
    model_figBD.setName ((char *)"D3A");
    display ((char *)" * B5N.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBE, dirs->getModels ((char *)"B5N.3ds"));
    model_figBE.setName ((char *)"B5N");
    display ((char *)" * Dauntless.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBF, dirs->getModels ((char *)"Dauntless.3ds"));
    model_figBF.setName ((char *)"Dauntless");
    display ((char *)" * Me110.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBG, dirs->getModels ((char *)"ME110.3ds"));
    model_figBG.setName ((char *)"ME110");
    display ((char *)" * DORNIER.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBH, dirs->getModels ((char *)"Dornier17.3ds"));
    model_figBH.setName ((char *)"DORNIER");
    display ((char *)" * He111.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBI, dirs->getModels ((char *)"HE111.3ds"));
    model_figBI.setName ((char *)"HE111");
    display ((char *)" * JU88.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBJ, dirs->getModels ((char *)"JU88.3ds"));
    model_figBJ.setName ((char *)"JU88");
    display ((char *)" * KI84.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBK, dirs->getModels ((char *)"KI84.3ds"));
    model_figBK.setName ((char *)"KI84");
    display ((char *)" * KI61.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBL, dirs->getModels ((char *)"KI61.3ds"));
    model_figBL.setName ((char *)"KI61");
    display ((char *)" * Generic01.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBM, dirs->getModels ((char *)"Generic01.3ds"));
    model_figBM.setName ((char *)"GENERIC01");
    display ((char *)" * A6M5.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBN, dirs->getModels ((char *)"a6m5.3ds"));
    model_figBN.setName ((char *)"A6M5");
    display ((char *)" * SPIT5.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBO, dirs->getModels ((char *)"SPIT5.3ds"));
    model_figBO.setName ((char *)"SPIT5");
    display ((char *)" * P51B.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBP, dirs->getModels ((char *)"P51B.3ds"));
    model_figBP.setName ((char *)"P51B");
    display ((char *)" * P47B.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBQ, dirs->getModels ((char *)"P47B.3ds"));
    model_figBQ.setName ((char *)"P47B");
    display ((char *)" * ME109F.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBR, dirs->getModels ((char *)"ME109F.3ds"));
    model_figBR.setName ((char *)"ME109F");
    display ((char *)" * P38F.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_figBS, dirs->getModels ((char *)"P38F.3ds"));
    model_figBS.setName ((char *)"P38F");
    
    display ((char *)" * cannon1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_cannon1, dirs->getModels ((char *)"cannon1.3ds"));
    model_cannon1.cubex = HitBubbleRadius;
    model_cannon1.cubey = HitBubbleRadius;
    model_cannon1.cubez = HitBubbleRadius;
    display ((char *)" * cannon1b.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_cannon1b, dirs->getModels ((char *)"cannon1b.3ds"));
    model_cannon1b.cubex = HitBubbleRadius;
    model_cannon1b.cubey = HitBubbleRadius;
    model_cannon1b.cubez = HitBubbleRadius;
    
    display ((char *)" * cannon2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_cannon2, dirs->getModels ((char *)"cannon2.3ds"));
    model_cannon2.nolight = true;
    model_cannon2.alpha = true;
    for (i = 0; i < 4; i ++)
        {
        model_cannon2.object [0]->vertex [i].color.c [0] = 255;
        model_cannon2.object [0]->vertex [i].color.c [1] = 255;
        model_cannon2.object [0]->vertex [i].color.c [2] = 0;
        model_cannon2.object [0]->vertex [i].color.c [3] = 255;
        }
    model_cannon2.object [0]->vertex [1].color.c [3] = 50;
    model_cannon2.object [0]->vertex [2].color.c [3] = 50;
    model_cannon2.cubex = HitBubbleRadius;
    model_cannon2.cubey = HitBubbleRadius;
    model_cannon2.cubez = HitBubbleRadius;
    display ((char *)" * cannon2b.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_cannon2b, dirs->getModels ((char *)"cannon2b.3ds"));
    model_cannon2b.nolight = true;
    model_cannon2b.alpha = true;
    for (int i2 = 0; i2 < 2; i2 ++)
        {
        for (i = 0; i < 4; i ++)
            {
            model_cannon2b.object [i2]->vertex [i].color.c [0] = 255;
            model_cannon2b.object [i2]->vertex [i].color.c [1] = 255;
            model_cannon2b.object [i2]->vertex [i].color.c [2] = 0;
            model_cannon2b.object [i2]->vertex [i].color.c [3] = 255;
            }
        model_cannon2b.object [i2]->vertex [1].color.c [3] = 50;
        model_cannon2b.object [i2]->vertex [2].color.c [3] = 50;
        }
    model_cannon2b.cubex = HitBubbleRadius;
    model_cannon2b.cubey = HitBubbleRadius;
    model_cannon2b.cubez = HitBubbleRadius;
    display ((char *)" * flare1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_flare1, dirs->getModels ((char *)"flare1.3ds"));
    model_flare1.setName ((char *)"FLARE");
    model_flare1.alpha = true;
    model_flare1.nolight = true;
    display ((char *)" * chaff1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_chaff1, dirs->getModels ((char *)"chaff1.3ds"));
    model_chaff1.setName ((char *)"CHAFF");
    model_chaff1.alpha = true;
    model_chaff1.nolight = true;
    display ((char *)" * bomb1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_bomb01, dirs->getModels ((char *)"bomb1.3ds"));
    model_bomb01.setName ((char *)"BOMB 500 LB.");
    display ((char *)" * missile2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile2, dirs->getModels ((char *)"missile2.3ds"));
    model_missile2.setName ((char *)"AAM HS MK2");
    display ((char *)" * missile3.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile3, dirs->getModels ((char *)"missile3.3ds"));
    model_missile3.setName ((char *)"AAM HS MK3");
    display ((char *)" * missile4.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile4, dirs->getModels ((char *)"missile4.3ds"));
    model_missile4.setName ((char *)"AGM MK1");
    display ((char *)" * missile5.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile5, dirs->getModels ((char *)"missile5.3ds"));
    model_missile5.setName ((char *)"AGM MK2");
    display ((char *)" * missile6.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile6, dirs->getModels ((char *)"missile6.3ds"));
    model_missile6.setName ((char *)"ROCKETS");
    display ((char *)" * missile7.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile7, dirs->getModels ((char *)"missile7.3ds"));
    model_missile7.setName ((char *)"AAM FF MK1");
    display ((char *)" * missile8.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_missile8, dirs->getModels ((char *)"missile8.3ds"));
    model_missile8.setName ((char *)"AAM FF MK2");
    display ((char *)" * flak2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_flak1, dirs->getModels ((char *)"flak2.3ds"));
    model_flak1.setName ((char *)"SA CANNON");
    display ((char *)" * tank1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_tank1, dirs->getModels ((char *)"tank1.3ds"));
    model_tank1.setName ((char *)"WIESEL");
    model_tank1.scaleTexture (0.5, 0.5);
    display ((char *)" * tank2.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_tank2, dirs->getModels ((char *)"tank2.3ds"));
    model_tank2.setName ((char *)"PANTHER");
    model_tank2.scaleTexture (0.5, 0.5);
    display ((char *)" * hall1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_battleship, dirs->getModels ((char *)"Battleship.3ds"));
    model_battleship.setName ((char *)"BATTLESHIP");
    display ((char *)" * radar.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_radar, dirs->getModels ((char *)"radar.3ds"));
    model_radar.setName ((char *)"RADAR");
    display ((char *)" * RadarReflector01.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_RadarReflector, dirs->getModels ((char *)"RadarReflector01.3ds"));
    model_RadarReflector.setName ((char *)"HQRADAR");
    display ((char *)" * Airfield00.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_Airfield00, dirs->getModels ((char *)"Airfield00.3ds"));
    model_Airfield00.setName ((char *)"AIRFIELD00");
    display ((char *)" * rubble.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_rubble1, dirs->getModels ((char *)"rubble.3ds"));
    model_rubble1.setName ((char *)"RUBBLE");
    display ((char *)" * depot1.3ds", LOG_MOST);
    g_Load3ds.Import3DS (&model_depot1, dirs->getModels ((char *)"depot1.3ds"));
    model_depot1.setName ((char *)"DEPOT");
    model_depot1.scaleTexture (2, 2);
    // enable Z-Buffer
    glEnable (GL_DEPTH_TEST);
    // fill polygons (GL_LINE for wireframe models)
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    display ((char *)"Setting up world geometry", LOG_MOST);
    space = new Space ();
    space->drawlight = true; 
    clip1 = space->z1;
    clip2 = space->z2;
    clip1->x = -ZOOM;
    clip1->y = -ZOOM;
    clip1->z = -ZOOM;
    clip2->x = ZOOM;
    clip2->y = ZOOM;
    clip2->z = ZOOM;
    //
    
    //
    tl.x = 0.0;
    tl.y = -0.5;
    tl.z = 0.5; 
    
    rot.a = 295; 
    rot.b = 10;
    rot.c = 00;
    textitle = new CTexture ();
    
    textitle = gl->genTextureTGA (dirs->getTextures ((char *)"LacTitle.tga"), 0, 0, 0, false);
    sungamma = 60;
    setLightSource (60);
    event_setAntialiasing ();
    glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHTING);
    glEnable (GL_COLOR_MATERIAL);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    } 

static void LacJoystickAxisFunc (int x, int y, int t, int r, int JoyHatX, int JoyHatY)
    {
    if (game == GAME_PLAY)
        {
        if (controls == CONTROLS_JOYSTICK || controls == CONTROLS_KEYBOARD)
            {
            
            game_joystickaxis (x, y, t, r, JoyHatX, JoyHatY);
            }
        }
    } 

static void LacJoystickButtonFunc (int button)
    {
    if (game == GAME_PLAY)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            game_JoystickButton (button);
            DoneFiringCannon=0;
            NetworkMissionIsActiveWithDataFlow = false; 
            }
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventJoystick (button);
        }
    } 

static void LacJoystickHatFunc (int hat)
    {
    
    int normhat = hat;

    if (hat % 1000 == SDL_HAT_RIGHT)
        {
        normhat = 100;
        display ((char *)"LacJoystickHatFunc() Hat100", LOG_MOST);
        }
    else if (hat % 1000 == SDL_HAT_UP)
        {
        normhat    = 101;
        display ((char *)"LacJoystickHatFunc() Hat101", LOG_MOST);
        }
    else if (hat % 1000 == SDL_HAT_LEFT)
        {
        normhat  = 102;
        display ((char *)"LacJoystickHatFunc() Hat102", LOG_MOST);
        }
    else if (hat % 1000 == SDL_HAT_DOWN)
        {
        normhat  = 103;
        display ((char *)"LacJoystickHatFunc() Hat103", LOG_MOST);
        }
    else
        {
        display ((char *)"LacJoystickHatFunc() Hat Released", LOG_MOST);
        event_stopCannon (); 
        DoneFiringCannon = 1;
        }
    normhat += (hat / 1000) * 1000;

    if (game == GAME_PLAY)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            game_joystickhat (normhat);
            }
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventJoystick (normhat);
        }
    } 

static void LacKeyboardFunc (unsigned char uckey, int x, int y)
    {
    int key = (int) uckey;

    sprintf (DebugBuf, "LacKeyboardFunc key= %d", key);
    display (DebugBuf, LOG_MOST);

// Note: Folded in from LacSpecial, no, causes problems
#ifdef NOTDEF
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        KeyGame (key + 256, x, y);
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventSpecial (key);
        }
#endif /* NOTDEF */
    
    if (key >= 'a' && key <= 'z')
        {
        key = toupper (key);
        }
    if (key >= 1 && key <= 13)
       { 
       sprintf (DebugBuf, "LacKeyboardFunc() detected num keypad key %d", key);
       display (DebugBuf, LOG_MOST);
       }
    if (key != 32)
       {
       SpaceBarPressed = false;
       }
    if (key != 13)
       {
       EnterPressed = false;
       }
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        KeyGame (key, x, y);
        }
    else if (game == GAME_INIT)
        {
        KeyInit (key, x, y);
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventKey (key);
        KeyMenu (key, x, y);
        }
    else if (game == GAME_CREDITS)
        {
        allmenus.eventKey (key);
        KeyCredits (key, x, y);
        }
    else if (game == GAME_MISSION)
        {
        MissionMenu.eventKey (key);
        if (EnterPressed)
           {
           display ((char*)"LacKeyboardFunc() EnterPressed = true", LOG_MOST);;
           }
        else
           {
           display ((char*)"LacKeyboardFunc() EnterPressed = false", LOG_MOST);;
           }
        if (SpaceBarPressed)
           {
           display ((char*)"LacKeyboardFunc() SpaceBarPressed = true", LOG_MOST);;
           }
        else
           {
           display ((char*)"LacKeyboardFunc() SpaceBarPressed = false", LOG_MOST);;
           }
        if (SpaceBarPressed || EnterPressed)
           {
           MouseMission(1, 1, mousex, mousey);
           display ((char*)"LacKeyboardFunc() invoking MouseMission(1, 1)", LOG_MOST);
           }
        else
           {
           MouseMission(1, 0, mousex, mousey);
           display ((char*)"LacKeyboardFunc() invoking MouseMission(1, 0)", LOG_MOST);
           }
        }
    else if (game == GAME_STATS)
        {
        KeyStats (key, x, y);
        }
    else if (game == GAME_FIGHTER)
        {
        fightermenu.eventKey(key);
        KeyFighter (key, x, y);
        }
    else if (game == GAME_FINISH)
        {
        allmenus.eventKey (key);
        KeyFinish (key, x, y);
        }
    else if (game == GAME_QUIT)
        {
        quitmenu.eventKey (key);
        MouseQuit (1, 1, x, y);
        }
    } 

static void LacKeyboardFuncUp (unsigned char key, int x, int y)
    {
    sprintf (DebugBuf, "LacKeyboardFuncUp key= %d", key);
    display (DebugBuf, LOG_MOST);

// Note: this causes problems, not doing it.
#ifdef NOTDEF
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        KeyupGame ((int) key + 256, x, y);
        }
#endif /* NOTDEF */


    if (key == 32)
       {
       SpaceBarPressed = true;
       display ((char*)"Setting SpaceBarPressed true.", LOG_MOST);
       }
    else
       {
       SpaceBarPressed = false;
       display ((char*)"Setting SpaceBarPressed false.", LOG_MOST);
       }
    if (key == 13)
       {
       EnterPressed = true;
       display ((char*)"Setting EnterPressed true.", LOG_MOST);
       }
    else
       {
       EnterPressed = false;
       display ((char*)"Setting EnterPressed false.", LOG_MOST);
       }

// Note: why wasn't this called here before? 
    if (key >= 'a' && key <= 'z')
        {
        key = toupper (key);
        }
    KeyupGame ((int) key, x, y);

    } 

static void LacMouseFunc (int button, int state, int x, int y)
    {
    display ((char*)"Function entry: LacMouseFunc()", LOG_MOST);
    // Abstract Event Branch
    void MouseMission (int button, int state, int x, int y);
    void MouseCreate (int button, int state, int x, int y);
    void MouseJoin (int button, int state, int x, int y);
    void MouseQuit (int button, int state, int x, int y);
    if (game == GAME_PLAY)
        {
        if (controls == CONTROLS_MOUSE)
            {
            sprintf (DebugBuf, "LacMouseFunc() game == CONTROLS_MOUSE. x=%d, y=%d, button=%d", x, y, button);
            display (DebugBuf, LOG_MOST);
            game_mouse (button, state, x, y);
            }
        }
    else if (game == GAME_INIT)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_INIT. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        init_mouse (button, state, x, y);
        }
    else if (game == GAME_MENU)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_MENU. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        allmenus.eventMouse (x, y, button);
        }
    else if (game == GAME_MISSION)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_MISSION. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        MissionMenu.eventMouse (x, y, button);
        MouseMission (button, state, x, y);
        }
    else if (game == GAME_STATS)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_STATS. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        statsmenu.eventMouse (x, y, button);
        }
    else if (game == GAME_FIGHTER)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_FIGHTER. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        fightermenu.eventMouse (x, y, button);
        }
    else if (game == GAME_QUIT)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_QUIT. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        quitmenu.eventMouse (x, y, button);
        MouseQuit (button, state, x, y);
        }
    else if (game == GAME_CREDITS)
        {
        sprintf (DebugBuf, "LacMouseFunc() game == GAME_CREDITS. x=%d, y=%d, button=%d", x, y, button);
        display (DebugBuf, LOG_MOST);
        credits_mouse (button, state, x, y);
        }
    } 

static void LacPassiveMotionFunc (int x, int y)
    {
    void MouseMission (int button, int state, int x, int y);
    void MouseCreate (int button, int state, int x, int y);
    void MouseJoin (int button, int state, int x, int y);
    void MouseQuit (int button, int state, int x, int y);
    mousex = x;
    mousey = y;
    if (game == GAME_PLAY)
        {
        game_mousemotion (x, y);
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventMouse (x, y, 0);
        }
    else if (game == GAME_STATS)
        {
        statsmenu.eventMouse (x, y, 0);
        }
    else if (game == GAME_MISSION)
        {
        MissionMenu.eventMouse (x, y, 0);
        MouseMission (-1, -1, x, y);
        }
    else if (game == GAME_FIGHTER)
        {
        fightermenu.eventMouse (x, y, 0);
        }
    else if (game == GAME_QUIT)
        {
        quitmenu.eventMouse (x, y, 0);
        MouseQuit (-1, -1, x, y);
        }
    } 

static void LacPassiveRelMotionFunc (int xrel, int yrel)
    {
    if (game == GAME_PLAY)
        if (controls == CONTROLS_MOUSE)
            {
            game_mouserelmotion (xrel, yrel);
            }
    }

void LacReshapeFunc (int width, int height)
    {
    display ((char*)"Function entry: LacReshapFunc()", LOG_MOST);
    // Abstract Event Branch
    void ReshapeGame ();
    void ReshapeMenu ();
    void ReshapeCredits ();
    void ReshapeStats ();
    ::width = width;
    ::height = height;
    ::wantwidth = width;
    ::wantheight = height;
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        ReshapeGame ();
        }
    else if (game == GAME_MENU || game == GAME_MISSION || game == GAME_QUIT)
        {
        ReshapeMenu ();
        }
    else if (game == GAME_INIT)
        {
        InitReshape ();
        }
    else if (game == GAME_CREDITS || game == GAME_FINISH)
        {
        ReshapeCredits ();
        }
    else if (game == GAME_STATS)
        {
        ReshapeStats ();
        }
    } 

// Note: these no longer used, remove once everything's working
#ifdef NOTDEF
static void LacSpecialFunc (int key, int x, int y)
    {
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        KeyGame (key + 256, x, y);
        }
    else if (game == GAME_MENU)
        {
        allmenus.eventSpecial (key);
        }
    } 

static void LacSpecialFuncUp (int key, int x, int y)
    {
    if (game == GAME_PLAY || game == GAME_PAUSE)
        {
        KeyupGame (key + 256, x, y);
        }
    } 

#endif /* NOTDEF */

static void LacTimerFunc (int value)
    {
    // Abstract Event Branch
    void switch_menu ();
    void TimerCreate (Uint32 dt);
    void TimerCredits (Uint32 dt);
    void TimerFinish (Uint32 dt);
    void TimerGame (int dt);
    void TimerJoin (Uint32 dt);
    void TimerMenu (Uint32 dt);
    void TimerMission (Uint32 dt);
    void TimerStats (Uint32 dt);
    Uint32 akttime, dt;
    
    akttime = SDL_GetTicks ();
    if (LastTime == 0)
        {
        
        dt = 1; 
        }
    else
        {
        
        dt = akttime - LastTime;
        }
    
    LastTime = akttime;
    
    if (dt > 1000)
        {
        dt = 1;
        }
    
    DeltaTime = dt;

    if (game == GAME_PLAY)
        {
        TimerGame (dt);
        }
    else if (game == GAME_INIT)
        {
        init_timer (dt);
        }
    else if (game == GAME_MENU)
        {
        if (!MissionActive)
            {
            TimerGame (dt);
            }
        TimerMenu (dt);
        }
    else if (game == GAME_CREDITS)
        {
        if (!MissionActive)
            {
            TimerGame (dt);
            }
        TimerCredits (dt);
        }
    else if (game == GAME_MISSION)
        {
        if (!MissionActive)
            {
            TimerGame (dt);
            }
        TimerMission (dt);
        }
    else if (game == GAME_STATS)
        {
        TimerStats (dt);
        }
    else if (game == GAME_FIGHTER)
        {
        if (!MissionActive)
            {
            TimerGame (dt);
            }
        TimerMission (dt);
        }
    else if (game == GAME_FINISH)
        {
        TimerFinish (dt);
        }
    else if (game == GAME_QUIT)
        {
        if (!MissionActive)
            {
            TimerGame (dt);
            }
        TimerMission (dt);
        }
    } 

void LookBack()
{
        fplayer->draw=1;
        float cgamma = fplayer->gamma - (AngleAheadToBestViewpoint + 50) * COS(fplayer->theta);
        float cphi = fplayer->phi - (AngleAheadToBestViewpoint + 50) * SIN(fplayer->theta);
        float fac = fplayer->zoom/5; 
        camx = fplayer->tl->x - COS(cgamma) * SIN(cphi) * (fac/PilotPositionFactor) * 0.5;
        camy = fplayer->tl->y + SIN(cgamma) * (fac/PilotPositionFactor) * 0.5;
        camz = fplayer->tl->z - COS(cgamma) * COS(cphi) * (fac/PilotPositionFactor) * 0.5;
        if (SkewViewBack)
            {
            
            SkewViewBack--;
            if (SkewViewBack == 19)
                {
                camphi = fplayer->phi -190;
                }
            else if (SkewViewBack == 18)
                {
                camphi = fplayer->phi -180;
                }
            else if (SkewViewBack == 17)
                {
                camphi = fplayer->phi -170;
                }
            else if (SkewViewBack == 16)
                {
                camphi = fplayer->phi -160;
                }
            else if (SkewViewBack == 15)
                {
                camphi = fplayer->phi -150;
                }
            else if (SkewViewBack == 14)
                {
                camphi = fplayer->phi -140;
                }
            else if (SkewViewBack == 13)
                {
                camphi = fplayer->phi -130;
                }
            else if (SkewViewBack == 12)
                {
                camphi = fplayer->phi -120;
                }
            else if (SkewViewBack == 11)
                {
                camphi = fplayer->phi -100;
                }
            else if (SkewViewBack == 10)
                {
                camphi = fplayer->phi -90;
                }
            else if (SkewViewBack == 9)
                {
                camphi = fplayer->phi -80;
                }
            else if (SkewViewBack == 8)
                {
                camphi = fplayer->phi -70;
                }
            else if (SkewViewBack == 7)
                {
                camphi = fplayer->phi -60;
                }
            else if (SkewViewBack == 6)
                {
                camphi = fplayer->phi -50;
                }
            else if (SkewViewBack == 5)
                {
                camphi = fplayer->phi -40;
                }
            else if (SkewViewBack == 4)
                {
                camphi = fplayer->phi -30;
                }
            else if (SkewViewBack == 3)
                {
                camphi = fplayer->phi -20;
                }
            else if (SkewViewBack == 2)
                {
                camphi = fplayer->phi -10;
                }
            }
        else
            {
            
            camphi = fplayer->phi; 
            }
        if (camphi > 360)
            {
            camphi -=360;
            }
        else if (camphi < -360)
            {
            camphi +=360;
            }
        camgamma = -fplayer->gamma; 
        camtheta = -fplayer->theta + 180; 
        if(camtheta >= 180)
            {
            camtheta-=360;
            }
    } 

void LookChase()
{
        ChaseViewDistance = fplayer->zoom * 1.5; 
        camx = fplayer->tl->x;
        camz = fplayer->tl->z;
        if (!view_x)
           { 
           camx = fplayer->tl->x + ChaseViewDistance * SIN(fplayer->phi);
           camz = fplayer->tl->z + ChaseViewDistance * COS(fplayer->phi);
           }
        else
        { 
           camx = fplayer->tl->x;
           camz = fplayer->tl->z;
        }
        if (!view_y)
           { 
           camy = fplayer->tl->y + fplayer->zoom * 0.25; 
           }
        else if (view_y < 0)
           { 
           camy = fplayer->tl->y - (ChaseViewDistance * 1.5);
           }
        else if (view_y > 0)
           { 
           camy = fplayer->tl->y + (ChaseViewDistance * 2);
           }
        camphi = fplayer->phi;
        fplayer->draw = 1;
        camgamma = 5; 
        camtheta = fplayer->theta; 
} 

void LookDown()
{
        camx = fplayer->tl->x;
        camy = fplayer->tl->y;
        camz = fplayer->tl->z;
        camphi = fplayer->phi;         
        camtheta = fplayer->theta;     
        camgamma = -fplayer->gamma-90; 
} 

void LookForward ()
   {
   
   SkewViewBack = 20;
   SkewViewLeft = 0;
   SkewViewRight = 0;
   SkewViewUp = 0;
   CamPhiTemp2 = 0;
   PriorThetaNormalized = ThetaNormalized;
   ThetaDelta = PriorThetaNormalized - ThetaNormalized;
   ThetaTemp3 = 0;
   PriorGamma = 0;
   GammaDelta = 0;
   if (fabs(ThetaDelta) > 5 )
      { 
      ThetaDelta = 5;
      }
   if (fabs(ThetaDelta) < 1.0 )
      { 
      ThetaDelta = 0;
      }
   
   float cgamma = fplayer->gamma + AngleAheadToBestViewpoint * COS(fplayer->theta);
   float cphi = fplayer->phi + AngleAheadToBestViewpoint * SIN(fplayer->theta);
   float fac = fplayer->zoom; 
   camx = fplayer->tl->x + COS(cgamma) * SIN(cphi) * (fac/PilotPositionFactor);
   camy = fplayer->tl->y - SIN(cgamma) * (fac/PilotPositionFactor);
   camz = fplayer->tl->z + COS(cgamma) * COS(cphi) * (fac/PilotPositionFactor);
   camphi = fplayer->phi;
   camgamma = -fplayer->gamma + 180;
   camtheta = fplayer->theta;
   
   fplayer->draw = 0;
} 

void LookLeft ()
   {
   
   SkewViewLeft++;
   if (SkewViewLeft>20)
      {
      SkewViewLeft=20;
      }
   
   float CamGammaTemp1 = 0; 
   float CamGammaTemp2 = 0; 
   float CamPhiTemp1   = 0; 
   float CamThetaTemp1 = 0; 
   float CamThetaTemp2 = 0; 
   
   float cgamma = fplayer->gamma - (AngleAheadToBestViewpoint + 50) * COS(fplayer->theta);
   float cphi = fplayer->phi - (AngleAheadToBestViewpoint + 50) * SIN(fplayer->theta);
   float fac = fplayer->zoom/5; 
   camx = fplayer->tl->x - COS(cgamma) * SIN(cphi) * (fac/PilotPositionFactor) * 0.5;
   camy = fplayer->tl->y + SIN(cgamma) * (fac/PilotPositionFactor) * 0.5;
   camz = fplayer->tl->z - COS(cgamma) * COS(cphi) * (fac/PilotPositionFactor) * 0.5;
   
   float DegreesToRadiansFactor = 0.01745333;
   float FlyingFlatComponent = cosf(((fplayer->gamma) * DegreesToRadiansFactor));
   FlyingFlatComponent = -(FlyingFlatComponent);
   float FlyingVerticalComponent = sinf(((fplayer->gamma) * DegreesToRadiansFactor));
   FlyingVerticalComponent = -(FlyingVerticalComponent);
   
   ThetaNormalized = fplayer->theta+180;
   if (ThetaNormalized > 360)
      {
      ThetaNormalized -= 360;
      }
   if (ThetaNormalized < 0)
      {
      ThetaNormalized += 360;
      }
   
   float RollDeviationFromHorizontal = 180-ThetaNormalized;
   
   if (!FlightIsInverted)
      { 
      CamGammaTemp1  =  -(RollDeviationFromHorizontal * FlyingFlatComponent) * (float)(SkewViewLeft/20.0);
      }
   else
      { 
      
      CamGammaTemp1  =  -((RollDeviationFromHorizontal-180) * FlyingFlatComponent) * (float) (SkewViewLeft/20.0);
      if (CamGammaTemp1 > 360)
         {
         CamGammaTemp1 -= 360;
         }
      if (CamGammaTemp1 < 0)
         {
         CamGammaTemp1 += 360;
         }
      }
   
   CamGammaTemp2 = 0; 
   
   if (!FlightIsInverted)
      { 
      CamPhiTemp1 =  (fplayer->phi) + 90 * (float)SkewViewLeft/20.0;
      if (CamPhiTemp1>360)
         {
         CamPhiTemp1-=360;
         }
      if (CamPhiTemp1<0)
         {
         CamPhiTemp1+=360;
         }
      } 
   else
      { 
      CamPhiTemp1 = (fplayer->phi) - 90 * (float)SkewViewLeft/20.0;
      if (CamPhiTemp1 > 360)
         {
         CamPhiTemp1 -=360;
         }
      if (CamPhiTemp1<0)
         {
         CamPhiTemp1 +=360;
         }
      }
   
   float ThetaDelta = PriorThetaNormalized - ThetaNormalized;
   
   if (fabs(ThetaDelta > 5))
     { 
     ThetaDelta=5;
     }
   if (fabs(ThetaDelta) < 1.0 )
     { 
     ThetaDelta = 0;
     }
   if (!FlightIsInverted)
     { 
     CamPhiTemp2 += (ThetaDelta * FlyingVerticalComponent); 
     
     CamPhiTemp2 *= (float)SkewViewLeft/20.0;
     if (CamPhiTemp2 > 360)
        {
        CamPhiTemp2 -=360;
        }
     if (CamPhiTemp2<0)
        {
        CamPhiTemp2 +=360;
        }
     }
   if (FlightIsInverted)
     { 
     CamPhiTemp2 += (ThetaDelta * FlyingVerticalComponent); 
     
     CamPhiTemp2 *= (float)SkewViewLeft/20.0;
     if (CamPhiTemp2 > 360)
        {
        CamPhiTemp2 -=360;
        }
     if (CamPhiTemp2<0)
        {
        CamPhiTemp2 +=360;
        }
     }
   
   if (FlightIsInverted)
      { 
      CamThetaTemp1 = -(fplayer->gamma) + 180; 
      if (CamThetaTemp1 >360)
         {
         CamThetaTemp1-=360;
         }
      if (CamThetaTemp1<0)
         {
         CamThetaTemp1+=360;
         }
      }
   else
      { 
      CamThetaTemp1 = (fplayer->gamma) + 180; 
      if (CamThetaTemp1 >360)
         {
         CamThetaTemp1-=360;
         }
      if (CamThetaTemp1<0)
         {
         CamThetaTemp1+=360;
         }
      }
   
   camgamma = CamGammaTemp1 + CamGammaTemp2; 
   camphi = CamPhiTemp1 + CamPhiTemp2;       
   camtheta = CamThetaTemp1 + CamThetaTemp2; 
   
   PriorThetaNormalized=ThetaNormalized;
   camtheta -=11;
   fplayer->draw=1; 
   } 

void LookMap ()
{ 
gl->enableFog (254.0); 
camx = fplayer->tl->x - (float) MapEastWestScroll;
camy = 160.0 + (float)MapZoomFactor; 
camz = fplayer->tl->z - (float) MapNorthSouthScroll;
camphi = 0;
fplayer->draw = 1;
camgamma = 90;
camtheta = 140.0 + (float)mission->heading; 
} 

void LookMissile (float cf)
{
cf = missile [0]->zoom * 10;
camx = missile [0]->tl->x + cf * SIN(missile [0]->phi);
camy = missile [0]->tl->y + fplayer->zoom * 2;
camz = missile [0]->tl->z + cf * COS(missile [0]->phi);
camphi = missile [0]->phi;
camtheta = fplayer->theta;
fplayer->draw = 1;
} 

void LookRight ()
{
        
        SkewViewRight++;
        if (SkewViewRight>20)
           {
           SkewViewRight=20;
           }
        
        float CamGammaTemp1 = 0; 
        float CamGammaTemp2 = 0; 
        float CamPhiTemp1   = 0; 
        float CamThetaTemp1 = 0; 
        float CamThetaTemp2 = 0; 
        
        float cgamma = fplayer->gamma - (AngleAheadToBestViewpoint + 50) * COS(fplayer->theta);
        float cphi = fplayer->phi - (AngleAheadToBestViewpoint + 50) * SIN(fplayer->theta);
        float fac = fplayer->zoom/5; 
        camx = fplayer->tl->x - COS(cgamma) * SIN(cphi) * (fac/PilotPositionFactor) * 0.5;
        camy = fplayer->tl->y + SIN(cgamma) * (fac/PilotPositionFactor) * 0.5;
        camz = fplayer->tl->z - COS(cgamma) * COS(cphi) * (fac/PilotPositionFactor) * 0.5;
        
        float DegreesToRadiansFactor = 0.01745333;
        float FlyingFlatComponent = cosf(((fplayer->gamma) * DegreesToRadiansFactor));
        FlyingFlatComponent = -(FlyingFlatComponent);
        float FlyingVerticalComponent = sinf(((fplayer->gamma) * DegreesToRadiansFactor));
        FlyingVerticalComponent = -(FlyingVerticalComponent);
        
        ThetaNormalized = fplayer->theta+180;
        if (ThetaNormalized > 360)
           {
           ThetaNormalized -= 360;
           }
        if (ThetaNormalized < 0)
           {
           ThetaNormalized += 360;
           }
        
         float RollDeviationFromHorizontal = 180-ThetaNormalized;
        
        if (!FlightIsInverted)
           {
           
           CamGammaTemp1  =  (RollDeviationFromHorizontal * FlyingFlatComponent) * (float)(SkewViewRight/20.0);
           }
        else
           { 
           
           CamGammaTemp1  =  ((RollDeviationFromHorizontal+180) * FlyingFlatComponent) * (float) (SkewViewRight/20.0);
           }
        if (CamGammaTemp1 > 360)
           {
           CamGammaTemp1 -= 360;
           }
        if (CamGammaTemp1 < 0)
           {
           CamGammaTemp1 += 360;
           }
        
        CamGammaTemp2 = 0; 
        
        if (!FlightIsInverted)
           { 
           CamPhiTemp1 =  (fplayer->phi) - 90 * (float)SkewViewRight/20.0;
           if (CamPhiTemp1>360)
              {
              CamPhiTemp1-=360;
              }
           if (CamPhiTemp1<0)
              {
              CamPhiTemp1+=360;
              }
           } 
        else
           { 
           CamPhiTemp1 = (fplayer->phi) + 90 * (float)SkewViewRight/20.0;
           if (CamPhiTemp1 > 360)
              {
              CamPhiTemp1 -=360;
              }
           if (CamPhiTemp1<0)
              {
              CamPhiTemp1 +=360;
              }
           }
        
        float ThetaDelta = PriorThetaNormalized - ThetaNormalized;
        
        if (fabs(ThetaDelta > 5))
          { 
          ThetaDelta=5;
          }
        if (fabs(ThetaDelta) < 1.0 )
          { 
          ThetaDelta = 0;
          }
        if (!FlightIsInverted)
          { 
          CamPhiTemp2 += (ThetaDelta * FlyingVerticalComponent); 
          
          CamPhiTemp2 *= (float)SkewViewRight/20.0;
          if (CamPhiTemp2 > 360)
             {
             CamPhiTemp2 -=360;
             }
          if (CamPhiTemp2<0)
             {
             CamPhiTemp2 +=360;
             }
          }
        if (FlightIsInverted)
          { 
          CamPhiTemp2 += (ThetaDelta * FlyingVerticalComponent); 
          
          CamPhiTemp2 *= (float)SkewViewRight/20.0;
          if (CamPhiTemp2 > 360)
             {
             CamPhiTemp2 -=360;
             }
          if (CamPhiTemp2<0)
             {
             CamPhiTemp2 +=360;
             }
          }
        
        if (FlightIsInverted)
           { 
           CamThetaTemp1 = (fplayer->gamma) + 180; 
           if (CamThetaTemp1 >360)
              {
              CamThetaTemp1-=360;
              }
           if (CamThetaTemp1<0)
              {
              CamThetaTemp1+=360;
              }
           }
        else
           { 
           CamThetaTemp1 = -(fplayer->gamma) + 180; 
           if (CamThetaTemp1 >360)
              {
              CamThetaTemp1-=360;
              }
           if (CamThetaTemp1<0)
              {
              CamThetaTemp1+=360;
              }
           }
        
        camgamma = CamGammaTemp1 + CamGammaTemp2; 
        camphi = CamPhiTemp1 + CamPhiTemp2;       
        camtheta = CamThetaTemp1 + CamThetaTemp2; 
        
        PriorThetaNormalized=ThetaNormalized;
        camtheta +=11;
        fplayer->draw=1; 
    } 

void LookUp ()
{
        
        SkewViewUp++;
        if (SkewViewUp>20)
            {
            SkewViewUp = 20;
            }
        
        camx = fplayer->tl->x;
        camy = fplayer->tl->y;
        camz = fplayer->tl->z;
        camphi = fplayer->phi; 
        
        float DegreesToRadiansFactor = 0.01745333;
        float FlyingFlatComponent = cosf(((fplayer->theta) * DegreesToRadiansFactor));
        float FlyingSidewaysComponent = sinf(((fplayer->theta) * DegreesToRadiansFactor));
        float FlyingVerticalComponent = sinf(((fplayer->gamma) * DegreesToRadiansFactor));
        float ThetaTemp1 = 0;
        float ThetaTemp2 = 0;
        
        GammaDelta = PriorGamma - fplayer->gamma;
        if (fabs(GammaDelta) > 3)
        { 
            GammaDelta = 0;
        }
        ThetaTemp1 = -fplayer->theta; 
        ThetaTemp2 = -(90 - fabs(fplayer->theta)) * FlyingSidewaysComponent;
        ThetaTemp3 += (GammaDelta * FlyingSidewaysComponent);

        camtheta = ThetaTemp1 + ThetaTemp2 + ThetaTemp3;
        if (SkewViewUp == 0)
         {
         camgamma = 180-fplayer->gamma + 177 * (FlyingFlatComponent);
         camgamma += fplayer->gamma - 177 * -(FlyingVerticalComponent);
         if (camgamma > 360)
             {
             camgamma -= 360;
             }
         if (camgamma < 0)
             {
             camgamma += 360;
             }
         camphi = (fplayer->phi - (177 * FlyingSidewaysComponent));
         if (camphi >360)
             {
             camphi -=360;
             }
         if (camphi <0)
             {
             camphi +=360;
             }
         }
        if (SkewViewUp == 1)
         {
         camgamma = 180-fplayer->gamma + 175 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 175 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 175 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (175 * FlyingSidewaysComponent));
            if (camphi >360)
              {
              camphi -=360;
              }
            if (camphi <0)
              {
              camphi +=360;
              }
         }
      if (SkewViewUp == 2)
       {
       camgamma = 180-fplayer->gamma + 172 * (FlyingFlatComponent);
       if (fabs(FlyingSidewaysComponent) < 0.5)
          {
          camgamma += fplayer->gamma - 172 * -(FlyingVerticalComponent);
          }
       else
          {
          camgamma += fplayer->gamma - 172 * (FlyingVerticalComponent);
          }
       if (camgamma > 360)
          {
          camgamma -= 360;
          }
       if (camgamma < 0)
          {
          camgamma += 360;
          }
       camphi = (fplayer->phi - (172 * FlyingSidewaysComponent));
       if (camphi >360)
          {
           camphi -=360;
          }
       if (camphi <0)
          {
          camphi +=360;
          }
       }
     if (SkewViewUp == 3)
       {
       camgamma = 180-fplayer->gamma + 171 * (FlyingFlatComponent);
       if (fabs(FlyingSidewaysComponent) < 0.5)
          {
          camgamma += fplayer->gamma - 171 * -(FlyingVerticalComponent);
          }
       else
          {
          camgamma += fplayer->gamma - 171 * (FlyingVerticalComponent);
          }
       if (camgamma > 360)
          {
          camgamma -= 360;
          }
       if (camgamma < 0)
          {
          camgamma += 360;
          }
       camphi = (fplayer->phi - (171 * FlyingSidewaysComponent));
       if (camphi >360)
           {
           camphi -=360;
           }
       if (camphi <0)
           {
           camphi +=360;
           }
       }
     if (SkewViewUp == 4)
       {
       camgamma = 180-fplayer->gamma + 170 * (FlyingFlatComponent);
       if (fabs(FlyingSidewaysComponent) < 0.5)
           {
           camgamma += fplayer->gamma - 170 * -(FlyingVerticalComponent);
           }
       else
           {
           camgamma += fplayer->gamma - 170 * (FlyingVerticalComponent);
           }
       if (camgamma > 360)
           {
           camgamma -= 360;
           }
       if (camgamma < 0)
           {
           camgamma += 360;
           }
       camphi = (fplayer->phi - (170 * FlyingSidewaysComponent));
       if (camphi >360)
           {
           camphi -=360;
           }
       if (camphi <0)
           {
           camphi +=360;
           }
       }
     if (SkewViewUp == 5)
       {
       camgamma = 180-fplayer->gamma + 167 * (FlyingFlatComponent);
       if (fabs(FlyingSidewaysComponent) < 0.5)
          {
          camgamma += fplayer->gamma - 167 * -(FlyingVerticalComponent);
          }
       else
          {
          camgamma += fplayer->gamma - 167 * (FlyingVerticalComponent);
          }
       if (camgamma > 360)
          {
          camgamma -= 360;
          }
       if (camgamma < 0)
          {
          camgamma += 360;
          }
       camphi = (fplayer->phi - (167 * FlyingSidewaysComponent));
          if (camphi >360)
            {
            camphi -=360;
            }
          if (camphi <0)
            {
            camphi +=360;
            }
         }
       if (SkewViewUp == 6)
         {
         camgamma = 180-fplayer->gamma + 163 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 163 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 163 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (163 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 7)
         {
         camgamma = 180-fplayer->gamma + 156 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 156 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 156 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (156 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 8)
         {
         camgamma = 180-fplayer->gamma  + 147 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 147 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 147 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (147 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 9)
         {
         camgamma = 180-fplayer->gamma + 137 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 137 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 137 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (137 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 10)
         {
         camgamma = 180-fplayer->gamma + 128 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 128 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 128 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (128 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 11)
         {
         camgamma = 180-fplayer->gamma + 120 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
             {
             camgamma += fplayer->gamma - 120 * -(FlyingVerticalComponent);
             }
         else
             {
             camgamma += fplayer->gamma - 120 * (FlyingVerticalComponent);
             }
         if (camgamma > 360)
             {
             camgamma -= 360;
             }
         if (camgamma < 0)
             {
             camgamma += 360;
             }
         camphi = (fplayer->phi - (120 * FlyingSidewaysComponent));
         if (camphi >360)
             {
             camphi -=360;
             }
         if (camphi <0)
             {
             camphi +=360;
             }
         }
       if (SkewViewUp == 12)
         {
         camgamma = 180-fplayer->gamma + 113 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 113 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 113 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (113 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
       if (SkewViewUp == 13)
         {
         camgamma = 180-fplayer->gamma + 107 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
             {
             camgamma += fplayer->gamma - 107 * -(FlyingVerticalComponent);
             }
         else
             {
             camgamma += fplayer->gamma - 107 * (FlyingVerticalComponent);
             }
         if (camgamma > 360)
             {
             camgamma -= 360;
             }
         if (camgamma < 0)
             {
             camgamma += 360;
             }
         camphi = (fplayer->phi - (107 * FlyingSidewaysComponent));
         if (camphi >360)
             {
             camphi -=360;
             }
         if (camphi <0)
             {
             camphi +=360;
             }
         }
       if (SkewViewUp == 14)
         {
         camgamma = 180-fplayer->gamma + 102 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 102 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 102 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (102 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 15)
         {
         camgamma = 180-fplayer->gamma + 98 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 98 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 98 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (98 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 16)
         {
         camgamma = 180-fplayer->gamma + 95 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 95 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 95 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (95 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 17)
         {
         camgamma = 180-fplayer->gamma + 93 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 93 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 93 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (93 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 18)
         {
         camgamma = 180-fplayer->gamma + 92 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 92 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 92 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (92 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         }
      if (SkewViewUp == 19)
         {
         camgamma = 180-fplayer->gamma + 91 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
           {
           camgamma += fplayer->gamma - 91 * -(FlyingVerticalComponent);
           }
         else
           {
           camgamma += fplayer->gamma - 91 * (FlyingVerticalComponent);
           }
         if (camgamma > 360)
           {
           camgamma -= 360;
           }
         if (camgamma < 0)
           {
           camgamma += 360;
           }
         camphi = (fplayer->phi - (91 * FlyingSidewaysComponent));
         if (camphi >360)
           {
           camphi -=360;
           }
         if (camphi <0)
           {
           camphi +=360;
           }
         }
      if (SkewViewUp == 20)
         {
         
         camgamma = 180-fplayer->gamma + 90 * (FlyingFlatComponent);
         if (fabs(FlyingSidewaysComponent) < 0.5)
            {
            camgamma += fplayer->gamma - 90 * -(FlyingVerticalComponent);
            }
         else
            {
            camgamma += fplayer->gamma - 90 * (FlyingVerticalComponent);
            }
         if (camgamma > 360)
            {
            camgamma -= 360;
            }
         if (camgamma < 0)
            {
            camgamma += 360;
            }
         camphi = (fplayer->phi - (90 * FlyingSidewaysComponent));
         if (camphi >360)
            {
            camphi -=360;
            }
         if (camphi <0)
            {
            camphi +=360;
            }
         PriorGamma = fplayer->gamma;
         }
} 

void mission_display ()
    {
    allmenus.setVisible (false);
    MissionMenu.setVisible (true);
    MissionMenu.draw ();
    int i;
    CColor *col = &menu_colwhite;
    Font *font2 = font1;
    CVector3 vec;
    CVector3 tl (-4.5, 5.5, -8.0);
    CRotation rot;
    rot.a = 270;
    rot.b = (270 - MissionMenuTimer * 4 / timestep) % 360;
    rot.c = 90;
    // Draw dummy missile
    glEnable (GL_LIGHTING);
    model_bomb01.draw (&vec, &tl, &rot, 0.05, 1.0, 0);
    glDisable (GL_LIGHTING);
    glEnable (GL_LIGHTING);
    glEnable (GL_DEPTH_TEST);
    tl.x = -0.24;
    tl.y = -0.075;
    tl.z = -0.5;
    rot.a = 300;
    rot.b = 0;
    
    if (missionnew == NULL)
       {
       display ((char*)"mission_display() alert. missionnew is null.", LOG_MOST);
       }
    tl.x = -0.16; 
    rot.c = (5 + MissionMenuTimer * 4 / timestep) % 360;
    ThreeDObjects [0]->zoom = 0.1; //Helps to shrink obscuring components of underlying scene.
    getModel (missionnew->selfighter [0])->draw (&vec, &tl, &rot, 0.04, 0.1, 0);
    tl.x = 0;
    tl.y = -0.075;
    tl.z = -0.5;
    rot.a = 300;
    rot.b = 0;
    for (i = 0; i < missionnew->selweapons; i ++)
        {
        tl.x = 0.08 + (float) i * 0.07;

        if (missionnew->wantweapon == i)
            {
            rot.c = (5 + MissionMenuTimer * 4 / timestep) % 360;
            }
        else
            {
            rot.c = 5;
            }
        //getModel (missionnew->selweapon [i])->draw (&vec, &tl, &rot, 0.04, 0.1, 0);
        }
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_LIGHTING);
    float textx = -12, texty = 9.8;
    font1->zoom = 0.105;
    font2->zoom = 0.105;
    float fontscale = 1.05;
    font1->drawTextCentered (0, texty / fontscale, -2, missionnew->name, col);
    texty -= 1.1;
    font1->zoom = 0.05;
    font2->zoom = 0.05;
    fontscale = 0.5;
    font1->drawText (textx / fontscale, texty / fontscale, -2, (char *)"BRIEFING:", col);
    texty -= 1;
    font1->drawText (textx / fontscale, texty / fontscale, -2, missionnew->briefing, col);
    float xstats = 0.8, ystats = 5;
    float xaircraft = -12;
    float yaircraft = 5;
    font1->drawText (textx / fontscale, -1 / fontscale, -2, (char *)"CURRENT AIRCRAFT:", col);
    font2->drawText (textx / fontscale, -6 / fontscale, -2, getModelName (missionnew->selfighter [missionnew->wantfighter]), col);
    font1->drawText (xstats - 0.25 / fontscale, ystats / fontscale, -2, (char *)"MISSION NOTES:", col);
    if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
        {
        font1->drawText (xstats / fontscale, (ystats -1) / fontscale, -2, (char *)"1 HEAVY BOMBER CAN DESTROY\nAN AIRBASE IN 2 SORTIES,\nBUT WITHOUT A BOMBER,\nMANY FIGHTERS ARE NEEDED.\n\nIF YOU ARE BEING VULCHED,\nUSE A DIFFERENT AIRFIELD.", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    else if (CurrentMissionNumber == MISSION_TUTORIAL)
        {
        font1->drawText (xstats / fontscale, (ystats -1) / fontscale, -2, (char *)"THIS IS A GOOD WAY TO LEARN\nENOUGH TO GET STARTED.", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    else if (CurrentMissionNumber == MISSION_TUTORIAL2)
        {
        font1->drawText (xstats / fontscale, (ystats -1) / fontscale, -2, (char *)"GROUND ATTACK IS BEST DONE\nWITH A BOMBER OR FIGHTER-\nBOMBER, BUT IT CAN ALSO BE\nDONE WITH A FIGHTER AIRCRAFT.", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    else if (CurrentMissionNumber == MISSION_TUTORIAL3)
        {
        font1->drawText (xstats - 0.25 / fontscale, (ystats -1) / fontscale, -2, (char *)"IT WON'T TAKE YOU VERY LONG TO\nLEARN EVERYTHING YOU CAN LEARN\nFROM THIS MISSION. AFTER THAT\nYOU WILL FIND THAT THE 10-PLAYER,\nONLINE, MULTIPLAYER MISSIONS ARE\nA LOT MORE FUN AND MORE\nSOPHISTICATED.", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    else if (CurrentMissionNumber == MISSION_HEADTOHEAD00)
        {
        font1->drawText (xstats - 0.25 / fontscale, (ystats -1) / fontscale, -2, (char *)"YOU CAN FIND THIS MISSION'S SINGLE\nONLINE OPPONENT EITHER BY\nSPECIFYING ITS IP ADDRESS IN YOUR\nLACCONFIG.TXT FILE, OR THROUGH\nLAC SERVER.", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    else
        {
        font1->drawText (xstats / fontscale, (ystats -1) / fontscale, -2, (char *)"NO FURTHER DETAILS", col);
        font1->drawText (xaircraft / fontscale, (yaircraft -1) / fontscale, -2, (char *)"YOU CAN CHOOSE A DIFFERENT\nAIRCRAFT FROM THE PREVIOUS\nMENU IF YOU DON'T LIKE THIS ONE.\n\nCLICK ON 'BACK TO MAIN MENU'\nTHEN CLICK ON\n'DEFAULT AIRCRAFT'.", col);
        }
    if (CurrentMissionNumber == MISSION_TUTORIAL || CurrentMissionNumber == MISSION_TUTORIAL3)
        {
        font1->drawText (xstats / fontscale, -1 / fontscale, -2, (char *)"FOR THIS MISSION, YOUR AIRCRAFT\nIS ONLY ARMED WITH ITS AIR/AIR\nWEAPONS.)", col);
        }
    else if (CurrentMissionNumber == MISSION_TUTORIAL2)
        {
        font1->drawText (xstats - 0.28 / fontscale, -1 / fontscale, -2, (char *)"FOR THIS MISSION, CHOOSE AN\nAIRCRAFT WITH GOOD AIR-TO-\nGROUND WEAPONS. (WE RECOMMEND\nROCKETS, BUT GUNS AND BOMBS\nCAN ALSO BE EFFECTIVE.)", col);
        font2->drawText (xstats / fontscale, -6 / fontscale, -2, getModelName (missionnew->selweapon [missionnew->wantweapon]), col);
        }
    else if (CurrentMissionNumber == MISSION_HEADTOHEAD00)
        {
        font1->drawText (xstats / fontscale, -1 / fontscale, -2, (char *)"FOR THIS MISSION, YOUR AIRCRAFT\nIS FULLY ARMED AND FUELED.\n\nIF THERE IS NO ONLINE OPPONENT,\nYOU WILL FLY AGAINST 9 BOTS,\nJUST ONE AT A TIME.)", col);
        }
    else if (CurrentMissionNumber == MISSION_FREEFLIGHTWW2)
        {
        font1->drawText (xstats / fontscale, -1 / fontscale, -2, (char *)"FOR THIS MISSION, YOUR AIRCRAFT\nIS ONLY ARMED WITH GUNS AND\nAIR/AIR WEAPONS.\n\nTHERE ARE NO AIR-TO-AIR\nTARGETS, BUT YOU CAN STRAFE\nTHE GROUND.\n\nTHIS MISSION IS ALSO GOOD\nFOR PRACTICING LANDINGS\nAND TAKE-OFFS.)", col);
        }
    else
        { 
        font1->drawText (xstats / fontscale, -1 / fontscale, -2, (char *)"SELECT STARTING AIRFIELD\n(CLICK AIRFIELD NAME BELOW):", col);
        if (AirfieldRequested == 1)
           {
           if (MyNetworkId%2)
              { 
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"NEAR THE ENEMY (RADAR BLIP 29): \nRED HQ AIRFIELD \n(BOMBERS 50% LOAD)", col);
              }
           else
              { // Player is on BlueTeam
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"NEAR THE ENEMY (RADAR BLIP 28): \nBLUE HQ AIRFIELD \n(BOMBERS 50% LOAD)", col);
              }
           }
        else if (AirfieldRequested == 2)
           {
           if (MyNetworkId%2)
              { 
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"FARTHER OUT: \nRED AIRFIELD 2 (ADJACENT)\n(BOMBERS 75% LOAD)", col);
              }
           else
              { 
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"FARTHER OUT: \nBLUE AIRFIELD 2 (ADJACENT)\n(BOMBERS 75% LOAD)", col);
              }
           }
        else if (AirfieldRequested == 3)
           {
           if (MyNetworkId%2)
              { 
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"EVEN FARTHER OUT: \nRED AIRFIELD 3 (FIGHTERS)\nOR RED AIRFIELD 4 (BOMBERS)\n(BOMBERS 100% LOAD)", col);
              }
           else
              { 
              font1->drawText (xstats / fontscale, -4 / fontscale, -2, (char *)"EVEN FARTHER OUT: \nBLUE AIRFIELD 3 (FIGHTERS)\nOR BLUE AIRFIELD 4 (BOMBERS)\n(BOMBERS 100% LOAD)", col);
              }
           }
        }
    font1->zoom = 0.1;
    font2->zoom = 0.1;
    drawMouseCursor ();
    } 

void MouseMission (int button, int state, int x, int y)
    {
    float rx = (float) x / width;
    float ry = (float) y / height;
    if (ry >= 0.53 && ry <= 0.65)
        { 
        if (rx >= 0.18 && rx < 0.28)
            { 
            if (state == MOUSE_DOWN)
                {
                display ((char*)"MouseMission() area 1 click detected", LOG_MOST);
                missionnew->wantfighter = 0;
                }
            }
        if (rx >= 0.35 && rx < 0.48)
            { 
            
            if (state == MOUSE_DOWN)
                { 
                display ((char*)"MouseMission() area 2 click detected", LOG_MOST);
                }
            }
        }
    if (ry >= 0.53 && ry <= 0.65)
        {
        if (rx >= 0.54 && rx < 0.64)
            { 
            if (state == MOUSE_DOWN)
                {
                display ((char*)"MouseMission() area 3 click detected", LOG_MOST);
                if (CurrentMissionNumber == MISSION_NETWORKBATTLE01 || CurrentMissionNumber == MISSION_NETWORKBATTLE02 || CurrentMissionNumber == MISSION_NETWORKBATTLE03)
                   {
                   display ((char*)"MouseMission() AirfieldSelectCommand detected", LOG_MOST);
                   AirfieldRequested ++;
                   if (AirfieldRequested >=4)
                      {
                      AirfieldRequested = 1;
                      }
                   display ((char*)"MouseMission() AirfieldRequested =", LOG_MOST);
                   sprintf (DebugBuf, "%d", AirfieldRequested);
                   display (DebugBuf, LOG_MOST);
                   }
                else
                   {
                   missionnew->wantweapon = 0;
                   }
                }
            }
        if (rx >= 0.64 && rx < 0.74 && missionnew->selweapons >= 2)
            {
            if (state == MOUSE_DOWN)
                {
                display ((char*)"MouseMission() area 4 click detected", LOG_MOST);
                missionnew->wantweapon = 1;
                }
            }
        if (rx >= 0.74 && rx < 0.84 && missionnew->selweapons >= 3)
            {
            if (state == MOUSE_DOWN)
                {
                display ((char*)"MouseMission() area 5 click detected", LOG_MOST);
                missionnew->wantweapon = 2;
                }
            }
        }
    } 

void MouseQuit (int button, int state, int x, int y)
    {
    float rx = (float) x / width;
    float ry = (float) y / height;
    if (ry >= 0.5 && ry <= 0.55)
        {
        if (rx >= 0.35 && rx <= 0.45)
            {
            if (state == MOUSE_DOWN)
                {
                KeyQuit ('y', 0, 0);
                }
            }
        if (rx >= 0.55 && rx <= 0.65)
            {
            if (state == MOUSE_DOWN)
                {
                KeyQuit (27, 0, 0);
                }
            }
        }
    } 

void pause_display ()
    {
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glPushMatrix ();
    font1->drawTextCentered (0, 4, -2, (char *)"PAUSED. PRESS ESCAPE.", &colorwhite);
    glPopMatrix ();
    } 

void playRandomMusic ()
    {
    int r = myrandom (4);
    if (r == 0)
        {
        display ((char*)"playRandomMusic() MUSIC_MUSIC00", LOG_MOST);
        sound->loadMusic (MUSIC_MUSIC01);
        }
    else if (r == 1)
        {
        display ((char*)"playRandomMusic() MUSIC_MUSIC01", LOG_MOST);
        sound->loadMusic (MUSIC_MUSIC01);
        }
    else if (r == 2)
        {
        display ((char*)"playRandomMusic() MUSIC_MUSIC02", LOG_MOST);
        sound->loadMusic (MUSIC_MUSIC02);
        }
    else if (r == 3)
        {
        display ((char*)"playRandomMusic() MUSIC_MUSIC03", LOG_MOST);
        sound->loadMusic (MUSIC_MUSIC03);
        }
    sound->playMusic (1);
    //Mix_HookMusicFinished (playRandomMusic); 
    } 

void pleaseWait ()
    {
    float xf = 1.0, yf = 0.2, zf = -2.5;
    int c1 = 100, c2 = 10, c3 = 180;
    glBegin (GL_QUADS);
    glColor4ub (c2, c2, c2, 255);
    glVertex3f (-xf, -yf, zf);
    glColor4ub (c1, c1, c1, 255);
    glVertex3f (xf, -yf, zf);
    glColor4ub (c2, c2, c2, 255);
    glVertex3f (xf, yf, zf);
    glColor4ub (c1, c1, c1, 255);
    glVertex3f (-xf, yf, zf);
    glEnd ();
    glLineWidth (LINEWIDTH(1));
    glBegin (GL_LINE_STRIP);
    glColor4ub (c3, c3, c3, 255);
    glVertex3f (-xf, -yf, zf);
    glVertex3f (xf, -yf, zf);
    glVertex3f (xf, yf, zf);
    glVertex3f (-xf, yf, zf);
    glVertex3f (-xf, -yf, zf);
    glEnd ();
    font1->drawTextCentered (0, -0.5, -1.5, (char *)"PLEASE WAIT...", &colorwhite);
    game_view ();
    } 

void proceedFire ()
    {
    int i, i2;
    for (i = maxfy - 2; i >= 1; i --)
        for (i2 = 2; i2 < maxfx - 2; i2 ++)
            {
            heat2 [i] [i2] = 0 * heat [i + 1] [i2 - 2] + 4 * heat [i + 1] [i2 - 1] + 16 * heat [i + 1] [i2] + 4 * heat [i + 1] [i2 + 1] + 0 * heat [i + 1] [i2 + 2]; // heat diffusion
            heat2 [i] [i2] += 0 * heat [i] [i2 - 2] + 4 * heat [i] [i2 - 1] + 16 * heat [i] [i2] + 4 * heat [i] [i2 + 1] + 0 * heat [i] [i2 + 2]; // heat diffusion
            heat2 [i] [i2] /= 48;
            heat2 [i] [i2] -= (int) (300.0F / maxfy); // heat sink
            if (heat2 [i] [i2] < 0)
                {
                heat2 [i] [i2] = 0;
                }
            }
    memcpy (heat, heat2, maxfx * maxfy * sizeof (int)); // copy back buffer to heat array
    for (i = 0; i < maxfy; i ++)
        {
        for (i2 = 0; i2 < maxfx; i2 ++)
            {
            // rotate through fire colors (white-yellow-red-black-blue-black)
            // col in [0...512]
            int yind = i;
            int h = heat [yind] [i2];
            int b = h * 5;
            if (h > 30)
                {
                b = (60 - h) * 5;
                }
            if (h >= 60)
                {
                b = 0;
                }
            h -= 50;
            int r = h * 2; // blend out late for red->black
            if (r > 255)
                {
                r = 255;
                }
            else if (r < 0)
                {
                r = 0;
                }
            h -= 127;
            int g = h * 2; // blend out for yellow->red
            if (g > 255)
                {
                g = 255;
                }
            else if (g < 0)
                {
                g = 0;
                }
            h -= 127;
            if (h > 0)
                {
                b = h - 256; // blend out early to get white->yellow
                if (b > 255)
                    {
                    b = 255;
                    }
                }
            else if (b < 0)
                {
                b = 0;
                }
            int a = r >= b ? r : b; // alpha value: transparent after yellow-red phase
            glColor4ub (r, g, b, a);
            firetex [(i * maxfx + i2) * 4] = r;
            firetex [(i * maxfx + i2) * 4 + 1] = g;
            firetex [(i * maxfx + i2) * 4 + 2] = b;
            firetex [(i * maxfx + i2) * 4 + 3] = a;
            }
        glEnd ();
        }
    glPopMatrix ();
    glBindTexture (GL_TEXTURE_2D, 5000);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, 4, maxfx, maxfy, 0, GL_RGBA, GL_UNSIGNED_BYTE, firetex);
    } 

void ReshapeCredits ()
    {
    glViewport(0, 0, (GLint) width, (GLint) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    float v = getView ();
    if (camera == 50)
        {
        v = 100000.0;
        }
    gluPerspective (VisibleAngle, 1.0, nearclippingplane * GLOBALSCALE, v * GLOBALSCALE);
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);
    SDL_ShowCursor (0);
    } 

void ReshapeGame ()
    {
    glViewport(0, 0, (GLint) width, (GLint) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    float v = getView ();
    if (camera == 50)
        {
        v = 100000.0;
        }
    
    nearclippingplane = fplayer->ClipDistance;
    gluPerspective (VisibleAngle, (float) width / height, nearclippingplane * GLOBALSCALE, v * GLOBALSCALE);
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);
    SDL_ShowCursor (0);
    } 

void ReshapeMenu ()
    {
    sound->stop (SOUND_WINDNOISE);
    WindNoiseOn = false;
    glViewport(0, 0, (GLint) width, (GLint) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    float v = getView ();
    if (camera == 50)
        {
        v = 100000.0;
        }
    gluPerspective (VisibleAngle, 1.0, nearclippingplane * GLOBALSCALE, v * GLOBALSCALE);
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);
    SDL_ShowCursor (0);
    } 

void ReshapeStats ()
    {
    glViewport(0, 0, (GLint) width, (GLint) height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    float v = getView ();
    if (camera == 50)
        {
        v = 100000.0;
        }
    gluPerspective (VisibleAngle, 1.0, nearclippingplane * GLOBALSCALE, v * GLOBALSCALE);
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);
    SDL_ShowCursor (0);
    } 

void sdlMainLoop ()
    {
    // int sym = 0;
    SDL_Event event;
    display ((char*)"Function Entry: sdlMainLoop()", LOG_MOST);
    while (true)
        {
        
        CountdownTimer--;
        if (CountdownTimer<0)
            {
            
            CountdownTimer=20; 
            
            Turbulence = ((float) myrandom (10000)) * 0.0000025 * TurbulenceFactor; 
            Turbulence -= TurbulenceFactor * 0.0125; 
            TargetVocalizeProcess1();
            TargetVocalizeProcess2();
            }
        while (SDL_PollEvent (&event))   
            {
            switch (event.type)
                {
                case SDL_MOUSEMOTION:
                    {
                    LacPassiveMotionFunc (event.motion.x, event.motion.y);
                    if (game == GAME_PLAY && controls == CONTROLS_MOUSE && mouse_relative)
                        {
                        fplayer->rolleffect = 0;
                        fplayer->elevatoreffect = 0;
                        fplayer->ruddereffect = 0;
                        }
                    LacPassiveRelMotionFunc (event.motion.xrel, event.motion.yrel);
                    break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    {
                    LacMouseFunc (event.button.button, event.button.state, event.button.x, event.button.y);
                    break;
                    }
                case SDL_KEYDOWN:
                    {
//  Note: there is no more unicode in SDL2, use one KeyboardFunc
#ifdef NOTDEF
                    if (!event.key.keysym.unicode)
                        {
                        LacSpecialFunc (event.key.keysym.sym, 0, 0);
                        }
                    else
                        {
                        LacKeyboardFunc (event.key.keysym.sym, 0, 0);
                        }
#endif /* NOTDEF */
                    LacKeyboardFunc(event.key.keysym.sym, 0, 0);

                    break;
                    }
                case SDL_KEYUP:
                    {
#ifdef NOTDEF
                    sym = event.key.keysym.sym;
                    if (sym == 8 || sym == 9 || sym == 13 || (sym >= 32 && sym <= 'y'))
                        {
                        LacKeyboardFuncUp (event.key.keysym.sym, 0, 0);
                        }
                    else
                        {
                        LacSpecialFuncUp (event.key.keysym.sym, 0, 0);
                        }
#endif /* NOTDEF */
                    LacKeyboardFuncUp (event.key.keysym.sym, 0, 0);
                    break;
                    }
                case SDL_JOYAXISMOTION:
                    {
                    if (abs (event.jaxis.value) < 2000)
                        {
                        jaxis [event.jaxis.axis + event.jaxis.which * 10] = 0;
                        }
                    else
                        {
                        if (event.jaxis.value < 0)
                            {
                            event.jaxis.value += 2500;
                            }
                        else
                            {
                            event.jaxis.value -= 2500;
                            }

                        jaxis [event.jaxis.axis + event.jaxis.which * 10] = (int) event.jaxis.value * 32768 / 30268;
                        }
                    break;
                    }
                case SDL_JOYBUTTONDOWN:
                    {
                    JoystickButton = event.jbutton.button + event.jbutton.which * 1000;
                    if (JoystickButton == (int) joystick_FIREPRIMARY)
                        {
                        JoystickFireButton = true;
                        }
                    LacJoystickButtonFunc (JoystickButton);
                    JoystickButton = -1;
                    break;
                    }
                case SDL_JOYBUTTONUP:
                    {
                    JoystickButton = event.jbutton.button + event.jbutton.which * 1000;
                    if (JoystickButton == (int) joystick_FIREPRIMARY)
                        {
                        JoystickFireButton = false;
                        }
                    JoystickButton = -1;
                    break;
                    }
                case SDL_JOYHATMOTION:
                    {
                    LacJoystickHatFunc (event.jhat.value + event.jhat.which * 1000);
                    break;
                    }
// TODO: does this cover all events now?
//                case SDL_ACTIVEEVENT:
                case SDL_WINDOWEVENT:
                    {
                    sdlreshape = true;
                    sdldisplay = true;
                    break;
                    }
                }
            }
        if (controls == CONTROLS_JOYSTICK)
            {
            int x = jaxis [getJoystickAxisIndex (joystick_aileron)];
            int y = jaxis [getJoystickAxisIndex (joystick_elevator)];
            int rudder = jaxis [getJoystickAxisIndex (joystick_rudder)];
            int throttle = jaxis [getJoystickAxisIndex (joystick_throttle)];
            int JoyHatX = jaxis [getJoystickAxisIndex (joystick_view_x)];
            int JoyHatY = jaxis [getJoystickAxisIndex (joystick_view_y)];
            LacJoystickAxisFunc (x, y, rudder, throttle, JoyHatX, JoyHatY);
            if (JoystickFireButton)
                {
                LacJoystickButtonFunc (joystick_FIREPRIMARY);
                }
            }
        if (game == GAME_PLAY)
            { 
            SkipCount++;
            if (SkipCount>=11)
                {
                SkipCount=0;
                }
            ZeroSpeedCorrection();
            CompressibilityTurbulenceEffects();
            SpeedBrakeEffects();
            CompressibilityEffects();
            StallEffects();
            DragEffects();
            }
        if (sdldisplay)
            {
            LacDisplayFunc ();
            }
        sdldisplay = false;
        if (sdlreshape)
            {
            LacReshapeFunc (width, height);
            }
        sdlreshape = false;
        LacTimerFunc (1); // dummy value
        }
    } 

void setLightSource (int gamma)
    {
    light_position0 [0] = -cosi [gamma];
    light_position0 [1] = sine [gamma];
    light_position0 [2] = 0;
    glLightfv (GL_LIGHT0, GL_POSITION, light_position0);
    } 

void setJoystickAxisString (int joysetting, char *joystr)
    {
    joystr [3] = 0;
    if (joysetting < 0)
        {
        strncpy (joystr, "N/A", 4);
        }
    else
        {
        joystr [0] = 'A' + (joysetting / 1000);
        joystr [1] = 32;
        joystr [2] = '0' + (joysetting % 1000);
        }
    } 

// calculate missile rack positions for a fighter
void setMissiles (CModel *model)
    {
    int i;
    CVector3 tlmissile (0, 0.3, 0.3);
    for (i = 0; i < model->numObjects; i ++)
        {
        if (model->object [i]->numVertices == 4)
            {
            CObject *o = model->object [i];
            float sumx = 0, sumz = 0;
            float maxy = 2;
            int i2;
            for (i2 = 0; i2 < o->numVertices; i2 ++)
                {
                sumx += o->vertex [i2].vector.x;
                if (o->vertex [i2].vector.y < maxy)
                    {
                    maxy = o->vertex [i2].vector.y;
                    }
                sumz += o->vertex [i2].vector.z;
                }
            tlmissile.x = sumx / 4.0F;
            tlmissile.y = maxy;
            tlmissile.z = sumz / 4.0F;
            tlmissile.y = maxy;
            model->addRefPoint (&tlmissile);
            }
        }
    } 

void setPlaneVolume ()
    {
    if (game == GAME_PLAY && fplayer->FuelLevel>0)
        {
        if (fplayer->WEPLevel && ((fplayer->thrust / fplayer->maxthrust)>0.9))
            {
            
            int lev = (int) ((float) 128 * (fplayer->thrust / fplayer->maxthrust)) -1;
            if (PlayerEngineType==0)
                {
                sound->setVolume (SOUND_PLANE1, lev); 
                }
            else if (PlayerEngineType==1)
                {
                sound->setVolume (SOUND_PLANE2, lev); 
                }
            }
        else
            {
            
            int lev = (int) ((float) 128 * (fplayer->thrust / fplayer->maxthrust)) - 64;
            if (lev < 1)
               {
               lev = 1;
               }
            if (PlayerEngineType==0)
                {
                sound->setVolume (SOUND_PLANE1, lev); 
                }
            else if (PlayerEngineType==1)
                {
                sound->setVolume (SOUND_PLANE2, lev); 
                }
            fplayer->WEPLevel=0; 
            }
        sound->engine = (int) (fplayer->thrust / fplayer->maxthrust * 20) - 10;
        if(PlayerEngineType==0)
            {
            sound->playLoop (SOUND_PLANE1);
            }
        else if (PlayerEngineType==1)
            {
            sound->playLoop (SOUND_PLANE2);
            }
        }
    else
        { 
        if (PlayerEngineType==0)
            {
            sound->setVolume (SOUND_PLANE1, 0);
            }
        else if (PlayerEngineType==1)
            {
            sound->setVolume (SOUND_PLANE2, 0);
            }
        }
    } 

int setScreen (int w, int h, int b, int f)
    {
    
    Uint32 video_flags;

    int numdisplays, nummodes;
    SDL_DisplayMode mode;

    if (f)
        {
// TODO: clean up around here
//        video_flags = SDL_OPENGL | SDL_FULLSCREEN;
        video_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
        }
    else
        {
//        video_flags = SDL_OPENGL;
        video_flags = SDL_WINDOW_OPENGL;
        }
    int rgb_size [3];
    switch (b)
        {
        case 8:
            rgb_size [0] = 2;
            rgb_size [1] = 3;
            rgb_size [2] = 3;
            break;
        case 15:
        case 16:
            rgb_size [0] = 5;
            rgb_size [1] = 5;
            rgb_size [2] = 5;
            break;
        default:
            rgb_size [0] = 8;
            rgb_size [1] = 8;
            rgb_size [2] = 8;
            break;
        }
    SDL_GL_SetAttribute (SDL_GL_RED_SIZE, rgb_size [0]);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, rgb_size [1]);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, rgb_size [2]);
    SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

//TODO: SDL2 window and gl contexts

	numdisplays = SDL_GetNumVideoDisplays();
        printf("\n%d displays.\n", numdisplays);
        for (int i = 0; i < numdisplays; ++i) {
                SDL_GetDisplayBounds(i, &bounds[i]);
                printf("Display %d: %s = %dx%d at %d,%d\n", i, SDL_GetDisplayName(i),
			bounds[i].w, bounds[i].h, bounds[i].x, bounds[i].y);

                printf("\nEnumerating modes for display %d\n", i);
                nummodes = SDL_GetNumDisplayModes(i);
                for (int j = 0; j < nummodes; ++j) {
                        SDL_GetDisplayMode(i, j, &mode);
                        printf("Display %d:  Mode %d:  %dx%d %dHz %d bpp\n", i, j, mode.w, mode
.h, mode.refresh_rate, SDL_BITSPERPIXEL(mode.format));
                }
                printf("  %d modes.\n", nummodes);
        }

	// win[0] = SDL_CreateWindow("LAC", bounds[0].x, bounds[0].y, bounds[0].w, bounds[0].h, video_flags);

	win[0] = SDL_CreateWindow("LAC", 0, 0, w, h, video_flags);
	glcontext[0] = SDL_GL_CreateContext(win[0]);
	SDL_GL_MakeCurrent(win[0], glcontext[0]);
	SDL_GL_SetSwapInterval(1);


#ifdef NOTDEF
    if (SDL_SetVideoMode (w, h, b, video_flags) == NULL)
        {
        if ((b = SDL_VideoModeOK (w, h, b, video_flags)) != 0)
            {
            b = 16;
            SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 5);
            SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
            SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 5);
            if (SDL_SetVideoMode (w, h, b, video_flags) == NULL)
                {
                b = 8;
                SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 2);
                SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 3);
                SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 3);
                if (SDL_SetVideoMode (w, h, b, video_flags) == NULL)
                    {
                    return 0;
                    }
                }
            }
        }
#endif /* NOTDEF */

    fprintf(stderr, "Note: setScreen Video %d %d %d\n", w, h, b);

    glViewport (0, 0, (GLint) w, (GLint) h);
    
    width = w;
    height = h;
    bpp = b; // Note: this is from input, not actual mode.
    fullscreen = f;
    wantwidth = w; 
    wantheight = h;
    wantfullscreen = f;
    return 1;
    }

void switch_credits ()
    {
    LastTime = 0;
    game = GAME_CREDITS;
    CreditsTimer = 300 * timestep;
    ReshapeCredits ();
    sound->stop (SOUND_WINDNOISE);
    if(PlayerEngineType==0)
        {
        sound->stop (SOUND_PLANE1);
        }
    else if (PlayerEngineType==1)
        {
        sound->stop (SOUND_PLANE2);
        }

    sound->stop (SOUND_CANNON1);
    } 

void switch_fighter ()
    { 
    display ((char*)"switch_fighter()", LOG_MOST);
    LastTime = 0;
    setLightSource (60);
    game = GAME_FIGHTER;
    ReshapeMenu ();
    if(PlayerEngineType==0)
        {
        sound->stop (SOUND_PLANE1);
        }
    else if (PlayerEngineType==1)
        {
        sound->stop (SOUND_PLANE2);
        }

    if (!sound->musicplaying)
        {
        playRandomMusic ();
        }
    } 

void switch_game ()
    {
    
    unsigned char i;
    for (i=0; i<=9; i++)
       {
       MorseSquelches[i] = false; 
       }
    TargetSelectNotYetInitialized = true; 
    MissionOutPacketCount = 0;  
    NetworkTransmitTimer = -1000; 
    NetworkMissionIsActiveWithDataFlow = false; 
    LastTime = 0;
    PlayersOriginalMaxThrust = fplayer->maxthrust;
    PlayersOriginalRollRate = fplayer->RollRate;
    PlayersOriginalDurability = (int) fplayer->Durability;
    ShotDownByBomberOrAirfieldGunners = false;
    ServerMessageTimer = 0;
    setLightSource ((int) sungamma);
    game = GAME_PLAY;
    ReshapeGame ();
    if (sound->musicplaying)
        {
        sound->haltMusic ();
        }
    if(PlayerEngineType==0)
        {
        sound->stop (SOUND_PLANE1);
        }
    else if (PlayerEngineType==1)
        {
        sound->stop (SOUND_PLANE2);
        }
    sound->stop (SOUND_LACSTARTUP00);
    GunCameraActive = false;
    setPlaneVolume ();
    sprintf (SystemMessageBuffer0, " "); 
    sprintf (SystemMessageBuffer1, " "); 
    sprintf (SystemMessageBuffer2, " "); 
    sprintf (SystemMessageBufferA, " "); 
    NewSystemMessageNeedsScrolling = true;

//    SDL_WM_GrabInput (SDL_GRAB_ON);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    } 

void switch_menu ()
    {
    display ((char*)"Function entry: switch_menu()", LOG_MOST);
    LastTime = 0;
    setLightSource ((int) sungamma);
    game = GAME_MENU;
    ReshapeMenu ();
    if(PlayerEngineType==0)
        {
        sound->stop (SOUND_PLANE1);
        }
    else if (PlayerEngineType==1)
        {
        sound->stop (SOUND_PLANE2);
        }
    sound->stop (SOUND_WINDNOISE);
    WindNoiseOn = false;
    if (!sound->musicplaying)
        {
        playRandomMusic ();
        }
    allmenus.visible = true;
    allmenus.components [0]->setVisible (true);
    
    if (currentsubmenu)
        {
        currentsubmenu->setVisible (true);
        }
    if (MissionActive)
        {
        mainbutton [6]->setVisible (true);
        }
    else
        {
        mainbutton [6]->setVisible (false);
        }

//    SDL_WM_GrabInput (SDL_GRAB_OFF);
    SDL_SetRelativeMouseMode(SDL_FALSE);	
    } 

void switch_mission (int missionid)
    {
    display ((char*)"switch_mission(). missionid = ", LOG_MOST);
    sprintf (DebugBuf, "%d", missionid);
    display (DebugBuf, LOG_MOST);
    LastTime = 0;
    setLightSource (60);
    game = GAME_MISSION;
    createMission (missionid);
    ReshapeMenu ();
    if(PlayerEngineType==0)
        {
        sound->stop (SOUND_PLANE1);
        }
    else if (PlayerEngineType==1)
        {
        sound->stop (SOUND_PLANE2);
        }
    if (!sound->musicplaying)
        {
        playRandomMusic ();
        }
    } 

void switch_quit ()
    {
    LastTime = 0;
    game = GAME_QUIT;
    } 

void TargetVocalizeProcess1 ()
   {
   extern char TargetVocalize1State;
   extern int TargetVocalizeTimer1;
   static bool State1Vocalized;
   static bool State2Vocalized;
   static bool State3Vocalized;
   static bool State4Vocalized;
   static bool State5Vocalized;
   static bool State6Vocalized;
   static bool State7Vocalized;
   static bool State8Vocalized;
   display ((char*)"TargetVocalizeProcess1()", LOG_ALL);
   if (TargetVocalize1State == 0)
      {
      return;
      }
   TargetVocalizeTimer1 += DeltaTime;
   if (TargetVocalizeTimer1 > 240)
      {
      TargetVocalize1State = 8;
      }
   else if (TargetVocalizeTimer1 > 205)
      {
      TargetVocalize1State = 7;
      }
   else if (TargetVocalizeTimer1 > 180)
      {
      TargetVocalize1State = 6;
      }
   else if (TargetVocalizeTimer1 > 150)
      {
      TargetVocalize1State = 5;
      }
   else if (TargetVocalizeTimer1 > 110)
      {
      TargetVocalize1State = 4;
      }
   else if (TargetVocalizeTimer1 > 70)
      {
      TargetVocalize1State = 3;
      }
   else if (TargetVocalizeTimer1 > 35)
      {
      TargetVocalize1State = 2;
      }
   else if (TargetVocalizeTimer1 > 1)
      {
      TargetVocalize1State = 1;
      }
   if (TargetVocalize1State == 1  && !State1Vocalized)
      {
      sound->setVolume (SOUND_TARGET, 90);
      sound->play (SOUND_TARGET, false);
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = true;
      }
   else if (TargetVocalize1State == 2  && !State2Vocalized)
      {
      switch (SelectedMissionTarget)
         {
         case 1:
             {
             sound->setVolume (SOUND_DIGIT001, 90);
             sound->play (SOUND_DIGIT001, false);
             break;
             }
         case 2:
             {
             sound->setVolume (SOUND_DIGIT002, 90);
             sound->play (SOUND_DIGIT002, false);
             break;
             }
         case 3:
             {
             sound->setVolume (SOUND_DIGIT003, 90);
             sound->play (SOUND_DIGIT003, false);
             break;
             }
         case 4:
             {
             sound->setVolume (SOUND_DIGIT004, 90);
             sound->play (SOUND_DIGIT004, false);
             break;
             }
         case 5:
             {
             sound->setVolume (SOUND_DIGIT005, 90);
             sound->play (SOUND_DIGIT005, false);
             break;
             }
         case 6:
             {
             sound->setVolume (SOUND_DIGIT006, 90);
             sound->play (SOUND_DIGIT006, false);
             break;
             }
         case 7:
             {
             sound->setVolume (SOUND_DIGIT007, 90);
             sound->play (SOUND_DIGIT007, false);
             break;
             }
         case 8:
             {
             sound->setVolume (SOUND_DIGIT008, 90);
             sound->play (SOUND_DIGIT008, false);
             break;
             }
         case 9:
             {
             sound->setVolume (SOUND_DIGIT009, 90);
             sound->play (SOUND_DIGIT009, false);
             break;
             }
         case 10:
             {
             sound->setVolume (SOUND_DIGIT010, 90);
             sound->play (SOUND_DIGIT010, false);
             break;
             }
         case 28:
             {
             sound->setVolume (SOUND_BLUE, 90);
             sound->play (SOUND_BLUE, false);
             break;
             }
         case 29:
             {
             sound->setVolume (SOUND_RED, 90);
             sound->play (SOUND_RED, false);
             break;
             }
         default:
             {
             break;
             }
        } 
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = true;
      State1Vocalized = false;
      } 
   else if (TargetVocalize1State == 3  && !State3Vocalized)
      {
      sound->setVolume (SOUND_IsAt, 90);
      sound->play (SOUND_IsAt, false);
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = true;
      State2Vocalized = false;
      State1Vocalized = false;
      }
   else if (TargetVocalize1State == 4  && !State4Vocalized)
      { 
      int InterceptAngle = fplayer->getAngle (ThreeDObjects [SelectedMissionTarget]);
      InterceptAngle *= -1;
      if (InterceptAngle < 0)
             { 
             InterceptAngle += 360;
             }
      if (InterceptAngle >= 0  && InterceptAngle < 15)
         {
         sound->play (SOUND_DIGIT012, false);
         }
      else if (InterceptAngle > 15 && InterceptAngle < 45)
         {
         sound->play (SOUND_DIGIT001, false);
         }
      else if (InterceptAngle > 45 && InterceptAngle < 75)
         {
         sound->play (SOUND_DIGIT002, false);
         }
      else if (InterceptAngle > 75 && InterceptAngle < 105 )
         {
         sound->play (SOUND_DIGIT003, false);
         }
      else if (InterceptAngle > 105 && InterceptAngle < 135)
         {
         sound->play (SOUND_DIGIT004, false);
         }
      else if (InterceptAngle > 135 && InterceptAngle < 165)
         {
         sound->play (SOUND_DIGIT005, false);
         }
      else if (InterceptAngle > 165 && InterceptAngle < 195)
         {
         sound->play (SOUND_DIGIT006, false);
         }
      else if (InterceptAngle > 195 && InterceptAngle < 225)
         {
         sound->play (SOUND_DIGIT007, false);
         }
      else if (InterceptAngle > 225 && InterceptAngle < 255)
         {
         sound->play (SOUND_DIGIT008, false);
         }
      else if (InterceptAngle > 255 && InterceptAngle < 285)
         {
         sound->play (SOUND_DIGIT009, false);
         }
      else if (InterceptAngle > 285 && InterceptAngle < 315)
         {
         sound->play (SOUND_DIGIT010, false);
         }
      else if (InterceptAngle > 315 && InterceptAngle < 345)
         {
         sound->play (SOUND_DIGIT011, false);
         }
      else if (InterceptAngle > 345 && InterceptAngle <= 360)
         {
         sound->play (SOUND_DIGIT012, false);
         }
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = true;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = false;
      } 
   else if (TargetVocalize1State == 5  && !State5Vocalized)
      {
      sound->setVolume (SOUND_Oclock, 90);
      sound->play (SOUND_Oclock, false);
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = true;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = false;
      }
   else if (TargetVocalize1State == 6  && !State6Vocalized)
      {
      sound->setVolume (SOUND_Angels, 90);
      sound->play (SOUND_Angels, false);
      State8Vocalized = false;
      State7Vocalized = false;
      State6Vocalized = true;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = false;
      }
   else if (TargetVocalize1State == 7  && !State7Vocalized)
      { 
      if ((fplayer->target->tl->y / 25.0) < 1.4)
         {
         sound->setVolume (SOUND_DIGIT000, 90);
         sound->play (SOUND_DIGIT000, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 3.4)
         {
         sound->setVolume (SOUND_DIGIT001, 90);
         sound->play (SOUND_DIGIT001, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 5.2)
         {
         sound->setVolume (SOUND_DIGIT002, 90);
         sound->play (SOUND_DIGIT002, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 7.0)
         {
         sound->setVolume (SOUND_DIGIT003, 90);
         sound->play (SOUND_DIGIT003, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 8.8)
         {
         sound->setVolume (SOUND_DIGIT004, 90);
         sound->play (SOUND_DIGIT004, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 10.6)
         {
         sound->setVolume (SOUND_DIGIT005, 90);
         sound->play (SOUND_DIGIT005, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 12.4)
         {
         sound->setVolume (SOUND_DIGIT006, 90);
         sound->play (SOUND_DIGIT006, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 14.2)
         {
         sound->setVolume (SOUND_DIGIT007, 90);
         sound->play (SOUND_DIGIT007, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 16.0)
         {
         sound->setVolume (SOUND_DIGIT008, 90);
         sound->play (SOUND_DIGIT008, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 18.0)
         {
         sound->setVolume (SOUND_DIGIT009, 90);
         sound->play (SOUND_DIGIT009, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 20.0)
         {
         sound->setVolume (SOUND_DIGIT010, 90);
         sound->play (SOUND_DIGIT010, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 22)
         {
         sound->setVolume (SOUND_DIGIT011, 90);
         sound->play (SOUND_DIGIT011, false);
         }
      else if ((fplayer->target->tl->y / 25.0) <24)
         {
         sound->setVolume (SOUND_DIGIT012, 90);
         sound->play (SOUND_DIGIT012, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 26)
         {
         sound->setVolume (SOUND_DIGIT013, 90);
         sound->play (SOUND_DIGIT013, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 28)
         {
         sound->setVolume (SOUND_DIGIT014, 90);
         sound->play (SOUND_DIGIT014, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 30)
         {
         sound->setVolume (SOUND_DIGIT015, 90);
         sound->play (SOUND_DIGIT015, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 32)
         {
         sound->setVolume (SOUND_DIGIT016, 90);
         sound->play (SOUND_DIGIT016, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 34)
         {
         sound->setVolume (SOUND_DIGIT017, 90);
         sound->play (SOUND_DIGIT017, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 36)
         {
         sound->setVolume (SOUND_DIGIT018, 90);
         sound->play (SOUND_DIGIT018, false);
         }
      else if ((fplayer->target->tl->y / 25.0) < 38)
         {
         sound->setVolume (SOUND_DIGIT019, 90);
         sound->play (SOUND_DIGIT019, false);
         }
      else if (((fplayer->target->tl->y / 25.0) > 38) && ((fplayer->target->tl->y / 25.0) < 58))
         {
         sound->setVolume (SOUND_DIGIT020, 90);
         sound->play (SOUND_DIGIT020, false);
         }
      else if ((fplayer->target->tl->y / 25.0) > 58)
         {
         sound->setVolume (SOUND_DIGIT030, 90);
         sound->play (SOUND_DIGIT030, false);
         }
      State8Vocalized = false;
      State7Vocalized = true;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = false;
      } 
   else if (TargetVocalize1State == 8  && !State8Vocalized)
      { 
      if (((fplayer->target->tl->y / 25.0) > 40) && ((fplayer->target->tl->y / 25.0) < 60))
         { 
         if ((fplayer->target->tl->y / 25.0) < 42)
            {
            sound->setVolume (SOUND_DIGIT001, 90);
            sound->play (SOUND_DIGIT001, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 44)
            {
            sound->setVolume (SOUND_DIGIT002, 90);
            sound->play (SOUND_DIGIT002, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 46)
            {
            sound->setVolume (SOUND_DIGIT003, 90);
            sound->play (SOUND_DIGIT003, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 48)
            {
            sound->setVolume (SOUND_DIGIT004, 90);
            sound->play (SOUND_DIGIT004, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 50)
            {
            sound->setVolume (SOUND_DIGIT005, 90);
            sound->play (SOUND_DIGIT005, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 52)
            {
            sound->setVolume (SOUND_DIGIT006, 90);
            sound->play (SOUND_DIGIT006, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 54)
            {
            sound->setVolume (SOUND_DIGIT007, 90);
            sound->play (SOUND_DIGIT007, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 56)
            {
            sound->setVolume (SOUND_DIGIT008, 90);
            sound->play (SOUND_DIGIT008, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 58)
            {
            sound->setVolume (SOUND_DIGIT009, 90);
            sound->play (SOUND_DIGIT009, false);
            }
         } 
      else if (((fplayer->target->tl->y / 25.0) > 60) && ((fplayer->target->tl->y / 25.0) < 80))
         { 
         if ((fplayer->target->tl->y / 25.0) < 62)
            {
            sound->setVolume (SOUND_DIGIT001, 90);
            sound->play (SOUND_DIGIT001, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 64)
            {
            sound->setVolume (SOUND_DIGIT002, 90);
            sound->play (SOUND_DIGIT002, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 66)
            {
            sound->setVolume (SOUND_DIGIT003, 90);
            sound->play (SOUND_DIGIT003, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 68)
            {
            sound->setVolume (SOUND_DIGIT004, 90);
            sound->play (SOUND_DIGIT004, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 70)
            {
            sound->setVolume (SOUND_DIGIT005, 90);
            sound->play (SOUND_DIGIT005, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 72)
            {
            sound->setVolume (SOUND_DIGIT006, 90);
            sound->play (SOUND_DIGIT006, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 74)
            {
            sound->setVolume (SOUND_DIGIT007, 90);
            sound->play (SOUND_DIGIT007, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 76)
            {
            sound->setVolume (SOUND_DIGIT008, 90);
            sound->play (SOUND_DIGIT008, false);
            }
         else if ((fplayer->target->tl->y / 25.0) < 78)
            {
            sound->setVolume (SOUND_DIGIT009, 90);
            sound->play (SOUND_DIGIT009, false);
            }
         }
      State8Vocalized = true;
      State7Vocalized = false;
      State6Vocalized = false;
      State5Vocalized = false;
      State4Vocalized = false;
      State3Vocalized = false;
      State2Vocalized = false;
      State1Vocalized = false;
      TargetVocalize1State = 0;
      TargetVocalizeTimer1 = 0;
      }
   } 

void TargetVocalizeProcess2 ()
{
extern int TargetVocalizeTimer2;
TargetVocalizeTimer2 -= DeltaTime; 
if (TargetVocalizeTimer2 < 0)
   { 
   TargetVocalizeTimer2 = 0;
   }
if (TargetVocalizeTimer2 ==0 && TargetVocalize2State)
   { 
   switch (SelectedMissionTarget)
      {
      case 1:
          {
          sound->setVolume (SOUND_DIGIT001, 90);
          sound->play (SOUND_DIGIT001, false);
          TargetVocalize2State = 0;
          break;
          }
      case 2:
          {
          sound->setVolume (SOUND_DIGIT002, 90);
          sound->play (SOUND_DIGIT002, false);
          TargetVocalize2State = 0;
          break;
          }
      case 3:
          {
          sound->setVolume (SOUND_DIGIT003, 90);
          sound->play (SOUND_DIGIT003, false);
          TargetVocalize2State = 0;
          break;
          }
      case 4:
          {
          sound->setVolume (SOUND_DIGIT004, 90);
          sound->play (SOUND_DIGIT004, false);
          TargetVocalize2State = 0;
          break;
          }
      case 5:
          {
          sound->setVolume (SOUND_DIGIT005, 90);
          sound->play (SOUND_DIGIT005, false);
          TargetVocalize2State = 0;
          break;
          }
      case 6:
          {
          sound->setVolume (SOUND_DIGIT006, 90);
          sound->play (SOUND_DIGIT006, false);
          TargetVocalize2State = 0;
          break;
          }
      case 7:
          {
          sound->setVolume (SOUND_DIGIT007, 90);
          sound->play (SOUND_DIGIT007, false);
          TargetVocalize2State = 0;
          break;
          }
      case 8:
          {
          sound->setVolume (SOUND_DIGIT008, 90);
          sound->play (SOUND_DIGIT008, false);
          TargetVocalize2State = 0;
          break;
          }
      case 9:
          {
          sound->setVolume (SOUND_DIGIT009, 90);
          sound->play (SOUND_DIGIT009, false);
          TargetVocalize2State = 0;
          break;
          }
      case 10:
          {
          sound->setVolume (SOUND_DIGIT010, 90);
          sound->play (SOUND_DIGIT010, false);
          TargetVocalize2State = 0;
          break;
          }
      default:
          {
          break;
          }
      }
   }
} 

void textControls (char *buf)
    {
    if (controls == CONTROLS_KEYBOARD)
        {
        sprintf (buf, "%s", "NEXT PAGE");
        }
    else if (controls == CONTROLS_MOUSE)
        {
        sprintf (buf, "%s", "MOUSE");
        }
    else if (controls == CONTROLS_JOYSTICK)
        {
        sprintf (buf, "%s", "NEXT PAGE");
        }
    }

void textMouseButton (char *buf, int button)
    {
    if (button == 0)
        {
        strncpy (buf, "NONE", 5);
        }
    else
        {
        sprintf (buf, "BUTTON%d", button);
        }
    } 

void TimerCredits (Uint32 dt)
    {
    CreditsTimer += dt;

    if (CreditsTimer > 1400 * timestep)
        {
        CreditsTimer = 0;
        }
    sdldisplay = true;
    } 

void TimerFinish (Uint32 dt)
    {
    FinishTimer += dt;

    if (FinishTimer > 800 * timestep)
        {
        FinishTimer = 0;
        }
    sdldisplay = true;
    } 

/*
*
* TimerGame()
*/
void TimerGame (int dt)
    {
    int i, i2;
    sunlight += (sunlight_dest - sunlight) / 10 * dt / timestep;
    cockpit->dt = dt;
    l->lsticker += dt;
    if (l->lsticker >= 36000000)
        {
        l->lsticker = 0;
        }
    
    if(fplayer->UndercarriageLevel)
        {
        
        if(fplayer->InertiallyDampenedPlayerSpeed > (fplayer->FlapSpeed * 0.85))
            {
            
            sound->setVolume (SOUND_UNDERCARRIAGE, 88); 
            sound->play (SOUND_UNDERCARRIAGE, false);
            fplayer->UndercarriageLevel=0;
            }
        }
    if(fplayer->FlapsLevel)
        {
        
        if(fplayer->InertiallyDampenedPlayerSpeed > (fplayer->FlapSpeed * 1.1))
            {
            
            sound->setVolume (SOUND_FLAPSUP, 88); 
            sound->play (SOUND_FLAPSUP, false);
            fplayer->FlapsLevel--;
            switch (fplayer->FlapsLevel)
                {
                case 1:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect1;
                    break;
                    }
                case 2:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect2;
                    break;
                    }
                case 3:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect3;
                    break;
                    }
                case 4:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect4;
                    break;
                    }
                default:
                    {
                    FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
                    }
                }
            if (fplayer->FlapsLevel<=0)
                {
                fplayer->FlapsLevel=0;
                FlapsLevelElevatorEffectNow=fplayer->FlapsLevelElevatorEffect0;
                }
            }
        }
    if (vibration > 0)
        {
        vibration -= dt;
        }
    if (fplayer->autofire && fplayer->active)
        {
        
        if (fplayer->ammo > 0)
            {
            
            fplayer->fireCannon (MachineGunBullet);
            sound->setVolume (SOUND_CANNON1, 120);
            sound->play (SOUND_CANNON1, 1);
            }
        else
            {
            
            sound->stop (SOUND_CANNON1);
            }
        }
    else
        {
        
        if (StartCannon)
            {
            StartCannon = false;
            sound->stop (SOUND_CANNON1);
            }
        }
    if (lastDurability > fplayer->Durability && !fplayer->ai)
        {
        
        fplayer->RollRate *=0.95; 
        fplayer->maxthrust*=0.995; 
        sound->play (SOUND_HIT1, false);
        lastDurability = (int) fplayer->Durability;
        vibration = 12 * timestep;
        }
    // create flash during thunderstorm
    if (weather == WEATHER_THUNDERSTORM && flash <= 0 && !myrandom (2000 / dt))
        {
        flash = 18 * timestep;
        int fphi = (int) camphi + myrandom (50) - 25;
        if (fphi < 0)
            {
            fphi += 360;
            }
        else if (fphi >= 360)
            {
            fphi -= 360;
            }
        float pseudoview = getView ();
        float fdist = myrandom ((int) pseudoview - 20) + 10;
        float fx = fplayer->tl->x - sine [fphi] * fdist;
        float fz = fplayer->tl->z - cosi [fphi] * fdist;
        flash1->set (fx, l->getHeight (fx, fz), fz, (int) camphi);
        }
    if (initing)
        {
        return;
        }
    if (flash > 0)
        {
        flash -= dt;
        }
    if (flash <= 7 * timestep && flash > 0)
        {
        flash -= dt;
        }
    if (!fplayer->ai)
        {
        if (sound->engine != (int) ((fplayer->thrust / fplayer->maxthrust * 20) - 10))
            {
            setPlaneVolume ();
            }
        }
    // collision tests
    for (i = 0; i < maxfighter; i ++)
        {
        for (i2 = 0; i2 < maxMachineGunBullet; i2 ++)
            if (MachineGunBullet [i2]->active)
                {
                ThreeDObjects [i]->collide (MachineGunBullet [i2], dt);
                }
        for (i2 = 0; i2 < maxmissile; i2 ++)
            if (missile [i2]->active)
                {
                ThreeDObjects [i]->collide (missile [i2], dt);
                }
        for (i2 = 0; i2 < i; i2 ++)
            if (ThreeDObjects [i2]->active)
                if (i != i2)
                    {
                    ThreeDObjects [i]->collide (ThreeDObjects [i2], dt);
                    }
        }
    for (i = 0; i < maxflare; i ++)
        {
        for (i2 = 0; i2 < maxmissile; i2 ++)
            if (missile [i2]->active)
                {
                flare [i]->collide (missile [i2], dt);
                }
        }
    for (i = 0; i < maxchaff; i ++)
        {
        for (i2 = 0; i2 < maxmissile; i2 ++)
            if (missile [i2]->active)
                {
                chaff [i]->collide (missile [i2], dt);
                }
        }
    // move objects
    for (i = 0; i < maxfighter; i ++)
        {
        ThreeDObjects [i]->aiAction (dt, (AIObj **) ThreeDObjects, missile, MachineGunBullet, flare, chaff);
        float lev;
        if (ThreeDObjects [i]->explode == 1 && (lev = fplayer->distance (ThreeDObjects [i])) < 32)
            { 
            sound->setVolume (SOUND_EXPLOSION1, 128);
            float angle = fplayer->getAngle (ThreeDObjects [i]);
            angle = 180 - angle;
            sound->setPosition (SOUND_EXPLOSION1, (int) angle, (int) (lev * 4.0));
            sound->play (SOUND_EXPLOSION1, false);
            }
        }
    for (i = 0; i < maxMachineGunBullet; i ++)
        {
        MachineGunBullet [i]->move (dt);
        }
    for (i = 0; i < maxmissile; i ++)
        {
        missile [i]->aiAction (dt, (AIObj **) ThreeDObjects, missile, MachineGunBullet, flare, chaff);
        }
    for (i = 0; i < maxflare; i ++)
        {
        flare [i]->move (dt);
        }
    for (i = 0; i < maxchaff; i ++)
        {
        chaff [i]->move (dt);
        }
    for (i = 0; i < maxexplosion; i ++)
        {
        explosion [i]->move (dt);
        }
    for (i = 0; i < maxblacksmoke; i ++)
        {
        blacksmoke [i]->move (dt);
        }
    
    if (blackout > 0)
        {
        blackout -= 3.0F * dt / timestep;
        }
    if (blackout < 0)
        {
        blackout = 0;
        }
    if (redout > 0)
        {
        redout -= 3.0F * dt / timestep;
        }
    if (redout < 0)
        {
        redout = 0;
        }
    if (blackout > 400)
        {
        blackout = 400;
        }
    if (redout > 400)
        {
        redout = 400;
        }
    /*
    * handle blackout and redout
    */
    float testout;
    if (redout < 1)
        {
        
        testout = (fplayer->realspeed*2 * (fplayer->elevatoreffect - 0.5F)); 
        testout = testout *  fplayer->BlackoutSensitivity;
        if (testout > 0.0)
            {
            blackout += testout * dt / timestep;
            }
        }
    if (blackout < 1)
        {
        
        testout = (fplayer->realspeed * (fplayer->elevatoreffect + 0.08F)); 
        testout=testout *  fplayer->RedoutSensitivity;
        if (testout < 0)
            {
            redout -= testout * dt / timestep;
            }
        }
    
    if (fplayer->Durability <= 0)
        {
        camera = 1;
        }
    
    float cf = -fplayer->zoom / 2;
    if (camera == 0)   
        {
        LookForward();
        }
    else if (camera == 1)   
        {
        LookChase();
        }
    else if (camera == 2)     
        {
        LookBack();
        }
    else if (camera == 4)     
        {
        LookMissile(cf);
        }
    else if (camera == 5)     
        {
        LookUp();
        }
    else if (camera == 6)     
        {
        LookLeft();
        } 
    else if (camera == 7)   
        {
        LookRight();
        } 
    else if (camera == 8)   
        {
        LookDown ();
        }
    else if (camera == 9)     
        {
        LookMap();
        }
    else if (camera == 50)
        {
        camx = 20;
        camz = 80;
        camy = 250;
        camphi = 20;
        camgamma = 75;
        camtheta = fplayer->theta;
        game = GAME_PAUSE;
        }
    if (camphi >= 360.0)
        {
        camphi -= 360.0;
        }
    if (camtheta>=360.0)
        {
        camtheta-=360.0;
        }
    if (camgamma>=360.0)
        {
        camgamma-=360.0;
        }
    sdldisplay = true;
    fplayer->rectheta -= dtheta;
    fplayer->recgamma += dgamma;
    missionstate = mission->processtimer (dt); // Causes segfault after program startup.
    if (missionstate == 2)
        {
        MissionEnding ++;
        if (MissionEnding >= 25)
            {
            MissionEnding = 0;
            if (mission->id == MISSION_DEMO)
                {
                pleaseWait ();
                createMission (MISSION_DEMO);
                game_levelInit ();
                }
            }
        }
    if (missionstate == 1)
        {
        MissionEnding ++;
        if (MissionEnding >= 25)
            {
            MissionEnding = 0;
            if (mission->id == MISSION_DEMO)
                {
                pleaseWait ();
                //createMission (MISSION_DEMO);
                game_levelInit ();
                }
            }
        }
    } 

void TimerMenu (Uint32 dt)
    {
    char buf [256];
    MenuTimer += dt;
    int akttime;
    akttime = SDL_GetTicks ();
    if (FirstStart)
        if (akttime - starttime < 20000)
            {
            if (lastfps != fps)
                {
                lastfps = fps;
                if (fps > 40)
                    {
                    if (view < quality * 20 + 60 && view < 100)
                        {
                        view += 20;
                        }
                    else if (quality < 5)
                        {
                        quality ++;
                        view = quality * 20 + 30;
                        if (quality >= 1)
                            {
                            antialiasing = 1;
                            specialeffects = 1;
                            dithering = 1;
                            dynamiclighting = 0;
                            }
                        }
                    sprintf (buf, "%d", (int) quality);
                    ((Label *) optmenu [0]->components [1])->setText (buf);
                    sprintf (buf, "%d", (int) view);
                    ((Label *) optmenu [0]->components [3])->setText (buf);
                    }
                else if (fps < 30)
                    {
                    if (view > quality * 20 + 30 && view > 20)
                        {
                        view -= 20;
                        }
                    else if (quality > 0)
                        {
                        quality --;
                        view = quality * 20 + 60;
                        if (quality < 1)
                            {
                            antialiasing = 0;
                            specialeffects = 0;
                            dithering = 0;
                            dynamiclighting = 0;
                            }
                        }
                    sprintf (buf, "%d", (int) quality);
                    ((Label *) optmenu [0]->components [1])->setText (buf);
                    sprintf (buf, "%d", (int) view);
                    ((Label *) optmenu [0]->components [3])->setText (buf);
                    }
                ReshapeMenu ();
                }
            }
    sdldisplay = true;
    } 

void TimerMission (Uint32 dt)
    {
    MissionMenuTimer += dt;
    sdldisplay = true;
    } 

void TimerStats (Uint32 dt)
    {
    MenuTimer += dt;
    sdldisplay = true;
    } 

void TransmitCharacterViaMorseRadio (int key)
{
unsigned char ArrayIndex = 0; 
while (MorseCharacterOut[ArrayIndex != 0] && ArrayIndex < 16)
   { 
   
   ArrayIndex++; 
   }
if (ArrayIndex > 15)
   { 
   ArrayIndex = 0;
   return;
   }
MorseCharacterOut[ArrayIndex] = (unsigned char)key; 
} 

void UpdateOnlineScoreLogFileWithCalculatedRisks()
{
if (!ShotDownByBomberOrAirfieldGunners)
   {
   display ((char*)"UpdateOnlineScoreLogFileWithCalculatedRisks()\n", LOG_MOST);
   time_t now = time(NULL);
   struct tm *CurrentTimeStruct = gmtime(&now);
   strftime(DebugBuf, sizeof(DebugBuf)-1, "SHOT DOWN BY BOMBER OR AIRFIELD GUNNERS %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Realm=%d, ", Realm);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My Velocity=%f.\n", fplayer->InertiallyDampenedPlayerSpeed / timestep * 50000.0F);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My XZ Position = %f, %f\n", fplayer->tl->x, fplayer->tl->z);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My altitude in feet: %5.0f.\n\n", (50.0 * (fplayer->tl->y + SeaLevel )) - 17.0);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   ShotDownByBomberOrAirfieldGunners = true;
   }
} 

void UpdateOnlineScoreLogFileWithCrashes()
{
if (!LandingLogged)
   {
   LandingLogged = true;
   display ((char*)"UpdateOnlineScoreLogFileWithCrashes()\n", LOG_MOST);
   time_t now = time(NULL);
   struct tm *CurrentTimeStruct = gmtime(&now);
   strftime(DebugBuf, sizeof(DebugBuf)-1, "CRASHED %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Realm=%d, ", Realm);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My XZ Position = %f, %f\n\n", fplayer->tl->x, fplayer->tl->z);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
} 

void UpdateOnlineScoreLogFileWithDefeats()
{
time_t now = time(NULL);
struct tm *CurrentTimeStruct = gmtime(&now);
char AircraftName[32]; 
strftime(DebugBuf, sizeof(DebugBuf)-1, "DEFEAT %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "\n");
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   Realm=%d, ", Realm);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
switch (PlayerAircraftType)
   {
   case 201:
       {
       sprintf (AircraftName, "Hawk");
       break;
       }
   case 202:
       {
       sprintf (AircraftName, "Fiat G55");
       break;
       }
   case 203:
       {
       sprintf (AircraftName, "Me109G");
       break;
       }
   case 204:
       {
       sprintf (AircraftName, "P40");
       break;
       }
   case 205:
       {
       sprintf (AircraftName, "Hurricane");
       break;
       }
   case 206:
       {
       sprintf (AircraftName, "Ki43");
       break;
       }
   case 207:
       {
       sprintf (AircraftName, "Spit9");
       break;
       }
   case 208:
       {
       sprintf (AircraftName, "Polikarpov I16");
       break;
       }
   case 209:
       {
       sprintf (AircraftName, "Ju87");
       break;
       }
   case 210:
       {
       sprintf (AircraftName, "A6M2");
       break;
       }
   case 211:
       {
       sprintf (AircraftName, "F4u");
       break;
       }
   case 212:
       {
       sprintf (AircraftName, "F6F");
       break;
       }
   case 213:
       {
       sprintf (AircraftName, "P38L");
       break;
       }
   case 214:
       {
       sprintf (AircraftName, "P47D");
       break;
       }
   case 215:
       {
       sprintf (AircraftName, "P51D");
       break;
       }
   case 216:
       {
       sprintf (AircraftName, "B17");
       break;
       }
   case 217:
       {
       sprintf (AircraftName, "FW190");
       break;
       }
   case 218:
       {
       sprintf (AircraftName, "Yak9");
       break;
       }
   case 219:
       {
       sprintf (AircraftName, "N1K1");
       break;
       }
   case 220:
       {
       sprintf (AircraftName, "B24");
       break;
       }
   case 221:
       {
       sprintf (AircraftName, "P39");
       break;
       }
   case 222:
       {
       sprintf (AircraftName, "Mitsubishi G5M");
       break;
       }
   case 223:
       {
       sprintf (AircraftName, "B25");
       break;
       }
   case 224:
       {
       sprintf (AircraftName, "B26");
       break;
       }
   case 225:
       {
       sprintf (AircraftName, "F4F");
       break;
       }
   case 226:
       {
       sprintf (AircraftName, "La5");
       break;
       }
   case 227:
       {
       sprintf (AircraftName, "La7");
       break;
       }
   case 228:
       {
       sprintf (AircraftName, "Il2");
       break;
       }
   case 229:
       {
       sprintf (AircraftName, "Macci 202");
       break;
       }
   case 230:
       {
       sprintf (AircraftName, "Lancaster");
       break;
       }
   case 231:
       {
       sprintf (AircraftName, "Mosquito");
       break;
       }
   case 232:
       {
       sprintf (AircraftName, "Typhoon");
       break;
       }
   case 233:
       {
       sprintf (AircraftName, "Yak1");
       break;
       }
   case 234:
       {
       sprintf (AircraftName, "B29");
       break;
       }
   case 235:
       {
       sprintf (AircraftName, "DW520");
       break;
       }
   case 236:
       {
       sprintf (AircraftName, "SB2C");
       break;
       }
   case 237:
       {
       sprintf (AircraftName, "TBF");
       break;
       }
   case 238:
       {
       sprintf (AircraftName, "ME163");
       break;
       }
   case 239:
       {
       sprintf (AircraftName, "Tempest");
       break;
       }
   case 240:
       {
       sprintf (AircraftName, "D3a");
       break;
       }
   case 241:
       {
       sprintf (AircraftName, "B5N");
       break;
       }
   case 242:
       {
       sprintf (AircraftName, "Dauntless");
       break;
       }
   case 243:
       {
       sprintf (AircraftName, "Me110");
       break;
       }
   case 244:
       {
       sprintf (AircraftName, "Dornier");
       break;
       }
   case 245:
       {
       sprintf (AircraftName, "He111");
       break;
       }
   case 246:
       {
       sprintf (AircraftName, "Ju88");
       break;
       }
   case 247:
       {
       sprintf (AircraftName, "KI84");
       break;
       }
   case 248:
       {
       sprintf (AircraftName, "KI61");
       break;
       }
   case 249:
       {
       sprintf (AircraftName, "Generic");
       break;
       }
   case 250:
       {
       sprintf (AircraftName, "A6M5");
       break;
       }
   case 251:
       {
       sprintf (AircraftName, "Spit5");
       break;
       }
   case 252:
       {
       sprintf (AircraftName, "P51B");
       break;
       }
   case 253:
       {
       sprintf (AircraftName, "P47B");
       break;
       }
   case 254:
       {
       sprintf (AircraftName, "Me109F");
       break;
       }
   case 255:
       {
       sprintf (AircraftName, "P38F");
       break;
       }
   default:
       {
       sprintf (AircraftName, "Unknown");
       }
   }
sprintf (DebugBuf, "   MyAircraft=%s.\n", AircraftName);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Velocity=%f.\n", fplayer->InertiallyDampenedPlayerSpeed / timestep * 50000.0F);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My altitude in feet: %5.0f.\n", (50.0 * (fplayer->tl->y + SeaLevel )) - 17.0);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My XZ Position = %f, %f\n", fplayer->tl->x, fplayer->tl->z);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My GammaPhiTheta orientation = %f, %f, %f\n", fplayer->gamma, fplayer->phi, fplayer->theta);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   Mission player or facility that destroyed me = %d.\n", InPacket.UdpObjPlayerNumber);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
if (InPacket.UdpObjPlayerNumber <= 10)
   { 
   sprintf (DebugBuf, "   Victorious Opponent's CommunityHandle = %s\n", PlayerIdStrings[InPacket.UdpObjPlayerNumber -1]);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
switch (InPacket.UdpObjVehicle)
   {
   case 201:
       {
       sprintf (AircraftName, "Hawk");
       break;
       }
   case 202:
       {
       sprintf (AircraftName, "Fiat G55");
       break;
       }
   case 203:
       {
       sprintf (AircraftName, "Me109G");
       break;
       }
   case 204:
       {
       sprintf (AircraftName, "P40");
       break;
       }
   case 205:
       {
       sprintf (AircraftName, "Hurricane");
       break;
       }
   case 206:
       {
       sprintf (AircraftName, "Ki43");
       break;
       }
   case 207:
       {
       sprintf (AircraftName, "Spit9");
       break;
       }
   case 208:
       {
       sprintf (AircraftName, "Polikarpov I16");
       break;
       }
   case 209:
       {
       sprintf (AircraftName, "Ju87");
       break;
       }
   case 210:
       {
       sprintf (AircraftName, "A6M2");
       break;
       }
   case 211:
       {
       sprintf (AircraftName, "F4u");
       break;
       }
   case 212:
       {
       sprintf (AircraftName, "F6F");
       break;
       }
   case 213:
       {
       sprintf (AircraftName, "P38L");
       break;
       }
   case 214:
       {
       sprintf (AircraftName, "P47D");
       break;
       }
   case 215:
       {
       sprintf (AircraftName, "P51D");
       break;
       }
   case 216:
       {
       sprintf (AircraftName, "B17");
       break;
       }
   case 217:
       {
       sprintf (AircraftName, "FW190");
       break;
       }
   case 218:
       {
       sprintf (AircraftName, "Yak9");
       break;
       }
   case 219:
       {
       sprintf (AircraftName, "N1K1");
       break;
       }
   case 220:
       {
       sprintf (AircraftName, "B24");
       break;
       }
   case 221:
       {
       sprintf (AircraftName, "P39");
       break;
       }
   case 222:
       {
       sprintf (AircraftName, "Mitsubishi G5M");
       break;
       }
   case 223:
       {
       sprintf (AircraftName, "B25");
       break;
       }
   case 224:
       {
       sprintf (AircraftName, "B26");
       break;
       }
   case 225:
       {
       sprintf (AircraftName, "F4F");
       break;
       }
   case 226:
       {
       sprintf (AircraftName, "La5");
       break;
       }
   case 227:
       {
       sprintf (AircraftName, "La7");
       break;
       }
   case 228:
       {
       sprintf (AircraftName, "Il2");
       break;
       }
   case 229:
       {
       sprintf (AircraftName, "Macci 202");
       break;
       }
   case 230:
       {
       sprintf (AircraftName, "Lancaster");
       break;
       }
   case 231:
       {
       sprintf (AircraftName, "Mosquito");
       break;
       }
   case 232:
       {
       sprintf (AircraftName, "Typhoon");
       break;
       }
   case 233:
       {
       sprintf (AircraftName, "Yak1");
       break;
       }
   case 234:
       {
       sprintf (AircraftName, "B29");
       break;
       }
   case 235:
       {
       sprintf (AircraftName, "DW520");
       break;
       }
   case 236:
       {
       sprintf (AircraftName, "SB2C");
       break;
       }
   case 237:
       {
       sprintf (AircraftName, "TBF");
       break;
       }
   case 238:
       {
       sprintf (AircraftName, "ME163");
       break;
       }
   case 239:
       {
       sprintf (AircraftName, "Tempest");
       break;
       }
   case 240:
       {
       sprintf (AircraftName, "D3a");
       break;
       }
   case 241:
       {
       sprintf (AircraftName, "B5N");
       break;
       }
   case 242:
       {
       sprintf (AircraftName, "Dauntless");
       break;
       }
   case 243:
       {
       sprintf (AircraftName, "Me110");
       break;
       }
   case 244:
       {
       sprintf (AircraftName, "Dornier");
       break;
       }
   case 245:
       {
       sprintf (AircraftName, "He111");
       break;
       }
   case 246:
       {
       sprintf (AircraftName, "Ju88");
       break;
       }
   case 247:
       {
       sprintf (AircraftName, "KI84");
       break;
       }
   case 248:
       {
       sprintf (AircraftName, "KI61");
       break;
       }
   case 249:
       {
       sprintf (AircraftName, "Generic");
       break;
       }
   case 250:
       {
       sprintf (AircraftName, "A6M5");
       break;
       }
   case 251:
       {
       sprintf (AircraftName, "Spit5");
       break;
       }
   case 252:
       {
       sprintf (AircraftName, "P51B");
       break;
       }
   case 253:
       {
       sprintf (AircraftName, "P47B");
       break;
       }
   case 254:
       {
       sprintf (AircraftName, "Me109F");
       break;
       }
   case 255:
       {
       sprintf (AircraftName, "P38F");
       break;
       }
   default:
       {
       sprintf (AircraftName, "Unknown");
       }
   } 
sprintf (DebugBuf, "   Aircraft that shot you down was of type %s, ", AircraftName);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
if (InPacket.UdpObjPlayerNumber <= 10)
   {
   sprintf (DebugBuf, "an aircraft ");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   if (OutPacket.UdpObjDamageId%2 !=0)
      { 
      sprintf (DebugBuf, "of RedTeam.\n");
      fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
      }
   else
      { 
      sprintf (DebugBuf, "of BlueTeam.\n");
      fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
      }
   }
if (InPacket.UdpObjDamageId == MyNetworkId)
   { 
   sprintf (DebugBuf, "   Opponent altitude was = %5.0f.\n", (50.0 * (InPacket.UdpObjYPosition)) - 17.0);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Opponent velocity was = %5.0f.\n", InPacket.UdpObjSpeed / timestep * 50000.0F);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Opponent XZ Position was %f, %f.\n", InPacket.UdpObjXPosition, InPacket.UdpObjZPosition);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Opponent GammaPhiTheta orientation was %f, %f, %f.\n", InPacket.UdpObjGamma, InPacket.UdpObjPhi, InPacket.UdpObjTheta);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
sprintf (DebugBuf, "   Damage amount = %f.\n", InPacket.UdpObjDamageAmount);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   BlueHqAirfieldStatus=%5.0f \n", (ThreeDObjects[28]->Durability / ThreeDObjects[28]->maxDurability) * 100.0 );
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   RedHqAirfieldStatus=%5.0f \n", (ThreeDObjects[29]->Durability / ThreeDObjects[29]->maxDurability) * 100.0 );
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Fuel =%4.0f\n", fplayer->FuelLevel);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Ammo = %4d\n", fplayer->ammo);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Aircraft Damage State before this hit = %3.0f\n\n", 100 * (1-(fplayer->Durability / fplayer->maxDurability)));
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
} 

void UpdateOnlineScoreLogFileWithLandings()
{
if (!LandingLogged)
   {
   LandingLogged = true;
   display ((char*)"UpdateOnlineScoreLogFileWithNewLandings()\n", LOG_MOST);
   time_t now = time(NULL);
   struct tm *CurrentTimeStruct = gmtime(&now);
   strftime(DebugBuf, sizeof(DebugBuf)-1, "LANDED %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Realm=%d, ", Realm);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My Fuel =%4.0f\n", fplayer->FuelLevel);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My Aircraft Damage State = %3.0f\n\n", 100 * (1-(fplayer->Durability / fplayer->maxDurability)));
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
} 

void UpdateOnlineScoreLogFileWithNewSession()
{
display ((char*)"UpdateOnlineScoreLogFileWithNewSession()\n", LOG_MOST);
time_t now = time(NULL);
struct tm *CurrentTimeStruct = gmtime(&now);
strftime(DebugBuf, sizeof(DebugBuf)-1, "\n\nNEW SESSION %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
}

void UpdateOnlineScoreLogFileWithNewSorties()
{
display ((char*)"UpdateOnlineScoreLogFileWithNewSorties()\n", LOG_MOST);
char AircraftName[32]; 
time_t now = time(NULL);
struct tm *CurrentTimeStruct = gmtime(&now);
strftime(DebugBuf, sizeof(DebugBuf)-1, "NEW SORTIE %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   Realm=%d, ", Realm);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
switch (PlayerAircraftType)
   {
   case 201:
       {
       sprintf (AircraftName, "Hawk");
       break;
       }
   case 202:
       {
       sprintf (AircraftName, "Fiat G55");
       break;
       }
   case 203:
       {
       sprintf (AircraftName, "Me109G");
       break;
       }
   case 204:
       {
       sprintf (AircraftName, "P40");
       break;
       }
   case 205:
       {
       sprintf (AircraftName, "Hurricane");
       break;
       }
   case 206:
       {
       sprintf (AircraftName, "Ki43");
       break;
       }
   case 207:
       {
       sprintf (AircraftName, "Spit9");
       break;
       }
   case 208:
       {
       sprintf (AircraftName, "Polikarpov I16");
       break;
       }
   case 209:
       {
       sprintf (AircraftName, "Ju87");
       break;
       }
   case 210:
       {
       sprintf (AircraftName, "A6M2");
       break;
       }
   case 211:
       {
       sprintf (AircraftName, "F4u");
       break;
       }
   case 212:
       {
       sprintf (AircraftName, "F6F");
       break;
       }
   case 213:
       {
       sprintf (AircraftName, "P38L");
       break;
       }
   case 214:
       {
       sprintf (AircraftName, "P47D");
       break;
       }
   case 215:
       {
       sprintf (AircraftName, "P51D");
       break;
       }
   case 216:
       {
       sprintf (AircraftName, "B17");
       break;
       }
   case 217:
       {
       sprintf (AircraftName, "FW190");
       break;
       }
   case 218:
       {
       sprintf (AircraftName, "Yak9");
       break;
       }
   case 219:
       {
       sprintf (AircraftName, "N1K1");
       break;
       }
   case 220:
       {
       sprintf (AircraftName, "B24");
       break;
       }
   case 221:
       {
       sprintf (AircraftName, "P39");
       break;
       }
   case 222:
       {
       sprintf (AircraftName, "Mitsubishi G5M");
       break;
       }
   case 223:
       {
       sprintf (AircraftName, "B25");
       break;
       }
   case 224:
       {
       sprintf (AircraftName, "B26");
       break;
       }
   case 225:
       {
       sprintf (AircraftName, "F4F");
       break;
       }
   case 226:
       {
       sprintf (AircraftName, "La5");
       break;
       }
   case 227:
       {
       sprintf (AircraftName, "La7");
       break;
       }
   case 228:
       {
       sprintf (AircraftName, "Il2");
       break;
       }
   case 229:
       {
       sprintf (AircraftName, "Macci 202");
       break;
       }
   case 230:
       {
       sprintf (AircraftName, "Lancaster");
       break;
       }
   case 231:
       {
       sprintf (AircraftName, "Mosquito");
       break;
       }
   case 232:
       {
       sprintf (AircraftName, "Typhoon");
       break;
       }
   case 233:
       {
       sprintf (AircraftName, "Yak1");
       break;
       }
   case 234:
       {
       sprintf (AircraftName, "B29");
       break;
       }
   case 235:
       {
       sprintf (AircraftName, "DW520");
       break;
       }
   case 236:
       {
       sprintf (AircraftName, "SB2C");
       break;
       }
   case 237:
       {
       sprintf (AircraftName, "TBF");
       break;
       }
   case 238:
       {
       sprintf (AircraftName, "ME163");
       break;
       }
   case 239:
       {
       sprintf (AircraftName, "Tempest");
       break;
       }
   case 240:
       {
       sprintf (AircraftName, "D3a");
       break;
       }
   case 241:
       {
       sprintf (AircraftName, "B5N");
       break;
       }
   case 242:
       {
       sprintf (AircraftName, "Dauntless");
       break;
       }
   case 243:
       {
       sprintf (AircraftName, "Me110");
       break;
       }
   case 244:
       {
       sprintf (AircraftName, "Dornier");
       break;
       }
   case 245:
       {
       sprintf (AircraftName, "He111");
       break;
       }
   case 246:
       {
       sprintf (AircraftName, "Ju88");
       break;
       }
   case 247:
       {
       sprintf (AircraftName, "KI84");
       break;
       }
   case 248:
       {
       sprintf (AircraftName, "KI61");
       break;
       }
   case 249:
       {
       sprintf (AircraftName, "Generic");
       break;
       }
   case 250:
       {
       sprintf (AircraftName, "A6M5");
       break;
       }
   case 251:
       {
       sprintf (AircraftName, "Spit5");
       break;
       }
   case 252:
       {
       sprintf (AircraftName, "P51B");
       break;
       }
   case 253:
       {
       sprintf (AircraftName, "P47B");
       break;
       }
   case 254:
       {
       sprintf (AircraftName, "Me109F");
       break;
       }
   case 255:
       {
       sprintf (AircraftName, "P38F");
       break;
       }
   default:
       {
       sprintf (AircraftName, "Unknown");
       }
   }
sprintf (DebugBuf, "   MyAircraft=%s.\n\n", AircraftName);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
} 

void UpdateOnlineScoreLogFileWithTakeoffs()
{
if (!TakeoffLogged)
   {
   TakeoffLogged = true;
   display ((char*)"UpdateOnlineScoreLogFileWithTakeoffs()\n", LOG_MOST);
   time_t now = time(NULL);
   struct tm *CurrentTimeStruct = gmtime(&now);
   strftime(DebugBuf, sizeof(DebugBuf)-1, "TOOK OFF %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Realm=%d, ", Realm);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   My CommunityHandle=%s.\n\n", CommunityHandle);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
} 

void UpdateOnlineScoreLogFileWithVictories()
{
time_t now = time(NULL);
struct tm *CurrentTimeStruct = gmtime(&now);
char AircraftName[32]; 
if (RecentVictimXPosition == 0.0 && RecentVictimZPosition == 0.0 && OutPacket.UdpObjDamageId == 1)
   { 
   return;
   }

strftime(DebugBuf, sizeof(DebugBuf)-1, "VICTORY %d %m %Y %H:%M:%S GMT (DD MM YYYY HH:MM:SS)\n", CurrentTimeStruct);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "\n");
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   %s\n", VERSIONSTRING);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   Realm=%d, ", Realm);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "Mission=%d.\n", CurrentMissionNumber);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   MyNetworkId=%d.\n", MyNetworkId);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My CommunityHandle=%s.\n", CommunityHandle);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
switch (PlayerAircraftType)
   {
   case 201:
       {
       sprintf (AircraftName, "Hawk");
       break;
       }
   case 202:
       {
       sprintf (AircraftName, "Fiat G55");
       break;
       }
   case 203:
       {
       sprintf (AircraftName, "Me109G");
       break;
       }
   case 204:
       {
       sprintf (AircraftName, "P40");
       break;
       }
   case 205:
       {
       sprintf (AircraftName, "Hurricane");
       break;
       }
   case 206:
       {
       sprintf (AircraftName, "Ki43");
       break;
       }
   case 207:
       {
       sprintf (AircraftName, "Spit9");
       break;
       }
   case 208:
       {
       sprintf (AircraftName, "Polikarpov I16");
       break;
       }
   case 209:
       {
       sprintf (AircraftName, "Ju87");
       break;
       }
   case 210:
       {
       sprintf (AircraftName, "A6M2");
       break;
       }
   case 211:
       {
       sprintf (AircraftName, "F4u");
       break;
       }
   case 212:
       {
       sprintf (AircraftName, "F6F");
       break;
       }
   case 213:
       {
       sprintf (AircraftName, "P38L");
       break;
       }
   case 214:
       {
       sprintf (AircraftName, "P47D");
       break;
       }
   case 215:
       {
       sprintf (AircraftName, "P51D");
       break;
       }
   case 216:
       {
       sprintf (AircraftName, "B17");
       break;
       }
   case 217:
       {
       sprintf (AircraftName, "FW190");
       break;
       }
   case 218:
       {
       sprintf (AircraftName, "Yak9");
       break;
       }
   case 219:
       {
       sprintf (AircraftName, "N1K1");
       break;
       }
   case 220:
       {
       sprintf (AircraftName, "B24");
       break;
       }
   case 221:
       {
       sprintf (AircraftName, "P39");
       break;
       }
   case 222:
       {
       sprintf (AircraftName, "Mitsubishi G5M");
       break;
       }
   case 223:
       {
       sprintf (AircraftName, "B25");
       break;
       }
   case 224:
       {
       sprintf (AircraftName, "B26");
       break;
       }
   case 225:
       {
       sprintf (AircraftName, "F4F");
       break;
       }
   case 226:
       {
       sprintf (AircraftName, "La5");
       break;
       }
   case 227:
       {
       sprintf (AircraftName, "La7");
       break;
       }
   case 228:
       {
       sprintf (AircraftName, "Il2");
       break;
       }
   case 229:
       {
       sprintf (AircraftName, "Macci 202");
       break;
       }
   case 230:
       {
       sprintf (AircraftName, "Lancaster");
       break;
       }
   case 231:
       {
       sprintf (AircraftName, "Mosquito");
       break;
       }
   case 232:
       {
       sprintf (AircraftName, "Typhoon");
       break;
       }
   case 233:
       {
       sprintf (AircraftName, "Yak1");
       break;
       }
   case 234:
       {
       sprintf (AircraftName, "B29");
       break;
       }
   case 235:
       {
       sprintf (AircraftName, "DW520");
       break;
       }
   case 236:
       {
       sprintf (AircraftName, "SB2C");
       break;
       }
   case 237:
       {
       sprintf (AircraftName, "TBF");
       break;
       }
   case 238:
       {
       sprintf (AircraftName, "ME163");
       break;
       }
   case 239:
       {
       sprintf (AircraftName, "Tempest");
       break;
       }
   case 240:
       {
       sprintf (AircraftName, "D3a");
       break;
       }
   case 241:
       {
       sprintf (AircraftName, "B5N");
       break;
       }
   case 242:
       {
       sprintf (AircraftName, "Dauntless");
       break;
       }
   case 243:
       {
       sprintf (AircraftName, "Me110");
       break;
       }
   case 244:
       {
       sprintf (AircraftName, "Dornier");
       break;
       }
   case 245:
       {
       sprintf (AircraftName, "He111");
       break;
       }
   case 246:
       {
       sprintf (AircraftName, "Ju88");
       break;
       }
   case 247:
       {
       sprintf (AircraftName, "KI84");
       break;
       }
   case 248:
       {
       sprintf (AircraftName, "KI61");
       break;
       }
   case 249:
       {
       sprintf (AircraftName, "Generic");
       break;
       }
   case 250:
       {
       sprintf (AircraftName, "A6M5");
       break;
       }
   case 251:
       {
       sprintf (AircraftName, "Spit5");
       break;
       }
   case 252:
       {
       sprintf (AircraftName, "P51B");
       break;
       }
   case 253:
       {
       sprintf (AircraftName, "P47B");
       break;
       }
   case 254:
       {
       sprintf (AircraftName, "Me109F");
       break;
       }
   case 255:
       {
       sprintf (AircraftName, "P38F");
       break;
       }
   default:
       {
       sprintf (AircraftName, "Unknown");
       }
   }
sprintf (DebugBuf, "   MyAircraft=%s.\n", AircraftName);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Velocity=%f.\n", fplayer->InertiallyDampenedPlayerSpeed / timestep * 50000.0F);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My altitude in feet: %5.0f.\n", (50.0 * (fplayer->tl->y + SeaLevel )) - 17.0);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My XZ Position = %f, %f\n", fplayer->tl->x, fplayer->tl->z);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My GammaPhiTheta orientation = %f, %f, %f\n", fplayer->gamma, fplayer->phi, fplayer->theta);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   VictoryCount=%d.\n", fplayer->fighterkills);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   Mission player or facility destroyed or damaged = %d.\n", OutPacket.UdpObjDamageId);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
if (OutPacket.UdpObjDamageId <= 10)
   { 
   sprintf (DebugBuf, "   Victim's CommunityHandle = %s\n", PlayerIdStrings[OutPacket.UdpObjDamageId -1]);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
switch (RecentVictim)
   {
   case 201:
       {
       sprintf (AircraftName, "Hawk");
       break;
       }
   case 202:
       {
       sprintf (AircraftName, "Fiat G55");
       break;
       }
   case 203:
       {
       sprintf (AircraftName, "Me109G");
       break;
       }
   case 204:
       {
       sprintf (AircraftName, "P40");
       break;
       }
   case 205:
       {
       sprintf (AircraftName, "Hurricane");
       break;
       }
   case 206:
       {
       sprintf (AircraftName, "Ki43");
       break;
       }
   case 207:
       {
       sprintf (AircraftName, "Spit9");
       break;
       }
   case 208:
       {
       sprintf (AircraftName, "Polikarpov I16");
       break;
       }
   case 209:
       {
       sprintf (AircraftName, "Ju87");
       break;
       }
   case 210:
       {
       sprintf (AircraftName, "A6M2");
       break;
       }
   case 211:
       {
       sprintf (AircraftName, "F4u");
       break;
       }
   case 212:
       {
       sprintf (AircraftName, "F6F");
       break;
       }
   case 213:
       {
       sprintf (AircraftName, "P38L");
       break;
       }
   case 214:
       {
       sprintf (AircraftName, "P47D");
       break;
       }
   case 215:
       {
       sprintf (AircraftName, "P51D");
       break;
       }
   case 216:
       {
       sprintf (AircraftName, "B17");
       break;
       }
   case 217:
       {
       sprintf (AircraftName, "FW190");
       break;
       }
   case 218:
       {
       sprintf (AircraftName, "Yak9");
       break;
       }
   case 219:
       {
       sprintf (AircraftName, "N1K1");
       break;
       }
   case 220:
       {
       sprintf (AircraftName, "B24");
       break;
       }
   case 221:
       {
       sprintf (AircraftName, "P39");
       break;
       }
   case 222:
       {
       sprintf (AircraftName, "Mitsubishi G5M");
       break;
       }
   case 223:
       {
       sprintf (AircraftName, "B25");
       break;
       }
   case 224:
       {
       sprintf (AircraftName, "B26");
       break;
       }
   case 225:
       {
       sprintf (AircraftName, "F4F");
       break;
       }
   case 226:
       {
       sprintf (AircraftName, "La5");
       break;
       }
   case 227:
       {
       sprintf (AircraftName, "La7");
       break;
       }
   case 228:
       {
       sprintf (AircraftName, "Il2");
       break;
       }
   case 229:
       {
       sprintf (AircraftName, "Macci 202");
       break;
       }
   case 230:
       {
       sprintf (AircraftName, "Lancaster");
       break;
       }
   case 231:
       {
       sprintf (AircraftName, "Mosquito");
       break;
       }
   case 232:
       {
       sprintf (AircraftName, "Typhoon");
       break;
       }
   case 233:
       {
       sprintf (AircraftName, "Yak1");
       break;
       }
   case 234:
       {
       sprintf (AircraftName, "B29");
       break;
       }
   case 235:
       {
       sprintf (AircraftName, "DW520");
       break;
       }
   case 236:
       {
       sprintf (AircraftName, "SB2C");
       break;
       }
   case 237:
       {
       sprintf (AircraftName, "TBF");
       break;
       }
   case 238:
       {
       sprintf (AircraftName, "ME163");
       break;
       }
   case 239:
       {
       sprintf (AircraftName, "Tempest");
       break;
       }
   case 240:
       {
       sprintf (AircraftName, "D3a");
       break;
       }
   case 241:
       {
       sprintf (AircraftName, "B5N");
       break;
       }
   case 242:
       {
       sprintf (AircraftName, "Dauntless");
       break;
       }
   case 243:
       {
       sprintf (AircraftName, "Me110");
       break;
       }
   case 244:
       {
       sprintf (AircraftName, "Dornier");
       break;
       }
   case 245:
       {
       sprintf (AircraftName, "He111");
       break;
       }
   case 246:
       {
       sprintf (AircraftName, "Ju88");
       break;
       }
   case 247:
       {
       sprintf (AircraftName, "KI84");
       break;
       }
   case 248:
       {
       sprintf (AircraftName, "KI61");
       break;
       }
   case 249:
       {
       sprintf (AircraftName, "Generic");
       break;
       }
   case 250:
       {
       sprintf (AircraftName, "A6M5");
       break;
       }
   case 251:
       {
       sprintf (AircraftName, "Spit5");
       break;
       }
   case 252:
       {
       sprintf (AircraftName, "P51B");
       break;
       }
   case 253:
       {
       sprintf (AircraftName, "P47B");
       break;
       }
   case 254:
       {
       sprintf (AircraftName, "Me109F");
       break;
       }
   case 255:
       {
       sprintf (AircraftName, "P38F");
       break;
       }
   default:
       {
       sprintf (AircraftName, "Unknown");
       }
   }
sprintf (DebugBuf, "   Destroyed target was of type %s, ", AircraftName);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
if (OutPacket.UdpObjDamageId == 24)
   { 
   sprintf (DebugBuf, "BlueTeam RADAR.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
else if (OutPacket.UdpObjDamageId == 25)
   { 
   sprintf (DebugBuf, "RedTeam RADAR.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
else if (OutPacket.UdpObjDamageId == 26)
   { 
   sprintf (DebugBuf, "BlueTeam Dormitory.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
else if (OutPacket.UdpObjDamageId == 27)
   { 
   sprintf (DebugBuf, "RedTeam Dormitory.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
else if (OutPacket.UdpObjDamageId == 28)
   { 
   sprintf (DebugBuf, "BlueTeam Airfield.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
else if (OutPacket.UdpObjDamageId == 29)
   { 
   sprintf (DebugBuf, "RedTeam Airfield.\n");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
if (OutPacket.UdpObjDamageId <= 10)
   {
   sprintf (DebugBuf, "an aircraft ");
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   if (OutPacket.UdpObjDamageId%2 !=0)
      { 
      sprintf (DebugBuf, "of RedTeam.\n");
      fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
      }
   else
      { 
      sprintf (DebugBuf, "of BlueTeam.\n");
      fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
      }
   }
if (OutPacket.UdpObjDamageId <= 10)
   { 
   sprintf (DebugBuf, "   Target altitude was = %5.0f.\n", (50.0 * (RecentVictimAltitude + SeaLevel )) - 17.0);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Target velocity was = %5.0f.\n", RecentVictimVelocity / timestep * 50000.0F);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Target XZ Position was %f, %f.\n", RecentVictimXPosition, RecentVictimZPosition);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   sprintf (DebugBuf, "   Target GammaPhiTheta orientation was %f, %f, %f.\n", RecentVictimGamma, RecentVictimPhi, RecentVictimTheta);
   fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
   }
sprintf (DebugBuf, "   Damage amount = %f.\n", OutPacket.UdpObjDamageAmount);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   BlueHqAirfieldStatus=%5.0f\n", (ThreeDObjects[28]->Durability / ThreeDObjects[28]->maxDurability) * 100.0 );
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   RedHqAirfieldStatus=%5.0f\n", (ThreeDObjects[29]->Durability / ThreeDObjects[29]->maxDurability) * 100.0 );
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Fuel =%4.0f\n", fplayer->FuelLevel);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Ammo = %4d\n", fplayer->ammo);
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
sprintf (DebugBuf, "   My Aircraft Damage State = %3.0f\n\n", 100 * (1-(fplayer->Durability / fplayer->maxDurability)));
fwrite (DebugBuf, strlen(DebugBuf), 1, OnlineScoreLogFile);
} 

void viewParameters ()
    {
    
    display ((char *)" ", LOG_NONE);
    display ((char *)"Usage: LacNNpNN [-h -m -v -dLEVEL]", LOG_NONE);
    display ((char *)" ", LOG_NONE);
    display ((char *)"-h: Display this help screen and quit", LOG_NONE);
    display ((char *)"-m: Request Mouse control instead of joystick.", LOG_NONE);
    display ((char *)"-v: Display version string and quit", LOG_NONE);
    display ((char *)"-dLEVEL: Set debug LEVEL to 0=silent... 4= show Network-related logs... 5=log all", LOG_NONE);
    display ((char *)"  ", LOG_NONE);
    display ((char *)" For help configuring joystick and keyboard,", LOG_NONE);
    display ((char *)" we recommend avoiding full-screen mode so you", LOG_NONE);
    display ((char *)" can see status messages, and running at debug level 4.", LOG_NONE);
    display ((char *)" Status messages in your main window will then show", LOG_NONE);
    display ((char *)" the value of keypresses and joystick buttons as they", LOG_NONE);
    display ((char *)" are detected during the game. Use those values as you", LOG_NONE);
    display ((char *)" edit the file named LacControls.txt which you will", LOG_NONE);
    display ((char *)" find in your home folder's .LAC directory. Edit that", LOG_NONE);
    display ((char *)" file with a simple text editor. Retain the general", LOG_NONE);
    display ((char *)" format of the file while changing the numeric values", LOG_NONE);
    display ((char *)" associated with keyboard key commands or joystick button", LOG_NONE);
    display ((char *)" commands. This will allow you to associate any keyboard", LOG_NONE);
    display ((char *)" key or any joystick button with any supported flight function.", LOG_NONE);
    display ((char *)" ", LOG_NONE);
    } 

void VocalizeBlueHqStatus ()
{
display ((char*)"VocalizeBlueHqStatus()", LOG_MOST);
 if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.1)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM01TO10PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM01TO10PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.2)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM10TO20PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM10TO20PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.3)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM20TO30PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM20TO30PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.4)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM30TO40PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM30TO40PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.5)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM40TO50PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM40TO50PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.6)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM50TO60PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM50TO60PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.7)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM60TO70PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM60TO70PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.8)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM70TO80PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM70TO80PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability * 0.9)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM80TO90PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM80TO90PCT, false);
    return;
    }
 else if (ThreeDObjects[28]->Durability <= ThreeDObjects[28]->maxDurability)
    { 
    display ((char*)"VocalizeBlueHqStatus() SOUND_ENEMYHQACKACKFROM90TO100PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM90TO100PCT, false);
    }
}

void VocalizeMorseCharacter (unsigned char MorseChar)
{
if (MorseChar == '0')
   {
   sound->play (SOUND_MORSE0000, false);
   }
else if (MorseChar == '1')
   {
   sound->play (SOUND_MORSE0100, false);
   }
else if (MorseChar == '2')
   {
   sound->play (SOUND_MORSE0200, false);
   }
else if (MorseChar == '3')
   {
   sound->play (SOUND_MORSE0300, false);
   }
else if (MorseChar == '4')
   {
   sound->play (SOUND_MORSE0400, false);
   }
else if (MorseChar == '5')
   {
   sound->play (SOUND_MORSE0500, false);
   }
else if (MorseChar == '6')
   {
   sound->play (SOUND_MORSE0600, false);
   }
else if (MorseChar == '7')
   {
   sound->play (SOUND_MORSE0700, false);
   }
else if (MorseChar == '8')
   {
   sound->play (SOUND_MORSE0800, false);
   }
else if (MorseChar == '9')
   {
   sound->play (SOUND_MORSE0900, false);
   }
else if (MorseChar == 'A' || MorseChar == 'a')
   {
   sound->play (SOUND_MORSEA00, false);
   }
else if (MorseChar == 'B' || MorseChar == 'b')
   {
   sound->play (SOUND_MORSEB00, false);
   }
else if (MorseChar == 'C' || MorseChar == 'c')
   {
   sound->play (SOUND_MORSEC00, false);
   }
else if (MorseChar == 'D' || MorseChar == 'd')
   {
   sound->play (SOUND_MORSED00, false);
   }
else if (MorseChar == 'E' || MorseChar == 'e')
   {
   sound->play (SOUND_MORSEE00, false);
   }
else if (MorseChar == 'F' || MorseChar == 'f')
   {
   sound->play (SOUND_MORSEF00, false);
   }
else if (MorseChar == 'G' || MorseChar == 'g')
   {
   sound->play (SOUND_MORSEG00, false);
   }
else if (MorseChar == 'H' || MorseChar == 'h')
   {
   sound->play (SOUND_MORSEH00, false);
   }
else if (MorseChar == 'I' || MorseChar == 'i')
   {
   sound->play (SOUND_MORSEI00, false);
   }
else if (MorseChar == 'J' || MorseChar == 'j')
   {
   sound->play (SOUND_MORSEJ00, false);
   }
else if (MorseChar == 'K' || MorseChar == 'k')
   {
   sound->play (SOUND_MORSEK00, false);
   }
else if (MorseChar == 'L' || MorseChar == 'l')
   {
   sound->play (SOUND_MORSEL00, false);
   }
else if (MorseChar == 'M' || MorseChar == 'm')
   {
   sound->play (SOUND_MORSEM00, false);
   }
else if (MorseChar == 'N' || MorseChar == 'n')
   {
   sound->play (SOUND_MORSEN00, false);
   }
else if (MorseChar == 'O' || MorseChar == 'o')
   {
   sound->play (SOUND_MORSEO00, false);
   }
else if (MorseChar == 'P' || MorseChar == 'p')
   {
   sound->play (SOUND_MORSEP00, false);
   }
else if (MorseChar == 'Q' || MorseChar == 'q')
   {
   sound->play (SOUND_MORSEQ00, false);
   }
else if (MorseChar == 'R' || MorseChar == 'r')
   {
   sound->play (SOUND_MORSER00, false);
   }
else if (MorseChar == 'S' || MorseChar == 's')
   {
   sound->play (SOUND_MORSES00, false);
   }
else if (MorseChar == 'T' || MorseChar == 't')
   {
   sound->play (SOUND_MORSET00, false);
   }
else if (MorseChar == 'U' || MorseChar == 'u')
   {
   sound->play (SOUND_MORSEU00, false);
   }
else if (MorseChar == 'V' || MorseChar == 'v')
   {
   sound->play (SOUND_MORSEV00, false);
   }
else if (MorseChar == 'W' || MorseChar == 'w')
   {
   sound->play (SOUND_MORSEW00, false);
   }
else if (MorseChar == 'X' || MorseChar == 'x')
   {
   sound->play (SOUND_MORSEX00, false);
   }
else if (MorseChar == 'Y' || MorseChar == 'y')
   {
   sound->play (SOUND_MORSEY00, false);
   }
else if (MorseChar == 'Z' || MorseChar == 'z')
   {
   sound->play (SOUND_MORSEZ00, false);
   }
else if (MorseChar == ',')
   {
   sound->play (SOUND_XMORSECOMMA00, false);
   }
else if (MorseChar == '.')
   {
   sound->play (SOUND_XMORSEPERIOD00, false);
   }
else if (MorseChar == '?')
   {
   sound->play (SOUND_XMORSEQUESTION00, false);
   }
else if (MorseChar == '/')
   {
   sound->play (SOUND_XMORSESLASH00, false);
   }
}

void VocalizeRedHqStatus ()
{
display ((char*)"VocalizeRedHqStatus()", LOG_MOST);
 if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.1)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM01TO10PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM01TO10PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.2)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM10TO20PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM10TO20PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.3)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM20TO30PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM20TO30PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.4)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM30TO40PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM30TO40PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.5)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM40TO50PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM40TO50PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.6)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM50TO60PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM50TO60PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.7)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM60TO70PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM60TO70PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.8)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM70TO80PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM70TO80PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability * 0.9)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM80TO90PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM80TO90PCT, false);
    return;
    }
 else if (ThreeDObjects[29]->Durability <= ThreeDObjects[29]->maxDurability)
    { 
    display ((char*)"VocalizeRedHqStatus() SOUND_ENEMYHQACKACKFROM90TO100PCT", LOG_MOST);
    sound->play (SOUND_ENEMYHQACKACKFROM90TO100PCT, false);
    }
}

