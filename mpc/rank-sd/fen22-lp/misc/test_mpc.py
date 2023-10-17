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
length = 30

gamma = [
    1062052931, 318757425, 131536265, 251353251, 426817080, 952616989, 1075179599, 1889633908, 163191705, 1043677930, 876342569, 218484677, 1518702865, 1624552946, 34148813, 959299497, 1023783821, 509944346, 4374131, 1078746116, 1830716906, 1972610775, 292545133, 840877104, 227024977, 1545277429, 617649774, 29612971, 1114016411, 310817965, 
]
eps = 1848050849
x = [
    1776241847, 1776959591, 2066679675, 433766168, 1878158710, 1237350714, 1515446635, 535686810, 853268252, 2067101496, 1342247916, 724853972, 1693860028, 539638109, 660958860, 1220139748, 1548860473, 1070061511, 3112083, 174587479, 536230740, 1156150577, 24749022, 0, 1670126525, 1465989294, 351409822, 660958860, 1527070246, 558387021, 
]
x_pow_r = [
    494047935, 408200709, 630772071, 188220280, 453022520, 807475090, 1414055321, 422671506, 1533619720, 837003018, 1687469760, 1433868746, 1920552200, 678801815, 963837316, 2027548716, 1130858185, 826590469, 291324631, 905596899, 1186782904, 1596639269, 1224271806, 0, 1717056929, 1691267076, 801439880, 963837316, 233660656, 1057029123,
]
w = [
    405461889, 1005495624, 1846726040, 2087899250, 251012286, 1999055408, 1765560042, 1005597264, 1943651136, 
]
z = 25806321
a = [
    711452680, 696278088, 505182350, 166915252, 2146101248, 79235519, 1262174832, 1963861418, 1081361245, 
]
alpha = [
    1196953754, 476294437, 1587521115, 1231704472, 2012702548, 286408001, 1280253343, 540783710, 1083609479,
]
    

gamma = [from_integer_repr(F, v) for v in gamma]
assert len(gamma) == length
x = [from_integer_repr(F, v) for v in x]
assert len(x) == length
x_pow_r = [from_integer_repr(F, v) for v in x_pow_r]
assert len(x_pow_r) == length
w = [from_integer_repr(F, v) for v in w]
assert len(x_pow_r) == length
a = [from_integer_repr(F, v) for v in a]
assert len(a) == weight
alpha = [from_integer_repr(F, v) for v in alpha]
assert len(alpha) == weight
eps = from_integer_repr(F, eps)
z = from_integer_repr(F, z)

w_2 = [
    sum(
        gamma[j]*(x[j]**(2**i))
        for j in range(length)
    )
    for i in range(weight)
]
x_pow_r_2 = [
    v**(2**weight) for v in x
]
z_2 = sum(
    gamma[j]*(x[j]**(2**weight))
    for j in range(length)
)
alpha_2 = [
    eps*w_2[i] + a[i]
    for i in range(weight)
]

for v in x_pow_r_2:
    print(to_integer_repr(v), end=' ')
print()
for i in range(length):
    assert x_pow_r[i] == x_pow_r_2[i]

for v in w_2:
    print(to_integer_repr(v), end=' ')
print()
for i in range(weight):
    assert w[i] == w_2[i]

print(to_integer_repr(z_2), end=' ')
print()
assert z == z_2

for v in alpha_2:
    print(to_integer_repr(v), end=' ')
print()
for i in range(weight):
    assert alpha[i] == alpha_2[i]

