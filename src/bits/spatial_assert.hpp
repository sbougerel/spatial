// -*- C++ -*-

/**
 *  @file   spatial_assert.hpp
 *  @brief  Provide a smiliar functionality than cassert, except that
 *  SPATIAL_ENABLE_ASSERT must be defined to enable it, by default, no spatial
 *  assertion check is performed.
 *
 *  This feature is built in the library for the sole purpose of the library
 *  developers, therefore it is encouraged that no one else but the library
 *  developers uses it. It is used during unit testing and debugging.
 *
 *  Change log:
 *
 *  - 31-12-2009 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - 12-11-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Adding few more feature such as the possibility to reset assertion by
 *    including the file again.
 *
 */

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#ifndef SPATIAL_ASSERT_HPP_DISPLAY
#  define SPATIAL_ASSERT_HPP_DISPLAY
#  include<cstdlib>
#  include<iostream>
namespace spatial
{
  namespace assert
  {
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
#endif

#ifdef SPATIAL_ASSERT_HPP
# undef SPATIAL_ASSERT_HPP
#endif

#define SPATIAL_ASSERT_HPP
#ifndef SPATIAL_ENABLE_ASSERT
#  define SPATIAL_ASSERT_CHECK(expr)     static_cast<void>(0)
#else
#  define SPATIAL_ASSERT_CHECK(expr)                                    \
  ((expr)                                                               \
   ? static_cast<void>(0)                                               \
   : spatial::assert::assert_fail(#expr, __FILE__, __LINE__))
#endif
