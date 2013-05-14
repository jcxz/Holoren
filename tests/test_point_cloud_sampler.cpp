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
#include "CPointCloudRWFactory.h"
#include "CBasePointCloudRW.h"
#include "CPointCloud.h"

#include <iostream>



int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing filename" << std::endl;
    return 1;
  }

  CBasePointCloudRW *rw = g_pcrw_factory.getReaderWriter(argv[1]);
  if (rw == NULL)
  {
    std::cerr << "The conversion from given file type to point cloud is not supported" << std::endl;
    return 1;
  }

  /* set sampling step */
  rw->setSamplingStep(10e-2);

  /* convert to point cloud from a given file type */
  CPointCloud pc;
  if (!rw->read(&pc))
  {
    std::cerr << "Failed to conver the file "
              << argv[1]
              << "the point cloud: "
              << rw->getErrorMsg()
              << std::endl;
    delete rw;
    return 1;
  }

  /* display the sampled points */
  std::cout << "Sampled points (step == " << rw->getSamplingStep() << "): " << std::endl;
  std::cout << pc << std::endl;

  delete rw;

  return 0;
}
