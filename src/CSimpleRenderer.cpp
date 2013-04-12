/**
 */

#include "global.h"
#include "CSimpleRenderer.h"
#include "COpticalField.h"
#include "CPointCloud.h"
#include "Geometry.h"

#include <cmath>

#ifdef DEBUG
# include <cstdio>
#endif

#ifndef M_PI
# define M_PI 3.1415926535897932384626433832795f
#endif



/**
 */
bool CSimpleRenderer::open(const char *filename)
{
  HOLOREN_UNUSED(filename);
  return true;
}


/**
 */
bool CSimpleRenderer::close(void)
{
  return true;
}


/**
 */
std::string CSimpleRenderer::getError(void) const
{
  return std::string();
}


/**
 */
bool CSimpleRenderer::renderObjectWave(const CPointCloud & pc, COpticalField *of)
{
  HOLOREN_ASSERT(of != 0);

  /* set up hologram rendering */
  of->zero();

  int rows = of->getNumRows();
  int cols = of->getNumCols();
  double k = (2 * M_PI) / of->getWaveLength();    /// wave number
  double pitch = of->getPitch();
  double corner_x = -(cols - 1) * pitch / 2;
  double corner_y = -(rows - 1) * pitch / 2;
  
  DBG("rows       : " << rows);
  DBG("cols       : " << cols);
  DBG("hologram_z : " << m_hologram_z);
  DBG("k          : " << k);
  DBG("sampling   : " << pitch);
  DBG("size_x     : " << (cols - 1) * pitch);
  DBG("size_y     : " << (rows - 1) * pitch);
  DBG("corner_x   : " << corner_x);
  DBG("corner_y   : " << corner_y);

  /* start rendering */
  for (unsigned int i = 0; i < pc.size(); ++i)
  {
    const SPointSource & ps = pc[i];
    //DBG("ps == " << ps);

    for (int row = 0; row < rows; ++row)
    {
      for (int col = 0; col < cols; ++col)
      {
        double x = (((col - 1) * pitch) + corner_x) - ps.x;   /// tu uz rovno odcitavam aj point source (nerobim to pri vypocte exp)
        double y = (((row - 1) * pitch) + corner_y) - ps.y;
        double z = m_hologram_z - ps.z;
        double r = std::sqrt(x * x + y * y + z * z);

        of->getSample(row, col) += std::exp(COpticalField::CComplex(0, k * r));
      }
    }
  }

  return true;
}


/**
 */
bool CSimpleRenderer::renderHologram(const CPointCloud & pc, COpticalField *of)
{
  HOLOREN_UNUSED(pc);
  HOLOREN_UNUSED(of);
  return true;
}



#if 0
/**
 */
void CRenderer::render(const CPointCloud<float> & cloud, COpticalField<float> *of)
{
  assert(of != 0);

  of->zero();

  int rows = of->getNumRows();
  int cols = of->getNumCols();
  double k = (2 * M_PI) / of->getWaveLength();    /// wave number
  double pitch = of->getPitch();
  double corner_x = -(cols - 1) * pitch / 2;
  double corner_y = -(rows - 1) * pitch / 2;

/*
  DBG("k            : " << k);
  DBG("pitch        : " << pitch);
  DBG("cols (res_x) : " << cols);
  DBG("rows (res_y) : " << rows);
  DBG("hologram_z   : " << m_hologram_z);
  DBG("corner_x     : " << corner_x);
  DBG("corner_y     : " << corner_y);
*/

#ifdef DEBUG
  fprintf(stderr, "k            : %f\n", k);
  fprintf(stderr, "pitch        : %f\n", pitch);
  fprintf(stderr, "cols (res_x) : %d\n", cols);
  fprintf(stderr, "rows (res_y) : %d\n", rows);
  fprintf(stderr, "hologram_z   : %f\n", m_hologram_z);
  fprintf(stderr, "corner_x     : %f\n", corner_x);
  fprintf(stderr, "corner_y     : %f\n", corner_y);
#endif

  for (unsigned int i = 0; i < cloud.size(); ++i)
  {
    DBG("point source i: " << i);

    const SVector3D<float> & ps = cloud.getPointSource(i);

    for (int row = 0; row < rows; ++row)
    {
      for (int col = 0; col < cols; ++col)
      {
        double x = ((col - 1) * pitch) + corner_x - ps.x;   /// tu uz rovno odcitavam aj point source (nerobim to pri vypocte exp)
        double y = ((row - 1) * pitch) + corner_y - ps.y;
        double z = m_hologram_z - ps.z;
        double r = std::sqrt(x * x + y * y + z * z);

        //DBG("r : " << r);

        //of->setSample(row, col, COpticalField<float>::CComplex(std::exp(k * r)));
        //of->setSample(row, col, COpticalField<float>::CComplex(std::cos(k * r)));

        //of->getSample(row, col) += COpticalField<float>::CComplex(std::exp(k * r));
        //of->getSample(row, col) += COpticalField<float>::CComplex(std::cos(k * r));

        of->getSample(row, col) += std::exp(COpticalField<float>::CComplex(0, k * r));
      }
      //DBG("\rSample[" << row << ", " << col << "]");
      //DBG("\rSample row: " << row);
    }
  }

  return;
}
#endif
