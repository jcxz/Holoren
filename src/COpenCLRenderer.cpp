/**
 */

#include "global.h"
#include "COpenCLRenderer.h"
#include "CPointCloud.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <fstream>
#include <cmath>

#ifndef M_PI
# define M_PI 3.1415926535897932384626433832795f
#endif



/** Kernel file */
static const char * const DEF_CL_SOURCE = "holoren_obj_wave.cl";

/** Kernel function for the single pass second algorithm */
static const char * const KERNEL_SINGLEPASS = "compObjWave_SinglePass";

/** Kernel function for the multipass algorithm */
static const char * const KERNEL_MULTIPASS = "compObjWave_MultiPass";

/** Kernel function for the multipass algorithm, which uses native instructions */
static const char * const KERNEL_MULTIPASS_NATIVE = "compObjWave_MultiPass_native";

/** Kernel function for the multipass algorithm which uses aligned access to point source array */
static const char * const KERNEL_MULTIPASS_ALIGNED = "compObjWave_MultiPass_aligned";






/**
 * A function to convert COpenCLRenderer::EAlgorithmType to string
 */
const char *COpenCLRenderer::algToStr(EAlgorithmType type)
{
  static const char *strings[] = {
    "(SinglePass) An algorithm that uses a single-pass to render the optical field",
    "(MultiPass) Unoptimised multi-pass rendering algorithm",
    "(MultiPass Native) An optimised multi-pass rendering algorithm that uses native instructions",
    "(MultiPass Aligned) A multi-pass rendering algorithm that requires each point source to be aligned to float4"
  };

  if ((type < COpenCLRenderer::ALGORITHM_SINGLEPASS) || (type > COpenCLRenderer::ALGORITHM_INVALID))
  {
    return "Invalid rendering algorithm";
  }

  return strings[type];
}


/**
 */
bool COpenCLRenderer::open(const char *filename)
{
  /* make sure all is reset before initializing something */
  std::string program_buf;
  cl_int err = CL_SUCCESS;
#ifdef HOLOREN_DEBUG_KERNEL
  uint32_t flags = (OpenCL::OPT_DEVICE_PREFER_ANY | OpenCL::OPT_PLATFORM_PREFER_INTEL);
#else
  uint32_t flags = (OpenCL::OPT_DEVICE_PREFER_ANY | OpenCL::OPT_PLATFORM_PREFER_ANY);
#endif
  m_err_msg = "";

  /* select the most suitable device */
  m_device = OpenCL::selectDevice(&flags, &err);
  if (m_device == NULL)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += (err != CL_SUCCESS) ? OpenCL::clErrToStr(err) : ("The given combination of devices and platforms could not be matched");
    goto error;
  }

  DBG("selected device:\n" << m_device);
  
  /* get the value of how many bytes of data can be allocated (and processed) at once by GPU */
  err = OpenCL::getDeviceInfo(m_device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, &m_mem_obj_max_size);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    goto error;
  }

  /* create OpenCL context for all GPU devices on the machine */
  m_context = clCreateContext(NULL, 1, &m_device, NULL, NULL, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
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
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": ";
      m_err_msg += OpenCL::clErrToStr(err);
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
	  /* ATTENTION: when debugging with intel OpenCL SDK,
	                this MUST be the FULL path to the OpenCL kernel,
					i.e. this cannot be a relative path like "../../my_kernel.cl".
					It MUST be "C:\kernels\my_kernel.cl"
					Otherwise intel's debugger will silently fail to break into
					kernel's code */
      options += " -s \"";
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
  {
    const char *kernel = "";
    switch (m_alg_type)
    {
      case ALGORITHM_SINGLEPASS:        kernel = KERNEL_SINGLEPASS;        break;
      case ALGORITHM_MULTIPASS:         kernel = KERNEL_MULTIPASS;         break;
      case ALGORITHM_MULTIPASS_NATIVE:  kernel = KERNEL_MULTIPASS_NATIVE;  break;
      case ALGORITHM_MULTIPASS_ALIGNED: kernel = KERNEL_MULTIPASS_ALIGNED; break;
      default: m_err_msg = "Unknown algorithm type"; goto error;
    }

    m_kernel = clCreateKernel(m_program, kernel, &err);
    if (err != CL_SUCCESS)
    {
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": ";
      m_err_msg += OpenCL::clErrToStr(err);
      goto error;
    }
  }

  /* create command queue */
  m_cmd_queue = clCreateCommandQueue(m_context, m_device, 0, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
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
bool COpenCLRenderer::renderHologram(const CPointCloud & pc, COpticalField *of)
{
  return true;
}


/**
 */
bool COpenCLRenderer::renderObjectWave(const CPointCloud & pc, COpticalField *of)
{
  /* check on preconditions */
  HOLOREN_ASSERT(of != NULL);

  /* intialize local variables */
  cl_int err = CL_SUCCESS;

#ifdef HOLOREN_HEAVY_DEBUG
  DBG("point cloud byte size: " << pc.getByteSize());
  DBG("point cloud data:");
  DBGHEX(pc.data(), pc.getByteSize());
  DBG("");
#endif

  /* ensure that the right amount of memory is allocated for memory object */   
  if (m_alg_type == ALGORITHM_SINGLEPASS)
  {
    DBG("of->getByteSize()  : " << of->getByteSize());
    DBG("m_mem_obj_max_size : " << m_mem_obj_max_size);
    if (of->getByteSize() > m_mem_obj_max_size)
    {
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": The optical field is too large for algorithm 2";
      return false;
    }
    
    m_chunk_size = of->getSize();
  }
  else if (m_chunk_size == 0)
  { // chunk size is described in elements (not in bytes)
    m_chunk_size = m_mem_obj_max_size / sizeof(COpticalField::CComplex);
  }
  else if (m_chunk_size > of->getSize())
  {
    m_chunk_size = of->getSize();
  }

  /* create a memory object for point cloud data */
  cl_mem pc_buf = ((m_alg_type == ALGORITHM_MULTIPASS_ALIGNED) ?
                     (
                       DBG("pc_buf allocated size: " << (pc.size() * sizeof(cl_float4))),
                       clCreateBuffer(m_context, CL_MEM_READ_ONLY, pc.size() * sizeof(cl_float4), NULL, &err)
                     ) :
                     (
                       DBG("pc_buf allocated size: " << pc.getByteSize()),
                       clCreateBuffer(m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, pc.getByteSize(), (void *) pc.data(), &err)
                     )
                  );
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }

  if ((m_alg_type == ALGORITHM_MULTIPASS_ALIGNED) && (!fillPCMemObj(pc, pc_buf)))
  {
    clReleaseMemObject(pc_buf);
    return false;
  }

  /* create a memory object for the optical field */
  cl_mem of_buf = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, m_chunk_size * sizeof(COpticalField::CComplex), NULL, &err);
  if (err != CL_SUCCESS)
  {
    clReleaseMemObject(pc_buf);
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }
  
  /* render the hologram */
  bool ret = false;
  switch (m_alg_type)
  {
    case ALGORITHM_SINGLEPASS: ret = renderAlgorithm_SinglePass(pc, pc_buf, of, of_buf); break;
    case ALGORITHM_MULTIPASS:
    case ALGORITHM_MULTIPASS_NATIVE:
    case ALGORITHM_MULTIPASS_ALIGNED: ret = renderAlgorithm_MultiPass(pc, pc_buf, of, of_buf); break;
    default: m_err_msg = "Unknown algorithm type"; break;
  }

  clReleaseMemObject(of_buf);
  clReleaseMemObject(pc_buf);

  return ret;
}


/**
 */
bool COpenCLRenderer::renderAlgorithm_SinglePass(const CPointCloud & pc, cl_mem pc_buf, COpticalField *of, cl_mem of_buf)
{
  DBG("Single Pass Algorithm");

  /* a macro to set the given kernel argument type */
  #define SET_ARG(num, arg) \
    err = clSetKernelArg(m_kernel, num, sizeof(arg), &arg); \
    if (err != CL_SUCCESS) \
    { \
      m_err_msg = DBGSTRLOC(); \
      m_err_msg += "Argument "; \
      m_err_msg += std::to_string(static_cast<long long>(num));\
      m_err_msg += ": "; \
      m_err_msg += OpenCL::clErrToStr(err); \
      return false; \
    }

  cl_int err = CL_SUCCESS;

  /* set kernel arguments */
  cl_uint pc_size = pc.size();
  cl_int rows = of->getNumRows();
  cl_int cols = of->getNumCols();
  cl_float hologram_z = (cl_float) m_hologram_z;
  cl_float k = (2 * M_PI) / of->getWaveLength();    // wave number
  cl_float pitch = of->getPitch();
  cl_float corner_x = -(cols - 1) * pitch / 2;
  cl_float corner_y = -(rows - 1) * pitch / 2;
  
  DBG("pc_size    : " << pc_size);
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
  SET_ARG(1, pc_size);
  SET_ARG(2, of_buf);
  SET_ARG(3, hologram_z);
  SET_ARG(4, k);
  SET_ARG(5, pitch);
  SET_ARG(6, corner_x);
  SET_ARG(7, corner_y);

  /* to cancel kernel argument setting macro */
  #undef SET_ARG

  /* execute kernel */
  // this is an array which defines the number of items in each nested loop
  size_t global_work_size[2] = { (size_t) rows, (size_t) cols };
  err = clEnqueueNDRangeKernel(m_cmd_queue,       // the command queue
                               m_kernel,          // the kernel to be excuted
                               2,                 // the number of nested for loops that OpenCL will generate
                               NULL,              // the starting index of each nested for loop (allways 0, for each nested loop in my case)
                               global_work_size,  // the number of items in each nested for loop
                               NULL,              // this is a local work size, not really sure how it relates to the above parameters
                               0,
                               NULL,
                               NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }

#ifdef HOLOREN_DEBUG_KERNEL
  err = clFinish(m_cmd_queue);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }
#endif

  /* read out results */
  err = clEnqueueReadBuffer(m_cmd_queue, of_buf, CL_TRUE, 0, of->getByteSize(), of->data(), 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }

  return true;
}


/**
 */
bool COpenCLRenderer::renderAlgorithm_MultiPass(const CPointCloud & pc, cl_mem pc_buf, COpticalField *of, cl_mem of_buf)
{
  DBG("Multi Pass Algorithm");

  /* a macro to set the given kernel argument type */
  // the static_cast<> is for msvc 2010, which lacks the proper support of c++11
  #define SET_ARG(num, arg) \
    err = clSetKernelArg(m_kernel, num, sizeof(arg), &arg); \
    if (err != CL_SUCCESS) \
    { \
      m_err_msg = DBGSTRLOC(); \
      m_err_msg += "Argument "; \
      m_err_msg += std::to_string(static_cast<long long>(num));\
      m_err_msg += ": "; \
      m_err_msg += OpenCL::clErrToStr(err); \
      return false; \
    }

  cl_int err = CL_SUCCESS;

  /* set kernel arguments */
  cl_uint pc_size = pc.size();
  size_t of_size = of->getSize();
  cl_int rows = of->getNumRows();
  cl_int cols = of->getNumCols();
  cl_float hologram_z = (cl_float) m_hologram_z;
  cl_float k = (2 * M_PI) / of->getWaveLength();    // wave number
  cl_float pitch = of->getPitch();
  cl_float corner_x = -(cols - 1) * pitch / 2;
  cl_float corner_y = -(rows - 1) * pitch / 2;
  
  DBG("pc_size    : " << pc_size);
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
  SET_ARG(1, pc_size);
  SET_ARG(2, of_buf);
  
  SET_ARG(4, cols);
  SET_ARG(5, hologram_z);
  SET_ARG(6, k);
  SET_ARG(7, pitch);
  SET_ARG(8, corner_x);
  SET_ARG(9, corner_y);

  DBG("of_size     : " << of_size);
  DBG("m_max_chunk_size : " << m_chunk_size);

  /* break the optical field into chunks that can be processed by gpu and
     render the object wave of hologram */
  for (size_t chunk = 0; chunk < of_size; chunk += m_chunk_size)
  {
    SET_ARG(3, chunk);

    size_t global_work_size = std::min(m_chunk_size, of_size - chunk);
    
    DBG("chunk num.         : " << (chunk / m_chunk_size));
    DBG("chunk              : " << (chunk));
    DBG("global_work_size   : " << (global_work_size));
    DBG("of->data() + chunk : " << (unsigned long long int) (of->data() + chunk));
    DBG("");

    /* execute kernel */
    err = clEnqueueNDRangeKernel(m_cmd_queue,        // the command queue
                                 m_kernel,           // the kernel to be excuted
                                 1,                  // the number of nested for loops that OpenCL will generate
                                 NULL,               // the starting index of each nested for loop (allways 0, for each nested loop in my case)
                                 &global_work_size,  // the number of items in each nested for loop
                                 NULL,               // this is a local work size, not really sure how it relates to the above parameters
                                 0,
                                 NULL,
                                 NULL);
    if (err != CL_SUCCESS)
    {
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": ";
      m_err_msg += OpenCL::clErrToStr(err);
      return false;
    }

#ifdef HOLOREN_DEBUG_KERNEL
    err = clFinish(m_cmd_queue);
    if (err != CL_SUCCESS)
    {
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": ";
      m_err_msg += OpenCL::clErrToStr(err);
      return false;
    }
#endif

    /* read the result */
    err = clEnqueueReadBuffer(m_cmd_queue,
                              of_buf,
                              CL_TRUE,
                              0,
                              global_work_size * sizeof(COpticalField::CComplex),
                              ((char *) (of->data() + chunk)),
                              0,
                              NULL,
                              NULL);
    if (err != CL_SUCCESS)
    {
      m_err_msg = DBGSTRLOC();
      m_err_msg += ": ";
      m_err_msg += OpenCL::clErrToStr(err);
      return false;
    }
  }

  /* to cancel kernel argument setting macro */
  #undef SET_ARG

  return true;
}


/**
 */
bool COpenCLRenderer::fillPCMemObj(const CPointCloud & pc, cl_mem pc_buf)
{
  /* Map point cloud buffer */
  cl_int err = CL_SUCCESS;
  cl_float4 *data = (cl_float4 *) clEnqueueMapBuffer(m_cmd_queue,
                                                     pc_buf,
                                                     CL_TRUE,
                                                     CL_MAP_WRITE,
                                                     0,
                                                     pc.size() * sizeof(cl_float4),
                                                     0,
                                                     NULL,
                                                     NULL,
                                                     &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }

  /* fill the buffer with point sources at correct offsets */
  for (size_t i = 0; i < pc.size(); ++i)
  {
    data[i].s[0] = pc[i].x;
    data[i].s[1] = pc[i].y;
    data[i].s[2] = pc[i].z;
    // skip the last element
  }

  /* unmap point cloud memory object */
  err = clEnqueueUnmapMemObject(m_cmd_queue, pc_buf, data, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": ";
    m_err_msg += OpenCL::clErrToStr(err);
    return false;
  }

  return true;
}


/**
 */
bool COpenCLRenderer::readCLSource(const char *filename, std::string *program_buf)
{
  HOLOREN_ASSERT(filename != NULL);
  HOLOREN_ASSERT(program_buf != NULL);

  DBG("OpenCL source: " << filename);

  /* open the input file */
  std::ifstream is(filename, std::ifstream::binary);
  if (!is)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": Failed to open OpenCL program file";
    return false;
  }

  /* get the size of the file to be read */
  is.seekg(0, is.end);
  std::ifstream::streamoff program_size = is.tellg();
  is.seekg(0, is.beg);

  DBG("Source size: " << program_size);

  /* this check protects from accessing invalid memory
     in is.read() (as the size of program buf will be changed to 0) */
  if (program_size == 0)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": OpenCL program file is empty";
    return false;
  }

  /* allocate the memory in string */
  program_buf->resize(static_cast<std::string::size_type>(program_size));
  //program_buf->clear();
  //program_buf->reserve();

  /* read contents of the file */
  is.read(&program_buf->front(), program_size);
   
#ifdef HOLOREN_HEAVY_DEBUG
  DBG("Kernel Program:\n" << *program_buf);
#endif

  if (!is)
  {
    m_err_msg = DBGSTRLOC();
    m_err_msg += ": Failed to read OpenCL program";
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