/**
 */

#include "global.h"
#include "COpenCLRenderer.h"
#include "CPointCloud.h"
#include "COpticalField.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <fstream>
#include <cmath>

#ifndef M_PI
# define M_PI 3.1415926535897932384626433832795f
#endif



/** Kernel file */
static const char *DEF_CL_SOURCE = "../../src/OpenCL/holoren.cl";

/** Kernel function (a main entry point to OpenCL program) */
static const char *KERNEL_NAME = "compObjWave";



/**
 */
bool COpenCLRenderer::open(const char *filename)
{
  /* make sure all is reset before initializing something */
  std::string program_buf;
  cl_int err = CL_SUCCESS;
//#ifdef HOLOREN_DEBUG_KERNEL
//  uint32_t flags = (OpenCL::OPT_DEVICE_PREFER_ANY | OpenCL::OPT_PLATFORM_PREFER_INTEL);
//#else
  uint32_t flags = (OpenCL::OPT_DEVICE_PREFER_ANY | OpenCL::OPT_PLATFORM_PREFER_ANY);
//#endif
  m_err_msg = "";

  /* select the most suitable device */
  //m_device = selectDevice();
  m_device = OpenCL::selectDevice(&flags, &err);
  if (m_device == NULL)
  {
    m_err_msg = (err != CL_SUCCESS) ? OpenCL::clErrToStr(err) : ("The given combination of devices and platforms could not be matched");
    goto error;
  }

  /* create OpenCL context for all GPU devices on the machine */
  m_context = clCreateContext(NULL, 1, &m_device, NULL, NULL, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto error;
  }

  /* read the OpenCL source file */
  if (!readCLSource(((filename == NULL) ? (DEF_CL_SOURCE) : (filename)), &program_buf))
  {
    goto error;
  }

  /* create program object */
  {
    const char *src_strings = program_buf.c_str();
    size_t src_sizes = program_buf.size();
    m_program = clCreateProgramWithSource(m_context, 1, &src_strings, &src_sizes, &err);
    if (err != CL_SUCCESS)
    {
      m_err_msg = OpenCL::clErrToStr(err);
      goto error;
    }
  }

  /* build program */
  {
#ifdef HOLOREN_DEBUG_KERNEL
    std::string options("-g");

    /* enable debugging options on intel platform */
    if (flags & OpenCL::OPT_PLATFORM_PREFER_INTEL)
    {
      options += "-s \"";
      options += filename;
      options += '\"';
    }
#endif

    err = clBuildProgram(m_program,        // program
                         0,                // 0 as we are not providing any device list
                         NULL,             // if NULL, then the program is built for all devices,
                                           // that the program is assocated with
                                           // (so probably all devices from the context)
#ifdef HOLOREN_DEBUG_KERNEL
                         options.c_str(),  // compilation options
#else
                         "",
#endif
                         NULL,
                         NULL);
    if (err != CL_SUCCESS)
    {
      constructBuildLog(err);
      goto error;
    }
  }

  /* create kernels from program */
  m_kernel = clCreateKernel(m_program, KERNEL_NAME, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto error;
  }

  /* create command queue */
  m_cmd_queue = clCreateCommandQueue(m_context, m_device, 0, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto error;
  }

  return true;

error:
  close();
  return false;
}


/**
 */
bool COpenCLRenderer::close(void)
{
  if (m_cmd_queue != NULL)
  {
    clReleaseCommandQueue(m_cmd_queue);
    m_cmd_queue = NULL;
  }

  if (m_kernel != NULL)
  {
    clReleaseKernel(m_kernel);
    m_kernel = NULL;
  }

  if (m_program != NULL)
  {
    clReleaseProgram(m_program);
    m_program = NULL;
  }

  if (m_context != NULL)
  {
    clReleaseContext(m_context);
    m_context = NULL;
  }

  return true;
}


/**
 */
std::string COpenCLRenderer::getError(void) const
{
  return m_err_msg;
}


/**
 */
bool COpenCLRenderer::renderObjectWave(const CPointCloud & pc, COpticalField *of)
{
  /* intialize local variables */
  cl_int err = CL_SUCCESS;
  cl_mem pc_buf = NULL;     // point cloud buffer
  cl_mem of_buf = NULL;     // optical field buffer

  of->zero();

  /* create memory objects from data passed in as arguments */
  pc_buf = clCreateBuffer(m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pc.getByteSize(), (void *) pc.data(), &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  of_buf = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, of->getByteSize(), NULL, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  /* a macro to set the given kernel argument type */
  #define SET_ARG(num, arg) \
    err = clSetKernelArg(m_kernel, num, sizeof(arg), &arg); \
    if (err != CL_SUCCESS) \
    { \
      m_err_msg = OpenCL::clErrToStr(err); \
      goto finalize; \
    }

  /* set kernel arguments */
  {
    cl_int rows = of->getNumRows();
    cl_int cols = of->getNumCols();
    cl_float hologram_z = (cl_float) m_hologram_z;
    cl_float k = (2 * M_PI) / of->getWaveLength();    // wave number
    cl_float pitch = of->getPitch();
    cl_float corner_x = -(cols - 1) * pitch / 2;
    cl_float corner_y = -(rows - 1) * pitch / 2;
    
    DBG("rows       : " << rows);
    DBG("cols       : " << cols);
    DBG("hologram_z : " << hologram_z);
    DBG("k          : " << k);
    DBG("sampling   : " << pitch);
    DBG("size_x     : " << (cols - 1) * pitch);
    DBG("size_y     : " << (rows - 1) * pitch);
    DBG("corner_x   : " << corner_x);
    DBG("corner_y   : " << corner_y);

    SET_ARG(0, pc_buf);
    SET_ARG(1, of_buf);
    SET_ARG(2, rows);
    SET_ARG(3, cols);
    SET_ARG(4, hologram_z);
    SET_ARG(5, k);
    SET_ARG(6, pitch);
    SET_ARG(7, corner_x);
    SET_ARG(8, corner_y);
  }

  /* to cancel kernel argument setting macro */
  #undef SET_ARG

  /* execute kernel */
  {
    // this is an array which defines the number of items in each nested loop
    size_t global_work_size[3] = { pc.size(), of->getNumRows(), of->getNumCols() };
    err = clEnqueueNDRangeKernel(m_cmd_queue,       // the command queue
                                 m_kernel,          // the kernel to be excuted
                                 3,                 // the number of nested for loops that OpenCL will generate
                                 NULL,              // the starting index of each nested for loop (allways 0, for each nested loop in my case)
                                 global_work_size,  // the number of items in each nested for loop
                                 NULL,              // this is a local work size, not really sure how it relates to the above parameters
                                 0,
                                 NULL,
                                 NULL);
    if (err != CL_SUCCESS)
    {
      m_err_msg = OpenCL::clErrToStr(err);
      goto finalize;
    }
  }

  /* read the result */
  err = clEnqueueReadBuffer(m_cmd_queue, of_buf, CL_TRUE, 0, of->getByteSize(), of->data(), 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

 
finalize:
  /* clean-up */
  clReleaseMemObject(pc_buf);
  clReleaseMemObject(of_buf);

  return (err == CL_SUCCESS);
}


/**
 */
bool COpenCLRenderer::renderHologram(const CPointCloud & pc, COpticalField *of)
{
  return true;
}


/**
 */
bool COpenCLRenderer::readCLSource(const char *filename, std::string *program_buf)
{
  HOLOREN_ASSERT(filename != NULL);
  HOLOREN_ASSERT(program_buf != NULL);

  /* open the input file */
  std::ifstream is(filename, std::ifstream::binary);
  if (!is)
  {
    m_err_msg = "Failed to open OpenCL program file";
    return false;
  }

  /* get the size of the file to be read */
  is.seekg(0, is.end);
  long program_size = is.tellg();
  is.seekg(0, is.beg);

  DBG("Kernel size: " << program_size);

  /* this check protects from accessing invalid memory
     in is.read() (as the size of program buf will be changed to 0) */
  if (program_size == 0)
  {
    m_err_msg = "OpenCL program file is empty";
    return false;
  }

  /* allocate the memory in string */
  program_buf->resize(program_size);
  //program_buf->clear();
  //program_buf->reserve();

  /* read contents of the file */
  is.read(&program_buf->front(), program_size);
   
  DBG("Kernel Program:\n" << *program_buf);
  
  if (!is)
  {
    m_err_msg = "Failed to read OpenCL program";
    return false;
  }

  is.close();

  return true;
}


/**
 */
void COpenCLRenderer::constructBuildLog(cl_int err)
{
  m_err_msg = OpenCL::clErrToStr(err);
  std::string str;
  
  if (OpenCL::getDeviceInfo(m_device, CL_DEVICE_NAME, &str) == CL_SUCCESS)
  {
    m_err_msg += "\n\"";
    m_err_msg += str;
    m_err_msg += "\"\n";
  }
  else
  {
    m_err_msg += "\"Unknown device\"\n";
  }
  
  if (OpenCL::getBuildLog(m_program, m_device, &str) == CL_SUCCESS)
  {
    m_err_msg += str;
    m_err_msg += '\n';
  }
  else
  {
    m_err_msg += "Failed to get log\n";
  }

  return;
}
