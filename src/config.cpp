// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : config.cpp
// Date : July 27, 2013
// Description : GBE configuration options
//
// Parses command-line arguments to configure GBE options

#include "config.h"

namespace config
{
	bool use_bios = false;
	bool use_opengl = false;
	std::string rom_file = "";
	std::vector <std::string> cli_args;
	bool use_scaling = false;
	int scaling_mode = 0;
	int scaling_factor = 1;

	//Default keyboard bindings
	//Arrow Z = A button, X = B button, START = Return, Select = Space
	//UP, LEFT, DOWN, RIGHT = Arrow keys
	int key_a = 122; int key_b = 120; int key_start = 13; int key_select = 32; 
	int key_left = 276; int key_right = 275; int key_down = 274; int key_up = 273;

	//Default joystick bindings
	int joy_a = 101; int joy_b = 100; int joy_start = 109; int joy_select = 108;
	int joy_left = 200; int joy_right = 201; int joy_up = 202; int joy_down = 203;

	//Default joystick dead-zone
	int dead_zone = 16000;
}

/****** Parse arguments passed from the command-line ******/
bool parse_cli_args()
{
	//If no arguments were passed, cannot run without ROM file
	if(config::cli_args.size() < 1) 
	{
		std::cout<<"Error : No ROM file in arguments \n";
		return false;
	}

	else 
	{
		//ROM file is always first argument
		config::rom_file = config::cli_args[0];
		
		for(int x = 1; x < config::cli_args.size(); x++)
		{
			//Use OpenGL hardware acceleration
			if(config::cli_args[x] == "--opengl") { config::use_opengl = true; }

			//Load and use GB BIOS
			else if(config::cli_args[x] == "--bios") { config::use_bios = true; }
			
			//Set scaling filter #1 - Nearest Neighbor 2x
			else if((config::cli_args[x] == "--f1") && (config::use_scaling == false))
			{
				config::scaling_mode = 1;
				config::use_scaling = true;
				config::scaling_factor = 2;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 2x\n";
			}

			//Set scaling filter #2 - Nearest Neighbor 3x
			else if((config::cli_args[x] == "--f2") && (config::use_scaling == false))
			{
				config::scaling_mode = 2;
				config::use_scaling = true;
				config::scaling_factor = 3;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 3x\n";
			}

			//Set scaling filter #2 - Nearest Neighbor 4x
			else if((config::cli_args[x] == "--f3") && (config::use_scaling == false))
			{
				config::scaling_mode = 3;
				config::use_scaling = true;
				config::scaling_factor = 4;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 4x\n";
			}
			
			else 
			{
				std::cout<<"Error : Unknown argument - " << config::cli_args[x] << "\n";
				return false;
			}
		}

		return true;
	}
}
