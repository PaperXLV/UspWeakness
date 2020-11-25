#ifndef USP_H
#define USP_H

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

  Matrix<T>(unsigned int n, unsigned int k, std::vector<T> data) : m_rows(n), m_cols(k), m_data(data)
  {}

  T &operator()(size_t y, size_t x)
  {
    return m_data.at(y * m_cols + x);
  }

private:
  std::vector<T> m_data;
  int m_rows{ 0 };
  int m_cols{ 0 };
};


/* Represents a CDCL variable. 
 * Contains a boolean (or unassigned) value, 
 * and information on the decision level and 
 * antecedent to construct the implication graph
 */
class Node
{
public:
private:
  bool m_assigned{ false };
  bool m_value{ false };
  int m_decision_level{ -1 };
  // antecedent?
};

/* Holds n^2 nodes, defines a permutation of the USP. 
 * At most one node per row will have a value of true, 
 * this defines the value of each item in the permutation
 */
class Permutation
{
public:
  Permutation(int n);

  int assignment(int row);

private:
  Matrix<Node> m_data;
};

/* Usp of size (n, k)
 */
class Usp
{
public:
  Usp(std::vector<int> data, unsigned int n, unsigned int k);

  // Query a triple of rows to determine if they satisfy the USP condition
  int query(unsigned int a, unsigned int b, unsigned int c) const;

  int rows() const;
  int cols() const;

private:
  Matrix<int> m_data;
  std::vector<bool> m_func;
  int m_rows{ 0 };
  int m_cols{ 0 };
};


}// namespace usp


#endif