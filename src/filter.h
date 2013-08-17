// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : filter.h
// Date : August 16, 2013
// Description : Image scaling filters
//
// Implements various image scaling techniques
// Current filters: Nearest Neighbor 2x

#ifndef GB_FILTER
#define GB_FILTER

#include "SDL/SDL.h"

#include "common.h"
#include "config.h"

void apply_scaling(SDL_Surface* input_image, SDL_Surface* output_image);
void scale_nearest_neighbor_2x(SDL_Surface* input_image, SDL_Surface* output_image);

#endif // GB_FILTER
		