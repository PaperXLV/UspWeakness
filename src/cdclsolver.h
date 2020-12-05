#ifndef CDCL_SOLVER_H
#define CDCL_SOLVER_H

#include "dpllsolver.h"

namespace usp {

std::optional<std::pair<Permutation, Permutation>> CdclSolverImpl(const Usp &puzzle, const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, unsigned int depth)
{
  // Check if any value cannot be assigned
  if (rho->checkContradiction() || sigma->checkContradiction()) {
    spdlog::debug("Contradiction found");
    return std::nullopt;
  }

  // Check assignments are not both the identity
  if (rho->checkIdentity() && sigma->checkIdentity()) {
    spdlog::debug("Identity found");
    return std::nullopt;
  }

  // Check if rho and sigma have complete assignments
  auto rhoAssignment = rho->nextAssignment();
  auto sigmaAssignment = sigma->nextAssignment();
  if (!rho->nextAssignment().has_value() && !sigma->nextAssignment().has_value()) {
    spdlog::debug("Solution found, Weak USP");
    return std::make_optional<std::pair<Permutation, Permutation>>(*rho, *sigma);
  }

  // Branch on an assignment, applying unit propagation
  if (rhoAssignment.has_value()) {
    std::vector<unsigned int> possibleAssignments = rho->possibleAssignments(rhoAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      rho->assignPropagate(rhoAssignment.value(), assignment, depth);

      // UspUnitPropagation(puzzle, rho, sigma, depth);

      auto result = DpllSolverImpl(puzzle, rho, sigma, depth + 1);
      // Success!
      if (result.has_value()) {
        return result;
      }
      // Try again
      rho->undoPropagation(depth);
      sigma->undoPropagation(depth);
    }
  } else {
    std::vector<unsigned int> possibleAssignments = sigma->possibleAssignments(sigmaAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      sigma->assignPropagate(sigmaAssignment.value(), assignment, depth);

      // UspUnitPropagation(puzzle, rho, sigma, depth);

      auto result = DpllSolverImpl(puzzle, rho, sigma, depth + 1);
      if (result.has_value()) {
        return result;
      }
      rho->undoPropagation(depth);
      sigma->undoPropagation(depth);
    }
  }
  return std::nullopt;
}

std::optional<std::pair<Permutation, Permutation>> CdclSolver(const Usp &puzzle)
{
  return CdclSolverImpl(puzzle, std::make_unique<Permutation>(puzzle.rows()), std::make_unique<Permutation>(puzzle.rows()), 0)
}

}// namespace usp


#endif