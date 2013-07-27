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
			if(config::cli_args[x] == "--opengl") { config::use_opengl = true; }
			else if(config::cli_args[x] == "--bios") { config::use_bios = true; }
			
			else 
			{
				std::cout<<"Error : Unknown argument - " << config::cli_args[x] << "\n";
				return false;
			}
		}

		return true;
	}
}
