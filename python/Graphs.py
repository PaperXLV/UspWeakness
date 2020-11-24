from matplotlib import pyplot as plt
from USP import UspRecursive, UspWeaknessVerifier, GenerateUsp, Permutation
from Sat import SatReduction, ExtractAssignment

import numpy as np
import scipy.stats as st

import timeit
import functools
import math


def TestDPLL(Usps):
    results = []
    for Usp in Usps:
        results += [timeit.timeit(functools.partial(UspRecursive, Usp,
                                                    Permutation(len(Usp)), Permutation(len(Usp))), number=1)]
    mean, sigma = np.mean(results), np.std(results)
    return mean, st.norm.interval(0.90,
                                  loc=mean, scale=sigma)


def TestSAT(Usps):
    results = []
    for Usp in Usps:
        results += [timeit.timeit(functools.partial(SatReduction, Usp), number=1)]
    mean, sigma = np.mean(results), np.std(results)
    return mean, st.norm.interval(0.90,
                                  loc=mean, scale=sigma)


def produce_graphs():
    sizes = [x for x in range(4, 20)]
    labels = ["(" + str(x) + ", " + str(math.floor(1.5 * x)) + ")" for x in sizes]
    results = []

    samples = 100

    for size in sizes:
        print("Running size: ", size)
        # Average runtime of 100 runs on both methods for each
        # data point
        genUsps = [GenerateUsp(size, math.floor(size * 1.5))
                   for x in range(samples)]

        rec = TestDPLL(genUsps)
        sat = TestSAT(genUsps)

        results += [(rec, sat)]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(15, 15))

    fig.suptitle("90% CI of runtimes solving USP")
    fig.tight_layout(pad=12.0)

    ax1.set_title("DPLL Solver")
    ax2.set_title("SAT reduction + pysat")

    ax1.set_xlabel("USP size (n, k)")
    ax2.set_xlabel("USP size (n, k)")

    ax1.set_ylabel("Time (seconds)")
    ax2.set_ylabel("Time (seconds)")

    ax1.errorbar(labels, [result[0][0] for result in results], yerr=[
        result[0][1][1] - result[0][0] for result in results])
    ax2.errorbar(labels, [result[1][0] for result in results], yerr=[
        result[1][1][1] - result[1][0] for result in results])

    plt.show()
    fig.savefig("USP Graphs")


if __name__ == "__main__":
    produce_graphs()
