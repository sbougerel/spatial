// -*- C++ -*-

/**
 *  @file   frozen_boxmap.hpp
 *  @brief
 *
 *  - 2009-02-26 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_FROZEN_BOXMAP_HPP
#define SPATIAL_FROZEN_BOXMAP_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<Key, Mapped> > >
  struct frozen_boxmap
    : details::Kdtree<details::Static_rank<Rank << 1>, Key, Mapped, Compare,
                      Alloc, false>
  {
  private:
    typedef
    details::Kdtree<details::Static_rank<Rank << 1>, Key, Mapped, Compare,
		    Alloc, false>                            base_type;
    typedef frozen_boxmap<Rank, Key, Mapped, Compare, Alloc> Self;

  public:
    frozen_boxmap() { }

    explicit frozen_boxmap(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    frozen_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, alloc)
    { }

    frozen_boxmap(const frozen_boxmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_boxmap&
    operator=(const frozen_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref frozen_boxmap with runtime rank support. The
   *  rank of the @ref frozen_boxmap can be determined at run time and does not
   *  need to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    frozen_boxmap<0, box, mapped> my_set;
   *  @endcode
   *  ...is therefore equivalent to:
   *  @code
   *    struct box { ... };
   *    struct mapped { ... };
   *    runtime_frozen_boxmap<box, mapped> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_boxmap for more information about how to use this
   *  container.
   */
  template<typename Key, typename Mapped, typename Compare, typename Alloc>
  struct frozen_boxmap<0, Key, Mapped, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
		      Alloc, false>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Mapped,
			    Compare, Alloc, false>         base_type;
    typedef frozen_boxmap<0, Key, Mapped, Compare, Alloc>  Self;

  public:
    frozen_boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit frozen_boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank_argument(dim); }

    frozen_boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank_argument(dim); }

    explicit frozen_boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    frozen_boxmap(dimension_type dim, const Compare& compare,
                  const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank_argument(dim); }

    frozen_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    frozen_boxmap(const frozen_boxmap& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_boxmap&
    operator=(const frozen_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  boxmap with dynamic rank support. The rank of the boxmap can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key, typename Mapped,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<std::pair<Key, Mapped> > >
  struct runtime_frozen_boxmap
    : details::Kdtree<details::Dynamic_rank, Key, Mapped, Compare,
		      Alloc, false>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Mapped,
			    Compare, Alloc, false>             base_type;
    typedef runtime_frozen_boxmap<Key, Mapped, Compare, Alloc> Self;

  public:
    runtime_frozen_boxmap() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_frozen_boxmap(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank_argument(dim); }

    runtime_frozen_boxmap(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank_argument(dim); }

    explicit runtime_frozen_boxmap(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_frozen_boxmap(dimension_type dim, const Compare& compare,
                          const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank_argument(dim); }

    runtime_frozen_boxmap(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    runtime_frozen_boxmap(const runtime_frozen_boxmap& other,
                          bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_frozen_boxmap&
    operator=(const runtime_frozen_boxmap& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

} // namespace spatial

#endif // SPATIAL_FROZEN_BOXMAP_HPP
