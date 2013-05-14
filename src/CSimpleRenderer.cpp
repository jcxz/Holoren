/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#ifdef HOLOREN_DEBUG_KERNEL
        float re = std::cos((float) (k * r));
        float im = std::sin((float) (k * r));
        COpticalField::CComplex sample(of->getSample(row, col));
        sample += COpticalField::CComplex(re, im);
        of->setSample(row, col, sample);
#else
        of->getSample(row, col) += std::exp(COpticalField::CComplex(0, k * r));
#endif
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
