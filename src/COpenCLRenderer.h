/**
 * OpenCL rendering engine
 */

#ifndef COPENCLRENDERER_H
#define COPENCLRENDERER_H

#include "CBaseRenderer.h"
#include "ocl.h"

#include <vector>
#include <string>


/**
 * A class to render holograms using OpenCL
 */
class COpenCLRenderer : public CBaseRenderer
{
  public:
    /**
     * Default constructor
     */
    explicit COpenCLRenderer(double hologram_z = 0)
      : CBaseRenderer(hologram_z),
        m_context(),
        m_cmd_queue(),
        m_program(),
        m_kernel(),
        m_device(),
        m_err_msg("")
    {
    }

    /**
     * Destructor
     */
    virtual ~COpenCLRenderer(void)
    {
      close();
    }

    /* implement the IRenderer interface */
    virtual bool open(const char *filename = NULL);
    virtual bool close(void);
    virtual std::string getError(void) const;
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of);
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of);

    
  private:
    /* disable assignment, copy and move
       If I wanted to implement these someday in future
       I would have to do so in terms of retain and release */
    COpenCLRenderer(const COpenCLRenderer & );
    COpenCLRenderer(COpenCLRenderer && );
    COpenCLRenderer & operator=(const COpenCLRenderer & );

  private:
    /**
     * A method to read the opencl program from a file
     *
     * On error values of program_buf and program_buf_size are undefined
     */
    bool readCLSource(const char *filename, std::string *program_buf);

    /**
     * A function to create a build log for the device
     */
    void constructBuildLog(cl_int err);

  private:
    /** An enumeration of error codes */
    enum EErrorCode {
      ERR_OK = 0,     /// no error
      ERR_OPEN,       /// error opening OpenCL source file
      ERR_EMPTY,      /// the file with OpenCL program was empty
      ERR_READ,       /// an error reading OpenCL source file
      ERR_BUILD,      /// an error building OpenCL source
      ERR_UNKNOWN     /// an undefined error
    };

  private:
    cl_context m_context;          /// OpenCL context (needed for buffer creation and cannot be destroyd before all computations are done)
    cl_command_queue m_cmd_queue;  /// OpenCL command queue (needed to enqueue commands to devices - reads, writes, start of computation, ...)
    cl_program m_program;          /// OpenCL program (destroying it before the computations are done, won't probably do any good)
    cl_kernel m_kernel;            /// kernel that will be executed (will be sent to device for execution)
    cl_device_id m_device;         /// the device used
    std::string m_err_msg;         /// a string to hold the error message and a build log
};

#endif
