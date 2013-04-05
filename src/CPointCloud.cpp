/**
 */

#include "CPointCloud.h"


/**
 */
std::ostream & operator<<(std::ostream & os, const CPointCloud & pc)
{
  for (CPointCloud::const_iterator it = pc.begin(); it < pc.end(); ++it)
  {
    os << *it << std::endl;
  }

  return os;
}
