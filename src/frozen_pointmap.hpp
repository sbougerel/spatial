// -*- C++ -*-

/**
 *  @file   frozen_pointmap.hpp
 *  @brief
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_FROZEN_POINTMAP_HPP
#define SPATIAL_FROZEN_POINTMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<Key, Mapped> > >
  struct frozen_pointmap
    : details::Kdtree<details::Static_rank<Rank>, Key, Mapped, Compare,
                      Alloc, false>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank>, Key, Mapped, Compare,
                            Alloc, false>                       base_type;
    typedef frozen_pointmap<Rank, Key, Mapped, Compare, Alloc>  Self;

  public:

    frozen_pointmap() { }

    explicit frozen_pointmap(const Compare& compare)
      : base_type(details::Static_rank<Rank>())
    { }

    frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank>(), compare, alloc)
    { }

    frozen_pointmap(const frozen_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_pointmap&
    operator=(const frozen_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref frozen_pointmap with runtime rank support. The
   *  rank of the @ref frozen_pointmap can be determined at run time and does
   *  not need to be fixed at compile time. Using:
   *  @code
   *    struct point { ... };
   *    frozen_pointmap<0, point> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct point { ... };
   *    runtime_frozen_pointmap<point> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_pointmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped, typename Compare, typename Alloc>
  struct frozen_pointmap<0, Key, Mapped, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
		      Alloc, false>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
			    Alloc, false>                    base_type;
    typedef frozen_pointmap<0, Key, Mapped, Compare, Alloc>  Self;

  public:
    frozen_pointmap() { }

    explicit frozen_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank_argument(dim); }

    frozen_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank_argument(dim); }

    explicit frozen_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    frozen_pointmap(dimension_type dim, const Compare& compare,
                    const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank_argument(dim); }

    frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    frozen_pointmap(const frozen_pointmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_pointmap&
    operator=(const frozen_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  pointset with dynamic rank support. The rank of the pointset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<Key, Mapped> > >
  struct runtime_frozen_pointmap
    : details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
		      Alloc, false>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
			    Alloc, false>                         base_type;
    typedef runtime_frozen_pointmap<Key, Mapped, Compare, Alloc>  Self;

  public:
    runtime_frozen_pointmap() { }

    explicit runtime_frozen_pointmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim))
    { except::check_rank_argument(dim); }

    runtime_frozen_pointmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim), compare)
    { except::check_rank_argument(dim); }

    explicit runtime_frozen_pointmap(const Compare& compare)
      : base_type(compare)
    { }

    runtime_frozen_pointmap(dimension_type dim, const Compare& compare,
                            const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim), compare, alloc)
    { except::check_rank_argument(dim); }

    runtime_frozen_pointmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(), compare, alloc)
    { }

    runtime_frozen_pointmap(const runtime_frozen_pointmap& other,
                            bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_frozen_pointmap&
    operator=(const runtime_frozen_pointmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

}

#endif // SPATIAL_FROZEN_POINTMAP_HPP
