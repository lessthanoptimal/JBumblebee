#!/usr/bin/python

# Uses removes the tail and head from an image sequence then creates a video using ffmpeg

import sys
import os
from subprocess import call
from modify_time_log import *

if len(sys.argv) <= 3:
    print 'Need to specify the directory, first frame (inclusive), last frame (inclusive)'
    exit()

dirName = sys.argv[1]
firstFrame = int(sys.argv[2])
lastFrame = int(sys.argv[3])

if lastFrame <= firstFrame:
    print 'the last frame is expected to be more than the first frame'
    exit()

# Find all the image files in the directory
listLeft = []
listRight = []

l = os.listdir(dirName)

for f in l:
    if f.find('left') == 0:
        listLeft.append(f)
    elif f.find('right') == 0:
        listRight.append(f)

# sanity check
if len(listLeft) != len(listRight):
    print 'Lists have different number of elements ',len(listLeft),' ',len(listRight)
    exit()
elif not len(listLeft):
    print 'No images found'
    exit()
elif len(listLeft) < lastFrame:
    print 'Number of images is less than the specified last frame'
    exit()

# order the two lists
listLeft.sort()
listRight.sort()

# delete the head and tails of the sequence
print 'Deleting the head and tail'
for i in range(0,firstFrame):
    os.remove(os.path.join(dirName, listLeft[i]))
    os.remove(os.path.join(dirName, listRight[i]))

for i in range(lastFrame+1,len(listLeft)):
    os.remove(os.path.join(dirName, listLeft[i]))
    os.remove(os.path.join(dirName, listRight[i]))

print 'Renaming files'
suffix = listLeft[0][-3:]

for i in range(firstFrame,lastFrame+1):
    num = i-firstFrame
    nameLeft = 'left%06d.%s' % (num,listLeft[i][-3:])
    nameRight = 'right%06d.%s' % (num,listRight[i][-3:])

    nameLeft = os.path.join(dirName, nameLeft)
    nameRight = os.path.join(dirName, nameRight)

    os.rename(os.path.join(dirName,listLeft[i]),nameLeft)
    os.rename(os.path.join(dirName,listRight[i]),nameRight)

print 'Switching directories'
os.chdir(dirName)

# Modify log file
print 'Modifying time log'
modifyLog(firstFrame,lastFrame)

# convert into a movie
print 'creating left movie'
call(["ffmpeg", "-i",'left%06d.'+suffix,'-vcodec','ffv1','left.avi'])
print 'creating right movie'
call(["ffmpeg", "-i",'right%06d.'+suffix,'-vcodec','ffv1','right.avi'])


print 'Done'

