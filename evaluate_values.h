#pragma once
#ifndef EVALUATE_VALUES_H
#define EVALUATE_VALUES_H

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "global.h"


template <typename T>
class EvaluateValue
{
	SizeType KEY_size, IV_size;
	Bitset KEY;
	int CubeSummation_end_;
	bool CubeSummation(const Bitset &KEY, const std::vector<int> &indices, int round);
	void InitializeIV(const std::size_t &i, const std::vector<int> &indices, Bitset &IV);

public: 
	EvaluateValue(int key_size, int iv_size): KEY_size(key_size), IV_size(iv_size){
		KEY.resize(KEY_size);
	};
	void GetIndexes();
};

template <typename T>
void EvaluateValue<T>::GetIndexes()
{

	std::vector<int> indexes;
	std::ifstream in("maxterms.csv");
	std::ofstream out("value.txt");
	assert(in.good());
	std::string line;
	int round_number;
	while (std::getline(in, line))
	{

		int index_number, indexes_size = 0;
		
		std::stringstream ss(line);
		while (ss >> index_number)
		{
			++indexes_size;
			indexes.push_back(index_number);
		}
		round_number = std::stoi(line.substr(line.size()-3, line.size()));

		CubeSummation_end_ = indexes_size;
		//std::for_each(indexes.cbegin(), indexes.cend(), [](const int &number) {std::cout << number << " "; });
	}
	out << CubeSummation(KEY, indexes, round_number);
}

template <typename T>
bool EvaluateValue<T>::CubeSummation(const Bitset &KEY, const std::vector<int> &indices, int round)
{
	bool superpoly_value = 0;

#pragma omp parallel for reduction(^:superpoly_value)
	for (int i = 0; i < CubeSummation_end_; ++i)
	{
		Bitset IV(IV_size);
		//adjust values of Cube Indexes in IV
		InitializeIV(i, indices, IV);
		T cipher(KEY, IV, round);
		superpoly_value ^= cipher.gen();
	}

	return superpoly_value;
}

template <typename T>
void EvaluateValue<T>::InitializeIV(const std::size_t &i, const std::vector<int> &indices, Bitset &IV)
{
	Bitset temp(indices.size(), i);
	for (std::size_t j = 0; j < indices.size(); ++j)
	{
		IV[IV_size - 1 - indices[j]] = temp[j];
	}
}

#endif // EVALUATE_VALUES_H