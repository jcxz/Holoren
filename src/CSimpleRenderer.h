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
