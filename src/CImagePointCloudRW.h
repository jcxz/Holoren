/**
 * An abstract base class for reading and writing point clouds
 */

#ifndef CIMAGEPOINTCLOUDRW_H
#define CIMAGEPOINTCLOUDRW_H

#include "CBasePointCloudRW.h"


struct SDL_Surface;

/**
 * A base class providing interface
 * for readng and writing point clouds
 */
class CImagePointCloudRW : public CBasePointCloudRW
{
  public:
    /**
     * Constructor
     */
    explicit CImagePointCloudRW(const std::string & filename = std::string())
      : CBasePointCloudRW(filename)
    {
    }

    /**
     * Destructor
     */
    virtual ~CImagePointCloudRW(void)
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
     * A method to convert the given SDL surface if necessary
     */
    bool toRGBASurface(SDL_Surface **surf);

    /**
     * A method to convert surface to greyscale
     */
    bool toGreyScale(SDL_Surface *surf);
    
    /**
     * A method to convert the surface to binary.
     * Method works in place and assumes that the surface was previously
     * converted to greyscale with toGreyScale
     */
    bool toBinary(SDL_Surface *surf, uint8_t treshold);

    /**
     * A method to compute the average intesity of grey color
     * in the image
     */
    bool getAvgIntensity(SDL_Surface *surf, uint8_t *intensity);

    /**
     * A method to generate point sources from a given greyscale
     * image by utilising the given treshold
     */
    bool genPointSources(SDL_Surface *surf, uint8_t treshold, tFPType z, double sampling, CPointCloud *pc);
};

#endif
