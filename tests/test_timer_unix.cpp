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

#include <unistd.h>   // ::sleep()



int main(void)
{
  CTimer t;

  /* sleep for 1 second */
  DBG_T_START(t, "Starting to test 1s interval:\n");

  ::sleep(1);

  DBG_T_END(t, "Real time meassured: ");


  /* sleep for 10 seconds */
  DBG_T_START(t, "Starting to meassure 5s interval:\n");

  ::sleep(5);

  DBG_T_END(t, "Real time meassured: ");

  return 0;
}
