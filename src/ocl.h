/**
 * This file contains some convenience helper functions
 * for working with OpenCL
 */

#ifndef OCL_H
#define OCL_H

#include "global.h"

#include <CL/cl.h>
#include <vector>
#include <ostream>


namespace OpenCL {


/**
 * A function to get the platform information (all platform information come as strings)
 */
cl_int getPlatformInfo(cl_platform_id platform, cl_platform_info info, std::string *ret);


/**
 * A function to enumerate all available platforms
 */
cl_int getPlatforms(std::vector<cl_platform_id> *platforms);


/**
 * A function to get the device information without allocating them
 */
template<typename T>
inline cl_int getDeviceInfo(cl_device_id device, cl_device_info info, T *ret)
{
  HOLOREN_ASSERT(ret != NULL);
  return clGetDeviceInfo(device, info, sizeof(T), (void *) ret, NULL);
}


/**
 * A function to get the device information, whose type is an array of elements
 */
template<typename T>
cl_int getDeviceInfo(cl_device_id device, cl_device_info info, std::vector<T> *ret)
{
  HOLOREN_ASSERT(ret != NULL);

  /* get the size of device info */
  size_t info_size = 0;
  cl_int err = clGetDeviceInfo(device, info, 0, NULL, &info_size);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* allocate enough memory */
  ret->resize(info_size / sizeof(T));

  /* get the actual information */
  return clGetDeviceInfo(device, info, info_size, &ret->front(), NULL);
}


/**
 * A function to get the device string information
 */
cl_int getDeviceInfo(cl_device_id device, cl_device_info info, std::string *ret);


/**
 * A function to get all devices of a given type
 *
 * @param platform a platform for which the devices should be retrieved
 * @param type the type of devices to be queried for
 * @param devices a pointer to a vector where the resulting device id-s will be stored
 *
 * @return an OpenCL error code
 */
cl_int getDevices(cl_platform_id platform, cl_device_type type, std::vector<cl_device_id> *devices);


/**
 * An enumeration of options that can be specified when selecting suitable device
 */
enum ESelectionOpts
{
  OPT_PLATFORM_PREFER_INTEL  = 0x00000001,
  OPT_PLATFORM_PREFER_AMD    = 0x00000002,
  OPT_PLATFORM_PREFER_NVIDIA = 0x00000004,
  OPT_PLATFORM_PREFER_OTHER  = 0x00000008,
  OPT_PLATFORM_PREFER_ANY    = OPT_PLATFORM_PREFER_INTEL | OPT_PLATFORM_PREFER_AMD | OPT_PLATFORM_PREFER_NVIDIA | OPT_PLATFORM_PREFER_OTHER,  /// the user does not care about underlying platform
  OPT_DEVICE_PREFER_GPU      = 0x00000010,
  OPT_DEVICE_PREFER_CPU      = 0x00000020,
  OPT_DEVICE_PREFER_OTHER    = 0x00000040,
  OPT_DEVICE_PREFER_ANY      = OPT_DEVICE_PREFER_GPU | OPT_DEVICE_PREFER_CPU | OPT_DEVICE_PREFER_OTHER            /// the user does not care about device type
};


/**
 * A function to select the most appropriate of all available devices
 *
 * @param options a pointer to variable with selection flags. Passing NULL to this
 *                parameter equals passing (OPT_PLATFORM_PREFER_ANY | OPT_DEVICE_PREFER_ANY).
 *                If the value of opts is any other than NULL, then upon function return,
 *                this variable will contain ored combination of deviceas and platforms that
 *                were choosen. So for exemple if an AMD GPU was selected, then the upon function
 *                return variable pointed to by opts will contain value (OPT_PLATFORM_PREFER_AMD | OPT_DEVICE_PREFER_GPU)
 * @param cl_err if this parameter is other than NULL, then upon function return it will contain
 *               an opencl error, that occured during its operation. When NULL is returned from this
 *               function and cl_err is CL_SUCCESS, then the given combination of options could not
 *               be satisfied.
 *
 * @return the cl_device_id handle of the device that has been selected, or NULL on error.
 *                          Note that NULL is returned in both cases, when an OpenCL error occures and
 *                          when the given combination of selection options can not be satisfied.
 */
cl_device_id selectDevice(uint32_t *options = NULL, cl_int *cl_err = NULL);


/**
 * This function will return all devices associated with the given context
 */
cl_int getContextDevices(cl_context context, std::vector<cl_device_id> *ret);


/**
 * A function to retrieve build log
 *
 * @param program OpenCL program object
 * @param device OpenCL device object
 * @param ret a pointer to string, where the build log will be saved
 *
 * @return an OpenCL error code
 */
cl_int getBuildLog(cl_program program, cl_device_id device, std::string *ret);


/**
 * A function to convert an error to an error string
 */
const char *clErrToStr(cl_int err);


} // End of OpenCL namespace


/* Note that these operator<< functions have to be outside of the namespace,
   because ADL does not search for primitive types like pointers which cl_platform_id and others are */

/**
 * A debugging function to print the most important information about the platform
 */
std::ostream & operator<<(std::ostream & os, cl_platform_id platform);

/**
 * A debugging function to print the most important informatio about the device
 */
std::ostream & operator<<(std::ostream & os, cl_device_id device);

#endif
