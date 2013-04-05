/**
 * OpenCL rendering engine
 */

#ifndef COPENCLRENDERER_H
#define COPENCLRENDERER_H

#include "IRenderer.h"

#include <CL/cl.h>
#include <vector>
#include <string>


/**
 * A class to render holograms using OpenCL
 */
class COpenCLRenderer : public IRenderer
{
  public:
    /**
     * Default constructor
     */
    explicit COpenCLRenderer(double hologram_z = 0)
      : m_err(""),
        m_context(),
        m_cmd_queue(),
        m_program(),
        m_kernel(),
        m_hologram_z(hologram_z)
    {
    }

    /**
     * Destructor
     */
    virtual ~COpenCLRenderer(void)
    {
      close();
    }

    void setHologramLocation(double z)
    {
      m_hologram_z = z;
      return;
    }

    /* implement the IRenderer interface */
    virtual bool open(void);
    virtual bool close(void);
    virtual std::string getError(void) const;
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of);
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of);

    
  private:
    /**
     * A method to get all devices of a given type
     *
     * @param type the type of devices to be queried for
     *
     * @return a pointer to dynamically allocated array fo device id-s,
     *         or NULL on error
     */
    static cl_int getDevices(cl_platform_id platform, cl_device_type type, std::vector<cl_device_id> *devices);

    /**
     * A method to read the opencl program from a file
     *
     * On error values of program_buf and program_buf_size are undefined
     */
    bool readCLSource(const char *filename, std::string *program_buf);

    /**
     * A method to translate OpenCL error
     */
    static const char *clErrToStr(cl_int err);

  private:
    const char *m_err;              /// an error string
    cl_context m_context;           /// OpenCL context (needed for buffer creation and cannot be destroyd before all computations are done)
    cl_command_queue m_cmd_queue;   /// OpenCL command queue (needed to enqueue commands to devices - reads, writes, start of computation, ...)
    cl_program m_program;           /// OpenCL program (destroying it before the computations are done, won't probably do any good)
    cl_kernel m_kernel;             /// kernel that will be executed (will be sent to device for execution)
    double m_hologram_z;            /// hologram location on the z axis
};

#endif
