#include <functional>
#include <iostream>

#include <spdlog/spdlog.h>

#include <docopt/docopt.h>

#include <iostream>

#include "usp.h"
#include "verifier.h"
#include "basicsolver.h"
#include "dpllsolver.h"

static constexpr auto USAGE =
  R"(Usage: usp

)";

int main(int argc, const char **argv)
{
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,// show help if requested
    "USP");// version string

  for (auto const &arg : args) {
    std::cout << arg.first << arg.second << std::endl;
  }

  //Use the default logger (stdout, multi-threaded, colored)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Debug Logging ON");

  usp::Usp puzzle({ 2, 2, 2, 3 }, 2, 2);
  /*
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


  if (usp::VerifyUspWeakness(puzzle, rho, sigma)) {
    spdlog::info("Weak USP");
  } else {
    spdlog::info("Strong USP");
  }
  */

  // Attempt dpll solver
  auto solved = usp::DpllSolver(puzzle);
}
