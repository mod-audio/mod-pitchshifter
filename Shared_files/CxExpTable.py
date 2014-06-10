#!/usr/bin/env python2

import sys

from math import *
from decimal import *
getcontext().prec = 50

if len(sys.argv) > 1:
    N = int(sys.argv[1])
else:
    N = Decimal(20000)

inicio = Decimal(-pi)
fim = Decimal(pi)
dx = (fim-inicio)/(N-1)

Idx = Decimal(1/dx)

f = open('Exponencial.h', 'w')

f.write('#include <complex> \n \n')

f.write('using namespace std; \n \n')

f.write('#define EXP_N ' + str(N) + '\n')
f.write('#define EXP_inicio ' + str(inicio) + '\n')
f.write('#define EXP_fim ' + str(fim) + '\n\n')

f.write('const complex<double> Exponencial[] = {')

for i in range(N):
    value = Decimal ( cos( Decimal ( i*dx + inicio ) ) )
    value2 = Decimal ( sin( Decimal ( i*dx + inicio ) ) )
    v = "%.40e" % value
    v2 = "%.40e" % value2
    f.write('complex<double>(' + v + ',' + v2 + '),\n')

f.write('\n};')
f.close()
