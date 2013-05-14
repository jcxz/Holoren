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

#include "global.h"
#include "CPointCloudRWFactory.h"
#include "CXMLPointCloudRW.h"
#include "CPCPointCloudRW.h"
#include "CImagePointCloudRW.h"

#include <string>


/* Factory instantiation */
CPointCloudRWFactory CPointCloudRWFactory::m_instance;


/**
 */
CPointCloudRWFactory::CPointCloudRWFactory(void)
{
  m_rws[RW_XML].cleanup_fn   = 0;
  m_rws[RW_XML].created      = false;
  m_rws[RW_IMAGE].cleanup_fn = 0;
  m_rws[RW_IMAGE].created    = false;
  m_rws[RW_PC].cleanup_fn    = 0;
  m_rws[RW_PC].created       = false;
}


/**
 */
CPointCloudRWFactory::~CPointCloudRWFactory(void)
{
  if (m_rws[RW_XML].cleanup_fn)
  {
    (*m_rws[RW_XML].cleanup_fn)();
  }

  if (m_rws[RW_IMAGE].cleanup_fn)
  {
    (*m_rws[RW_IMAGE].cleanup_fn)();
  }

  if (m_rws[RW_PC].cleanup_fn)
  {
    (*m_rws[RW_PC].cleanup_fn)();
  }
}


/**
 */
CBasePointCloudRW *CPointCloudRWFactory::getReaderWriter(const std::string & filename, EReaderWriterType type)
{
  std::string::size_type len = filename.size();

  /* xml based point cloud reader-writer */
  if ((type == RW_XML) || (filename.compare(len - 4, 4, ".xml") == 0))
  {
    DBG("Creating *.xml handler");

    CXMLPointCloudRW *rw = new CXMLPointCloudRW(filename);

    if (!m_rws[RW_XML].created)
    {
      if (!rw->init(&m_rws[RW_XML].cleanup_fn))
      {
        m_rws[RW_XML].cleanup_fn = NULL;  // reset the handler (just in case)
        delete rw;
        return NULL;
      }

      m_rws[RW_XML].created = true;
    }

    return rw;
  }
  /* *.pc file based point cloud reader-writer */
  else if ((type == RW_PC) || (filename.compare(len - 3, 3, ".pc") == 0))
  {
    DBG("Creating *.pc handler");

    CPCPointCloudRW *rw = new CPCPointCloudRW(filename);

    if (!m_rws[RW_PC].created)
    {
      if (!rw->init(&m_rws[RW_PC].cleanup_fn))
      {
        m_rws[RW_PC].cleanup_fn = NULL;
        delete rw;
        return NULL;
      }

      m_rws[RW_PC].created = true;
    }

    return rw;
  }
  /* image based point cloud reader-writer */
  else if ((type == RW_IMAGE) || (filename.compare(len - 5, 5, ".jpeg") == 0) ||
                                 (filename.compare(len - 4, 4, ".jpg") == 0)  ||
                                 (filename.compare(len - 4, 4, ".png") == 0)  ||
                                 (filename.compare(len - 4, 4, ".gif") == 0)  ||
                                 (filename.compare(len - 4, 4, ".bmp") == 0)  ||
                                 (filename.compare(len - 4, 4, ".tga") == 0)  ||
                                 (filename.compare(len - 4, 4, ".tif") == 0)  ||
                                 (filename.compare(len - 5, 5, ".tiff") == 0))
  {
    DBG("Creating *" << filename.substr(len - 4, 4) << " handler");

    CImagePointCloudRW *rw = new CImagePointCloudRW(filename);

    if (!m_rws[RW_IMAGE].created)
    {
      if (!rw->init(&m_rws[RW_IMAGE].cleanup_fn))
      {
        m_rws[RW_IMAGE].cleanup_fn = NULL;
        delete rw;
        return NULL;
      }

      m_rws[RW_IMAGE].created = true;
    }

    return rw;
  }

  return NULL;
}
