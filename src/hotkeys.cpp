// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : hotkeys.cpp
// Date : August 17, 2013
// Description : Process hotkeys
//
// Processes action for various hotkey actions
// Handles things like save states, screenshots, pausing, etc

#include <ctime>
#include <sstream>

#include "hotkeys.h"
#include "config.h"

/****** Process key input - Do hotkey action or send input to Game Pad ******/
void process_keys(CPU& z80, GPU& gb_gpu, SDL_Event& event)
{
	//Quit on Q or ESC
	if((event.type == SDL_KEYDOWN) && ((event.key.keysym.sym == SDLK_q) || (event.key.keysym.sym == SDLK_ESCAPE)))
	{
		z80.running = false; 
		SDL_Quit();
	}

	//Mouse coordinates
	else if(event.type == SDL_MOUSEMOTION)
	{
		config::mouse_x = event.motion.x;
		config::mouse_y = event.motion.y;
	}

	//Mouse click
	else if((event.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_LEFT)) { config::mouse_click = true; }

	//Screenshot on F9
	else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_F9)) { take_screenshot(gb_gpu); }

	//Switch between fullscreen and windowed mode
	else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_F10)) { toggle_fullscreen(gb_gpu); }

	//Temporarily disable disable framelimit on TAB
	else if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_TAB)) { config::turbo = true; }

	//Re-enable framelimit
	else if((event.type == SDL_KEYUP) && (event.key.keysym.sym == SDLK_TAB)) { config::turbo = false; }

	//Send input to Game Pad if not a hotkey
	else if((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP) 
	|| (event.type == SDL_JOYBUTTONDOWN) || (event.type == SDL_JOYBUTTONUP)
	|| (event.type == SDL_JOYAXISMOTION) || (event.type == SDL_JOYHATMOTION)) { z80.mem.pad.handle_input(event); }
}

/****** Takes screenshot - Accounts for image scaling ******/
void take_screenshot(GPU& gb_gpu)
{
	std::stringstream save_stream;
	std::string save_name = "";

	//Prefix SDL Ticks to screenshot name
	save_stream << SDL_GetTicks();
	save_name += save_stream.str();
	save_stream.str(std::string());

	//Append random number to screenshot name
	srand(SDL_GetTicks());
	save_stream << rand() % 1024 << rand() % 1024 << rand() % 1024;
	save_name += save_stream.str() + ".bmp";
	
	SDL_SaveBMP(gb_gpu.gpu_screen, save_name.c_str());
}

/****** Toggles between fullscreen mode and windowed mode ******/
void toggle_fullscreen(GPU& gb_gpu)
{
	//Switch flags
	if(config::flags == 0x80000000) { config::flags = 0; }
	else { config::flags = 0x80000000; }

	//Initialize the screen - account for scaling, fullscreen
	if((config::custom_sprite_scale > 1) && (!config::use_opengl) && (config::load_sprites)) 
	{ 
		gb_gpu.gpu_screen = SDL_SetVideoMode((160 * config::scaling_factor), (144 * config::scaling_factor), 32, SDL_SWSURFACE | config::flags); 
	}

	else if((!config::use_scaling) && (!config::use_opengl)) 
	{ 
		gb_gpu.gpu_screen = SDL_SetVideoMode(160, 144, 32, SDL_SWSURFACE | config::flags); 
	}
	
	else if((config::use_scaling) && (!config::use_opengl)) 
	{ 
		gb_gpu.gpu_screen = SDL_SetVideoMode((160 * config::scaling_factor), (144 * config::scaling_factor), 32, SDL_SWSURFACE | config::flags); 
	}
	
	else if(config::use_opengl) { gb_gpu.opengl_init(); }
}