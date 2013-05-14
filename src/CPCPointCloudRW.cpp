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