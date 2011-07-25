// -*- C++ -*-

/**
 *  @file   spatial_traits.hpp
 *  @brief
 *
 *  Change Log:
 *
 *  - 2009-09-10 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#ifndef SPATIAL_TRAITS_HPP
#define SPATIAL_TRAITS_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include <utility> // for std::pair

// for std::tr1::false_type and std::tr1::true_type
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

    /*
     *  The category of invariants for a k-d tree: strict or relaxed.
     */
    struct relaxed_invariant_tag { };
    struct strict_invariant_tag { };

    // Forward declaration needed for the traits
    template<typename Key> struct Kdtree_node;

    //@{
    /*
     *  The node traits of the trees in spatial.
     */
    template<typename>
    struct node_traits
    { typedef relaxed_invariant_tag invariant_category; };

    template<typename Key>
    struct node_traits<Kdtree_node<Key> >
    { typedef strict_invariant_tag invariant_category; };
    //@}

    /*
     *  Help to retrieve the invariant category for a node type.
     */
    template<typename Node>
    typename node_traits<Node>::invariant_category
    invariant_category(const Node&)
    { return typename node_traits<Node>::invariant_category(); }

    //@{
    /*
     * A helper function to tag container for contant or non constant
     * iterators.
     */
    template<bool ConstantIterator>
    struct true_or_false_type
    { typedef std::tr1::false_type type; };

    template<>
    struct true_or_false_type<true>
    { typedef std::tr1::true_type type; };
    //@}

    //@{
    /**
     *  A helper that tells if two types are the same.
     */
    template<typename Tp1, typename Tp2>
    struct is_same : std::tr1::false_type { };
    template <typename Tp>
    struct is_same<Tp, Tp> : std::tr1::true_type { };
    //@}

  } // namespace details

  /**
   *  @brief  The traits type for all containers in the spatial
   *  namespace. Defines all the types that must be published or resolved from
   *  the spatial container types.
   */
  template<typename Tp>
  struct container_traits
  {
    /**
     *  The type representing the key managed by the container. @c key_type
     *  holds the coordinates that keeps its instances ordered in the container.
     */
    typedef typename Tp::key_type               key_type;

    /**
     *  The type representing the mapped values managed by the container. When
     *  @c mapped_type and @c key_type are different, as in mapped containers
     *  @ref pointmap or @ref boxmap; mapped_type holds the mutable part of the
     *  container value. Modifing the @c mapped_type does not affect @c
     *  key_type.
     */
    typedef typename Tp::mapped_type            mapped_type;

    /**
     *  When mapped_type and key_type are different, value_type holds a pair
     *  formed by key_type (first) and mapped_type (second); this is the case
     *  for @ref pointmap or @ref boxmap containers. In @ref pointset and @ref
     *  boxset containers, @c key_type, @c mapped_type and @c value_type are all
     *  similar types.
     */
    typedef typename Tp::value_type             value_type;

    /**
     *  @brief  Represent a pointer to a @c value_type. This type is used by the
     *  iterators of the container.
     */
    typedef typename Tp::pointer                pointer;

    /**
     *  @brief  Represent a pointer to a key_type. key_type is always constant.
     */
    typedef typename Tp::const_pointer          const_pointer;

    /**
     *  @brief  Represent a reference to a key_type. May or may not be mutable,
     *  depending on the type of key_type.
     */
    typedef typename Tp::reference              reference;

    /**
     *  @brief  Represent a reference to a key_type. key_type is always
     *  constant.
     */
    typedef typename Tp::const_reference        const_reference;

    typedef typename Tp::node_type              node_type;
    typedef typename Tp::size_type              size_type;
    typedef typename Tp::difference_type        difference_type;
    typedef typename Tp::allocator_type         allocator_type;

    /**
     *  Comparison functor used to compare two instances of @c key_type. It is
     *  also the type provided by the user in the various containers, @ref
     *  pointset, @ref boxset, @ref pointmap or @ref boxmap.
     */
    typedef typename Tp::key_compare            key_compare;

    /**
     *  Comparison functor used to compare two instances of @c value_type. This
     *  type is deduced from the key_compare in all containers.
     */
    typedef typename Tp::value_compare          value_compare;

    typedef typename Tp::rank_type              rank_type;

    /**
     *  @brief  A tag that defines whether the iterator of the container are
     *  constant or not. It can take 2 values, either std::tr1::true_type or
     *  std::tr1::false_type.
     */
    typedef typename Tp::const_iterator_tag     const_iterator_tag;
  };

  /**
   *  @brief  The traits type for all geometries in the spatial
   *  namespace. Defines all the types that must be published or resolved from a
   *  geometry.
   */
  template <typename Tp>
  struct geometry_traits
  {
    /**
     *  @brief  The type used by the geometry to represent distances.
     *
     *  Distance may be user defined in some geometry such as
     *  spatial::manhathan, but in this case, the type must follow specific
     *  rules to the geometry, such as overload of specific operators.
     */
    typedef typename Tp::distance_type         distance_type;
  };

} //namespace spatial

#endif // SPATIAL_TRAITS_HPP
