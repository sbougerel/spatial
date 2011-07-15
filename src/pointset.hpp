// -*- C++ -*-

/**
 *  @file   pointset.hpp
 *  @brief  
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_POINTSET_HPP
#define SPATIAL_POINTSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_set_macro.hpp"
#include "bits/spatial_traits.hpp"
#include "bits/spatial_exceptions.hpp"
#include "bits/spatial_assert.hpp"
#include "bits/spatial_details.hpp"
#include "bits/spatial_function.hpp"
#include "bits/spatial_node.hpp"
#include "bits/spatial_mapping.hpp"
#include "bits/spatial_range.hpp"
#include "bits/spatial_geometry.hpp"
#include "bits/spatial_neighbor.hpp"
#include "bits/spatial_kdtree_base.hpp"
#include "bits/spatial_kdtree.hpp"
#include "bits/spatial_relaxed_kdtree.hpp"
#include "bits/spatial_node.tpp"
#include "bits/spatial_mapping.tpp"
#include "bits/spatial_range.tpp"
#include "bits/spatial_neighbor.tpp"
#include "bits/spatial_kdtree_base.tpp"
#include "bits/spatial_kdtree.tpp"
#include "bits/spatial_relaxed_kdtree.tpp"
#include "bits/spatial_unset_macro.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
	   typename Compare = bracket_less<Key>,
	   typename BalancingPolicy = loose_balancing,
	   typename Alloc = std::allocator<Key> >
  struct pointset
    : details::Relaxed_kdtree<details::Static_rank<Rank>, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree<details::Static_rank<Rank>, Key, Compare,
				    BalancingPolicy, Alloc, true>   base_type;
    typedef pointset<Rank, Key, Compare, BalancingPolicy, Alloc>    Self;

  public:
    pointset() { }

    explicit pointset(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    pointset(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank>(), compare, balancing)
    { }

    pointset(const Compare& compare, const BalancingPolicy& balancing,
	     const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, balancing, alloc)
    { }

    pointset(const pointset& other)
      : base_type(other)
    { }

    pointset&
    operator=(const pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref pointset with runtime rank support. The rank of
   *  the @ref pointset can be determined at run time and does not need to be
   *  fixed at compile time. Using:
   *  @code
   *    struct point { \/* ... *\/ };
   *    pointset<0, point> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { \/* ... *\/ };
   *    runtime_pointset<point> my_set;
   *  @endcode
   *
   *  @see runtime_pointset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename BalancingPolicy,
	   typename Alloc>
  struct pointset<0, Key, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
				    BalancingPolicy, Alloc, true> base_type;
    typedef pointset<0, Key, Compare, BalancingPolicy, Alloc>     Self;

  public:
    pointset() { }

    explicit pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { }

    pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { }

    pointset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim), compare, policy)
    { }

    pointset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, policy, alloc)
    { }

    explicit pointset(const Compare& compare)
      : base_type(details::Dynamic_rank(), compare)
    { }

    pointset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(), compare, policy)
    { }

    pointset(const Compare& compare, const BalancingPolicy& policy,
		     const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, policy, alloc)
    { }

    pointset(const pointset& other)
      : base_type(other)
    { }

    pointset&
    operator=(const pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  @brief  pointset with runtime rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
	   typename Compare = bracket_less<Key>,
	   typename BalancingPolicy = loose_balancing,
	   typename Alloc = std::allocator<Key> >
  struct runtime_pointset
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
				    BalancingPolicy, Alloc, true>  base_type;
    typedef runtime_pointset<Key, Compare, BalancingPolicy, Alloc> Self;

  public:
    runtime_pointset() { }

    explicit runtime_pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { }

    runtime_pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { }

    runtime_pointset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim), compare, policy)
    { }

    runtime_pointset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, policy, alloc)
    { }

    explicit runtime_pointset(const Compare& compare)
      : base_type(details::Dynamic_rank(), compare)
    { }

    runtime_pointset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(), compare, policy)
    { }

    runtime_pointset(const Compare& compare, const BalancingPolicy& policy,
		     const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, policy, alloc)
    { }

    runtime_pointset(const runtime_pointset& other)
      : base_type(other)
    { }

    runtime_pointset&
    operator=(const runtime_pointset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_POINTSET_HPP
