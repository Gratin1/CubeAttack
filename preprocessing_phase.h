#pragma once
#ifndef PreprocessingPhase_H_
#define PreprocessingPhase_H_


#include <omp.h>
#include <chrono>
#include <cassert>
#include <fstream>
#include <iostream>
#include <cmath>
#include <random>
#include <vector>

#include "generate_random.h"
#include "global.h"


template <typename T>
class PreprocessingPhase
{
	struct IndexData;

	SizeType KEY_size_, IV_size_;
	SizeType round_, round_numb, init_bits;	//cipher initialization rounds, number of initial output bits
	std::size_t numb_of_equations_, CubeSummation_end_;
	std::size_t cube_size_;
	std::vector<IndexData> linear_superpolys_;	//store all linear superpoly structures 

	struct IndexData
	{
		std::size_t bit_indx;	//output bit index
		std::vector<int> indexes;
		bool free_memb{};	//free member of superpoly
		Bitset polynomial_coefficients;	//coefficient of each monomial xi in superpoly
		std::vector<Bitset> quadratic_coefficients;
		IndexData(const PreprocessingPhase &obj, const std::vector<int> &ind, 
														std::size_t index = 0)
			: bit_indx(index), indexes(ind)
		{
			polynomial_coefficients.resize(obj.KEY_size_);
			for (auto &coeff : quadratic_coefficients)
				coeff.resize(obj.KEY_size_ * obj.KEY_size_);
		}
	};
	//summing over Cube
	bool CubeSummation(const Bitset &KEY, const std::vector<int> &indices); 
	void InitializeIV(const std::size_t &i, const std::vector<int> &indices, Bitset &IV);
	void InitializeKEY(const std::size_t &i, Bitset &KEY, int flag = 0);
	void FindCoefficients(IndexData &obj);
	void FindQuadraticCoefficients(IndexData &obj);
	//checking set of indexes(obj argument) is constant or not.
	bool ConstancyCheck(const IndexData &obj);
	bool LinearityTest(const std::vector<int> &indices);
	bool QuadraticityTest(const std::vector<int> &indices);
	bool QuadraticityTesting(const bool &free_memb, const Bitset &x,
							 const Bitset &y, const Bitset &z,
							 const std::vector<int> &ind);
	bool BlrTest(const bool &free_memb, const Bitset &x,
		const Bitset &y, const std::vector<int> &ind);
	std::vector<int> GetCurrentIndexes(std::vector<int> bitmask);
	//void gen_rand(Bitset &set);	//generating random
	void WriteToCsv(const IndexData &obj);
	void WriteQuadToCsv(const IndexData &obj);

public:
	PreprocessingPhase() = default;
	~PreprocessingPhase() = default;
	PreprocessingPhase(const SizeType &k_size, const SizeType &I_size)
		: KEY_size_(k_size),
		IV_size_(I_size),
		round_numb(),
		init_bits(),
		round_() {}
	PreprocessingPhase(const PreprocessingPhase&) = delete;
	PreprocessingPhase &operator=(const PreprocessingPhase&) = delete;
	// set round number and amount of testing output bits
	void HardPreprocessing(); //check every indexes
	void SetCubeSize(const SizeType &size);
	void SetRoundNumber(const SizeType &num, const SizeType &amt = 0); 
	void SetEquationNumber(const std::size_t &num);
	//friend bool operator<(const CubeAttack::IndexData &Lobj, const CubeAttack::IndexData &Robj);
};



template <typename T>
bool PreprocessingPhase<T>::CubeSummation(const Bitset &KEY, const std::vector<int> &indices)
{
	//auto cube_size = indices.size();	//Boolean cube dimension (test - 12)	
	bool superpoly_value = 0;

#pragma omp parallel for reduction(^:superpoly_value)
	for (int i = 0; i < CubeSummation_end_; ++i)
	{
		Bitset IV(IV_size_);
		//adjust values of Cube Indexes in IV
		InitializeIV(i, indices, IV);	
		T cipher(KEY, IV, round_);
		superpoly_value ^= cipher.gen();

		//Trivium temp(KEY, IV, round_);
		//function temp(KEY, IV);
		//superpoly_value ^= temp.gen();
	}

	return superpoly_value;
}


template <typename T>
void PreprocessingPhase<T>::InitializeIV(const std::size_t &i, const std::vector<int> &indices, Bitset &IV)
{
	Bitset temp(indices.size(), i);
	for (std::size_t j = 0; j < indices.size(); ++j)
	{
		IV[IV_size_ - 1 - indices[j]] = temp[j];
	}
}

template <typename T>
void PreprocessingPhase<T>::InitializeKEY(const std::size_t &i, Bitset &KEY, int flag)
{
	if (flag)
		KEY.reset();
	KEY[KEY_size_ - 1 - i] = 1;
}

template <typename T>
void PreprocessingPhase<T>::FindCoefficients(IndexData &obj)
{
	Bitset KEY(KEY_size_);
	obj.free_memb = CubeSummation(KEY, obj.indexes);

	for (SizeType i = 0; i < KEY_size_; ++i)
	{
		InitializeKEY(i, KEY, 1);
		obj.polynomial_coefficients[KEY_size_ - 1 - i] = CubeSummation(KEY, obj.indexes);
	}

	if (obj.free_memb)
		obj.polynomial_coefficients.flip();
}

template <typename T>
void PreprocessingPhase<T>::FindQuadraticCoefficients(IndexData &obj)
{
	Bitset KEY(KEY_size_), first_KEY(KEY_size_);
	obj.free_memb = CubeSummation(KEY, obj.indexes);
	for (SizeType i = 0; i < KEY_size_; ++i)
	{
		InitializeKEY(i, KEY, 1);
		InitializeKEY(i, first_KEY, 1);
		auto first = CubeSummation(KEY, obj.indexes);
		for (SizeType j = 0; j < KEY_size_; ++j)
		{
			InitializeKEY(j, KEY, 1);
			obj.quadratic_coefficients[i][KEY_size_ - 1 - j] =
				CubeSummation(KEY, obj.indexes) ^
				CubeSummation(first_KEY, obj.indexes) ^
				CubeSummation(KEY^first_KEY, obj.indexes);
		}
	}
	if (obj.free_memb)
		for (auto &coeff : obj.quadratic_coefficients)
			coeff.flip();
}

template <typename T>
inline bool PreprocessingPhase<T>::ConstancyCheck(const IndexData &obj)
{
	return obj.polynomial_coefficients.any();
}

template <typename T>
bool PreprocessingPhase<T>::LinearityTest(const std::vector<int> &indices)
{
	//Bitset x(KEY_size_), y(KEY_size_);
	bool free_memb = CubeSummation(Bitset(KEY_size_), indices);
	for (int i = 0; i < kLinearityTestsNumber; ++i)
	{
		//gen_rand(x);
		//gen_rand(y);
		GenerateRandom random;
		auto x = random.GenerateBitset(KEY_size_);
		auto y = random.GenerateBitset(IV_size_);
		if (!BlrTest(free_memb, x, y, indices))
			return false;
	}
	return true;
}

template <typename T>
bool PreprocessingPhase<T>::QuadraticityTest(const std::vector<int> &indices)
{
	bool free_memb = CubeSummation(Bitset(KEY_size_), indices);
	for (int i = 0; i < kQuadraticityTestsNumber; ++i)
	{
		GenerateRandom random;
		auto x = random.GenerateBitset(KEY_size_);
		auto y = random.GenerateBitset(IV_size_);
		auto z = random.GenerateBitset(IV_size_);
		if (!QuadraticityTesting(free_memb, x, y, z, indices))
			return false;
	}
	return true;
}

template <typename T>
inline bool PreprocessingPhase<T>::QuadraticityTesting(const bool &free_memb, const Bitset &x,
	const Bitset &y, const Bitset &z,
	const std::vector<int> &ind)
{
	//f(x+y) + f(x+z) + f(y+z) + f(x) +
	//+ f(y) + f(z) + f(0) == f(x+y+z)
	return ((CubeSummation(x^y, ind) ^ CubeSummation(x^z, ind) ^ CubeSummation(y^z, ind) ^
		CubeSummation(x, ind) ^ CubeSummation(y, ind) ^ CubeSummation(z, ind) ^ free_memb) ==
		CubeSummation(x^y^z, ind));
}

template <typename T>
inline bool PreprocessingPhase<T>::BlrTest(const bool &free_memb, const Bitset &x,
	const Bitset &y, const std::vector<int> &ind)
{
	//f(0) + f(x) + f(y) == f(x+y)
	return ((free_memb ^ CubeSummation(x, ind) ^ CubeSummation(y, ind)) == CubeSummation(x^y, ind));
}

template <typename T>
void PreprocessingPhase<T>::HardPreprocessing()
{
	std::vector<int> bitmask(cube_size_, 1);
	std::vector<int> current_testing_indexes;

	CubeSummation_end_ = pow(2, bitmask.size());
	bitmask.resize(KEY_size_, 0);
	//permute bitmask
	do
	{
		for (int i = 0; i < KEY_size_; ++i) // [0..N-1] integers
		{
			if (bitmask[i])
				current_testing_indexes.push_back(i);
		}

		for (const auto &el : current_testing_indexes)
			std::cout << el << " ";
		std::cout << std::endl;

		while (round_ <= round_numb + init_bits)
		{
			if (LinearityTest(current_testing_indexes))	//if polynomial is linear
			{
				IndexData ind(*this, current_testing_indexes, round_);
				FindCoefficients(ind);
				if (ConstancyCheck(ind))
				{
					WriteToCsv(ind);
					break;
				}
			}
			++round_; //testing all output bits in interval [round_numb, round_numb + init_bits]
		}
		current_testing_indexes.clear();
		round_ = round_numb;
	} while (std::prev_permutation(bitmask.begin(), bitmask.end()));
}

template <typename T>
std::vector<int> PreprocessingPhase<T>::GetCurrentIndexes(std::vector<int> bitmask)
{
	std::vector<int> current_testing_indexes;
	for (int i = 0; i < KEY_size_; ++i) // [0..N-1] integers
	{
		if (bitmask[i])
			current_testing_indexes.push_back(i);
	}
	return current_testing_indexes;
}

template <typename T>
void PreprocessingPhase<T>::WriteToCsv(const IndexData &obj)
{
	std::ofstream out("maxterms.csv", std::ofstream::app);
	assert(out.good());

	if (obj.free_memb)
		out << "1+";	//print free_member
	auto count = obj.polynomial_coefficients.count();
	for (auto i = obj.polynomial_coefficients.size() - 1; i > 0; --i)
	{
		if (obj.polynomial_coefficients[i])
			out << "x" << obj.polynomial_coefficients.size() - 1 - i << (--count ? "+" : "");	//print maxterm
	}
	out << ",";
	for (const auto &inx : obj.indexes)
		out << inx << " ";	//print indexes
	out << ",";
	out << obj.bit_indx;
}

template <typename T>
void PreprocessingPhase<T>::WriteQuadToCsv(const IndexData &obj)
{
	std::ofstream out("quad_superpolys.csv", std::ofstream::app);
	assert(out.good());
	if (obj.free_memb)
		out << "1+";

	for (auto i = obj.quadratic_coefficients.size() - 1; i > 0; --i)
	{
		auto count = obj.quadratic_coefficients[i].count();
		for (auto j = obj.quadratic_coefficients.size() - 1; j > 0; --j)
		{
			if (obj.quadratic_coefficients[i][j])
				out << "x" << obj.quadratic_coefficients[i].size() - 1 - i
				<< "x" << obj.quadratic_coefficients[i].size() - 1 - i
				<< (--count ? "+" : "");	//print quadratic_superpolys
		}
	}
	out << ",";
	for (const auto &inx : obj.indexes)
		out << inx << " ";	//print indexes
	out << ",";
	out << obj.bit_indx;
}

template <typename T>
void PreprocessingPhase<T>::SetRoundNumber(const SizeType &num, const SizeType &amt)
{
	round_numb = round_ = num;
	init_bits = amt;
}

template <typename T>
void PreprocessingPhase<T>::SetCubeSize(const SizeType &size)
{
	cube_size_ = size;
}

template <typename T>
void PreprocessingPhase<T>::SetEquationNumber(const std::size_t &num)
{
	numb_of_equations_ = num;
}


/*
bool operator<(const CubeAttack::IndexData &Lobj, const CubeAttack::IndexData &Robj)
{
return (Lobj.polynomial_coefficients.count() < Robj.polynomial_coefficients.count());
}
*/
#endif // !PreprocessingPhase_H_