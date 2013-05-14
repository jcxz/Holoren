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

#include "config.h"
#include "global.h"
#include "CBasePointCloudRW.h"
#include "CPointCloud.h"



/**
 */
void CBasePointCloudRW::sample(const Geometry::SLine & line, CPointCloud *pc)
{
  /* compute direction vector of the line */
  Geometry::SPoint3D dir(line.p2.x - line.p1.x,
                         line.p2.y - line.p1.y,
                         line.p2.z - line.p1.z);
  double t = 0.0;

  /* sample the line with a given fixed step */
  while (t < 1.0f)
  {
    //DBG("t == " << t);
    pc->addPointSource(Geometry::SPoint3D(line.p1.x + dir.x * t,
                                          line.p1.y + dir.y * t,
                                          line.p1.z + dir.z * t));
    t += m_sampling_step;
  }

  /* this line is to compensate for lack of accuracy in floating point
     numbers and to make sure that the end point is allways included */
  //pc->addPointSource(Geometry::SPoint3D(line.p2.x, line.p2.y, line.p2.z));

  return;
}
