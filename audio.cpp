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

#include "audio.h"
#include "dirs.h"

extern char DebugBuf[];

WaveFile *wave = NULL;


WaveFile::WaveFile ()
    {
    channel = -1;
    } //@ end WaveFile::WaveFile ()


WaveFile::WaveFile (char *filename)
    {
    channel = -1;
    volume = 100;
    load (filename);
    } //@ end WaveFile::WaveFile (char *filename)


WaveFile::~WaveFile () {}


void WaveFile::load (char *filename)
    {
    display ((char *)"WaveFile::load (char *filename)", LOG_MOST);
    sprintf (DebugBuf, "%s", filename);
    display (DebugBuf, LOG_MOST);
    char buf [STDSIZE];

    chunk = Mix_LoadWAV (filename);

    if (chunk == NULL)
        {
        sprintf (buf, "SDL_Mixer: %s", Mix_GetError ());
        display (buf, LOG_FATAL);
        exit (EXIT_LOADFILE);
        }
    } //@ end void WaveFile::load (char *filename)


void WaveFile::play (int chan, bool loop)
    {
    if (!loop)
        {
        channel = Mix_PlayChannel (chan, chunk, 0);
        }
    else
        {
        channel = Mix_PlayChannel (chan, chunk, -1);
        }
    } //@ end void WaveFile::play (int chan, bool loop)


void WaveFile::stop ()
    {
    if (channel < 0)
        {
        return;
        }

    if (Mix_Playing (channel))
        {
        Mix_HaltChannel (channel);
        }
    } //@ end void WaveFile::stop ()


void WaveFile::setVolume (int level)
    {
    if (abs (volume - level) <= 5)
        {
        return;
        }
    volume = level;
    bool pause = false;
    if (channel >= 0)
        {
        if (Mix_Playing (channel))
            {
            Mix_Pause (channel);
            pause = true;
            }
        }
    Mix_VolumeChunk (chunk, level);
    if (pause && channel >= 0)
        {
        Mix_Resume (channel);
        }
    } //@ end void WaveFile::setVolume (int level)


SoundSystem::SoundSystem ()
    {
    audio = false;
    sound = true;
    music = true;
    musicplaying = false;
    volumesound = 100;
    volumemusic = 100;
    char buf [STDSIZE];
    display ((char *)"SoundSystem::SoundSystem() Using SDL_mixer", LOG_MOST);
    if (Mix_OpenAudio (22050, AUDIO_S16, 2, 4096))
        {
        display ((char *)"Unable to open audio device", LOG_ERROR);
        audio = false;
        return;
        }
    else
        {
        audio = true;
        }
    music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music00.wav"));
    if (music1 == NULL)
        {
        sprintf (buf, "Cannot open Music00.wav: %s", Mix_GetError ());
        display (buf, LOG_FATAL);
        exit (EXIT_LOADFILE);
        }
    Mix_FreeMusic (music1);
    music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music00.wav"));
    if (music1 == NULL)
        {
        sprintf (buf, "Cannot open Music00.wav: %s", Mix_GetError ());
        display (buf, LOG_FATAL);
        exit (EXIT_LOADFILE);
        }
    int i;
    waveexplosion1 = new WaveFile (dirs->getSounds ((char *)"explode1.wav"));
    waveclick1 = new WaveFile (dirs->getSounds ((char *)"click1.wav"));
    wavecannon1 = new WaveFile (dirs->getSounds ((char *)"cannon1.wav"));
    wavebomb01 = new WaveFile (dirs->getSounds ((char *)"bomb01.wav"));
    waveWindNoise = new WaveFile (dirs->getSounds ((char *)"WindNoise.wav"));
    for (i = 0; i < 2; i ++)
        {
        sprintf (buf, "plane%d.wav", i + 1);
        waveplane [i] = new WaveFile (dirs->getSounds (buf));
        }
    engine = 0;
    wavehit1 = new WaveFile (dirs->getSounds ((char *)"hit1.wav"));
    wavebeep1 = new WaveFile (dirs->getSounds ((char *)"beep1.wav"));
    wavechaff1 = new WaveFile (dirs->getSounds ((char *)"chaff1.wav"));
    wavebeep2 = new WaveFile (dirs->getSounds ((char *)"beep2.wav"));
    wavecannon1->setVolume (50);
    wavebomb01->setVolume (110);
    waveWindNoise->setVolume (1);
    waveclick1->setVolume (80);
    waveflapsdn = new WaveFile (dirs->getSounds ((char *)"flapsdn.wav"));
    waveflapsdn->setVolume (80);
    waveflapsup = new WaveFile (dirs->getSounds ((char *)"flapsup.wav"));
    waveflapsup->setVolume (80);
    wavespeedbrake = new WaveFile (dirs->getSounds ((char *)"speedbrake.wav"));
    wavespeedbrake->setVolume (80);
    wavespeedbrake1 = new WaveFile (dirs->getSounds ((char *)"speedbrake1.wav"));
    wavespeedbrake1->setVolume (80);
    wavetiresqueak1 = new WaveFile (dirs->getSounds ((char *)"TireSqueak00.wav"));
    wavetiresqueak1->setVolume (80);
    wavedamage00 = new WaveFile (dirs->getSounds ((char *)"Damage00.wav"));
    wavedamage00->setVolume (80);
    wavecrashocean00 = new WaveFile (dirs->getSounds ((char *)"CrashOcean00.wav"));
    wavecrashocean00->setVolume (80);
    wavebellyscrape00 = new WaveFile (dirs->getSounds ((char *)"BellyScrape00.wav"));
    wavebellyscrape00->setVolume (80);
    wavewheelrollout00 = new WaveFile (dirs->getSounds ((char *)"WheelRollout00.wav"));
    wavewheelrollout00->setVolume (80);
    wavedigit000 = new WaveFile (dirs->getSounds ((char *)"000.wav"));
    wavedigit000->setVolume (80);
    wavedigit001 = new WaveFile (dirs->getSounds ((char *)"001.wav"));
    wavedigit001->setVolume (80);
    wavedigit002 = new WaveFile (dirs->getSounds ((char *)"002.wav"));
    wavedigit002->setVolume (80);
    wavedigit003 = new WaveFile (dirs->getSounds ((char *)"003.wav"));
    wavedigit003->setVolume (80);
    wavedigit004 = new WaveFile (dirs->getSounds ((char *)"004.wav"));
    wavedigit004->setVolume (80);
    wavedigit005 = new WaveFile (dirs->getSounds ((char *)"005.wav"));
    wavedigit005->setVolume (80);
    wavedigit006 = new WaveFile (dirs->getSounds ((char *)"006.wav"));
    wavedigit006->setVolume (80);
    wavedigit007 = new WaveFile (dirs->getSounds ((char *)"007.wav"));
    wavedigit007->setVolume (80);
    wavedigit008 = new WaveFile (dirs->getSounds ((char *)"008.wav"));
    wavedigit008->setVolume (80);
    wavedigit009 = new WaveFile (dirs->getSounds ((char *)"009.wav"));
    wavedigit009->setVolume (80);
    wavedigit010 = new WaveFile (dirs->getSounds ((char *)"010.wav"));
    wavedigit010->setVolume (80);
    wavedigit011 = new WaveFile (dirs->getSounds ((char *)"011.wav"));
    wavedigit011->setVolume (80);
    wavedigit012 = new WaveFile (dirs->getSounds ((char *)"012.wav"));
    wavedigit012->setVolume (80);
    wavedigit013 = new WaveFile (dirs->getSounds ((char *)"013.wav"));
    wavedigit013->setVolume (80);
    wavedigit014 = new WaveFile (dirs->getSounds ((char *)"014.wav"));
    wavedigit014->setVolume (80);
    wavedigit015 = new WaveFile (dirs->getSounds ((char *)"015.wav"));
    wavedigit015->setVolume (80);
    wavedigit016 = new WaveFile (dirs->getSounds ((char *)"016.wav"));
    wavedigit016->setVolume (80);
    wavedigit017 = new WaveFile (dirs->getSounds ((char *)"017.wav"));
    wavedigit017->setVolume (80);
    wavedigit018 = new WaveFile (dirs->getSounds ((char *)"018.wav"));
    wavedigit018->setVolume (80);
    wavedigit019 = new WaveFile (dirs->getSounds ((char *)"019.wav"));
    wavedigit019->setVolume (80);
    wavedigit020 = new WaveFile (dirs->getSounds ((char *)"020.wav"));
    wavedigit020->setVolume (80);
    wavedigit030 = new WaveFile (dirs->getSounds ((char *)"030.wav"));
    wavedigit030->setVolume (80);
    waveBlue = new WaveFile (dirs->getSounds ((char *)"Blue.wav"));
    waveBlue->setVolume (80);
    waveBoseNetDotNoDashIpDotBiz = new WaveFile (dirs->getSounds ((char *)"BoseNetDotNoDashIpDotBiz.wav"));
    waveBoseNetDotNoDashIpDotBiz->setVolume (80);
    waveChannel = new WaveFile (dirs->getSounds ((char *)"Channel.wav"));
    waveChannel->setVolume (80);
    waveGlobalAssignmentAudioAtMumble = new WaveFile (dirs->getSounds ((char *)"GlobalAssignmentAudioAtMumble.wav"));
    waveGlobalAssignmentAudioAtMumble->setVolume (80);
    waveLinuxAirCombat = new WaveFile (dirs->getSounds ((char *)"LinuxAirCombat.wav"));
    waveLinuxAirCombat->setVolume (80);
    waveMissionCommsOnChannel = new WaveFile (dirs->getSounds ((char *)"MissionCommsOnChannel.wav"));
    waveMissionCommsOnChannel->setVolume (80);
    waveMissionNetworkBattle01 = new WaveFile (dirs->getSounds ((char *)"MissionNetworkBattle01.wav"));
    waveMissionNetworkBattle01->setVolume (80);
    waveMissionNetworkBattle02 = new WaveFile (dirs->getSounds ((char *)"MissionNetworkBattle02.wav"));
    waveMissionNetworkBattle02->setVolume (80);
    waveMissionNetworkBattle03 = new WaveFile (dirs->getSounds ((char *)"MissionNetworkBattle03.wav"));
    waveMissionNetworkBattle03->setVolume (80);
    waveMissionTelemetryAvailableFromLacServerAt = new WaveFile (dirs->getSounds ((char *)"MissionTelemetryAvailableFromLacServerAt.wav"));
    waveMissionTelemetryAvailableFromLacServerAt->setVolume (80);
    waveRed = new WaveFile (dirs->getSounds ((char *)"Red.wav"));
    waveRed->setVolume (80);
    waveSquadCommsOnSubchannel = new WaveFile (dirs->getSounds ((char *)"SquadCommsOnSubchannel.wav"));
    waveSquadCommsOnSubchannel->setVolume (80);
    waveWeaponsFree = new WaveFile (dirs->getSounds ((char *)"WeaponsFree.wav"));
    waveWeaponsFree->setVolume (80);
    waveYouHaveAuthorityToProceed = new WaveFile (dirs->getSounds ((char *)"YouHaveAuthorityToProceed.wav"));
    waveYouHaveAuthorityToProceed->setVolume (80);
    waveundercarriage = new WaveFile (dirs->getSounds ((char *)"undercarriage.wav"));
    waveundercarriage->setVolume (80);
    waveNewBanditOnRadar01 = new WaveFile (dirs->getSounds ((char *)"NewBanditOnRadar01.wav"));
    waveNewBanditOnRadar01->setVolume (80);
    waveRadarBanditDisappeared01 = new WaveFile (dirs->getSounds ((char *)"RadarBanditDisappeared01.wav"));
    waveRadarBanditDisappeared01->setVolume (80);
    waveTarget = new WaveFile (dirs->getSounds ((char *)"Target.wav"));
    waveTarget->setVolume (80);
    waveIsAt = new WaveFile (dirs->getSounds ((char *)"IsAt.wav"));
    waveIsAt->setVolume (80);
    waveAngels = new WaveFile (dirs->getSounds ((char *)"Angels.wav"));
    waveAngels->setVolume (80);
    waveOclock = new WaveFile (dirs->getSounds ((char *)"Oclock.wav"));
    waveOclock->setVolume (80);
    waveVoiceCommsZello1 = new WaveFile (dirs->getSounds ((char *)"VoiceCommsMumble01.wav"));
    waveVoiceCommsZello1->setVolume (80);
    waveMissionEndingInstrumental = new WaveFile (dirs->getSounds ((char *)"MissionEndingInstrumental.wav"));
    waveMissionEndingInstrumental->setVolume (127);
    waveBlueTeamHasWonBattle = new WaveFile (dirs->getSounds ((char *)"BlueTeamHasWonBattle.wav"));
    waveBlueTeamHasWonBattle->setVolume (127);
    waveRedTeamHasWonBattle = new WaveFile (dirs->getSounds ((char *)"RedTeamHasWonBattle.wav"));
    waveRedTeamHasWonBattle->setVolume (127);
    waveMissionEndingIn15SecCountdown = new WaveFile (dirs->getSounds ((char *)"MissionEndingIn15SecCountdown.wav"));
    waveMissionEndingIn15SecCountdown->setVolume (127);
    waveEnemyHqStatus100a = new WaveFile (dirs->getSounds ((char *)"EnemyHqAirfieldStatus100Percent.wav"));
    waveEnemyHqStatus100a->setVolume (127);
    LacStartup00 = new WaveFile (dirs->getSounds ((char *)"Startup00.wav"));
    LacStartup00->setVolume (127);
    EnemyHqAckAckFrom01to10Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom01to10Pct.wav"));
    EnemyHqAckAckFrom01to10Pct->setVolume (127);
    EnemyHqAckAckFrom10to20Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom10to20Pct.wav"));
    EnemyHqAckAckFrom10to20Pct->setVolume (127);
    EnemyHqAckAckFrom20to30Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom20to30Pct.wav"));
    EnemyHqAckAckFrom20to30Pct->setVolume (127);
    EnemyHqAckAckFrom30to40Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom30to40Pct.wav"));
    EnemyHqAckAckFrom30to40Pct->setVolume (127);
    EnemyHqAckAckFrom40to50Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom40to50Pct.wav"));
    EnemyHqAckAckFrom40to50Pct->setVolume (127);
    EnemyHqAckAckFrom50to60Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom50to60Pct.wav"));
    EnemyHqAckAckFrom50to60Pct->setVolume (127);
    EnemyHqAckAckFrom60to70Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom60to70Pct.wav"));
    EnemyHqAckAckFrom60to70Pct->setVolume (127);
    EnemyHqAckAckFrom70to80Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom70to80Pct.wav"));
    EnemyHqAckAckFrom70to80Pct->setVolume (127);
    EnemyHqAckAckFrom80to90Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom80to90Pct.wav"));
    EnemyHqAckAckFrom80to90Pct->setVolume (127);
    EnemyHqAckAckFrom90to100Pct = new WaveFile (dirs->getSounds ((char *)"EnemyHqAckAckFrom90to100Pct.wav"));
    EnemyHqAckAckFrom90to100Pct->setVolume (127);
    EnemyHqRepairsProceedingNormally = new WaveFile (dirs->getSounds ((char *)"EnemyHqRepairsProceedingNormally.wav"));
    EnemyHqRepairsProceedingNormally->setVolume (127);
    EnemyHqRepairsProceedingRapidly = new WaveFile (dirs->getSounds ((char *)"EnemyHqRepairsProceedingRapidly.wav"));
    EnemyHqRepairsProceedingRapidly->setVolume (127);
    EnemyHqRepairsStopped = new WaveFile (dirs->getSounds ((char *)"EnemyHqRepairsStopped.wav"));
    EnemyHqRepairsStopped->setVolume (127);
    Morse0000 = new WaveFile (dirs->getSounds ((char *)"Morse0000.wav"));
    Morse0000->setVolume (127);
    Morse0100 = new WaveFile (dirs->getSounds ((char *)"Morse0100.wav"));
    Morse0100->setVolume (127);
    Morse0200 = new WaveFile (dirs->getSounds ((char *)"Morse0200.wav"));
    Morse0200->setVolume (127);
    Morse0300 = new WaveFile (dirs->getSounds ((char *)"Morse0300.wav"));
    Morse0300->setVolume (127);
    Morse0400 = new WaveFile (dirs->getSounds ((char *)"Morse0400.wav"));
    Morse0400->setVolume (127);
    Morse0500 = new WaveFile (dirs->getSounds ((char *)"Morse0500.wav"));
    Morse0500->setVolume (127);
    Morse0600 = new WaveFile (dirs->getSounds ((char *)"Morse0600.wav"));
    Morse0600->setVolume (127);
    Morse0700 = new WaveFile (dirs->getSounds ((char *)"Morse0700.wav"));
    Morse0700->setVolume (127);
    Morse0800 = new WaveFile (dirs->getSounds ((char *)"Morse0800.wav"));
    Morse0800->setVolume (127);
    Morse0900 = new WaveFile (dirs->getSounds ((char *)"Morse0900.wav"));
    Morse0900->setVolume (127);
    MorseA00 = new WaveFile (dirs->getSounds ((char *)"MorseA00.wav"));
    MorseA00->setVolume (127);
    MorseB00 = new WaveFile (dirs->getSounds ((char *)"MorseB00.wav"));
    MorseB00->setVolume (127);
    MorseC00 = new WaveFile (dirs->getSounds ((char *)"MorseC00.wav"));
    MorseC00->setVolume (127);
    MorseD00 = new WaveFile (dirs->getSounds ((char *)"MorseD00.wav"));
    MorseD00->setVolume (127);
    MorseE00 = new WaveFile (dirs->getSounds ((char *)"MorseE00.wav"));
    MorseE00->setVolume (127);
    MorseF00 = new WaveFile (dirs->getSounds ((char *)"MorseF00.wav"));
    MorseF00->setVolume (127);
    MorseG00 = new WaveFile (dirs->getSounds ((char *)"MorseG00.wav"));
    MorseG00->setVolume (127);
    MorseH00 = new WaveFile (dirs->getSounds ((char *)"MorseH00.wav"));
    MorseH00->setVolume (127);
    MorseI00 = new WaveFile (dirs->getSounds ((char *)"MorseI00.wav"));
    MorseI00->setVolume (127);
    MorseJ00 = new WaveFile (dirs->getSounds ((char *)"MorseJ00.wav"));
    MorseJ00->setVolume (127);
    MorseK00 = new WaveFile (dirs->getSounds ((char *)"MorseK00.wav"));
    MorseK00->setVolume (127);
    MorseL00 = new WaveFile (dirs->getSounds ((char *)"MorseL00.wav"));
    MorseL00->setVolume (127);
    MorseM00 = new WaveFile (dirs->getSounds ((char *)"MorseM00.wav"));
    MorseM00->setVolume (127);
    MorseN00 = new WaveFile (dirs->getSounds ((char *)"MorseN00.wav"));
    MorseN00->setVolume (127);
    MorseO00 = new WaveFile (dirs->getSounds ((char *)"MorseO00.wav"));
    MorseO00->setVolume (127);
    MorseP00 = new WaveFile (dirs->getSounds ((char *)"MorseP00.wav"));
    MorseP00->setVolume (127);
    MorseQ00 = new WaveFile (dirs->getSounds ((char *)"MorseQ00.wav"));
    MorseQ00->setVolume (127);
    MorseR00 = new WaveFile (dirs->getSounds ((char *)"MorseR00.wav"));
    MorseR00->setVolume (127);
    MorseS00 = new WaveFile (dirs->getSounds ((char *)"MorseS00.wav"));
    MorseS00->setVolume (127);
    MorseT00 = new WaveFile (dirs->getSounds ((char *)"MorseT00.wav"));
    MorseT00->setVolume (127);
    MorseU00 = new WaveFile (dirs->getSounds ((char *)"MorseU00.wav"));
    MorseU00->setVolume (127);
    MorseV00 = new WaveFile (dirs->getSounds ((char *)"MorseV00.wav"));
    MorseV00->setVolume (127);
    MorseW00 = new WaveFile (dirs->getSounds ((char *)"MorseW00.wav"));
    MorseW00->setVolume (127);
    MorseX00 = new WaveFile (dirs->getSounds ((char *)"MorseX00.wav"));
    MorseX00->setVolume (127);
    MorseY00 = new WaveFile (dirs->getSounds ((char *)"MorseY00.wav"));
    MorseY00->setVolume (127);
    MorseZ00         = new WaveFile (dirs->getSounds ((char *)"MorseZ00.wav"));
    MorseZ00->setVolume (127);
    XMorseComma00    = new WaveFile (dirs->getSounds ((char *)"XMorseComma00.wav"));
    XMorseComma00->setVolume (127);
    XMorsePeriod00   = new WaveFile (dirs->getSounds ((char *)"XMorsePeriod00.wav"));
    XMorsePeriod00->setVolume (127);
    XMorseQuestion00 = new WaveFile (dirs->getSounds ((char *)"XMorseQuestion00.wav"));
    XMorseQuestion00->setVolume (127);
    XMorseSlash00    = new WaveFile (dirs->getSounds ((char *)"XMorseSlash.wav"));
    XMorseSlash00->setVolume (127);
    AirRaidSiren = new WaveFile (dirs->getSounds ((char *)"AirRaidSiren.wav"));
    AirRaidSiren->setVolume (127);
    Defeat00 = new WaveFile (dirs->getSounds ((char *)"Defeat00.wav"));
    Defeat00->setVolume (127);
    FiveBeeps00 = new WaveFile (dirs->getSounds ((char *)"FiveLoudBeeps.wav"));
    FiveBeeps00->setVolume (127);
    } //@ end SoundSystem::SoundSystem ()


SoundSystem::~SoundSystem ()
    {
    int i;

    if (!audio)
        {
        return;
        }

    if (music1)
        {
        Mix_FreeMusic (music1);
        }
    Mix_CloseAudio ();
    SDL_CloseAudio ();
    delete waveexplosion1;
    delete waveclick1;
    delete wavecannon1;
    delete wavebomb01;
    delete waveWindNoise;
    for (i = 0; i < 1; i ++)
        {
        delete waveplane [i];
        }
    delete wavehit1;
    delete wavebeep1;
    delete wavechaff1;
    delete wavebeep2;
    delete waveflapsdn;
    delete waveflapsup;
    delete wavespeedbrake;
    delete wavespeedbrake1;
    delete wavetiresqueak1;
    delete wavedamage00;
    delete wavecrashocean00;
    delete wavebellyscrape00;
    delete wavewheelrollout00;
    delete wavedigit000;
    delete wavedigit001;
    delete wavedigit002;
    delete wavedigit003;
    delete wavedigit004;
    delete wavedigit005;
    delete wavedigit006;
    delete wavedigit007;
    delete wavedigit008;
    delete wavedigit009;
    delete wavedigit010;
    delete wavedigit011;
    delete wavedigit012;
    delete wavedigit013;
    delete wavedigit014;
    delete wavedigit015;
    delete wavedigit016;
    delete wavedigit017;
    delete wavedigit018;
    delete wavedigit019;
    delete wavedigit020;
    delete wavedigit030;
    delete waveBlue;
    delete waveBoseNetDotNoDashIpDotBiz;
    delete waveChannel;
    delete waveGlobalAssignmentAudioAtMumble;
    delete waveLinuxAirCombat;
    delete waveMissionCommsOnChannel;
    delete waveMissionNetworkBattle01;
    delete waveMissionNetworkBattle02;
    delete waveMissionNetworkBattle03;
    delete waveMissionTelemetryAvailableFromLacServerAt;
    delete waveRed;
    delete waveSquadCommsOnSubchannel;
    delete waveWeaponsFree;
    delete waveYouHaveAuthorityToProceed;
    delete waveundercarriage;
    delete waveNewBanditOnRadar01;
    delete waveRadarBanditDisappeared01;
    delete waveTarget;
    delete waveIsAt;
    delete waveAngels;
    delete waveOclock;
    delete waveVoiceCommsZello1;
    delete waveMissionEndingInstrumental;
    delete waveBlueTeamHasWonBattle;
    delete waveRedTeamHasWonBattle;
    delete waveMissionEndingIn15SecCountdown;
    delete waveEnemyHqStatus100a;
    delete LacStartup00;
    delete AirRaidSiren;
    delete Defeat00;
    delete FiveBeeps00;
    delete EnemyHqAckAckFrom01to10Pct;
    delete EnemyHqAckAckFrom10to20Pct;
    delete EnemyHqAckAckFrom20to30Pct;
    delete EnemyHqAckAckFrom30to40Pct;
    delete EnemyHqAckAckFrom40to50Pct;
    delete EnemyHqAckAckFrom50to60Pct;
    delete EnemyHqAckAckFrom60to70Pct;
    delete EnemyHqAckAckFrom70to80Pct;
    delete EnemyHqAckAckFrom80to90Pct;
    delete EnemyHqAckAckFrom90to100Pct;
    delete EnemyHqRepairsProceedingNormally;
    delete EnemyHqRepairsProceedingRapidly;
    delete EnemyHqRepairsStopped;
    delete Morse0000;
    delete Morse0100;
    delete Morse0200;
    delete Morse0300;
    delete Morse0400;
    delete Morse0500;
    delete Morse0600;
    delete Morse0700;
    delete Morse0800;
    delete Morse0900;
    delete MorseA00;
    delete MorseB00;
    delete MorseC00;
    delete MorseD00;
    delete MorseE00;
    delete MorseF00;
    delete MorseG00;
    delete MorseH00;
    delete MorseI00;
    delete MorseJ00;
    delete MorseK00;
    delete MorseL00;
    delete MorseM00;
    delete MorseN00;
    delete MorseO00;
    delete MorseP00;
    delete MorseQ00;
    delete MorseR00;
    delete MorseS00;
    delete MorseT00;
    delete MorseU00;
    delete MorseV00;
    delete MorseW00;
    delete MorseX00;
    delete MorseY00;
    delete MorseZ00;
    delete XMorseComma00;
    delete XMorsePeriod00;
    delete XMorseQuestion00;
    delete XMorseSlash00;
    } //@ end SoundSystem::~SoundSystem ()

//@
//@ SoundSystem::play (sample, loop)
//@
//@ The "sample" argument really should be named "channel". It
//@ routes the referenced sound to one of several separate
//@ audio "channels" so it isn't pre-empted by a subsequent
//@ sound. If two sounds are routed to the same channel, the
//@ last one prevails and stops the preceeding one. Use this
//@ function to optimize the distribution of audio channels
//@ accordingly.
//@
void SoundSystem::play (int sample, bool loop)
    {
    if (!audio)
        {
        return;
        }
    if (!sound)
        {
        return;
        }
    switch (sample)
        {
        case SOUND_CLICK1:
            waveclick1->play (0, loop);
            break;
        case SOUND_CANNON1:
            wavecannon1->play (0, loop);
            break;
        case SOUND_BOMB01:
            wavebomb01->play (0, loop);
            break;
        case SOUND_EXPLOSION1:
            waveexplosion1->play (2, loop);
            break;
        case SOUND_WINDNOISE:
            waveWindNoise->play (3, loop);
            break;
        case SOUND_PLANE1:
            waveplane [0]->play (4, loop); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_PLANE2:
            waveplane [1]->play (4, loop); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_HIT1:
            wavehit1->play (5, loop);
            break;
        case SOUND_BEEP1:
            wavebeep1->play (6, loop);
            break;
        case SOUND_CHAFF1:
            wavechaff1->play (1, loop);
            break;
        case SOUND_BEEP2:
            wavebeep2->play (6, loop);
            break;
        case SOUND_FLAPSDN:
            waveflapsdn->play (1, loop);
            break;
        case SOUND_FLAPSUP:
            waveflapsup->play (1, loop);
            break;
        case SOUND_SPEEDBRAKE:
            wavespeedbrake->play (1, loop);
            break;
        case SOUND_SPEEDBRAKE1:
            wavespeedbrake1->play (1, loop);
            break;
        case SOUND_TIRESQUEAK1:
            wavetiresqueak1->play (2, loop);
            break;
        case SOUND_DAMAGE00:
            wavedamage00->play (2, loop);
            break;
        case SOUND_CRASHOCEAN00:
            wavecrashocean00->play (2, loop);
            break;
        case SOUND_BELLYSCRAPE00:
            wavebellyscrape00->play (2, loop);
            break;
        case SOUND_WHEELROLLOUT00:
            wavewheelrollout00->play (3, loop);
            break;
        case SOUND_DIGIT000:
            wavedigit000->play (2, loop);
            break;
        case SOUND_DIGIT001:
            wavedigit001->play (2, loop);
            break;
        case SOUND_DIGIT002:
            wavedigit002->play (2, loop);
            break;
        case SOUND_DIGIT003:
            wavedigit003->play (2, loop);
            break;
        case SOUND_DIGIT004:
            wavedigit004->play (2, loop);
            break;
        case SOUND_DIGIT005:
            wavedigit005->play (2, loop);
            break;
        case SOUND_DIGIT006:
            wavedigit006->play (2, loop);
            break;
        case SOUND_DIGIT007:
            wavedigit007->play (2, loop);
            break;
        case SOUND_DIGIT008:
            wavedigit008->play (2, loop);
            break;
        case SOUND_DIGIT009:
            wavedigit009->play (2, loop);
            break;
        case SOUND_DIGIT010:
            wavedigit010->play (2, loop);
            break;
        case SOUND_DIGIT011:
            wavedigit011->play (2, loop);
            break;
        case SOUND_DIGIT012:
            wavedigit012->play (2, loop);
            break;
        case SOUND_DIGIT013:
            wavedigit013->play (2, loop);
            break;
        case SOUND_DIGIT014:
            wavedigit014->play (2, loop);
            break;
        case SOUND_DIGIT015:
            wavedigit015->play (2, loop);
            break;
        case SOUND_DIGIT016:
            wavedigit016->play (2, loop);
            break;
        case SOUND_DIGIT017:
            wavedigit017->play (2, loop);
            break;
        case SOUND_DIGIT018:
            wavedigit018->play (2, loop);
            break;
        case SOUND_DIGIT019:
            wavedigit019->play (2, loop);
            break;
        case SOUND_DIGIT020:
            wavedigit020->play (2, loop);
            break;
        case SOUND_DIGIT030:
            wavedigit030->play (2, loop);
            break;
        case SOUND_BLUE:
            waveBlue->play (1, loop);
            break;
        case SOUND_BOSENETDOTNODASHIPDOTBIZ:
            waveBoseNetDotNoDashIpDotBiz->play (5, loop);
            break;
        case SOUND_CHANNEL:
            waveChannel->play (1, loop);
            break;
        case SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE:
            waveGlobalAssignmentAudioAtMumble->play (1, loop);
            break;
        case SOUND_LINUXAIRCOMBAT:
            waveLinuxAirCombat->play (1, loop);
            break;
        case SOUND_MISSIONCOMMSONCHANNEL:
            waveMissionCommsOnChannel->play (1, loop);
            break;
        case SOUND_MISSIONNETWORKBATTLE01:
            waveMissionNetworkBattle01->play (1, loop);
            break;
        case SOUND_MISSIONNETWORKBATTLE02:
            waveMissionNetworkBattle02->play (1, loop);
            break;
        case SOUND_MISSIONNETWORKBATTLE03:
            waveMissionNetworkBattle03->play (1, loop);
            break;
        case SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT:
            waveMissionTelemetryAvailableFromLacServerAt->play (1, loop);
            break;
        case SOUND_RED:
            waveRed->play (1, loop);
            break;
        case SOUND_SQUADCOMMSONSUBCHANNEL:
            waveSquadCommsOnSubchannel->play (1, loop);
            break;
        case SOUND_WEAPONSFREE:
            waveWeaponsFree->play (1, loop);
            break;
        case SOUND_YOUHAVEAUTHORITYTOPROCEED:
            waveYouHaveAuthorityToProceed->play (1, loop);
            break;
        case SOUND_UNDERCARRIAGE:
            waveundercarriage->play (1, loop);
            break;
        case SOUND_NEWBANDITONRADARO1:
            waveNewBanditOnRadar01->play (2, loop);
            break;
        case SOUND_RADARBANDITDISAPPEARED01:
            waveRadarBanditDisappeared01->play (2, loop); //@ was 2
            break;
        case SOUND_TARGET:
            waveTarget->play (1, loop);
            break;
        case SOUND_IsAt:
            waveIsAt->play (1, loop);
            break;
        case SOUND_Angels:
            waveAngels->play (1, loop);
            break;
        case SOUND_Oclock:
            waveOclock->play (1, loop);
            break;
        case SOUND_VOICECOMMSMUMBLE01:
            waveVoiceCommsZello1->play (2, loop);
            break;
        case SOUND_BLUETEAMHASWONBATTLE:
            waveBlueTeamHasWonBattle->play (6, loop);
            break;
        case SOUND_REDTEAMHASWONBATTLE:
            waveRedTeamHasWonBattle->play (6, loop);
            break;
        case SOUND_MISSIONENDINGIN15SECCOUNTDOWN:
            waveMissionEndingIn15SecCountdown->play (6, loop);
            break;
        case SOUND_ENEMYHQSTATUS100A:
            waveEnemyHqStatus100a->play (7, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM01TO10PCT:
            EnemyHqAckAckFrom01to10Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM10TO20PCT:
            EnemyHqAckAckFrom10to20Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM20TO30PCT:
            EnemyHqAckAckFrom20to30Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM30TO40PCT:
            EnemyHqAckAckFrom30to40Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM40TO50PCT:
            EnemyHqAckAckFrom40to50Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM50TO60PCT:
            EnemyHqAckAckFrom50to60Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM60TO70PCT:
            EnemyHqAckAckFrom60to70Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM70TO80PCT:
            EnemyHqAckAckFrom70to80Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM80TO90PCT:
            EnemyHqAckAckFrom80to90Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQACKACKFROM90TO100PCT:
            EnemyHqAckAckFrom90to100Pct->play (2, loop);
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY:
            EnemyHqRepairsProceedingNormally->play (2, loop);
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY:
            EnemyHqRepairsProceedingRapidly->play (2, loop);
            break;
        case SOUND_ENEMYHQREPAIRSSTOPPED:
            EnemyHqRepairsStopped->play (2, loop);
            break;
        case SOUND_MORSE0000:
            Morse0000->play (7, loop);
            break;
        case SOUND_MORSE0100:
            Morse0100->play (7, loop);
            break;
        case SOUND_MORSE0200:
            Morse0200->play (7, loop);
            break;
        case SOUND_MORSE0300:
            Morse0300->play (7, loop);
            break;
        case SOUND_MORSE0400:
            Morse0400->play (7, loop);
            break;
        case SOUND_MORSE0500:
            Morse0500->play (7, loop);
            break;
        case SOUND_MORSE0600:
            Morse0600->play (7, loop);
            break;
        case SOUND_MORSE0700:
            Morse0700->play (7, loop);
            break;
        case SOUND_MORSE0800:
            Morse0800->play (7, loop);
            break;
        case SOUND_MORSE0900:
            Morse0900->play (7, loop);
            break;
        case SOUND_MORSEA00:
            MorseA00->play (7, loop);
            break;
        case SOUND_MORSEB00:
            MorseB00->play (7, loop);
            break;
        case SOUND_MORSEC00:
            MorseC00->play (7, loop);
            break;
        case SOUND_MORSED00:
            MorseD00->play (7, loop);
            break;
        case SOUND_MORSEE00:
            MorseE00->play (7, loop);
            break;
        case SOUND_MORSEF00:
            MorseF00->play (7, loop);
            break;
        case SOUND_MORSEG00:
            MorseG00->play (7, loop);
            break;
        case SOUND_MORSEH00:
            MorseH00->play (7, loop);
            break;
        case SOUND_MORSEI00:
            MorseI00->play (7, loop);
            break;
        case SOUND_MORSEJ00:
            MorseJ00->play (7, loop);
            break;
        case SOUND_MORSEK00:
            MorseK00->play (7, loop);
            break;
        case SOUND_MORSEL00:
            MorseL00->play (7, loop);
            break;
        case SOUND_MORSEM00:
            MorseM00->play (7, loop);
            break;
        case SOUND_MORSEN00:
            MorseN00->play (7, loop);
            break;
        case SOUND_MORSEO00:
            MorseO00->play (7, loop);
            break;
        case SOUND_MORSEP00:
            MorseP00->play (7, loop);
            break;
        case SOUND_MORSEQ00:
            MorseQ00->play (7, loop);
            break;
        case SOUND_MORSER00:
            MorseR00->play (7, loop);
            break;
        case SOUND_MORSES00:
            MorseS00->play (7, loop);
            break;
        case SOUND_MORSET00:
            MorseT00->play (7, loop);
            break;
        case SOUND_MORSEU00:
            MorseU00->play (7, loop);
            break;
        case SOUND_MORSEV00:
            MorseV00->play (7, loop);
            break;
        case SOUND_MORSEW00:
            MorseW00->play (7, loop);
            break;
        case SOUND_MORSEX00:
            MorseX00->play (7, loop);
            break;
        case SOUND_MORSEY00:
            MorseY00->play (7, loop);
            break;
        case SOUND_MORSEZ00:
            MorseZ00->play (7, loop);
            break;
        case SOUND_XMORSECOMMA00:
            XMorseComma00->play (7, loop);
            break;
        case SOUND_XMORSEPERIOD00:
            XMorsePeriod00->play (7, loop);
            break;
        case SOUND_XMORSEQUESTION00:
            XMorseQuestion00->play (7, loop);
            break;
        case SOUND_XMORSESLASH00:
            XMorseSlash00->play (7, loop);
            break;
        case SOUND_AIRRAIDSIREN:
            AirRaidSiren->play (7, loop);
            break;
        case SOUND_MISSIONENDINGINSTRUMENTAL:
            waveMissionEndingInstrumental->play (5, loop); //@ was 2
            break;
        case SOUND_FIVEBEEPS00:
            FiveBeeps00->play (5, loop);
            break;
        case SOUND_DEFEAT00:
            Defeat00->play (5, loop);
            break;
        case SOUND_LACSTARTUP00:
            LacStartup00->play (2, loop);
        }
    } //@ end void SoundSystem::play (int sample, bool loop)


void SoundSystem::playLoop (int sample)
    {
    if (!audio)
        {
        return;
        }
    if (!sound)
        {
        return;
        }
    play (sample, true);
    }


void SoundSystem::playMusic ()
    {
    if (!audio)
        {
        return;
        }
    if (!music)
        {
        return;
        }
    if (volumemusic == 0)
        {
        return;
        }
    musicplaying = true;
    Mix_PlayMusic (music1, -1);
    setVolumeMusic ();
    } //@ end void SoundSystem::playMusic ()


void SoundSystem::playMusic (int times)
    {
    if (!audio)
        {
        return;
        }
    if (!music)
        {
        return;
        }
    if (volumemusic == 0)
        {
        return;
        }
    musicplaying = true;
    Mix_PlayMusic (music1, times);
    setVolumeMusic ();
    } //@ end void SoundSystem::playMusic (int times)


void SoundSystem::haltMusic ()
    {
    if (!audio)
        {
        return;
        }
    musicplaying = false;
    Mix_HaltMusic ();
    } //@ end void SoundSystem::haltMusic ()


void SoundSystem::loadMusic (int sample)
    {
    if (!audio)
        {
        return;
        }
    if (music1)
        {
        Mix_FreeMusic (music1);
        }
    switch (sample)
        {
        case MUSIC_ANDCOMBAT:
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicAndCombat00.wav"));
            break;
        case MUSIC_TUTORIAL01:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_TUTORIAL01", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicTutorial1.wav"));
            break;
        case MUSIC_TUTORIAL02:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_TUTORIAL02", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicTutorial2.wav"));
            break;
        case MUSIC_TUTORIAL03:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_TUTORIAL03", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicTutorial3.wav"));
            break;
        case MUSIC_MISSIONNETWORKBATTLE01:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_MISSIONNETWORKBATTLE01", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicMissionNetworkBattle01.wav"));
            break;
        case MUSIC_MISSIONNETWORKBATTLE02:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_MISSIONNETWORKBATTLE02", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicMissionNetworkBattle02.wav"));
            break;
        case MUSIC_MISSIONNETWORKBATTLE03:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_MISSIONNETWORKBATTLE03", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicMissionNetworkBattle03.wav"));
            break;
        case MUSIC_HEADTOHEAD:
            display ((char *)"SoundSystem::loadMusic() loading MUSIC_HEADTOHEAD", LOG_MOST);
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"MusicHeadToHead00.wav"));
            break;
        case MUSIC_MUSIC01:
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music00.wav"));
            break;
        case MUSIC_MUSIC02:
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music01.wav"));
            break;
        case MUSIC_MUSIC03:
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music02.wav"));
            break;
        case MUSIC_MUSIC02ANDFURBALLAA:
            music1 = Mix_LoadMUS (dirs->getMusic ((char *)"Music02AndFurballAA.wav"));
            break;
        }
    } //@ end void SoundSystem::loadMusic (int sample)


void SoundSystem::setVolume (int sample, int level)
    {
    if (!audio)
        {
        return;
        }
    level *= volumesound;
    level /= 100;
    switch (sample)
        {
        case SOUND_CLICK1:
            waveclick1->setVolume (level);
            break;
        case SOUND_CANNON1:
            wavecannon1->setVolume (level);
            break;
        case SOUND_BOMB01:
            wavebomb01->setVolume (level);
            break;
        case SOUND_EXPLOSION1:
            waveexplosion1->setVolume (level);
            break;
        case SOUND_WINDNOISE:
            waveWindNoise->setVolume (level);
            break;
        case SOUND_PLANE1:
            waveplane [0]->setVolume (level); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_PLANE2:
            waveplane [1]->setVolume (level); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_HIT1:
            wavehit1->setVolume (level);
            break;
        case SOUND_BEEP1:
            wavebeep1->setVolume (level);
            break;
        case SOUND_CHAFF1:
            wavechaff1->setVolume (level);
            break;
        case SOUND_BEEP2:
            wavebeep2->setVolume (level);
            break;
        case SOUND_FLAPSDN:
            waveflapsdn->setVolume (level);
            break;
        case SOUND_FLAPSUP:
            waveflapsup->setVolume (level);
            break;
        case SOUND_SPEEDBRAKE:
            wavespeedbrake->setVolume (level);
            break;
        case SOUND_SPEEDBRAKE1:
            wavespeedbrake1->setVolume (level);
            break;
        case SOUND_TIRESQUEAK1:
            wavetiresqueak1->setVolume (level);
            break;
        case SOUND_DAMAGE00:
            wavedamage00->setVolume (level);
            break;
        case SOUND_CRASHOCEAN00:
            wavecrashocean00->setVolume (level);
            break;
        case SOUND_BELLYSCRAPE00:
            wavebellyscrape00->setVolume (level);
            break;
        case SOUND_WHEELROLLOUT00:
            wavewheelrollout00->setVolume (level);
            break;
        case SOUND_DIGIT000:
            wavedigit000->setVolume (level);
            break;
        case SOUND_DIGIT001:
            wavedigit001->setVolume (level);
            break;
        case SOUND_DIGIT002:
            wavedigit002->setVolume (level);
            break;
        case SOUND_DIGIT003:
            wavedigit003->setVolume (level);
            break;
        case SOUND_DIGIT004:
            wavedigit004->setVolume (level);
            break;
        case SOUND_DIGIT005:
            wavedigit005->setVolume (level);
            break;
        case SOUND_DIGIT006:
            wavedigit006->setVolume (level);
            break;
        case SOUND_DIGIT007:
            wavedigit007->setVolume (level);
            break;
        case SOUND_DIGIT008:
            wavedigit008->setVolume (level);
            break;
        case SOUND_DIGIT009:
            wavedigit009->setVolume (level);
            break;
        case SOUND_DIGIT010:
            wavedigit010->setVolume (level);
            break;
        case SOUND_DIGIT011:
            wavedigit011->setVolume (level);
            break;
        case SOUND_DIGIT012:
            wavedigit012->setVolume (level);
            break;
        case SOUND_DIGIT013:
            wavedigit013->setVolume (level);
            break;
        case SOUND_DIGIT014:
            wavedigit014->setVolume (level);
            break;
        case SOUND_DIGIT015:
            wavedigit015->setVolume (level);
            break;
        case SOUND_DIGIT016:
            wavedigit016->setVolume (level);
            break;
        case SOUND_DIGIT017:
            wavedigit017->setVolume (level);
            break;
        case SOUND_DIGIT018:
            wavedigit018->setVolume (level);
            break;
        case SOUND_DIGIT019:
            wavedigit019->setVolume (level);
            break;
        case SOUND_DIGIT020:
            wavedigit020->setVolume (level);
            break;
        case SOUND_DIGIT030:
            wavedigit030->setVolume (level);
            break;
        case SOUND_BLUE:
            waveBlue->setVolume (level);
            break;
        case SOUND_BOSENETDOTNODASHIPDOTBIZ:
            waveBoseNetDotNoDashIpDotBiz->setVolume (level);
            break;
        case SOUND_CHANNEL:
            waveChannel->setVolume (level);
            break;
        case SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE:
            waveGlobalAssignmentAudioAtMumble->setVolume (level);
            break;
        case SOUND_LINUXAIRCOMBAT:
            waveLinuxAirCombat->setVolume (level);
            break;
        case SOUND_MISSIONCOMMSONCHANNEL:
            waveMissionCommsOnChannel->setVolume (level);
            break;
        case SOUND_MISSIONNETWORKBATTLE01:
            waveMissionNetworkBattle01->setVolume (level);
            break;
        case SOUND_MISSIONNETWORKBATTLE02:
            waveMissionNetworkBattle02->setVolume (level);
            break;
        case SOUND_MISSIONNETWORKBATTLE03:
            waveMissionNetworkBattle03->setVolume (level);
            break;
        case SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT:
            waveMissionTelemetryAvailableFromLacServerAt->setVolume (level);
            break;
        case SOUND_RED:
            waveRed->setVolume (level);
            break;
        case SOUND_SQUADCOMMSONSUBCHANNEL:
            waveSquadCommsOnSubchannel->setVolume (level);
            break;
        case SOUND_WEAPONSFREE:
            waveWeaponsFree->setVolume (level);
            break;
        case SOUND_YOUHAVEAUTHORITYTOPROCEED:
            waveYouHaveAuthorityToProceed->setVolume (level);
            break;
        case SOUND_UNDERCARRIAGE:
            waveundercarriage->setVolume (level);
            break;
        case SOUND_NEWBANDITONRADARO1:
            waveNewBanditOnRadar01->setVolume (level);
            break;
        case SOUND_RADARBANDITDISAPPEARED01:
            waveRadarBanditDisappeared01->setVolume (level);
            break;
        case SOUND_TARGET:
            waveTarget->setVolume (level);
            break;
        case SOUND_IsAt:
            waveIsAt->setVolume (level);
            break;
        case SOUND_Angels:
            waveAngels->setVolume (level);
            break;
        case SOUND_Oclock:
            waveOclock->setVolume (level);
            break;
        case SOUND_VOICECOMMSMUMBLE01:
            waveVoiceCommsZello1->setVolume (level);
            break;
        case SOUND_BLUETEAMHASWONBATTLE:
            waveBlueTeamHasWonBattle->setVolume (level);
            break;
        case SOUND_REDTEAMHASWONBATTLE:
            waveRedTeamHasWonBattle->setVolume (level);
            break;
        case SOUND_MISSIONENDINGIN15SECCOUNTDOWN:
            waveMissionEndingIn15SecCountdown->setVolume (level);
            break;
        case SOUND_ENEMYHQSTATUS100A:
            waveEnemyHqStatus100a->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM01TO10PCT:
            EnemyHqAckAckFrom01to10Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM10TO20PCT:
            EnemyHqAckAckFrom10to20Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM20TO30PCT:
            EnemyHqAckAckFrom20to30Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM30TO40PCT:
            EnemyHqAckAckFrom30to40Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM40TO50PCT:
            EnemyHqAckAckFrom40to50Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM50TO60PCT:
            EnemyHqAckAckFrom50to60Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM60TO70PCT:
            EnemyHqAckAckFrom60to70Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM70TO80PCT:
            EnemyHqAckAckFrom70to80Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM80TO90PCT:
            EnemyHqAckAckFrom80to90Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQACKACKFROM90TO100PCT:
            EnemyHqAckAckFrom90to100Pct->setVolume (level);
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY:
            EnemyHqRepairsProceedingNormally->setVolume (level);
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY:
            EnemyHqRepairsProceedingRapidly->setVolume (level);
            break;
        case SOUND_ENEMYHQREPAIRSSTOPPED:
            EnemyHqRepairsStopped->setVolume (level);
            break;
        case SOUND_MORSE0000:
            Morse0000->setVolume (level);
            break;
        case SOUND_MORSE0100:
            Morse0100->setVolume (level);
            break;
        case SOUND_MORSE0200:
            Morse0200->setVolume (level);
            break;
        case SOUND_MORSE0300:
            Morse0300->setVolume (level);
            break;
        case SOUND_MORSE0400:
            Morse0400->setVolume (level);
            break;
        case SOUND_MORSE0500:
            Morse0500->setVolume (level);
            break;
        case SOUND_MORSE0600:
            Morse0600->setVolume (level);
            break;
        case SOUND_MORSE0700:
            Morse0700->setVolume (level);
            break;
        case SOUND_MORSE0800:
            Morse0800->setVolume (level);
            break;
        case SOUND_MORSE0900:
            Morse0900->setVolume (level);
            break;
        case SOUND_MORSEA00:
            MorseA00->setVolume (level);
            break;
        case SOUND_MORSEB00:
            MorseB00->setVolume (level);
            break;
        case SOUND_MORSEC00:
            MorseC00->setVolume (level);
            break;
        case SOUND_MORSED00:
            MorseD00->setVolume (level);
            break;
        case SOUND_MORSEE00:
            MorseE00->setVolume (level);
            break;
        case SOUND_MORSEF00:
            MorseF00->setVolume (level);
            break;
        case SOUND_MORSEG00:
            MorseG00->setVolume (level);
            break;
        case SOUND_MORSEH00:
            MorseH00->setVolume (level);
            break;
        case SOUND_MORSEI00:
            MorseI00->setVolume (level);
            break;
        case SOUND_MORSEJ00:
            MorseJ00->setVolume (level);
            break;
        case SOUND_MORSEK00:
            MorseK00->setVolume (level);
            break;
        case SOUND_MORSEL00:
            MorseL00->setVolume (level);
            break;
        case SOUND_MORSEM00:
            MorseM00->setVolume (level);
            break;
        case SOUND_MORSEN00:
            MorseN00->setVolume (level);
            break;
        case SOUND_MORSEO00:
            MorseO00->setVolume (level);
            break;
        case SOUND_MORSEP00:
            MorseP00->setVolume (level);
            break;
        case SOUND_MORSEQ00:
            MorseQ00->setVolume (level);
            break;
        case SOUND_MORSER00:
            MorseR00->setVolume (level);
            break;
        case SOUND_MORSES00:
            MorseS00->setVolume (level);
            break;
        case SOUND_MORSET00:
            MorseT00->setVolume (level);
            break;
        case SOUND_MORSEU00:
            MorseU00->setVolume (level);
            break;
        case SOUND_MORSEV00:
            MorseV00->setVolume (level);
            break;
        case SOUND_MORSEW00:
            MorseW00->setVolume (level);
            break;
        case SOUND_MORSEX00:
            MorseX00->setVolume (level);
            break;
        case SOUND_MORSEY00:
            MorseY00->setVolume (level);
            break;
        case SOUND_MORSEZ00:
            MorseZ00->setVolume (level);
            break;
        case SOUND_XMORSECOMMA00:
            XMorseComma00->setVolume (level);
            break;
        case SOUND_XMORSEPERIOD00:
            XMorsePeriod00->setVolume (level);
            break;
        case SOUND_XMORSEQUESTION00:
            XMorseQuestion00->setVolume (level);
            break;
        case SOUND_XMORSESLASH00:
            XMorseSlash00->setVolume (level);
            break;
        case SOUND_AIRRAIDSIREN:
            AirRaidSiren->setVolume (level);
            break;
        case SOUND_DEFEAT00:
            Defeat00->setVolume (level);
            break;
        case SOUND_FIVEBEEPS00:
            FiveBeeps00->setVolume (level);
            break;
        case SOUND_MISSIONENDINGINSTRUMENTAL:
            waveMissionEndingInstrumental->setVolume (level);
            break;
        case SOUND_LACSTARTUP00:
            LacStartup00->setVolume (level);
        }
    } //@ end void SoundSystem::setVolume (int sample, int level)


void SoundSystem::setVolume ()
    {
    if (!audio)
        {
        return;
        }
    int i;
    int level = 128 * volumesound / 100;
    waveclick1->setVolume (level * 80 / 128);
    wavecannon1->setVolume (level * 50 / 128);
    wavebomb01->setVolume (level * 110 / 128);
    waveexplosion1->setVolume (level);
    waveWindNoise->setVolume (level);
    for (i = 0; i < 1; i ++)
        {
        waveplane [i]->setVolume (level);
        }
    wavehit1->setVolume (level);
    wavebeep1->setVolume (level);
    wavechaff1->setVolume (level);
    wavebeep2->setVolume (level);
    waveflapsdn->setVolume (level);
    waveflapsup->setVolume (level);
    wavespeedbrake->setVolume (level);
    wavespeedbrake1->setVolume (level);
    wavetiresqueak1->setVolume (level);
    wavedamage00->setVolume (level);
    wavecrashocean00->setVolume (level);
    wavebellyscrape00->setVolume (level);
    wavewheelrollout00->setVolume (level);
    wavedigit000->setVolume (level);
    wavedigit001->setVolume (level);
    wavedigit002->setVolume (level);
    wavedigit003->setVolume (level);
    wavedigit004->setVolume (level);
    wavedigit005->setVolume (level);
    wavedigit006->setVolume (level);
    wavedigit007->setVolume (level);
    wavedigit008->setVolume (level);
    wavedigit009->setVolume (level);
    wavedigit010->setVolume (level);
    wavedigit011->setVolume (level);
    wavedigit012->setVolume (level);
    wavedigit013->setVolume (level);
    wavedigit014->setVolume (level);
    wavedigit015->setVolume (level);
    wavedigit016->setVolume (level);
    wavedigit017->setVolume (level);
    wavedigit018->setVolume (level);
    wavedigit019->setVolume (level);
    wavedigit020->setVolume (level);
    wavedigit030->setVolume (level);
    waveBlue->setVolume (level);
    waveBoseNetDotNoDashIpDotBiz->setVolume (level);
    waveChannel->setVolume (level);
    waveGlobalAssignmentAudioAtMumble->setVolume (level);
    waveLinuxAirCombat->setVolume (level);
    waveMissionCommsOnChannel->setVolume (level);
    waveMissionNetworkBattle01->setVolume (level);
    waveMissionNetworkBattle02->setVolume (level);
    waveMissionNetworkBattle03->setVolume (level);
    waveMissionTelemetryAvailableFromLacServerAt->setVolume (level);
    waveRed->setVolume (level);
    waveSquadCommsOnSubchannel->setVolume (level);
    waveWeaponsFree->setVolume (level);
    waveYouHaveAuthorityToProceed->setVolume (level);
    waveundercarriage->setVolume (level);
    waveNewBanditOnRadar01->setVolume (level);
    waveRadarBanditDisappeared01->setVolume (level);
    waveTarget->setVolume (level);
    waveIsAt->setVolume (level);
    waveAngels->setVolume (level);
    waveOclock->setVolume (level);
    waveVoiceCommsZello1->setVolume (level);
    waveBlueTeamHasWonBattle->setVolume (level);
    waveRedTeamHasWonBattle->setVolume (level);
    waveMissionEndingIn15SecCountdown->setVolume (level);
    waveMissionEndingInstrumental->setVolume (level);
    waveEnemyHqStatus100a->setVolume (level);
    AirRaidSiren->setVolume (level);
    Defeat00->setVolume (level);
    FiveBeeps00->setVolume (level);
    EnemyHqAckAckFrom01to10Pct->setVolume (level);
    EnemyHqAckAckFrom10to20Pct->setVolume (level);
    EnemyHqAckAckFrom20to30Pct->setVolume (level);
    EnemyHqAckAckFrom30to40Pct->setVolume (level);
    EnemyHqAckAckFrom40to50Pct->setVolume (level);
    EnemyHqAckAckFrom50to60Pct->setVolume (level);
    EnemyHqAckAckFrom60to70Pct->setVolume (level);
    EnemyHqAckAckFrom70to80Pct->setVolume (level);
    EnemyHqAckAckFrom80to90Pct->setVolume (level);
    EnemyHqAckAckFrom90to100Pct->setVolume (level);
    EnemyHqRepairsProceedingNormally->setVolume (level);
    EnemyHqRepairsProceedingRapidly->setVolume (level);
    EnemyHqRepairsStopped->setVolume (level);
    LacStartup00->setVolume (level);
    Morse0000->setVolume (level);
    Morse0100->setVolume (level);
    Morse0200->setVolume (level);
    Morse0300->setVolume (level);
    Morse0400->setVolume (level);
    Morse0500->setVolume (level);
    Morse0600->setVolume (level);
    Morse0700->setVolume (level);
    Morse0800->setVolume (level);
    Morse0900->setVolume (level);
    MorseA00->setVolume (level);
    MorseB00->setVolume (level);
    MorseC00->setVolume (level);
    MorseD00->setVolume (level);
    MorseE00->setVolume (level);
    MorseF00->setVolume (level);
    MorseG00->setVolume (level);
    MorseH00->setVolume (level);
    MorseI00->setVolume (level);
    MorseJ00->setVolume (level);
    MorseK00->setVolume (level);
    MorseL00->setVolume (level);
    MorseM00->setVolume (level);
    MorseN00->setVolume (level);
    MorseO00->setVolume (level);
    MorseP00->setVolume (level);
    MorseQ00->setVolume (level);
    MorseR00->setVolume (level);
    MorseS00->setVolume (level);
    MorseT00->setVolume (level);
    MorseU00->setVolume (level);
    MorseV00->setVolume (level);
    MorseW00->setVolume (level);
    MorseX00->setVolume (level);
    MorseY00->setVolume (level);
    MorseZ00->setVolume (level);
    XMorseComma00->setVolume (level);
    XMorsePeriod00->setVolume (level);
    XMorseQuestion00->setVolume (level);
    XMorseSlash00->setVolume (level);
    } //@ end void SoundSystem::setVolume ()


void SoundSystem::setVolumeMusic ()
    {
    if (!audio)
        {
        return;
        }
    int level = 128 * volumemusic / 100;
    Mix_VolumeMusic (level);
    }


void SoundSystem::setPosition (int sample, int angle, int dist)
    {
    if (!audio)
        {
        return;
        }

    switch (sample)
        {
        case SOUND_EXPLOSION1:
            Mix_SetPosition (2, angle, dist);
            break;
        }
    } //@ end void SoundSystem::setVolumeMusic ()


void SoundSystem::stop (int sample)
    {
    if (!audio)
        {
        return;
        }
    switch (sample)
        {
        case SOUND_CLICK1:
            waveclick1->stop ();
            break;
        case SOUND_CANNON1:
            wavecannon1->stop ();
            break;
        case SOUND_BOMB01:
            wavebomb01->stop ();
            break;
        case SOUND_EXPLOSION1:
            waveexplosion1->stop ();
            break;
        case SOUND_WINDNOISE:
            waveWindNoise->stop ();
            break;
        case SOUND_PLANE1:
            waveplane [0]->stop (); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_PLANE2:
            waveplane [1]->stop (); // should be [engine] instead of [0], first find better sounds
            break;
        case SOUND_HIT1:
            wavehit1->stop ();
            break;
        case SOUND_BEEP1:
            wavebeep1->stop ();
            break;
        case SOUND_CHAFF1:
            wavechaff1->stop ();
            break;
        case SOUND_BEEP2:
            wavebeep2->stop ();
            break;
        case SOUND_FLAPSDN:
            waveflapsdn->stop ();
            break;
        case SOUND_FLAPSUP:
            waveflapsup->stop ();
            break;
        case SOUND_SPEEDBRAKE:
            wavespeedbrake->stop ();
            break;
        case SOUND_SPEEDBRAKE1:
            wavespeedbrake1->stop ();
            break;
        case SOUND_TIRESQUEAK1:
            wavetiresqueak1->stop ();
            break;
        case SOUND_DAMAGE00:
            wavedamage00->stop ();
            break;
        case SOUND_CRASHOCEAN00:
            wavecrashocean00->stop ();
            break;
        case SOUND_BELLYSCRAPE00:
            wavebellyscrape00->stop ();
            break;
        case SOUND_WHEELROLLOUT00:
            wavewheelrollout00->stop ();
            break;
        case SOUND_DIGIT000:
            wavedigit000->stop ();
            break;
        case SOUND_DIGIT001:
            wavedigit001->stop ();
            break;
        case SOUND_DIGIT002:
            wavedigit002->stop ();
            break;
        case SOUND_DIGIT003:
            wavedigit003->stop ();
            break;
        case SOUND_DIGIT004:
            wavedigit004->stop ();
            break;
        case SOUND_DIGIT005:
            wavedigit005->stop ();
            break;
        case SOUND_DIGIT006:
            wavedigit006->stop ();
            break;
        case SOUND_DIGIT007:
            wavedigit007->stop ();
            break;
        case SOUND_DIGIT008:
            wavedigit008->stop ();
            break;
        case SOUND_DIGIT009:
            wavedigit009->stop ();
            break;
        case SOUND_DIGIT010:
            wavedigit010->stop ();
            break;
        case SOUND_DIGIT011:
            wavedigit011->stop ();
            break;
        case SOUND_DIGIT012:
            wavedigit012->stop ();
            break;
        case SOUND_DIGIT013:
            wavedigit013->stop ();
            break;
        case SOUND_DIGIT014:
            wavedigit014->stop ();
            break;
        case SOUND_DIGIT015:
            wavedigit015->stop ();
            break;
        case SOUND_DIGIT016:
            wavedigit016->stop ();
            break;
        case SOUND_DIGIT017:
            wavedigit017->stop ();
            break;
        case SOUND_DIGIT018:
            wavedigit018->stop ();
            break;
        case SOUND_DIGIT019:
            wavedigit019->stop ();
            break;
        case SOUND_DIGIT020:
            wavedigit020->stop ();
            break;
        case SOUND_DIGIT030:
            wavedigit030->stop ();
            break;
        case SOUND_BLUE:
            waveBlue->stop ();
            break;
        case SOUND_BOSENETDOTNODASHIPDOTBIZ:
            waveBoseNetDotNoDashIpDotBiz->stop ();
            break;
        case SOUND_CHANNEL:
            waveChannel->stop ();
            break;
        case SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE:
            waveGlobalAssignmentAudioAtMumble->stop ();
            break;
        case SOUND_LINUXAIRCOMBAT:
            waveLinuxAirCombat->stop ();
            break;
        case SOUND_MISSIONCOMMSONCHANNEL:
            waveMissionCommsOnChannel->stop ();
            break;
        case SOUND_MISSIONNETWORKBATTLE01:
            waveMissionNetworkBattle01->stop ();
            break;
        case SOUND_MISSIONNETWORKBATTLE02:
            waveMissionNetworkBattle02->stop ();
            break;
        case SOUND_MISSIONNETWORKBATTLE03:
            waveMissionNetworkBattle03->stop ();
            break;
        case SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT:
            waveMissionTelemetryAvailableFromLacServerAt->stop ();
            break;
        case SOUND_RED:
            waveRed->stop ();
            break;
        case SOUND_SQUADCOMMSONSUBCHANNEL:
            waveSquadCommsOnSubchannel->stop ();
            break;
        case SOUND_WEAPONSFREE:
            waveWeaponsFree->stop ();
            break;
        case SOUND_YOUHAVEAUTHORITYTOPROCEED:
            waveYouHaveAuthorityToProceed->stop ();
            break;
        case SOUND_UNDERCARRIAGE:
            waveundercarriage->stop ();
            break;
        case SOUND_NEWBANDITONRADARO1:
            waveNewBanditOnRadar01->stop ();
            break;
        case SOUND_RADARBANDITDISAPPEARED01:
            waveRadarBanditDisappeared01->stop ();
            break;
        case SOUND_TARGET:
            waveTarget->stop ();
            break;
        case SOUND_IsAt:
            waveIsAt->stop ();
            break;
        case SOUND_Angels:
            waveAngels->stop ();
            break;
        case SOUND_Oclock:
            waveOclock->stop ();
            break;
        case SOUND_BLUETEAMHASWONBATTLE:
            waveBlueTeamHasWonBattle->stop ();
            break;
        case SOUND_REDTEAMHASWONBATTLE:
            waveRedTeamHasWonBattle->stop ();
            break;
        case SOUND_MISSIONENDINGIN15SECCOUNTDOWN:
            waveMissionEndingIn15SecCountdown->stop ();
            break;
        case SOUND_MISSIONENDINGINSTRUMENTAL:
            waveMissionEndingInstrumental->stop ();
            break;
        case SOUND_ENEMYHQSTATUS100A:
            waveEnemyHqStatus100a->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM01TO10PCT:
            EnemyHqAckAckFrom01to10Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM10TO20PCT:
            EnemyHqAckAckFrom10to20Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM20TO30PCT:
            EnemyHqAckAckFrom20to30Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM30TO40PCT:
            EnemyHqAckAckFrom30to40Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM40TO50PCT:
            EnemyHqAckAckFrom40to50Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM50TO60PCT:
            EnemyHqAckAckFrom50to60Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM60TO70PCT:
            EnemyHqAckAckFrom60to70Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM70TO80PCT:
            EnemyHqAckAckFrom70to80Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM80TO90PCT:
            EnemyHqAckAckFrom80to90Pct->stop ();
            break;
        case SOUND_ENEMYHQACKACKFROM90TO100PCT:
            EnemyHqAckAckFrom90to100Pct->stop ();
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGNORMALLY:
            EnemyHqRepairsProceedingNormally->stop ();
            break;
        case SOUND_ENEMYHQREPAIRSPROCEEDINGRAPIDLY:
            EnemyHqRepairsProceedingRapidly->stop ();
            break;
        case SOUND_ENEMYHQREPAIRSSTOPPED:
            EnemyHqRepairsStopped->stop ();
            break;
        case SOUND_MORSE0000:
            Morse0000->stop ();
            break;
        case SOUND_MORSE0100:
            Morse0100->stop ();
            break;
        case SOUND_MORSE0200:
            Morse0200->stop ();
            break;
        case SOUND_MORSE0300:
            Morse0300->stop ();
            break;
        case SOUND_MORSE0400:
            Morse0400->stop ();
            break;
        case SOUND_MORSE0500:
            Morse0500->stop ();
            break;
        case SOUND_MORSE0600:
            Morse0600->stop ();
            break;
        case SOUND_MORSE0700:
            Morse0700->stop ();
            break;
        case SOUND_MORSE0800:
            Morse0800->stop ();
            break;
        case SOUND_MORSE0900:
            Morse0900->stop ();
            break;
        case SOUND_MORSEA00:
            MorseA00->stop ();
            break;
        case SOUND_MORSEB00:
            MorseB00->stop ();
            break;
        case SOUND_MORSEC00:
            MorseC00->stop ();
            break;
        case SOUND_MORSED00:
            MorseD00->stop ();
            break;
        case SOUND_MORSEE00:
            MorseE00->stop ();
            break;
        case SOUND_MORSEF00:
            MorseF00->stop ();
            break;
        case SOUND_MORSEG00:
            MorseG00->stop ();
            break;
        case SOUND_MORSEH00:
            MorseH00->stop ();
            break;
        case SOUND_MORSEI00:
            MorseI00->stop ();
            break;
        case SOUND_MORSEJ00:
            MorseJ00->stop ();
            break;
        case SOUND_MORSEK00:
            MorseK00->stop ();
            break;
        case SOUND_MORSEL00:
            MorseL00->stop ();
            break;
        case SOUND_MORSEM00:
            MorseM00->stop ();
            break;
        case SOUND_MORSEN00:
            MorseN00->stop ();
            break;
        case SOUND_MORSEO00:
            MorseO00->stop ();
            break;
        case SOUND_MORSEP00:
            MorseP00->stop ();
            break;
        case SOUND_MORSEQ00:
            MorseQ00->stop ();
            break;
        case SOUND_MORSER00:
            MorseR00->stop ();
            break;
        case SOUND_MORSES00:
            MorseS00->stop ();
            break;
        case SOUND_MORSET00:
            MorseT00->stop ();
            break;
        case SOUND_MORSEU00:
            MorseU00->stop ();
            break;
        case SOUND_MORSEV00:
            MorseV00->stop ();
            break;
        case SOUND_MORSEW00:
            MorseW00->stop ();
            break;
        case SOUND_MORSEX00:
            MorseX00->stop ();
            break;
        case SOUND_MORSEY00:
            MorseY00->stop ();
            break;
        case SOUND_MORSEZ00:
            MorseZ00->stop ();
            break;
        case SOUND_XMORSECOMMA00:
            XMorseComma00->stop ();
            break;
        case SOUND_XMORSEPERIOD00:
            XMorsePeriod00->stop ();
            break;
        case SOUND_XMORSEQUESTION00:
            XMorseQuestion00->stop ();
            break;
        case SOUND_XMORSESLASH00:
            XMorseSlash00->stop ();
            break;
        case SOUND_AIRRAIDSIREN:
            AirRaidSiren->stop ();
            break;
        case SOUND_DEFEAT00:
            Defeat00->stop ();
            break;
        case SOUND_FIVEBEEPS00:
            FiveBeeps00->stop ();
            break;
        case SOUND_LACSTARTUP00:
            LacStartup00->stop ();
        }
    } //@ end void SoundSystem::stop (int sample)

void SoundSystem::stopAll ()
    {
    if (!audio)
        {
        return;
        }
    Mix_HaltChannel (-1);
    } //@ end void SoundSystem::stopAll ()
