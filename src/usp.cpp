#include "usp.h"

#include <iostream>
#include <string>
#include <sstream>

#include <spdlog/spdlog.h>


namespace usp {

Usp::Usp(std::vector<int> data, unsigned int n, unsigned int k) : m_data(n, k, std::move(data)), m_rows(n), m_cols(k)
{
  m_func.reserve(n * n * n);

  auto dataString = [this, n, k]() -> std::string {
    std::stringstream ss;
    ss << "Data: \n";
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < k; ++j) {
        ss << m_data(i, j) << " ";
      }
      ss << "\n";
    }
    return ss.str();
  };


  spdlog::debug(dataString());
  spdlog::debug("Computing Function:");

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
        spdlog::debug("({},{},{}): {}", a, b, c, m_func.back());
      }
    }
  }
}

bool Usp::query(unsigned int a, unsigned int b, unsigned int c) const
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

Permutation::Permutation(int n) : m_data(n, n), m_size(n)
{}

void Permutation::assign(unsigned int y, unsigned int x, bool value)
{
  Node &node = m_data(y, x);
  node.m_assigned = true;
  node.m_value = value;
  // not messing with decision level yet
}

int Permutation::assignment(int row)
{
  for (unsigned int i = 0; i < m_size; ++i) {
    if (const Node &node = m_data(row, i); node.m_value && node.m_assigned) {
      return i;
    }
  }
  // Failure, should not reach here
  spdlog::error("Extracting assignment from permutation failed");
  return -1;
}

}// namespace usp