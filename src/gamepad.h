// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : gamepad.h
// Date : July 27, 2013
// Description : Game Boy joypad emulation and input handling
//
// Reads and writes to the P1 register
// Handles input from keyboard using SDL events

#ifndef GB_GAMEPAD
#define GB_GAMEPAD

#include "SDL/SDL.h"
#include <string>
#include <iostream>

#include "common.h"

class GamePad
{
	public:

	u8 p14, p15;
	u8 column_id;

	GamePad();
	~GamePad();

	void handle_input(SDL_Event &event);
	u8 read();
};

#endif // GB_GAMEPAD
