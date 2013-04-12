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