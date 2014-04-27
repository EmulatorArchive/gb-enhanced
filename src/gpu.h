// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : gpu.h
// Date : July 27, 2013
// Description : Game Boy GPU/LCD emulation
//
// Draws background, window, and sprites to screen
// Responsible for blitting pixel data and limiting frame rate

/****** GameBoy Emulated GPU ******/

#ifndef GB_GPU
#define GB_GPU

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

#include "common.h"
#include "mmu.h"
#include "config.h"
#include "hash.h"

struct gb_sprite
{
	u32 raw_data [0x80];
	u32 custom_data[0x80];
	u8 x;
	int y; //TODO: Find a better way to handle off-screen coordinates
	u8 tile_number;
	u8 options;
	std::string hash;
	bool custom_data_loaded;
};

struct gb_tile
{
	u32 raw_data[0x40];
	u32 custom_data[0x40];
	std::string hash;
	bool custom_data_loaded;
};

struct gbc_tile
{
	u32 raw_data[0x40];
};

class GPU
{
	public:
	
	MMU* mem_link;

	//Screen Data
	SDL_Surface* gpu_screen;
	SDL_Surface* src_screen;
	SDL_Surface* temp_screen;
	GLuint gpu_texture;

	//Core Functions
	GPU();
	~GPU();

	void step(int cpu_clock);
	void opengl_init();

	private:

	u8 gpu_mode;
	u8 gpu_mode_change;
	int gpu_clock;

	int frame_start_time;
	int frame_current_time;

	bool lcd_enabled;

	//Tile set data
	gb_tile tile_set_1[0x100];
	gb_tile tile_set_0[0x100];

	gbc_tile gbc_tile_set_1[0x100][2];
	gbc_tile gbc_tile_set_0[0x100][2];

	gbc_tile win_tile;
	gbc_tile bg_tile;

	//Pixel data
	u32 scanline_pixel_data [0x100];
	u32 final_pixel_data [0x10000];

	//Palettes
	u8 bgp[4];
	u8 obp[4][2];

	u16 sprite_colors_raw[4][8];
	u16 background_colors_raw[4][8];

	u32 sprite_colors_final[4][8];
	u32 background_colors_final[4][8]; 

	gb_sprite sprites[40];

	//HDMA
	u8 current_hdma_line;

	//Sprite Hash Data
	std::vector<std::string> sprite_hash_list;
	std::vector<u8> tile_set_0_updates;
	std::vector<u8> tile_set_1_updates;
	std::map<std::string, SDL_Surface*> custom_sprite_list;
	std::map<std::string, SDL_Surface*>::iterator custom_sprite_list_itr;

	void render_screen();
	void scanline_compare();
	void update_bg_tile();
	void update_gbc_bg_tile();

	void generate_scanline();
	void generate_sprites();

	void horizontal_flip(u16 width, u16 height, u32 pixel_data[]);
	void vertical_flip(u16 width, u16 height, u32 pixel_data[]);
	u8 signed_tile(u8 tile_number);

	//Custom graphics functions and variables
	void dump_sprites();
	void dump_bg_tileset_1();
	void dump_bg_tileset_0();
	void dump_bg_window();

	void load_sprites();
	void load_bg_tileset_1();
	void load_bg_tileset_0();

	u32 dump_mode;

	u32 dump_tile_0;
	u32 dump_tile_1;
	u32 dump_tile_win;

	u8 last_bgp;

	void opengl_blit();
};

#endif // GB_GPU