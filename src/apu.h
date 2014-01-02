// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : apu.h
// Date : November 18, 2013
// Description : Game Boy APU emulation
//
// Sets up SDL audio for mixing
// Generates and mixes samples for the GB's 4 sound channels 

#ifndef GB_APU
#define GB_APU

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include "mmu.h"

struct voice
{
	u32 freq_dist;
	int sample_length;
	double frequency;
	u32 duration;
	u32 volume;
	bool playing;

	u32 duty_cycle_start;
	u32 duty_cycle_end;

	u32 envelope_direction;
	u32 envelope_step;
	u32 envelope_counter;

	u32 sweep_direction;
	u32 sweep_step;
	u32 sweep_time;
	u32 sweep_counter;

	double wave_step;
	u8 wave_shift;
};

class APU
{
	public:
	
	MMU* mem_link;

	voice channel[4];
	bool setup;

	SDL_AudioSpec desired_spec;
    	SDL_AudioSpec obtained_spec;

	APU();
	~APU();

	void generate_channel_1_samples(s16* stream, int length);
	void play_channel_1();

	void generate_channel_2_samples(s16* stream, int length);
	void play_channel_2();

	void generate_channel_3_samples(s16* stream, int length);
	void play_channel_3();

	void step();
};

/****** SDL Audio Callback ******/ 
void audio_callback(void* _apu, u8 *_stream, int _length);

#endif // GB_APU