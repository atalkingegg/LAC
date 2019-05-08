This is the "ReadMe" file for LINUX Air Combat (or "LAC"), the premier open-source combat flight simulator as of this writing in March of 2019. 





CONTENTS:

This folder contains:

-- A compiled version of Linux Air combat in the bin/Release subfolder. (This version
   has been compiled for 64-bit PcLinuxOs. It is also known to work in recent, 64-bit 
   versions of Ubuntu Linux after loading two free prerequisites with "apt-get", but 
   you may need to compile it to make it work on other distributions.)

-- An installation script named "install.sh" that will install and configure Linux Air Combat.

-- All of the source code necessary to compile or customize your own version of Linux Air Combat

-- A "Codeblocks Project File" to make it easy to use the well-known "Codeblocks" compiler GUI

-- A "bin" subfolder containing all other necessary resources

-- A "Makefile" for those that don't want to use "CodeBlocks" when compiling.





HARDWARE NOTES:

Linux Air Combat is compatible with all of the leading versions of desktop LINUX. It generally runs acceptably on any Computer ever built with enough power to run any version of Microsoft Windows. Any PC built since about 2007 should support it well enough to enjoy smooth flight in a large window. 





FLIGHT CONTROLS:

A "numeric keypad" helps a lot, because numeric keypad keys are used, by default, to help you look left, right, forward, backward, up, or down while you are flying your aircraft. A standard, full-size keyboard also helps a lot, since the physical arrangement of flight controls is optimized for the usual keyboard layout (but you can re-map your keyboard keys for your convenience if you want to work out some other control layout).

LAC can use either your joystick or your mouse pointer as your primary flight control, moving your aircraft's elevator and aileron surfaces and firing your primary weapons. You can control your rudder and throttle with a joystick or with your keyboard.





DISPLAY ADAPTERS:

   "Intel Integrated Graphics": LAC generally runs acceptably even if your PC is equipped only with "Intel Integrated Graphics".
   
   "nVidia": Users of every popular "nVidia" display adapter built since about 2004 have reported splendid success with LAC. They report amazing, smooth frame rates even at very high resolution and/or full-screen settings.
   
   "Ati/Radeon": Users of Ati and Radeon display adapters have also reported splendid success while LAC is flying any of its missions. However, some of these adapters suffer from severely degraded performance reading "mouse pointer" positions within LAC's startup and configuration menus, making our menu navigation cumbersome. To help with this, LAC allows menu navigation using the keys of the numeric keypad. According to the usual layout of numeric keypad keys, when navigating within LAC's menus, you can use "NUM4" (generally labeled with a left-pointing arrow) to move the mouse cursor left, "NUM6" (generally labeled with a right-pointing arrow) to move right, "NUM8" (generally labeled with an up-pointing arrow) to move up, "NUM2" (generally labeled with a down-pointing arrow) to move down, and "ENTER" or "SPACE" to select the menu item under the mouse cursor. (Note that these are the same keys that are used in flight to look left, right, forward, or back, and they can be re-mapped if you prefer some other arrangement.)
   

   
   

PREREQUISITES:

Whether you just want to try running the precompiled version that is delivered in our standard download, or whether you will compile your own executable version from the supplied source code, your desktop LINUX system ALWAYS needs some supporting library files.

For compatibility with any compiled binary version, LAC requires each of these well-known, popular LINUX libraries and tools, which are generally preinstalled in many major LINUX desktop distributions:

libfreeglut3
libSDL1.2_0
libSDL_mixer1.2_0
libmesaglu1
libmesa

As of October 2017, two of those prerequisites are NOT pre-installed on Ubuntu desktop Linux, but it is very easy to obtain them using the well-known "apt-get" command. The commands to install those two libraries, issued into a bash command shell, are:

sudo apt-get install freeglut3
sudo apt-get install libsdl-mixer1.2

Additional Prerequisites for compiling your own version from the LINUX AIR COMBAT source code

If your LINUX system is typical, you will need to compile LINUX AIR COMBAT from the free source code (the well-organized source code makes this very easy, even for non-programmers). In addition to the prerequisites listed above, you will also need gcc (almost always present),  and all of these tools and libraries, which are generally NOT preinstalled in most major LINUX desktop distributions:

gcc-c++
Code::Blocks
Libfreeglut-devel
libSDL-devel
libSDL_mixer-devel

Experienced LINUX users will recognize all of these as well-known LINUX components. For most of the popular LINUX desktop distributions, every one of these components will be freely available through the usual and customary means, using free package managers. If you have a good Internet connection, you should be able to get everything within 5 or 10 minutes and with just a few mouse clicks. For best compatibility with other members of our online community, you will want to make sure your libraries are up-to-date.  For a YouTube video showing how we obtained all of the necessary tools and compiled a recent version of LAC, check our main web site at AskMisterWizard.com.

   

   
   
COMPILING:

LAC's source code supports compilation or re-compilation in two different ways as follows:

Option 1 of 2: Traditional "make". If you are familiar with the usual "make" process, you can use "make", "make clean", and "make install" in the usual way, followed by "./install" or "./install.sh".

Option 2 of 2: You can use the free, open-source "CodeBlocks" Integrated Development Environment. This may be easier for beginners that want to experiment with compiling. 





INSTALLING:

Once LAC has been compiled, it is essential that you run our "./install.sh" or "./install" utility, which will copy all of the required files into the required locations of your filesystem.





STARTUP:

At first, you will want to start LAC from a bash shell according to long-standing LINUX norms. After compiling LAC and running the installer, the executable will be found beneath your home folder, at "LAC/bin". To run it, cd to that folder and type "./" followed by its filename according to the usual program startup pattern. LAC will start immediately. As LAC starts and runs, it always writes a detailed log of all events in a file named "logfile.txt", located in a new, hidden folder named ".LAC" beneath your home folder. If LAC does not run properly in your environment, the contents of that "logfile.txt" will be very helpful after any LAC session. (Pay particular attention to the last few lines in that file.)





OPTIMIZING LAC:

LAC offers a prominent "SETUP OPTIONS" menu through which you can change display resolution, display size, video options, sounds, flight controls, etc. 

In addition to the options that you will see there, you can tweak additional details by editing the two files named:

   "LacConfig.txt" and
   "LacControls.txt"

...that you will find in your new, hidden ".LAC" folder. Use any simple text editor to modify the contents of those files according to the tutorial text that you will find within them, according to long-standing LINUX norms.
   

   
   
   
FURTHER INFORMATION:   

LAC is a rich, powerful simulator that will reward you for as much attention as you dedicate to its configuration and mastery. At first you may find it both primitive-looking and intimidating, because it starts out configured for compatibility with simple, inexpensive computers, but you may find yourself competing with other people that have taken the time to optimize their own setups and to master LAC's flight and combat tactics. Be patient; work through LAC's myriad options, and you can make it look, sound, and fly a lot better on any modern LINUX PC.

If you spend more than a few minutes with LAC, you will DEFINITELY want to get further information, including detailed pictures of our standard control layout, from our online web pages at:

http://askmisterwizard.com

From there, follow the prominent "Linux Air Combat" links. The resulting web pages are unusually comprehensive for an online flight simulator, and constitute LAC's primary documentation and support tool, containing many, many links to "YouTube" video clips that explain and illustrate all of the important concepts. Users that want to become competitive with other LAC players will want to use printed copies of those pages for reference.


