#!/usr/bin/env python2

import sys

from mpmath import mp
mp.dps = 50

N = long(sys.argv[1]) if len(sys.argv) > 1 else 3000000

dx = mp.mpf("0.0005")

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
        f.write('{},\n'.format(mp.atan(i * dx)))

    f.write('\n};')
