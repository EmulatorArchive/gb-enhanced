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
#include <ctime>

/****** MMU Constructor ******/
MMU::MMU() 
{ 
	in_bios = false; 

	gpu_update_bg_tile = false;
	gpu_update_sprite = false;
	gpu_reset_ticks = false;
	gpu_update_addr = 0;
	gpu_hdma_in_progress = false;
	gpu_update_sprite_colors = false;
	gpu_update_bg_colors = false;

	apu_update_channel = false;
	apu_update_addr = 0;

	cart_rom_size = 0;
	cart_ram_size = 0;

	mbc_type = ROM_ONLY;
	cart_battery = false;
	cart_ram = false;
	cart_rtc = false;
	rtc_enabled = false;
	rtc_latch_1 = rtc_latch_2 = 0xFF;

	rom_bank = 1;
	ram_bank = 0;
	wram_bank = 1;
	vram_bank = 0;
	bank_bits = 0;
	bank_mode = 0;
	ram_banking_enabled = false;

	save_ram_file = "";

	read_only_bank.resize(0x200);
	for(int x = 0; x < 0x200; x++) { read_only_bank[x].resize(0x4000, 0); }

	random_access_bank.resize(0x10);
	for(int x = 0; x < 0x10; x++) { random_access_bank[x].resize(0x2000, 0); }

	working_ram_bank.resize(0x8);
	for(int x = 0; x < 0x8; x++) { working_ram_bank[x].resize(0x1000, 0); }

	video_ram.resize(0x2);
	for(int x = 0; x < 0x2; x++) { video_ram[x].resize(0x2000, 0); }
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
	if((address >= 0x4000) && (address <= 0x7FFF) && (mbc_type != ROM_ONLY))
	{
		return mbc_read(address);
	}

	//Read using RAM Banking
	if((address >= 0xA000) && (address <= 0xBFFF) && (cart_ram) && (mbc_type != ROM_ONLY))
	{
		return mbc_read(address);
	}

	//Read from VRAM, GBC uses banking
	if((address >= 0x8000) && (address <= 0x9FFF))
	{
		//GBC read from VRAM Bank 1
		if((vram_bank == 1) && (config::gb_type == 2)) { return video_ram[1][address-0x8000]; }
		
		//GBC read from VRAM Bank 0 - DMG read normally, also from Bank 0, though it doesn't use banking technically
		else { return video_ram[0][address-0x8000]; }
	}

	//In GBC mode, read from Working RAM using Banking
	if((address >= 0xC000) && (address <= 0xDFFF) && (config::gb_type == 2)) 
	{
		//Read from Bank 0 always when address is within 0xC000 - 0xCFFF
		if((address >= 0xC000) && (address <= 0xCFFF)) { return working_ram_bank[0][address-0xC000]; }
			
		//Read from selected Bank when address is within 0xD000 - 0xDFFF
		else if((address >= 0xD000) && (address <= 0xDFFF)) { return working_ram_bank[wram_bank][address-0xD000]; }
	}

	//Read background color palette data
	if(address == REG_BCPD)
	{ 
		u8 hi_lo = (memory_map[REG_BCPS] & 0x1);
		u8 color = (memory_map[REG_BCPS] >> 1) & 0x3;
		u8 palette = (memory_map[REG_BCPS] >> 3) & 0x7;

		//Read lower-nibble of color
		if(hi_lo == 0) 
		{ 
			return (background_colors_raw[color][palette] & 0xFF);
		}

		//Read upper-nibble of color
		else
		{
			return (background_colors_raw[color][palette] >> 8);
		}
	}
	

	//Read sprite color palette data
	if(address == REG_OCPD) 
	{ 
		u8 hi_lo = (memory_map[REG_OCPS] & 0x1);
		u8 color = (memory_map[REG_OCPS] >> 1) & 0x3;
		u8 palette = (memory_map[REG_OCPS] >> 3) & 0x7;

		//Read lower-nibble of color
		if(hi_lo == 0) 
		{ 
			return (sprite_colors_raw[color][palette] & 0xFF);
		}

		//Read upper-nibble of color
		else
		{
			return (sprite_colors_raw[color][palette] >> 8);
		}
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
	u8 temp = read_byte(address);
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

	//Read normally
	u16 val = read_byte(address+1);
	val = (val << 8) | read_byte(address);
	return val;
}

/****** Write Byte To Memory ******/
void MMU::write_byte(u16 address, u8 value) 
{
	if(mbc_type != ROM_ONLY) { mbc_write(address, value); }

	//Read from VRAM, GBC uses banking
	if((address >= 0x8000) && (address <= 0x9FFF))
	{
		//GBC read from VRAM Bank 1
		if((vram_bank == 1) && (config::gb_type == 2)) { video_ram[1][address-0x8000] = value; }
		
		//GBC read from VRAM Bank 0 - DMG read normally, also from Bank 0, though it doesn't use banking technically
		else { video_ram[0][address-0x8000] = value; }

		//VRAM - Background tiles update
		if((address >= 0x8000) && (address <= 0x97FF))
		{
			gpu_update_bg_tile = true;
			gpu_update_addr = address;
			if(address <= 0x8FFF) { gpu_update_sprite = true; }
		}
	}	
	
	//BGP
	else if(address == REG_BGP)
	{
		gpu_update_bg_tile = true;
		gpu_update_addr = address;
		memory_map[address] = value;
	}

	//OBP0 and OBP1
	else if((address == REG_OBP0) || (address == REG_OBP1))
	{
		gpu_update_sprite = true;
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
		while (dma_dest < 0xFEA0) { write_byte(dma_dest++, read_byte(dma_orig++)); }
		gpu_update_sprite = true;
	}

	//Internal RAM - Write to ECHO RAM as well
	else if((address >= 0xC000) && (address <= 0xDFFF)) 
	{
		//DMG mode - Normal writes
		if(config::gb_type != 2)
		{
			memory_map[address] = value;
			if(address + 0x2000 < 0xFDFF) { memory_map[address+0x2000] = value; }
		}

		//GBC mode - Use banks
		else if(config::gb_type == 2)
		{
			//Write to Bank 0 always when address is within 0xC000 - 0xCFFF
			if((address >= 0xC000) && (address <= 0xCFFF)) { working_ram_bank[0][address-0xC000] = value; }
			
			//Write to selected Bank when address is within 0xD000 - 0xDFFF
			else if((address >= 0xD000) && (address <= 0xDFFF)) { working_ram_bank[wram_bank][address-0xD000] = value; }
		}
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

	//P1 - Joypad register
	else if(address == REG_P1) { pad.column_id = (value & 0x30); memory_map[REG_P1] = pad.read(); }

	//Update Sound Channels
	else if((address >= 0xFF10) && (address <= 0xFF25)) 
	{
		memory_map[address] = value;
		apu_update_channel = true; 
		apu_update_addr = address; 
	}

	//HDMA transfer
	else if(address == REG_HDMA5)
	{
		//Halt Horizontal DMA transfer if one is already in progress and 0 is now written to Bit 7
		if((memory_map[address] & 0x80) && ((value & 0x80) == 0)) { gpu_hdma_in_progress = false; }
		else { gpu_hdma_in_progress = true; }

		memory_map[address] = value;
	}

	//VBK - Update VRAM bank
	else if(address == REG_VBK) 
	{ 
		vram_bank = value & 0x1; 
		memory_map[address] = value; 
	}
		

	//BCPD - Update background color palettes
	else if(address == REG_BCPD)
	{
		gpu_update_bg_colors = true;
		memory_map[address] = value;
	}

	//OCPD - Update sprite color palettes
	else if(address == REG_OCPD)
	{
		gpu_update_sprite_colors = true;
		memory_map[address] = value;
	}

	//SVBK - Update Working RAM bank
	else if(address == REG_SVBK) 
	{
		wram_bank = value & 0x7;
		if(wram_bank == 0) { wram_bank = 1; }
		memory_map[address] = value;
	}

	else if(address > 0x7FFF) { memory_map[address] = value; }
}

/****** Write word to memory ******/
void MMU::write_word(u16 address, u16 value)
{
	write_byte(address, (value & 0xFF));
	write_byte((address+1), (value >> 8));
}

/****** Determines which if any MBC to read from ******/
u8 MMU::mbc_read(u16 address)
{
	switch(mbc_type)
	{
		case MBC1:
			return mbc1_read(address);
			break;

		case MBC2:
			return mbc2_read(address);
			break;

		case MBC3:
			return mbc3_read(address);
			break;

		case MBC5:
			return mbc5_read(address);
			break;
	}
}

/****** Determines which if any MBC to write to ******/
void MMU::mbc_write(u16 address, u8 value)
{
	switch(mbc_type)
	{
		case MBC1:
			mbc1_write(address, value);
			break;

		case MBC2:
			mbc2_write(address, value);
			break;

		case MBC3:
			mbc3_write(address, value);
			break;

		case MBC5:
			mbc5_write(address, value);
			break;
	}
}

/****** Read binary file to memory ******/
bool MMU::read_file(std::string filename)
{
	memset(memory_map, 0, sizeof(memory_map));

	std::ifstream file(filename.c_str(), std::ios::binary);

	if(!file.is_open()) 
	{
		std::cout<<"MMU : " << filename << " could not be opened. Check file path or permissions. \n";
		return false;
	}

	//Read 32KB worth of data from ROM file
	file.read((char*)memory_map, 0x8000);

	//Manually HLE MMIO
	if(!in_bios) 
	{
		memory_map[REG_LCDC] = 0x91;
		memory_map[REG_BGP] = 0xFC;
		memory_map[REG_OBP0] = 0xFF;
		memory_map[REG_OBP1] = 0xFF;
		memory_map[REG_P1] = 0xFF;
		memory_map[REG_DIV] = 0xAF;
		memory_map[REG_TAC] = 0xF8;
		memory_map[0xFF10] = 0x80;
		memory_map[0xFF11] = 0xBF;
   		memory_map[0xFF12] = 0xF3; 
  		memory_map[0xFF14] = 0xBF; 
   		memory_map[0xFF16] = 0x3F; 
   		memory_map[0xFF17] = 0x00; 
   		memory_map[0xFF19] = 0xBF; 
   		memory_map[0xFF1A] = 0x7F; 
   		memory_map[0xFF1B] = 0xFF; 
   		memory_map[0xFF1C] = 0x9F; 
   		memory_map[0xFF1E] = 0xBF; 
   		memory_map[0xFF20] = 0xFF; 
   		memory_map[0xFF21] = 0x00; 
   		memory_map[0xFF22] = 0x00; 
   		memory_map[0xFF23] = 0xBF; 
   		memory_map[0xFF24] = 0x77; 
   		memory_map[0xFF25] = 0xF3; 
	}

	//Determine MBC type
	switch(memory_map[ROM_MBC])
	{
		case 0x0: 
			mbc_type = ROM_ONLY;

			std::cout<<"MMU : Cartridge Type - ROM Only \n";
			break;

		case 0x1:
			mbc_type = MBC1;

			std::cout<<"MMU : Cartridge Type - MBC1 \n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x2: 
			mbc_type = MBC1;
			cart_ram = true;

			std::cout<<"MMU : Cartridge Type - MBC1 + RAM \n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x3:
			mbc_type = MBC1;
			cart_ram = true;
			cart_battery = true;

			std::cout<<"MMU : Cartridge Type - MBC1 + RAM + Battery \n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x5:
			mbc_type = MBC2;
			cart_ram = true;

			std::cout<<"MMU : Cartridge Type - MBC2 \n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x6:
			mbc_type = MBC2;
			cart_ram = true;
			cart_battery = true;

			std::cout<<"MMU : Cartridge Type - MBC2 + Battery\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x10:
			mbc_type = MBC3;
			cart_ram = true;
			cart_battery = true;
			cart_rtc = true;

			std::cout<<"MMU : Cartridge Type - MBC3 + RAM + Battery + Timer\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";

			grab_time();

			break;

		case 0x11:
			mbc_type = MBC3;

			std::cout<<"MMU : Cartridge Type - MBC3\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x12:
			mbc_type = MBC3;
			cart_ram = true;

			std::cout<<"MMU : Cartridge Type - MBC3 + RAM\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x13:
			mbc_type = MBC3;
			cart_ram = true;
			cart_battery = true;

			std::cout<<"MMU : Cartridge Type - MBC3 + RAM + Battery\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x19:
			mbc_type = MBC5;

			std::cout<<"MMU : Cartridge Type - MBC5\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x1A:
			mbc_type = MBC5;
			cart_ram = true;

			std::cout<<"MMU : Cartridge Type - MBC5 + RAM\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x1B:
			mbc_type = MBC5;
			cart_ram = true;
			cart_battery = true;

			std::cout<<"MMU : Cartridge Type - MBC5 + RAM + Battery\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x1C:
			mbc_type = MBC5;

			std::cout<<"MMU : Cartridge Type - MBC5 + Rumble\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;
			
		case 0x1D:
			mbc_type = MBC5;
			cart_ram = true;

			std::cout<<"MMU : Cartridge Type - MBC5 + RAM + Rumble\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		case 0x1E:
			mbc_type = MBC5;
			cart_ram = true;
			cart_battery = true;

			std::cout<<"MMU : Cartridge Type - MBC5 + RAM + Battery + Rumble\n";
			cart_rom_size = 32 << memory_map[ROM_ROMSIZE];
			std::cout<<"MMU : ROM Size - " << cart_rom_size << "KB\n";
			break;

		default:
			std::cout<<"Catridge Type - 0x" << std::hex << (int)memory_map[ROM_MBC] << "\n";
			std::cout<<"MMU : MBC type currently unsupported \n";
			return false;
	}

	//Read additional ROM data to banks
	if(mbc_type != ROM_ONLY)
	{
		//Use a file positioner
		u32 file_pos = 0x8000;
		u8 bank_count = 0;

		while(file_pos < (cart_rom_size * 1024))
		{
			file.read(reinterpret_cast<char*> (&read_only_bank[bank_count][0]), 0x4000);
			file_pos += 0x4000;
			bank_count++;
		}
	}

	file.close();
	std::cout<<"MMU : " << filename << " loaded successfully. \n"; 

	//Load Saved RAM if available
	if(cart_battery)
	{
		save_ram_file = filename.substr(0, (filename.length() - 2)) + "sram";
		std::ifstream sram(save_ram_file.c_str(), std::ios::binary);

		if(!sram.is_open()) { std::cout<<"MMU : " << save_ram_file << " battery file could not be opened. Check file path or permission\n"; }

		else 
		{
			for(int x = 0; x < 0x10; x++)
			{
				sram.read(reinterpret_cast<char*> (&random_access_bank[x][0]), 0x2000); 
			}
		}

		sram.close();
	}

	//Determine if cart is DMG or GBC and which system GBE will try to emulate
	//Only necessary for Auto system detection.
	//For now, even if forcing GBC, when encountering DMG carts, revert to DMG mode, dunno how the palettes work yet
	if(memory_map[ROM_COLOR] == 0) { config::gb_type = 1; }
	else if((memory_map[ROM_COLOR] == 0x80) && (config::gb_type == 0)) { config::gb_type = 2; }
	else if((memory_map[ROM_COLOR] == 0xC0) && (config::gb_type == 0)) { config::gb_type = 2; }

	return true;
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

/****** Save battery-backed RAM to file ******/
void MMU::save_sram()
{
	if(cart_battery)
	{
		std::ofstream sram(save_ram_file.c_str(), std::ios::binary);

		if(!sram.is_open()) { std::cout<<"MMU :  " << save_ram_file << " battery file could not be saved. Check file path or permission\n";  }

		else 
		{
			for(int x = 0; x < 0x10; x++)
			{
				sram.write(reinterpret_cast<char*> (&random_access_bank[x][0]), 0x2000); 
			}

			sram.close();
			std::cout<<"MMU :  " << save_ram_file << " battery file saved.\n";
		}
	}
}