// -*- C++ -*-

/**
 *  @file   spatial_geometry.hpp
 *  @brief  
 *
 *  Change log:
 *
 *  - 08-08-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_GEOMETRY_HPP
#define SPATIAL_GEOMETRY_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

namespace spatial
{
  namespace details
  {
    namespace geometry
    {
      /**
       *  @brief  This functor wraps an accessor providing an extra level of
       *  indirection to cast the Tp's component into Distance or assign the
       *  coordinate of one parameter to the other along the same dimension.
       */
      template <typename Tp, typename Distance, typename Accessor>
      struct cast_accessor
	: Accessor
      {
	explicit cast_accessor(const Accessor& accessor = Accessor())
	  : Accessor(accessor)
	{ }

	Distance
	operator() (dimension_type n, const Tp& x) const
	{ return static_cast<Distance>(Accessor::operator()(n, x)); }

	void
	operator() (dimension_type n, const Tp& x, Tp& y) const
	{ Accessor::operator()(n, y) = Accessor::operator()(n, x); }
      };

      /**
       *  @brief  This functor accesses the coordinates of Tp via a bracket
       *  operator and casts them into Distance or assign the coordinate of one
       *  parameter to the other along the same dimension.
       *  @test   test_bracket_cast_accessor
       */
      template <typename Tp, typename Distance, typename Compare = int>
      struct bracket_cast_accessor
      {
	bracket_cast_accessor() { }
	explicit bracket_cast_accessor(const Compare&) { }

	Distance
	operator() (dimension_type n, const Tp& x) const
	{ return static_cast<Distance>(x[n]); }

	void
	operator() (dimension_type n, const Tp& x, Tp& y) const
	{ y[n] = x[n]; }
      };

      /**
       *  @brief  This functor accesses the coordinates of Tp via a parenthesis
       *  operator and casts them into Distance or assign the coordinate of one
       *  parameter to the other along the same dimension.
       *  @test   test_paren_cast_accessor
       */
      template <typename Tp, typename Distance, typename Compare = int>
      struct paren_cast_accessor
      {
	paren_cast_accessor() { }
	explicit paren_cast_accessor(const Compare&) { }

	Distance
	operator() (dimension_type n, const Tp& x)
	{ return static_cast<Distance>(x(n)); }

	void
	operator() (dimension_type n, const Tp& x, Tp& y) const
	{ y(n) = x(n); }
      };

      /**
       *  @brief  This functor accesses the coordinates of Tp via a iteration
       *  operator and casts them into Distance or assign the coordinate of one
       *  parameter to the other along the same dimension.
       *  @test   test_iterator_accessor
       */
      template <typename Tp, typename Distance, typename Compare = int>
      struct iterator_cast_accessor
      {
	iterator_cast_accessor() { }
	explicit iterator_cast_accessor(const Compare&) { }

	Distance
	operator() (dimension_type n, const Tp& x)
	{
	  typename Tp::const_iterator i = x.begin();
	  { using namespace ::std; advance(i, n); }
	  return static_cast<Distance>(*i);
	}

	void
	operator() (dimension_type n, const Tp& x, Tp& y) const
	{
	  typename Tp::const_iterator i = x.begin();
	  typename Tp::iterator j = y.begin();
	  { using namespace ::std; advance(i, n); advance(j, n); }
	  *j = *i;
	}
      };

      /**
       *  @brief  Rebind the accessor to the an accessor that can cast the
       *  internal components of Key into the Distance type. Also helps to
       *  change standards comparators into accessors using the same
       *  operations.
       *
       *  @{
       */
      template <typename Key, typename Distance, typename Accessor>
      struct rebind
      { typedef cast_accessor<Key, Distance, Accessor> type; };

      template <typename Key, typename Distance>
      struct rebind<Key, Distance, bracket_less<Key> >
      {
	typedef bracket_cast_accessor
	<Key, Distance, bracket_less<Key> > type;
      };

      template <typename Key, typename Distance>
      struct rebind<Key, Distance, paren_less<Key> >
      {
	typedef paren_cast_accessor
	<Key, Distance, paren_less<Key> > type;
      };

      template <typename Key, typename Distance>
      struct rebind<Key, Distance, iterator_less<Key> >
      {
	typedef iterator_cast_accessor
	<Key, Distance, iterator_less<Key> > type;
      };
      // @}

    } // namespace geometry
  } // namespace details

  namespace math
  {
    /**
     *  @brief  Uses hypot algorithm in order to compute the distance: minimize
     *  possibilities to overflow and underflow.
     *  @test   test_euclidian_distance_to_key
     *
     *  The trick is to find the maximum value among all the component of the
     *  Key and then divide all other component with this one.
     *
     *  Here is the rationale for the distance:
     *  sqrt( x^2 + y^2 + z^2 + ... ) = abs(x) * sqrt( 1 + (y/x)^2 + (z/x)^2 )
     *
     *  Providing x statisfies x>y, x>z, etc, the second form is less likely to
     *  overflow or underflow than the first form.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_distance_to_key
    (dimension_type rank, Key origin, Key key, Accessor access)
    {
      Distance zero = static_cast<Distance>(0);
      Distance one = static_cast<Distance>(1);
      // Find a non zero maximum or return 0
      Distance max = zero;
      dimension_type max_dim;
      using namespace ::std;
      for (dimension_type i=0; i<rank; ++i)
	{
	  Distance diff
        = abs(static_cast<Distance>(access(i, origin) - access(i, key)));
	  if (diff == zero) continue;
	  if (max == zero || diff > max)
	    { max = diff; max_dim = i; }
	}
      if (max == zero) return zero; // they're all zero!
      // Compute the distance
      Distance sum = zero;
      for (dimension_type i=0; i<rank; ++i)
	{
	  if (i == max_dim) continue;
	  Distance diff = static_cast<Distance>(access(i, origin) - access(i, key));
	  Distance div = diff/max;
	  sum += div * div;
	}
      return max * sqrt(one + sum);
    }

    /**
     *  @brief  Compute the distance between @p origin and the further edge of
     *  the box formed by @p low and @p high.
     *
     *  Uses the hyoth algorithm to minimize risk of overflow or underflow.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_distance_to_box_edge
    (dimension_type rank, Key origin, Key low, Key high, Accessor access)
    {
      Distance zero = static_cast<Distance>(0);
      Distance one = static_cast<Distance>(1);
      Distance two = static_cast<Distance>(2);
      // Find a non zero maximum or return 0
      Distance max = zero;
      dimension_type max_dim = 0;
      using namespace ::std;
      for (dimension_type i=0; i<rank; ++i)
	{
	  Distance diff = abs(access(i, origin)
			      - ((access(i, high) + access(i, low)) / two));
	  if (diff == zero) continue;
	  if (max == zero || diff > max)
	    { max = diff; max_dim = i; }
	}
      max += (access(max_dim, high) - access(max_dim, low)) / two;
      if (max == zero) return zero; // they're all zero!
      // Compute the distance
      Distance sum = zero;
      for (dimension_type i=0; i<rank; ++i)
	{
	  SPATIAL_ASSERT_CHECK(access(i, high) >= access(i, low));
	  if (i == max_dim) continue;
	  Distance diff = abs(access(i, origin)
			      - ((access(i, high) + access(i, low)) / two));
	  Distance radius = (access(i, high) - access(i, low)) / two;
	  Distance div = (radius + diff) / max;
	  sum += div * div;
	}
      return max * sqrt(one + sum);
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     *  @test   test_euclidian_distance_to_plane
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_distance_to_plane
    (dimension_type dim, Key origin, Key key, Accessor access)
    {
      using namespace ::std;
      return abs(static_cast<Distance>(access(dim, origin) - access(dim, key)));
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     *  @test   test_euclidian_square_distance_to_plane
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_square_distance_to_plane
    (dimension_type dim, Key origin, Key key, Accessor access)
    {
      Distance diff = static_cast<Distance>
        (access(dim, origin) - access(dim, key));
      return diff*diff;
    }

    /**
     *  @brief  Compute the square value of the distance between @p origin and
     *  @p key.
     *  @test   test_euclidian_square_distance_to_key
     *
     *  The computation of the square value is much faster than the computation
     *  of the distance, but it tends to overflow quickly if large values are
     *  being used in the computation. On the contrary, if small values are
     *  being used at all times, and if you can guarentee that the square
     *  distance will not overflow, this method of computation should be
     *  preferred over the regular distance computation. Remember to compute the
     *  square root of the square distance if you want to read the actual
     *  distance.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_square_distance_to_key
    (dimension_type rank, Key origin, Key key, Accessor access)
    {
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
	{
	  sum += euclidian_square_distance_to_plane
	    <Key, Accessor, Distance>(i, origin, key, access);
	}
      return sum;
    }


    /**
     *  @brief  Compute the square value of the distance between @p origin and
     *  the further edge of the box formed by @p low and @p high.
     *
     *  The computation of the square value is much faster than the computation
     *  of the distance, but it tends to overflow quickly if large values are
     *  being used in the computation. On the contrary, if small values are
     *  being used at all times, and if you can guarentee that the square
     *  distance will not overflow, this method of computation should be
     *  preferred over the regular distance computation. Remember to compute the
     *  square root of the square distance if you want to read the actual
     *  distance.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    euclidian_square_distance_to_box_edge
    (dimension_type rank, Key origin, Key low, Key high, Accessor access)
    {
      Distance sum = static_cast<Distance>(0);
      Distance two = static_cast<Distance>(2);
      for (dimension_type i=0; i<rank; ++i)
	{
	  SPATIAL_ASSERT_CHECK(access(i, high) >= access(i, low));
	  using namespace ::std;
	  Distance diff = abs(access(i, origin)
			      - ((access(i, low) + access(i, high)) / two));
	  Distance radius = (access(i, high) - access(i, low)) / two;
	  Distance diff_radius = diff + radius;
	  sum += diff_radius * diff_radius;
	}
      return sum;
    }

    /**
     *  @brief  Compute the distance between the @p origin and the closest point
     *  to the plane orthogonal to the axis of dimension @c dim and passing by
     *  @c key.
     *  @test   test_manhattan_distance_to_plane
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    manhattan_distance_to_plane
    (dimension_type dim, Key origin, Key key, Accessor access)
    {
      using namespace ::std;
      Distance diff = abs(static_cast<Distance>
                          (access(dim, origin) - access(dim, key)));
      return diff;
    }

    /**
     *  @brief  Compute the manhattan distance between @p origin and @p key.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    manhattan_distance_to_key
    (dimension_type rank, Key origin, Key key, Accessor access)
    {
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
	{
	  sum += manhattan_distance_to_plane
	    <Key, Accessor, Distance>(i, origin, key, access);
	}
      return sum;
    }

    /**
     *  @brief  Compute the manhattan distance between @p origin and the further
     *  edge of the box formed by @p low and @p high.  @test
     *
     *  The computation of the square value is much faster than the computation
     *  of the distance, but it tends to overflow quickly if large values are
     *  being used in the computation. On the contrary, if small values are
     *  being used at all times, and if you can guarentee that the square
     *  distance will not overflow, this method of computation should be
     *  preferred over the regular distance computation. Remember to compute the
     *  square root of the square distance if you want to read the actual
     *  distance.
     */
    template <typename Key, typename Accessor, typename Distance>
    inline Distance
    manhattan_distance_to_box_edge
    (dimension_type rank, Key origin, Key low, Key high, Accessor access)
    {
      Distance two = static_cast<Distance>(2);
      Distance sum = static_cast<Distance>(0);
      for (dimension_type i=0; i<rank; ++i)
	{
	  SPATIAL_ASSERT_CHECK(access(i, high) >= access(i, low));
	  using namespace ::std;
	  Distance diff = abs(access(i, origin)
			      - ((access(i, low) + access(i, high)) / two));
	  Distance radius = (access(i, high) - access(i, low)) / two;
	  sum += diff + radius;
	}
      return sum;
    }

    /*
      // For a future implementation where we take earth-like spheroid as an
      // example for non-euclidian spaces, or manifolds.

      math::great_circle_distance_to_origin
      (rank, origin, dim, key, accumulator)

      math::great_circle_distance_to_axis
      (rank, origin, dim, key, accumulator)

      math::vincenty_distance_to_origin
      (rank, origin, dim, key, accumulator)

      math::vincenty_distance_to_axis
      (rank, origin, dim, key, accumulator)
    */
  } // namespace math

  /**
   *  @brief  Defines the geometry for the euclidian space where the distance can
   *  be computed from a double or a type that can be casted into a double.
   *  @test   test_geometry_euclidian_double
   *
   *  The type @c Accessor type should provide read/write access on its
   *  components along each dimensions.
   *
   *  struct Accessor
   *  {
   *    const component_type& operator()(dimension_type dim, const Key& key);
   *    component_type& operator()(dimension_type dim, Key& key);
   *  };
   */
  template<typename Key, typename Accessor>
  class euclidian_double
    : details::geometry::rebind<Key, double, Accessor>::type
  {
    typedef typename details::geometry::rebind<Key, double, Accessor>
    ::type access_type;

  public:
    /**
     *  @brief  The distance type being used for distance calculations.
     */
    typedef double distance_type;

    euclidian_double(const Accessor& access = Accessor())
      : access_type(access)
    { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return  The resulting distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Key& origin, const Key& key) const
    {
      return math::euclidian_distance_to_key<Key, access_type, double>
	(rank, origin, key, static_cast<const access_type>(*this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting distance.
     *
     *  Given any 2 points 'origin' and 'key', the result of distance_to_plane
     *  must always be less or equal to the result of distance_to_key.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Key& origin, const Key& key) const
    {
      return math::euclidian_distance_to_plane<Key, access_type, double>
	(dim, origin, key, static_cast<const access_type>(*this));
    }
  };

  /**
   *  @brief  Defines the geometry for the euclidian space where the distance can
   *  be computed from a float or a type that can be casted into a float.
   *  @test   test_geometry_euclidian_float
   *
   *  The type @c Accessor type should provide read/write access on its
   *  components along each dimensions.
   *
   *  struct Accessor
   *  {
   *    const component_type& operator()(dimension_type dim, const Key& key);
   *    component_type& operator()(dimension_type dim, Key& key);
   *  };
   */
  template<typename Key, typename Accessor>
  class euclidian_float
    : details::geometry::rebind<Key, float, Accessor>::type
  {
    typedef typename details::geometry::rebind<Key, float, Accessor>
    ::type access_type;

  public:
    typedef float distance_type;

    euclidian_float(const Accessor& access = Accessor())
      : access_type(access)
    { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return  The resulting distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Key& origin, const Key& key) const
    {
      return math::euclidian_distance_to_key<Key, access_type, float>
	(rank, origin, key, static_cast<const access_type>(*this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Key& origin, const Key& key) const
    {
      return math::euclidian_distance_to_plane<Key, access_type, float>
	(dim, origin, key, static_cast<const access_type>(*this));
    }
  };

  /**
   *  @brief  Defines the geometry for the euclidian space where only the square
   *  of the distances are being computed into a scalar value expressed as a
   *  double.
   *  @test   test_geometry_euclidian_square_double
   *
   *  The type @c Accessor type should provide read/write access on its
   *  components along each dimensions.
   *
   *  struct Accessor
   *  {
   *    const component_type& operator()(dimension_type dim, const Key& key);
   *    component_type& operator()(dimension_type dim, Key& key);
   *  };
   */
  template<typename Key, typename Accessor>
  class euclidian_square_double
    : details::geometry::rebind<Key, double, Accessor>::type
  {
    typedef typename details::geometry::rebind<Key, double, Accessor>
    ::type access_type;

  public:
    typedef double distance_type;

    euclidian_square_double(const Accessor& access = Accessor())
      : access_type(access)
    { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Key& origin, const Key& key) const
    {
      return math::euclidian_square_distance_to_key<Key, access_type, double>
	(rank, origin, key, static_cast<const access_type>(*this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Key& origin, const Key& key) const
    {
      return math::euclidian_square_distance_to_plane<Key, access_type, double>
	(dim, origin, key, static_cast<const access_type>(*this));
    }
  };

  /**
   *  @brief  Defines the geometry for the euclidian space where only the square
   *  of the distances are being computed into a scalar value expressed as a
   *  double.
   *  @test   test_geometry_euclidian_square_float
   *
   *  The type @c Accessor type should provide read/write access on its
   *  components along each dimensions.
   *
   *  struct Accessor
   *  {
   *    const component_type& operator()(dimension_type dim, const Key& key);
   *    component_type& operator()(dimension_type dim, Key& key);
   *  };
   */
  template<typename Key, typename Accessor>
  class euclidian_square_float
    : details::geometry::rebind<Key, float, Accessor>::type
  {
    typedef typename details::geometry::rebind<Key, float, Accessor>
    ::type access_type;

  public:
    typedef float distance_type;

    euclidian_square_float(const Accessor& access = Accessor())
      : access_type(access)
    { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Key& origin, const Key& key) const
    {
      return math::euclidian_square_distance_to_key<Key, access_type, float>
	(rank, origin, key, static_cast<const access_type>(*this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Key& origin, const Key& key) const
    {
      return math::euclidian_square_distance_to_plane<Key, access_type, float>
	(dim, origin, key, static_cast<const access_type>(*this));
    }
  };

  /**
   *  @brief  Defines a geometry for the euclidian space where distances are the
   *  sum of all the elements of a vector. Also known as the taxicab geometry.
   *  @test   test_geometry_manhattan
   *
   *  The type @c Accessor type should provide read/write access on its
   *  components along each dimensions.
   *
   *  struct Accessor
   *  {
   *    const component_type& operator()(dimension_type dim, const Key& key);
   *    component_type& operator()(dimension_type dim, Key& key);
   *  };
   */
  template<typename Key, typename Accessor, typename Distance>
  struct manhattan
    : details::geometry::rebind<Key, Distance, Accessor>::type
  {
    typedef typename details::geometry::rebind<Key, Distance, Accessor>
    ::type access_type;

  public:
    typedef Distance distance_type;

    manhattan (const Accessor& access = Accessor())
      : access_type(access)
    { }

    /**
     *  @brief  Compute the distance between the point of @c origin and the @c
     *  key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_key(dimension_type rank,
		    const Key& origin, const Key& key) const
    {
      return math::manhattan_distance_to_key<Key, access_type, Distance>
	(rank, origin, key, static_cast<const access_type>(*this));
    }

    /**
     *  @brief  The distance between the point of @c origin and the closest
     *  point to the plane orthogonal to the axis of dimension @c dim and
     *  crossing @c key.
     *  @return The resulting square distance.
     */
    distance_type
    distance_to_plane(dimension_type, dimension_type dim,
		      const Key& origin, const Key& key) const
    {
      return math::manhattan_distance_to_plane<Key, access_type, Distance>
	(dim, origin, key, static_cast<const access_type>(*this));
    }
  };

} // namespace spatial

#endif // SPATIAL_GEOMETRY_HPP
