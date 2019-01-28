#pragma once
#ifndef GENERATE_RANDOM_H_

#include <chrono>
#include <random>

#include "global.h"


class GenerateRandom
{
	
public:
	GenerateRandom() = default;
	Bitset GenerateBitset(SizeType size);
};



#endif // !GENERATE_RANDOM_H_