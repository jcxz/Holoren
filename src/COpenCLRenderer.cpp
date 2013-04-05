/**
 */

#include "COpenCLRenderer.h"
#include "global.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>


/** Kernel file */
static const char *DEF_KERNEL = "holoren.cl";

/** Kernal function (a main entry point to OpenCL program) */
static const char *KERNEL_NAME = "compObjWave";



/**
 */
bool COpenCLRenderer::open(void)
{
  m_err = "";  // reset all previous errors

  /* get information about underlying platform */
  cl_platform_id platform;
  cl_int err = clGetPlatformIDs(1, &platform, NULL);
  if (err != CL_SUCCESS)
  {
    m_err = clErrToStr(err);
    return false;
  }

  /* get all GPU devices from the platform */
  std::vector<cl_device_id> devices;
  err = getDevices(platform, CL_DEVICE_TYPE_GPU, &devices);
  if (err == CL_SUCCESS)
  {
    m_err = clErrToStr(err);
    return false;
  }

  /* create OpenCL context */
  m_context = clCreateContext(NULL, devices.size(), devices.data(), NULL, NULL, &err);
  if (err != CL_SUCCESS)
  {
    m_err = clErrToStr(err);
    return false;
  }

  /* read the OpenCL source file */
  std::string program_buf;
  if (!readCLSource(DEF_KERNEL, &program_buf))
  {
    return false;
  }

  /* create program */
  const char *src_strings = program_buf.c_str();
  size_t src_sizes = program_buf.size();
  m_program = clCreateProgramWithSource(m_context, 1, &src_strings, &src_sizes, &err);
  if (err != CL_SUCCESS)
  {
    return false;
  }

  /* compile program */
  err = clBuildProgram(m_program,  // program
                       0,          // 0 as we are not providing any device list
                       NULL,       // if NULL, then the program is built for all devices, that the program is assocated with (so probably all devices from the context)
                       "",         // compilation options
                       NULL, 
                       NULL);
  if (err != CL_SUCCESS)
  {
    return false;
  }

  /* create kernels from program */
  m_kernel = clCreateKernel(m_program, KERNEL_NAME, &err);
  if (err != CL_SUCCESS)
  {
    return false;
  }

  /* create command queue */
  m_cmd_queue = clCreateCommandQueue(m_context, devices[0], NULL, &err);
  if (err != CL_SUCCESS)
  {
    return false;
  }

  return true;
}


/**
 */
bool COpenCLRenderer::close(void)
{
  clReleaseProgram(m_program);
  clReleaseContext(m_context);

  return true;
}


/**
 */
std::string COpenCLRenderer::getError(void) const
{
  return std::string(m_err);
}


/**
 */
bool COpenCLRenderer::renderObjectWave(const CPointCloud & pc, COpticalField *of)
{
  /* create memory objects from data passed in as arguments */

  /* set kernel arguments */

  /* execute kernel */

  /* read the result */

  return true;
}


/**
 */
bool COpenCLRenderer::renderHologram(const CPointCloud & pc, COpticalField *of)
{
  return true;
}


/**
 */
cl_int COpenCLRenderer::getDevices(cl_platform_id platform, cl_device_type type, std::vector<cl_device_id> *devices)
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

  /* query for the devices ids */
  return clGetDeviceIDs(platform, type, num_devices, &devices->front(), NULL);
}


/**
 */
bool COpenCLRenderer::readCLSource(const char *filename, std::string *program_buf)
{
  HOLOREN_ASSERT(program_buf != NULL);

  /* read the program kernel */
  FILE *f = fopen(filename, "rb");
  if (f == NULL)
  {
    m_err = "Failed to open kernel file";
    return false;
  }

  /* read the kernel size */
  struct stat st;

  if (fstat(fileno(f), &st) == -1)
  {
    fclose(f);
    m_err = "Failed to get the kernel size";
    return false;
  }

  DBG("Kernel size: " << st.st_size);

  /* allocate memory for kernel */
  program_buf->resize(st.st_size + 1);  // +1 for terminating 0

  /* read the kernel program and close the file */
  if (fread(&program_buf->front(), sizeof(char), st.st_size, f) != st.st_size)
  {
    fclose(f);
    m_err = "Failed to read kernel program";
    return false;
  }

  fclose(f);

  DBG("Kernel Program:\n" << *program_buf);

  return true;
}


/**
 * A function to translate the OpenCL error
 */
const char *COpenCLRenderer::clErrToStr(cl_int err)
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

  return "Unknown";
}