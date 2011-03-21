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
	   typename Alloc = std::allocator<Key> >
  struct pointset
    : details::Kdtree<details::Static_rank<Rank>, Key, Compare, Alloc, true>
  {
    typedef pointset<Rank, Key, Compare, Alloc> mapping_iterable;

  private:
    typedef details::Kdtree<details::Static_rank<Rank>,
			    Key, Compare, Alloc, true>   base_type;

  public:
    pointset() { }

    explicit pointset(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    pointset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, alloc)
    { }

    pointset(const pointset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    pointset&
    operator=(const pointset& other)
    {
      base_type::operator=(other);
    }
  };

  /**
   *  @brief  This specialization allows one to specify the dimension for the
   *  point set at run time.
   */
  template<typename Key, typename Compare, typename Alloc>
  struct pointset<0, Key, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, Key, Compare, Alloc, true>
  {
    typedef pointset<0, Key, Compare, Alloc>  mapping_iterable;

  private:
    typedef details::Kdtree<details::Dynamic_rank,
			    Key, Compare, Alloc, true> base_type;

  public:
    pointset() { }

    explicit pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { }

    pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { }

    explicit pointset(const Compare& compare)
      : base_type(compare)
    { }

    pointset(dimension_type dim, const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { }

    pointset(const pointset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    pointset&
    operator=(const pointset& other)
    {
      base_type::operator=(other);
    }
  };

  /**
   *  @brief  pointset with dynamic rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
	   typename Compare = bracket_less<Key>,
	   typename Alloc = std::allocator<Key> >
  struct dynamic_pointset
    : details::Kdtree<details::Dynamic_rank, Key, Compare, Alloc, true>
  {
    typedef dynamic_pointset<Key, Compare, Alloc> mapping_iterable;

  private:
    typedef details::Kdtree<details::Dynamic_rank,
			    Key, Compare, Alloc, true>     base_type;

  public:
    dynamic_pointset() { }

    explicit dynamic_pointset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { }

    dynamic_pointset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { }

    explicit dynamic_pointset(const Compare& compare)
      : base_type(compare)
    { }

    dynamic_pointset(dimension_type dim, const Compare& compare,
		     const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { }

    dynamic_pointset(const dynamic_pointset& other,
		     bool balancing = false)
      : base_type(other, balancing)
    { }

    dynamic_pointset&
    operator=(const dynamic_pointset& other)
    {
      base_type::operator=(other);
    }
  };

}

#endif // SPATIAL_POINTSET_HPP
