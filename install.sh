#!/bin/bash -e

function jumpto
{
    label=$1
    cmd=$(sed -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

LAC_DIR=~/LAC
EXECUTABLE_DIR=$LAC_DIR/bin
CONFIG_DIR=~/.LAC
MUSIC_DIR=$LAC_DIR/music
SOUNDS_DIR=$LAC_DIR/sounds
MODELS_DIR=$LAC_DIR/models
TEXTURES_DIR=$LAC_DIR/textures

clear
echo
echo Linux Air Combat Installer.
echo
echo Linux Air Combat will only run on LINUX machines that are already
echo equipped with several well-known, very popular software packages.
echo Most popular desktop LINUX distributions support all of these as
echo standard components, and virtually all of them at least make them
echo available through their free repositories. Here is the list:
echo 
echo "   libfreeglut3"
echo "   libSDL1.2_0"
echo "   libSDL_mixer1.2_0"
echo "   libmesaglu1"
echo "   mesa"
echo
echo Some LINUX distributions support later or different, fully compatible
echo versions of those packages with similar names, and Linux Air Combat
echo generally runs in those circumstances too.
echo
echo "do you want to proceed?"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac
clear
echo
echo OK. Proceeding....


echo
echo This will install Linux Air Combat in a new subdirectory tree structure,
echo commencing at LAC in your home directory, with several lower-level
echo subdirectories beneath that point. It will also create a new hidden
echo subdirectory named .LAC in your home directory, where it will store
echo related configuration and log files.
echo
echo If you have already installed Linux Air Combat, this will re-install it
echo and reset your existing configuration back to default values. You may
echo want to preserve any previously optimized "LacConfig.txt" and 
echo "LacControls.txt" files before you take this step.
echo

echo "do you still want to proceed?"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac
clear
echo
echo OK. Proceeding....
echo

cp ./lac ~/lac

if [ -d "$LAC_DIR" ]; then
    echo $LAC_DIR already exists.
    jumpto skip1
fi
    echo $LAC_DIR does not exist. Creating it...
    mkdir $LAC_DIR
    echo Created $LAC_DIR.
    jumpto skip2


skip1:
    echo Using EXISTING LAC directory $LAC_DIR

skip2:
if [ -d "$CONFIG_DIR" ]; then
    echo $CONFIG_DIR already exists.
    jumpto skip3
fi
    echo $CONFIG_DIR does not exist. Creating it...
    mkdir $CONFIG_DIR
    echo Created $CONFIG_DIR.
    jumpto skip4

skip3:
    echo Using EXISTING config directory $CONFIG_DIR

skip4:
    echo Copying keyboard, mouse and joystick map configuration file LacControls.txt....
    cp ./LacControls.Generic.txt $CONFIG_DIR/LacControls.txt
    echo Copying Video and Network configuration file LacConfig.txt....
    cp ./LacConfig.txt $CONFIG_DIR/LacConfig.txt


skip5:
if [ -d "$EXECUTABLE_DIR" ]; then
    echo $EXECUTABLE_DIR already exists.
    jumpto skip6
fi
    echo $EXECUTABLE_DIR does not exist. Creating it...
    mkdir $EXECUTABLE_DIR
    echo Created $EXECUTABLE_DIR.
    jumpto skip7

skip6:
    echo Using EXISTING executable subdirectory $EXECUTABLE_DIR   

skip7:
    echo Copying main executable file...
    cp bin/Release/LacSdl2-191122A $EXECUTABLE_DIR/LacSdl2-191122A
    echo Copying default terrain...
    cp DefaultHeightMap.LAC $CONFIG_DIR/DefaultHeightMap.LAC
    echo Copying icon...
    cp lac.png $CONFIG_DIR/lac.png


skip8:
if [ -d "$MUSIC_DIR" ]; then
    echo $MUSIC_DIR already exists.
    jumpto skip9
fi
    echo $MUSIC_DIR does not exist. Creating it...
    mkdir $MUSIC_DIR
    echo Created $MUSIC_DIR.
    jumpto skip10

skip9:
    echo Using EXISTING music subdirectory $MUSIC_DIR

skip10:
    echo Copying music files into music subdirectory....
    cp bin/music/* $MUSIC_DIR


skip11:
if [ -d "$SOUNDS_DIR" ]; then
    echo $SOUNDS_DIR already exists.
    jumpto skip12
fi
    echo $SOUNDS_DIR does not exist. Creating it...
    mkdir $SOUNDS_DIR
    echo Created $SOUNDS_DIR.
    jumpto skip13

skip12:
    echo Using EXISTING sounds subdirectory $SOUNDS_DIR

skip13:
    echo Copying sound files into sounds subdirectory....
    cp bin/sounds/* $SOUNDS_DIR


skip14:
if [ -d "$MODELS_DIR" ]; then
    echo $MODELS_DIR already exists.
    jumpto skip15
fi
    echo $MODELS_DIR does not exist. Creating it...
    mkdir $MODELS_DIR
    echo Created $MODELS_DIR.
    jumpto skip16

skip15:
    echo Using EXISTING models subdirectory $MODELS_DIR

skip16:
    echo Copying model files into models subdirectory....
    cp bin/models/* $MODELS_DIR


skip17:
if [ -d "$TEXTURES_DIR" ]; then
    echo $TEXTURES_DIR already exists.
    jumpto skip18
fi
    echo $TEXTURES_DIR does not exist. Creating it...
    mkdir $TEXTURES_DIR
    echo Created $TEXTURES_DIR.
    jumpto skip19

skip18:
    echo Using EXISTING textures subdirectory $TEXTURES_DIR

skip19:
    echo Copying texture files into textures subdirectory....
    cp bin/textures/* $TEXTURES_DIR
echo
echo
echo PRESS ANY KEYBOARD KEY TO CONTINUE....
read -n 1 -s
clear
echo
echo A fresh copy of LINUX AIR COMBAT has been installed.
echo
echo For maximum compatibility and to help you with additional
echo configuration, it will run in low resolution and with low
echo display quality in a small window. This will allow you to
echo see diagnostic messages in the console display. Later on,
echo you can easily configure it for full-screen mode and much
echo higher video quality from the prominent SETUP OPTIONS menu.
echo
echo PRESS ANY KEYBOARD KEY TO CONTINUE....
read -n 1 -s
clear
echo
echo To run it, cd to your home directory, where you will find
echo a new, executable script file named lac. Run that script
echo by typing its name, preceeded by ./ according to 
echo well-established Linux/Unix norms.
echo
echo Accordingly, from your home folder, the command line
echo "to launch LINUX AIR COMBAT will look a lot like this:"
echo
echo "./lac"
echo
echo LINUX AIR COMBAT will start immediately.
echo
echo Alternatively, you can edit your LINUX menus or create
echo "desktop" files to launch lac graphically, according to
echo well established LINUX norms. To help you with this, a 
echo sample "lac.desktop" file is provided in the source code
echo folder. A sample "lac.png" icon has also been copied into
echo your $CONFIG_DIR folder. Adjust and use these as
echo necessary for your system.
echo
echo Your controls will be configured as described in the
echo LacControls.txt file, which is stored in your new, hidden
echo $CONFIG_DIR folder. You can change all of your flight
echo control definitions by editing that file with a simple
echo text editor. You can also change the most common control
echo definitions from prominent menus within the simulator.
echo
echo PRESS ANY KEYBOARD KEY TO CONTINUE....
read -n 1 -s
echo
echo A logfile will record all important events after every LAC 
echo session. If LAC does not work properly, PLEASE check that
echo logfile for help. It is located at $CONFIG_DIR/logfile.txt.
echo
echo Online help is always available through the prominent
echo "Linux Air Combat" link at
echo
echo https://AskMisterWizard.com
echo
echo Thanks for supporting Linux Air Combat.
echo
exit
















