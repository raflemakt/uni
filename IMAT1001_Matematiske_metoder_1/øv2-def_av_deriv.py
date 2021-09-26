import numpy as np

A = np.zeros((5,5))
for i in range(5):
    for j in range(5):
        A[i, j] = (i + 1)**(4 - j)

b = np.array([2,0,1,0,0])

# Finner x_2
sol = np.linalg.solve(A, b)
print("x_2 = " + str(round(sol[1], 3)))

# Bytter ut kolonne 2 med b
A_2b = A
for row in range(5):
    for col in range(5):
        A_2b[row][1] = b[row]

detA_2b = np.linalg.det(A_2b)
print(A_2b)
print("detA_2(b) = " + str(round(detA_2b, 2)))
