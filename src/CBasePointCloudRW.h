/**
 * An abstract base class for reading and writing point clouds
 */

#ifndef CBASEPOINTCLOUDRW_H
#define CBASEPOINTCLOUDRW_H

#include "Geometry.h"


class CPointCloud;

/**
 * A base class providing interface
 * for readng and writing point clouds
 */
class CBasePointCloudRW
{
  public:
    /* Clean-up handler type */
    typedef void (*tCleanUpHandler)(void);

  public:
    /**
     * Constructor
     */
    explicit CBasePointCloudRW(const std::string & filename = std::string(), float step = 10e-2)
      : m_filename(filename),
        m_error(""),
        m_step(step)
    {
    }

    /**
     * Destructor
     */
    virtual ~CBasePointCloudRW(void)
    {
    }

    /**
     * A method to get the name of file from which this point cloud
     * was generated.
     *
     * @return filename used
     */
    const std::string & getFileName(void) const
    {
      return m_filename;
    }

    /**
     * A method to set the name of file to be used for reading
     * or writing point cloud
     *
     * @param filename the name of the file to be used
     */
    void setFileName(const std::string & filename)
    {
      m_filename = filename;
      return;
    }

    /**
     * Get the value of sampling step used for sampling
     * lines in geometry
     *
     * @return sampling step
     */
    float getStep(void) const
    {
      return m_step;
    }

    /**
     * Set the step used for sampling geometry
     * For example a step of 10e-2 means that there
     * will be 10 points generated for each sampled line.
     *
     * @param step the step to be used
     */
    void setStep(float step)
    {
      m_step = step;
      return;
    }

    /**
     * A method to return an error description
     *
     * @return a detailed english error message
     */
    const std::string & getErrorMsg(void) const
    {
      return m_error;
    }

    /**
     * A method which is called once before
     * using any instance of a given concrete
     * point cloud reader class.
     * This method can be used for things like
     * library initialisation.
     * Users should not directly access this method
     * it is meant to be used only by CPointCloudRWFactory
     *
     * @param p_handler a pointer to variable, where a pointer
     *                  to cleanup handler will be stored.
     *                  If the given concrete PointCloud reader
     *                  does not require one, it has to be set to NULL.
     *
     * @return true on success, false on error
     */
    virtual bool init(tCleanUpHandler *p_handler) = 0;

    /**
     * A method to read a file which can be converted
     * to point cloud data structure
     */
    virtual bool read(CPointCloud *pc) = 0;

    /**
     * A method to write point cloud to a file.
     * If this method is not supported for a given
     * reader it should just return false.
     */
    virtual bool write(CPointCloud *pc) = 0;

    
  protected:
    /**
     * A method to convert a line to a point cloud
     * all the sampled points of a line will be added
     * to the point cloud
     */
    void sample(const Geometry::SLine & line, CPointCloud *pc);

  protected:
    std::string m_filename;  /// the name of the file to be used for reading/writing
    std::string m_error;     /// error description

  private:
    float m_step;            /// the step to be used when sampling lines
};

#endif
