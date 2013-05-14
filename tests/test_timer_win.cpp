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
 * A project to do some simple tests of the CTimer class
 */

/* Make sure that this test will work even in case
   this macro is not defined by compiler */
#ifndef HOLOREN_DEBUG_PERF
#  define HOLOREN_DEBUG_PERF
#endif

#include "CTimer.h"

#include <cstdlib>
#include <ctime>


/**
 * A function to return a random number from a given itnerval
 */
static unsigned int random(unsigned int low, unsigned int high)
{
  /* convert the value returned from rand to <0, 1> and then adjust the range */
  return (double(std::rand()) / RAND_MAX) * high + low;
}



int main(void)
{
  CTimer timer;

  /* initialize random unmber generator */
  std::srand(time(NULL));

  /* sleep for 1 second */
  DBG_T_START(timer, "Waiting for 1 second:\n");

  ::Sleep(1000);

  DBG_T_END(timer, "Real time elapsed: ");

  /* sleep for a random time of 1 or 2 seconds */
  unsigned t = random(1000, 2000);
  DBG_T_START(timer, "Waiting for " << t << " miliseconds:\n");

  ::Sleep(t);

  DBG_T_END(timer, "Real time elapsed: ");


  /* sleep for 10 seconds */
  DBG_T_START(timer, "Waiting 5 seconds:\n");

  ::Sleep(5000);

  DBG_T_END(timer, "Real time elapsed: ");

  return 0;
}
