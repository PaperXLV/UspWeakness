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

SatVariable::SatVariable(std::pair<unsigned int, unsigned int> position, bool positive, bool rho) : m_position(position), m_positive(positive), m_rho(rho)
{}

long unsigned int SatClause::size() const
{
  return m_variables.size();
}

void SatClause::addVariable(SatVariable var)
{
  m_variables.insert(var);
}

SatClause::State SatClause::state() const
{
  return m_state;
}

SatClause::State SatClause::evaluate(const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, int depth)
{
  SatVariable lastUnassigned;
  unsigned int assignmentCounter = 0;
  for (auto &variable : m_variables) {
    int nodeValue = (variable.m_rho) ? rho->value(variable.m_position) : sigma->value(variable.m_position);
    // assignment satisfies clause
    if ((nodeValue == 1 && variable.m_positive) || (nodeValue == 0 && !variable.m_positive)) {
      m_state = State::SATISFIED;
      return State::SATISFIED;
    }
    // assignment doesn't satisfy clause
    if ((nodeValue == 1 && !variable.m_positive) || (nodeValue == 0 && variable.m_positive)) {
      ++assignmentCounter;
    }
    // unassigned
    if (nodeValue == 2) {
      lastUnassigned = variable;
    }
  }
  // Every variable assigned and clause is not satisfied, contradiction
  if (assignmentCounter == m_variables.size()) {
    m_state = State::CONFLICTING;
    return State::CONFLICTING;
  }
  // Unit clause
  else if (assignmentCounter == m_variables.size() - 1) {
    if (lastUnassigned.m_positive) {
      (lastUnassigned.m_rho) ? rho->assignPropagate(lastUnassigned.m_position.first, lastUnassigned.m_position.second, true, depth) : sigma->assignPropagate(lastUnassigned.m_position.first, lastUnassigned.m_position.second, false, depth);
    } else {
      (lastUnassigned.m_rho) ? rho->assign(lastUnassigned.m_position.first, lastUnassigned.m_position.second, false, depth) : sigma->assign(lastUnassigned.m_position.first, lastUnassigned.m_position.second, false, depth);
    }
    // Set to satisfied, but return unit to tell algorithm to loop propagation again.
    m_state = State::SATISFIED;
    return State::UNIT;
  }
  m_state = State::UNRESOLVED;
  return State::UNRESOLVED;
}


bool operator==(const SatVariable &lhs, const SatVariable &rhs)
{
  return lhs.m_position == rhs.m_position && lhs.m_rho == rhs.m_rho && lhs.m_positive == rhs.m_positive;
}

bool operator<(const SatVariable &lhs, const SatVariable &rhs)
{
  return lhs.m_position < rhs.m_position;
}

bool operator==(const SatClause &lhs, const SatClause &rhs)
{
  return lhs.m_variables == rhs.m_variables;
}

bool operator<(const SatClause &lhs, const SatClause &rhs)
{
  return lhs.m_variables < rhs.m_variables;
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
  // if any row can't be assigned, we have a contradiction
  // Note: can also iterate through columns
  for (unsigned int i = 0; i < m_size; ++i) {
    bool flag = false;
    for (unsigned int j = 0; j < m_size; ++j) {
      const Node &node = m_data(i, j);
      if (!node.m_assigned || (node.m_assigned && node.m_value)) {
        flag = true;
        break;
      }
    }
    if (!flag) {
      return true;
    }
  }
  return false;
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

void Permutation::assign(unsigned int y, unsigned int x, bool value, int decision_level, std::vector<SatVariable> antecedents)
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
    node.m_antecedents = antecedents;
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

std::vector<SatVariable> Permutation::contradictionAntecedents(int decision_level) const
{
  std::vector<SatVariable> antecedents;
  for (unsigned int i = 0; i < m_size; ++i) {
    bool flag = false;
    for (unsigned int j = 0; j < m_size; ++j) {
      const Node &node = m_data(i, j);
      if (!node.m_assigned || (node.m_assigned && node.m_value)) {
        flag = true;
        break;
      }
    }
    if (!flag) {
      // Contradictary row, add all antecedents at decision_level
      for (unsigned int j = 0; j < m_size; ++j) {
        const Node &node = m_data(i, j);
        if (node.m_decision_level == decision_level) {
          std::vector<SatVariable> tempAntecedents = this->antecedents({ i, j });
          antecedents.insert(std::end(antecedents), std::begin(tempAntecedents), std::end(tempAntecedents));
        }
      }
    }
  }
  return antecedents;
}

void Permutation::assignPropagate(unsigned int y, unsigned int x, bool rho, int decision_level)
{
  for (unsigned int i = 0; i < m_size; ++i) {
    if (i != x) {
      Node &colNode = m_data(y, i);
      if (!colNode.m_assigned) {
        colNode.m_assigned = true;
        colNode.m_value = false;
        colNode.m_decision_level = decision_level;
        colNode.m_antecedents.push_back(SatVariable({ y, x }, false, rho));
      }
    }
    if (i != y) {
      Node &rowNode = m_data(i, x);
      if (!rowNode.m_assigned) {
        rowNode.m_assigned = true;
        rowNode.m_value = false;
        rowNode.m_decision_level = decision_level;
        rowNode.m_antecedents.push_back(SatVariable({ y, x }, false, rho));
      }
    }
  }

  Node &assignedNode = m_data(y, x);
  assignedNode.m_assigned = true;
  assignedNode.m_value = true;
  assignedNode.m_decision_level = decision_level;
}

std::vector<SatVariable> Permutation::antecedents(std::pair<unsigned int, unsigned int> assignment) const
{
  return m_data(assignment.first, assignment.second).m_antecedents;
}

int Permutation::nodeDecisionLevel(std::pair<unsigned int, unsigned int> assignment) const
{
  return m_data(assignment.first, assignment.second).m_decision_level;
}

void Permutation::undoPropagation(int decision_level)
{
  for (unsigned int i = 0; i < m_size; ++i) {
    for (unsigned int j = 0; j < m_size; ++j) {
      Node &node = m_data(i, j);
      if (node.m_decision_level >= decision_level) {
        node.m_assigned = false;
        // Clear antecedents
        node.m_antecedents.clear();
      }
    }
  }
}

int Permutation::value(std::pair<unsigned int, unsigned int> assignment) const
{
  const Node &node = m_data(assignment.first, assignment.second);
  return (node.m_assigned) ? static_cast<int>(node.m_value) : 2;
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