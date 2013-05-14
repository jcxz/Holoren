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

#include "CPCPointCloudRW.h"
#include "CPointCloud.h"

#include <iostream>



int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: input_file output_file" << std::endl;
    return 1;
  }

  CPointCloud pc;
  CPCPointCloudRW rw(argv[1]);

  if (!rw.read(&pc))
  {
    std::cerr << "Failed to read point cloud file. Read sofar:" << std::endl;
    std::cout << pc << std::endl;
    return 1;
  }

  std::cout << "Resulting point cloud:" << std::endl;
  std::cout << pc << std::endl;

  rw.setFileName(argv[2]);

  if (!rw.write(&pc))
  {
    std::cerr << "Failed to write to point cloud file" << std::endl;
    return 1;
  }

  return 0;
}
