// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_assign.hpp
 *  Defines the \ref assign() function.
 */

#include <utility>

#ifndef SPATIAL_ASSIGN_HPP
#define SPATIAL_ASSIGN_HPP

namespace spatial
{
  namespace details
  {
    template <typename Type1, typename Type2>
    void
    assign(Type1& first, Type2& second, std::pair<Type1, Type2> source)
    {
      first = source.first;
      second = source.second;
    }
  }
}

#endif // SPATIAL_ASSIGN_HPP
