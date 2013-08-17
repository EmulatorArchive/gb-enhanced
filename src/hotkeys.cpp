// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : hotkeys.cpp
// Date : August 17, 2013
// Description : Process hotkeys
//
// Processes action for various hotkey actions
// Handles things like save states, screenshots, pausing, etc

#include "hotkeys.h"

/****** Process key input - Do hotkey action or send input to Game Pad ******/
void process_keys(CPU& z80, GPU& gb_gpu, SDL_Event& event)
{
	//Quit on Q or ESC
	if((event.type == SDL_KEYDOWN) && ((event.key.keysym.sym == SDLK_q) || (event.key.keysym.sym == SDLK_ESCAPE)))
	{
		z80.running = false; 
		SDL_Quit();
	}

	//Send input to Game Pad if not a hotkey
	else if((event.type == SDL_KEYDOWN) || (event.type == SDL_KEYUP)) { z80.mem.pad.handle_input(event); }
}

