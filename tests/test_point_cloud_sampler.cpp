/**
 */

#include "global.h"
#include "CPointCloudRWFactory.h"
#include "CBasePointCloudRW.h"
#include "CPointCloud.h"

#include <iostream>



int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing filename" << std::endl;
    return 1;
  }

  CBasePointCloudRW *rw = g_pcrw_factory.getReaderWriter(argv[1]);
  if (rw == NULL)
  {
    std::cerr << "The conversion from given file type to point cloud is not supported" << std::endl;
    return 1;
  }

  /* set sampling step */
  rw->setStep(10e-2);
  
  /* convert to point cloud from a given file type */
  CPointCloud pc;
  if (!rw->read(&pc))
  {
    std::cerr << "Failed to conver the file "
              << argv[1]
              << "the point cloud: "
              << rw->getErrorMsg()
              << std::endl;
    delete rw;
    return 1;
  }
  
  /* display the sampled points */
  std::cout << "Sampled points (step == " << rw->getStep() << "): " << std::endl;
  std::cout << pc << std::endl;

  delete rw;
  
  return 0;
}
