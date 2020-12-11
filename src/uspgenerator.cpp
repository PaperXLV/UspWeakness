#include "uspgenerator.h"

#include <chrono>

namespace usp {

UspGenerator::UspGenerator()
{
  m_generator.seed(static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()));
}

Usp UspGenerator::generateRandomPuzzle(unsigned int n, unsigned int k)
{
  std::vector<int> data(n * k);
  for (unsigned int i = 0; i < n * k; ++i) {
    data[i] = m_distribution(m_generator);
  }
  return Usp(std::move(data), n, k);
}

}//namespace usp