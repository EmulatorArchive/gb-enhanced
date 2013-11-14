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

	pad = 0;

	jstick = NULL;
	jstick = SDL_JoystickOpen(0);

	if((jstick == NULL) && (SDL_NumJoysticks() >= 1)) { std::cout<<"Input : Could not initialize joystick \n"; }
	else if((jstick == NULL) && (SDL_NumJoysticks() == 0)) { std::cout<<"Input : No joysticks detected \n"; }
}

/****** GamePad Destructor *******/
GamePad::~GamePad() { }

/****** Handle Input From Keyboard ******/
void GamePad::handle_input(SDL_Event &event)
{
	//Key Presses
	if(event.type == SDL_KEYDOWN)
	{
		pad = event.key.keysym.sym;
		process_keyboard(pad, true);
	}

	//Key Releases
	else if(event.type == SDL_KEYUP)
	{
		pad = event.key.keysym.sym;
		process_keyboard(pad, false);
	}

	//Joystick Button Presses
	else if(event.type == SDL_JOYBUTTONDOWN)
	{
		pad = 100 + event.jbutton.button;
		process_joystick(pad, true);
	}

	//Joystick Button Releases
	else if(event.type == SDL_JOYBUTTONUP)
	{
		pad = 100 + event.jbutton.button;
		process_joystick(pad, false);
	}

	//Joystick axes
	else if(event.type == SDL_JOYAXISMOTION)
	{
		pad = 200 + (event.jaxis.axis * 2);
		int axis_pos = event.jaxis.value;
		if(axis_pos > 0) { pad++; }
		else { axis_pos *= -1; }

		if(axis_pos > config::dead_zone) { process_joystick(pad, true); }
		else { process_joystick(pad, false); }
	}

	//Joystick hats
        else if(event.type == SDL_JOYHATMOTION)
	{
		pad = 300;
		pad += event.jhat.hat * 4;

		switch(event.jhat.value)
		{
			case SDL_HAT_LEFT:
				process_joystick(pad, true);
				process_joystick(pad+2, false);
				break;

			case SDL_HAT_LEFTUP:
				process_joystick(pad, true);
				process_joystick(pad+2, true);
				break;

			case SDL_HAT_LEFTDOWN:
				process_joystick(pad, true);
				process_joystick(pad+3, true);
				break;

			case SDL_HAT_RIGHT:
				process_joystick(pad+1, true);
				process_joystick(pad+2, false);
				break;

			case SDL_HAT_RIGHTUP:
				process_joystick(pad+1, true);
				process_joystick(pad+2, true);
				break;

			case SDL_HAT_RIGHTDOWN:
				process_joystick(pad+1, true);
				process_joystick(pad+3, true);
				break;

			case SDL_HAT_UP:
				process_joystick(pad+2, true);
				process_joystick(pad, false);
				break;

			case SDL_HAT_DOWN:
				process_joystick(pad+3, true);
				process_joystick(pad, false);
				break;

			case SDL_HAT_CENTERED:
				process_joystick(pad, false);
				process_joystick(pad+2, false);
				break;
		}
	}
}

/****** Processes input based on unique pad # for keyboards ******/
void GamePad::process_keyboard(int pad, bool pressed)
{
	//Emulate A button press
	if((pad == config::key_a) && (pressed)) { p14 &= ~0x1; }

	//Emulate A button release
	else if((pad == config::key_a) && (!pressed)) { p14 |= 0x1; }

	//Emulate B button press
	else if((pad == config::key_b) && (pressed)) { p14 &= ~0x2; }

	//Emulate B button release
	else if((pad == config::key_b) && (!pressed)) { p14 |= 0x2; }

	//Emulate Select button press
	else if((pad == config::key_select) && (pressed)) { p14 &= ~0x4; }

	//Emulate Select button release
	else if((pad == config::key_select) && (!pressed)) { p14 |= 0x4; }

	//Emulate Start button press
	else if((pad == config::key_start) && (pressed)) { p14 &= ~0x8; }

	//Emulate Start button release
	else if((pad == config::key_start) && (!pressed)) { p14 |= 0x8; }

	//Emulate Right DPad press
	else if((pad == config::key_right) && (pressed)) { p15 &= ~0x1; p15 |= 0x2; }

	//Emulate Right DPad release
	else if((pad == config::key_right) && (!pressed)) { p15 |= 0x1; p15 |= 0x2;}

	//Emulate Left DPad press
	else if((pad == config::key_left) && (pressed)) { p15 &= ~0x2; p15 |= 0x1; }

	//Emulate Left DPad release
	else if((pad == config::key_left) && (!pressed)) { p15 |= 0x2; p15 |= 0x1; }

	//Emulate Up DPad press
	else if((pad == config::key_up) && (pressed)) { p15 &= ~0x4; p15 |= 0x8; }

	//Emulate Up DPad release
	else if((pad == config::key_up) && (!pressed)) { p15 |= 0x4; p15 |= 0x8;}

	//Emulate Down DPad press
	else if((pad == config::key_down) && (pressed)) { p15 &= ~0x8; p15 |= 0x4;}

	//Emulate Down DPad release
	else if((pad == config::key_down) && (!pressed)) { p15 |= 0x8; p15 |= 0x4; }
}

/****** Processes input based on unique pad # for joysticks ******/
void GamePad::process_joystick(int pad, bool pressed)
{
	//Emulate A button press
	if((pad == config::joy_a) && (pressed)) { p14 &= ~0x1; }

	//Emulate A button release
	else if((pad == config::joy_a) && (!pressed)) { p14 |= 0x1; }

	//Emulate B button press
	else if((pad == config::joy_b) && (pressed)) { p14 &= ~0x2; }

	//Emulate B button release
	else if((pad == config::joy_b) && (!pressed)) { p14 |= 0x2; }

	//Emulate Select button press
	else if((pad == config::joy_select) && (pressed)) { p14 &= ~0x4; }

	//Emulate Select button release
	else if((pad == config::joy_select) && (!pressed)) { p14 |= 0x4; }

	//Emulate Start button press
	else if((pad == config::joy_start) && (pressed)) { p14 &= ~0x8; }

	//Emulate Start button release
	else if((pad == config::joy_start) && (!pressed)) { p14 |= 0x8; }

	//Emulate Right DPad press
	else if((pad == config::joy_right) && (pressed)) { p15 &= ~0x1; p15 |= 0x2; }

	//Emulate Right DPad release
	else if((pad == config::joy_right) && (!pressed)) { p15 |= 0x1; p15 |= 0x2;}

	//Emulate Left DPad press
	else if((pad == config::joy_left) && (pressed)) { p15 &= ~0x2; p15 |= 0x1; }

	//Emulate Left DPad release
	else if((pad == config::joy_left) && (!pressed)) { p15 |= 0x2; p15 |= 0x1; }

	//Emulate Up DPad press
	else if((pad == config::joy_up) && (pressed)) { p15 &= ~0x4; p15 |= 0x8; }

	//Emulate Up DPad release
	else if((pad == config::joy_up) && (!pressed)) { p15 |= 0x4; p15 |= 0x8;}

	//Emulate Down DPad press
	else if((pad == config::joy_down) && (pressed)) { p15 &= ~0x8; p15 |= 0x4;}

	//Emulate Down DPad release
	else if((pad == config::joy_down) && (!pressed)) { p15 |= 0x8; p15 |= 0x4; }
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
