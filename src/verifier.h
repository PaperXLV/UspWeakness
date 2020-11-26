#ifndef VERIFIER_H
#define VERIFIER_H

#include "usp.h"

namespace usp {

/* Verifier using witnesses rho and sigma. 
 * Checks the condition holds for each element in the USP.
 * Returns true iff the permutations prove the usp is weak. 
 * Throws 'std::bad_optional_access' if rho or sigma don't contain 
 * a valid assignment.
 */
bool VerifyUspWeakness(const usp::Usp &usp, const Permutation &rho, const Permutation &sigma)
{
  for (unsigned int i = 0; i < usp.rows(); ++i) {
    spdlog::debug("Query: ({},{},{}): {}", i, rho.assignment(i).value(), sigma.assignment(i).value(), usp.query(i, rho.assignment(i).value(), sigma.assignment(i).value()));
    if (usp.query(i, rho.assignment(i).value(), sigma.assignment(i).value())) {
      return false;
    }
  }
  return true;
}

}// namespace usp


#endif