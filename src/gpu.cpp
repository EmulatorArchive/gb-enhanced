// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : gpu.cpp
// Date : July 27, 2013
// Description : Game Boy GPU/LCD emulation
//
// Draws background, window, and sprites to screen
// Responsible for blitting pixel data and limiting frame rate

#include "gpu.h"
#include "filter.h"

/****** GPU Constructor ******/
GPU::GPU() 
{
	gpu_mode = 0;
	gpu_clock = 0;
	frame_start_time = 0;
	frame_current_time = 0;
	gpu_screen = NULL;
	mem_link = NULL;
	lcd_enabled = false;
	
	src_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 256, 32, 0, 0, 0, 0);

	//Initialize a bunch of data to 0 - Let's avoid segfaults...
	memset(tile_set_1, 0, sizeof(tile_set_1));
	memset(tile_set_0, 0, sizeof(tile_set_0));
	memset(scanline_pixel_data, 0, sizeof(scanline_pixel_data));
	memset(final_pixel_data, 0, sizeof(final_pixel_data));

	for(int x = 0; x < 40; x++)
	{
		memset(sprites[x].raw_data, 0, sizeof(sprites[x].raw_data));
	}
}

/****** GPU Deconstructor ******/
GPU::~GPU() { }

/****** Flip pixel data horizontally - For sprites only ******/
void GPU::horizontal_flip(u16 width, u16 height, u32 pixel_data[])
{
	u32 target, origin = 0;

	for(u16 x = 0; x < height; x++)
	{
		for(u16 y = 0; y < width/2; y++)
		{
			target = (((x *width) + width) - y) - 1;
			origin = (x * width) + y;
			std::swap(pixel_data[target], pixel_data[origin]);
		}
	}
}

/****** Flip pixel data vertically - For sprites only ******/
void GPU::vertical_flip(u16 width, u16 height, u32 pixel_data[])
{
	u32 target, origin = 0;

	for(u16 x = 0; x < height/2; x++)
	{
		for(u16 y = 0; y < width; y++)
		{
			target = ((height - 1 - x) * width) + y;
			origin = (x * width) + y;
			std::swap(pixel_data[target], pixel_data[origin]);
		}
	}
}

/****** Converts signed tile numbers to regular tile numbers (0-255) ******/
u8 GPU::signed_tile(u8 tile_number) 
{
	if(tile_number <= 127)
	{
		tile_number += 128;
		return tile_number;
	}

	else 
	{ 
		tile_number -= 128;
		return tile_number;
	}
}

/****** Compares LY and LYC - Generates STAT Interrupt ******/
void GPU::scanline_compare()
{
	if(mem_link->memory_map[REG_LY] == mem_link->memory_map[REG_LYC]) 
	{ 
		mem_link->memory_map[REG_STAT] |= 0x4; 
		if(mem_link->memory_map[REG_STAT] & 0x40) { mem_link->memory_map[REG_IF] |= 2; }
	}
	else { mem_link->memory_map[REG_STAT] &= ~0x4; }
}

/****** Updates a specific tile ******/
void GPU::update_bg_tile()
{
	u8 high_byte, low_byte = 0;
	u8 high_bit, low_bit = 0;
	u8 final_byte = 0;

	u16 tile_addr = 0;
	u8 tile_number = 0;

	u16 pixel_counter = 0;

	//Update Tile Set #1
	if((mem_link->gpu_update_addr >= 0x8000) && (mem_link->gpu_update_addr <= 0x8FFF))
	{
		tile_number = (mem_link->gpu_update_addr - 0x8000)/16;
		tile_addr = 0x8000 + (tile_number * 16);

		//Generate tile for Tile Set #1
		for(int x = 0; x < 8; x++)
		{
			//Grab High and Low Bytes for Background Tile
			high_byte = mem_link->memory_map[tile_addr];
			low_byte = mem_link->memory_map[tile_addr+1];

			//Cycle through High and Low bytes
			for(int y = 7; y >= 0; y--)
			{
				high_bit = (high_byte >> y) & 0x01;
				low_bit = (low_byte >> y) & 0x01;
				final_byte = high_bit + (low_bit * 2);

				tile_set_1[tile_number].raw_data[pixel_counter] = final_byte;
				pixel_counter++;
			}
			
			//Move on to next address for tile
			tile_addr += 2;
		}
	}

	pixel_counter = 0;

	//Update Tile Set #0
	if((mem_link->gpu_update_addr >= 0x8800) && (mem_link->gpu_update_addr <= 0x97FF))
	{
		tile_number = (mem_link->gpu_update_addr - 0x8800)/16;
		tile_addr = 0x8800 + (tile_number * 16);

		//Generate tile for Tile Set #0
		for(int x = 0; x < 8; x++)
		{
			//Grab High and Low Bytes for Background Tile
			high_byte = mem_link->memory_map[tile_addr];
			low_byte = mem_link->memory_map[tile_addr+1];

			//Cycle through High and Low bytes
			for(int y = 7; y >= 0; y--)
			{
				high_bit = (high_byte >> y) & 0x01;
				low_bit = (low_byte >> y) & 0x01;
				final_byte = high_bit + (low_bit * 2);

				tile_set_0[tile_number].raw_data[pixel_counter] = final_byte;
				pixel_counter++;
			}
			
			//Move on to next address for tile
			tile_addr += 2;
		}
	}
}

/****** Prepares scanline for rendering - Pulls data from BG, Window, and Sprites ******/
void GPU::generate_scanline()
{
	u8 current_scanline = mem_link->memory_map[REG_LY] + mem_link->memory_map[REG_SY];
	u8 current_bgp = mem_link->memory_map[REG_BGP];
	u8 current_pixel = 0;

	u32 temp_scanline[0x100];

	u16 map_addr = 0;
	u16 tile_set_addr = 0;
	u8 map_entry = 0;
	u8 tile_pixel = 0;

	//Determine Tile Map Address
	if(mem_link->memory_map[REG_LCDC] & 0x08) { map_addr = 0x9C00; }
	else { map_addr = 0x9800; }

	//Determine Background/Window Palette - From lightest to darkest
	bgp[0] = current_bgp & 0x3;
	bgp[1] = (current_bgp >> 2) & 0x3;
	bgp[2] = (current_bgp >> 4) & 0x3;
	bgp[3] = (current_bgp >> 6) & 0x3;

	//Determine which tiles we should generate to get the scanline data - integer division ftw :p
	u16 tile_lower_range = (current_scanline/8) * 32;
	u16 tile_upper_range = tile_lower_range + 32;

	//Determine which line of the tiles we should generate pixels for this scanline
	u8 tile_line = current_scanline % 8;

	//Generate background pixel data for selected tiles
	for(int x = tile_lower_range; x < tile_upper_range; x++)
	{
		for(int y = (tile_line * 8); y < ((tile_line * 8) + 8); y++)
		{
			map_entry = mem_link->memory_map[map_addr + x];

			//Choose from the correct Tile Set
			if(mem_link->memory_map[REG_LCDC] & 0x10) { tile_pixel = tile_set_1[map_entry].raw_data[y]; }
			else { map_entry = signed_tile(map_entry); tile_pixel = tile_set_0[map_entry].raw_data[y]; }

			//Output Scanline data to RGBA
			switch(bgp[tile_pixel])
			{
				case 0: 
					scanline_pixel_data[current_pixel] = 0xFFFFFFFF;
					break;

				case 1: 
					scanline_pixel_data[current_pixel] = 0xFFC0C0C0;
					break;

				case 2: 
					scanline_pixel_data[current_pixel] = 0xFF606060;
					break;

				case 3: 
					scanline_pixel_data[current_pixel] = 0xFF000000;
					break;
			}

			current_pixel++;
		}
	}
			
	//Account for X-Scroll
	current_pixel = mem_link->memory_map[REG_SX];

	for(int x = 0; x < 0x100; x++)
	{
		final_pixel_data[(mem_link->memory_map[REG_LY] * 0x100) + x] = scanline_pixel_data[current_pixel];
		temp_scanline[x] = scanline_pixel_data[current_pixel];
		current_pixel++;
	}

	//TODO: Find a better fix
	for(int x = 0; x < 0x100; x++) { scanline_pixel_data[x] = temp_scanline[x]; }

	//Render Window Pixel Data
	if((mem_link->memory_map[REG_LY] - mem_link->memory_map[REG_WY] >= 0) && (mem_link->memory_map[REG_LCDC] & 0x20))
	{
		//Determine Tile Map Address
		if(mem_link->memory_map[REG_LCDC] & 0x40) { map_addr = 0x9C00; }
		else { map_addr = 0x9800; }

		current_pixel = mem_link->memory_map[REG_WX] - 7;
		u8 window_line = (mem_link->memory_map[REG_LY] - mem_link->memory_map[REG_WY]) % 8;

		//Determine which tiles we should generate to get the scanline data
		tile_lower_range = ((mem_link->memory_map[REG_LY] - mem_link->memory_map[REG_WY])/8) * 32;
		tile_upper_range = tile_lower_range + 32;

		//Generate window pixel data for selected tiles
		for(int x = tile_lower_range; x < tile_upper_range; x++)
		{
			for(int y = (window_line * 8); y < ((window_line * 8) + 8); y++)
			{
				map_entry = mem_link->memory_map[map_addr + x];

				//Choose from the correct Tile Set
				if(mem_link->memory_map[REG_LCDC] & 0x10) { tile_pixel = tile_set_1[map_entry].raw_data[y]; }
				else { map_entry = signed_tile(map_entry); tile_pixel = tile_set_0[map_entry].raw_data[y]; }
			
				//Output Scanline data to RGBA
				switch(bgp[tile_pixel])
				{
					case 0: 
						scanline_pixel_data[current_pixel] = 0xFFFFFFFF;
						break;

					case 1: 
						scanline_pixel_data[current_pixel] = 0xFFC0C0C0;
						break;

					case 2: 
						scanline_pixel_data[current_pixel] = 0xFF606060;
						break;

					case 3: 
						scanline_pixel_data[current_pixel] = 0xFF000000;
						break;
				}

				current_pixel++;
				if(current_pixel == 0) { x = tile_upper_range; break; }
			}
		}

		for(int x = 0, current_pixel = 0; x < 0x100; x++)
		{
			final_pixel_data[(mem_link->memory_map[REG_LY] * 0x100) + x] = scanline_pixel_data[current_pixel];
			current_pixel++;
		}
	}

	//Render Sprite Pixel Data
	if(mem_link->memory_map[REG_LCDC] & 0x02)
	{
		//Simple list to keep track of which sprites (10 max) to render for scanline
		u8 sprite_render_list[10];
		u8 sprite_render_line[10];
		u16 sprite_height = 0;
		int sprite_counter = -1;

		current_pixel = 0;

		//Determine if in 8x8 or 8x16 mode
		if(mem_link->memory_map[REG_LCDC] & 0x04) { sprite_height = 16; }
		else { sprite_height = 8; }

		//Cycle through all sprites to see if they're within height-range
		for(int x = 0; x < 40; x++)
		{
			//Check to see if X coordinate is on screen
			if(sprite_counter < 10)
			{
				//Check to see if Y coordinate is within rendering range
				for(int y = 0; y < sprite_height; y++)
				{
					if((sprites[x].y + y) == mem_link->memory_map[REG_LY])
					{
						sprite_counter++;
						sprite_render_list[sprite_counter] = x;
						sprite_render_line[sprite_counter] = y;
					}
				}
			}
		}

		//Cycle through sprite list
		for(int x = sprite_counter; x >= 0; x--)
		{
			u8 current_sprite = sprite_render_list[x];
			u8 sprite_line = sprite_render_line[x];
			u8 priority = sprites[current_sprite].options & 0x80 ? 1 : 0;
			u8 pal = sprites[current_sprite].options & 0x10 ? 1 : 0;

			current_pixel = sprites[current_sprite].x;

			//Draw each sprite
			for(int y = (sprite_line * 8); y < (sprite_line  * 8) + 8; y++)
			{
				bool draw_sprite_pixel = false;

				//If raw data is 0, that's the sprites transparency
				//In this case, we leave scanline data untouched
				if((sprites[current_sprite].raw_data[y] != 0) && (priority == 0)) { draw_sprite_pixel = true; }
				else if((sprites[current_sprite].raw_data[y] != 0) && (priority == 1) && (scanline_pixel_data[current_pixel] == 0xFFFFFFFF)) { draw_sprite_pixel = true; }

				if(draw_sprite_pixel) 
				{
					//Output Scanline data to RGBA
					switch(obp[sprites[current_sprite].raw_data[y]][pal])
					{
						case 0: 
							scanline_pixel_data[current_pixel] = 0xFFFFFFFF;
							break;

						case 1: 
							scanline_pixel_data[current_pixel] = 0xFFC0C0C0;
							break;

						case 2: 
							scanline_pixel_data[current_pixel] = 0xFF606060;
							break;

						case 3: 
							scanline_pixel_data[current_pixel] = 0xFF000000;
							break;
					}
				}

				current_pixel++;
			}
		}

		for(int x = 0, current_pixel = 0; x < 0x100; x++)
		{
			final_pixel_data[(mem_link->memory_map[REG_LY] * 0x100) + x] = scanline_pixel_data[current_pixel];
			current_pixel++;
		}
	}
}

/****** Prepares sprites for rendering - Pulls data from OAM, sets sprite palettes, etc ******/
void GPU::generate_sprites()
{
	u8 high_byte, low_byte = 0;
	u8 high_bit, low_bit = 0;
	u8 final_byte = 0;

	u16 sprite_tile_addr = 0;
	u16 sprite_height = 0;

	//Read sprite attributes from OAM
	for(int x = 0, y = 0; x < 40; x++, y+= 4)
	{
		sprites[x].y = mem_link->memory_map[OAM+y] - 16;
		sprites[x].x = mem_link->memory_map[OAM+y+1] - 8;
		sprites[x].tile_number = mem_link->memory_map[OAM+y+2];
		sprites[x].options = mem_link->memory_map[OAM+y+3];
	}

	//Determine if in 8x8 or 8x16 mode
	if(mem_link->memory_map[REG_LCDC] & 0x04) 
	{ 
		sprite_height = 16; 

		//Set LSB of tile number to 0
		for(int x = 0; x < 40; x++) { sprites[x].tile_number &= ~0x1; }
	}

	else { sprite_height = 8; }

	u8 sp_zero = mem_link->memory_map[REG_OBP0];
	u8 sp_one = mem_link->memory_map[REG_OBP1];

	//Determine Sprite Palettes - From lightest to darkest
	obp[0][0] = sp_zero  & 0x3;
	obp[1][0] = (sp_zero >> 2) & 0x3;
	obp[2][0] = (sp_zero >> 4) & 0x3;
	obp[3][0] = (sp_zero >> 6) & 0x3;

	obp[0][1] = sp_one  & 0x3;
	obp[1][1] = (sp_one >> 2) & 0x3;
	obp[2][1] = (sp_one >> 4) & 0x3;
	obp[3][1] = (sp_one >> 6) & 0x3;

	//Read sprite pixel data
	for(int x = 0; x < 40; x++)
	{
		sprite_tile_addr = (sprites[x].tile_number * 16) + 0x8000;
		u8 pixel_counter = 0;

		//Read Sprite Options
		u8 pal = sprites[x].options & 0x10 ? 1 : 0;
		u8 h_flip = sprites[x].options & 0x20 ? 1 : 0;
		u8 v_flip = sprites[x].options & 0x40 ? 1 : 0;

		//Cycles through tile
		for(int y = 0; y < sprite_height; y++)
		{
			//Grab High and Low Bytes for Tile
			high_byte = mem_link->memory_map[sprite_tile_addr];
			low_byte = mem_link->memory_map[sprite_tile_addr+1];

			//Cycle through High and Low bytes
			for(int z = 7; z >= 0; z--)
			{
				high_bit = (high_byte >> z) & 0x01;
				low_bit = (low_byte >> z) & 0x01;
				final_byte = high_bit + (low_bit * 2);

				sprites[x].raw_data[pixel_counter] = final_byte;
				pixel_counter++;
			}

			sprite_tile_addr += 2;
		}

		//Handle horizontal and vertical flipping
		if(h_flip == 1) { horizontal_flip(8, sprite_height, sprites[x].raw_data); }
		if(v_flip == 1) { vertical_flip(8, sprite_height, sprites[x].raw_data); }
	}
}
	

/****** Render final frame ******/
void GPU::render_screen() 
{
	//Lock source surface
	if(SDL_MUSTLOCK(src_screen)){ SDL_LockSurface(src_screen); }
	u32* out_pixel_data = (u32*)src_screen->pixels;

	//Draw background to framebuffer
	if(mem_link->memory_map[REG_LCDC] & 0x1) 
	{
		for(int a = 0; a < 0x10000; a++) { out_pixel_data[a] = final_pixel_data[a]; }
	}

	//Unlock source surface
	if(SDL_MUSTLOCK(src_screen)){ SDL_UnlockSurface(src_screen); }

	//Scale the source image...
	if(config::use_scaling) 
	{ 
		SDL_Surface* temp_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, (256 * config::scaling_factor), (256 * config::scaling_factor), 32, 0, 0, 0, 0);
		apply_scaling(src_screen, temp_screen);
		SDL_BlitSurface(temp_screen, 0, gpu_screen, 0);
		SDL_FreeSurface(temp_screen);
	}
	
	//Or just blit to unscaled image to screen
	else { SDL_BlitSurface(src_screen, 0, gpu_screen, 0); }

	if(SDL_Flip(gpu_screen) == -1) { std::cout<<"Could not blit? \n"; }

	//Limit FPS to 60
	frame_current_time = SDL_GetTicks();
	if((frame_current_time - frame_start_time) < (1000/60)) { SDL_Delay((1000/60) - (frame_current_time - frame_start_time));}
	else { std::cout<<"GPU : Late Blit\n"; }
}

/****** Execute GPU Operations ******/
void GPU::step(int cpu_clock) 
{
	if(mem_link->gpu_reset_ticks) { gpu_clock = 0; mem_link->gpu_reset_ticks = false; }

	//Enable LCD
	if(mem_link->memory_map[REG_LCDC] & 0x80) { lcd_enabled = true; }

	//Update background tile
	if(mem_link->gpu_update_bg_tile)
	{ 
		update_bg_tile();
		mem_link->gpu_update_bg_tile = false;
	}

	//Update sprites
	if(mem_link->gpu_update_sprite)
	{
		generate_sprites();
		mem_link->gpu_update_sprite = false;
	}

	//Perform LCD operations when only LCD enabled
	if(lcd_enabled)
	{
		gpu_clock += cpu_clock;

		//VBlank - Mode 1
		if(gpu_clock >= 456)
		{
			gpu_clock -= 456;
			generate_scanline();
			scanline_compare();
			mem_link->memory_map[REG_LY]++; //This is a problem. See Mega Man STAT Interrupt (likely others).

			if(mem_link->memory_map[REG_LY] >= 154) { mem_link->memory_map[REG_LY] -= 154; }

			//VBlank
			if(mem_link->memory_map[REG_LY] == 144)
			{
				gpu_mode = 1;
				render_screen();
				frame_start_time = SDL_GetTicks();
				mem_link->memory_map[REG_IF] |= 1;

				//Disable LCD - Must be done during VBlank only
				if(!(mem_link->memory_map[REG_LCDC] & 0x80)) 
				{ 
					lcd_enabled = false; 
					mem_link->memory_map[REG_LY] = 0; 
					gpu_clock = 0; 
				}
			}
		}

		//If not in VBlank
		//TODO: Add the rest of the STAT interrupts here
		if(mem_link->memory_map[REG_LY] < 144)
		{
			//HBlank - Mode 0
			if(gpu_clock <= 204) { gpu_mode = 0; }
		
			//OAM Read - Mode 2
			else if(gpu_clock <= 284) { gpu_mode = 2; }

			//VRAM Read - Mode 3
			else { gpu_mode = 3; }
		}

		mem_link->memory_map[REG_STAT] = (mem_link->memory_map[REG_STAT] & ~0x3) + gpu_mode;
	}
}
			

			
