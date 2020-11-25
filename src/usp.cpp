#include "usp.h"

#include <iostream>

namespace usp {

Usp::Usp(std::vector<int> data, unsigned int n, unsigned int k) : m_data(n, k, std::move(data)), m_rows(n), m_cols(k)
{
  m_func.reserve(n * n * n);

  std::cout << "Data: \n";
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < k; ++j) {
      std::cout << m_data(i, j) << " ";
    }
    std::cout << "\n";
  }

  std::cout << "Computing Function:\n";

  // Compute function
  for (unsigned int a = 0; a < n; ++a) {
    for (unsigned int b = 0; b < n; ++b) {
      for (unsigned int c = 0; c < n; ++c) {
        m_func.push_back(false);
        for (unsigned int element = 0; element < k; ++element) {
          if ((m_data(a, element) == 1) + (m_data(b, element) == 2) + (m_data(c, element) == 3) == 2) {
            m_func.back() = true;
          }
        }
        std::cout << "(" << a << ", " << b << ", " << c << "): " << m_func.back() << "\n";
      }
    }
  }
}

int Usp::query(unsigned int a, unsigned int b, unsigned int c) const
{
  return m_func.at((a * m_rows * m_rows) + (b * m_rows) + c);
}

int Usp::rows() const
{
  return m_rows;
}

int Usp::cols() const
{
  return m_cols;
}

Permutation::Permutation(int n) : m_data(n, n)
{}


}// namespace usp