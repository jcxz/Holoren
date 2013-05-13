/**
 */

#include "CPCPointCloudRW.h"
#include "Debug.h"
#include "CPointCloud.h"

#include <string>
#include <fstream>



/**
 */
bool CPCPointCloudRW::init(tCleanUpHandler *p_handler)
{
  if (p_handler != NULL)
  {
    *p_handler = NULL;
  }

  return true;
}


/**
 */
bool CPCPointCloudRW::read(CPointCloud *pc)
{
  HOLOREN_ASSERT(pc != NULL);

  std::ifstream is(m_filename);

  if (!is.is_open())
  {
    DBG("Failed to read from PC file: " << m_filename);
    return false;
  }

  pc->clear();
  
  SPointSource ps;

  while (1)
  {
    if (!(readFPNumber(is, ',', &ps.x) && readFPNumber(is, ',', &ps.y) && readFPNumber(is, '\n', &ps.z)))
    {
      return false;
    }

    pc->addPointSource(ps);
    
    // see if there is more lines
    if (is.peek() == EOF)
    {
      return true;
    }
  }

  return false;  // should never get here
}


/**
 */
bool CPCPointCloudRW::write(CPointCloud *pc)
{
  HOLOREN_ASSERT(pc != NULL);

  std::ofstream os(m_filename);

  if (!os.is_open())
  {
    DBG("Failed to write to PC file: " << m_filename);
    return false;
  }
  
  for (unsigned int i = 0; i < pc->size(); ++i)
  {
    const SPointSource & ps = pc->getPointSource(i);
    os << ps.x << ", " << ps.y << ", " << ps.z << '\n';
  }

  os.close();  // make sure the output is actually flushed

  return true;
}


/**
 */
bool CPCPointCloudRW::readFPNumber(std::istream & is, char end_delim, tFPType *num)
{
  HOLOREN_ASSERT(num != NULL);

  // skip leading whitespace
  while (isspace(is.peek()))
  {
    is.ignore();
  }

  // read in the number
  is >> *num;

  // skip trailing whitespace and delimiter
  while (1)
  {
    int c = is.get();
    if (c == end_delim)
    {
      return true;
    }

    if (!isspace(c))
    {
      return false;
    }
  }

  return false;  // should not get here
}