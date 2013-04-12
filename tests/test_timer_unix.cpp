/**
 * A project to do some simple tests of the CTimer class
 */

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
