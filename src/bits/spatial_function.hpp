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
   *  @concept accessor_less is a model of BoxComparison to accomodate both
   *  pointset and boxset family of container.
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

    bool operator()
    (dimension_type a, const Tp& x, dimension_type b, const Tp& y) const
    {
      return (Accessor::operator()(a, x) < Accessor::operator()(b, y));
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via the bracket operator.
   *  @concept accessor_less is a model of BoxComparison to accomodate both
   *  pointset and boxset family of container.
   */
  template <typename Tp>
  struct bracket_less
  {
    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      return (x[n] < y[n]);
    }

    bool operator()
    (dimension_type a, const Tp& x, dimension_type b, const Tp& y) const
    {
      return (x[a] < y[b]);
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via the parenthesis operator.
   *  @concept accessor_less is a model of BoxComparison to accomodate both
   *  pointset and boxset family of container.
   */
  template <typename Tp>
  struct paren_less
  {
    bool
    operator() (dimension_type n, const Tp& x, const Tp& y) const
    {
      return (x(n) < y(n));
    }

    bool operator()
    (dimension_type a, const Tp& x, dimension_type b, const Tp& y) const
    {
      return (x(a) < y(b));
    }
  };

  /**
   *  @brief  A comparator that simplifies using the spatial containers with a
   *  Key type that has coordiates accessible via iterator deference.
   *  @concept accessor_less is a model of BoxComparison to accomodate both
   *  pointset and boxset family of container.
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

    bool operator()
    (dimension_type a, const Tp& x, dimension_type b, const Tp& y) const
    {
      typename Tp::const_iterator ix = x.begin();
      typename Tp::const_iterator iy = y.begin();
      {
	using namespace ::std; advance(ix, a); advance(iy, b);
      }
      return (*ix < *iy);
    }
  };

} // namespace spatial

#endif // SPATIAL_FUNCTION_HPP

