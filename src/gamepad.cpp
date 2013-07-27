// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : gamepad.cpp
// Date : July 27, 2013
// Description : Game Boy joypad emulation and input handling
//
// Reads and writes to the P1 register
// Handles input from keyboard using SDL events

#include "gamepad.h"

/****** GamePad Constructor *******/
GamePad::GamePad()
{
	p14 = 0xDF;
	p15 = 0xEF;
	column_id = 0;
}
/****** GamePad Destructor *******/
GamePad::~GamePad() { }

/****** Handle Input From Keyboard ******/
void GamePad::handle_input(SDL_Event &event)
{
	//Key Presses
	if(event.type == SDL_KEYDOWN)
	{
		switch(event.key.keysym.sym)
		{

			case SDLK_RIGHT :
				p15 &= 0xFE;
				break;

			case SDLK_LEFT : 
				p15 &= 0xFD;
				break;

			case SDLK_UP :
				p15 &= 0xFB;
				break;

			case SDLK_DOWN :
				p15 &= 0xF7;
				break;

			case SDLK_z :
				p14 &= 0xFE;
				break;

			case SDLK_x :
				p14 &= 0xFD;
				break;

			case SDLK_SPACE :
				p14 &= 0xFB;
				break;

			case SDLK_RETURN :
				p14 &= 0xF7;
				break;
		}
	}

	//Key Releases
	else if(event.type == SDL_KEYUP)
	{
		switch(event.key.keysym.sym)
		{
			case SDLK_DOWN :
				p15 |= 0x8;
				break;

			case SDLK_UP :
				p15 |= 0x4;
				break;
	
			case SDLK_LEFT : 
				p15 |= 0x2;
				break;

			case SDLK_RIGHT :
				p15 |= 0x1;
				break;

			case SDLK_RETURN :
				p14 |= 0x8;
				break;

			case SDLK_SPACE :
				p14 |= 0x4;
				break;

			case SDLK_x :
				p14 |= 0x2;
				break;

			case SDLK_z :
				p14 |= 0x1;
				break;
		}
	}
}

/****** Update P1 ******/
u8 GamePad::read()
{
	switch(column_id)
	{
		case 0x20 :
			return p15;
			break;
		
		case 0x10 :
			return p14;
			break;

		default :
			return 0xFF;
	}
}
