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
    for (unsigned int i = 0; i < n; ++i) {
      for (unsigned int j = 0; j < k; ++j) {
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

unsigned int Usp::rows() const
{
  return m_rows;
}

unsigned int Usp::cols() const
{
  return m_cols;
}

Permutation::Permutation(unsigned int n) : m_data(n, n), m_size(n)
{}

bool Permutation::checkIdentity() const
{
  for (unsigned int i = 0; i < m_size; ++i) {
    const Node &node = m_data(i, i);
    if (!node.m_value || !node.m_assigned) {
      return false;
    }
  }
  return true;
}

bool Permutation::checkContradiction() const
{
  for (unsigned int i = 0; i < m_size; ++i) {
    bool rowflag = false;
    bool colflag = false;
    for (unsigned int j = 0; j < m_size; ++j) {
      const Node &rowNode = m_data(i, j);
      const Node &colNode = m_data(j, i);

      if (!rowNode.m_assigned || (rowNode.m_assigned && rowNode.m_value)) {
        rowflag = true;
      }
      if (!colNode.m_assigned || (colNode.m_assigned && colNode.m_value)) {
        colflag = true;
      }

      if (rowflag && colflag) {
        break;
      }
    }
    if (rowflag || colflag) {
      return false;
    }
  }
  return true;
}

std::optional<unsigned int> Permutation::nextAssignment() const
{
  for (unsigned int i = 0; i < m_size; ++i) {
    bool flag = false;
    for (unsigned int j = 0; j < m_size; ++j) {
      const Node &node = m_data(i, j);
      if (node.m_assigned && node.m_value) {
        flag = true;
        break;
      }
    }
    if (!flag) {
      return std::make_optional<unsigned int>(i);
    }
  }
  return std::nullopt;
}

void Permutation::assign(unsigned int y, unsigned int x, bool value, int decision_level)
{
  // Disable all others in row if setting something to true
  if (value) {
    for (unsigned int col = 0; col < m_size; ++col) {
      if (col != x) {
        Node &node = m_data(y, col);
        if (!node.m_assigned) {
          node.m_assigned = true;
          node.m_value = false;
          node.m_decision_level = decision_level;
        }
      }
    }
  }

  Node &node = m_data(y, x);
  if (!node.m_assigned) {
    node.m_assigned = true;
    node.m_value = value;
    node.m_decision_level = decision_level;
  }
}

std::optional<unsigned int> Permutation::assignment(unsigned int row) const
{
  for (unsigned int i = 0; i < m_size; ++i) {
    if (const Node &node = m_data(row, i); node.m_value && node.m_assigned) {
      return std::make_optional<unsigned int>(i);
    }
  }
  return std::nullopt;
}

std::vector<unsigned int> Permutation::possibleAssignments(unsigned int row) const
{
  std::vector<unsigned int> assignments;
  for (unsigned int i = 0; i < m_size; ++i) {
    if (!m_data(row, i).m_assigned) {
      assignments.push_back(i);
    }
  }
  return assignments;
}

void Permutation::assignPropagate(unsigned int y, unsigned int x, int decision_level)
{
  for (unsigned int i = 0; i < m_size; ++i) {
    if (i != x) {
      Node &colNode = m_data(y, i);
      if (!colNode.m_assigned) {
        colNode.m_assigned = true;
        colNode.m_value = false;
        colNode.m_decision_level = decision_level;
      }
    }
    if (i != y) {
      Node &rowNode = m_data(i, x);
      if (!rowNode.m_assigned) {
        rowNode.m_assigned = true;
        rowNode.m_value = false;
        rowNode.m_decision_level = decision_level;
      }
    }
  }

  Node &assignedNode = m_data(y, x);
  assignedNode.m_assigned = true;
  assignedNode.m_value = true;
  assignedNode.m_decision_level = decision_level;
}

void Permutation::undoPropagation(int decision_level)
{
  for (unsigned int i = 0; i < m_size; ++i) {
    for (unsigned int j = 0; j < m_size; ++j) {
      Node &node = m_data(i, j);
      if (node.m_decision_level >= decision_level) {
        node.m_assigned = false;
      }
    }
  }
}

void Permutation::logData() const
{
  std::stringstream ss;
  for (unsigned int i = 0; i < m_size; ++i) {
    for (unsigned int j = 0; j < m_size; ++j) {
      const Node &node = m_data(i, j);
      if (node.m_assigned) {
        ss << static_cast<int>(node.m_value) << " ";
      } else {
        ss << "x ";
      }
    }
    ss << "\n";
  }
  spdlog::debug(ss.str());
}


}// namespace usp