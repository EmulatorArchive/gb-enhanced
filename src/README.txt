Overview:

GB Enhanced (GBE) is a Game Boy emulator that uses C++ and SDL. It aims to be very portable, serve as documentation for the Game Boy hardware (and an example for those wishing to write their own GB emulators), and provide numerous enhancements over the original handheld. The project is in its infancy, but most of the hard work has already been done. Still, there's more to be worked on. Only a handful of games are more or less perfectly and there's no sound at the moment.

Compiling GBE:

The only requirement at this moment is SDL and a C++ compiler. Linux users need only run the compile.sh script to build with G++. Windows users will have to wait.

Running GBE:

The program is CLI only at this time. GBE takes only two options right now. The first is always the path to the ROM file. Optionally, if the GB BIOS have been dumped, append "--bios" to the end of the command to have GBE run the BIOS (note: BIOS are not necessary to play games). The BIOS must be named "bios.bin" and put in the same directory as the GBE executable.