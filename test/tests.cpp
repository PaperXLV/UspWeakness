#include <catch2/catch.hpp>

#include <spdlog/spdlog.h>

#include "usp.h"
#include "verifier.h"
#include "basicsolver.h"

TEST_CASE("USP and Permutation construction", "[usp]")
{
  usp::Usp puzzle({ 2, 2, 2, 3 }, 2, 2);

  usp::Permutation rho(2);

  // Rho swaps 0 and 1
  rho.assign(0, 0, false);
  rho.assign(0, 1, true);
  rho.assign(1, 0, true);
  rho.assign(1, 1, false);

  REQUIRE(!puzzle.query(0, 0, 0));
  REQUIRE(puzzle.query(0, 0, 1));
  REQUIRE(rho.assignment(0) == 1);
  REQUIRE(rho.assignment(1) == 0);
}

TEST_CASE("USP Verifier on small weak puzzles", "[usp]")
{
  usp::Usp puzzle({ 2, 2, 2, 3 }, 2, 2);

  usp::Permutation rho(2);
  usp::Permutation sigma(2);

  // Rho swaps 0 and 1
  rho.assign(0, 0, false);
  rho.assign(0, 1, true);
  rho.assign(1, 0, true);
  rho.assign(1, 1, false);

  sigma.assign(0, 0, false);
  sigma.assign(0, 1, true);
  sigma.assign(1, 0, true);
  sigma.assign(1, 1, false);

  REQUIRE(usp::VerifyUspWeakness(puzzle, rho, sigma));
}

TEST_CASE("USP Verifier failure on small strong puzzles", "[usp]")
{
  usp::Usp puzzle({ 1, 1, 2, 3 }, 2, 2);

  usp::Permutation rho(2);
  usp::Permutation sigma(2);

  // Rho swaps 0 and 1
  rho.assign(0, 0, false);
  rho.assign(0, 1, true);
  rho.assign(1, 0, true);
  rho.assign(1, 1, false);

  sigma.assign(0, 0, false);
  sigma.assign(0, 1, true);
  sigma.assign(1, 0, true);
  sigma.assign(1, 1, false);

  REQUIRE(!usp::VerifyUspWeakness(puzzle, rho, sigma));
}

TEST_CASE("Basic Solver works on small puzzles", "[solver]")
{
  usp::Usp weakPuzzle({ 2, 2, 2, 3 }, 2, 2);
  usp::Usp strongPuzzle({ 1, 1, 2, 3 }, 2, 2);

  auto solver = usp::BasicSolver(weakPuzzle);
  auto solverStrong = usp::BasicSolver(strongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!solverStrong.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(weakPuzzle, rho, sigma));
}
