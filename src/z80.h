// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : z80.cpp
// Date : July 27, 2013
// Description : Game Boy Z80 CPU emulator
//
// Emulates the GB CPU in software

#ifndef GB_CPU
#define GB_CPU

#include <string>
#include <iostream>

#include "common.h"
#include "mmu.h"

class CPU
{
	public:
	
	//Internal Registers - 1 Byte
	struct registers
	{
		//AF
		union
		{
			struct 
			{
				u8 f;
				u8 a;
			};
			u16 af;
		};

		//BC
		union
		{
			struct 
			{
				u8 c;
				u8 b;
			};
			u16 bc;
		};

		//DE
		union
		{
			struct 
			{
				u8 e;
				u8 d;
			};
			u16 de;
		};

		//HL
		union
		{
			struct 
			{
				u8 l;
				u8 h;
			};
			u16 hl;
		};

		u16 pc, sp;
	} reg;

	u8 temp_byte;
	u16 temp_word;

	//Internal CPU clock
	int cpu_clock_m, cpu_clock_t;
	int cycles;

	//DIV and TIMA timer counters
	int div_counter, tima_counter;
	int tima_speed;

	//Memory management unit
	MMU mem;

	//CPU Running flag
	bool running;
	
	//Interrupt flag
	bool interrupt;

	bool halt;
	bool pause;

	//Core Functions
	CPU();
	~CPU();
	void reset();
	void reset_bios();
	void exec_op(u8 opcode);
	void exec_op(u16 opcode);

	//Interrupt handling
	bool handle_interrupts();

	inline void jr(u8 reg_one);

	//Math functions
	inline u8 add_byte(u8 reg_one, u8 reg_two);
	inline u16 add_word(u16 reg_one, u16 reg_two);
	inline u8 add_carry(u8 reg_one, u8 reg_two);
	inline u16 add_signed_byte(u16 reg_one, u8 reg_two);

	inline u8 sub_byte(u8 reg_one, u8 reg_two);
	inline u8 sub_carry(u8 reg_one, u8 reg_two);

	inline u8 inc_byte(u8 reg_one);
	inline u8 dec_byte(u8 reg_one);

	//Binary functions
	inline u8 and_byte(u8 reg_one, u8 reg_two);
	inline u8 or_byte(u8 reg_one, u8 reg_two);
	inline u8 xor_byte(u8 reg_one, u8 reg_two);

	inline u8 rotate_left(u8 reg_one);
	inline u8 rotate_left_carry(u8 reg_one);
	inline u8 rotate_right(u8 reg_one);
	inline u8 rotate_right_carry(u8 reg_one);

	inline u8 sla(u8 reg_one);
	inline u8 sra(u8 reg_one);
	inline u8 srl(u8 reg_one);

	inline u8 swap(u8 reg_one);
	inline void bit(u8 reg_one, u8 check_bit);
	inline u8 res(u8 reg_one, u8 reset_bit);
	inline u8 set(u8 reg_one, u8 set_bit);
	inline u8 daa();
};

#endif // GB_CPU
