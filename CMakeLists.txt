cmake_minimum_required(VERSION 3.13)

project(LAC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set (LAC_SRCS
  3ds.cpp
  aiobject.cpp
  audio.cpp
  cockpit.cpp
  common.cpp
  conf.cpp
  dirs.cpp
  effects.cpp
  fileloader.cpp
  FlightModel.cpp
  gl.cpp
  glland.cpp
  land.cpp
  loader_tga.cpp
  main.cpp
  mathtab.cpp
  menu.cpp
  mission.cpp
  model.cpp
  NetworkApi.cpp
  object.cpp
  objload.cpp
  pilots.cpp
  vertexarray.cpp
  )

find_package(OpenGL)
find_package(SDL2)
find_package(SDL2_mixer)
find_package(GLUT)

if(NOT DEFINED LAC_DIR)
  set(LAC_DIR "$ENV{HOME}/.LAC")
endif(NOT DEFINED LAC_DIR)

add_executable(lac ${LAC_SRCS})
target_link_libraries(lac ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES})
set(LAC_EXECUTABLE_DIR "${LAC_DIR}/bin")
install(TARGETS lac DESTINATION ${LAC_EXECUTABLE_DIR})

set(CONFIG_FILES
  DefaultHeightMap.LAC
  lac.png
  )
set(LAC_CONFIG_DIR "${LAC_DIR}")
install(FILES ${CONFIG_FILES} DESTINATION ${LAC_CONFIG_DIR})

set(MUSIC_FILES
  bin/music/Music00.wav
  bin/music/Music01.wav
  bin/music/Music02.wav
  bin/music/Music02AndFurballAA.wav
  bin/music/Music03.wav
  bin/music/MusicHeadToHead00.wav
  bin/music/MusicMissionNetworkBattle01.wav
  bin/music/MusicMissionNetworkBattle02.wav
  bin/music/MusicMissionNetworkBattle03.wav
  bin/music/MusicTutorial1.wav
  bin/music/MusicTutorial2.wav
  bin/music/MusicTutorial3.wav
  )
set(LAC_MUSIC_DIR "${LAC_DIR}/music")
install(FILES ${MUSIC_FILES} DESTINATION ${LAC_MUSIC_DIR})

set(SOUNDS_FILES
  bin/sounds/000.wav
  bin/sounds/001.wav
  bin/sounds/002.wav
  bin/sounds/003.wav
  bin/sounds/004.wav
  bin/sounds/005.wav
  bin/sounds/006.wav
  bin/sounds/007.wav
  bin/sounds/008.wav
  bin/sounds/009.wav
  bin/sounds/010.wav
  bin/sounds/011.wav
  bin/sounds/012.wav
  bin/sounds/013.wav
  bin/sounds/014.wav
  bin/sounds/015.wav
  bin/sounds/016.wav
  bin/sounds/017.wav
  bin/sounds/018.wav
  bin/sounds/019.wav
  bin/sounds/020.wav
  bin/sounds/030.wav
  bin/sounds/AirRaidSiren.wav
  bin/sounds/Angels.wav
  bin/sounds/BellyScrape00.wav
  bin/sounds/Blue.wav
  bin/sounds/BlueTeamHasWonBattle.wav
  bin/sounds/BoseNetDotNoDashIpDotBiz.wav
  bin/sounds/Channel.wav
  bin/sounds/CrashOcean00.wav
  bin/sounds/Damage00.wav
  bin/sounds/Defeat00.wav
  bin/sounds/EnemyHqAckAckFrom01to10Pct.wav
  bin/sounds/EnemyHqAckAckFrom10to20Pct.wav
  bin/sounds/EnemyHqAckAckFrom20to30Pct.wav
  bin/sounds/EnemyHqAckAckFrom30to40Pct.wav
  bin/sounds/EnemyHqAckAckFrom40to50Pct.wav
  bin/sounds/EnemyHqAckAckFrom50to60Pct.wav
  bin/sounds/EnemyHqAckAckFrom60to70Pct.wav
  bin/sounds/EnemyHqAckAckFrom70to80Pct.wav
  bin/sounds/EnemyHqAckAckFrom80to90Pct.wav
  bin/sounds/EnemyHqAckAckFrom90to100Pct.wav
  bin/sounds/EnemyHqAirfieldStatus100Percent.wav
  bin/sounds/EnemyHqRepairsProceedingNormally.wav
  bin/sounds/EnemyHqRepairsProceedingRapidly.wav
  bin/sounds/EnemyHqRepairsStopped.wav
  bin/sounds/FiveLoudBeeps.wav
  bin/sounds/GlobalAssignmentAudioAtMumble.wav
  bin/sounds/IsAt.wav
  bin/sounds/LinuxAirCombat.wav
  bin/sounds/MissionCommsOnChannel.wav
  bin/sounds/MissionEndingIn15SecCountdown.wav
  bin/sounds/MissionEndingInstrumental.wav
  bin/sounds/MissionNetworkBattle01.wav
  bin/sounds/MissionNetworkBattle02.wav
  bin/sounds/MissionNetworkBattle03.wav
  bin/sounds/MissionTelemetryAvailableFromLacServerAt.wav
  bin/sounds/Morse0000.wav
  bin/sounds/Morse0100.wav
  bin/sounds/Morse0200.wav
  bin/sounds/Morse0300.wav
  bin/sounds/Morse0400.wav
  bin/sounds/Morse0500.wav
  bin/sounds/Morse0600.wav
  bin/sounds/Morse0700.wav
  bin/sounds/Morse0800.wav
  bin/sounds/Morse0900.wav
  bin/sounds/MorseA00.wav
  bin/sounds/MorseB00.wav
  bin/sounds/MorseC00.wav
  bin/sounds/MorseD00.wav
  bin/sounds/MorseE00.wav
  bin/sounds/MorseF00.wav
  bin/sounds/MorseG00.wav
  bin/sounds/MorseH00.wav
  bin/sounds/MorseI00.wav
  bin/sounds/MorseJ00.wav
  bin/sounds/MorseK00.wav
  bin/sounds/MorseL00.wav
  bin/sounds/MorseM00.wav
  bin/sounds/MorseN00.wav
  bin/sounds/MorseO00.wav
  bin/sounds/MorseP00.wav
  bin/sounds/MorseQ00.wav
  bin/sounds/MorseR00.wav
  bin/sounds/MorseS00.wav
  bin/sounds/MorseT00.wav
  bin/sounds/MorseU00.wav
  bin/sounds/MorseV00.wav
  bin/sounds/MorseW00.wav
  bin/sounds/MorseX00.wav
  bin/sounds/MorseY00.wav
  bin/sounds/MorseZ00.wav
  bin/sounds/NewBanditOnRadar01.wav
  bin/sounds/Oclock.wav
  bin/sounds/RadarBanditDisappeared01.wav
  bin/sounds/Red.wav
  bin/sounds/RedTeamHasWonBattle.wav
  bin/sounds/SquadCommsOnSubchannel.wav
  bin/sounds/Startup00.wav
  bin/sounds/Target.wav
  bin/sounds/TireSqueak00.wav
  bin/sounds/VoiceCommsMumble01.wav
  bin/sounds/WeaponsFree.wav
  bin/sounds/WheelRollout00.wav
  bin/sounds/WindNoise.wav
  bin/sounds/XMorseComma00.wav
  bin/sounds/XMorsePeriod00.wav
  bin/sounds/XMorseQuestion00.wav
  bin/sounds/XMorseSlash.wav
  bin/sounds/YouHaveAuthorityToProceed.wav
  bin/sounds/beep1.wav
  bin/sounds/beep2.wav
  bin/sounds/bomb01.wav
  bin/sounds/cannon1.wav
  bin/sounds/chaff1.wav
  bin/sounds/click1.wav
  bin/sounds/explode1.wav
  bin/sounds/flapsdn.wav
  bin/sounds/flapsup.wav
  bin/sounds/hit1.wav
  bin/sounds/plane1.wav
  bin/sounds/plane2.wav
  bin/sounds/speedbrake.wav
  bin/sounds/speedbrake1.wav
  bin/sounds/temp.txt
  bin/sounds/undercarriage.wav
  )
set(LAC_SOUNDS_DIR "${LAC_DIR}/sounds")
install(FILES ${SOUNDS_FILES} DESTINATION ${LAC_SOUNDS_DIR})

set(MODELS_FILES
  bin/models/Airfield00.3ds
  bin/models/B25.3ds
  bin/models/B26.3ds
  bin/models/B29.3ds
  bin/models/B5N.3ds
  bin/models/Battleship.3ds
  bin/models/D3A.3ds
  bin/models/DW520.3ds
  bin/models/Dauntless.3ds
  bin/models/Dornier17.3ds
  bin/models/F4F.3ds
  bin/models/F4U.3ds
  bin/models/F6F.3ds
  bin/models/FIATG55.3ds
  bin/models/FW190.3ds
  bin/models/G4m.3ds
  bin/models/Generic01.3ds
  bin/models/HE111.3ds
  bin/models/HURRICANE.3ds
  bin/models/IL16.3ds
  bin/models/IL2.3ds
  bin/models/JU87.3ds
  bin/models/JU88.3ds
  bin/models/KI43.3ds
  bin/models/KI61.3ds
  bin/models/KI84.3ds
  bin/models/La5.3ds
  bin/models/La7.3ds
  bin/models/Lancaster.3ds
  bin/models/ME109.3ds
  bin/models/ME109F.3ds
  bin/models/ME110.3ds
  bin/models/ME163.3ds
  bin/models/MacciC202.3ds
  bin/models/MosquitoB.3ds
  bin/models/N1K1.3ds
  bin/models/P38F.3ds
  bin/models/P39.3ds
  bin/models/P40.3ds
  bin/models/P47B.3ds
  bin/models/P47aa.3ds
  bin/models/P51B.3ds
  bin/models/P51a00.3ds
  bin/models/RadarReflector01.3ds
  bin/models/RadarTower01.3ds
  bin/models/SB2C.3ds
  bin/models/SPIT5.3ds
  bin/models/TBF.3ds
  bin/models/TEMPEST.3ds
  bin/models/Typhoon.3ds
  bin/models/Yak1.3ds
  bin/models/Yak9.3ds
  bin/models/a6m2.3ds
  bin/models/a6m5.3ds
  bin/models/b17aa.3ds
  bin/models/b24.3ds
  bin/models/bomb1.3ds
  bin/models/cannon1.3ds
  bin/models/cannon1b.3ds
  bin/models/cannon2.3ds
  bin/models/cannon2b.3ds
  bin/models/chaff1.3ds
  bin/models/depot1.3ds
  bin/models/flak2.3ds
  bin/models/flare1.3ds
  bin/models/gl-14c.3ds
  bin/models/hall1.3ds
  bin/models/missile2.3ds
  bin/models/missile3.3ds
  bin/models/missile4.3ds
  bin/models/missile5.3ds
  bin/models/missile6.3ds
  bin/models/missile7.3ds
  bin/models/missile8.3ds
  bin/models/p38Laa.3ds
  bin/models/radar.3ds
  bin/models/rubble.3ds
  bin/models/spitfire.3ds
  bin/models/tank1.3ds
  bin/models/tank2.3ds
  )
set(LAC_MODELS_DIR "${LAC_DIR}/models")
install(FILES ${MODELS_FILES} DESTINATION ${LAC_MODELS_DIR})

set(TEXTURES_FILES
  bin/textures/LacTitle.tga
  bin/textures/cactus1.tga
  bin/textures/cactusu1.tga
  bin/textures/chaff_ch.tga
  bin/textures/clouds1.tga
  bin/textures/clouds2.tga
  bin/textures/clouds3.tga
  bin/textures/cross.tga
  bin/textures/cross2.tga
  bin/textures/depot_fr.tga
  bin/textures/earth.tga
  bin/textures/flare1.tga
  bin/textures/flare2.tga
  bin/textures/flare3.tga
  bin/textures/flare4.tga
  bin/textures/flare_ch.tga
  bin/textures/font1.tga
  bin/textures/font2.tga
  bin/textures/gl-14_c2.tga
  bin/textures/gl-14_ch.tga
  bin/textures/gl-50_c2.tga
  bin/textures/gl-50_ch.tga
  bin/textures/glitter.tga
  bin/textures/grass1.tga
  bin/textures/gravel1.tga
  bin/textures/hall1b_g.tga
  bin/textures/moon1.tga
  bin/textures/panzer1_.tga
  bin/textures/panzer2_.tga
  bin/textures/radar1.tga
  bin/textures/radar2.tga
  bin/textures/redsand1.tga
  bin/textures/redstone2.tga
  bin/textures/rocks1.tga
  bin/textures/sand1.tga
  bin/textures/ship2_fr.tga
  bin/textures/ship_fra.tga
  bin/textures/smoke1.tga
  bin/textures/smoke2.tga
  bin/textures/smoke3.tga
  bin/textures/sun2.tga
  bin/textures/tree1.tga
  bin/textures/tree2.tga
  bin/textures/tree3.tga
  bin/textures/tree4.tga
  bin/textures/tree5.tga
  bin/textures/treeu1.tga
  bin/textures/treeu2.tga
  bin/textures/treeu3.tga
  bin/textures/treeu4.tga
  bin/textures/treeu5.tga
  bin/textures/water1.tga
  )
set(LAC_TEXTURES_DIR "${LAC_DIR}/textures")
install(FILES ${TEXTURES_FILES} DESTINATION ${LAC_TEXTURES_DIR})


# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8
