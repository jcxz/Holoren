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
