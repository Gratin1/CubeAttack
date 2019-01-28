#pragma once
#ifndef GLOBAL_H_

#include "boost/dynamic_bitset.hpp"

typedef boost::dynamic_bitset<>::size_type SizeType;
typedef boost::dynamic_bitset<> Bitset;

const int kLinearityTestsNumber = 100;
const int kQuadraticityTestsNumber = 25;
#endif // !GLOBAL_H_
