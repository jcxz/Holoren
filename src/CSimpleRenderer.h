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
};

#endif
