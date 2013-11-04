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

#include <ctime>

#include "mmu.h"

/****** Grab current system time for Real-Time Clock ******/
void MMU::grab_time()
{
	//Grab local time
	time_t system_time = time(0);
	tm* current_time = localtime(&system_time);

	//Seconds - Disregard tm_sec's 60 or 61 seconds
	rtc_reg[0] = current_time->tm_sec;
	if(rtc_reg[0] > 59) { rtc_reg[0] = 59; }
		
	//Minutes
	rtc_reg[1] = current_time->tm_min;

	//Hours
	rtc_reg[2] = current_time->tm_hour;
				
	//Days
	u16 temp_day = current_time->tm_yday;
	rtc_reg[3] = temp_day & 0xFF;
	temp_day >>= 8;
	if(temp_day == 1) { rtc_reg[4] |= 0x1; }
	else { rtc_reg[4] &= ~0x1; }
}

/****** Performs write operations specific to the MBC3 ******/
void MMU::mbc3_write(u16 address, u8 value)
{
	//Write to External RAM or RTC register
	if((address >= 0xA000) && (address <= 0xBFFF))
	{
		if((ram_banking_enabled) && (bank_bits <= 3)) { random_access_bank[bank_bits][address - 0xA000] = value; }
		else if((rtc_enabled) && (bank_bits >= 0x8) && (bank_bits <= 0xC)) { rtc_reg[bank_bits - 8] = value; }
	}

	//MBC register - Enable or Disable RAM Banking - Enable or Disable RTC if present
	if(address <= 0x1FFF)
	{
		if((value & 0xF) == 0xA) 
		{ 
			if(cart_ram) { ram_banking_enabled = true; } 
			if(cart_rtc) { rtc_enabled = true; }
		}
		else { ram_banking_enabled = false; rtc_enabled = false; }
	}

	//MBC register - Select ROM bank - Bits 0 to 6
	if((address >= 0x2000) && (address <= 0x3FFF)) 
	{ 
		rom_bank = (value & 0x7F);
	}

	//MBC register - Select RAM bank or RTC register
	if((address >= 0x4000) && (address <= 0x5FFF)) 
	{ 
		bank_bits = value;
	}

	//Latch current time to RTC regs
	if((address >= 0x6000) && (address <= 0x7FFF))
	{
		if(rtc_enabled)
		{
			//1st latch check
			if((rtc_latch_1 == 0xFF) && (value == 0)) { rtc_latch_1 = 0; }

			//After latch checks pass, grab system time to put into RTC regs
			else if((rtc_latch_2 == 0xFF) && (value == 1)) 
			{
				grab_time();

				//Reset latches
				rtc_latch_1 = rtc_latch_2 = 0xFF;
			}
		}
	}	
}

/****** Performs write operations specific to the MBC3 ******/
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

	//Read using RAM Banking or RTC regs
	else if((address >= 0xA000) && (address <= 0xBFFF))
	{
		if((ram_banking_enabled) && (bank_bits <= 3)) { return random_access_bank[bank_bits][address - 0xA000]; }
		else if((rtc_enabled) && (bank_bits >= 0x8) && (bank_bits <= 0xC)) { return rtc_reg[bank_bits - 8]; }
		else { return 0x00; }
	}
}