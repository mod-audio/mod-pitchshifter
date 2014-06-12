#!/usr/bin/env python2

import sys

from math import *
from decimal import *
getcontext().prec = 50

if len(sys.argv) > 1:
    N = int(sys.argv[1])
else:
    N = Decimal(3000000)

dx = Decimal(0.0005)

inicio = 0
fim = (N-1)*dx
Idx = Decimal(1/dx)

f = open('arc.h', 'w')

f.write('using namespace std; \n \n')

f.write('#define ATAN_N ' + str(N) + '\n')
f.write('#define ATAN_Idx ' + str(Idx) + '\n')
f.write('#define ATAN_inicio ' + str(inicio) + '\n')
f.write('#define ATAN_fim ' + str(fim) + '\n\n')

f.write('const double Arctg[] = {')

for i in range(N):
    value = Decimal ( atan( Decimal ( i*dx ) ) )
    v = "%.40e" % value
    f.write(v + ',\n')	

f.write('\n};')
f.close()
