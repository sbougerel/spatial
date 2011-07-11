/**
 *  TrivialComparison concept defines the model for a functor class used to
 *  perform strict comparison between two values of a spatial container,
 *  over a single dimension. The functor shall return a boolean value equal
 *  true if the first value is strictly lower than the second value over that
 *  dimension.
 *  @concept Definition of the TrivialComparison concept.
 *
 *  The models of TrivialComparison shall publicly provides the following
 *  signatures:
 *
 *  <table>
 *  <tr><td></td><td>Signature/Type</td><td>Description</td></tr>
 *  <tr><td>Legend</td><td>\c T</td><td>A model of TrivialComparison.</td></tr>
 *  <tr><td>Legend</td><td>\c V</td><td>The value type of a spatial container.
 *  E.g. in \c spatial::pointset<3, point>, \c point is the value type.
 *  </td></tr>
 *  <tr><td>Requirement</td>
 *  <td><tt>bool T::operator()(dimension_type d, const V& a, const V&b) const
 *  </tt></td><td>Returns true if <tt> a < b </tt> over dimension \c d.</td>
 *  </tr></table>
 *
 *  Models of this concept are required for comparison functors in pointset,
 *  dynamic_pointset and similar containers. Comparison functors in boxset,
 *  dynamic_boxset, and similar containers are required to be models of
 *  BoxComparison.
 */
struct TrivialComparison { };

/**
 *  BoxComparison concept defines the model for a functor used to perform a
 *  strict comparison between two values of a spatial container, over different
 *  dimensions. The functor shall return a boolean value equal true if the first
 *  value is strictly lower than the second value over that dimension.
 *  @concept Definition of the BoxComparison concept.
 *
 *  The models of BoxComparison shall also model TrivialComparison and publicly
 *  provides the following signatures:
 *
 *  <table>
 *  <tr><td></td><td>Signature/Type</td><td>Description</td></tr>
 *  <tr><td>Legend</td><td>T</td><td>A model of BoxComparison.</td></tr>
 *  <tr><td>Legend</td><td>V</td><td>The value type of a spatial container. E.g.
 *  in \c spatial::pointset<3, point>, \c point is the value type.
 *  </td></tr>
 *  <tr><td>Requirement</td>
 *  <td>bool T::operator()(dimension_type x, const V& a, dimension_type y,
 *  const V&b) const
 *  </td><td>Returns true if <tt> a(x) < b(y) </tt> where a(x) is the value of
 *  \c a over dimension \c x and \c b(y) is the value of \c b over dimension \c
 *  y.</td></tr></table>
 *
 *  When comparing boxes during tree traversal for overlap or enclose ranges,
 *  the library often need to compare the lower bound of a box against the
 *  higher bound of a box over a particular axis. Since lower bounds and higher
 *  bounds are stored on different dimensions in the library, the functor need
 *  to be able to perform comparison on different dimension.
 *
 *  As you can guess, models of this concept are required for comparison functors
 *  in boxset, dynamic_boxset, and similar containers.
 */
struct BoxComparison : TrivialComparison { };
