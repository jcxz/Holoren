/**
 * Implementation of OpenCL helper functions
 */

#include "ocl.h"

#include <string>



/* a private namespace for helper functions */
namespace {

/**
 * A helper function to get platform type
 */
cl_int getPlatformType(cl_platform_id platform, OpenCL::ESelectionOpts *type)
{
  HOLOREN_ASSERT(type != NULL);

  std::string name;
  cl_int err = OpenCL::getPlatformInfo(platform, CL_PLATFORM_NAME, &name);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  if (name == "AMD Accelerated Parallel Processing")
  {
    *type = OpenCL::OPT_PLATFORM_PREFER_AMD;
  }
  else if (name == "Intel(R) OpenCL")
  {
    *type = OpenCL::OPT_PLATFORM_PREFER_INTEL;
  }
  else if (name == "NVIDIA CUDA")
  {
    *type = OpenCL::OPT_PLATFORM_PREFER_NVIDIA;
  }
  else
  {
    *type = OpenCL::OPT_PLATFORM_PREFER_OTHER;
  }

  return CL_SUCCESS;
}


/**
 * A helper function to get the device type
 */
cl_int getDeviceType(cl_device_id device, OpenCL::ESelectionOpts *type)
{
  HOLOREN_ASSERT(type != NULL);

  cl_device_type dev_type;
  cl_int err = OpenCL::getDeviceInfo(device, CL_DEVICE_TYPE, &dev_type);
  if (err != CL_SUCCESS)
  {
    return err;
  }
  
  if (dev_type == CL_DEVICE_TYPE_GPU)
  {
    *type = OpenCL::OPT_DEVICE_PREFER_GPU;
  }
  else if (dev_type == CL_DEVICE_TYPE_CPU)
  {
    *type = OpenCL::OPT_DEVICE_PREFER_CPU;
  }
  else
  {
    *type = OpenCL::OPT_DEVICE_PREFER_OTHER;
  }

  return CL_SUCCESS;
}

}  // End of private namespace with helper functions



/* OpenCL namespace */
namespace OpenCL {

/**
 */
cl_int getPlatformInfo(cl_platform_id platform, cl_platform_info info, std::string *ret)
{
#if 0
  HOLOREN_ASSERT(ret != NULL);

  /* get the size of platform info */
  size_t info_size = 0;
  cl_int err = clGetPlatformInfo(platform, info, 0, NULL, &info_size);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* allocate size for platform info */
  ret->resize(info_size);
  //ret->clear();
  //ret->reserve();

  /* get the actual information */
  return clGetPlatformInfo(platform, info, info_size, &ret->front(), NULL);
#endif
  HOLOREN_ASSERT(ret != NULL);

  /* get the size of platform info */
  size_t info_size = 0;
  cl_int err = clGetPlatformInfo(platform, info, 0, NULL, &info_size);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* allocate size for platform info */
  char *tmp = (char *) malloc(info_size);
  if (tmp == NULL)
  {
    return CL_SUCCESS;
  }
  //ret->clear();
  //ret->reserve();

  /* get the actual information */
  err = clGetPlatformInfo(platform, info, info_size, tmp, NULL);
  if (err !=  CL_SUCCESS)
  {
    free(tmp);
    return err;
  }

  *ret = tmp;

  free(tmp);

  return CL_SUCCESS;
}


/**
 */
cl_int getPlatforms(std::vector<cl_platform_id> *platforms)
{
  HOLOREN_ASSERT(platforms != NULL);

  /* query for the number of devices */
  cl_uint num_platforms = 0;
  cl_int err = clGetPlatformIDs(0, NULL, &num_platforms);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* resize the vector to have enough space for device id-s */
  platforms->resize(num_platforms);
  //devices->clear();
  //devices->reserve(num_devices);

  /* query for the devices ids */
  return clGetPlatformIDs(num_platforms, &platforms->front(), NULL);
}


/**
 */
cl_int getDeviceInfo(cl_device_id device, cl_device_info info, std::string *ret)
{
  HOLOREN_ASSERT(ret != NULL);

  /* get the size of platform info */
  size_t info_size = 0;
  cl_int err = clGetDeviceInfo(device, info, 0, NULL, &info_size);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* allocate size for platform info */
  ret->resize(info_size);
  //ret->clear();
  //ret->reserve();

  /* get the actual information */
  return clGetDeviceInfo(device, info, info_size, &ret->front(), NULL);
}


/**
 */
cl_int getDevices(cl_platform_id platform, cl_device_type type, std::vector<cl_device_id> *devices)
{
  HOLOREN_ASSERT(devices != NULL);

  /* query for the number of devices */
  cl_uint num_devices = 0;
  cl_int err = clGetDeviceIDs(platform, type, 0, NULL, &num_devices);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  /* resize the vector to have enough space for device id-s */
  devices->resize(num_devices);
  //devices->clear();
  //devices->reserve(num_devices);

  /* query for the devices ids */
  return clGetDeviceIDs(platform, type, num_devices, &devices->front(), NULL);
}


/**
 */
cl_device_id selectDevice(uint32_t *options, cl_int *cl_err)
{
  uint32_t opts = (options != NULL) ? (*options) : (OPT_PLATFORM_PREFER_ANY | OPT_DEVICE_PREFER_ANY);
  cl_int err = CL_SUCCESS;
  std::vector<cl_platform_id> platforms;
  std::vector<cl_platform_id>::iterator platforms_it;  // contains the selected platform
  std::vector<cl_device_id> devices;
  std::vector<cl_device_id>::iterator devices_it;  // contains the selected device

  /* enumerate all available platforms */
  err = getPlatforms(&platforms);
  if (err != CL_SUCCESS)
  {
    goto error;
  }

  /* go through all the enumerated platforms and select one according to options */
  for (platforms_it = platforms.begin(); platforms_it != platforms.end(); ++platforms_it)
  {      
    DBG(*platforms_it);

    ESelectionOpts type;
    err = getPlatformType(*platforms_it, &type);
    if (err != NULL)
    {
      continue;  // ignore the error in case there are more platforms
    }
    
    if (opts & type)
    {
      opts &= (~OPT_PLATFORM_PREFER_ANY);  /// clear all platforms
      opts |= type;                        /// set the platform found
      break;
    }
  }

  /* make sure that we have at least one platform */
  if (platforms_it == platforms.end())
  {
    goto error;
  }

  /* enumerate all devices available on platform */
  err = OpenCL::getDevices(*platforms_it, CL_DEVICE_TYPE_ALL, &devices);
  if (err != CL_SUCCESS)
  {
    goto error;
  }

  /* select the one that is the most suitable according to given options */
  for (devices_it = devices.begin(); devices_it != devices.end(); ++devices_it)
  {
    DBG(*devices_it);
    
    ESelectionOpts type;
    err = getDeviceType(*devices_it, &type);
    if (err != NULL)
    {
      continue;  // ignore the error in case there are more platforms
    }
    
    if (opts & type)
    {
      opts &= (~OPT_DEVICE_PREFER_ANY);  /// clear all platforms
      opts |= type;                      /// set the platform found
      break;
    }
  }

  /* make sure that at least one device has been found */
  if (devices_it == devices.end())
  {
    goto error;
  }
  
  if (options != NULL) *options = opts;
  if (cl_err != NULL) *cl_err = CL_SUCCESS;

  return *devices_it;

error:
    if (options != NULL) *options = 0;
    if (cl_err != NULL) *cl_err = err;

    return NULL;
}


/**
 */
cl_int getContextDevices(cl_context context, std::vector<cl_device_id> *ret)
{
    HOLOREN_ASSERT(ret != NULL);

  /* get the build log size */
  size_t device_num = 0;
  cl_int err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &device_num);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  if (device_num == 0)
  {
    ret->clear();  // reset the contents of output
    return CL_SUCCESS;
  }

  /* allocate memory for the string */
  ret->resize(device_num);
  //ret->clear();
  //ret->reserve(log_size);

  /* get the actual data */
  return clGetContextInfo(context, CL_CONTEXT_DEVICES, device_num, &ret->front(), NULL);
}


/**
 */
cl_int getBuildLog(cl_program program, cl_device_id device, std::string *ret)
{
  HOLOREN_ASSERT(ret != NULL);

  /* get the build log size */
  size_t log_size = 0;
  cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
  if (err != CL_SUCCESS)
  {
    return err;
  }

  if (log_size == 0)
  {
    ret->clear();  // reset the contents of output
    return CL_SUCCESS;
  }

  /* allocate memory for the string */
  ret->resize(log_size);
  //ret->clear();
  //ret->reserve(log_size);

  /* get the actual data */
  return clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, &ret->front(), NULL);
}


/**
 */
const char *clErrToStr(cl_int err)
{
  switch (err)
  {
    case CL_SUCCESS:                            return "Success!";
    case CL_DEVICE_NOT_FOUND:                   return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:               return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:             return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:                   return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:                 return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:       return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:                   return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:              return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:              return "Program build failure";
    case CL_MAP_FAILURE:                        return "Map failure";
    case CL_INVALID_VALUE:                      return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:                return "Invalid device type";
    case CL_INVALID_PLATFORM:                   return "Invalid platform";
    case CL_INVALID_DEVICE:                     return "Invalid device";
    case CL_INVALID_CONTEXT:                    return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:           return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:              return "Invalid command queue";
    case CL_INVALID_HOST_PTR:                   return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:                 return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:                 return "Invalid image size";
    case CL_INVALID_SAMPLER:                    return "Invalid sampler";
    case CL_INVALID_BINARY:                     return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:              return "Invalid build options";
    case CL_INVALID_PROGRAM:                    return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:         return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:                return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:          return "Invalid kernel definition";
    case CL_INVALID_KERNEL:                     return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:                  return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:                  return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:                   return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:                return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:             return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:            return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:             return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:              return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:            return "Invalid event wait list";
    case CL_INVALID_EVENT:                      return "Invalid event";
    case CL_INVALID_OPERATION:                  return "Invalid operation";
    case CL_INVALID_GL_OBJECT:                  return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:                return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:                  return "Invalid mip-map level";
  }

  return "Unknown OpenCL error";
}

} // End of OpenCL namespace


/* debug output fucntions */

/**
 */
std::ostream & operator<<(std::ostream & os, cl_platform_id platform)
{
  std::string info;

  /* name */
  OpenCL::getPlatformInfo(platform, CL_PLATFORM_NAME, &info);
  os << "Platform name       : \"" << info << "\"" << std::endl;

  /* vendor */
  OpenCL::getPlatformInfo(platform, CL_PLATFORM_VENDOR, &info);
  os << "Platform vendor     : \"" << info << "\"" << std::endl;

  /* version */
  OpenCL::getPlatformInfo(platform, CL_PLATFORM_VERSION, &info);
  os << "Platform version    : \"" << info << "\"" << std::endl;

  /* extensions */
  OpenCL::getPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, &info);
  os << "Platform extensions : \"" << info << "\"" << std::endl;

  return os;
}


/**
 */
std::ostream & operator<<(std::ostream & os, cl_device_id device)
{
  std::string str_info;
  cl_uint uint_info;

  /* name */
  OpenCL::getDeviceInfo(device, CL_DEVICE_NAME, &str_info);
  os << "Device name                   : \"" << str_info << "\"" << std::endl;

  /* vendor */
  OpenCL::getDeviceInfo(device, CL_DEVICE_VENDOR, &str_info);
  os << "Device vendor                 : \"" << str_info << "\"" << std::endl;

  /* vendor id */
  OpenCL::getDeviceInfo(device, CL_DEVICE_VENDOR_ID, &uint_info);
  os << "Device vendor id              : " << uint_info << std::endl;

  /* version */
  OpenCL::getDeviceInfo(device, CL_DEVICE_VERSION, &str_info);
  os << "Device version                : \"" << str_info << "\"" << std::endl;

  /* extensions */
  OpenCL::getDeviceInfo(device, CL_DEVICE_EXTENSIONS, &str_info);
  os << "Device extensions             : \"" << str_info << "\"" << std::endl;

  /* maximum number of compute units */
  OpenCL::getDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, &uint_info);
  os << "Device max compute units      : " << uint_info << std::endl;

  /* device's preferred vector width (in chars) */
  OpenCL::getDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, &uint_info);
  os << "Device preferred vector width : " << uint_info << " (in chars)" << std::endl;

  return os;
}
