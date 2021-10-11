from math import e

EPSILON = 1e-12


def derivative(func, x):
    return (func(x + EPSILON) - func(x)) / EPSILON


def newtons_method(func, x=1, decimals=6):
    """Finds a root of func"""
    try:
        for i in range(1000):
            f = func(x)
            d = derivative(func, x)
            if d == 0:
                d += EPSILON
            x_new = x - (f/d)
            assert not isinstance(x_new, complex)
            print(f"Iteration {i+1}: {x_new}")
            if round(x, decimals + 2) == round(x_new, decimals + 2):
                if round(func(x), decimals) == 0:
                    print(f"Found root: {round(x_new, decimals)}")
                    return 0
            x = x_new
        print("No root found after 1000 iterations.")
    except TypeError:
        print("Invalid input function")
    except AssertionError:
        print("Complex roots are not implemented")


################### Example running #######################
from math import sin

func = lambda x: e**x + x - 3
#func = lambda x: x**2 + 8
#func = lambda x: -x**4 - 3*x**2 + 9.2*x + 1943.1328
#func = lambda x: -sin(13 - x) + x**2
#func = lambda x: 8*x ** -1/x + x
#func = lambda x: -sin(13 - x) ** x**2       # complex roots are not implemented
#func = lambda x: x + "abcdef"               # invalid function
newtons_method(func, 1, 6)
