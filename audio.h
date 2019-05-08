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

/* This file includes all SDL and SDL_mixer audio processing: sound and music */

#ifndef IS_AUDIO_H
#define IS_AUDIO_H

#include "common.h" // ok

class WaveFile
{
  public:
  SDL_AudioSpec spec;
  Uint8 *sound;
  Uint32 soundlen;
  int soundpos;
  Mix_Chunk *chunk;
  int channel;
  int volume;

  WaveFile ();
  WaveFile (char *filename);
  ~WaveFile ();
  void load (char *filename);
  void play (int channel, bool loop);
  void stop ();
  void setVolume (int level);
};

#define SOUND_EXPLOSION1 1000
#define SOUND_CLICK1 1
#define SOUND_CANNON1 2
#define SOUND_BOMB01 3
#define SOUND_WINDNOISE 4
#define SOUND_PLANE1 5
#define SOUND_HIT1 6
#define SOUND_BEEP1 7
#define SOUND_CHAFF1 8
#define SOUND_BEEP2 9
#define SOUND_FLAPSDN 10
#define SOUND_FLAPSUP 11
#define SOUND_SPEEDBRAKE 12
#define SOUND_UNDERCARRIAGE 13
#define SOUND_SPEEDBRAKE1 14
#define SOUND_PLANE2 15
#define SOUND_NEWBANDITONRADARO1 16
#define SOUND_RADARBANDITDISAPPEARED01 17
#define SOUND_TIRESQUEAK1 18
#define SOUND_DAMAGE00 19
#define SOUND_CRASHOCEAN00 20
#define SOUND_BELLYSCRAPE00 21
#define SOUND_WHEELROLLOUT00 22
#define SOUND_DIGIT001 23
#define SOUND_DIGIT002 24
#define SOUND_DIGIT003 25
#define SOUND_DIGIT004 26
#define SOUND_DIGIT005 27
#define SOUND_DIGIT006 28
#define SOUND_DIGIT007 29
#define SOUND_DIGIT008 30
#define SOUND_DIGIT009 31
#define SOUND_DIGIT010 32
#define SOUND_DIGIT011 33
#define SOUND_DIGIT012 34
#define SOUND_DIGIT013 35
#define SOUND_DIGIT014 36
#define SOUND_DIGIT015 37
#define SOUND_DIGIT016 38
#define SOUND_DIGIT017 39
#define SOUND_DIGIT018 40
#define SOUND_DIGIT019 41
#define SOUND_DIGIT020 42
#define SOUND_DIGIT030 43
#define SOUND_TARGET   44
#define SOUND_IsAt     45
#define SOUND_Angels   46
#define SOUND_Oclock   47
#define SOUND_DIGIT000 48
#define SOUND_VOICECOMMSMUMBLE01 49
#define SOUND_BLUE 50
#define SOUND_BOSENETDOTNODASHIPDOTBIZ 51
#define SOUND_CHANNEL 52
#define SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE 53
#define SOUND_LINUXAIRCOMBAT 54
#define SOUND_MISSIONCOMMSONCHANNEL 55
#define SOUND_MISSIONNETWORKBATTLE01 56
#define SOUND_MISSIONNETWORKBATTLE02 57
#define SOUND_MISSIONNETWORKBATTLE03 58
#define SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT 59
#define SOUND_RED 60
#define SOUND_SQUADCOMMSONSUBCHANNEL 61
#define SOUND_WEAPONSFREE 62
#define SOUND_YOUHAVEAUTHORITYTOPROCEED 63
#define SOUND_MISSIONENDINGINSTRUMENTAL 64
#define SOUND_BLUETEAMHASWONBATTLE 65
#define SOUND_REDTEAMHASWONBATTLE 66
#define SOUND_MISSIONENDINGIN15SECCOUNTDOWN 67
#define SOUND_ENEMYHQSTATUS100A 68
#define SOUND_ENEMYHQACKACKFROM01TO10PCT 69
#define SOUND_ENEMYHQACKACKFROM10TO20PCT 70
#define SOUND_ENEMYHQACKACKFROM20TO30PCT 71
#define SOUND_ENEMYHQACKACKFROM30TO40PCT 72
#define SOUND_ENEMYHQACKACKFROM40TO50PCT 73
#define SOUND_ENEMYHQACKACKFROM50TO60PCT 74
#define SOUND_ENEMYHQACKACKFROM60TO70PCT 75
#define SOUND_ENEMYHQACKACKFROM70TO80PCT 76
#define SOUND_ENEMYHQACKACKFROM80TO90PCT 77
#define SOUND_ENEMYHQACKACKFROM90TO100PCT 78
#define SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY 79
#define SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY 80
#define SOUND_ENEMYHQREPAIRSSTOPPED 81
#define SOUND_MORSE0000 83
#define SOUND_MORSE0100 84
#define SOUND_MORSE0200 85
#define SOUND_MORSE0300 86
#define SOUND_MORSE0400 87
#define SOUND_MORSE0500 88
#define SOUND_MORSE0600 89
#define SOUND_MORSE0700 90
#define SOUND_MORSE0800 91
#define SOUND_MORSE0900 92
#define SOUND_MORSEA00 93
#define SOUND_MORSEB00 94
#define SOUND_MORSEC00 95
#define SOUND_MORSED00 96
#define SOUND_MORSEE00 97
#define SOUND_MORSEF00 98
#define SOUND_MORSEG00 99
#define SOUND_MORSEH00 100
#define SOUND_MORSEI00 101
#define SOUND_MORSEJ00 102
#define SOUND_MORSEK00 103
#define SOUND_MORSEL00 104
#define SOUND_MORSEM00 105
#define SOUND_MORSEN00 106
#define SOUND_MORSEO00 107
#define SOUND_MORSEP00 108
#define SOUND_MORSEQ00 109
#define SOUND_MORSER00 110
#define SOUND_MORSES00 111
#define SOUND_MORSET00 112
#define SOUND_MORSEU00 113
#define SOUND_MORSEV00 114
#define SOUND_MORSEW00 115
#define SOUND_MORSEX00 116
#define SOUND_MORSEY00 117
#define SOUND_MORSEZ00 118
#define SOUND_XMORSECOMMA00 119
#define SOUND_XMORSEPERIOD00 120
#define SOUND_XMORSEQUESTION00 121
#define SOUND_XMORSESLASH00 122
#define SOUND_AIRRAIDSIREN 123
#define SOUND_DEFEAT00 124
#define SOUND_FIVEBEEPS00 125
#define SOUND_LACSTARTUP00 126

#define MUSIC_ANDCOMBAT 0
#define MUSIC_MUSIC00 1
#define MUSIC_COMBAT00 2
#define MUSIC_FURBALL 3
#define MUSIC_MUSIC01 4
#define MUSIC_MUSIC02 5
#define MUSIC_MUSIC03 6
#define MUSIC_MUSIC02ANDFURBALLAA 7
#define MUSIC_TUTORIAL01 8
#define MUSIC_TUTORIAL02 9
#define MUSIC_TUTORIAL03 11
#define MUSIC_MISSIONNETWORKBATTLE01 23
#define MUSIC_MISSIONNETWORKBATTLE02 24
#define MUSIC_MISSIONNETWORKBATTLE03 25
#define MUSIC_HEADTOHEAD 29

class SoundSystem
{
  public:
  bool audio;
  bool sound, music;
  bool musicplaying;
  int volumesound, volumemusic;
  int engine;
  Mix_Music *music1;
  WaveFile *waveexplosion1, *waveclick1, *wavecannon1, *wavebomb01;
  WaveFile *waveWindNoise, *waveplane [10], *wavehit1, *wavebeep1;
  WaveFile *wavechaff1, *wavebeep2, *waveflapsdn, *waveflapsup;
  WaveFile *wavespeedbrake, *wavespeedbrake1, *waveundercarriage;
  WaveFile *wavetiresqueak1, *waveNewBanditOnRadar01, *wavebellyscrape00;
  WaveFile *waveRadarBanditDisappeared01, *wavedamage00, *wavecrashocean00;
  WaveFile *wavewheelrollout00;
  WaveFile *wavedigit000;
  WaveFile *wavedigit001;
  WaveFile *wavedigit002;
  WaveFile *wavedigit003;
  WaveFile *wavedigit004;
  WaveFile *wavedigit005;
  WaveFile *wavedigit006;
  WaveFile *wavedigit007;
  WaveFile *wavedigit008;
  WaveFile *wavedigit009;
  WaveFile *wavedigit010;
  WaveFile *wavedigit011;
  WaveFile *wavedigit012;
  WaveFile *wavedigit013;
  WaveFile *wavedigit014;
  WaveFile *wavedigit015;
  WaveFile *wavedigit016;
  WaveFile *wavedigit017;
  WaveFile *wavedigit018;
  WaveFile *wavedigit019;
  WaveFile *wavedigit020;
  WaveFile *wavedigit030;
  WaveFile *waveBlue;
  WaveFile *waveBoseNetDotNoDashIpDotBiz;
  WaveFile *waveChannel;
  WaveFile *waveGlobalAssignmentAudioAtMumble;
  WaveFile *waveLinuxAirCombat;
  WaveFile *waveMissionCommsOnChannel;
  WaveFile *waveMissionNetworkBattle01;
  WaveFile *waveMissionNetworkBattle02;
  WaveFile *waveMissionNetworkBattle03;
  WaveFile *waveMissionTelemetryAvailableFromLacServerAt;
  WaveFile *waveRed;
  WaveFile *waveSquadCommsOnSubchannel;
  WaveFile *waveWeaponsFree;
  WaveFile *waveYouHaveAuthorityToProceed;
  WaveFile *waveTarget;
  WaveFile *waveIsAt;
  WaveFile *waveAngels;
  WaveFile *waveOclock;
  WaveFile *waveVoiceCommsZello1;
  WaveFile *waveMissionEndingInstrumental;
  WaveFile *waveBlueTeamHasWonBattle;
  WaveFile *waveRedTeamHasWonBattle;
  WaveFile *waveMissionEndingIn15SecCountdown;
  WaveFile *waveEnemyHqStatus100a;
  WaveFile *AirRaidSiren;
  WaveFile *Defeat00;
  WaveFile *EnemyHqAckAckFrom01to10Pct;
  WaveFile *EnemyHqAckAckFrom10to20Pct;
  WaveFile *EnemyHqAckAckFrom20to30Pct;
  WaveFile *EnemyHqAckAckFrom30to40Pct;
  WaveFile *EnemyHqAckAckFrom40to50Pct;
  WaveFile *EnemyHqAckAckFrom50to60Pct;
  WaveFile *EnemyHqAckAckFrom60to70Pct;
  WaveFile *EnemyHqAckAckFrom70to80Pct;
  WaveFile *EnemyHqAckAckFrom80to90Pct;
  WaveFile *EnemyHqAckAckFrom90to100Pct;
  WaveFile *EnemyHqAirfieldStatus100Percent;
  WaveFile *EnemyHqRepairsProceedingNormally;
  WaveFile *EnemyHqRepairsProceedingRapidly;
  WaveFile *EnemyHqRepairsStopped;
  WaveFile *FiveBeeps00;
  WaveFile *LacStartup00;
  WaveFile *Morse0000, *Morse0100, *Morse0200, *Morse0300, *Morse0400;
  WaveFile *Morse0500, *Morse0600, *Morse0700, *Morse0800, *Morse0900;
  WaveFile *MorseA00;
  WaveFile *MorseB00;
  WaveFile *MorseC00;
  WaveFile *MorseD00;
  WaveFile *MorseE00;
  WaveFile *MorseF00;
  WaveFile *MorseG00;
  WaveFile *MorseH00;
  WaveFile *MorseI00;
  WaveFile *MorseJ00;
  WaveFile *MorseK00;
  WaveFile *MorseL00;
  WaveFile *MorseM00;
  WaveFile *MorseN00;
  WaveFile *MorseO00;
  WaveFile *MorseP00;
  WaveFile *MorseQ00;
  WaveFile *MorseR00;
  WaveFile *MorseS00;
  WaveFile *MorseT00;
  WaveFile *MorseU00;
  WaveFile *MorseV00;
  WaveFile *MorseW00;
  WaveFile *MorseX00;
  WaveFile *MorseY00;
  WaveFile *MorseZ00;
  WaveFile *XMorseComma00;
  WaveFile *XMorsePeriod00;
  WaveFile *XMorseQuestion00;
  WaveFile *XMorseSlash00;

  SoundSystem ();
  ~SoundSystem ();
  void play (int sample, bool loop);
  void playLoop (int sample);
  void setPosition (int sample, int angle, int dist);
  void stop (int sample);
  void stopAll ();
  void playMusic ();
  void playMusic (int times);
  void loadMusic (int music);
  void haltMusic ();
  void setVolume (int sample, int level);
  void setVolume ();
  void setVolumeMusic ();
};

#endif
