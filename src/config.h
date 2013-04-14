/**
 * This file defines the compile time program configuration
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Portable types */
#include <stdint.h>

/* define the basic floating point type that will be used */
#ifdef USE_DOUBLE
typedef double tFPType;
#else
typedef float tFPType;
#endif

#endif
