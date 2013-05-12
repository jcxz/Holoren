/**
 * Implementation of geometry shapes and matrix transformations.
 * The code for matrix transformations is based on project skeleton from IZG classes
 * (Computer graphics basics) at FIT VUT Brno created by Michal Spanel.
 */

#include "config.h"
#include "Geometry.h"

#include <ostream>
#include <cmath>


/* Aktualni velikost okna */
//int frame_height = 0, frame_width = 0;

namespace Geometry {

/**
 */
void SMatrix4D::loadIdentity(void)
{
  for (int j = 0; j < 4; ++j)
  {
    for (int i = 0; i < 4; ++i)
    {
      data[j * 4 + i] = (i == j) ? 1.0 : 0.0;
    }
  }

  return;
}


/**
 */
void SMatrix4D::multMatrix(const SMatrix4D & other)
{
  SMatrix4D tmp;

  for (int j = 0; j < 4; ++j)
  {
    for (int i = 0; i < 4; ++i)
    {
      tmp.data[j * 4 + i] = 0.0;
      for (int k = 0; k < 4; ++k)
      {
        // result           += (row in the first matrix) * (column in the second matrix)
        tmp.data[j * 4 + i] += other.data[j * 4 + k] * data[k * 4 + i];
      }
    }
  }

  *this = tmp;  // and copy the multiplication result back

  return;
}


/**
 */
void SMatrix4D::translate(tFPType dx, tFPType dy, tFPType dz)
{
  SMatrix4D m;

  m.loadIdentity();

  m.setElem(3, 0, dx);
  m.setElem(3, 1, dy);
  m.setElem(3, 2, dz);

  multMatrix(m);

  return;
}


/**
 */
void SMatrix4D::rotateX(tFPType angle)
{
  SMatrix4D m;
  double sina, cosa;

  sina = sin(angle);
  cosa = cos(angle);

  m.loadIdentity();
  m.setElem(1, 1, cosa);
  m.setElem(1, 2, sina);
  m.setElem(2, 1, -sina);
  m.setElem(2, 2, cosa);

  multMatrix(m);

  return;
}


/**
 */
void SMatrix4D::rotateY(tFPType angle)
{
  SMatrix4D m;
  double sina, cosa;

  sina = sin(angle);
  cosa = cos(angle);

  m.loadIdentity();
  m.setElem(0, 0, cosa);
  m.setElem(0, 2, -sina);
  m.setElem(2, 0, sina);
  m.setElem(2, 2, cosa);

  multMatrix(m);

  return;
}


/**
 */
void SMatrix4D::rotateZ(tFPType angle)
{
  SMatrix4D m;
  double sina, cosa;

  sina = sin(angle);
  cosa = cos(angle);

  m.loadIdentity();
  m.setElem(0, 0, cosa);
  m.setElem(0, 1, sina);
  m.setElem(1, 0, -sina);
  m.setElem(1, 1, cosa);

  multMatrix(m);

  return;
}


/**
 */
void SMatrix4D::scale(tFPType sx, tFPType sy, tFPType sz)
{
  SMatrix4D m;

  m.loadIdentity();
  m.setElem(0, 0, sx);
  m.setElem(1, 1, sy);
  m.setElem(2, 2, sz);

  multMatrix(m);

  return;
}


/**
 */
#if 0
void trProjectionPerspective(double d, int width, int height)
{
    IZG_ASSERT(d > 0);

    matUnit(&projection);
    matGet(&projection, 2, 2) = 0.0;
    matGet(&projection, 2, 3) = 1.0 / d;

    /* schovame si velikost okna... */
    frame_width = width;
    frame_height = height;
}
#endif


/**
 */
SPoint3D SMatrix4D::transformVertex(const SPoint3D & p)
{
  SPoint3D res;

  /* multiply vector (x,y,z,1) by this transformation matrix */
  res.x = p.x * getElem(0, 0) + p.y * getElem(1, 0) + p.z * getElem(2, 0) + getElem(3, 0);
  res.y = p.x * getElem(0, 1) + p.y * getElem(1, 1) + p.z * getElem(2, 1) + getElem(3, 1);
  res.z = p.x * getElem(0, 2) + p.y * getElem(1, 2) + p.z * getElem(2, 2) + getElem(3, 2);

  return res;
}


/**
 */
#if 0
SVector3D transformVector(const SVector3D & v);
{
  SVector3D res;

    /* vynasobeni vektoru (x,y,z,0) transformacni matici */
    v2->x = v->x * matGet(&model, 0, 0) + v->y * matGet(&model, 1, 0) + v->z * matGet(&model, 2, 0);
    v2->y = v->x * matGet(&model, 0, 1) + v->y * matGet(&model, 1, 1) + v->z * matGet(&model, 2, 1);
    v2->z = v->x * matGet(&model, 0, 2) + v->y * matGet(&model, 1, 2) + v->z * matGet(&model, 2, 2);

  return res;
}
#endif


/**
 */
#if 0
void trProjectVertex(int *u, int *v, const S_Coords *p)
{
    double xx, yy, ww, iww;

    IZG_ASSERT(u && v && p);

    /* vynasobeni vektoru (x,y,z,1) projekcni matici */
    xx = p->x;
    yy = p->y;
    ww = p->x * matGet(&projection, 0, 3) + p->y * matGet(&projection, 1, 3)
       + p->z * matGet(&projection, 2, 3) + matGet(&projection, 3, 3);

    /* homogenni souradnice */
    iww = 1.0 / ww;
    xx *= iww;
    yy *= iww;

    /* zaokrouhleni promitnutych souradnic */
    *u = (int)(xx + (frame_width >> 1));
    *v = (int)(yy + (frame_height >> 1));
}
#endif

}  // End of namespace Geometry