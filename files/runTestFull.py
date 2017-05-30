#!/usr/bin/env python
import sys, os
import subprocess
from math import sqrt

args = list(sys.argv)
if len(args) < 2:
    sys.exit("You must specify repeats!")  
repeats = int(args[1])
print("Repeats: {}\n".format(repeats))

def getD(M, spentTime):
    D = 0.00
    for value in spentTime:
        D = D + (value - M) ** 2
    if repeats == 1:
        D = D/repeats
    else:
        D = D/(repeats - 1)
    return D;

def getIntervalH(M):
    sigma = sqrt(D)
    t1=1.984
    return M + t1*(sigma/(sqrt(repeats)))

def getIntervalL(M):
    sigma = sqrt(D)
    t1=1.984
    return M - t1*(sigma/(sqrt(repeats)))


# cycle for programs
for programName in ["mergeSort", "openMPSort", "pthreadsSort"]:
    program=os.path.abspath(programName)
    # cycle for elements
    for elements in [1000, 100000, 250000, 500000, 25000000]:
        # cycle for distributions
        for path in ["tests/testUniform", "tests/testExp", "tests/testNorm"]:
	    fileName=os.path.abspath(path + str(elements) + ".txt")
            print("Program: {}, fileName: {}, N: {}".format(program, fileName, elements))
            # run program in cycle for 1, 2, 4, 8, 16 threads
            for t in [1, 2, 4, 8, 16]:
                spentTime = []
                spentTimeJustParallel = []
                for n in range(repeats):
                    p = subprocess.Popen([program, fileName, str(t)], stdout=subprocess.PIPE)
                    for line in p.stdout:
                        if 'Time spent (ms): ' in line :
                            spentTime.append(float(line.split()[-1]))
		            #print(line)
                        if 'Time spent for parallel part (ms): ' in line :
                            spentTimeJustParallel.append(float(line.split()[-1]))
		            #print(line)     
                M = sum(spentTime)/repeats
                D = getD(M, spentTime)
                print("{} threads : M = {:.4f}, D = {:.5f}, interval = [{:.4f}, {:.4f}]".format(t, M, D, getIntervalL(M), getIntervalH(M)))

                # result just for parallel part
                if (len(spentTimeJustParallel) > 0):
                    M =sum(spentTimeJustParallel)/repeats
                    D = getD(M, spentTimeJustParallel)
                    print("{} threads (just parallel part): M = {:.4f}, D = {:.5f}, interval = [{:.4f}, {:.4f}]\n".format(t, M, D, getIntervalL(M), getIntervalH(M)))
                if (t == 16):
                    print("")
                if (programName == "mergeSort"):
                    print("")
                    break
