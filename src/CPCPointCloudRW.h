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

#ifndef CPCPOINTCLOUDRW_H
#define CPCPOINTCLOUDRW_H

#include "CBasePointCloudRW.h"


/**
 * A base class providing interface
 * for readng and writing point clouds
 */
class CPCPointCloudRW : public CBasePointCloudRW
{
  public:
    /**
     * Constructor
     */
    explicit CPCPointCloudRW(const std::string & filename = std::string())
      : CBasePointCloudRW(filename)
    {
    }

    /**
     * Destructor
     */
    virtual ~CPCPointCloudRW(void)
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
     * A helper function to read a floating point number from input
     * stream separated by others with given delimiter
     * The function strips any leading or trailing whitespace.
     */
    static bool readFPNumber(std::istream & is, char end_delim, tFPType *num);
};

#endif
