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
 */

#include "global.h"
#include "Utils.h"

#include <cstdlib>
#include <cerrno>
#include <ctime>


namespace Utils {

/**
 */
bool strToFP(const char *str, tFPType *res)
{
  errno = 0;
  char *endptr = NULL;

#if defined(USE_DOUBLE) || defined(HOLOREN_CC_MSVC)
  *res = ::strtod(str, &endptr);
#else
  *res = ::strtof(str, &endptr);
#endif

  if ((errno != 0) || (*endptr != 0) || (endptr == str))
  {
    return false;
  }

  return true;
}


/**
 */
bool strToDbl(const char *str, double *res)
{
  errno = 0;
  char *endptr = NULL;

  *res = ::strtod(str, &endptr);
  if ((errno != 0) || (*endptr != 0) || (endptr == str))
  {
    return false;
  }

  return true;
}


/**
 */
bool strToUInt(const char *str, unsigned int *res)
{
  errno = 0;
  char *endptr = NULL;

  *res = ::strtoul(str, &endptr, 10);
  if ((errno != 0) || (*endptr != 0) || (endptr == str))
  {
    return false;
  }

  return true;
}


/**
 */
bool strToULong(const char *str, unsigned long *res)
{
  errno = 0;
  char *endptr = NULL;

  *res = ::strtoul(str, &endptr, 10);
  if ((errno != 0) || (*endptr != 0) || (endptr == str))
  {
    return false;
  }

  return true;
}


/**
 */
const char *strSetEnd(const char *str, const char *set)
{
  if ((str == NULL) || (set == NULL))
  {
    return NULL;
  }

  while (*set)
  {
    const char *found = ::strrchr(str, *set);
    if (found)
    {
      return found;
    }
    ++set;
  }

  return NULL;
}


/**
 */
const char *getFileName(const char *path)
{
  if ((path == NULL) || (*path == 0))
  {
    return NULL;
  }

#ifdef HOLOREN_OS_WIN
  return strSetEnd(path, "\\/");
#else
  return ::strrchr(path, '/');
#endif
}


/**
 */
const char *getFileName(const std::string & path)
{
#ifdef HOLOREN_OS_WIN
  size_t pos = path.find_last_of("\\/");
#else
  size_t pos = path.rfind('/');
#endif
  return (pos == std::string::npos) ? (NULL) : (&path.front() + pos);
}


/**
 */
const char *fmtDateTime(const char *fmt, time_t time, bool utc)
{
  HOLOREN_ASSERT(fmt != NULL);

  static char buf[256];
  struct tm *broken_down_time;

  if (utc)
  {
    broken_down_time = gmtime(&time);
  }
  else
  {
    broken_down_time = localtime(&time);
  }

  if (broken_down_time == NULL)
  {
    return "";
  }

  errno = 0;
  size_t len = strftime(buf, sizeof(buf), fmt, broken_down_time);
  if ((len == 0) && (errno != 0))
  {
    return "";
  }

  return buf;
}

} /// End of namespace Utils
