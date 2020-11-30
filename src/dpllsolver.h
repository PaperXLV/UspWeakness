#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include "usp.h"
#include "verifier.h"

#include <utility>
#include <optional>
#include <algorithm>
#include <numeric>
#include <sstream>

#include <spdlog/spdlog.h>


namespace usp {

void UspUnitPropagation(const Usp &puzzle, const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, int depth)
{
  // Modify rho and sigma to remove assignments by unit propagation
  for (unsigned int i = 0; i < puzzle.rows(); ++i) {
    // Only consider case where one of rho(i) or sigma(i) is undefined
    std::optional<unsigned int> rhoAssignment = rho->assignment(i);
    std::optional<unsigned int> sigmaAssignment = sigma->assignment(i);
    if (rhoAssignment.has_value() && !sigmaAssignment.has_value()) {
      // Remove all invalid assignments from sigma (all assignments that query returns 1)
      for (unsigned int j = 0; j < puzzle.rows(); ++j) {
        if (puzzle.query(i, rhoAssignment.value(), j)) {
          sigma->assign(i, j, false, depth);
        }
      }
    }
    if (sigmaAssignment.has_value() && !rhoAssignment.has_value()) {
      // Remove all invalid assignments from rho (all assignments that query returns 1)
      for (unsigned int j = 0; j < puzzle.rows(); ++j) {
        if (puzzle.query(i, j, sigmaAssignment.value())) {
          rho->assign(i, j, false, depth);
        }
      }
    }
  }
}

std::optional<std::pair<Permutation, Permutation>> DpllSolverImpl(const Usp &puzzle, const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, int depth)
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
    return std::make_optional<std::pair<Permutation, Permutation>>(*rho, *sigma);
  }

  spdlog::debug("Beginning Branching");
  // Branch on an assignment, applying unit propagation
  if (rhoAssignment.has_value()) {
    std::vector<unsigned int> possibleAssignments = rho->possibleAssignments(rhoAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      spdlog::debug("Attempting assignment ({}, {}) to rho", rhoAssignment.value(), assignment);

      rho->assignPropagate(rhoAssignment.value(), assignment, depth);
      UspUnitPropagation(puzzle, rho, sigma, depth);
      auto result = DpllSolverImpl(puzzle, rho, sigma, depth + 1);
      // Success!
      if (result.has_value()) {
        return result;
      }
      // Try again
      rho->undoPropagation(depth);
    }
  } else {
    std::vector<unsigned int> possibleAssignments = sigma->possibleAssignments(sigmaAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      sigma->assignPropagate(sigmaAssignment.value(), assignment, depth);
      UspUnitPropagation(puzzle, rho, sigma, depth);

      auto result = DpllSolverImpl(puzzle, rho, sigma, depth + 1);
      if (result.has_value()) {
        return result;
      }
      sigma->undoPropagation(depth);
    }
  }
  // If control ever reaches here, we have a strong USP
  return std::nullopt;
}


std::optional<std::pair<Permutation, Permutation>> DpllSolver(const Usp &puzzle)
{
  return DpllSolverImpl(puzzle, std::make_unique<Permutation>(puzzle.rows()), std::make_unique<Permutation>(puzzle.rows()), 0);
}

}// namespace usp

#endif