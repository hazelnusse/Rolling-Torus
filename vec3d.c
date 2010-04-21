/*
 * =====================================================================================
 *
 *       Filename:  vec3d.c
 *
 *    Description:  Some basic functions for length 3 arrays of doubles
 *
 *        Version:  1.0
 *        Created:  04/20/2010 10:03:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <math.h>

void cross(double a[3], double b[3], double cp[3])
{
  cp[0] =  a[1]*b[2] - a[2]*b[1];
  cp[1] = -a[0]*b[2] + a[2]*b[0];
  cp[2] =  a[0]*b[1] - a[1]*b[0];
}

double dot(double a[3], double b[3])
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void normalize(double a[3])
{
  double mag = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
  a[0] /= mag;
  a[1] /= mag;
  a[2] /= mag;
}
