/**
 * A project to do some simple tests of the CTimer class
 */

/* Make sure that this test will work even in case 
   this macro is not defined by compiler */
#ifndef HOLOREN_DEBUG_PERF
#  define HOLOREN_DEBUG_PERF
#endif

#include "CTimer.h"



int main(void)
{
  CTimer timer;

  /* sleep for 1 second */
  DBG_T_START(timer, "Waiting for 1 second:\n");

  ::Sleep(1000);

  DBG_T_END(timer, "Real time elapsed: ");


  /* sleep for 10 seconds */
  DBG_T_START(timer, "Waiting 5 seconds:\n");

  ::Sleep(5000);

  DBG_T_END(timer, "Real time elapsed: ");

  return 0;
}
