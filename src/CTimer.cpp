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
 * Implementation of timing functionality
 */

#include "global.h"
#include "CTimer.h"

#include <cmath>

/* a workaround for MSVC 2010 */
#ifdef HOLOREN_CC_MSVC2010
# define fmod(x, y) fmod(static_cast<double>((x)), static_cast<double>((y)))
#endif



/**
 */
std::ostream & operator<<(std::ostream & os, const CTimer & timer)
{ 
  double elapsed_time = timer.getElapsedTime();  // get elapsed microseconds

  /* make sure that the returned value is all right */
  if (elapsed_time <= 0.0f)
  {
    os << "Elapsed time: " << elapsed_time << "us";
    return os;
  }

  /* print elapsed times */
  os << "Elapsed time:";

  double microseconds = fmod(elapsed_time, 1000.0f);

  if (microseconds > 1)
  {
    elapsed_time /= 1000.0f;
    double miliseconds = fmod(elapsed_time, 1000.0f);

    if (miliseconds > 1)
    {
      elapsed_time /= 1000.0f;
      double seconds = fmod(elapsed_time, 60.0f);

      if (seconds > 1)
      {
        elapsed_time /= 60.0f;
        double minutes = fmod(elapsed_time, 60.0f);

        if (minutes > 1)
        {
          elapsed_time /= 60.0f;

          if (elapsed_time > 1)
          {
            os << ' ' << ((unsigned long long) (elapsed_time)) << "h";  // hours
          }
          
          os << ' ' << ((unsigned) (minutes)) << "min";
        }
        
        os << ' ' << ((unsigned) (seconds)) << "s";
      }
      
      os << ' ' << ((unsigned) (miliseconds)) << "ms";
    }

    os << ' ' << ((unsigned) (microseconds)) << "us";
  }

  return os;
}