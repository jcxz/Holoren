/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        m_sampling_step(step)
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
    float getSamplingStep(void) const
    {
      return m_sampling_step;
    }

    /**
     * Set the step used for sampling geometry
     * For example a step of 10e-2 means that there
     * will be 10 points generated for each sampled line.
     * The sampling step for images means the size of pixel
     * in image in meters.
     *
     * @param step the step to be used
     */
    void setSamplingStep(float step)
    {
      m_sampling_step = step;
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
    float m_sampling_step;   /// the step to be used when sampling lines/images
};

#endif
