// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_compare_builtin.hpp
 *  @brief  Defines the set of meta-programing types to find if a comparator is
 *  built-in or not.
 */

#ifndef SPATIAL_COMPARE_BUILTIN_HPP
#define SPATIAL_COMPARE_BUILTIN_HPP

#include "spatial_pull_tr1.hpp"

namespace spatial
{
  // Forward declarations for builtin compartors
  template<typename Tp> struct bracket_less;
  template<typename Tp> struct paren_less;
  template<typename Tp> struct iterator_less;
  template<typename Accessor, typename Tp> struct accessor_less;

  namespace details
  {
    /**
     * Help to resolve whether the type used is a builtin comparator or not. It
     * is used as the base type of \ref is_compare_builtin.
     */
    //@{
    template <typename>
    struct is_compare_builtin_helper : std::tr1::false_type { };
    template <typename Tp>
    struct is_compare_builtin_helper<bracket_less<Tp> >
      : std::tr1::true_type { };
    template <typename Tp>
    struct is_compare_builtin_helper<paren_less<Tp> >
      : std::tr1::true_type { };
    template <typename Tp>
    struct is_compare_builtin_helper<iterator_less<Tp> >
      : std::tr1::true_type { };
    template <typename Accessor, typename Tp>
    struct is_compare_builtin_helper<accessor_less<Accessor, Tp> >
      : std::tr1::true_type { };
    //@}

    /**
     *  Statically resolve if key_compare used in the container \c corresponds
     *  to one of the builtin library comparators or not. Designed to be used
     *  with \ref enable_if.
     */
    template <typename Ct>
    struct is_compare_builtin
      : is_compare_builtin_helper<typename container_traits<Ct>::key_compare>
    { };
  }
}

#endif // SPATIAL_COMPARE_BUILTIN_HPP
