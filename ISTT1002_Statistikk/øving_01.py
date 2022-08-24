import numpy


#######################################################
OPPG = "1a"
DATA = [4, 10, 6, 8, 2, 5, 3, 9, 3, 3]

def gjennomsnitt(data):
    return sum(data) / len(data)

svar = gjennomsnitt(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "1b"
DATA = [4, 10, 6, 8, 2, 5, 3, 9, 3, 3]

def median(data):
    is_odd = len(data) % 2
    data.sort()
    if is_odd:
        return data[(len(data) // 2)]
    else:
        mid_low = data[(len(data) // 2) - 1]
        mid_hi = data[(len(data) // 2)]
        return (mid_low + mid_hi) / 2

svar = median(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "2a"
DATA = [5, 4, 5, 4, 6, 3, 2, 2, 2, 6]

def variasjonsbredde(data):
    data.sort()
    return max(data) - min(data)

svar = variasjonsbredde(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "2b"

def empirisk_standardavvik(data):
    mean = gjennomsnitt(data)
    numerator = sum([(x - mean)**2 for x in data])
    denominator = len(data)
    return (numerator / denominator) ** 0.5

svar = empirisk_standardavvik(DATA)
print(f"Oppg. {OPPG}): {svar}")
print(f"    numpy: {numpy.std(DATA)}")


#######################################################
OPPG = "3"
DATA = {1: 7, 2: 3, 3: 5, 4: 4, 5: 3, 6: 3}

def gjennomsnitt_av_diskrete_data(data):
    return sum([x*data[x] for x in data]) / sum([data[x] for x in data])

svar = gjennomsnitt_av_diskrete_data(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "4a"
DATA = [0.1109, 0.0951, 0.0862, 0.1015, 0.1579, 0.0981, 0.0925, 0.0987]

svar = gjennomsnitt(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "4b"
DATA = [0.1109, 0.0951, 0.0862, 0.1015, 0.1579, 0.0981, 0.0925, 0.0987]

svar = median(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "4c"
DATA = [0.1109, 0.0951, 0.0862, 0.1015, 0.0981, 0.0925, 0.0987]

svar = gjennomsnitt(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "4d"
DATA = [0.1109, 0.0951, 0.0862, 0.1015, 0.0981, 0.0925, 0.0987]

svar = median(DATA)
print(f"Oppg. {OPPG}): {svar}")


#######################################################
OPPG = "5"
DATA = [0.56, 0.21, 0.3, 0.44, 0.65, 0.21]

svar = empirisk_standardavvik(DATA) ** 2
print(f"Oppg. {OPPG}): {svar}")
print(f"   numpy: {numpy.std(DATA) ** 2}")


#######################################################
OPPG = "9"
DATA = {
    1: (2000, 9200),
    2: (2500, 10500),
    3: (3500, 13700),
    4: (3500, 13800),
    5: (4000, 15200),
    6: (4500, 17000),
    7: (5000, 19000),
    8: (5500, 20100),
    9: (6500, 23200),
    10: (7000, 25000)
    }

def minste_kvadratsums_metode(data):
    x_mean = gjennomsnitt([data[i][0] for i in data])
    y_mean = gjennomsnitt([data[i][1] for i in data])
    
    numerator = sum([(data[n][0] - x_mean) * (data[n][1] - y_mean) for n in data])
    denominator = sum([(data[n][0] - x_mean)**2 for n in data])
    b = numerator / denominator
    a = y_mean - b * x_mean
    return a, b

a, b = minste_kvadratsums_metode(DATA)
x = 6500
estimat = x * b + a
print(f"Oppg. {OPPG}): y = {b}x + {a}")
print(f"    y({x}) = {estimat}")
