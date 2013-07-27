// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : common.h
// Date : July 27, 2013
// Description : Common functions and definitions
//
// A bunch of typedefs and consts
// Typedefs make handling GB hardware easier than C++ types
// Consts are quick references to commonly used memory locations 

/****** Common Function & Definitions ******/  

#ifndef GB_COMMON
#define GB_COMMON

/* Byte Definitions */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;

/* ROM Header */

//Memory Bank Controller Type
const u16 ROM_MBC = 0x147;

//ROM Size
const u16 ROM_ROMSIZE = 0x148;

//RAM Size
const u16 ROM_RAMSIZE = 0x149;

/* Special Memory Addresses */

//Object Attribute Memory
const u16 OAM = 0xFE00;

/* Special Memory Registers */

//Joypad
const u16 REG_P1 = 0xFF00;

//DIV Timer
const u16 REG_DIV = 0xFF04;

//TIMA Timer
const u16 REG_TIMA = 0xFF05;

//Timer Modulo
const u16 REG_TMA = 0xFF06;

//TAC
const u16 REG_TAC = 0xFF07;

//Interrupt Flag
const u16 REG_IF = 0xFF0F;

//Enabled Interrupts
const u16 REG_IE = 0xFFFF;

//LCD Control
const u16 REG_LCDC = 0xFF40;

//LCD Status
const u16 REG_STAT = 0xFF41;

//Scroll-Y
const u16 REG_SY = 0xFF42;

//Scroll-X
const u16 REG_SX = 0xFF43;

//Scanline
const u16 REG_LY = 0xFF44;

//LY Coincidence
const u16 REG_LYC = 0xFF45;

//DMA
const u16 REG_DMA = 0xFF46;

//BG Palette
const u16 REG_BGP = 0xFF47;

//Sprite Palette 0
const u16 REG_OBP0 = 0xFF48;

//Sprite Palette 1
const u16 REG_OBP1 = 0xFF49;

//Window-Y
const u16 REG_WY = 0xFF4A;

//Window-X
const u16 REG_WX = 0xFF4B;

#endif // GB_COMMON