// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file spatial_import_array.hpp Contains the macro to pull certain
 *  "array" dependancies into the spatial::import namespace.
 *
 *  Depending on the library support and compiler version, the array
 *  headers can only be found in TR1 namespace (MSVC 2010 and glibc++ without
 *  c++11 support), only be found in STD name space (libc++), or both (MSVC2012
 *  and glibc++ with C++11 support). This file is written to import these types,
 *  regardless of the library being used, into the spatial::import namespace.
 */

#ifndef SPATIAL_IMPORT_ARRAY
#define SPATIAL_IMPORT_ARRAY

#ifdef SPATIAL_ARRAY_NAMESPACE
#  undef SPATIAL_ARRAY_NAMESPACE
#endif

#if defined(__LIBCPP_VERSION) || __cplusplus >= 201103L
#  include <array>
#  define SPATIAL_ARRAY_NAMESPACE std
#elif defined(__GLIBCXX__)
#  include <tr1/array>
#  define SPATIAL_ARRAY_NAMESPACE std::tr1
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <array>
#  define SPATIAL_ARRAY_NAMESPACE std::tr1
#endif

namespace spatial
{
  namespace import
  {
    using SPATIAL_ARRAY_NAMESPACE::array;
  }
}

#endif // SPATIAL_IMPORT_ARRAY
