// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : mbc3.cpp
// Date : November 3, 2013
// Description : Game Boy Memory Bank Controller 3 I/O handling
//
// Handles reading and writing bytes to memory locations for MBC3
// Used to switch ROM and RAM banks in MBC3
// Also used for RTC functionality if present

#include "mmu.h"

//Performs write operations specific to the MBC1
void MMU::mbc3_write(u16 address, u8 value)
{
	//Write to External RAM
	if((address >= 0xA000) && (address <= 0xBFFF) && (cart_ram))
	{
		if(ram_banking_enabled) { random_access_bank[bank_bits][address - 0xA000] = value; }
	}

	//MBC register - Enable or Disable RAM Banking
	if(address <= 0x1FFF)
	{
		if((value & 0xF) == 0xA) { ram_banking_enabled = true; }
		else { ram_banking_enabled = false; }
	}

	//MBC register - Select ROM bank - Bits 0 to 6
	if((address >= 0x2000) && (address <= 0x3FFF)) 
	{ 
		rom_bank = (value & 0x7F);
	}

	//MBC register - Select RAM bank or RTC register
	if((address >= 0x4000) && (address <= 0x5FFF)) 
	{ 
		if(value <= 3) { bank_bits = value; }
	}
}

u8 MMU::mbc3_read(u16 address)
{
	//Read using ROM Banking
	if((address >= 0x4000) && (address <= 0x7FFF))
	{
		if(rom_bank >= 2) 
		{ 
			return read_only_bank[rom_bank - 2][address - 0x4000];
		}

		//When reading from Banks 0-1, just use the memory map
		else { return memory_map[address]; }
	}

	//Read using RAM Banking
	else if((address >= 0xA000) && (address <= 0xBFFF))
	{
		if(ram_banking_enabled) { return random_access_bank[bank_bits][address - 0xA000]; }
		else { return 0x00; }
	}
}