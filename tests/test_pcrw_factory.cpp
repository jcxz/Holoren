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

  std::cout << "Reader-writer created" << std::endl;

  std::cout << "Reading point cloud" << std::endl;
  CPointCloud pc;
  if (!rw->read(&pc))
  {
    std::cout << "Failed to read file: " << argv[1] << " " << rw->getErrorMsg() << std::endl;
    delete rw;
    return 1;
  }
  std::cout << "Reading point cloud ... DONE" << std::endl;

  delete rw;

  //g_pcrw_factory.close_all();

  std::cout << "Reader-writer destroyed" << std::endl;
  
  return 0;
}
