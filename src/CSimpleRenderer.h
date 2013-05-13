/**
 * Simple SW rendering engine
 */

#ifndef CSIMPLERENDERER_H
#define CSIMPLERENDERER_H

#include "CBaseRenderer.h"


/**
 */
class CSimpleRenderer : public CBaseRenderer
{
  public:
    explicit CSimpleRenderer(double hologram_z = 0)
      : CBaseRenderer(hologram_z)
    {
    }

    /* implement the IRenderer interface */
    virtual bool open(const char *filename = NULL);
    virtual bool close(void);
    virtual std::string getError(void) const;
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of);
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of);

  private:
    /* disable assignment, copy and move
       Though this is not technically necessary for this class,
       it is to keep it consistent with OpenCL renderer */
    CSimpleRenderer(const CSimpleRenderer & );
    HOLOREN_DECL_MOVE_CONSTRUCTOR(CSimpleRenderer);
    CSimpleRenderer & operator=(const CSimpleRenderer & );
};

#endif
