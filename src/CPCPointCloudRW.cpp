/**
 */

#include "CPCPointCloudRW.h"



/**
 */
bool CPCPointCloudRW::init(tCleanUpHandler *p_handler)
{
  if (p_handler != NULL)
  {
    *p_handler = NULL;
  }

  return false;
}


/**
 */
bool CPCPointCloudRW::read(CPointCloud *pc)
{
  return false;
}


/**
 */
bool CPCPointCloudRW::write(CPointCloud *pc)
{
  return false;
}
