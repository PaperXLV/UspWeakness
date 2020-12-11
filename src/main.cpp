#include <functional>
#include <iostream>

#include <spdlog/spdlog.h>

#include <docopt/docopt.h>

#include <iostream>

#include "usp.h"
#include "uspgenerator.h"
#include "verifier.h"
#include "dpllsolver.h"
#include "cdclsolver.h"

#include <chrono>
#include <cmath>
#include <fstream>

static constexpr auto USAGE =
  R"(Usage: runsolver
Computes mean and standard deviations of the runtime of a CDCL solver on USP-Weakness. 
Outputs data into "runtime.csv" in the same directory.  

)";

static constexpr unsigned int trials = 10000;
static constexpr unsigned int maxHeight = 50;

int main(int argc, const char **argv)
{
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,// show help if requested
    "USP");// version string

  for (auto const &arg : args) {
    std::cout << arg.first << arg.second << std::endl;
  }

  // Use the default logger (stdout, multi-threaded, colored)
  spdlog::set_level(spdlog::level::info);
  spdlog::debug("Debug Logging ON");

  std::ofstream csvFile;
  csvFile.open("runtime.csv");
  csvFile << "Depth,Width,Mean(ms),Deviation(ms)\n";

  auto calculateMeanAndDeviation = [](std::vector<double> times) -> std::pair<double, double> {
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double mean = sum / static_cast<double>(times.size());
    auto variance = std::accumulate(times.begin(), times.end(), 0.0, [&mean](double acc, double elt) {
      return acc + ((elt - mean) * (elt - mean));
    });
    return std::make_pair<double, double>(sum / static_cast<double>(times.size()), std::sqrt(variance / static_cast<double>(times.size())));
  };

  usp::UspGenerator generator;
  // Generate and write data for (i, j) USPs
  auto generateData = [&generator, &csvFile, &calculateMeanAndDeviation](unsigned int i, unsigned int j) {
    std::vector<double> executionTimes;
    executionTimes.reserve(trials);
    for (unsigned int k = 0; k < trials; ++k) {
      usp::Usp usp = generator.generateRandomPuzzle(i, j);
      auto startTime = std::chrono::steady_clock::now();
      auto solution = usp::CdclSolver(usp);
      auto endTime = std::chrono::steady_clock::now();
      // Time in seconds
      std::chrono::duration<double> duration = endTime - startTime;
      executionTimes.push_back(duration.count());
      // Verify solution
      if (solution.has_value()) {
        auto [rho, sigma] = solution.value();
        if (!usp::VerifyUspWeakness(usp, rho, sigma)) {
          spdlog::info("CDCL solver failure");
        }
      }
    }
    // Report mean and standard deviation of runtimes to file
    auto [mean, deviation] = calculateMeanAndDeviation(executionTimes);
    csvFile << i << "," << j << "," << mean * 1000 << "," << deviation * 1000 << std::endl;
  };

  for (unsigned int i = 1; i < maxHeight + 1; ++i) {
    generateData(i, 10);
    generateData(i, 15);
  }

  csvFile.close();
}
