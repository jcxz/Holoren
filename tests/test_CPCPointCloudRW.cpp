#include "CPCPointCloudRW.h"
#include "CPointCloud.h"

#include <iostream>



int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: input_file output_file" << std::endl;
    return 1;
  }

  CPointCloud pc;
  CPCPointCloudRW rw(argv[1]);

  if (!rw.read(&pc))
  {
    std::cerr << "Failed to read point cloud file. Read sofar:" << std::endl;
    std::cout << pc << std::endl;
    return 1;
  }

  std::cout << "Resulting point cloud:" << std::endl;
  std::cout << pc << std::endl;

  rw.setFileName(argv[2]);

  if (!rw.write(&pc))
  {
    std::cerr << "Failed to write to point cloud file" << std::endl;
    return 1;
  }

  return 0;
}