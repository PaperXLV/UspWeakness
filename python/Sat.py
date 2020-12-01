from pysat.solvers import Glucose3
from USP import UspWeaknessVerifier, GenerateUsp
import math


def ExtractAssignment(U, model):
    """
    Extracts assignment in permutations given a model from pysat
    """
    n = len(U)
    k = len(U[0])

    # Function to translate any glucose number into it's
    # corresponding x_{ij}
    def xi(g):
        g = g - 1
        i = g // n
        j = g - (i * n)
        return (i, j)

    # Function to translate any glucose number into it's
    # corresponding y_{ij}
    def yi(g):
        g = g - 1
        i = (g - (n * n)) // n
        j = g - (n * n) - (i * n)
        return (i, j)

    p = [-1 for x in range(n)]
    s = [-1 for x in range(n)]
    for m in model:
        if m > 0 and m <= (n * n):
            i, j = xi(m)
            p[j] = i
        elif m > 0 and m > (n * n):
            i, j = yi(m)
            s[j] = i
    return p, s


def SatReduction(U):
    """
    Reduce USP U to a CNF-SAT formula
    """
    g = Glucose3()

    n = len(U)
    k = len(U[0])

    # Function to translate any x_{ij} into it's corresponding
    # number for glucose
    def x(i, j): return (i * n) + j + 1

    # Function to translate any y_{ij} into it's corresponding
    # number for glucose
    def y(i, j): return (n * n) + (i * n) + j + 1
    """
    Clauses to define valid permutations
    """
    for i in range(n):
        for j in range(n):
            # Add clauses to require each number can only appear
            # once in each permutation
            for l in range(n):
                if l != i:
                    g.add_clause([-x(i, j), -x(l, j)])
                    g.add_clause([-y(i, j), -y(l, j)])
            # Add clausees to require only one assignment per
            # spot in each permutation
            for l in range(n):
                if l != j:
                    g.add_clause([-x(i, j), -x(i, l)])
                    g.add_clause([-y(i, j), -y(i, l)])

    # Add clauses to require each entry of a permutation has
    # an assignment
    for j in range(n):
        clause_x = []
        clause_y = []
        for i in range(n):
            clause_x += [x(i, j)]
            clause_y += [y(i, j)]
        g.add_clause(clause_x)
        g.add_clause(clause_y)

    # Add clause to require both permutations are not the identity
    clause = []
    for i in range(n):
        clause += [-x(i, i), -y(i, i)]
    g.add_clause(clause)

    """
    Clauses to encode USP conditions
    """
    for i in range(n):
        for j in range(k):
            # U[p(i)][j] = 2 and U[s(i)][j] = 3 must agree
            if U[i][j] == 1:
                for m in range(n):
                    if m != i:
                        # An assignment of p(i) = m => s(i) = k
                        # for some k s.t. U[k][j] = 3
                        if U[m][j] == 2:
                            for l in range(n):
                                if U[l][j] != 3:
                                    g.add_clause([-x(m, i), -y(l, i)])
                        # An assignment of s(i) = m => p(i) = k
                        # for some k s.t. U[k][j] = 2
                        if U[m][j] == 3:
                            for l in range(n):
                                if U[l][j] != 2:
                                    g.add_clause([-y(m, i), -x(l, i)])
            # U[p(i)][j] = 2 and U[s(i)][j] = 3 must not both hold
            else:
                for m in range(n):
                    if U[m][j] == 2:
                        for l in range(n):
                            if U[l][j] == 3:
                                g.add_clause([-x(m, i), -y(l, i)])

    return (g.solve(), g.get_model())


def verify_solution():
    """
    Ensures the verifier accepts permutations output by the 
    CNF-SAT reduction
    """
    failed = 0
    for i in range(1000):
        print("USP " + str(i))
        genUsp = GenerateUsp(20, 20)
        res, model = SatReduction(genUsp)

        if res:
            p, s = ExtractAssignment(genUsp, model)
            if not UspWeaknessVerifier(genUsp, p, s):
                failed += 1
                print("Failed to verify :(")
    if failed == 0:
        print("No failures!")
    else:
        print("Failed: " + str(failed / 1000 * 100) + "%")


if __name__ == "__main__":
    # verify_solution()
    usp = GenerateUsp(8, 8)
    res, model = SatReduction(usp)
    if res:
        print(usp)
        p, s = ExtractAssignment(usp, model)
        print(p)
        print(s)
    else:
        print("Strong")
        print(usp)
