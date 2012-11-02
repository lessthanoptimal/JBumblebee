# Truncates the time log file

import os

def modifyLog( firstFrame , lastFrame ):
    fin = open('time.txt','r')
    fout = open('tmp_time.txt','w')

    line = 0
    s = fin.readline()
    while s:
        if len(s) == 0 or s[0] == '#':
            fout.write(s)
        else:
            if firstFrame <= line <= lastFrame:
                fout.write(s)
            line += 1
        s = fin.readline()

    fin.close()
    fout.close()

    os.remove('time.txt')
    os.rename('tmp_time.txt','time.txt')