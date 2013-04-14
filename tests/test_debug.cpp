/**
 * A file to perform tests of debugging subsystem
 */

/* Make sure that this test will work even in case 
   these macros are not defined by compiler */
#ifndef HOLOREN_DEBUG
#  define HOLOREN_DEBUG
#endif

#ifndef HOLOREN_DEBUG_KERNEL
#  define HOLOREN_DEBUG_KERNEL
#endif

#include "Debug.h"

#include <iostream>
#include <cstring>



int main(void)
{
  const char *str1 = "Hello";  // a short string
  const char *str2 = "Hello, Darling. Yabada 89809\x90. Having some brain farts. Yikes";  // a long string

  Debug::hexdump(std::cout, str1, std::strlen(str1) + 1);
  std::cout << std::endl;

  Debug::hexdump(std::cout, NULL, 0);
  std::cout << std::endl;

  Debug::hexdump(std::cout, NULL, std::strlen(str1) + 1);   // this still should not crash the program
  std::cout << std::endl;

  Debug::hexdump(std::cout, str2, std::strlen(str2) + 1);
  std::cout << std::endl;

  Debug::hexdump(std::cout, str2, std::strlen(str2) + 1, 32);
  std::cout << std::endl;

  Debug::hexdump(std::cout, str2, std::strlen(str2) + 1, 20);
  std::cout << std::endl;

  Debug::hexdump(std::cout, str1, std::strlen(str1) + 1, 0);   // should not print anything
  std::cout << std::endl;

  Debug::hexdump(std::cout, str1, std::strlen(str1) + 1, 1);
  std::cout << std::endl;

  return 0;
}
