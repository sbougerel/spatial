// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  Contains the definition of many utilities used across the entire library but
 *  that should not generally be manipulated by the end users of the
 *  library. All utilities are declared within the ::spatial::details namespace.
 *
 *  \file   spatial_details.hpp
 */

#ifndef SPATIAL_DETAILS_HPP
#define SPATIAL_DETAILS_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <algorithm> // provides: ::std::swap
#include <utility> // for std::pair

// provides: ::std::tr1::is_empty, std::tr1::false_type and std::tr1::true_type
#ifdef __GLIBCXX__
#  include <tr1/type_traits>
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <type_traits>
#endif

namespace spatial
{
  /**
   *  Defines the namespace that contains all implementation-level
   *  utilities needed by the component of the library.
   *
   *  The types and functions defined within this namespace should not normally
   *  be needed by end-users of the library. If you are currently using one of
   *  them, please refer to the documentation for these functions or object as
   *  there may be more appropriate functions for you to use.
   *
   *  If you do not find a more appropriate function to use, then you may
   *  suggest a feature improvement to the library author.
   */
  namespace details
  {
    //@{
    /*
     *  A type trait that choses between 2 types
     */
    template<bool, typename Tp1, typename Tp2>
    struct condition
    { typedef Tp1 type; };
    template<typename Tp1, typename Tp2>
    struct condition<false, Tp1, Tp2>
    { typedef Tp2 type; };
    //@}

    /**
     *  Uses the empty base class optimization in order to compress a
     *  potentially empty base class with a member of a class.
     *
     *  Provide the \c base() function to access the base class. Provide the
     *  \c operator() to quickly access the stored member.
     *  \tparam Base The base class that will be compressed through empty member
     *  optimization by the compiler, hopefully.
     *  \tparam Member The member class that will be used as a value for the
     *  compression, this class should not be an empty member class.
     */
    template<typename Base, typename Member>
    struct Compress
      : private Base // Empty member optimization
    {
      //! Uninitialized compressed base.
      Compress() { }

      //! Compressed member with uninitialized base.
      //! \param member The value of the \c Member type.
      Compress(const Member& member)
        : Base(), member_(member) { }

      //!
      Compress(const Base& compressed_base, const Member& member)
        : Base(compressed_base), member_(member) { }

      //@{
      /**
       *  Accessor to the base class.
       */
      const Base&
      base() const
      { return *static_cast<const Base*>(this); }

      Base&
      base()
      { return *static_cast<Base*>(this); }
      //@}

      //@{
      /**
       *  Quick accessor to the member.
       */
      const Member&
      operator()() const
      { return member_; }

      Member&
      operator()()
      { return member_; }
      //@}

    private:
      //! Storage for the member value.
      Member member_;
    };

    /**
     *  The dimension value is set by a template value, thus consuming
     *  no memory.
     *
     *  \tparam The dimension of the rank.
     */
    template <dimension_type Value>
    struct Static_rank
    {
      //! Returns the dimension for the rank specified in the template parameter
      //! \c Value.
      dimension_type operator()() const
      { return Value; }
    };

    /**
     *  The dimension value is stored by a member of the object, but can be
     *  modified at run time.
     */
    struct Dynamic_rank
    {
      //! Returns the dimension for the rank stored in \c rank_
      dimension_type operator()() const
      { return rank_; }

      //! Build a rank with a default dimension of 1.
      //! \param rank The specified rank dimension.
      explicit
      Dynamic_rank(dimension_type rank = 1)
        : rank_(rank)
      { }

    private:
      //! The value that stores the rank dimension.
      dimension_type rank_;
    };

    //@{
    /**
     *  @brief  Perform a specialized assign for empty classes.
     */
    template<bool, typename Tp>
    struct template_member_assign_provider
    {
      static void do_it(Tp& left, const Tp& right)
      { left = right; }
    };

    template<typename Tp>
    struct template_member_assign_provider<true, Tp>
    { static void do_it(Tp&, const Tp&) { } };

    template <typename Tp>
    struct template_member_assign
      : template_member_assign_provider<std::tr1::is_empty<Tp>::value, Tp>
    { };
    //@}

    //@{
    /**
     *  @brief  Perform a specialized swap for empty classes.
     */
    template<bool, typename Tp>
    struct template_member_swap_provider
    {
      static void do_it(Tp& left, Tp& right)
      {
        using ::std::swap;
        swap(left, right);
      }
    };

    template<typename Tp>
    struct template_member_swap_provider<true, Tp>
    { static void do_it(Tp&, Tp&) { } };

    template <typename Tp>
    struct template_member_swap
      : template_member_swap_provider<std::tr1::is_empty<Tp>::value, Tp>
    { };
    //@}

    /**
     *  Increment dimension \c node_dim, given \c rank.
     *  \tparam Rank The model of \ref Rank used in this function.
     *  \param rank An object that is a model of \ref Rank.
     *  \param node_dim The value of the dimension for the node.
     */
    template<typename Rank>
    inline dimension_type
    incr_dim(const Rank& rank, dimension_type node_dim)
    { return (node_dim + 1) % rank(); }

    /**
     *  Decrement dimension \c node_dim, given \c rank.
     *  \tparam Rank The model of \ref Rank used in this function.
     *  \param rank An object that is a model of \ref Rank.
     *  \param node_dim The value of the dimension for the node.
     */
    template<typename Rank>
    inline dimension_type
    decr_dim(const Rank& rank, dimension_type node_dim)
    { return node_dim ? node_dim - 1 : rank() - 1; }

    /**
     *  Return true if \c x coordinate is less than \c y coordinate over
     *  dimension \c node_dim, given \c compare. If both coordinate are equal,
     *  then return true if address of \c x is less than address of \c y.
     *
     *  This operator always discriminate \c x and \c y unless they are
     *  precisely the same object.
     *  \tparam Key The key object to use in the comparison.
     *  \tparam Compare The comparison object that is a model of
     *  \ref TrivialComparison.
     */
    template <typename Key, typename Compare>
    inline bool
    less_by_ref(const Compare& compare, dimension_type node_dim,
                const Key& x, const Key& y)
    {
      return (compare(node_dim, x, y)
              || (&x < &y && !compare(node_dim, y, x)));
    }

    /**
     *  Return a boolean indicating whether all of \c key's coordinates are
     *  within range or not.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_all(const Rank& rank, const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        {
          if (predicate(i, key, rank()) != matching)
            { return false; }
        }
      return true;
    }

    /**
     *  Return a boolean indicating whether one of \c key's coordinates is
     *  within range or not.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_any(const Rank& rank, const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        {
          if (predicate(i, key, rank()) == matching)
            { return true; }
        }
      return false;
    }

    /**
     *  Return a boolean indicating whether all of \c key's coordinates are
     *  within range or not, except for \c exlude_dim which is left out of the
     *  test.
     *
     *  The key is simply tested across all dimesions over the predicate.
     *  \tparam Rank A type that is a model of \ref Rank.
     *  \tparam Key The key type that is used in the comparison.
     *  \tparam Predicate A type that is a model of \ref RangePredicate.
     *  \param rank The object of type \c Rank.
     *  \param exclude_dim The dimension that will be skipped during the test.
     *  \param key The key whose coordinates are verified to be within the
     *  range.
     *  \param predicate The \ref RangePredicate object used to represent the
     *  range.
     */
    template <typename Rank, typename Key, typename Predicate>
    inline bool
    match_most(const Rank& rank, dimension_type exclude_dim,
               const Key& key, const Predicate& predicate)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        {
          if (i != exclude_dim && predicate(i, key, rank()) != matching)
            { return false; }
        }
      return true;
    }

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
       *  \param a The left value to compare.
       *  \param b The right value to compare.
       *  \return The returned value is equivalent to <tt>KeyCompare()(a, b)</tt>.
       */
      bool operator()(const Value& a, const Value& b) const
      {
        return KeyCompare::operator()(a.first, b.first);
      }
    };

  } // namespace details

} // namespace spatial

#endif // SPATIAL_DETAILS_HPP
