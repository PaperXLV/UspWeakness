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

  // usp::Usp medStrongPuzzle({ 1, 2, 2, 2, 2, 3, 3, 3, 2, 2, 3, 2, 2, 1, 1, 3, 2, 2, 3, 2, 3, 1, 2, 3, 3, 1, 2, 1, 1, 3, 1, 3, 2, 3, 3, 1, 3, 3, 3, 3, 2, 3, 3, 3, 2, 3, 1, 2, 1, 1, 3, 3, 1, 2, 1, 3, 1, 3, 2, 1, 2, 3, 2, 2 }, 8, 8);
  usp::Usp weakPuzzle({ 2, 2, 2, 3 }, 2, 2);


  // Attempt dpll solver
  auto solved = usp::DpllSolver(weakPuzzle);
}
