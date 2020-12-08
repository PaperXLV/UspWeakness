#ifndef CDCL_SOLVER_H
#define CDCL_SOLVER_H

#include "dpllsolver.h"

#include <queue>
#include <set>

namespace usp {

bool ClauseUnitPropagation(const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, std::set<SatClause> &learnedClauses, int depth)
{
  bool loop = true;
  while (loop) {
    loop = false;
    for (auto &satClause : learnedClauses) {
      if (satClause.state() != SatClause::State::SATISFIED) {
        // evaluate will only modify state, which does not affect ordering of clauses.
        SatClause::State state = const_cast<SatClause &>(satClause).evaluate(rho, sigma, depth);
        if (state == SatClause::State::CONFLICTING) {
          return false;
        }
        if (state == SatClause::State::UNIT) {
          loop = true;
        }
      }
    }
  }
  return true;
}

void CdclUnitPropagation(const Usp &puzzle, const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, std::pair<unsigned int, unsigned int> assignment, bool assignmentToRho, int depth)
{
  // Apply unit propagation from setting (assignment) = true in the corresponding permutation
  for (unsigned int i = 0; i < puzzle.rows(); ++i) {
    if (assignmentToRho && puzzle.query(assignment.first, assignment.second, i)) {
      sigma->assign(assignment.first, i, false, depth, { SatVariable(assignment, true, assignmentToRho) });// antecedent should be rho(assignment)
    } else if (!assignmentToRho && puzzle.query(assignment.first, i, assignment.second)) {
      rho->assign(assignment.first, i, false, depth, { SatVariable(assignment, true, assignmentToRho) });// antecedent should be sigma(assignment)
    }
  }
  // Apply unit propagation through learned clauses
}

SatClause CdclConflictAnalysis(const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, int depth)
{
  // Simulate a sequence of resolution operations by traversing the
  // implication graph backwards
  SatClause learnedClause;
  std::queue<SatVariable> implicationGraphQueue;
  std::vector<SatVariable> rhoAntecedents = rho->contradictionAntecedents(depth);
  std::vector<SatVariable> sigmaAntecedents = sigma->contradictionAntecedents(depth);
  std::vector<SatVariable> antecedents;

  for (unsigned int i = 0; i < rhoAntecedents.size(); ++i) {
    implicationGraphQueue.push(rhoAntecedents[i]);
  }
  for (unsigned int i = 0; i < sigmaAntecedents.size(); ++i) {
    implicationGraphQueue.push(sigmaAntecedents[i]);
  }

  while (!implicationGraphQueue.empty()) {
    SatVariable front = implicationGraphQueue.front();
    implicationGraphQueue.pop();

    // In the space of variables implied at depth, continue traversing
    if (int decisionLevel = ((front.m_rho) ? rho->nodeDecisionLevel(front.m_position) : sigma->nodeDecisionLevel(front.m_position));
        decisionLevel == depth) {
      antecedents = (front.m_rho) ? rho->antecedents(front.m_position) : sigma->antecedents(front.m_position);
      // Add all antecedents of this variable to the queue
      if (antecedents.size() > 0) {
        for (unsigned int i = 0; i < antecedents.size(); ++i) {
          implicationGraphQueue.push(antecedents[i]);
        }
      }
      // No antecedents, this must be the decision variable. Add to clause
      else {
        learnedClause.addVariable(front);
      }
    }
    // Variable decided at some other level, add to clause
    else {
      learnedClause.addVariable(front);
    }
  }
  return learnedClause;
}

std::optional<std::pair<Permutation, Permutation>> CdclSolverImpl(const Usp &puzzle, const std::unique_ptr<Permutation> &rho, const std::unique_ptr<Permutation> &sigma, std::set<SatClause> &learnedClauses, int depth)
{
  // Update clauses
  /*
  for (auto &satClause : learnedClauses) {
    const_cast<SatClause &>(satClause).evaluate(rho, sigma, depth);
  }
  */

  // Check contradiction
  if (rho->checkContradiction() || sigma->checkContradiction()) {
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
    // Copy rho and sigma instead of just dereferencing.
    return std::make_optional<std::pair<Permutation, Permutation>>(*rho, *sigma);
  }

  // Branch on an assignment, applying unit propagation
  if (rhoAssignment.has_value()) {
    std::vector<unsigned int> possibleAssignments = rho->possibleAssignments(rhoAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      rho->assignPropagate(rhoAssignment.value(), assignment, true, depth);
      CdclUnitPropagation(puzzle, rho, sigma, { rhoAssignment.value(), assignment }, true, depth);

      bool success = ClauseUnitPropagation(rho, sigma, learnedClauses, depth);

      // Check if any value cannot be assigned
      if (rho->checkContradiction() || sigma->checkContradiction()) {
        SatClause learnedClause = CdclConflictAnalysis(rho, sigma, depth);
        if (learnedClause.size() != 0) {
          learnedClauses.insert(learnedClause);
        }
      }

      // Continue through the tree only if the clause propagation didn't find a contradiction
      if (success) {
        auto result = CdclSolverImpl(puzzle, rho, sigma, learnedClauses, depth + 1);

        // Success!
        if (result.has_value()) {
          return result;
        }
      }
      // Try again
      rho->undoPropagation(depth);
      sigma->undoPropagation(depth);
    }
  } else {
    std::vector<unsigned int> possibleAssignments = sigma->possibleAssignments(sigmaAssignment.value());
    for (unsigned int assignment : possibleAssignments) {
      sigma->assignPropagate(sigmaAssignment.value(), assignment, false, depth);
      CdclUnitPropagation(puzzle, rho, sigma, { sigmaAssignment.value(), assignment }, false, depth);

      bool success = ClauseUnitPropagation(rho, sigma, learnedClauses, depth);

      // Check if any value cannot be assigned
      if (rho->checkContradiction() || sigma->checkContradiction()) {
        SatClause learnedClause = CdclConflictAnalysis(rho, sigma, depth);
        if (learnedClause.size() != 0) {
          learnedClauses.insert(learnedClause);
        }
      }

      if (success) {
        auto result = CdclSolverImpl(puzzle, rho, sigma, learnedClauses, depth + 1);
        if (result.has_value()) {
          return result;
        }
      }
      rho->undoPropagation(depth);
      sigma->undoPropagation(depth);
    }
  }
  return std::nullopt;
}

std::optional<std::pair<Permutation, Permutation>> CdclSolver(const Usp &puzzle)
{
  std::set<SatClause> learnedClauses;
  return CdclSolverImpl(puzzle, std::make_unique<Permutation>(puzzle.rows()), std::make_unique<Permutation>(puzzle.rows()), learnedClauses, 0);
}

}// namespace usp


#endif