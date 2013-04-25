/**
 */

#include "global.h"
#include "Utils.h"

#include <cstdlib>
#include <cerrno>


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

} /// End of namespace Utils
