// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : mmu.h
// Date : July 27, 2013
// Description : Game Boy memory manager unit
//
// Handles reading and writing bytes to memory locations
// Used to switch ROM and RAM banks
// Also loads ROM and BIOS files

#ifndef GB_MMU
#define GB_MMU

#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include "common.h"
#include "gamepad.h"

class MMU
{
	public:
		
	//Default Memory Map - 64KB
	u8 memory_map[0x10000];
	u8 bios [0x100];

	//Memory Banks
	std::vector< std::vector<u8> > read_only_bank;
	std::vector< std::vector<u8> > random_access_bank;

	//Working RAM Banks - GBC only
	std::vector< std::vector<u8> > working_ram_bank;

	u16 rom_bank;
	u8 ram_bank;
	u8 wram_bank;
	u8 bank_bits;
	u8 bank_mode;
	bool ram_banking_enabled;

	GamePad pad;

	bool in_bios;

	MMU();
	~MMU();

	u8 read_byte(u16 address);
	u16 read_word(u16 address);

	s8 read_signed_byte(u16 address);

	void write_byte(u16 address, u8 value);
	void write_word(u16 address, u16 value);

	bool read_file(std::string filename);
	bool read_bios(std::string filename);

	void save_sram();
	void grab_time();

	//Memory Bank Controller dedicated read/write operations
	void mbc_write(u16 address, u8 value);
	u8 mbc_read(u16 address);

	void mbc1_write(u16 address, u8 value);
	u8 mbc1_read(u16 address);

	void mbc2_write(u16 address, u8 value);
	u8 mbc2_read(u16 address);

	void mbc3_write(u16 address, u8 value);
	u8 mbc3_read(u16 address);

	void mbc5_write(u16 address, u8 value);
	u8 mbc5_read(u16 address);

	//Memory Bank Controller data
	enum cart_type{ ROM_ONLY, MBC1, MBC2, MBC3, MBC5 };
	cart_type mbc_type;
	u8 rtc_latch_1, rtc_latch_2, rtc_reg[5];
	bool cart_battery;
	bool cart_ram;
	bool cart_rtc;
	bool rtc_enabled;

	//Variables read by the GPU
	//TODO: Extern these into a seperate namespace, this is messy 
	bool gpu_update_bg_tile;
	bool gpu_update_sprite;
	bool gpu_reset_ticks;
	bool gpu_hdma_in_progress;
	u16 gpu_update_addr;

	//Variables read by the APU
	//TODO: Extern these into a separate namespace
	bool apu_update_channel;
	u16 apu_update_addr;

	//Cartridge Info
	u32 cart_rom_size;
	u32 cart_ram_size;
	
	std::string save_ram_file;
};

#endif // GB_MMU