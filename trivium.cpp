#include "trivium.h"


Trivium::Trivium(const boost::dynamic_bitset<> &key, const boost::dynamic_bitset<> &IV, std::size_t bit_indx)
	: bit_index(bit_indx)
{
	S1.resize(93);
	S2.resize(84);
	S3.resize(111);

	init(key, IV);
}

void Trivium::init(const boost::dynamic_bitset<> &key, const boost::dynamic_bitset<> &IV)
{
	bool t1, t2, t3;
	for (size_type i = 0; i < 80; ++i)
	{
		S1[92 - i] = key[79 - i];
		S2[83 - i] = IV[79 - i];
	}

	S3[0] = S3[1] = S3[2] = 1;
	for (size_type i = 0; i < bit_index; ++i)
	{
		t1 = S1[92 - 65] ^ S1[92 - 92];
		t2 = S2[83 - 68] ^ S2[83 - 83];
		t3 = S3[110 - 65] ^ S3[110 - 110];

		t1 ^= S1[92 - 90] & S1[92 - 91] ^ S2[83 - 77];
		t2 ^= S2[83 - 81] & S2[83 - 82] ^ S3[110 - 86];
		t3 ^= S3[110 - 108] & S3[110 - 109] ^ S1[92 - 68];

		S1 >>= 1;
		S2 >>= 1;
		S3 >>= 1;
		S1[92] = t3;
		S2[83] = t1;
		S3[110] = t2;
	}
}

bool Trivium::gen(size_type numb)
{
	bool t1, t2, t3, z;
	t1 = S1[92 - 65] ^ S1[92 - 92];
	t2 = S2[83 - 68] ^ S2[83 - 83];
	t3 = S3[110 - 65] ^ S3[110 - 110];
	//first generated bit of gamma
	z = t1 ^ t2 ^ t3;
	t1 ^= S1[92 - 90] & S1[92 - 91] ^ S2[83 - 77];
	t2 ^= S2[83 - 81] & S2[83 - 82] ^ S3[110 - 86];
	t3 ^= S3[110 - 108] & S3[110 - 109] ^ S1[92 - 68];

	S1 >>= 1;
	S2 >>= 1;
	S3 >>= 1;
	S1[92] = t3;
	S2[83] = t1;
	S3[110] = t2;

	return z;
}