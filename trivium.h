#pragma once
#ifndef TRIVIUM_H_
#define TRIVIUM_H_

#include<omp.h>
#include<iostream>

#include "boost/dynamic_bitset.hpp"



class Trivium
{
	typedef boost::dynamic_bitset<> bitset;
	typedef boost::dynamic_bitset<>::size_type size_type;
	//3 LFSR states
	boost::dynamic_bitset<> S1;
	boost::dynamic_bitset<> S2;
	boost::dynamic_bitset<> S3;
	//output bit index
	std::size_t bit_index;
	//initialising internal states
	void init(const boost::dynamic_bitset<> &key, const boost::dynamic_bitset<> &IV);

public:
	Trivium() = default;
	Trivium(const boost::dynamic_bitset<> &key, const boost::dynamic_bitset<> &IV, std::size_t bit_indx);
	//keystream generation
	bool gen(size_type numb=1);
};

#endif // !TRIVIUM_H_





