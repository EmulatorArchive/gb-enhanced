// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : filter.h
// Date : August 16, 2013
// Description : Image scaling filters
//
// Implements various image scaling techniques
// Current filters: Nearest Neighbor 2x-4x

#include "filter.h"
#include "common.h"
#include "config.h"

/****** Selects the appropiate scaling method ******/
void apply_scaling(SDL_Surface* input_image, SDL_Surface* output_image)
{
	switch(config::scaling_mode)
	{

		//Nearest Neighbor 2x
		case 1: 
			scale_nearest_neighbor_2x(input_image, output_image);
			break;

		//Nearest Neighbor 3x
		case 2: 
			scale_nearest_neighbor_3x(input_image, output_image);
			break;

		//Nearest Neighbor 3x
		case 3: 
			scale_nearest_neighbor_4x(input_image, output_image);
			break;

		//What?
		default:
			std::cout<<"Just so you know, this shouldn't happen... \n";
			break;
	}
}

/****** Nearest Neighbor 2x implementation ******/
void scale_nearest_neighbor_2x(SDL_Surface* input_image, SDL_Surface* output_image)
{
	u32 input_width = input_image->w;
	u32 input_height = input_image->h;

	int output_width = input_width * 2;
	int output_height = input_height * 2;

	u32 input_pixel_plotter = 0;
	u32 output_pixel_plotter = 0;

	//Lock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_LockSurface(input_image); }

	//Lock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_LockSurface(output_image); }

	//Grab Pixel Data
	u32* input_pixel_data = (u32*)input_image->pixels; 
	u32* output_pixel_data = (u32*)output_image->pixels;

	//Cycles through input image - Interpolate pixels to output image
	for(; input_pixel_plotter < (input_width * input_height);)
	{
		output_pixel_data[output_pixel_plotter] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 1] = input_pixel_data[input_pixel_plotter];

		output_pixel_data[output_pixel_plotter + output_width] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 1] = input_pixel_data[input_pixel_plotter];

		input_pixel_plotter++;
		output_pixel_plotter += 2;

		if(output_pixel_plotter % output_width == 0) 
		{ 
			output_pixel_plotter += output_width; 
		}

	}

	//Unlock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_UnlockSurface(input_image); }

	//Unlock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_UnlockSurface(output_image); }
}

/****** Nearest Neighbor 3x implementation ******/
void scale_nearest_neighbor_3x(SDL_Surface* input_image, SDL_Surface* output_image)
{
	u32 input_width = input_image->w;
	u32 input_height = input_image->h;

	int output_width = input_width * 3;
	int output_height = input_height * 3;

	u32 input_pixel_plotter = 0;
	u32 output_pixel_plotter = 0;

	//Lock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_LockSurface(input_image); }

	//Lock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_LockSurface(output_image); }

	//Grab Pixel Data
	u32* input_pixel_data = (u32*)input_image->pixels; 
	u32* output_pixel_data = (u32*)output_image->pixels;

	//Cycles through input image - Interpolate pixels to output image
	for(; input_pixel_plotter < (input_width * input_height);)
	{
		output_pixel_data[output_pixel_plotter] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 2] = input_pixel_data[input_pixel_plotter];
		
		output_pixel_data[output_pixel_plotter + output_width] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 2] = input_pixel_data[input_pixel_plotter];

		output_pixel_data[output_pixel_plotter + (output_width * 2)] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 2) + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 2) + 2] = input_pixel_data[input_pixel_plotter];

		input_pixel_plotter++;
		output_pixel_plotter += 3;

		if(output_pixel_plotter % output_width == 0) 
		{ 
			output_pixel_plotter += (output_width * 2); 
		}

	}

	//Unlock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_UnlockSurface(input_image); }

	//Unlock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_UnlockSurface(output_image); }
}

/****** Nearest Neighbor 4x implementation ******/
void scale_nearest_neighbor_4x(SDL_Surface* input_image, SDL_Surface* output_image)
{
	u32 input_width = input_image->w;
	u32 input_height = input_image->h;

	int output_width = input_width * 4;
	int output_height = input_height * 4;

	u32 input_pixel_plotter = 0;
	u32 output_pixel_plotter = 0;

	//Lock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_LockSurface(input_image); }

	//Lock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_LockSurface(output_image); }

	//Grab Pixel Data
	u32* input_pixel_data = (u32*)input_image->pixels; 
	u32* output_pixel_data = (u32*)output_image->pixels;

	//Cycles through input image - Interpolate pixels to output image
	for(; input_pixel_plotter < (input_width * input_height);)
	{
		output_pixel_data[output_pixel_plotter] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 2] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + 3] = input_pixel_data[input_pixel_plotter];
		
		output_pixel_data[output_pixel_plotter + output_width] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 2] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + output_width + 3] = input_pixel_data[input_pixel_plotter];

		output_pixel_data[output_pixel_plotter + (output_width * 2)] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 2) + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 2) + 2] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 2) + 3] = input_pixel_data[input_pixel_plotter];

		output_pixel_data[output_pixel_plotter + (output_width * 3)] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 3) + 1] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 3) + 2] = input_pixel_data[input_pixel_plotter];
		output_pixel_data[output_pixel_plotter + (output_width * 3) + 3] = input_pixel_data[input_pixel_plotter];

		input_pixel_plotter++;
		output_pixel_plotter += 4;

		if(output_pixel_plotter % output_width == 0) 
		{ 
			output_pixel_plotter += (output_width * 3); 
		}

	}

	//Unlock Input Surface
	if(SDL_MUSTLOCK(input_image)){ SDL_UnlockSurface(input_image); }

	//Unlock Output Surface
	if(SDL_MUSTLOCK(output_image)){ SDL_UnlockSurface(output_image); }
}
