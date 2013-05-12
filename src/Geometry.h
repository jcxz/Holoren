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
   * An empty default constructor, so that the caller does not have
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


/**
 * A matrix for 3D transformations.
 * This class is based on project skeleton from IZG classes (Computer graphics basics)
 * at FIT VUT Brno created by Michal Spanel.
 */
struct SMatrix4D
{
  tFPType data[16];  /// elements of matrix
  /*
  union
  {
    struct
    {
      tFPType s11, s21, s31, s41;
      tFPType s12, s22, s32, s42;
      tFPType s13, s23, s33, s43;
      tFPType s14, s24, s34, s44;
    };
    tFPType data[16];
  };
  */

  /**
   * A function to get a given element of matrix
   */
  inline tFPType getElem(int row, int col)
  {
    return *(data + row * 4 + col);
  }

  /**
   * A function to get a given element of matrix
   */
  inline tFPType setElem(int row, int col, tFPType elem)
  {
    return *(data + row * 4 + col) = elem;
  }

  /**
   * Initializes matrix to a unit matrix
   */
  void loadIdentity(void);

  /**
   * Multiplies this matrix with the other matrix, i.e. performs other * this
   *
   * @param other the other matrix that this will be multiplied with
   */
  void multMatrix(const SMatrix4D & other);

  /**
   * Multiplies this matrix by a given translation matrix
   *
   * @param dx x-axis translation
   * @param dy y-axis translation
   * @param dz z-axis translation
   */
  void translate(tFPType dx, tFPType dy, tFPType dz);

  /**
   * Rotation about x-axis. The transformation uses right-handed coordinates like OpenGL.
   *
   * @param angle angle of rotation in radians
   */
  void rotateX(tFPType angle);

  /**
   * Rotation about y-axis. The transformation uses right-handed coordinates like OpenGL.
   *
   * @param angle angle of rotation in radians
   */
  void rotateY(tFPType angle);

  /**
   * Rotation about z-axis. The transformation uses right-handed coordinates like OpenGL.
   *
   * @param angle angle of rotation in radians
   */
  void rotateZ(tFPType angle);

  /**
   * Multiplies this matrix with a given scale
   *
   * @param dx scale in x direction
   * @param dy scale in y direction
   * @param dz scale in z direction
   */
  void scale(tFPType dx, tFPType dy, tFPType dz);

  /* Nastavi projekcni matici, ktera zaridi PERSPEKTIVNI promitani.
   * d > 0 - vzdalenost kamery na z-tove ose
   * width, height - velikost aktualniho okna (frame bufferu) */
  //void trProjectionPerspective(double d, int width, int height);

  /**
   * Transforms a given point in 3D by this matrix
   *
   * @param the point to be transformed
   *
   * @return transformed point
   */
  SPoint3D transformVertex(const SPoint3D & p);

  /* Transformuje vektor ve 3D pomoci matice model */
  //SPoint3D transformVector(const SPoint3D & v);

  /* Promitne bod ve 3D na obrazovku pomoci projection matice
   * p - souradnice ve 3D
   * u,v -  souradnice na obrazovce */
  //void trProjectVertex(int *u, int *v, const S_Coords *p);
};

} // End of namespace geometry

#endif
