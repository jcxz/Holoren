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
