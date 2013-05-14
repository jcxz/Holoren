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

#include "ocl.h"

#include <iostream>


void runTestCase(uint32_t *options)
{
  cl_int err = CL_SUCCESS;
  cl_device_id device = OpenCL::selectDevice(options, &err);
  if (device == NULL)
  {
    if (err != CL_SUCCESS)
    {
      std::cerr << "An OpenCL error occured: " << OpenCL::clErrToStr(err) << std::endl;
    }
    else
    {
      std::cerr << "Couldn't match specified options" << std::endl;
    }

    return;
  }

  std::cout << "====================== SELECTED ============================" << std::endl;
  std::cout << device << std::endl;

  return;
}



int main(void)
{
  std::cout << "============================ selectDevice(NULL) ===============================" << std::endl;
  runTestCase(NULL);

  #define TEST(opt) \
    { \
      std::cout << "============================ selectDevice(" #opt ") ===============================" << std::endl; \
      uint32_t __tmp = (opt); \
      runTestCase(&__tmp); \
    }

  TEST(OpenCL::OPT_PLATFORM_PREFER_INTEL);
  TEST(OpenCL::OPT_PLATFORM_PREFER_INTEL | OpenCL::OPT_DEVICE_PREFER_ANY);
  TEST(OpenCL::OPT_PLATFORM_PREFER_ANY | OpenCL::OPT_DEVICE_PREFER_GPU);
  TEST(OpenCL::OPT_PLATFORM_PREFER_ANY | OpenCL::OPT_DEVICE_PREFER_CPU);

  #undef TEST

  return 0;
}
