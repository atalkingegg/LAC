# LAC - Linux Air Combat - The Lac on Mac project
A multiplayer WW2 air combat flight simulator
This is a project to port from SDL1/Glut to a more modern SDL2, get this working
on MacOS, and generally add to the stability and awesomness of this fun game.
################################################################################

Phase 1, now done. 2019-05-08, onto code cleanup.

################################################################################

INITIAL CHECKIN - 2019-05-07
Starting with Lac07p64 - the plan for the next few weeks...
* Phase 1 (Lac08pre1) replaceing SDL1 / GLUT with SDL2, builds working on Linux/MacOS GCC/LLVM
* Phase 2 (Lac08pre2) compile code cleanup, attempt to get down to zero compile warnings.
* Phase 3 (Lac08pre3) runtime cleanup, stablize menu system and input handling.
* Phase 4 (Lac08pre4) game cleanup, any game / flight / networking and remaining runtime issues.
Then release as Lac08p00. 

Note: no plans in this project for ANY new features, code structure changes, flight improvements,
optimizations, etc. Just getting it working and as stable as possible.

Once this hits v08, I personally would like to start seeing features borrowed from SGI's dogfight, and
Sun's / Artificial Horizon's Aviator, two classic UNIX air combat simulators I've spent time with.
Moving to SDL2 opens up the possibilty of adding multiple display screens.

I'd also like to see a "Find/Join Battle Servers" function added in the style of BZFlag,
another classic SGI game, which is still being played. Check out https://www.bzflag.org

If you want to contribute to the LAC on Mac project, please download, build, play,
report bugs, read code, fix bugs, and send me an email at atalkingegg@yahoo.com

For questions about LAC's inner workings, please contact Bob Bosen at the website below

LAC's home page: https://askmisterwizard.com/2019/LinuxAirCombat/LinuxAirCombat.htm
and: https://sourceforge.net/projects/linuxaircombat/

Also a shout out to the original GL-117 project, where this code started back in 2005.

################################################################################
