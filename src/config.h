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

#include "common.h"

bool parse_cli_args();
bool parse_config_file();

namespace config
{ 
	extern bool use_bios;
	extern bool use_opengl;
	extern bool dump_sprites;
	extern bool load_sprites;
	extern u32 custom_sprite_transparency;
	extern std::string rom_file;
	extern std::vector <std::string> cli_args;
	extern bool use_scaling;
	extern int scaling_mode;
	extern int scaling_factor;
	extern int key_a, key_b, key_start, key_select, key_up, key_down, key_left, key_right;
	extern int joy_a, joy_b, joy_start, joy_select, joy_up, joy_down, joy_left, joy_right;
	extern int dead_zone;
	extern std::vector <u32> ini_parameters;
	extern u32 flags;
	extern bool turbo;
	extern bool mouse_click;
	extern u32 mouse_x;
	extern u32 mouse_y;
	extern u8 gb_type;
}

#endif // GB_CONFIG