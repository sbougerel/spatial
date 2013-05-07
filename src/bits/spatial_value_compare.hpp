// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_value_compare.hpp
 *  @brief  Define the ValueCompare class.
 */

#ifndef SPATIAL_VALUE_COMPARE_HPP
#define SPATIAL_VALUE_COMPARE_HPP

namespace spatial
{
  namespace details
  {
    /**
     *  Value compare functor for container storing pairs of (Key, Value) types,
     *  such as in \ref pointmap, \ref boxmap, etc. These container provide a
     *  \c key_compare functor type that is being used for the comparison of the
     *  value.
     *
     *  In \ref pointmap, \ref boxmap and other containers, the value type
     *  differs from the key type. Value type is a pair of key type and mapped
     *  type. The \c KeyCompare functor, provided to the container is reused
     *  to compare the value by using the first element of each value which is
     *  the key.
     *
     *  \tparam Value A ::std::pair of key and value type.
     *  \tparam KeyCompare A type that is a model of \ref RegularComparison.
     */
    template <typename Value, typename KeyCompare>
    struct ValueCompare : private KeyCompare
    {
      //! Comparator being initilized with a specific key comparison.
      ValueCompare(const KeyCompare& x) : KeyCompare(x) { }

      //! Unintialized comparator.
      ValueCompare() : KeyCompare() { }

      /**
       *  Compare 2 values \c a and \c b with the comparison operator provided
       *  by \c KeyCompare.
       *
       *  \param dim The dimension of comparison.
       *  \param a The left value to compare.
       *  \param b The right value to compare.
       *  \return The returned value is equivalent to <tt>KeyCompare()(a, b)</tt>.
       */
      bool operator()(dimension_type dim, const Value& a, const Value& b) const
      {
        return KeyCompare::operator()(dim, a.first, b.first);
      }
    };
  } // namespace details
} // namespace spatial


#endif // SPATIAL_VALUE_COMPARE_HPP
