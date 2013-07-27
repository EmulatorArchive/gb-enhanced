// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : config.h
// Date : July 27, 2013
// Description : GBE configuration options
//
// Parses command-line arguments to configure GBE options

#ifndef GB_CONFIG
#define GB_CONFIG

#include <vector>
#include <string>
#include <iostream>

bool parse_cli_args();

namespace config
{ 
	extern bool use_bios;
	extern bool use_opengl;
	extern std::string rom_file;
	extern std::vector <std::string> cli_args;
}

#endif // GB_CONFIG