/**
 */

#include "config.h"
#include "global.h"
#include "CBasePointCloudRW.h"
#include "CPointCloud.h"



/**
 */
void CBasePointCloudRW::sample(const Geometry::SLine & line, CPointCloud *pc)
{
  /* compute direction vector of the line */
  Geometry::SPoint3D dir(line.p2.x - line.p1.x,
                         line.p2.y - line.p1.y,
                         line.p2.z - line.p1.z);
  double t = 0.0;

  /* sample the line with a given fixed step */
  while (t < 1.0f)
  {
    //DBG("t == " << t);
    pc->addPointSource(Geometry::SPoint3D(line.p1.x + dir.x * t,
                                          line.p1.y + dir.y * t,
                                          line.p1.z + dir.z * t));
    t += m_sampling_step;
  }

  /* this line is to compensate for lack of accuracy in floating point
     numbers and to make sure that the end point is allways included */
  //pc->addPointSource(Geometry::SPoint3D(line.p2.x, line.p2.y, line.p2.z));

  return;
}
