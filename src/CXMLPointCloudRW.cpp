/**
 */


#include "global.h"
#include "Utils.h"
#include "CPointCloud.h"
#include "CXMLPointcloudRW.h"


static void cleanup_handler(void)
{
  DBG("Cleaningup parser");
  xmlCleanupParser();
  return;
}

/**
 */
bool CXMLPointCloudRW::init(tCleanUpHandler *p_handler)
{
  xmlInitParser();

  if (p_handler != NULL)
  {
    //*p_handler = xmlCleanupParser;
    *p_handler = &cleanup_handler;
  }

  return true;
}


/**
 */
bool CXMLPointCloudRW::read(CPointCloud *pc)
{
  HOLOREN_ASSERT(pc != NULL);

  /* necessary initialisations */
  m_error = "No error";
  xmlTextReaderPtr reader = NULL;
  xmlChar *name = NULL;
  int ret = 0;

  pc->clear();

  /* create an instance of XML file reader */
  reader = xmlReaderForFile(m_filename.c_str(), NULL, XML_PARSE_NOBLANKS);
  if (reader == NULL)
  {
    m_error = "Failed to create an instance of XML reader";
    goto error;
  }

  /* read the root element */
  ret = xmlTextReaderRead(reader);
  if ((ret != 1) || (xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT))
  {
    m_error = "Missing root element";
    goto error;
  }

  /* check on the root element */
  name = xmlTextReaderName(reader);
  if ((name == NULL) || (xmlStrcasecmp(name, (xmlChar *) "scene") != 0))
  {
    m_error = "Incorrect root element";
    goto error;
  }

  xmlFree(name);

  /* read the contents of an XML file */
  while ((ret = xmlTextReaderRead(reader)) == 1)
  {
    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
    {
      name = xmlTextReaderName(reader);
      if (name == NULL)
      {
        m_error = "Empty element name";
        goto error;
      }

      // process a cuboid declaration
      if (xmlStrcasecmp(name, (xmlChar *) "cuboid") == 0)
      {
        if (!processCuboid(reader, pc)) goto error;
      }
      else if (xmlStrcasecmp(name, (xmlChar *) "point") == 0)
      {
        if (!processPoint(reader, pc)) goto error;
      }
      else if (xmlStrcasecmp(name, (xmlChar *) "line") == 0)
      {
        if (!processLine(reader, pc)) goto error;
      }
      else
      {
        m_error = "Unknown element \"";
        m_error += (const char *) name;  // libxml2 uses utf8 strings, so this should work
        m_error += "\"";
        goto error;
      }

      xmlFree(name);
    }
  }

  /* deallocate reader */
  xmlFreeTextReader(reader);

  /* report parsing result */
  if (ret != 0)
  {
    m_error = "Failed to parse file \"";
    m_error += m_filename;
    m_error += "\"";
    return false;
  }

  return true;

error:
  xmlFree(name);
  xmlFreeTextReader(reader);

  return false;
}


/**
 */
bool CXMLPointCloudRW::write(CPointCloud *pc)
{
  HOLOREN_UNUSED(pc);
  return false;
}


/**
 */
bool CXMLPointCloudRW::processPoint(xmlTextReaderPtr reader, CPointCloud *pc)
{
  Geometry::SPoint3D pt(0.0f, 0.0f, 0.0f);
  xmlChar *value = NULL;

  /* covert x attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "x");
  if (!Utils::strToFP((const char *) value, &pt.x))
  {
    m_error = "Failed to convert attribute x of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert y attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "y");
  if (!Utils::strToFP((const char *) value, &pt.y))
  {
    m_error = "Failed to convert attribute y of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert z attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "z");
  if (!Utils::strToFP((const char *) value, &pt.z))
  {
    m_error = "Failed to convert attribute z of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* print attributes for debug purposes */
  DBG("*** POINT ***");
  DBG("pt.x == " << pt.x);
  DBG("pt.y == " << pt.y);
  DBG("pt.z == " << pt.z);

  /* add the point to the scene */
  pc->addPointSource(pt);

  return true;

error:
  xmlFree(value);
  return false;
}


/**
 */
bool CXMLPointCloudRW::processLine(xmlTextReaderPtr reader, CPointCloud *pc)
{
  Geometry::SLine line(Geometry::SPoint3D(0.0f, 0.0f, 0.0f),
                       Geometry::SPoint3D(0.0f, 0.0f, 0.0f));
  xmlChar *value = NULL;

  /* covert x1 attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "x1");
  if (!Utils::strToFP((const char *) value, &line.p1.x))
  {
    m_error = "Failed to convert attribute x1 of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert y1 attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "y1");
  if (!Utils::strToFP((const char *) value, &line.p1.y))
  {
    m_error = "Failed to convert attribute y of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert z1 attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "z1");
  if (!Utils::strToFP((const char *) value, &line.p1.z))
  {
    m_error = "Failed to convert attribute z1 of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert x2 atribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "x2");
  if (!Utils::strToFP((const char *) value, &line.p2.x))
  {
    m_error = "Failed to convert attribute x2 of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert y2 attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "y2");
  if (!Utils::strToFP((const char *) value, &line.p2.y))
  {
    m_error = "Failed to convert attribute y2 of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert z2 attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "z2");
  if (!Utils::strToFP((const char *) value, &line.p2.z))
  {
    m_error = "Failed to convert attribute z2 of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* print attributes for debug purposes */
  DBG("*** LINE ***");
  DBG("line.p1.x == " << line.p1.x);
  DBG("line.p1.y == " << line.p1.y);
  DBG("line.p1.z == " << line.p1.z);
  DBG("line.p2.x == " << line.p2.x);
  DBG("line.p2.y == " << line.p2.y);
  DBG("line.p2.z == " << line.p2.z);

  /* add the line to the scene */
  sample(line, pc);

  return true;

error:
  xmlFree(value);
  return false;
}


/**
 */
bool CXMLPointCloudRW::processCuboid(xmlTextReaderPtr reader, CPointCloud *pc)
{
  Geometry::SLine line;
  Geometry::SPoint3D o(0.0f, 0.0f, 0.0f);
  tFPType w(0.0f);
  tFPType h(0.0f);
  tFPType d(0.0f);
  xmlChar *value = NULL;
  
  /* covert x attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "x");
  if (!Utils::strToFP((const char *) value, &o.x))
  {
    m_error = "Failed to convert attribute x of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert y attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "y");
  if (!Utils::strToFP((const char *) value, &o.y))
  {
    m_error = "Failed to convert attribute y of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert z attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "z");
  if (!Utils::strToFP((const char *) value, &o.z))
  {
    m_error = "Failed to convert attribute z of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert width attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "width");
  if (!Utils::strToFP((const char *) value, &w))
  {
    m_error = "Failed to convert attribute z of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert height attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "height");
  if (!Utils::strToFP((const char *) value, &h))
  {
    m_error = "Failed to convert attribute z of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* convert z attribute */
  value = xmlTextReaderGetAttribute(reader, (xmlChar *) "depth");
  if (!Utils::strToFP((const char *) value, &d))
  {
    m_error = "Failed to convert attribute z of cuboid declaration";
    goto error;
  }

  xmlFree(value);

  /* print attributes for debug purposes */
  DBG("*** CUBOID ***");
  DBG("x      == " << o.x);
  DBG("y      == " << o.y);
  DBG("z      == " << o.z);
  DBG("width  == " << w);
  DBG("height == " << h);
  DBG("depth  == " << d);

  /* convert cuboid to lines */
  for (unsigned int i = 0; i < 4; ++i)
  {
    /* vertical line */
    line.p1 = Geometry::SPoint3D(o.x,     o.y + h * (i >> 1), o.z + d * (i & 1));
    line.p2 = Geometry::SPoint3D(o.x + w, o.y + h * (i >> 1), o.z + d * (i & 1));
    sample(line, pc);

    /* horizontal line */
    line.p1 = Geometry::SPoint3D(o.x + w * (i >> 1), o.y,     o.z + d * (i & 1));
    line.p2 = Geometry::SPoint3D(o.x + w * (i >> 1), o.y + h, o.z + d * (i & 1));
    sample(line, pc);
    
    /* a line along the depth */
    line.p1 = Geometry::SPoint3D(o.x + w * (i >> 1), o.y + h * (i & 1), o.z);
    line.p2 = Geometry::SPoint3D(o.x + w * (i >> 1), o.y + h * (i & 1), o.z + d);
    sample(line, pc);
  }

  return true;

error:
  xmlFree(value);
  return false;
}
