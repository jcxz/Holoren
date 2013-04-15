/** 
 * Header file defining timing utilities
 */

#ifndef CTIMER_H
#define CTIMER_H

#include "global.h"

#include <iostream>

#if defined(HOLOREN_OS_WIN)
#  define WIN32_LEAN_AND_MEAN  // do not compile unnecessary bloat
#  include <Windows.h>
typedef class CTimerWindows CTimer;
#elif defined(HOLOREN_OS_LINUX)
#  include <ctime>
typedef class CTimerUnix CTimer;
#else
#  ifdef HOLOREN_CC_GCC
#    warning No supported timing API on this platform
#  endif
typedef class CTimerDummy CTimer;
#endif


/**
 * A Windows performance counter utilizing Windows's QueryPerformanceCounter() function
 */
#ifdef HOLOREN_OS_WIN
class CTimerWindows
{
  public:
    /**
     * Default constructor
     */
    CTimerWindows(void)
      : m_start(),
        m_end(),
        m_frekv(1.0f)  // 1.0f to avoid division by zero on QueryPerformanceFrequency failure
    {
      LARGE_INTEGER f;
      if (!::QueryPerformanceFrequency(&f))
      {
        WARN("QueryPerformanceFrequency failed: high resolution timer not available on this platform, you might get incorrect results");
        return;
      }

      // convert frequency from counts per second to counts per milisecond
      m_frekv = double(f.QuadPart) / 1000.0f;
    }

    /**
     * A method to fire performance counter
     */
    inline bool start(void)
    {
      return ::QueryPerformanceCounter(&m_start);
    }

    /**
     * A method to stop performance counter
     */
    inline bool stop(void)
    {
      return ::QueryPerformanceCounter(&m_end);
    }

    /**
     * A function to return the time in miliseconds elapsed between
     * the calls to start and stop
     */
    inline double getElapsedTime(void) const
    {
      return (m_end.QuadPart - m_start.QuadPart) / m_frekv;
    }

  private:
    LARGE_INTEGER m_start;   /// the start clock cycle counter
    LARGE_INTEGER m_end;     /// end clock cycle counter
    double m_frekv;          /// performance counter frequency
};
#endif


/**
 * Linex (Unix) performance counter
 */
#ifdef HOLOREN_OS_LINUX
class CTimerUnix
{
  public:
    /**
     * A method to fire performance counter
     */
    inline bool start(void)
    {
      return (::clock_gettime(CLOCK_REALTIME, &m_start) == 0);
    }

    /**
     * A method to stop performance counter
     */
    inline bool stop(void)
    {
      return (::clock_gettime(CLOCK_REALTIME, &m_end) == 0);
    }

    /**
     * A function to return the time in miliseconds elapsed between
     * the calls to start and stop
     */
    inline double getElapsedTime(void) const
    {
      return (m_end.tv_sec - m_start.tv_sec) * 1000.0f +
             (m_end.tv_nsec - m_start.tv_nsec) * 1e-6;
    }

  private:
    struct timespec m_start;   /// start time
    struct timespec m_end;     /// end time
};
#endif


/**
 * A dummy timer used on platforms that lack supported API-s
 */
class CTimerDummy
{
  public:
    /**
     * Default constructor
     */
    CTimerDummy(void)
    {
      WARN("Using dummy timer: No real supported timing API on this platform");
    }

    /**
     * A method to fire performance counter
     */
    inline bool start(void)
    {
      return false;
    }

    /**
     * A method to stop performance counter
     */
    inline bool stop(void)
    {
      return false;
    }

    /**
     * A function to return the time in miliseconds elapsed between
     * the calls to start and stop
     */
    inline double getElapsedTime(void) const
    {
      return 0.0f;
    }
};


/**
 * A function to output elapsed time meassured by timer
 *
 * @param os an output stream
 * @param timer the class capable of meassuring time
 *
 * @return the output stream originally passed in
 */
inline std::ostream & operator<<(std::ostream & os, const CTimer & timer)
{
  return os << "Elapsed time: " << timer.getElapsedTime() << " ms";
}


/**
 * Define a set of performance timing functions when
 * HOLOREN_DEBUG_PERF preprocessor macro is enabled
 */
#ifdef HOLOREN_DEBUG_PERF
#  define DBG_T_START(timer, msg) \
     do { \
       std::cerr << msg; \
       if (!timer.start()) \
       { \
         WARN("Failed to start timer"); \
       } \
     } while (0)

#  define DBG_T_END(timer, msg) \
     do { \
       std::cerr << msg; \
       if (!timer.stop()) \
       { \
         WARN("Failed to stop timer"); \
       } \
       else \
       { \
         std::cerr << timer << std::endl; \
       } \
     } while (0)
#else
#  define DBG_T_START(timer, msg) \
     (void) (timer), (void) (msg)

#  define DBG_T_END(timer, msg) \
     (void) (timer), (void) (msg)
#endif

#endif
