/**
 */

#include "global.h"
#include "ocl.h"

#include <iostream>
#include <fstream>
#include <string>

#define KERNEL_NAME "hello"


class COCLTest
{
  public:
    COCLTest(void)
     : m_context(),
       m_cmd_queue(),
       m_program(),
       m_kernel(),
       m_device(),
       m_err_msg()
    {
    }

    std::string getError(void) const
    {
      return m_err_msg;
    }

    bool open(const char *program_file);
    bool run(void);
    void close(void);

  private:
    bool readCLSource(const char *filename, std::string *program_buf);
    cl_device_id selectDevice(void);
    void constructBuildLog(cl_int err);

  private:
    cl_context m_context;
    cl_command_queue m_cmd_queue;
    cl_program m_program;
    cl_kernel m_kernel;
    cl_device_id m_device;
    std::string m_err_msg;
};


/**
 */
bool COCLTest::open(const char *program_file)
{
 /* make sure all is reset before initializing something */
  std::string program_buf;
  cl_int err = CL_SUCCESS;
  m_err_msg = "";

  /* select the most suitable device */
  m_device = selectDevice();
  if (m_device == NULL)
  {
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
  if (!readCLSource(program_file, &program_buf))
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
  err = clBuildProgram(m_program,   // program
                            0,      // 0 as we are not providing any device list
                            NULL,   // if NULL, then the program is built for all devices, that the program is assocated with (so probably all devices from the context)
                            "",     // compilation options
                            NULL,
                            NULL);
  if (err != CL_SUCCESS)
  {
    constructBuildLog(err);
    goto error;
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
bool COCLTest::run(void)
{
  cl_int err = CL_SUCCESS;
  const size_t out_buf_size = sizeof(char) * 16;
  cl_mem out_buf = NULL;
  cl_mem i_buf = NULL;

  /* allocate a write-only memory object to hold the resutts of the computation */
  out_buf = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, out_buf_size, NULL, &err);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  {
    char i = 0;
    i_buf = clCreateBuffer(m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(i), &i, &err);
    if (err != CL_SUCCESS)
    {
      m_err_msg = OpenCL::clErrToStr(err);
      goto finalize;
    }
  }

  /* set kernel arguments */
  err = clSetKernelArg(m_kernel, 0, sizeof(out_buf), &out_buf);   // for memory object, this is a pointer to memory obejct
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  err = clSetKernelArg(m_kernel, 1, sizeof(i_buf), &i_buf);   // for memory object, this is a pointer to memory obejct
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  /* execute kernel */
  err = clEnqueueTask(m_cmd_queue, m_kernel, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    goto finalize;
  }

  /* read results */
  {
    char res_buf[out_buf_size] = { 0 };
    err = clEnqueueReadBuffer(m_cmd_queue, out_buf, CL_TRUE, 0, out_buf_size, res_buf, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
      m_err_msg = OpenCL::clErrToStr(err);
      goto finalize;
    }

    std::cout << "An output from kernel: " << std::endl;
    std::cout << "\"" << res_buf << "\"" << std::endl;
  }
  
  std::cout << "Running was OK" << std::endl;

  /* clean-up all resources */
finalize:
  clReleaseMemObject(i_buf);
  clReleaseMemObject(out_buf);

  return (err == CL_SUCCESS);
}

void COCLTest::close(void)
{
  clReleaseCommandQueue(m_cmd_queue);
  m_cmd_queue = NULL;

  clReleaseKernel(m_kernel);
  m_kernel = NULL;

  clReleaseProgram(m_program);
  m_program = NULL;

  clReleaseContext(m_context);
  m_context = NULL;

  return;
}


/**
 */
bool COCLTest::readCLSource(const char *filename, std::string *program_buf)
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
cl_device_id COCLTest::selectDevice(void)
{
    /* declare local variables */
  cl_platform_id platform;

  /* get the first available platform */
  cl_int err = clGetPlatformIDs(1, &platform, NULL);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    return NULL;
  }

  DBG(platform);

  /* select all devices availabel on the platform */
  std::vector<cl_device_id> devices;
  err = OpenCL::getDevices(platform, CL_DEVICE_TYPE_ALL, &devices);
  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
    return NULL;
  }

  /* select the one that is the most suitable (that is prefer GPU-s) */
  std::vector<cl_device_id>::iterator it = devices.begin();
  cl_device_id cpu_device = NULL;
  cl_device_id other_device = NULL;

  while (it != devices.end())
  {
    DBG(*it);

    cl_device_type type;

    err = OpenCL::getDeviceInfo(*it, CL_DEVICE_TYPE, &type);
    if (err != CL_SUCCESS)
    {
      ++it;
      continue;
    }

    if (type == CL_DEVICE_TYPE_GPU)
    {
      // immediately return if the device found was a GPU
      return *it;
    }
    else if (type == CL_DEVICE_TYPE_CPU)
    {
      cpu_device = *it;
    }
    else
    {
      other_device = *it;
    }

    ++it;
  }

  if (err != CL_SUCCESS)
  {
    m_err_msg = OpenCL::clErrToStr(err);
  }

  return ((cpu_device == NULL) ? other_device : cpu_device);
}

	
/**
 */
void COCLTest::constructBuildLog(cl_int err)
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



/////////////////////////////////////////////
//// Main Program
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "NO OpenCL program file" << std::endl;
    return 1;
  }

  COCLTest ocl_test;

  if (!ocl_test.open(argv[1]))
  {
    std::cerr << "Failed to init OpenCL" << std::endl;
    std::cerr << ocl_test.getError() << std::endl;
    return 1;
  }

  if (!ocl_test.run())
  {
    std::cerr << "Failed to run OpenCL test" << std::endl;
    std::cerr << ocl_test.getError() << std::endl;
    return 1;
  }

  ocl_test.close();

  return 0;
}
