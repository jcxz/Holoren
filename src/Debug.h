/**
 * Debugging functionality
 *
 * @author Matus Fedorko (xfedor01)
 * @date 14.04.2013
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

#include <iostream>

/** A macro for assert statements */
#include <cassert>
#define HOLOREN_ASSERT(x) assert(x)

/** A debugging macro */
#ifdef HOLOREN_DEBUG
#  define DBG(x) std::cerr << x << std::endl
#  define DBGHEX(data, len) Debug::hexdump(std::cerr, data, len)
#else
#  define DBG(x) ((void) (0))
#  define DBGHEX(data, len)
#endif

/** A macro to print diagnostic messages */
#ifndef HOLOREN_NO_WARNINGS
#  define WARN(x) std::cerr << x << std::endl
#else
#  define WARN(x)
#endif

/** Macros to identify a location in code */
#ifdef HOLOREN_DEBUG
#  define DBGLOC() \
     std::cerr << Debug::detail::formatCodeLoc(HOLOREN_FILE, \
                                               HOLOREN_LINE, \
                                               HOLOREN_FUNC_PLAIN) \
               << std::endl
#  define DBGSTRLOC() \
     Debug::detail::formatCodeLoc(HOLOREN_FILE, \
                                  HOLOREN_LINE, \
                                  HOLOREN_FUNC_PLAIN)
#else
#  define DBGLOC()
#  define DBGSTRLOC() ""
#endif


namespace Debug {

/** A namespace with private helper functions, you should not use functions in this namespace */
namespace detail {
  
/**
 * This function will print a formatted code location
 */
std::string formatCodeLoc(const char *filepath,
                          int line_num,
                          const char *func_name);

}  // End of namespace detail


/**
 * This method will print binary data in human readable format.
 * First a line offset is printed, then a hex dump of binary data and
 * finally an ascii dump of those data.
 *
 * @param os an output stream
 * @param *data the data to be printed
 * @param len the length of the data
 * @param line_width width of the binary dump (e.g. how many bytes
 *                   will be printed on a single line)
 *
 * @return a reference to the output passed in as the first argument.
 *         This is to allow chaining these function calls.
 */
std::ostream & hexdump(std::ostream & os,
                       const void *data,
                       uint32_t len,
                       uint32_t line_width = 16);

} // End of namespace Debug

#endif
