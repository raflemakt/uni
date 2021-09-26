h = 1e-8

def f(x):
    return (1 + (1/x))**x

def derive(func, x):
    return (func(x + h) - func(x))/ h

d = derive(f, 0.1)
print(d)
