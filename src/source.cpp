// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : source.cpp
// Date : July 27, 2013
// Description : Main program entry
//
// Other projects call this main.cpp or main.c
// I've always called it source.cpp :\ 

#include <iostream>
#include <iomanip>

#include "common.h"
#include "config.h"
#include "mmu.h"
#include "z80.h"
#include "gpu.h"
#include "hotkeys.h"

int main(int argc, char* args[]) 
{
	SDL_Event event;

	//Parse Command-line arguments
	for(int x = 0; x++ < argc - 1;) 
	{ 
		std::string temp_arg = args[x]; 
		config::cli_args.push_back(temp_arg);
	}

	if(!parse_cli_args()) { return 1; }

	std::cout<<"Initializing Z80 CPU... \n";
	CPU z80;
	
	std::cout<<"Initializing GPU... \n";
	GPU gb_gpu;

	//Link GPU and MMU
	gb_gpu.mem_link = &z80.mem;

	//Determine if BIOS are HLE'd or LLE'd - Reset CPU accordingly
	z80.mem.in_bios = config::use_bios;
	
	if(z80.mem.in_bios) { z80.reset_bios(); }
	else { z80.reset(); }

	u8 op = 0;
	
	//Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) 
	{
		std::cout<<"Error : Could not initialize SDL\n";
		return 1;
	}

	//Initialize the screen - account for scaling
	if(!config::use_scaling) { gb_gpu.gpu_screen = SDL_SetVideoMode(160, 144, 32, SDL_SWSURFACE); }
	else { gb_gpu.gpu_screen = SDL_SetVideoMode((160 * config::scaling_factor), (144 * config::scaling_factor), 32, SDL_SWSURFACE); }

	//Read BIOS
	if((z80.mem.in_bios) && (!z80.mem.read_bios("bios.bin"))) { return 1; }
	
	//Load ROM file
	if(!z80.mem.read_file(config::rom_file)) { return 1; }
	else { z80.running = true; }

	//Main loop
	while(z80.running)
	{
		//Handle SDL Events
		if((z80.mem.memory_map[REG_LY] == 144) && SDL_PollEvent(&event))
		{
			//X out of a window
			if(event.type == SDL_QUIT) { z80.running = false; SDL_Quit(); }
	
			//Handle hotkeys or Game Pad input
			else { process_keys(z80, gb_gpu, event); }
		}

		z80.cycles = 0;

		//Handle Interrupts
		z80.handle_interrupts();
	
		//Halt CPU if necessary
		if(z80.halt == true) { z80.cycles += 4; }

		else 
		{
			//Process Op Codes
			op = z80.mem.read_byte(z80.reg.pc++);
			z80.exec_op(op);
		}

		//Update Z80 clock
		z80.cpu_clock_t = z80.cycles;
		z80.cpu_clock_m = z80.cycles/4;

		//Update GPU
		gb_gpu.step(z80.cpu_clock_t);

		//Update DIV timer - Every 4 M clocks
		z80.div_counter += z80.cpu_clock_t;
		
		if(z80.div_counter >= 256) 
		{
			z80.div_counter -= 256;
			z80.mem.memory_map[REG_DIV]++;
		}

		//Update TIMA timer
		if(z80.mem.memory_map[REG_TAC] & 0x4) 
		{	
			z80.tima_counter += z80.cpu_clock_t;

			switch(z80.mem.memory_map[REG_TAC] & 0x3)
			{
				case 0x00: z80.tima_speed = 1024; break;
				case 0x01: z80.tima_speed = 16; break;
				case 0x02: z80.tima_speed = 64; break;
				case 0x03: z80.tima_speed = 256; break;
			}
	
			if(z80.tima_counter >= z80.tima_speed)
			{
				z80.mem.memory_map[REG_TIMA]++;
				z80.tima_counter -= z80.tima_speed;

				if(z80.mem.memory_map[REG_TIMA] == 0)
				{
					z80.mem.memory_map[REG_IF] |= 0x04;
					z80.mem.memory_map[REG_TIMA] = z80.mem.memory_map[REG_TMA];
				}	

			}
		}
	}

	//Save battery-backed RAM 
	z80.mem.save_sram();

	std::cout<<"Exiting... \n";
	return 0;
}

