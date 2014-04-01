Game Boy Enhanced - Game Boy emulator focused on enhancements
===============
Project Site: http://code.google.com/p/gb-enhanced/

Author(s): D.S. Baxter - aka Shonumi

Release Version: 1.0

Release Date: April 1, 2014


License
===============
Free Open Source Software available under the GPLv2. See license.txt for full details


Overview
===============
Game Boy Enhanced (GBE for short) aims to be a fully functional Game Boy emulator. The goal is to create a highly portable emulator using C++ and SDL 1.2, document the Game Boy's functions through clear code, and add as many enhancements (scaling filters, cheats, custom sprites) as reasonably possible.


Features
===============
* Emulates MBC1, MBC2, MBC3, and MBC5 cartridges
* Saves battery-backed RAM
* Nearest-Neighbor scaling filters 2x - 4x
* Custom user-generated graphics
* Built-in screenshot capability
* Joystick support


Compiling GBE
===============
The only requirement at this moment is SDL 1.2 and a C++ compiler. Linux users need only run the compile.sh script to build with G++. Users on Windows can compile GBE easily as well. If MinGW is installed, and if the SDL 1.2 development files are installed in MinGW's directories, and the MinGW g++ executable is added to Windows' PATH environment variable, users need only run the compile.bat script to build GBE.


Running GBE
===============
The program is CLI only at this time. For Windows users, especially those who prefer not to use the CLI, simply drag and drop the Game Boy ROM onto the gbe.exe to play a game. Options can be configured through the gbe.ini file.

GBE's command-line usage is as follows:

gbe [path_to_game_file] [options ...]

Any options passed to GBE through the command-line will override any of the settings in gbe.ini. Below are the descriptions of the possible options GBE will accept:

--bios                Tells GBE to emulate the Game Boy Bootstrap ROM. GBE will look for a file called "bios.bin" in the same location as the GBE executable.
--open_gl             Tells GBE to use OpenGL for blit operations instead of SDL.
--dump_sprites        Tells GBE to enter graphics dumping mode to rip/extract sprites and background tiles.
--load_sprites        Tells GBE to enter custom graphics loading mode to replace sprites and background tiles with user-generated pixel data.
--fullscreen          Runs GBE in fullscreen mode
--f1                  Sets the current scaling filter to Nearest Neighbor 2x
--f2                  Sets the current scaling filter to Nearest Neighbor 3x
--f3                  Sets the current scaling filter to Nearest Neighbor 4x

Note that --dump_sprites and --load_sprites cannot be used at the same time. Whichever one GBE parses last will be used. Only the first scaling filter will be parsed, the rest are ignored if multiple ones are passed to GBE.

Note that when using --dump_sprites, OpenGL cannot be used for blit operations. GBE will default back to SDL. This is due to how background tiles are manually highlighted and dumped.


GBE Hotkeys
===============
Q                     Exit GBE
Esc                   Exit GBE
Tab                   Disable framerate limiter (turbo mode)
F9                    Take screenshot
F10                   Toggle between fullscreen and windowed mode


Configuring GBE
===============
GBE provides an easy way (easy for something with no GUI yet, that is) to configure global settings without having to use the command-line options. The options can simply be edited through the gbe.ini file. Keyboard and joystick controls, scaling filters, bootstrap rom usage, OpenGL blitting, and custom graphics can be controlled through this file. These options are thoroughly documented in the file's comments as well.


Using custom graphics in GBE
===============
Custom graphics are graphics that the user can dynamically replace with his or her own pixel data. Game Boy Enhanced allows for users to "dump" or "rip" sprites and background tiles directly from the game. The user can then edit these graphics and tell Game Boy Enhanced to load them when playing a game.

This method allows for users to easily modify a game's graphical elements without having to actually edit the game's code itself. The game's graphics can merely be altered to show a different palette, fully colorize old monochrome sprites, or create entirely new graphics altogether.

For a full usage guide on custom graphics, please see http://code.google.com/p/gb-enhanced/wiki/CustomGraphicsGuide