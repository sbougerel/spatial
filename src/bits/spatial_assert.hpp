// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_assert.hpp
 *  @brief  Provide a smiliar functionality than \c cassert, except that
 *  SPATIAL_ENABLE_ASSERT must be defined to enable it, by default, no spatial
 *  assertion check is performed.
 *
 *  This feature is built in the library for the sole purpose of the library
 *  developers, therefore it is encouraged that no one else but the library
 *  developers uses it. It is used during unit testing and debugging.
 */

#ifdef SPATIAL_ENABLE_ASSERT
#  ifndef SPATIAL_ASSERT_HPP_ONCE
#    define SPATIAL_ASSERT_HPP_ONCE
#    include<cstdlib>
#    include<iostream>
namespace spatial
{
  /**
   *  This namespace is meant to isolate the assertion function, preventing that
   *  it conflicts with other functions unintended for that usage.
   */
  namespace assert
  {
    /**
     *  This function will call abort() (and therefore cause the program to stop
     *  with abnormal termination) and will print an error giving the cause of
     *  the failure.
     *
     *  This function is not meant to be used directly, rather, the
     *  \ref SPATIAL_ASSERT_CHECK macro is meant to be used instead:
     *  \code
     *  SPATIAL_ASSERT_CHECK(test == true);
     *  \endcode
     *
     *  If \c test in the file \c example.cpp above is found to be \c true, then
     *  the program carries with its execution. If \c is \c false, then the
     *  program will abort with abort with the following messages:
     *  \code
     *  Assertion failed (example.cpp: line 34): 'test == true'
     *  \endcode
     */
    inline void assert_fail
    (const char* msg, const char* filename, unsigned int line) throw()
    {
      try
        {
          std::cerr << std::endl
                    << "Assertion failed (" << filename << ":" << line
                    << "): '" << msg << "'" << std::endl;
        }
      catch (...) { }
      abort();
    }
  }
}
#  endif
#endif

#ifdef SPATIAL_ASSERT_CHECK
#  undef SPATIAL_ASSERT_CHECK
#endif

#ifndef SPATIAL_ENABLE_ASSERT
/**
 *  \def SPATIAL_ASSERT_CHECK(expr)
 *  Check that expression is true. If expression is false, the program will be
 *  aborted and the expression \c expr, along with the file name and the line
 *  where it occurs will be printed on the output of the program.
 *
 *  The macro is meant to be used this way in a program:
 *  \code
 *  SPATIAL_ASSERT_CHECK(test == true);
 *  \endcode
 *
 *  If \c test in the file \c example.cpp above is found to be \c true, then
 *  the program carries with its execution. If \c is \c false, then the
 *  program will abort with abort with the following messages:
 *  \code
 *  Assertion failed (example.cpp: line 34): 'test == true'
 *  \endcode
 *
 *  \param expr A test expression.
 */
#  define SPATIAL_ASSERT_CHECK(expr)     static_cast<void>(0)
#else
#  define SPATIAL_ASSERT_CHECK(expr)                                    \
  ((expr)                                                               \
   ? static_cast<void>(0)                                               \
   : ::spatial::assert::assert_fail(#expr, __FILE__, __LINE__))
#endif
