#!/usr/bin/env python
import sys, os
import subprocess
from math import sqrt

args = list(sys.argv)
if len(args) < 4:
    sys.exit("You must specify programm, input file and repeats!")  
programm = os.path.abspath(args[1])
fileName = os.path.abspath(args[2])
repeats = int(args[3])

# run program in cycle for 1, 2, 3, 4, 6, 8, 16 threads
for t in [1, 2, 3, 4, 6, 8, 16]:
    spentTime = []
    spentTimeJustParallel = []
    for n in range(repeats):
        p = subprocess.Popen([programm, fileName, str(t)], stdout=subprocess.PIPE)
        for line in p.stdout:
            if 'Time spent (ms): ' in line :
                spentTime.append(float(line.split()[-1]))
		#print(line)
            if 'Time spent for parallel part (ms): ' in line :
                spentTimeJustParallel.append(float(line.split()[-1]))
		#print(line)     
    M =sum(spentTime)/repeats
    D = 0.00
    for value in spentTime:
        D = D + (value - M) ** 2
    if repeats == 1:
        D = D/repeats
    else:
        D = D/(repeats - 1)

    sigma = sqrt(D)
    t1=1.984
    intervalH = M + t1*(sigma/(sqrt(repeats)))
    intervalL = M - t1*(sigma/(sqrt(repeats)))
    print("{} threads : M = {:.4f}, D = {:.4f}, interval = [{:.4f}, {:.4f}]".format(t, M, D, intervalL, intervalH))

    # result just for parallel part
    if (len(spentTimeJustParallel) > 0):
        M =sum(spentTimeJustParallel)/repeats
        D = 0.00
        for value in spentTimeJustParallel:
            D = D + (value - M) ** 2
        if repeats == 1:
            D = D/repeats
        else:
            D = D/(repeats - 1)

        sigma = sqrt(D)
        t1=1.984
        intervalH = M + t1*(sigma/(sqrt(repeats)))
        intervalL = M - t1*(sigma/(sqrt(repeats)))
        print("{} threads (just parallel part): M = {:.4f}, D = {:.4f}, interval = [{:.4f}, {:.4f}]\n".format(t, M, D, intervalL, intervalH))
