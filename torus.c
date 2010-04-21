#include <math.h>
#include "torus.h"

int TorusEOMS(double t, const double x[8], double f[8], void *params)
{
  double *p = (double *) params, _M00, _M11, _M12, _M22, _rhs0, _rhs1, _rhs2;
  // p = [g, r1, r2]

  _M00 = -2*p[1]*p[2]*cos(x[1]) - 13*pow(p[2], 2)/8 - 3*pow(p[1], 2)/2;
  _M11 = -2*p[1]*p[2]*cos(x[1]) - 2*pow(p[1], 2) - 3*pow(p[2], 2)/4 - pow(p[2], 2)*pow(cos(x[1]), 2);
  _M12 = p[1]*p[2]*sin(x[1]) + pow(p[2], 2)*cos(x[1])*sin(x[1]);
  _M22 = -5*pow(p[2], 2)/8 - pow(p[1], 2)/2 - pow(p[2], 2)*pow(sin(x[1]), 2);
  _rhs0 = -p[0]*p[1]*sin(x[1]) - 2*pow(p[1], 2)*x[6]*x[7] - 3*pow(p[2], 2)*x[6]*x[7]/4 + p[1]*p[2]*pow(x[7], 2)*sin(x[1]) + pow(p[2], 2)*pow(x[7], 2)*cos(x[1])*sin(x[1]) - p[1]*p[2]*pow(x[5], 2)*sin(x[1]) - pow(p[2], 2)*pow(cos(x[1]), 2)*x[6]*x[7] - 2*p[1]*p[2]*cos(x[1])*x[6]*x[7];
  _rhs1 = pow(p[1], 2)*x[5]*x[7] + pow(p[2], 2)*x[5]*x[7] - pow(p[2], 2)*pow(cos(x[1]), 2)*x[5]*x[7] + p[1]*p[2]*x[5]*x[7]/cos(x[1]) - p[1]*p[2]*sin(x[1])*x[5]*x[6] - pow(p[2], 2)*cos(x[1])*sin(x[1])*x[5]*x[6];
  _rhs2 = pow(p[1], 2)*x[5]*x[6] + 3*pow(p[2], 2)*x[5]*x[6]/4 + pow(p[2], 2)*pow(sin(x[1]), 2)*x[5]*x[6] + pow(p[2], 2)*cos(x[1])*sin(x[1])*x[5]*x[7] - p[1]*p[2]*sin(x[1])*x[5]*x[7] - pow(p[2], 2)*sin(x[1])*x[5]*x[7]/cos(x[1]);

  f[0] = x[7]/cos(x[1]);
  f[1] = x[5];
  f[2] = -tan(x[1])*x[7] + x[6];
  f[3] = (-p[1]*cos(x[0]) - p[2]*cos(x[0])*cos(x[1]))*f[2] - p[2]*f[1]*sin(x[0]);
  f[4] = (-p[1]*sin(x[0]) - p[2]*cos(x[1])*sin(x[0]))*f[2] + p[2]*f[1]*cos(x[0]);
  f[5] = _rhs0/_M00;
  f[6] = (_M22*_rhs1 - _M12*_rhs2)/(_M11*_M22 - pow(_M12, 2));
  f[7] = (_M11*_rhs2 - _M12*_rhs1)/(_M11*_M22 - pow(_M12, 2));

  return GSL_SUCCESS;
}
