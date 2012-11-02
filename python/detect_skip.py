#!/usr/bin/python

# Looks a time log and sees if there are any excessively large gaps in the data
# example: ./detect_skip.py path/to/file.txt 50

import sys

def readNextLine( f ):
    while True:
        l = f.readline()
        if not l:
            return False
        if len(l) == 0 or l[0] == '#':
            continue
        return l

if len(sys.argv) <= 2:
    print 'Need to specify an input file and period'
    exit()

fileName = sys.argv[1]
period = int(sys.argv[2])

f = open(fileName,'r')

if not f:
    print "Couldn't open file",fileName
    exit()

print 'Reading file '+fileName

s = readNextLine(f)
prevTime = int(s.split(' ')[1])
s = readNextLine(f)
lineNumber = 1

while s:
    time = int(s.split(' ')[1])

    if time - prevTime > period*1.1: # give it a bit of slack timing wise
        print 'Large gap at line '+str(lineNumber)+' of '+str(time-prevTime)

    lineNumber += 1
    prevTime = time
    s = readNextLine(f)

print 'Done'

