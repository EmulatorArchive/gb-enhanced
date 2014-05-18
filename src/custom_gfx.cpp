// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : custom_gfx.cpp
// Date : March 10, 2014
// Description : Game Boy Enhanced custom graphics
//
// Handles dumping original BG and Sprite tiles or loading custom pixel data

#include "gpu.h"
#include "SDL/SDL_image.h"

/****** Takes 24-bit data from loaded image and converts it to 32-bit ARGB ******/
void GPU::load_image_data(int size, SDL_Surface* custom_source, u32 custom_dest[])
{
	int custom_bpp = custom_source->format->BitsPerPixel;
	
	if(SDL_MUSTLOCK(custom_source)){ SDL_LockSurface(custom_source); }

	//Load 24-bit data
	else if(custom_bpp == 24)
	{
		u8* custom_pixel_data = (u8*)custom_source->pixels;
		for(int a = 0, b = 0; a < size; a++, b+=3)
		{

			//Convert BMP data
			if(config::custom_sprite_type == 0) 
			{
				custom_dest[a] = 0xFF000000 | (custom_pixel_data[b+2] << 16) | (custom_pixel_data[b+1] << 8) | (custom_pixel_data[b]);
			}

			//Convert PNG data
			else 
			{
				custom_dest[a] = 0xFF000000 | (custom_pixel_data[b] << 16) | (custom_pixel_data[b+1] << 8) | (custom_pixel_data[b+2]);
			}
		}
	}

	//If a format other than 24-bit, just draw white pixels
	else 
	{
		for(int a = 0; a < size; a++)
		{
			custom_dest[a] = 0xFFFFFFFF;
		}
	}
	

	if(SDL_MUSTLOCK(custom_source)){ SDL_UnlockSurface(custom_source); }
}

/****** Dumps sprites to files ******/
void GPU::dump_sprites()
{
	SDL_Surface* custom_sprite = NULL;
	u8 sprite_height = 0;

	u16 hash_salt = ((mem_link->memory_map[REG_OBP0] << 8) | mem_link->memory_map[REG_OBP1]);

	//Determine if in 8x8 or 8x16 mode
	if(mem_link->memory_map[REG_LCDC] & 0x04) { sprite_height = 16; }
	else { sprite_height = 8; }

	//Read sprite pixel data
	for(int x = 0; x < 40; x++)
	{
		u16 sprite_tile_addr = (sprites[x].tile_number * 16) + 0x8000;

		sprites[x].hash = "";
		bool add_sprite_hash = true;

		//Create a hash for each sprite
		for(int a = 0; a < sprite_height/2; a++)
		{
			u16 temp_hash = mem_link->read_byte((a * 4) + sprite_tile_addr);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + sprite_tile_addr + 1);
			temp_hash = temp_hash ^ hash_salt;
			sprites[x].hash += raw_to_64(temp_hash);

			temp_hash = mem_link->read_byte((a * 4) + sprite_tile_addr + 2);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + sprite_tile_addr + 3);
			temp_hash = temp_hash ^ hash_salt;
			sprites[x].hash += raw_to_64(temp_hash);
		}

		//Update the sprite hash list
		for(int a = 0; a < sprite_hash_list.size(); a++)
		{
			if(sprites[x].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
		}

		//For new sprites, dump BMP file
		if(add_sprite_hash) 
		{ 
			sprite_hash_list.push_back(sprites[x].hash);

			u8 pal = sprites[x].options & 0x10 ? 1 : 0;
			custom_sprite = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, sprite_height, 32, 0, 0, 0, 0);
			std::string dump_file = "Dump/Sprites/" + sprites[x].hash + ".bmp";

			if(SDL_MUSTLOCK(custom_sprite)){ SDL_LockSurface(custom_sprite); }

			u32* dump_pixel_data = (u32*)custom_sprite->pixels;

			bool solid_color = true;

			//Generate RGBA values of the sprite for the dump file
			for(int a = 0; a < (8 * sprite_height); a++)
			{
				switch(obp[sprites[x].raw_data[a]][pal])
				{
					case 0: 
						dump_pixel_data[a] = 0xFFFFFFFF;
						break;

					case 1: 
						dump_pixel_data[a] = 0xFFC0C0C0;
						break;

					case 2: 
						dump_pixel_data[a] = 0xFF606060;
						break;

					case 3: 
						dump_pixel_data[a] = 0xFF000000;
						break;
				}

				if(dump_pixel_data[a] != dump_pixel_data[0]) { solid_color = false; }
			}

			//Reverse any flipping to get the original sprite's orentation
			if(sprites[x].options & 0x20) { horizontal_flip(8, sprite_height, dump_pixel_data); }
			if(sprites[x].options & 0x40) { vertical_flip(8, sprite_height, dump_pixel_data); }

			if(SDL_MUSTLOCK(custom_sprite)){ SDL_UnlockSurface(custom_sprite); }

			//If the dumped sprites are one solid color, it is usually safe to ignore it completely
			//Only dump the sprites here if it is not a solid color
			if(!solid_color)
			{
				//Save to BMP
				std::cout<<"GPU : Saving Sprite - " << dump_file << "\n";
				SDL_SaveBMP(custom_sprite, dump_file.c_str());
			}
		}
	}
}

/****** Dumps highlighted BG tiles to files - Primarily for custom graphics ******/
void GPU::dump_bg_tileset_1()
{
	SDL_Surface* custom_tile = NULL;

	u16 hash_salt = mem_link->memory_map[REG_BGP];
	
	//Dump BG tile from Tile Set 1
	u16 tile_addr = (dump_tile_1 * 16) + 0x8000;

	tile_set_1[dump_tile_1].hash = "";
	bool add_sprite_hash = true;

	//Create a hash for the tile
	for(int a = 0; a < 4; a++)
	{
		u16 temp_hash = mem_link->read_byte((a * 4) + tile_addr);
		temp_hash << 8;
		temp_hash += mem_link->read_byte((a * 4) + tile_addr + 1);
		temp_hash = temp_hash ^ hash_salt;
		tile_set_1[dump_tile_1].hash += raw_to_64(temp_hash);

		temp_hash = mem_link->read_byte((a * 4) + tile_addr + 2);
		temp_hash << 8;
		temp_hash += mem_link->read_byte((a * 4) + tile_addr + 3);
		temp_hash = temp_hash ^ hash_salt;
		tile_set_1[dump_tile_1].hash += raw_to_64(temp_hash);
	}

	//Update the sprite hash list
	for(int a = 0; a < sprite_hash_list.size(); a++)
	{
		if(tile_set_1[dump_tile_1].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
	}

	//For new tiles, dump BMP file
	if(add_sprite_hash) 
	{ 
		sprite_hash_list.push_back(tile_set_1[dump_tile_1].hash);

		custom_tile = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 32, 0, 0, 0, 0);
		std::string dump_file = "Dump/BG/" + tile_set_1[dump_tile_1].hash + ".bmp";

		if(SDL_MUSTLOCK(custom_tile)){ SDL_LockSurface(custom_tile); }

		u32* dump_pixel_data = (u32*)custom_tile->pixels;

		//Generate RGBA values of the sprite for the dump file
		for(int a = 0; a < 0x40; a++)
		{
			switch(bgp[tile_set_1[dump_tile_1].raw_data[a]])
			{
				case 0: 
					dump_pixel_data[a] = 0xFFFFFFFF;
					break;

				case 1: 
					dump_pixel_data[a] = 0xFFC0C0C0;
					break;

				case 2: 
					dump_pixel_data[a] = 0xFF606060;
					break;

				case 3: 
					dump_pixel_data[a] = 0xFF000000;
					break;
			}
		}

		if(SDL_MUSTLOCK(custom_tile)){ SDL_UnlockSurface(custom_tile); }

		//Save to BMP
		std::cout<<"GPU : Saving BG Tile - " << dump_file << "\n";
		SDL_SaveBMP(custom_tile, dump_file.c_str());
	}
}

/****** Dumps highlighted BG tiles to files - Primarily for custom graphics ******/
void GPU::dump_bg_tileset_0()
{
	SDL_Surface* custom_tile = NULL;

	u16 hash_salt = mem_link->memory_map[REG_BGP];
	
	//Dump BG tile from Tile Set 0
	u16 tile_addr = (dump_tile_0 * 16) + 0x8800;

	tile_set_0[dump_tile_0].hash = "";
	bool add_sprite_hash = true;

	//Create a hash for the tile
	for(int a = 0; a < 4; a++)
	{
		u16 temp_hash = mem_link->read_byte((a * 4) + tile_addr);
		temp_hash << 8;
		temp_hash += mem_link->read_byte((a * 4) + tile_addr + 1);
		temp_hash = temp_hash ^ hash_salt;
		tile_set_0[dump_tile_0].hash += raw_to_64(temp_hash);

		temp_hash = mem_link->read_byte((a * 4) + tile_addr + 2);
		temp_hash << 8;
		temp_hash += mem_link->read_byte((a * 4) + tile_addr + 3);
		temp_hash = temp_hash ^ hash_salt;
		tile_set_0[dump_tile_0].hash += raw_to_64(temp_hash);
	}

	//Update the sprite hash list
	for(int a = 0; a < sprite_hash_list.size(); a++)
	{
		if(tile_set_0[dump_tile_0].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
	}

	//For new tiles, dump BMP file
	if(add_sprite_hash) 
	{ 
		sprite_hash_list.push_back(tile_set_0[dump_tile_0].hash);

		custom_tile = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 32, 0, 0, 0, 0);
		std::string dump_file = "Dump/BG/" + tile_set_0[dump_tile_0].hash + ".bmp";

		if(SDL_MUSTLOCK(custom_tile)){ SDL_LockSurface(custom_tile); }

		u32* dump_pixel_data = (u32*)custom_tile->pixels;

		//Generate RGBA values of the sprite for the dump file
		for(int a = 0; a < 0x40; a++)
		{
			switch(bgp[tile_set_0[dump_tile_0].raw_data[a]])
			{
				case 0: 
					dump_pixel_data[a] = 0xFFFFFFFF;
					break;

				case 1: 
					dump_pixel_data[a] = 0xFFC0C0C0;
					break;

				case 2: 
					dump_pixel_data[a] = 0xFF606060;
					break;

				case 3: 
					dump_pixel_data[a] = 0xFF000000;
					break;
			}
		}

		if(SDL_MUSTLOCK(custom_tile)){ SDL_UnlockSurface(custom_tile); }

		//Save to BMP
		std::cout<<"GPU : Saving BG Tile - " << dump_file << "\n";
		SDL_SaveBMP(custom_tile, dump_file.c_str());
	}
}

/****** Dumps highlighted BG tiles to files ******/
void GPU::dump_bg_window()
{
	u32 temp_dump_tile = 0;

	if(dump_mode == 2) 
	{ 
		temp_dump_tile = dump_tile_1;
		dump_tile_1 = dump_tile_win;
		dump_bg_tileset_1();
		dump_tile_1 = temp_dump_tile;
	}

	else if(dump_mode == 3)
	{
		temp_dump_tile = dump_tile_0;
		dump_tile_0 = dump_tile_win;
		dump_bg_tileset_0();
		dump_tile_0 = temp_dump_tile;
	}
}

/****** Loads sprites from files ******/
void GPU::load_sprites()
{
	u8 high_byte, low_byte = 0;
	u8 high_bit, low_bit = 0;
	u8 final_byte = 0;

	u8 sprite_height = 0;

	SDL_Surface* custom_sprite = NULL;

	//Determine if in 8x8 or 8x16 mode
	if(mem_link->memory_map[REG_LCDC] & 0x04) { sprite_height = 16; }
	else { sprite_height = 8; }

	u16 hash_salt = ((mem_link->memory_map[REG_OBP0] << 8) | mem_link->memory_map[REG_OBP1]);

	//Read sprite pixel data
	for(int x = 0; x < 40; x++)
	{
		u16 sprite_tile_addr = (sprites[x].tile_number * 16) + 0x8000;

		sprites[x].hash = "";
		bool add_sprite_hash = true;

		//Create a hash for each sprite
		for(int a = 0; a < sprite_height/2; a++)
		{
			u16 temp_hash = mem_link->read_byte((a * 4) + sprite_tile_addr);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + sprite_tile_addr + 1);
			temp_hash = temp_hash ^ hash_salt;
			sprites[x].hash += raw_to_64(temp_hash);

			temp_hash = mem_link->read_byte((a * 4) + sprite_tile_addr + 2);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + sprite_tile_addr + 3);
			temp_hash = temp_hash ^ hash_salt;
			sprites[x].hash += raw_to_64(temp_hash);
		}

		//Search for already loaded custom sprite data
		custom_sprite_list_itr = custom_sprite_list.find(sprites[x].hash);

		//Check to see if hash exists already
		for(int a = 0; a < sprite_hash_list.size(); a++)
		{
			if(sprites[x].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
		}

		//If hash does not exist add it to the list, try to read custom sprite data and update map
		if(add_sprite_hash) 
		{ 
			sprite_hash_list.push_back(sprites[x].hash);

			std::string load_file = "";
			if(config::custom_sprite_type == 0) 
			{
				load_file = "Load/Sprites/" + sprites[x].hash + ".bmp";
				custom_sprite = SDL_LoadBMP(load_file.c_str()); 
			}

			else 
			{
				load_file = "Load/Sprites/" + sprites[x].hash + ".png";
				custom_sprite = IMG_Load(load_file.c_str());
			} 

			//Load custom sprite data into map and custom data
			if(custom_sprite != NULL) 
			{ 
				custom_sprite_list[sprites[x].hash] = custom_sprite; 
				std::cout<<"GPU : Loading custom sprite - " << load_file << "\n";

				//Account for sizes, e.g. 1:1 original, 2:1 original, 3:1 original, etc.
				u32 size = (custom_sprite_list[sprites[x].hash]->w * custom_sprite_list[sprites[x].hash]->h);
				sprites[x].custom_width = custom_sprite_list[sprites[x].hash]->w;
				sprites[x].custom_height = custom_sprite_list[sprites[x].hash]->h;

				//Verify sizes
				if((sprites[x].custom_width != (8 * config::custom_sprite_scale)) || (sprites[x].custom_height != (sprite_height * config::custom_sprite_scale)))
				{
					std::cout<<"GPU : Custom sprite - " << load_file << " was the wrong size! This will cause issues.\n";
				}

				if(sprites[x].custom_data.size() != size) { sprites[x].custom_data.resize(size, 0); }
				
				load_image_data(size, custom_sprite_list[sprites[x].hash], &sprites[x].custom_data[0]);

				sprites[x].custom_data_loaded = true;
			}

			else { sprites[x].custom_data_loaded = false; }
		}

		//If hash already exists in the list, try to read custom sprite data from the map
		else if((!add_sprite_hash) && (custom_sprite_list_itr != custom_sprite_list.end()))
		{	
			u32 size = (custom_sprite_list[sprites[x].hash]->w * custom_sprite_list[sprites[x].hash]->h);
			sprites[x].custom_width = custom_sprite_list[sprites[x].hash]->w;
			sprites[x].custom_height = custom_sprite_list[sprites[x].hash]->h;

			if(sprites[x].custom_data.size() != size) { sprites[x].custom_data.resize(size, 0); }

			load_image_data(size, custom_sprite_list[sprites[x].hash], &sprites[x].custom_data[0]);

			sprites[x].custom_data_loaded = true;
		}

		else { sprites[x].custom_data_loaded = false; }
	}
}

/****** Loads BG tiles from files ******/
void GPU::load_bg_tileset_1()
{
	SDL_Surface* custom_tile = NULL;

	u16 hash_salt = mem_link->memory_map[REG_BGP];

	//Load BG tiles from Tile Set 1
	for(int x = 0; x < tile_set_1_updates.size(); x++)
	{
		u16 tile_addr = (tile_set_1_updates[x] * 16) + 0x8000;

		tile_set_1[tile_set_1_updates[x]].hash = "";
		bool add_sprite_hash = true;

		//Create a hash for the tile
		for(int a = 0; a < 4; a++)
		{
			u16 temp_hash = mem_link->read_byte((a * 4) + tile_addr);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + tile_addr + 1);
			temp_hash = temp_hash ^ hash_salt;
			tile_set_1[tile_set_1_updates[x]].hash += raw_to_64(temp_hash);

			temp_hash = mem_link->read_byte((a * 4) + tile_addr + 2);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + tile_addr + 3);
			temp_hash = temp_hash ^ hash_salt;
			tile_set_1[tile_set_1_updates[x]].hash += raw_to_64(temp_hash);
		}	

		//Search for already loaded custom sprite data
		custom_sprite_list_itr = custom_sprite_list.find(tile_set_1[tile_set_1_updates[x]].hash);

		//Check to see if hash exists already
		for(int a = 0; a < sprite_hash_list.size(); a++)
		{
			if(tile_set_1[tile_set_1_updates[x]].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
		}

		//If hash does not exist add it to the list, try to read custom sprite data and update map
		if(add_sprite_hash) 
		{ 
			sprite_hash_list.push_back(tile_set_1[tile_set_1_updates[x]].hash);

			std::string load_file = "";
			if(config::custom_sprite_type == 0) 
			{
				load_file = "Load/BG/" + tile_set_1[tile_set_1_updates[x]].hash + ".bmp";
				custom_tile = SDL_LoadBMP(load_file.c_str()); 
			}

			else 
			{
				load_file = "Load/BG/" + tile_set_1[tile_set_1_updates[x]].hash + ".png";
				custom_tile = IMG_Load(load_file.c_str());
			} 

			//Load custom sprite data into map and raw data
			if(custom_tile != NULL) 
			{ 
				custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash] = custom_tile; 
				std::cout<<"GPU : Loading custom tile - " << load_file << "\n";
			
				//Account for sizes, e.g. 1:1 original, 2:1 original, 3:1 original, etc.
				u32 size = (custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->w * custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->h);

				//Verify sizes
				if((custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->w != (8 * config::custom_sprite_scale)) 
				|| (custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->h != (8 * config::custom_sprite_scale)))
				{
					std::cout<<"GPU : Custom tile - " << load_file << " was the wrong size! This will cause issues.\n";
				}

				if(tile_set_1[tile_set_1_updates[x]].custom_data.size() != size) { tile_set_1[tile_set_1_updates[x]].custom_data.resize(size, 0); }
		
				load_image_data(size, custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash], &tile_set_1[tile_set_1_updates[x]].custom_data[0]);

				tile_set_1[tile_set_1_updates[x]].custom_data_loaded = true;
			}

			else { tile_set_1[tile_set_1_updates[x]].custom_data_loaded = false; }
		}

		//If hash already exists in the list, try to read custom sprite data from the map
		else if((!add_sprite_hash) && (custom_sprite_list_itr != custom_sprite_list.end()))
		{	
			u32 size = (custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->w * custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash]->h);

			if(tile_set_1[tile_set_1_updates[x]].custom_data.size() != size) { tile_set_1[tile_set_1_updates[x]].custom_data.resize(size, 0); }

			load_image_data(size, custom_sprite_list[tile_set_1[tile_set_1_updates[x]].hash], &tile_set_1[tile_set_1_updates[x]].custom_data[0]);

			tile_set_1[tile_set_1_updates[x]].custom_data_loaded = true;
		}

		else { tile_set_1[tile_set_1_updates[x]].custom_data_loaded = false; }
	}

	//Clear tileset updates
	tile_set_1_updates.clear();
}

/****** Loads BG tiles from files ******/
void GPU::load_bg_tileset_0()
{
	SDL_Surface* custom_tile = NULL;

	u16 hash_salt = mem_link->memory_map[REG_BGP];

	//Load BG tiles from Tile Set 0
	for(int x = 0; x < tile_set_0_updates.size(); x++)
	{
		u16 tile_addr = (tile_set_0_updates[x] * 16) + 0x8800;

		tile_set_0[tile_set_0_updates[x]].hash = "";
		bool add_sprite_hash = true;

		//Create a hash for the tile
		for(int a = 0; a < 4; a++)
		{
			u16 temp_hash = mem_link->read_byte((a * 4) + tile_addr);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + tile_addr + 1);
			temp_hash = temp_hash ^ hash_salt;
			tile_set_0[tile_set_0_updates[x]].hash += raw_to_64(temp_hash);

			temp_hash = mem_link->read_byte((a * 4) + tile_addr + 2);
			temp_hash << 8;
			temp_hash += mem_link->read_byte((a * 4) + tile_addr + 3);
			temp_hash = temp_hash ^ hash_salt;
			tile_set_0[tile_set_0_updates[x]].hash += raw_to_64(temp_hash);
		}	

		//Search for already loaded custom sprite data
		custom_sprite_list_itr = custom_sprite_list.find(tile_set_0[tile_set_0_updates[x]].hash);

		//Check to see if hash exists already
		for(int a = 0; a < sprite_hash_list.size(); a++)
		{
			if(tile_set_0[tile_set_0_updates[x]].hash == sprite_hash_list[a]) { add_sprite_hash = false; }
		}

		//If hash does not exist add it to the list, try to read custom sprite data and update map
		if(add_sprite_hash) 
		{ 
			sprite_hash_list.push_back(tile_set_0[tile_set_0_updates[x]].hash);

			std::string load_file = "";
			if(config::custom_sprite_type == 0) 
			{
				load_file = "Load/BG/" + tile_set_0[tile_set_0_updates[x]].hash + ".bmp";
				custom_tile = SDL_LoadBMP(load_file.c_str()); 
			}

			else 
			{
				load_file = "Load/BG/" + tile_set_0[tile_set_0_updates[x]].hash + ".png";
				custom_tile = IMG_Load(load_file.c_str());
			} 

			//Load custom sprite data into map and raw data
			if(custom_tile != NULL) 
			{ 
				custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash] = custom_tile; 
				std::cout<<"GPU : Loading custom tile - " << load_file << "\n";

				//Account for sizes, e.g. 1:1 original, 2:1 original, 3:1 original, etc.
				u32 size = (custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->w * custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->h);

				//Verify sizes
				if((custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->w != (8 * config::custom_sprite_scale)) 
				|| (custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->h != (8 * config::custom_sprite_scale)))
				{
					std::cout<<"GPU : Custom tile - " << load_file << " was the wrong size! This will cause issues.\n";
				}

				if(tile_set_0[tile_set_0_updates[x]].custom_data.size() != size) { tile_set_0[tile_set_0_updates[x]].custom_data.resize(size, 0); }

				load_image_data(size, custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash], &tile_set_0[tile_set_0_updates[x]].custom_data[0]);

				tile_set_0[tile_set_0_updates[x]].custom_data_loaded = true;
			}

			else { tile_set_0[tile_set_0_updates[x]].custom_data_loaded = false; }
		}

		//If hash already exists in the list, try to read custom sprite data from the map
		else if((!add_sprite_hash) && (custom_sprite_list_itr != custom_sprite_list.end()))
		{
			u32 size = (custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->w * custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash]->h);

			if(tile_set_0[tile_set_0_updates[x]].custom_data.size() != size) { tile_set_0[tile_set_0_updates[x]].custom_data.resize(size, 0); }

			load_image_data(size, custom_sprite_list[tile_set_0[tile_set_0_updates[x]].hash], &tile_set_0[tile_set_0_updates[x]].custom_data[0]);

			tile_set_0[tile_set_0_updates[x]].custom_data_loaded = true;
		}

		else { tile_set_0[tile_set_0_updates[x]].custom_data_loaded = false; }
	}

	//Clear tileset updates
	tile_set_0_updates.clear();
}
