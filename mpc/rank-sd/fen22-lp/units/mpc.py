from abstractmpc import AbtractMPC
from sage.all import *
from bigint import from_integer_repr

def format_data_old(F, d):
    formatted_d = {}
    for key, value in d.items():
        assert len(value) % 4 == 0
        formatted_value = []
        for i in range(len(value)//4):
            v = 0
            for j in range(4):
                v += value[4*i+j] * (256**(j))
            formatted_value.append(
                from_integer_repr(F, v)
            )
        formatted_d[key] = formatted_value
    return formatted_d

def format_data(F, value):
    assert len(value) % 4 == 0
    formatted_value = []
    for i in range(len(value)//4):
        v = 0
        for j in range(4):
            v += value[4*i+j] * (256**(j))
        formatted_value.append(
            from_integer_repr(F, v)
        )
    return formatted_value

def to_matrix(l, n):
    return [l[i:i+n] for i in range(0, len(l), n)]

class MPC(AbtractMPC):
    def __init__(self):
        super().__init__()

        # Get Field
        F = FiniteField(2**31)
        z31 = F.gen()
        assert z31**31 + z31**3 + F(1) == 0, F.polynomial()
        self.F = F

    def format(self, name, value):
        if name in ['x_A', 'x_B', 'beta', 'c', 'z', 'v', 'eps', 'y', 'H']:
            return format_data(self.F, value)
        elif name in ['a', 'w', 'alpha', 'gamma']:
            return to_matrix(format_data(self.F, value), self.param['eta'])
        
    def get_correlated(self):
        beta = self.get_input('beta')
        a = self.get_input('a')

        r = self.param['r']
        eta = self.param['eta']

        c = [
            - sum(
                beta[j]*a[j][i]
                for j in range(r)
            )
            for i in range(eta)
        ]

        return {
            'c': c
        }
    
    def run(self):
        x_A = self.get_input('x_A')
        beta = self.get_input('beta')
        a = self.get_input('a')
        c = self.get_input('c')

        eps = self.get_challenge('eps')
        gamma = self.get_challenge('gamma')

        y = self.get_inst('y')
        H = self.get_inst('H')

        n = self.param['n']
        k = self.param['k']
        r = self.param['r']
        eta = self.param['eta']

        # Compute x_B
        #size_col = (32//4)*ceil((n-k)*4/32)
        size_col = n-k
        x_B = [
            y[i] - sum(
                H[size_col*j+i]*x_A[j]
                for j in range(k)
            )
            for i in range(n-k)
        ]
        self.log('x_B', x_B)

        x = x_A + x_B
        assert len(x) == n

        assert eta == 1

        # Compute w
        w = [
            [sum(
                gamma[j][0]*(x[j]**(2**i))
                for j in range(n)
            )]
            for i in range(r)
        ]
        self.log('w', w)

        # Compute z
        z = [sum(
            gamma[j][0]*(x[j]**(2**r))
            for j in range(n)
        )]
        self.log('z', z)
        alpha = [
            [eps[0]*w[i][0] + a[i][0]]
            for i in range(r)
        ]
        self.log('alpha', alpha)
        v = [eps[0]*z[0] - c[0] - sum(
            alpha[i][0] * beta[i]
            for i in range(r)
        )]
        self.log('v', v)

