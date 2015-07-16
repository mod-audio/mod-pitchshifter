#!/usr/bin/env python3

import sys

from mpmath import mp
mp.dps = 50
    
N = long(sys.argv[1]) if len(sys.argv) > 1 else 20000

inicio = -1*mp.pi
fim = 1*mp.pi
dx = (fim-inicio)/(N-1)
idx = 1 / dx

Idx = mp.mpf(1/dx)

with open('Sin.h', 'w') as f:

    f.write("\n".join([
        '#define SIN_N {N}',
        '#define SIN_Idx {idx}',
        '#define SIN_inicio {inicio}',
        '#define SIN_fim {fim}',
        '',
        '',
    ]).format(**locals()))

    f.write('const double Sin[] = {')

    for i in range(N):
        f.write('' + str(mp.sin(i*dx + inicio)) + ',\n')

    f.write('\n};')