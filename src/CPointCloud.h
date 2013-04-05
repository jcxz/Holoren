/**
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
    const SPointSource *getPointSources(void) const
    {
      return &front();
    }

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
     * Output stream inserter
     */
    friend std::ostream & operator<<(std::ostream & os, const CPointCloud & pc);
};

#endif
