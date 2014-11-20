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

with open('Exponencial.h', 'w') as f:

    f.write("\n".join([
        '#include <complex>',
        '',
        'using namespace std;',
        '',
        '#define EXP_N {N}',
        '#define EXP_Idx {idx}',
        '#define EXP_inicio {inicio}',
        '#define EXP_fim {fim}',
        '',
        '',
    ]).format(**locals()))

    f.write('const complex<double> Exponencial[] = {')

    for i in range(N):
        value = mp.cos(i*dx + inicio)
        value2 = mp.sin(i*dx + inicio)
        v = "%.50e" % value
        v2 = "%.50e" % value2
        f.write('complex<double>(' + v + ',' + v2 + '),\n')

    f.write('\n};')
