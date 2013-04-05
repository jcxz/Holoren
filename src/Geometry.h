/**
 * This file defines classes for various geometric shapes
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "config.h"

#include <ostream>


namespace Geometry {

/**
 * A basic simple 3D point
 */
struct SPoint3D
{
  tFPType x;
  tFPType y;
  tFPType z;

  /**
   * An empty default constructor, so that the claller does not have
   * to pay for initilisation, when he does not use it
   */
  SPoint3D(void)
  {
  }

  SPoint3D(tFPType x, tFPType y, tFPType z)
    : x(x), y(y), z(z)
  {
  }

  friend std::ostream & operator<<(std::ostream & os, const SPoint3D & pt)
  {
    return os << "SPoint3D(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
  }
};


/**
 * A line of two points
 */
struct SLine
{
  SPoint3D p1;
  SPoint3D p2;

  SLine(void)
  {
  }

  SLine(SPoint3D p1, SPoint3D p2)
    : p1(p1), p2(p2)
  {
  }

  friend std::ostream & operator<<(std::ostream & os, const SLine & l)
  {
    return os << "SLine(" << l.p1 << ", " << l.p2 << ")";
  }
};

} // End of namespace geometry

#endif
