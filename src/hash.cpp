// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : hash.cpp
// Date : Febuary 17, 2014
// Description : Generate alphanumeric hashes
//
// Produces alphanumeric hashes based on binary input
// Primarily used for custom graphics

#include "hash.h"

namespace hash
{
	std::string base_64_index = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-";
}

/***** Converts 16-bit data to Base 64 text ******/
std::string raw_to_64(u16 input_word)
{
	u32 temp = 0;
	std::string output = "";
	
	temp = (input_word >> 12);
	input_word -= (temp << 12);
	output += hash::base_64_index[temp];	

	temp = (input_word >> 6);
	input_word -= (temp << 6);
	output += hash::base_64_index[temp];

	output += hash::base_64_index[input_word];

	return output;
}


	 
	


