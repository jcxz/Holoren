/**
 * Various free helper functions
 */

#ifndef UTILS_H
#define UTILS_H

#include "config.h"

#include <cstring>


namespace Utils {

/**
 * The function will convert a given string
 * to a floating point number. The string must
 * be entirely convertible (i.e. only composed
 * of digits optionally separated by a floating
 * point separator or a number in scientific e-notation)
 *
 * @param str the string to be converted
 * @param res the result of conversion (undefined on error)
 *
 * @return true on success, false on error
 */
bool strToFP(const char *str, tFPType *res);

/**
 * The function will convert a given string
 * to a double precision floating point number.
 * The string must be entirely convertible (i.e. only composed
 * of digits optionally separated by a floating
 * point separator or a number in scientific e-notation)
 *
 * @param str the string to be converted
 * @param res the result of conversion (undefined on error)
 *
 * @return true on success, false on error
 */
bool strToDbl(const char *str, double *res);

/**
 * This function will convert the the given string
 * to an unsigned integer. The string must be entirely
 * convertible (i.e. composed of numbers from start to finish,
 * including an optional initial sign)
 *
 * @param str the string to be converted
 * @param res a pointer to variable where the result will be saved
 *
 * @reutrn true on success, false on error
 */
bool strToUInt(const char *str, unsigned int *res);

/**
 * A function to do a case insensitive compare of two c-like strings
 */
inline int strCaseCmp(const char *str1, const char *str2)
{
#ifdef HOLOREN_CC_MSVC
  return stricmp(str1, str2);
#else
  return strcasecmp(str1, str2);
#endif
}

} // End of namespace Utils

#endif
