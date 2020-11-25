#ifndef BASIC_SOLVER_H
#define BASIC_SOLVER_H

#include "usp.h"
#include "verifier.h"

#include <utility>
#include <optional>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace usp {

/* Naive algorithm to solve USP Weakness
 * Attempts all permutations (n!)
 * Returns a pair of permutations if one has been found 
 * which verifies the USP as weak.
 */
std::optional<std::pair<Permutation, Permutation>> BasicSolver(Usp puzzle)
{
  // Generate the identity {0, ..., n}
  auto generateRange = [&puzzle]() -> std::vector<unsigned int> {
    std::vector<unsigned int> range(puzzle.rows());
    std::iota(range.begin(), range.end(), 0);
    return range;
  };

  // O(n) algorithm to check if a vector is the identity {0, ..., n}
  auto isIdentity = [](const std::vector<unsigned int> &range) {
    auto it = range.begin();
    while (it != range.end()) {
      if (*it != std::distance(range.begin(), it)) {
        return false;
      }
      ++it;
    }
    return true;
  };

  Permutation rho(puzzle.rows());
  Permutation sigma(puzzle.rows());

  std::vector<unsigned int> range1 = generateRange();
  do {
    std::vector<unsigned int> range2 = generateRange();
    for (unsigned int i = 0; i < puzzle.rows(); ++i) {

      rho.assign(i, range1[i], true);
    }
    do {
      // Check at least one string is not the identity
      if (!isIdentity(range1) || !isIdentity(range2)) {
        for (unsigned int i = 0; i < puzzle.rows(); ++i) {
          sigma.assign(i, range2[i], true);
        }
        if (VerifyUspWeakness(puzzle, rho, sigma)) {
          return std::make_optional<std::pair<Permutation, Permutation>>(rho, sigma);
        }
      }
    } while (std::next_permutation(range2.begin(), range2.end()));
  } while (std::next_permutation(range1.begin(), range1.end()));

  // Strong USP, return nullopt
  return std::nullopt;
}

}// namespace usp


#endif