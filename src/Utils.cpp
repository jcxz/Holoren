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

} /// End of namespace Utils
