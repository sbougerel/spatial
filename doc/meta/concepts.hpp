/**
 *  A link mode defines the relationship between a node and the links that bears
 *  the node. It provides all necessary informations in order to get from the
 *  node to the link, and to access the key and value information contained in
 *  the link.
 *  \concept Definition of the LinkMode concept.
 *
 *  The important concept to understand is that the link is the type that
 *  aggregates the key, the value and the node. A tree is composed of a series
 *  of nodes all tied to their key and values through the link type.
 *
 *  There are multiple types of link in the library. Sometimes, the link derives
 *  from the node and the key and values are attributes. Some other times, the
 *  value type is the link itself, and it contains the node as an
 *  attribute. This diversity explains why, in the tree algorithm, only the node
 *  is being used, and the key, the value or the link are only retreived on
 *  demand.
 *
 *  The models of LinkMode shall publicly provide the following interface:
 *  \concept_tab
 *  \concept_leg T \concept_des A model of LinkMode
 *  \concept_req typename T::link_type;
 *  \concept_des The link type which should be defined as Link.
 *  \concept_req typename T::key_type;
 *  \concept_des The key type in the link.
 *  \concept_req typename T::value_type;
 *  \concept_des The value type in the link.
 *  \concept_req typename T::link_ptr
 *  \concept_des A pointer to a mutable value of \c T::link_type.
 *  \concept_req typename T::const_link_ptr
 *  \concept_des A pointer to a contant value of \c T::link_type.
 *  \concept_req T::key_type& key(Node<T>::ptr);
 *  \concept_des A function to return a \c T::key_type reference from a node
 *  pointer must exists.
 *  \concept_req const T::key_type& key(Node<T>::const_ptr);
 *  \concept_des A function to return a const \c T::key_type reference from a
 *  constant node pointer must exists.
 *  \concept_req T::value_type& value(Node<T>::ptr);
 *  \concept_des A function to return a \c T::value_type reference from a node
 *  pointer must exists.
 *  \concept_req const T::value_type& value(Node<T>::const_ptr);
 *  \concept_des A function to return a const \c T::value_type reference from a
 *  constant node pointer must exists.
 *  \concept_req T::link_ptr link(Node<T>::ptr);
 *  \concept_des A function to return a \c T::link_ptr from a node pointer must
 *  exists.
 *  \concept_req T::const_link_ptr link(Node<T>::const_ptr);
 *  \concept_des A function to return a \c T::const_link_type from a constant
 *  node pointer must exists.
 *
 *  This level of abstraction allows the same algorithms to be used for regular
 *  and intrusive containers, without loss of performance and without the use of
 *  a cumbersome syntax.
 *
 *  Currently, \ref Kdtree_link and \ref Relaxed_kdtree_link are models of
 *  LinkMode. These two types have the peculiarity to be both the link and the
 *  linking mode.
 */
struct LinkMode { };

/**
 *  Rank concept defines the model for a functor class used to represent the
 *  rank of a container, or the minimum number of dimensions in space expressed
 *  in all values stored in the container.
 *  \concept Definition of the Rank concept.
 *
 *  The models of Rank shall publicly provide the following interface:
 *  \concept_tab
 *  \concept_leg T
 *  \concept_des A model of Rank
 *  \concept_req dimension_type T::operator()() const
 *  \concept_des Returns the number of dimensions expressed in all values stored
 *  in the container.
 *  \concept_end
 *
 *  Values may be expressed in many more dimensions than the container in which
 *  they are stored. However, if a container as a rank of 3, therefore all the
 *  values in this container are expected to be represented in at least 3
 *  dimensions in space.
 *
 *  Models of this concepts are not normally used by the user, however, they are
 *  used internally by all containers. Their are two models of Rank in the
 *  library:
 *  \ul Static_rank, which holds a value of the number of dimension that is known
 *  at compile time, and will not change during program execution. That value is
 *  a template parameter.
 *  \l Dynamic_rank, which holds a variable value of the number of dimension,
 *  which defaults to 1, and which may change during execution of the program.
 *  \lu
 *
 *  Finally, it is useful to know that in the box\* familly of containers, such
 *  as boxset or boxmap, the model of Rank always holds twice the dimension
 *  value given in parameter. If your boxes are represented in 2 dimensions
 *  (like a rectangle), the Rank of the container will return 4. That is because
 *  box\* containers treat 2D boxes as a 4D point in space, giving rise to a
 *  double number of axis, for each of the low and high coordinate of a box.
 */
struct Rank { };

/**
 *  TrivialComparison concept defines the model for a functor class used to
 *  perform strict comparison between two values of a spatial container,
 *  over a single dimension. The functor shall return a boolean value equal
 *  true if the first value is strictly lower than the second value over that
 *  dimension.
 *  @concept Definition of the TrivialComparison concept.
 *
 *  The models of TrivialComparison shall publicly provide the following
 *  interface:
 *  \concept_tab
 *  \concept_leg T
 *  \concept_des A model of RegularComparison
 *  \concept_leg V
 *  \concept_des The key of a spatial container. E.g. in
 *  <tt>spatial::pointset<3, point></tt>, \c point is the value type.
 *  \concept_req bool T::operator()(spatial::dimension_type x, const V& a,
 *  const V&b) const
 *  \concept_des Returns true if <tt> a < b </tt> over dimension \c d.
 *  \concept_end
 *
 *  Models of this concept are required for comparison functors in pointset,
 *  dynamic_pointset and similar containers. Comparison functors in boxset,
 *  dynamic_boxset, and similar containers are required to be models of
 *  RegularComparison.
 */
struct TrivialComparison { };

/**
 *  RegularComparison concept defines the model for a functor used to perform a
 *  strict comparison between two values of a spatial container, over different
 *  dimensions. The functor shall return a boolean value equal true if the first
 *  value is strictly lower than the second value over that dimension.
 *  @concept Definition of the RegularComparison concept.
 *
 *  The models of RegularComparison shall also model TrivialComparison and publicly
 *  provide the following interface:
 *  \concept_tab
 *  \concept_leg T
 *  \concept_des A model of RegularComparison
 *  \concept_leg V
 *  \concept_des The key of a spatial container. E.g. in
 *  <tt>spatial::pointset<3, point></tt>, \c point is the value type.
 *  \concept_req bool T::operator()(spatial::dimension_type x, const V& a,
 *  const V&b) const
 *  \concept_des Returns true if <tt> a < b </tt> over dimension \c d.
 *  \concept_req bool T::operator()(spatial::dimension_type x, const V& a,
 *  spatial::dimension_type y, const V&b) const
 *  \concept_des Returns true if <tt> a(x) < b(y) </tt> where a(x) is the value
 *  of \c a over dimension \c x and \c b(y) is the value of \c b over dimension
 *  \c y.
 *  \concept_end
 *
 *  When comparing boxes during tree traversal for overlap or enclose ranges,
 *  the library often need to compare the lower bound of a box against the
 *  higher bound of a box over a particular axis. Since lower bounds and higher
 *  bounds are stored on different dimensions in the library, the functor need
 *  to be able to perform comparison on different dimension.
 *
 *  As you can guess, models of this concept are required for comparison
 *  functors in boxset, dynamic_boxset, and similar containers.
 */
struct RegularComparison : TrivialComparison { };

/**
 *  This concept defines the requirements for a predicate to be used in range
 *  queries. Range queries are used for orthogonal range search, overlaping or
 *  enclosing searches. The model of RangePredicate are used to match keys of
 *  the spatial containers against predefined intervals along each dimensions.
 *
 *  @concept Definition of the RangePredicate concept.
 *
 *  The models of RangePredicate shall publicly provide the following
 *  interfaces:
 *  \concept_tab
 *  \concept_leg P
 *  \concept_des A model of RangePredicate
 *  \concept_leg V
 *  \concept_des The key of a spatial container. E.g. in
 *  \concept_req spatial::relative_order P::operator()(spatial::dimension_type
 *  dim, const V& key, spatial::dimension_type rank) const
 *  \concept_des Returns \c spatial::below if \c key is below the interval
 *  considered along \c dim; \c spatial::above if \c key is above the interval
 *  being considered along \c dim; \c spatial::matching if \c key matches the
 *  interval being considered along \c dim. For a more complete explanation read
 *  the detailed description.
 *  \concept_end
 *
 *  The definition of a RangePredicate functor is generally not required. Before
 *  you define such predicate, consider using spatial::range(),
 *  spatial::open_range(), spatial::close_range(), spatial::overlapping(), or
 *  spatial::enclosing().
 *
 *  A model of RangePredicate generally represents an multi-dimension continuous
 *  interval which is going to be used for orthogonal range search. In order to
 *  provide a generic model of iteration over interval, \Spatial provides a
 *  tribool value spatial::relative_order to represent whether any key of the
 *  container is situated above, below or in the interval, for a given
 *  dimension.
 *
 *  For example, if you are storing integers in 1 dimension, and are interested
 *  in finding all values between 0 and 10 included, you could write the
 *  following predicate:
 *
 *  \code
 *  struct MySimplePredicate
 *  {
 *    spatial::relative_order
 *    operator()(dimension_type, const int& key, dimension_type)
 *    {
 *      using spatial;
 *      return key < 0 ? below : ( key > 10 ? above : matching );
 *    }
 *  };
 *  \endcode
 *
 *  In this very simple example, there are no more than one dimension being
 *  considered, so the first and last parameters are no used. As can be seen
 *  from the implementation of the predicate, any value between 0 and 10
 *  included for \c key, would result in the return value equal to
 *  spatial::matching. If the value of \c key is less than 0, the lower bound of
 *  the interval, then the return value is equal to spatial::below. If the value
 *  of \c key is more than 10, the return value is spatial::above.
 *
 *  It may not be appearing immediately from the example above, but there are
 *  several limitation, by design, for a model a RangePredicate:
 *  \ul Comparison have to be along the axes of your spaces, you can't compare
 *  along a complicated polygon or a circle: i.e. for an euclidian space of rank
 *  2, if the interval is closed, then the shape of the interval will be a box.
 *  \l The interval of comparison must be continuous. There cannot be holes in
 *  in the interval. If you \b must have a predicate with a hole, you have no
 *  choice but to split this predicate in several different predicate which have
 *  no holes, and make separated queries.
 *  \lu
 *
 *  Let's look at a more complex example, probably closer to reality. This
 *  predicate will work on any key that is a \c std::vector<double>, and
 *  matches only values that are within the slice [-1, 1] of the highest
 *  dimension:
 *
 *  \code
 *  struct HigherSlice
 *  {
 *    typedef std::vector<double> key_type;
 *
 *    spatial::relative_order
 *    operator()(dimension_type dim, const key_type& key, dimension_type rank)
 *    {
 *      using spatial;
 *      return rank - 1 == dim
 *        ? (key[dim] < -1.0 ? below : (key[dim] > 1.0 ? above : matching))
 *        : matching;
 *    }
 *  };
 *  \endcode
 *
 *  The value of \c rank is equal to the rank of the container used for the
 *  orthogonal range search. The value of \c dim is the current dimension being
 *  considered. It is always in the interval [0, rank).
 *
 *  More examples of predicates can be found in the example and the tutorial.
 */
struct RangePredicate { };
