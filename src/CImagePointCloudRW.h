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

#ifndef CIMAGEPOINTCLOUDRW_H
#define CIMAGEPOINTCLOUDRW_H

#include "CBasePointCloudRW.h"


struct SDL_Surface;

/**
 * A base class providing interface
 * for readng and writing point clouds
 */
class CImagePointCloudRW : public CBasePointCloudRW
{
  public:
    /**
     * Constructor
     */
    explicit CImagePointCloudRW(const std::string & filename = std::string(), float step = 0.000025)
      : CBasePointCloudRW(filename, step)
    {
    }

    /**
     * Destructor
     */
    virtual ~CImagePointCloudRW(void)
    {
    }

    /**
     * A method which is called once before
     * using any instance of a given concrete
     * point cloud reader class.
     * This method can be used for things like
     * library initialisation.
     * Users should not directly access this method
     * it is meant to be used only by CPointCloudRWFactory
     */
    virtual bool init(tCleanUpHandler *p_handler);

    /**
     * A method to read a file which can be converted
     * to point cloud data structure
     */
    virtual bool read(CPointCloud *pc);

    /**
     * A method to write point cloud to a file.
     * If this method is not supported for a given
     * reader it should just return false.
     */
    virtual bool write(CPointCloud *pc);

  private:
    /**
     * A method to convert the given SDL surface if necessary
     */
    bool toRGBASurface(SDL_Surface **surf);

    /**
     * A method to convert surface to greyscale
     */
    bool toGreyScale(SDL_Surface *surf);
    
    /**
     * A method to convert the surface to binary.
     * Method works in place and assumes that the surface was previously
     * converted to greyscale with toGreyScale
     */
    bool toBinary(SDL_Surface *surf, uint8_t treshold);

    /**
     * A method to compute the average intesity of grey color
     * in the image
     */
    bool getAvgIntensity(SDL_Surface *surf, uint8_t *intensity);

    /**
     * A method to generate point sources from a given greyscale
     * image by utilising the given treshold
     */
    bool genPointSources(SDL_Surface *surf, uint8_t treshold, tFPType z, double sampling, CPointCloud *pc);
};

#endif
