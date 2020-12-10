#ifndef USP_GENERATOR_H
#define USP_GENERATOR_H

#include "usp.h"

#include <random>

namespace usp {

/* Utility class to generate random USPs.
 */
class UspGenerator
{
public:
  UspGenerator();
  // Randomly generate a (n, k) USP
  Usp generateRandomPuzzle(unsigned int n, unsigned int k);

private:
  std::default_random_engine m_generator;
  std::uniform_int_distribution<int> m_distribution{ 1, 3 };
};

}// namespace usp

#endif