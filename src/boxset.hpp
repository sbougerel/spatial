// -*- C++ -*-

/**
 *  @file   boxset.hpp
 *  @brief  
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_BOXSET_HPP
#define SPATIAL_BOXSET_HPP

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
  struct boxset
    : details::Relaxed_kdtree<details::Static_rank<Rank << 1>, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree
    <details::Static_rank<Rank << 1>, Key, Compare,
		          BalancingPolicy, Alloc, true>           base_type;
    typedef boxset<Rank, Key, Compare, BalancingPolicy, Alloc>    Self;

  public:
    boxset() { }

    explicit boxset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    boxset(const Compare& compare, const BalancingPolicy& balancing)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing)
    { }

    boxset(const Compare& compare, const BalancingPolicy& balancing,
	     const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, balancing, alloc)
    { }

    boxset(const boxset& other)
      : base_type(other)
    { }

    boxset&
    operator=(const boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  Specialization for @ref boxset with runtime rank support. The
   *  rank of the @ref boxset can be determined at run time and does not need
   *  to be fixed at compile time. Using:
   *  @code
   *    struct box { \/* ... *\/ };
   *    boxset<0, box> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct box { \/* ... *\/ };
   *    runtime_boxset<box> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_boxset for more information about how to use this
   *  container.
   */
  template<typename Key,
	   typename Compare,
	   typename BalancingPolicy,
	   typename Alloc>
  struct boxset<0, Key, Compare, BalancingPolicy, Alloc>
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
				    BalancingPolicy, Alloc, true> base_type;
    typedef boxset<0, Key, Compare, BalancingPolicy, Alloc>     Self;

  public:
    boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { }

    boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { }

    boxset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { }

    boxset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { }

    explicit boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    boxset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    boxset(const Compare& compare, const BalancingPolicy& policy,
		     const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    boxset(const boxset& other)
      : base_type(other)
    { }

    boxset&
    operator=(const boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

  /**
   *  @brief  boxset with runtime rank support. The rank of the boxset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
	   typename Compare = bracket_less<Key>,
	   typename BalancingPolicy = loose_balancing,
	   typename Alloc = std::allocator<Key> >
  struct runtime_boxset
    : details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
			      BalancingPolicy, Alloc, true>
  {
  private:
    typedef details::Relaxed_kdtree<details::Dynamic_rank, Key, Compare,
				    BalancingPolicy, Alloc, true>  base_type;
    typedef runtime_boxset<Key, Compare, BalancingPolicy, Alloc> Self;

  public:
      runtime_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { }

    runtime_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { }

    runtime_boxset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy)
    { }

    runtime_boxset(dimension_type dim, const Compare& compare,
		     const BalancingPolicy& policy, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, policy, alloc)
    { }

    explicit runtime_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_boxset(const Compare& compare, const BalancingPolicy& policy)
      : base_type(details::Dynamic_rank(2), compare, policy)
    { }

    runtime_boxset(const Compare& compare, const BalancingPolicy& policy,
		     const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, policy, alloc)
    { }

    runtime_boxset(const runtime_boxset& other)
      : base_type(other)
    { }

    runtime_boxset&
    operator=(const runtime_boxset& other)
    { return static_cast<Self&>(base_type::operator=(other)); }
  };

}

#endif // SPATIAL_BOXSET_HPP
