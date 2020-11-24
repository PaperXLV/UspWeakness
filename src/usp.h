#ifndef USP_H
#define USP_H

#include <bitset>

namespace usp {

/* Dense (n, k) matrix of objects of type T.
 * Used both to represent USPs and to hold 
 * CDCL variables in each Permutation
 */
template<typename T>
class Matrix
{
public:
  Matrix<T>(int n, int k) : m_rows(n), m_cols(k)
  {
    m_data = std::vector<T>(n * k, T());
  }

  Matrix<T>(int n, int k, std::vector<T> data) : m_rows(n), m_cols(k), m_data(data)
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
  // True, False, or unassigned
  std::bitset<2> m_value;
  int m_decision_level{ -1 };
  // antecedent?
};

/* Holds n^2 nodes, defines a permutation of the USP
 */
class Permutation
{
public:
  Permutation(int n);

private:
  Matrix<Node> m_data;
};


}// namespace usp


#endif