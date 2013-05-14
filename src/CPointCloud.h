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
 * Point Cloud data structure
 */

#ifndef CPOINTCLOUD_H
#define CPOINTCLOUD_H

#include "Geometry.h"

#include <vector>
#include <ostream>



/**
 * A typedef imitating Point Source structure
 */
typedef Geometry::SPoint3D SPointSource;

/**
 */
class CPointCloud : private std::vector<SPointSource>
{
  private:
    typedef std::vector<SPointSource> CPointCloudBase;

  public:
    using CPointCloudBase::size;
    using CPointCloudBase::clear;
    using CPointCloudBase::resize;
    using CPointCloudBase::data;
    using CPointCloudBase::operator[];

  public:
    /**
     * Default constructor
     */
    CPointCloud(void)
      : CPointCloudBase()
    {
    }

    /**
     * A method to get a pointer to contigious memory
     * with point sources
     */
    //const SPointSource *getPointSources(void) const
    //{
    //  return &front();
    //}

    /**
     * A method to add a single point source to point cloud
     */
    void addPointSource(const SPointSource & ps)
    {
      CPointCloudBase::push_back(ps);
      return;
    }

    /**
     * A method to get the value of point source at a given index
     */
    const SPointSource & getPointSource(unsigned int i) const
    {
      return CPointCloudBase::operator[](i);
    }

    /**
     * Get the size of data in bytes held by this container
     */
    size_t getByteSize(void) const
    {
      return size() * sizeof(SPointSource);
    }

    /**
     * Output stream inserter
     */
    friend std::ostream & operator<<(std::ostream & os, const CPointCloud & pc);
};

#endif
