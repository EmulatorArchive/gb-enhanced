// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : config.cpp
// Date : July 27, 2013
// Description : GBE configuration options
//
// Parses command-line arguments to configure GBE options

#include <iostream>
#include <fstream>
#include <sstream>

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
	std::vector <int> ini_parameters;

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

		//Make sure to only parse the first scaling argument passed
		bool scaling_parsed = false;
		
		for(int x = 1; x < config::cli_args.size(); x++)
		{
			//Use OpenGL hardware acceleration
			if(config::cli_args[x] == "--opengl") { config::use_opengl = true; }

			//Load and use GB BIOS
			else if(config::cli_args[x] == "--bios") { config::use_bios = true; }
			
			//Set scaling filter #1 - Nearest Neighbor 2x
			else if((config::cli_args[x] == "--f1") && (scaling_parsed == false))
			{
				config::scaling_mode = 1;
				config::use_scaling = true;
				config::scaling_factor = 2;
				scaling_parsed = true;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 2x\n";
			}

			//Set scaling filter #2 - Nearest Neighbor 3x
			else if((config::cli_args[x] == "--f2") && (scaling_parsed == false))
			{
				config::scaling_mode = 2;
				config::use_scaling = true;
				config::scaling_factor = 3;
				scaling_parsed = true;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 3x\n";
			}

			//Set scaling filter #2 - Nearest Neighbor 4x
			else if((config::cli_args[x] == "--f3") && (scaling_parsed == false))
			{
				config::scaling_mode = 3;
				config::use_scaling = true;
				config::scaling_factor = 4;
				scaling_parsed = true;
				std::cout<<"Scaling Filter : On \n";
				std::cout<<"Scaling Mode : Nearest Neighbor 4x\n";
			}

			//Warn users about passing multiple scaling methods
			else if((config::cli_args[x] == "--f1") || (config::cli_args[x] == "--f2")
			|| (config::cli_args[x] == "--f3") && (scaling_parsed == true))
			{
				std::cout<<"Warning : Multiple scaling filters selected. Only the first will be applied\n";
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

/****** Parse config file for values ******/
bool parse_config_file()
{
	std::ifstream file("gbe.ini", std::ios::in); 
	std::string input_line = "";
	std::string line_char = "";
	int output = 0;

	//Clear existing .ini parameters
	config::ini_parameters.clear();

	if(!file.is_open()) 
	{
		std::cout<<"Error : Could not open gbe.ini configuration file. Check file path or permissions. \n";
		return false; 
	}

	//Cycle through whole file, line-by-line
	while(getline(file, input_line))
	{
		line_char = input_line[0];	
	
		//Check if line starts with [ - if not, skip line
		if(line_char == "[")
		{
			std::string line_item = "";

			//Cycle through line, character-by-character
			for(int x = 0; ++x < input_line.length();)
			{
				line_char = input_line[x];

				//Check the character for item limiter : or ] - Push to Vector
				if((line_char == ":") || (line_char == "]")) 
				{
					//Convert string to int before pushing it to the vector
					output = 0;
					std::stringstream temp_stream(line_item);
					temp_stream >> output;
					config::ini_parameters.push_back(output); 
					line_item = ""; 
				}

				else { line_item += line_char; }
			}
		}
	}
	
	file.close();

	//Check for BIOS - 1st value in config file
	if(config::ini_parameters.size() >= 1) 
	{
		if(config::ini_parameters[0] == 1) { config::use_bios = true; }
	}

	//Check for OpenGL - 2nd value in config file
	if(config::ini_parameters.size() >= 2) 
	{
		if(config::ini_parameters[1] == 1) { config::use_opengl = true; }
	}

	//Check for Scaling Filter - 3rd value in config file
	if(config::ini_parameters.size() >= 3)
	{
		switch(config::ini_parameters[2])
		{
			//Nearest Neighbor 2x
			case 1:
				config::scaling_mode = 1;
				config::use_scaling = true;
				config::scaling_factor = 2;
				break;

			case 2:
				config::scaling_mode = 2;
				config::use_scaling = true;
				config::scaling_factor = 3;
				break;

			case 3:
				config::scaling_mode = 3;
				config::use_scaling = true;
				config::scaling_factor = 4;
				break;

			default:
				break;
		}
	}

	//Check for keyboard bindings
	if(config::ini_parameters.size() >= 11)
	{
		config::key_a = config::ini_parameters[3];
		config::key_b = config::ini_parameters[4];
		config::key_start = config::ini_parameters[5];
		config::key_select = config::ini_parameters[6];
		config::key_left = config::ini_parameters[7];
		config::key_right = config::ini_parameters[8];
		config::key_up = config::ini_parameters[9];
		config::key_down = config::ini_parameters[10];
	}

	//Check for joystick bindings
	if(config::ini_parameters.size() >= 19)
	{
		config::joy_a = config::ini_parameters[11];
		config::joy_b = config::ini_parameters[12];
		config::joy_start = config::ini_parameters[13];
		config::joy_select = config::ini_parameters[14];
		config::joy_left = config::ini_parameters[15];
		config::joy_right = config::ini_parameters[16];
		config::joy_up = config::ini_parameters[17];
		config::joy_down = config::ini_parameters[18];
	}	

	//Check for dead zone
	if(config::ini_parameters.size() >= 20)
	{
		config::dead_zone = config::ini_parameters[19];
	}

	return true;
}