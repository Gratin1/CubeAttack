#include "generate_random.h"


Bitset GenerateRandom::GenerateBitset(SizeType size)
{
	std::random_device seed;
	std::mt19937 gen(seed());
	std::uniform_int_distribution<uint16_t> dis(0, UINT16_MAX);
	std::vector<uint16_t> random_buffer;
	Bitset generated_set(size);
	for (SizeType i = 0; i <= size / 16; ++i)
		random_buffer.push_back(dis(gen));

	SizeType i = 0;
	for (const auto &generated_number : random_buffer)
	{
		int j = 0;
		while (i < generated_set.size() && j < 16)
		{
			generated_set[i++] = (generated_number >> j++) & 1;
		}
	}
	return generated_set;
}