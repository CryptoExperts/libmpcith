# Import
import sys
sys.path.append('../../units')

from sage.all import *
from bigint import to_integer_repr, from_integer_repr

# Get Tested Field
F = FiniteField(2**31)
z31 = F.gen()
assert z31**31 + z31**3 + F(1) == 0, F.polynomial()

weight = 9

gs = [
2064751089, 1969700163, 2030551550, 1262412948, 1397823828, 1489973966, 1194677445, 452761747, 544290265, 
]
gs = [from_integer_repr(F, x) for x in gs]
assert len(gs) == weight

#beta = [
#242730848, 1159694551, 1072421498, 1257585086, 194796688, 873622541, 348797714, 875455990, 2090073691, 
#]
#beta = [from_integer_repr(F, x) for x in beta]
#assert len(beta) == weight

tab = [
    [
        gs[i]**(2**j)
        for j in range(weight+1)
    ] for i in range(weight)
]

mat = Matrix(F, tab)
M = Matrix(F, tab)
M, last = M[:,:weight], mat[:,weight]

for i in range(weight):
    for j in range(weight+1):
        print(to_integer_repr(mat[i][j]), end=' ')
    print()
print()

mat[0] = mat[0] / mat[0,0]
for i in range(1,weight):
    mat[i] = mat[i] - mat[i,0]*mat[0]

for i in range(weight):
    for j in range(weight+1):
        print(to_integer_repr(mat[i][j]), end=' ')
    print()
print()

mat[1] = mat[1] / mat[1,1]
for i in range(2,weight):
    mat[i] = mat[i] - mat[i,1]*mat[1]

for i in range(weight):
    for j in range(weight+1):
        print(to_integer_repr(mat[i][j]), end=' ')
    print()
print()

print('=== BETA ===')
beta = M.inverse() * (-last)
for j in range(weight):
    print(to_integer_repr(beta[j,0]), end=' ')
print()
test = M * beta + last
for j in range(weight):
    print(to_integer_repr(test[j,0]), end=' ')
print()
