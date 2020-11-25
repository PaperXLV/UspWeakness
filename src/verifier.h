#ifndef VERIFIER_H
#define VERIFIER_H

#include "usp.h"

namespace usp {

bool VerifyUspWeakness(Matrix<int> usp, Permutation rho, Permutation sigma)
{
  for (int i = 0; i < usp.rows(); ++i) {
    if (usp.query(i, rho.assignment(i), sigma.assigment(i))) {
      return false;
    }
  }
  return true;
}

}// namespace usp


#endif