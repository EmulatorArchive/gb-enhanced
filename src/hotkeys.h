// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : hotkeys.h
// Date : August 17, 2013
// Description : Process hotkeys
//
// Processes action for various hotkey actions
// Handles things like save states, screenshots, pausing, etc

#ifndef GB_HOTKEYS
#define GB_HOTKEYS

#include "SDL/SDL.h"
#include "z80.h"
#include "gpu.h"

void process_keys(CPU& z80, GPU& gb_gpu, SDL_Event& event);
void take_screenshot(GPU& gb_gpu);

#endif // GB_HOTKEYS


