#include <gsl/gsl_errno.h>
// torus.h
// Defines equations of motion for a rolling torus of uniform density, major
// radius of r1, minor radius r2, in a gravitational field g.
// params should be a length 3 double array with [g, r1, r2] as its entries.
int TorusEOMS(double t, const double x[8], double f[8], void *params);
