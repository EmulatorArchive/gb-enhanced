// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : hash.h
// Date : Febuary 17, 2014
// Description : Generate alphanumeric hashes
//
// Produces alphanumeric hashes based on binary input
// Primarily used for custom graphics

#ifndef GB_HASH
#define GB_HASH

#include <iostream>
#include <string>

#include "common.h"

std::string raw_to_64(u16 input_word);

namespace hash
{
	extern std::string base_64_index;
}

#endif // GB_HASH
	
