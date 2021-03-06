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
typedef signed int s32;

/* ROM Header */

//DMG or GBC Support
const u16 ROM_COLOR = 0x143;

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

//NR52
const u16 REG_NR52 = 0xFF26;

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

//Double Speed Control
const u16 REG_KEY1 = 0xFF4D;

//HDMA Source High
const u16 REG_HDMA1 = 0xFF51;

//HDMA Source Low
const u16 REG_HDMA2 = 0xFF52;

//HDMA Destination High
const u16 REG_HDMA3 = 0xFF53;

//HDMA Destination Low
const u16 REG_HDMA4 = 0xFF54;

//HDMA Control
const u16 REG_HDMA5 = 0xFF55;

//Video RAM Bank
const u16 REG_VBK = 0xFF4F;

//Background Color Palette Select
const u16 REG_BCPS = 0xFF68;

//Background Color Palette Data
const u16 REG_BCPD = 0xFF69;

//Object Color Palette Select
const u16 REG_OCPS = 0xFF6A;

//Object Color Palette Data
const u16 REG_OCPD = 0xFF6B;

//Working RAM Bank
const u16 REG_SVBK = 0xFF70;

#endif // GB_COMMON