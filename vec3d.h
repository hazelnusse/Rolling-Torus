/*
 * =====================================================================================
 *
 *       Filename:  vec3d.h
 *
 *    Description:  Some basic functions for length 3 arrays of doubles
 *
 *        Version:  1.0
 *        Created:  04/20/2010 09:59:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

void cross3d(double a[3], double b[3], double cp[3]);

double dot(double a[3], double b[3]);

void normalize(double a[3]);
