import csv
import matplotlib.pyplot as plt
import numpy as np

# create graphs with confidence intervals for each
# instance of width 10 and 15

with open('runtime.csv') as runtime:
    reader = csv.reader(runtime, delimiter=',')
    lines = [line for line in reader]

    w10data = []
    w15data = []

    for line in lines[1:]:
        line = list(map(float, line))
        if int(line[1]) == 10:
            w10data += [(int(line[0]), line[2] / 1000, line[3] / 1000)]
        if int(line[1]) == 15:
            # Add column for 90% CI error
            w15data += [(int(line[0]), line[2] / 1000, line[3] /
                         1000, 1.645 * line[3] / 1000 / 100)]

    w10data = np.array(w10data)
    w15data = np.array(w15data)

    fig, ax1 = plt.subplots(1, 1)

    ax1.semilogy(w10data[:, 1], label="Width 10")
    ax1.semilogy(w15data[:, 1], label="Width 15")

    ax1.grid(True, which="both")
    ax1.set_title("CDCL Solver Mean Runtimes")
    ax1.set_xlabel("Depth of USP")
    ax1.set_ylabel("Mean Runtime (sec)")
    plt.legend()

    plt.show()
