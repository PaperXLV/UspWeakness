#ifndef USP_H
#define USP_H

#include <memory>
#include <optional>
#include <set>
#include <vector>

namespace usp {

/* Dense (n, k) matrix of objects of type T.
 * Used both to represent USPs and to hold 
 * CDCL variables in each Permutation
 */
template<typename T>
class Matrix
{
public:
  Matrix<T>(unsigned int n, unsigned int k) : m_rows(n), m_cols(k)
  {
    m_data = std::vector<T>(n * k, T());
  }

  Matrix<T>(unsigned int n, unsigned int k, std::vector<T> data) : m_data(std::move(data)), m_rows(n), m_cols(k)
  {}

  T &operator()(unsigned int y, unsigned int x)
  {
    return m_data.at(y * m_cols + x);
  }

  const T &operator()(unsigned int y, unsigned int x) const
  {
    return m_data.at(y * m_cols + x);
  }

private:
  std::vector<T> m_data;
  unsigned int m_rows{ 0 };
  unsigned int m_cols{ 0 };
};

// Forward declaration
class Permutation;

/* Represents a CNF-SAT variable to be used in learned 
 * clauses in the CDCL solver.
 * Each instance corresponds to the Node at position in a   
 * Permutation. Negated if positive is set to false, and 
 * belongs to Permutation rho if rho is set to true. 
 */
class SatVariable
{
public:
  SatVariable() = default;
  SatVariable(std::pair<unsigned int, unsigned int> position, bool positive, bool rho);

  std::pair<unsigned int, unsigned int> m_position{ 0, 0 };
  bool m_positive{ true };
  bool m_rho{ true };

  // Comparison operator to hold objects in a set
  friend bool operator==(const SatVariable &lhs, const SatVariable &rhs);
  friend bool operator<(const SatVariable &lhs, const SatVariable &rhs);
};

class SatClause
{
public:
  void addVariable(SatVariable var);

  // Return the number of variables in the clause
  long unsigned int size() const;

  enum class State {
    UNRESOLVED,
    SATISFIED,
    CONFLICTING,
    UNIT
  };

  // Return the current state of the clause
  State state() const;

  // Look up values of variables, re-evaluate state and propagate
  State evaluate(const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, int depth);

  // Comparison operator to hold objects in a set
  friend bool operator==(const SatClause &lhs, const SatClause &rhs);
  friend bool operator<(const SatClause &lhs, const SatClause &rhs);

private:
  std::set<SatVariable>
    m_variables;

  SatClause::State m_state{ SatClause::State::UNRESOLVED };
};

/* Represents a CDCL variable. 
 * Contains a boolean (or unassigned) value, 
 * and information on the decision level and 
 * antecedent to construct the implication graph
 */
class Node
{
public:
  // Consider enum class for the node state, instead of two booleans
  bool m_assigned{ false };
  bool m_value{ false };
  int m_decision_level{ -1 };
  std::vector<SatVariable> m_antecedents{};
};

/* Holds n^2 nodes, defines a permutation of the USP. 
 * At most one node per row will have a value of true, 
 * this defines the value of each item in the permutation
 */
class Permutation
{
public:
  Permutation(unsigned int n);

  // Check if the current assignment is the identity {0, ..., n}
  bool checkIdentity() const;
  // Check if any row or column is unable to have an assignment
  bool checkContradiction() const;
  // Return the next row to assign
  std::optional<unsigned int> nextAssignment() const;
  // Return which column is assigned by row
  std::optional<unsigned int> assignment(unsigned int row) const;
  // Return all antecedents of every contradictary row at decision_level
  std::vector<SatVariable> contradictionAntecedents(int decision_level) const;
  // Return all possible assignments by row
  std::vector<unsigned int> possibleAssignments(unsigned int row) const;
  // Assign element (y, x) to value
  void assign(unsigned int y, unsigned int x, bool value, int decision_level = -1, std::vector<SatVariable> antecedents = {});
  // Assigns element (y, x) to true. Performs simple unit propagation
  void assignPropagate(unsigned int y, unsigned int x, bool rho, int decision_level);
  // Undo all propagation that happened at decision_level or below
  void undoPropagation(int decision_level);
  // Return the antecedents to the Node at (assignment)
  std::vector<SatVariable> antecedents(std::pair<unsigned int, unsigned int> assignment) const;
  // Return the decision level to the Node at (assignment)
  int nodeDecisionLevel(std::pair<unsigned int, unsigned int> assignment) const;
  // Return the value of the Node at (assignment). 0 if false, 1 if true, 2 if unassigned
  int value(std::pair<unsigned int, unsigned int> assignment) const;
  // Debug log the data matrix
  void logData() const;

private:
  Matrix<Node> m_data;
  unsigned int m_size{ 0 };
};

/* Usp of size (n, k)
 */
class Usp
{
public:
  Usp(std::vector<int> data, unsigned int n, unsigned int k);

  // Query a triple of rows to determine if they satisfy the USP condition
  bool query(unsigned int a, unsigned int b, unsigned int c) const;

  unsigned int rows() const;
  unsigned int cols() const;

private:
  Matrix<int> m_data;
  std::vector<bool> m_func;
  unsigned int m_rows{ 0 };
  unsigned int m_cols{ 0 };
};


}// namespace usp


#endif