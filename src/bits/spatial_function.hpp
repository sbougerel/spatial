// -*- C++ -*-

/**
 *  @file   spatial_function.hpp
 *  @brief  Main functors that are used in the library.
 *
 *  Change Log:
 *
 *  - 2009-02-27 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_FUNCTION_HPP
#define SPATIAL_FUNCTION_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <stdexcept>
#include <iterator>

namespace spatial
{
  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordinate that are not accessible via the bracket,
   *  parenthesis operator or iterator deference.
   *  @test   test_accessor_less
   *
   *  Generally, the spatial containers are used with one of bracket_less,
   *  paren_less, or iterator_less. However, when the Key to the spatial
   *  container cannot be compared through one of the above mean, the following
   *  helper comparator can be used.
   */
  template<typename Accessor, typename Tp>
  struct accessor_less
    : private Accessor // empty member optimization
  {
    accessor_less(const Accessor& accessor = Accessor())
      : Accessor(accessor)
    { }

    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      return (Accessor::operator()(n, x) < Accessor::operator()(n, y));
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via the bracket operator.
   *  @test   test_bracket_less
   */
  template <typename Tp>
  struct bracket_less
  {
    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      return (x[n] < y[n]);
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via the parenthesis operator.
   *  @test   test_paren_less
   */
  template <typename Tp>
  struct paren_less
  {
    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      return (x(n) < y(n));
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via iterator deference.
   *  @test   test_iterator_less
   */
  template <typename Tp>
  struct iterator_less
  {
    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      typename Tp::const_iterator ix = x.begin();
      typename Tp::const_iterator iy = y.begin();
      {
	using namespace ::std; advance(ix, n); advance(iy, n);
      }
      return (*ix < *iy);
    }
  };

  /**
   *  @brief  For all @c x in the set @c S, we define @c y such that
   *  orthogonal ranges iterates over all the @e{x} that satify: for all
   *  dimensions {0, ..., k-1 | yi <= xi && yi >= xi}. In other words, iterates
   *  orthogonally over all the range of elements in @c S that are equal to y
   *  with respect to all dimensions.
   */
  template <typename Key, typename Compare>
  struct equal_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    equal_bounds()
      : Compare(), match() { }

    /**
     *  @brief  Set the key for the boundaries.
     */
    equal_bounds(const Compare &compare, const Key& match)
      : Compare(compare), match(match)
    { }

    /**
     *  @brief  The operator that tells wheather the key is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare* compare = static_cast<const Compare*>(this);
      return (details::less(*compare, dim, key, match)
	      ? below
	      : (details::less(*compare, dim, match, key)
		 ? above
		 : matching));
    }

    /**
     *  @brief  The unique element that defines both lower and upper range for
     *  the equal range iterator.
     */
    Key match;
  };

  /**
   *  @brief  Equal bound factory that takes in a @c container and a @c key,
   *  returning an @c equal_bounds<Key, Compare> type.
   *  @see equal_bounds<Key, Compare>
   */
  template <typename Tp>
  equal_bounds<typename container_traits<Tp>::key_type,
	       typename container_traits<Tp>::compare_type>
  make_equal_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& key)
  {
    return equal_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), key);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   */
  template <typename Key, typename Compare>
  struct open_range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    open_range_bounds()
      : Compare(), lower(), upper() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     *  @see make_open_bounds
     *
     *  The constructor does not check the lower and upper satisfy the following
     *  requierment: @c compare(d, lower, upper) must return true for all @c d
     *  for the keys @c lower and @c upper.
     */
    open_range_bounds(const Compare& compare, const Key& lower,
		      const Key& upper)
      : Compare(compare), lower(lower), upper(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare* compare = static_cast<const Compare*>(this);
      return (!details::less(*compare, dim, lower, key)
	      ? below
	      : (details::less(*compare, dim, key, upper)
		 ? matching
		 : above));
    }

    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper;
  };

  /**
   *  @brief  Open bound factory that takes in a @c container, a @c lower and @c
   *  upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also checks that lower and upper satisfy range requirements
   *  over an open interval for each dimension, e.g:
   *  @c compare.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c tree.dimension().
   */
  template <typename Tp>
  open_range_bounds<typename container_traits<Tp>::key_type,
		    typename container_traits<Tp>::compare_type>
  make_open_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_open_range_bounds(container, lower, upper);
    return open_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   */
  template <typename Key, typename Compare>
  struct range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    range_bounds()
      : Compare(), lower(), upper() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     */
    range_bounds(const Compare& compare, const Key& lower, const Key& upper)
      : Compare(compare), lower(lower), upper(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare* compare = static_cast<const Compare*>(this);
      return (details::less(*compare, dim, key, lower)
	      ? below
	      : (details::less(*compare, dim, key, upper)
		 ? matching
		 : above));
    }

    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper;
  };

  /**
   *  @brief  Range bounds factory that takes in a @c container, a @c lower and
   *  @c upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also check that lower and upper satisfy range requirements
   *  over an regular interval for each dimension, e.g: @c
   *  container.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.compare()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  range_bounds<typename container_traits<Tp>::key_type,
	       typename container_traits<Tp>::compare_type>
  make_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_range_bounds(container, lower, upper);
    return range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

  /**
   *  @brief  For all @e{x} in the set @e{S}, we define @e{lower} and @e{upper}
   *  such that orthogonal ranges iterates over all the @e{x} that satify: for
   *  all dimensions {0, ..., k-1 | loweri <= xi && higheri >= xi}. In other
   *  words, iterates over all the range of elements in @{S} that are within the
   *  interval {lower, upper} with respect to all dimensions.
   */
  template <typename Key, typename Compare>
  struct closed_range_bounds
    : private Compare // empty member optimization
  {
    /**
     *  @brief  The default constructor leaves everything un-initialized
     */
    closed_range_bounds()
      : Compare(), lower(), upper() { }

    /**
     *  @brief  Set the lower and upper boundary for the orthogonal range
     *  search.
     *
     *  @throws a std::out_of_range if an element of lower is strictly greater
     *  than the corresponding element of upper with respect to their dimension.
     */
    closed_range_bounds(const Compare& compare, const Key& lower,
			const Key& upper)
      : Compare(compare), lower(lower), upper(upper)
    { }

    /**
     *  @brief  The operator that tells wheather the point is in range or not.
     */
    relative_order
    operator()(dimension_type dim, const Key& key) const
    {
      const Compare* compare = static_cast<const Compare*>(this);
      return (details::less(*compare, dim, key, lower)
	      ? below
	      : (details::less(*compare, dim, upper, key)
		 ? above
		 : matching));
    }

    /**
     *  @brief  The lower bound for the orthogonal range iterator.
     */
    Key lower;

    /**
     *  @brief  The upper bound for the orthogonal range iterator.
     */
    Key upper;
  };

  /**
   *  @brief  Closed bounds factory that takes in a @c container, a @c lower and
   *  @c upper key range, returning an @c open_bounds<Key, Compare> type.  @see
   *  equal_bounds<Key, Compare>
   *
   *  This factory also check that lower and upper satisfy range requirements
   *  over an regular interval for each dimension, e.g: @c
   *  container.compare()(d, lower, upper) must be true for all value of @c d
   *  within 0 and @c container.dimension() or container.compare()(d, upper,
   *  lower) must also be false (which mean the values are equal on that
   *  particular dimension).
   */
  template <typename Tp>
  closed_range_bounds<typename container_traits<Tp>::key_type,
		      typename container_traits<Tp>::compare_type>
  make_closed_range_bounds
  (const Tp& container,
   const typename container_traits<Tp>::key_type& lower,
   const typename container_traits<Tp>::key_type& upper)
  {
    except::check_closed_range_bounds(container, lower, upper);
    return closed_range_bounds
      <typename container_traits<Tp>::key_type,
      typename container_traits<Tp>::compare_type>
      (container.compare(), lower, upper);
  }

} // namespace spatial

#endif // SPATIAL_FUNCTION_HPP

