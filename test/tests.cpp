#include <catch2/catch.hpp>

#include <spdlog/spdlog.h>

#include "usp.h"
#include "verifier.h"
#include "basicsolver.h"
#include "cdclsolver.h"
#include "dpllsolver.h"

namespace data {
const usp::Usp weakPuzzle({ 2, 2, 2, 3 }, 2, 2);
const usp::Usp strongPuzzle({ 1, 1, 2, 3 }, 2, 2);

const usp::Usp medWeakPuzzle({ 3, 2, 3, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 3, 1, 2, 3, 2, 2, 3, 3, 3, 3, 2, 3, 2, 3, 1, 1, 1, 2, 1, 1, 2, 3, 3, 2, 3, 1, 3, 3, 2, 3, 3, 2, 3, 3, 1, 3, 1, 2, 3, 1, 1, 3, 3, 3, 2, 3, 1, 1, 3, 3, 3 }, 8, 8);
const usp::Usp medStrongPuzzle({ 1, 2, 2, 2, 2, 3, 3, 3, 2, 2, 3, 2, 2, 1, 1, 3, 2, 2, 3, 2, 3, 1, 2, 3, 3, 1, 2, 1, 1, 3, 1, 3, 2, 3, 3, 1, 3, 3, 3, 3, 2, 3, 3, 3, 2, 3, 1, 2, 1, 1, 3, 3, 1, 2, 1, 3, 1, 3, 2, 1, 2, 3, 2, 2 }, 8, 8);
}// namespace data

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
  REQUIRE(rho.assignment(0).value() == 1);
  REQUIRE(rho.assignment(1).value() == 0);
}

TEST_CASE("USP Verifier on small weak puzzles", "[usp]")
{
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

  REQUIRE(usp::VerifyUspWeakness(data::weakPuzzle, rho, sigma));
}

TEST_CASE("USP Verifier failure on small strong puzzles", "[usp]")
{
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

  REQUIRE(!usp::VerifyUspWeakness(data::strongPuzzle, rho, sigma));
}

TEST_CASE("Basic Solver works on small puzzles", "[solver]")
{
  auto solver = usp::BasicSolver(data::weakPuzzle);
  auto solverStrong = usp::BasicSolver(data::strongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!solverStrong.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(data::weakPuzzle, rho, sigma));
}

TEST_CASE("DPLL Solver works on small puzzles", "[solver]")
{
  auto solver = usp::DpllSolver(data::weakPuzzle);
  auto solverStrong = usp::DpllSolver(data::strongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!solverStrong.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(data::weakPuzzle, rho, sigma));
}

TEST_CASE("DPLL Solver works on medium sized puzzles", "[solver]")
{
  auto solver = usp::DpllSolver(data::medWeakPuzzle);
  auto strongSolver = usp::DpllSolver(data::medStrongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!strongSolver.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(data::medWeakPuzzle, rho, sigma));
}

TEST_CASE("CDCL Solver works on small puzzles", "[solver]")
{
  auto solver = usp::CdclSolver(data::weakPuzzle);
  auto solverStrong = usp::CdclSolver(data::strongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!solverStrong.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(data::weakPuzzle, rho, sigma));
}

TEST_CASE("CDCL Solver works on medium sized puzzles", "[solver]")
{
  auto solver = usp::CdclSolver(data::medWeakPuzzle);
  auto strongSolver = usp::CdclSolver(data::medStrongPuzzle);
  REQUIRE(solver.has_value());
  REQUIRE(!strongSolver.has_value());
  auto [rho, sigma] = solver.value();
  REQUIRE(usp::VerifyUspWeakness(data::medWeakPuzzle, rho, sigma));
}
