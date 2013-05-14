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
 * A common renderer interface
 */

#ifndef CBASERENDERER_H
#define CBASERENDERER_H

#include "global.h"

#include <string>


class CPointCloud;
class COpticalField;

/**
 * This class defines an interface that all renderers should comply to
 */
class CBaseRenderer
{
  public:
	/**
     * Default constructor
	 */
	explicit CBaseRenderer(double z = 0.0f)
      : m_hologram_z(z)
    {
    }

    /**
     * Destructor
     */
    virtual ~CBaseRenderer(void)
    {
    }

	/**
	 * A function to set the hologram position on z-axis (in metres)
     *
     * @param z the location of hologram in z axis
	 */
    void setHologramLocation(double z)
    {
      m_hologram_z = z;
      return;
    }

    /**
     * A method to initialize renderer
     * E.g to query platform, devices, create an OpenCL/CUDA context
     * compile kernels ... and do all the oprations that are necessary
     * to be executed only once before rendering.
     * If this method would require any arguments, that can be provided
     * as setters in the derived class
     *
     * @param filename the name of the OpenCL/CUDA source file, that will be used
     *                 by this renderer (if none is given renderer will use its own
     *                 built in program)
     *
     * @return true of the method succeeded, false otherwise
     */
    virtual bool open(const char *filename = NULL) = 0;

    /**
     * A method to deinitialize renderer
     * This method is intended for all one time operations that have to
     * be performed after all rendering is done
     *
     * @return true of operation succeede, false otherwise
     */
    virtual bool close(void) = 0;

    /**
     * A method to return a string describing the error
     * that occured whiel rendering or intialising the renderer
     *
     * @return a string describing the error that happened,
     *         e.g. a build log or an error message from OpenCL/CUDA
     */
    virtual std::string getError(void) const = 0;

    /**
     * A method to render only an object wave
     *
     * @param pc the point cloud that will be rendered
     * @param of an optical field where the rendering results will be saved
     *
     * @return true of the operation succeeded, false otherwise
     */
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of) = 0;

    /**
     * A method to render the comple hologram including adding the reference wave
     *
     * @param pc the point cloud that will be rendered
     * @param of an optical field where the rendering results will be saved
     *
     * @return true of the operation succeeded, false otherwise
     */
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of) = 0;

  private:
    /* disable assignment, copy and move */
    CBaseRenderer(const CBaseRenderer & );
    HOLOREN_DECL_MOVE_CONSTRUCTOR(CBaseRenderer);
    CBaseRenderer & operator=(const CBaseRenderer & );

  protected:
    double m_hologram_z;  /// hologram location on the z axis
};

#endif
