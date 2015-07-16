#!/usr/bin/env python3

import sys

from mpmath import mp
mp.dps = 50

N = int(sys.argv[1]) if len(sys.argv) > 1 else 500000

dx = mp.mpf("0.002")

inicio = 0
fim = (N-1)*dx
idx = 1 / dx

with open('arc.h', 'w') as f:

    f.write("\n".join([
        'using namespace std;',
        '',
        '#define ATAN_N {N}',
        '#define ATAN_Idx {idx}',
        '#define ATAN_inicio {inicio}',
        '#define ATAN_fim {fim}',
        '',
        '',
    ]).format(**locals()))

    f.write('const double Arctg[] = {')

    for i in range(N):
        f.write('' + str(mp.atan(i * dx)) + ',\n')

    f.write('\n};')
