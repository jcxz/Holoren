/**
 * Simple SW rendering engine
 */

#ifndef CSIMPLERENDERER_H
#define CSIMPLERENDERER_H

#include "IRenderer.h"


/**
 */
class CSimpleRenderer : public IRenderer
{
  public:
    explicit CSimpleRenderer(double hologram_z = 0)
      : m_hologram_z(hologram_z)
    {
    }

    void setHologramLocation(double z)
    {
      m_hologram_z = z;
      return;
    }

    /* implement the IRenderer interface */
    virtual bool open(void);
    virtual bool close(void);
    virtual std::string getError(void) const;
    virtual bool renderObjectWave(const CPointCloud & pc, COpticalField *of);
    virtual bool renderHologram(const CPointCloud & pc, COpticalField *of);


  private:
    double m_hologram_z;  /// hologram location on the z axis
};

#endif
