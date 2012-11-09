/*
 * vector.c: some vector math functions
 * author: Kelly Mathews
 */

#include <math.h>

#include "vector.h"

// Returns the magnitude of the given vector
float getMagnitude(Vector3f vec)
{
  return fabs(sqrt((vec.x*vec.x)+(vec.y*vec.y)+(vec.z*vec.z)));
}

// Returns the give vector in normalized form
Vector3f normalize(Vector3f vec)
{
  float mag = getMagnitude(vec);
  Vector3f ret;
  ret.x=0;ret.y=0;ret.z=0;
  if(mag == 0) return ret;
  ret.x = vec.x/mag;
  ret.y = vec.y/mag;
  ret.z = vec.z/mag;
  return ret;
}
