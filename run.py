from numpy import zeros, array

from _torus import TorusEOMS

f = zeros(8, dtype="double")
x = array([0, 0, 0, 0, 0, 0, 0, 0], dtype="double")
params = array([9.8, 1, 0.5], dtype="double")

print f
result = TorusEOMS(0, x, f, params)
print f
print result
