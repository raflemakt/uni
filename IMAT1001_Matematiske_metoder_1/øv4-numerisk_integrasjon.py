def midtpkt(func, a, b, N=10, resolution=6):
    print(f"Midtpunktsmetode: funksjon: {func} intervall: {a} -- {b}, steg: {N}")
    func = eval("lambda x: " + func)
    num_integral = 0
    delta_x = (b - a)/N
    for n in range(N):
        x = a + (n * delta_x)
        x_bar = func(x)
        print(n, x_bar)
        num_integral += round(x_bar, resolution)
    return num_integral * (delta_x)


def trapesmetode(func, a, b, N=10, resolution=6):
    print(f"Trapesmetoden: funksjon: {func} intervall: {a} -- {b}, steg: {N}")
    func = eval("lambda x: " + func)
    num_integral = 0
    delta_x = (b - a)/N
    for n in range(1, N+1):
        if n == 1 or n == N:
            koef = 1
        else:
            koef = 2
        x = a + (n * delta_x)
        x_bar = koef * func(x)
        print(n, x_bar)
        num_integral += round(x_bar, resolution)
    result = num_integral * (delta_x) / 2
    return round(result, resolution)


def simpsons(func, a, b, N=10, resolution=6):
    print(f"Simsons metode: funksjon: {func} intervall: {a} -- {b}, steg: {N}")
    func = eval("lambda x: " + func)
    num_integral = 0
    delta_x = (b - a)/N
    for n in range(1, N+1):
        if n == 1 or n == N:
            koef = 1
        elif n%2 == 0:
            koef = 4
        elif (n + 1)%2 == 0:
            koef = 2
        x = a + (n * delta_x)
        x_bar = koef * func(x)
        print(n, x_bar)
        num_integral += round(x_bar, resolution)
    result = num_integral * (delta_x) / 3
    return round(result, resolution)


############ Example running ###############
# Careful, eval() is dangerous. Do not use in production
func = "(x**3 - 1)**0.5"
print("result is", midtpkt(func, 1, 2, N=10), "\n")
print("result is", trapesmetode(func, 1, 2, N=10), "\n")
print("result is", simpsons(func, 1, 2, N=10), "\n")
