/**
 * XML reading functionality
 */

#ifndef CXMLPOINTCLOUDRW_H
#define CXMLPOINTCLOUDRW_H

#include "CBasePointCloudRW.h"

#include <libxml/xmlreader.h>



/**
 * A class to load point cloud from XML
 */
class CXMLPointCloudRW : public CBasePointCloudRW
{
  public:
    /**
     * Constructor
     */
    explicit CXMLPointCloudRW(const std::string & filename = std::string())
      : CBasePointCloudRW(filename)
    {
    }

    /**
     * Destructor
     */
    virtual ~CXMLPointCloudRW(void)
    {
    }

    /**
     * A method which is called once before
     * using any instance of a given concrete
     * point cloud reader class.
     * This method can be used for things like
     * library initialisation.
     * Users should not directly access this method
     * it is meant to be used only by CPointCloudRWFactory
     */
    virtual bool init(tCleanUpHandler *p_handler);

    /**
     * A method to read a file which can be converted
     * to point cloud data structure
     */
    virtual bool read(CPointCloud *pc);

    /**
     * A method to write point cloud to a file.
     * If this method is not supported for a given
     * reader it should just return false.
     */
    virtual bool write(CPointCloud *pc);

  private:
    /**
     * Reads the optional transformations that can be specified for a geometric primitive.
     * These include rotation and scale. The rotation is allways done aroud object's bounding
     * box centre.
     */
    bool readTransformations(xmlTextReaderPtr reader, Geometry::SMatrix4D *m);

    bool processPoint(xmlTextReaderPtr reader, CPointCloud *pc);
    bool processLine(xmlTextReaderPtr reader, CPointCloud *pc);
    bool processCuboid(xmlTextReaderPtr reader, CPointCloud *pc);
};

#endif
