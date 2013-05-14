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

#include "COpticalField.h"

#include <iostream>


int main(void)
{
  std::cout << "* Creating optical field n. 1" << std::endl;
  COpticalField of1(200, 200, 0.0f, 0.0f);

  std::cout << "* Setting the last sample of optical field" << std::endl;
  of1.getSample(199, 199) = 20;

  std::cout << "* Creating optical field n. 2" << std::endl;
  COpticalField of2(of1);

  std::cout << "* Accessing the last sample of optical field" << std::endl;
  std::cout << of2.getSample(199, 199) << std::endl;

  std::cout << "* Clearing the optical field" << std::endl;
  of2.clear();

  std::cout << "* Creating optical field number 3" << std::endl;
  COpticalField of3;

  std::cout << "* Copying optical field number 2 to optical field number 3" << std::endl;
  of3 = of2;

  std::cout << "* Reseting optical field number 3" << std::endl;
  of3.reset(1, 1);

  std::cout << "* Copying optical field number 1 to optical field number 3" << std::endl;
  of3 = of1;

  std::cout << "* Zero-ing optical field number 3" << std::endl;
  of3.zero();

  std::cout << "* Saving optical field number 3" << std::endl;
  if (!of3.save("out.df"))
  {
    std::cerr << "Failed to save optical field" << std::endl;
    return 1;
  }

  std::cout << "* Creating optical field n. 4" << std::endl;
  COpticalField of4(200, 200, 0.0f, 0.0f);

  std::cout << "* Optical field 4 information: " << std::endl;
  std::cout << "   rows   : " << of4.getNumRows() << std::endl;
  std::cout << "   cols   : " << of4.getNumCols() << std::endl;
  std::cout << "   lambda : " << of4.getWaveLength() << std::endl;
  std::cout << "   pitch  : " << of4.getPitch() << std::endl;

  std::cout << "* of4.getSample(1, 2) == " << of4.getSample(1, 2) << std::endl;

  std::cout << "* Testing self assignment on optical field n. 4" << std::endl;
  of4 = of4;

  /* to test copying (this should trigger assignment operator
     on COpticalfieldPrivate) */
  //of4.getSample(1, 2) = 1.2f;

  std::cout << "* Setting sample in optical field number 4" << std::endl;
  of4.setSample(1, 2, 1.2f);

  return 0;
}
