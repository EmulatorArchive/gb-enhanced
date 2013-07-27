// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : mmu.cpp
// Date : July 27, 2013
// Description : Game Boy memory manager unit
//
// Handles reading and writing bytes to memory locations
// Used to switch ROM and RAM banks
// Also loads ROM and BIOS files

/****** GameBoy Memory Manager Unit ******/ 

#include "mmu.h"
#include <iostream>

/****** MMU Constructor ******/
MMU::MMU() 
{ 
	in_bios = false; 

	gpu_update_bg_tile = false;
	gpu_update_sprite = false;
	gpu_reset_ticks = false;
	gpu_update_addr = 0;

	cart_mbc = 0;
	cart_rom_size = 0;
	cart_ram_size = 0;

	current_bank = 0;
	rom_bank = 0;
	bank_set = 0;
}

/****** MMU Deconstructor ******/
MMU::~MMU() { }

/****** Read byte from memory ******/
u8 MMU::read_byte(u16 address) 
{ 
	//Read from BIOS
	if(in_bios)
	{
		if(address < 0x100) { return bios[address]; }
		else if(address == 0x100) { in_bios = false; std::cout<<"MMU : Exiting BIOS \n"; }
	}

	//Read using ROM Banking
	if((address >= 0x4000) && (address <= 0x7FFF) && (memory_map[ROM_MBC] > 0) && (rom_bank >= 2))
	{
		return memory_bank[rom_bank-2][address-0x4000];
	}

	//Read from P1
	else if(address == 0xFF00) { return pad.read(); }

	//Read normally
	return memory_map[address]; 

}

/****** Read signed byte from memory ******/
s8 MMU::read_signed_byte(u16 address) 
{ 
	//Read from BIOS
	if((in_bios) && (address < 0x100))
	{
		u8 temp = bios[address];
		s8 s_temp = (s8)temp;
		return s_temp;
	}

	//Read normally
	u8 temp = memory_map[address];
	s8 s_temp = (s8)temp;
	return s_temp;
}

/****** Read word from memory ******/
u16 MMU::read_word(u16 address) 
{
	//Read from BIOS
	if((in_bios) && (address < 0x100))
	{
		u16 val = bios[address+1];
		val = (val << 8) | bios[address];
		return val;
	}

	//Read using ROM Banking
	if((address >= 0x4000) && (address <= 0x7FFF) && (memory_map[ROM_MBC] > 0) && (rom_bank >= 2))
	{

		u16 val = memory_bank[rom_bank-2][(address+1)-0x4000];
		val = (val << 8) | memory_bank[rom_bank-2][address-0x4000];
		return val;
	}

	//Read normally
	u16 val = memory_map[address+1];
	val = (val << 8) | memory_map[address];
	return val;
}

/****** Write Byte To Memory ******/
void MMU::write_byte(u16 address, u8 value) 
{
	//MBC register - Select ROM bank
	if((address >= 0x2000) && (address <= 0x3FFF)) { rom_bank = (value & 0x1F); }

	//MBC register - Select ROM bank Set
	if((address >= 0x4000) && (address <= 0x5FFF)) { bank_set = (value >> 6); }

	//P1 - Joypad register
	if(address == REG_P1) { pad.column_id = (value & 0x30); memory_map[REG_P1] = pad.read(); }

	//VRAM - Background tiles
	else if((address >= 0x8000) && (address <= 0x97FF))
	{
		memory_map[address] = value;
		gpu_update_bg_tile = true;
		gpu_update_addr = address;
	}

	//VRAM - Background map
	else if((address >= 0x9800) && (address <= 0x9FFF))
	{
		memory_map[address] = value;
	}	
	
	//BGP
	else if(address == REG_BGP)
	{
		memory_map[address] = value;
	}

	//Current scanline
	else if(address == REG_LY) 
	{ 
		memory_map[0xFF44] = 0;
	}

	//LCDC - Sprite mode changes
	else if(address == REG_LCDC)
	{
		u8 current_bit = (memory_map[REG_LCDC] & 0x04) ? 1 : 0;
		u8 new_bit = (value & 0x04) ? 1 : 0;

		//We're switching sprite modes, so update all sprites)
		if(current_bit != new_bit) { gpu_update_sprite = true; }

		memory_map[address] = value;
	}

	//DMA transfer
	else if(address == REG_DMA) 
	{
		u16 dma_orig = value << 8;
		u16 dma_dest = 0xFE00;
		while (dma_dest < 0xFEA0) { memory_map[dma_dest++] = memory_map[dma_orig++]; }
		gpu_update_sprite = true;
	}

	//Internal RAM - Write to ECHO RAM as well
	else if((address >= 0xC000) && (address <= 0xDFFF)) 
	{
		memory_map[address] = value;
		if(address + 0x2000 < 0xFDFF) { memory_map[address+0x2000] = value; }
	}

	//ECHO RAM - Write to Internal RAM as well
	else if((address >= 0xE000) && (address <= 0xFDFF))
	{
		memory_map[address] = value;
		memory_map[address-0x2000] = value;
	}

	//OAM - Direct writes
	else if((address >= 0xFE00) && (address <= 0xFEA0))
	{
		memory_map[address] = value;
		gpu_update_sprite = true;
	}

	else if(address > 0x7FFF) { memory_map[address] = value; }
}

/****** Read binary file to memory ******/
bool MMU::read_file(std::string filename)
{
	memset(memory_map, 0, sizeof(memory_map));
	memset(memory_bank, 0, sizeof(memory_bank));

	std::ifstream file(filename.c_str(), std::ios::binary);

	if(!file.is_open()) 
	{
		std::cout<<"MMU : " << filename << " could not be opened. Check file path or permission. \n";
		return false;
	}

	//Read 32KB worth of data from ROM file
	file.read((char*)memory_map, 0x8000);

	//Manually HLE MMIO
	if(!in_bios) 
	{
		write_byte(REG_LCDC, 0x91);
		write_byte(REG_BGP, 0xFC);
		write_byte(REG_OBP0, 0xFF);
		write_byte(REG_OBP1, 0xFF);
		write_byte(REG_P1, 0xFF);
		write_byte(REG_DIV, 0xAF);
		write_byte(REG_TAC, 0xF8);
	}

	//Determine MBC type
	switch(memory_map[ROM_MBC])
	{
		case 0: 
			std::cout<<"MMU : Cartridge Type - ROM Only \n";
			break;

		case 1:
			std::cout<<"MMU : Cartridge Type - MBC1 \n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 2: 
			std::cout<<"MMU : Cartridge Type - MBC1 + RAM \n";
			std::cout<<"MMU : MBC type currently unsupported \n";
			return false;

		default:
			std::cout<<"Catridge Type - 0x" << (int)memory_map[ROM_MBC] << "\n";
			std::cout<<"MMU : MBC type currently unsupported \n";
			return false;
	}

	//Read additional ROM data to banks
	if(memory_map[ROM_MBC] > 0)
	{
		//Use a file positioner
		u32 file_pos = 0x8000;
		u8 bank_count = 0;

		while(file_pos < (cart_rom_size * 1024))
		{
			file.read((char*)memory_bank[bank_count], 0x4000);
			file_pos += 0x4000;
			bank_count++;
		}
	}

	file.close();
	std::cout<<"MMU : " << filename << " loaded successfully. \n"; 

	return true;
}

/****** Write word to memory ******/
void MMU::write_word(u16 address, u16 value)
{
	memory_map[address] = value & 0xFF;
	memory_map[address+1] = value >> 8;
}

/****** Read GB BIOS ******/
bool MMU::read_bios(std::string filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary);

	if(!file.is_open()) 
	{
		std::cout<<"MMU : bios.bin could not be opened. Check file path or permission. \n";
		return false; 
	}

	//Read 256B BIOS
	file.read((char*)bios, 0x100);
	file.close();

	std::cout<<"MMU : bios.bin loaded successfully. \n";

	return true;
}
