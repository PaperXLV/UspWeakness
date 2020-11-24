from itertools import permutations
import numpy as np
import random
import copy
import timeit
import functools


class Node:
    def __init__(self, n, value=None, potential=None):
        self.value = -1
        self.potential = list(range(n))
        if value is not None:
            self.value = value
        if potential is not None:
            self.potential = potential

    def __str__(self):
        return str(self.value)

    def __repr__(self):
        return str(self.value)


class Permutation:
    def __init__(self, n, values=None):
        self.values = [Node(n) for x in range(n)]
        if values is not None:
            self.values = values

    def __str__(self):
        return str(self.values)

    def __repr__(self):
        return str(self.values)


def UspWeaknessVerifier(U, p, s):
    """
    Verifies an assigment to permutations @p and @s.
    Returns True iff the assignment shows @U is a weak USP.
    """
    n = len(U)
    k = len(U[0])

    def is_identity(x): return sum([x[i] == i for i in range(n)]) == n

    if is_identity(p) and is_identity(s):
        return False

    if len(p) != n or len(s) != n:
        print("Wrong dimensions for permutations")
        return False

    for i in range(n):
        for j in range(k):
            satisfied = 0
            satisfied += U[i][j] == 1
            satisfied += U[p[i]][j] == 2
            satisfied += U[s[i]][j] == 3

            if satisfied == 2:
                return False
    return True


def UspUnitPropagation(U, p, s):
    """
    Modify p and s to remove assignments by unit propagation
    """
    n = len(U)

    for i in range(n):
        # Only consider the case where only one of p[i] or s[i] is undefined
        if p.values[i].value != -1 and s.values[i].value == -1:
            for j in range(len(U[i])):
                # Count satisfied conditions
                satisfied = (U[i][j] == 1) + (U[p.values[i].value][j] == 2)
                if satisfied == 1:
                    # Remove all potential k s.t. U[k][j] == 3
                    for k in range(n):
                        if U[k][j] == 3:
                            if k in s.values[i].potential:
                                s.values[i].potential.remove(k)
                elif satisfied == 2:
                    # Remove all potential k s.t. U[k][j] != 3
                    for k in range(n):
                        if U[k][j] != 3:
                            if k in s.values[i].potential:
                                s.values[i].potential.remove(k)
        # Probably don't use this due to the way I'm assigning, but I'll add it anyway
        elif p.values[i].value == -1 and s.values[i].value != -1:
            for j in range(len(U[i])):
                # Count satisfied conditions
                satisfied = (U[i][j] == 1) + (U[s.values[i].value][j] == 3)
                if satisfied == 1:
                    # Remove all potential k s.t. U[k][j] == 2
                    for k in range(n):
                        if U[k][j] == 2:
                            if k in p.values[i].potential:
                                p.values[i].potential.remove(k)
                elif satisfied == 2:
                    # Remove all potential k s.t. U[k][j] != 2
                    for k in range(n):
                        if U[k][j] != 2:
                            if k in p.values[i].potential:
                                p.values[i].potential.remove(k)


def UspRecursive(U, p, s):
    """
    Recursive DPLL-style backtracking USP solver
    """
    # Check if any value cannot be assigned
    for (e1, e2) in zip(p.values, s.values):
        if len(e1.potential) == 0 and e1.value == -1 \
                or len(e2.potential) == 0 and e2.value == -1:
            return False, p, s
    # Check the assignment is not the identity
    for i in range(len(U)):
        if p.values[i].value != i or s.values[i].value != i:
            break
    else:
        return False, p, s
    # Check if everything is assigned
    for (e1, e2) in zip(p.values, s.values):
        if e1.value == -1 or e2.value == -1:
            break
    else:
        return True, p, s

    # Perform unit propogation
    UspUnitPropagation(U, p, s)

    # Branching, choose a literal to assign
    for i in range(len(U)):
        if p.values[i].value == -1:
            # try all possible assignments to p.values[i]
            for assignment in p.values[i].potential:
                copyp = copy.deepcopy(p)
                copyp.values[i].value = assignment

                for j in range(len(U)):
                    if assignment in copyp.values[j].potential:
                        copyp.values[j].potential.remove(assignment)

                # issue is these recursive calls, I need to
                # have immutable p and s
                ret = UspRecursive(U, copyp, copy.deepcopy(s))
                if ret[0]:
                    return ret
                # handle base case where?
                # handle propogation where?
            break
        elif s.values[i].value == -1:
            # try all possible assignments to s.values[i]
            for assignment in s.values[i].potential:
                copys = copy.deepcopy(s)
                copys.values[i].value = assignment

                # propogation, no other element in s can have
                # the same assignment
                for j in range(len(U)):
                    if assignment in copys.values[j].potential:
                        copys.values[j].potential.remove(assignment)

                ret = UspRecursive(U, copy.deepcopy(p), copys)
                if ret[0]:
                    return ret
            break
    # Control will only ever reach here if we have a strong USP
    return False, p, s


def UspWeakness(U):
    """
    Naive USP solver, attemps all possible permutations
    for p and s.
    """
    n = len(U)
    k = len(U[0])

    # naive solution (n!), iterate through all possible permutations
    p_perm = list(permutations(range(n)))
    s_perm = list(permutations(range(n)))

    for p in p_perm:
        for s in s_perm:
            if UspWeaknessVerifier(U, p, s):
                return True, (p, s)
    return False, "Strong"


def GenerateUsp(n, k):
    """
    Generate a USP of size n x k
    """
    return [random.choices([1, 2, 3], k=k) for x in range(n)]


def verify_solution():
    """
    Ensures naive UspWeakness and advanced UspRecursive agree on randomly generated puzzles
    """
    failed = 0
    strong = 0
    for i in range(1000):
        print("USP " + str(i))
        genUsp = GenerateUsp(4, 6)
        res1 = UspRecursive(copy.deepcopy(genUsp),
                            Permutation(4), Permutation(4))
        res2 = UspWeakness(copy.deepcopy(genUsp))
        if res1[0] != res2[0]:
            failed += 1
        if res1[0]:
            # Run verifier on output permutations
            if not UspWeaknessVerifier(genUsp, [x.value for x in res1[1].values], [x.value for x in res1[2].values]):
                failed += 1
                print("Failed to verify :(")
        else:
            strong += 1
    if failed == 0:
        print("No failures!")
    else:
        print("Failed: " + str(failed / 1000 * 100) + "%")
    print("Total ", strong, " strong USPs")


if __name__ == "__main__":
    verify_solution()
