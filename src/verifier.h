#ifndef VERIFIER_H
#define VERIFIER_H

#include "usp.h"

namespace usp {

/* Verifier using witnesses rho and sigma. 
 * Checks the condition holds for each element in the USP.
 * Returns true iff the permutations prove the usp is weak. 
 */
bool VerifyUspWeakness(const usp::Usp &usp, const Permutation &rho, const Permutation &sigma)
{
  for (unsigned int i = 0; i < usp.rows(); ++i) {
    spdlog::debug("Query: ({},{},{}): {}", i, rho.assignment(i), sigma.assignment(i), usp.query(i, rho.assignment(i), sigma.assignment(i)));
    if (usp.query(i, rho.assignment(i), sigma.assignment(i))) {
      return false;
    }
  }
  return true;
}

}// namespace usp


#endif