#!/usr/bin/env python2

import sys

from mpmath import mp
mp.dps = 50
    
N = long(sys.argv[1]) if len(sys.argv) > 1 else 20000

inicio = -1*mp.pi
fim = 1*mp.pi
dx = (fim-inicio)/(N-1)
idx = 1 / dx

Idx = mp.mpf(1/dx)

with open('Cos.h', 'w') as f:

    f.write("\n".join([
        '#define COS_N {N}',
        '#define COS_Idx {idx}',
        '#define COS_inicio {inicio}',
        '#define COS_fim {fim}',
        '',
        '',
    ]).format(**locals()))

    f.write('const double Cos[] = {')

    for i in range(N):
        f.write('' + str(mp.cos(i*dx + inicio)) + ',\n')

    f.write('\n};')