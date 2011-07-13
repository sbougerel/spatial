// -*- C++ -*-

/**
 *  @file   spatial_details.hpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-09-10 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_DETAILS_HPP
#define SPATIAL_DETAILS_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <algorithm> // provides: std::swap
#include <cstddef> // int32_t, available on all platforms

// provides: std::tr1::is_empty
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
  namespace details
  {
    /**
     *  @brief  Uses the empty base class optimization in order to compress a
     *  potentially empty base class with a member of a class.
     *
     *  Provide the base() function to access the base class.
     *
     *  Provide the operator() has a quick accessor to member, and the
     *  possibility to cast the class into the Base class has an empty
     *  constructor.
     */
    template<typename Base, typename Member>
    struct Compress
      : private Base // Empty member optimization
    {
      Compress() { }

      Compress(const Member& member)
	: Base(), member_(member) { }

      Compress(const Base& compressed_base, const Member& member)
	: Base(compressed_base), member_(member) { }

      //@{
      /**
       *  @brief  Accessor to the base class.
       */
      const Base&
      base() const
      { return *static_cast<const Base*>(this); }

      Base&
      base()
      { return *static_cast<Base*>(this); }
      //@}

      /**
       *  @brief  Quick accessor to the member.
       *  @{
       */
      const Member&
      operator()() const
      { return member_; }

      Member&
      operator()()
      { return member_; }
      //@}

    private:
      Member member_;
    };

    /**
     *  @brief  The dimension value is set by a template value, thus consuming
     *  no memory.
     */
    template <dimension_type Value>
    struct Static_rank
    {
      dimension_type operator()() const
      { return Value; }
    };

    /**
     *  @brief  The dimension value is stored by a member of the object, but can
     *  be modified at run time.
     */
    struct Dynamic_rank
    {
      dimension_type operator()() const
      { return rank_; }

      explicit
      Dynamic_rank(dimension_type rank = 1)
	: rank_(rank)
      { }

    private:
      dimension_type rank_;
    };

    //@{
    /**
     *  @brief  Perform a specialized swap for empty classes.
     */
    template<bool, typename Tp>
    struct template_member_swap_provider
    {
      static void do_it(Tp& left, Tp& right)
      {
	using std::swap;
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
     *  @brief  Increment dimension @c node_dim, given @c key_dim.
     */
    template<typename KeyDimension>
    inline dimension_type
    incr_dim(const KeyDimension& dim, dimension_type node_dim)
    { return (node_dim + 1) % dim(); }

    /**
     *  @brief  Decrement dimension @c node_dim, given @c key_dim.
     */
    template<typename KeyDimension>
    inline dimension_type
    decr_dim(const KeyDimension& dim, dimension_type node_dim)
    { return node_dim ? node_dim - 1 : dim() - 1; }

    /**
     *  @brief  Return true if x coordinate is less than y coordinate over
     *  dimension @node_dim, given @compare. If both coordinate are equal,
     *  then return true if address of x is less than address of y.
     *
     *  This operator always discriminate x and y unless they are the same
     *  object.
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
     *  @brief  Return a boolean indicating whether all @c key coordinates are
     *  within range or not.
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
     *  @brief  Return a boolean indicating whether a single @c key coordinates
     *  is within range or not.
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
     *  @brief  Return a boolean indicating whether all @c key coordinates are
     *  within range or not, but without checking dimension @c exclude.
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

  } // namespace details

} // namespace spatial

#endif // SPATIAL_DETAILS_HPP
