from numpy import zeros, array

from _torus import TorusEOMS

x = array([0, 0, 0, 0, 0, 0, 0, 0], dtype="double")
params = array([9.8, 1, 0.5], dtype="double")

result = TorusEOMS(0, x, params)
print result
