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
 * Globally applicable definitions and declarations
 */

#ifndef GLOBAL_H
#define GLOBAL_H

/** OS detection */
#if defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(__bsdi__)   || \
    defined(__DragonFly__)
# define HOLOREN_OS_BSD
#elif defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
# define HOLOREN_OS_WIN
#elif defined(__linux__) || defined(linux) || defined(__linux)
# define HOLOREN_OS_LINUX
#else
# error Unsupported operating system
#endif

/** Compiler detection */
#if defined(_MSC_VER)
# define HOLOREN_CC_MSVC
# if (_MSC_VER >= 1600 && _MSC_VER < 1700)
#  define HOLOREN_CC_MSVC2010
# elif (_MSC_VER >= 1500 && _MSC_VER < 1600)
#  define HOLOREN_CC_MSVC2008
# endif
#elif defined(__GNUC__)
# define HOLOREN_CC_GCC
#else
# error Unsupported compiler
#endif

/**
 * Structure packing macro
 * @see http://stackoverflow.com/questions/1537964/visual-c-equivalent-of-gccs-attribute-packed
 */
#if defined(HOLOREN_CC_MSVC)
# define HOLOREN_PACKED_STRUCT(decl) __pragma(pack(push, 1)) decl __pragma(pack(pop))
#elif defined(HOLOREN_CC_GCC)
# define HOLOREN_PACKED_STRUCT(decl) decl __attribute__((packed))
#else
# define HOLOREN_PACKED_STRUCT(decl) decl
#endif

/** Define portable line, function, and file macros */
# define HOLOREN_FILE __FILE__
# define HOLOREN_LINE __LINE__
# define HOLOREN_FUNC_PLAIN __FUNCTION__
#if defined(HOLOREN_CC_MSVC)
# define HOLOREN_FUNC __FUNCSIG__
#elif defined(HOLOREN_CC_GCC)
# define HOLOREN_FUNC __PRETTY_FUNCTION__
#else
# define HOLOREN_FUNC __FUNCTION__
#endif

/** Silence noisy but otherwise useless MSVC warnings */
#if defined(HOLOREN_CC_MSVC)
# define _CRT_SECURE_NO_WARNINGS
#endif

/**
 * This macro can be used to declare c++11 move constructor,
 * which might be usefull for certain tools like Enterprise Architect,
 * which cannot process C++11 correctly
 *
 * @param cls the class name
 */
#define HOLOREN_DECL_MOVE_CONSTRUCTOR(cls) cls(cls &&)

/** A macro to designate unused parameters */
#define HOLOREN_UNUSED(x) ((void) (x))

/** include debugging support */
#include "Debug.h"

#endif
