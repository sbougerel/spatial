// -*- C++ -*-

/**
 *  @file   verify.cpp
 *  @brief  Executes the series of test one by one and display simple messages
 *  for the outcome of the test.
 *
 *  Uses Boost.Test; needs to be compiled with -lboost_unit_test_framework.
 *
 *  - 2009-02-24 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 *  - (next change goes here)
 */

#include <cstdlib> // rand(), srand()
#include <memory> // std::allocator
#include <utility> // std::pair
#include <algorithm> // std::find() and others
#include <vector>
#include <tr1/array>
#include <limits>
#include <iomanip>

#define BOOST_TEST_MAIN // single (a bit) long test file
#include <boost/test/included/unit_test.hpp>

#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur
#include "../src/pointset.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE ( install_srand )
{
  // not sure where else to put this...
  srand((unsigned int)time(NULL));
}

////////////////////  TESTING MODULES FROM BASE TO HIGH ORDER  /////////////////
// 1. spatial.hpp (no tests at the moment)
// 2. spatial_assert.hpp (single macro with assert and a throw)
// 3. spatial_traits.hpp
// 4. spatial_details.hpp
// 5. spatial_tree.hpp
// 6. spatial_exceptions.hpp
// 7. spatial_function.hpp
// 8. spatial_kdtree_base.hpp
// 9. spatial_kdtree.hpp
// 10. spatial_mapping.hpp
// 11. spatial_range.hpp
// 12. spatial_geometry.hpp
// 13. spatial_neighbor.hpp
// 14. spatial_relaxed_kdtree.hpp (the rebalancing guy!)
// 15. pointset.hpp (define pointset, dynamic_pointset)
// 16. pointmap.hpp (define pointmap, variable_pointmap)
// 17. boxset.hpp (define boxset, dynamic_boxset)
// 18. boxmap.hpp (define boxmap, variable_boxmap)
// 19. spatial_intersect.hpp
// 20. spatial_include.hpp
// ... and one day, VP-trees as well may be...
////////////////////////////////////////////////////////////////////////////////

///////////////////////////  spatial_traits.hpp  ///////////////////////////////

BOOST_AUTO_TEST_CASE( test_traits_condition )
{
  typedef details::condition<true, std::pair<int, int>,
    std::allocator<int> >::type type1;
  type1 must_compile_1 = std::pair<int, int>();
  typedef details::condition<false, std::pair<int, int>,
    std::allocator<int> >::type type2;
  type2 must_compile_2 = std::allocator<int>();
}

BOOST_AUTO_TEST_CASE( test_node_traits )
{
  typedef details::node_traits<details::Kdtree_node<int> >
    ::invariant_category type1;
  type1 must_compile_1 = details::strict_invariant_tag();
  typedef details::node_traits<details::Relaxed_kdtree_node<int> >
    ::invariant_category type2;
  type2 must_compile_2 = details::relaxed_invariant_tag();
}

BOOST_AUTO_TEST_CASE( test_constant_required )
{
  typedef details::constant_required<int>::type type1;
  type1 must_compile_1 = std::tr1::true_type();
  typedef details::constant_required<std::pair<int, int> >::type type2;
  type2 must_compile_2 = std::tr1::false_type();
}

///////////////////////////  spatial_details.hpp  //////////////////////////////

typedef std::tr1::array<int, 2> point2d;

point2d zeros = { { 0, 0 } };
point2d ones = { { 1, 1 } };
point2d twos = { { 2, 2 } };
point2d threes = { { 3, 3 } };
point2d fours = { { 4, 4 } };

void swap(point2d& left, point2d& right)
{
  point2d tmp = right;
  right = left;
  left = tmp;
}

using details::Kdtree_base;
using details::Kdtree_node;

struct Empty_base_2D_fixture
{
  typedef Kdtree_base<details::Dynamic_rank, point2d,
		      Kdtree_node<point2d>, bracket_less<point2d>,
		      std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  Empty_base_2D_fixture() : kdtree(details::Dynamic_rank(2)) { }
};

struct closed_test_range
{
  relative_order
  operator()(dimension_type dim, const point2d& point) const
  {
    return ((point[dim] < 0)
	    ? below
	    : ((point[dim] > 1)
	       ? above
	       : matching));
  }
};

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_static )
{
  details::Static_rank<2> dim;
  BOOST_CHECK_EQUAL(incr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(dim, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(dim, 1), 0);
}

BOOST_AUTO_TEST_CASE( test_details_incr_decr_dim_dynamic )
{
  details::Dynamic_rank dim(2);
  BOOST_CHECK_EQUAL(incr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(incr_dim(dim, 1), 0);
  BOOST_CHECK_EQUAL(decr_dim(dim, 0), 1);
  BOOST_CHECK_EQUAL(decr_dim(dim, 1), 0);
}

BOOST_AUTO_TEST_CASE( test_details_template_swap )
{
  point2d z = zeros;
  point2d o = ones;
  BOOST_REQUIRE_NO_THROW
    (details::template_member_swap<point2d>::do_it(z, o));
  BOOST_CHECK_EQUAL(z[0], ones[0]);
  BOOST_CHECK_EQUAL(z[1], ones[1]);
  BOOST_CHECK_EQUAL(o[0], zeros[0]);
  BOOST_CHECK_EQUAL(o[1], zeros[1]);
}

BOOST_AUTO_TEST_CASE( test_details_less )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 1 } };
  bracket_less<point2d> cmp;
  BOOST_CHECK(details::less(cmp, 0, x, y));
  BOOST_CHECK(!details::less(cmp, 0, y, x));
  BOOST_CHECK(!details::less(cmp, 0, x, z));
  BOOST_CHECK(!details::less(cmp, 1, x, y));
  BOOST_CHECK(details::less(cmp, 1, y, x));
  BOOST_CHECK(!details::less(cmp, 1, x, x));
}

BOOST_AUTO_TEST_CASE( test_details_less_by_ref )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 1 } };
  bracket_less<point2d> cmp;
  BOOST_CHECK(details::less_by_ref(cmp, 0, x, y));
  BOOST_CHECK(!details::less_by_ref(cmp, 0, y, x));
  BOOST_CHECK(!details::less_by_ref(cmp, 1, x, y));
  BOOST_CHECK(details::less_by_ref(cmp, 1, y, x));
  BOOST_CHECK((&x < &z)
	      ? details::less_by_ref(cmp, 0, x, z)
	      : !details::less_by_ref(cmp, 0, x, z));
  BOOST_CHECK((&x < &z)
	      ? !details::less_by_ref(cmp, 0, z, x)
	      : details::less_by_ref(cmp, 0, z, x));
  BOOST_CHECK(!details::less_by_ref(cmp, 1, x, x));
}

BOOST_AUTO_TEST_CASE( test_details_match )
{
  // Rather than testing match, it's testing that closed_test_range is properly
  // written, which important for subsequent tests.
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d _x = { { -1, -1 } };
  point2d x_ = { { 2, 2 } };
  BOOST_CHECK(details::match(0, x, closed_test_range()) == matching);
  BOOST_CHECK(details::match(1, y, closed_test_range()) == matching);
  BOOST_CHECK(details::match(0, _x, closed_test_range()) == below);
  BOOST_CHECK(details::match(1, x_, closed_test_range()) == above);
}

BOOST_AUTO_TEST_CASE( test_details_match_all )
{
  Empty_base_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_all
	      (fix.kdtree.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_all
	      (fix.kdtree.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_all
	      (fix.kdtree.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_all
	      (fix.kdtree.rank(), w, closed_test_range()));
  BOOST_CHECK(!details::match_all
	      (fix.kdtree.rank(), _x, closed_test_range()));
  BOOST_CHECK(!details::match_all
	      (fix.kdtree.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_all
	      (fix.kdtree.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_any )
{
  Empty_base_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), x, closed_test_range()));
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), y, closed_test_range()));
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), z, closed_test_range()));
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), w, closed_test_range()));
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), _x, closed_test_range()));
  BOOST_CHECK(details::match_any
	      (fix.kdtree.rank(), y_, closed_test_range()));
  BOOST_CHECK(!details::match_any
	      (fix.kdtree.rank(), _w_, closed_test_range()));
}

BOOST_AUTO_TEST_CASE( test_details_match_most )
{
  Empty_base_2D_fixture fix;
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d z = { { 1, 0 } };
  point2d w = { { 0, 1 } };
  point2d _x = { { 0, -1 } };
  point2d y_ = { { 2, 0 } };
  point2d _w_ = { { 2, 2 } };
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 0, x, closed_test_range()));
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 1, y, closed_test_range()));
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 1, z, closed_test_range()));
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 0, w, closed_test_range()));
  BOOST_CHECK(!details::match_most
	      (fix.kdtree.rank(), 0, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 1, _x, closed_test_range()));
  BOOST_CHECK(details::match_most
	      (fix.kdtree.rank(), 0, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
	      (fix.kdtree.rank(), 1, y_, closed_test_range()));
  BOOST_CHECK(!details::match_most
	      (fix.kdtree.rank(), 0, _w_, closed_test_range()));
  BOOST_CHECK(!details::match_most
	      (fix.kdtree.rank(), 1, _w_, closed_test_range()));
}

/////////////////////////////  spatial_node.hpp  ///////////////////////////////

typedef std::pair<int, int> pair_type;
using details::Node_base;
using details::Relaxed_kdtree_node;

struct Node_base_empty_fixture
{
  Node_base header;
  Node_base_empty_fixture()
  {
    header.parent = &header;
    header.left = &header;
    header.right = &header;
  }
};

struct Five_Node_base_fixture
{
  /*         H
             |
	     Root
	     /   \
	     L     R
	     /  \
	     LL    LR       */
  Node_base header;
  Node_base node_root;
  Node_base node_left;
  Node_base node_left_left;
  Node_base node_left_right;
  Node_base node_right;
  Five_Node_base_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
  }
};

struct Five_Kdtree_node_fixture
{
  /*         H
             |
	     (2,2)
	     /   \
	     (1,1)   (3,3)
	     /  \
	     (0,0)  (1,1)   */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Kdtree_node<point2d> node_root;
  Kdtree_node<point2d> node_left;
  Kdtree_node<point2d> node_left_left;
  Kdtree_node<point2d> node_left_right;
  Kdtree_node<point2d> node_right;
  Five_Kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.key_field = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.key_field = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.key_field = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.key_field = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.key_field = zeros;
  }
};

struct Five_Relaxed_kdtree_node_fixture
{
  /*         H
             |
	   (2,2)5
	   /   \
      (1,1)3  (3,3)1
       /  \
  (0,0)1  (1,1)1       */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Relaxed_kdtree_node<point2d> node_root;
  Relaxed_kdtree_node<point2d> node_left;
  Relaxed_kdtree_node<point2d> node_left_left;
  Relaxed_kdtree_node<point2d> node_left_right;
  Relaxed_kdtree_node<point2d> node_right;
  Five_Relaxed_kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.weight = 5;
    node_root.key_field = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.weight = 3;
    node_left.key_field = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.weight = 1;
    node_right.key_field = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.weight = 1;
    node_left_right.key_field = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.weight = 1;
    node_left_left.key_field = zeros;
  }
};

BOOST_AUTO_TEST_CASE( test_empty_header )
{
  Node_base_empty_fixture fix;
  BOOST_CHECK(Node_base::header(&fix.header));
}

BOOST_AUTO_TEST_CASE( test_5_node )
{
  Five_Node_base_fixture fix;
  BOOST_CHECK(Node_base::header(&fix.header));
  BOOST_CHECK(!Node_base::header(&fix.node_root));
  BOOST_CHECK(!Node_base::header(&fix.node_left));
  BOOST_CHECK(!Node_base::header(&fix.node_right));
  BOOST_CHECK(!Node_base::header(&fix.node_left_right));
  BOOST_CHECK(!Node_base::header(&fix.node_left_left));
  BOOST_CHECK(Node_base::minimum(&fix.node_root) == &fix.node_left_left);
  BOOST_CHECK(Node_base::maximum(&fix.node_root) == &fix.node_right);
  Node_base::Base_ptr node = &fix.node_left_left;
  BOOST_REQUIRE((node = Node_base::increment(node)) == &fix.node_left);
  BOOST_REQUIRE((node = Node_base::increment(node)) == &fix.node_left_right);
  BOOST_REQUIRE((node = Node_base::increment(node)) == &fix.node_root);
  BOOST_REQUIRE((node = Node_base::increment(node)) == &fix.node_right);
  BOOST_REQUIRE((node = Node_base::increment(node)) == &fix.header);
  BOOST_REQUIRE((node = Node_base::decrement(node)) == &fix.node_right);
  BOOST_REQUIRE((node = Node_base::decrement(node)) == &fix.node_root);
  BOOST_REQUIRE((node = Node_base::decrement(node)) == &fix.node_left_right);
  BOOST_REQUIRE((node = Node_base::decrement(node)) == &fix.node_left);
  BOOST_REQUIRE((node = Node_base::decrement(node)) == &fix.node_left_left);
  Node_base::Const_Base_ptr cnode = &fix.node_root;
  BOOST_REQUIRE((cnode = Node_base::preorder_increment(cnode))
		== &fix.node_left);
  BOOST_REQUIRE((cnode = Node_base::preorder_increment(cnode))
		== &fix.node_left_left);
  BOOST_REQUIRE((cnode = Node_base::preorder_increment(cnode))
		== &fix.node_left_right);
  BOOST_REQUIRE((cnode = Node_base::preorder_increment(cnode))
		== &fix.node_right);
  BOOST_REQUIRE((cnode = Node_base::preorder_increment(cnode))
		== &fix.header);
}

BOOST_AUTO_TEST_CASE( test_swap_node )
{
  {
    Five_Node_base_fixture fix; // swap with non-root
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr right = &fix.node_right;
    swap(*left_left, *right);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.left == 0);
    BOOST_CHECK(fix.node_left_left.right == 0);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.left == &fix.node_left);
    BOOST_CHECK(fix.node_root.parent == &fix.header);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.node_root);
  }
  {
    Five_Node_base_fixture fix; // swap with non-root, invert args
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr right = &fix.node_right;
    swap(*right, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.left == 0);
    BOOST_CHECK(fix.node_left_left.right == 0);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.left == &fix.node_left);
    BOOST_CHECK(fix.node_root.parent == &fix.header);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.node_root);
  }
  {
    Five_Node_base_fixture fix; // swap with root
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr root = &fix.node_root;
    swap(*left_left, *root);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.left == &fix.header);
    BOOST_CHECK(fix.header.right == &fix.node_right);
  }
  {
    Five_Node_base_fixture fix; // swap with root, invert args
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr root = &fix.node_root;
    swap(*root, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.left == &fix.header);
    BOOST_CHECK(fix.header.right == &fix.node_right);
  }
  {
    Five_Node_base_fixture fix; // swap with left child
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*left_left, *left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_left == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_left == fix.node_left_right.parent);
  }
  {
    Five_Node_base_fixture fix; // swap with left child, invert args
    Node_base::Base_ptr left_left = &fix.node_left_left;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*left, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_left == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_left == fix.node_left_right.parent);
  }
  {
    Five_Node_base_fixture fix; // swap with right child
    Node_base::Base_ptr left_right = &fix.node_left_right;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*left_right, *left);
    BOOST_CHECK(left_right == &fix.node_left_right);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_right.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_right.right == &fix.node_left);
    BOOST_CHECK(fix.node_left_right.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_right == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_right == fix.node_left_left.parent);
  }
  {
    Five_Node_base_fixture fix; // swap with right child, invert args
    Node_base::Base_ptr left_right = &fix.node_left_right;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*left, *left_right);
    BOOST_CHECK(left_right == &fix.node_left_right);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_right.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_right.right == &fix.node_left);
    BOOST_CHECK(fix.node_left_right.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_right == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_right == fix.node_left_left.parent);
  }
  {
    Five_Node_base_fixture fix; // swap root with left child
    Node_base::Base_ptr root = &fix.node_root;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*left, *root);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left);
    BOOST_CHECK(&fix.node_root == fix.node_left_left.parent);
    BOOST_CHECK(&fix.node_root == fix.node_left_right.parent);
  }
  {
    Five_Node_base_fixture fix; // swap root with left child, invert args
    Node_base::Base_ptr root = &fix.node_root;
    Node_base::Base_ptr left = &fix.node_left;
    swap(*root, *left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left);
    BOOST_CHECK(&fix.node_root == fix.node_left_left.parent);
    BOOST_CHECK(&fix.node_root == fix.node_left_right.parent);
  }
  {
    Five_Node_base_fixture fix; // swap root with right child
    Node_base::Base_ptr root = &fix.node_root;
    Node_base::Base_ptr right = &fix.node_right;
    swap(*right, *root);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_right.left == &fix.node_left);
    BOOST_CHECK(fix.node_right.right == &fix.node_root);
    BOOST_CHECK(fix.node_right.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_right);
    BOOST_CHECK(fix.header.parent == &fix.node_right);
    BOOST_CHECK(&fix.node_right == fix.node_left.parent);
  }
  {
    Five_Node_base_fixture fix; // swap root with right child, invert args
    Node_base::Base_ptr root = &fix.node_root;
    Node_base::Base_ptr right = &fix.node_right;
    swap(*root, *right);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_right.left == &fix.node_left);
    BOOST_CHECK(fix.node_right.right == &fix.node_root);
    BOOST_CHECK(fix.node_right.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_right);
    BOOST_CHECK(fix.header.parent == &fix.node_right);
    BOOST_CHECK(&fix.node_right == fix.node_left.parent);
  }
}

BOOST_AUTO_TEST_CASE( test_swap_kdtree_node )
{ // swap only position (tested before). Do not swap values!
  Five_Kdtree_node_fixture fix;
  Kdtree_node<point2d>* left_left = &fix.node_left_left;
  Kdtree_node<point2d>* right = &fix.node_right;
  swap(*left_left, *right);
  BOOST_CHECK(fix.node_left_left.key_field == zeros);
  BOOST_CHECK(fix.node_right.key_field == threes);
}

BOOST_AUTO_TEST_CASE( test_swap_relaxed_kdtree_node )
{ // swap position and weight, but not values!
  Five_Relaxed_kdtree_node_fixture fix;
  Relaxed_kdtree_node<point2d>* left = &fix.node_left;
  Relaxed_kdtree_node<point2d>* right = &fix.node_right;
  swap(*left, *right);
  BOOST_CHECK_EQUAL(fix.node_left.weight, 1);
  BOOST_CHECK_EQUAL(fix.node_right.weight, 3);
  BOOST_CHECK(fix.node_left.key_field == ones);
  BOOST_CHECK(fix.node_right.key_field == threes);
}

BOOST_AUTO_TEST_CASE( test_Node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Node_iterator<pair_type,
      Kdtree_node<pair_type>, false> iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.key_field = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    // should compile: non-const iterator!
    BOOST_CHECK((*iter = test_object) == test_object);
    BOOST_CHECK_EQUAL((iter->first = 3), 3);
  }
  {
    typedef details::Node_iterator<pair_type,
      Relaxed_kdtree_node<pair_type>, false> iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.key_field = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    // should compile: non-const iterator!
    BOOST_CHECK((*iter = test_object) == test_object);
    BOOST_CHECK_EQUAL((iter->first = 3), 3);
  }
  {
    // Make sure that constant iterator does not invalidate anything
    Five_Kdtree_node_fixture fix;
    typedef details::Node_iterator<point2d, Kdtree_node<point2d>, true> iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Node_iterator<point2d, Kdtree_node<point2d>, false> iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_right);
    BOOST_REQUIRE((--iter).node == &fix.node_root);
    BOOST_REQUIRE((iter++).node == &fix.node_root);
    BOOST_REQUIRE(iter.node == &fix.node_right);
    BOOST_REQUIRE((iter--).node == &fix.node_right);
    BOOST_REQUIRE(iter.node == &fix.node_root);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Const_Node_iterator<pair_type,
      Kdtree_node<pair_type>, false> iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.key_field = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    typedef details::Const_Node_iterator<pair_type,
      Relaxed_kdtree_node<pair_type>, false> iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.key_field = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    // Make sure that constant iterator does not invalidate anything
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<point2d, Kdtree_node<point2d>, true> iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<point2d, Kdtree_node<point2d>, false> iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_right);
    BOOST_REQUIRE((--iter).node == &fix.node_root);
    BOOST_REQUIRE((iter++).node == &fix.node_root);
    BOOST_REQUIRE(iter.node == &fix.node_right);
    BOOST_REQUIRE((iter--).node == &fix.node_right);
    BOOST_REQUIRE(iter.node == &fix.node_root);
  }
}

BOOST_AUTO_TEST_CASE( test_Preorder_node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Preorder_node_iterator<pair_type,
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.key_field = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<point2d, Kdtree_node<point2d> > iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_left);
    BOOST_REQUIRE((iter++).node == &fix.node_left);
    BOOST_REQUIRE(iter.node == &fix.node_left_left);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<point2d, Kdtree_node<point2d> > iterator;
    iterator a(&fix.node_root);
    iterator b(&fix.node_root);
    BOOST_CHECK(a == b);
    iterator c(&fix.node_left);
    BOOST_CHECK(a != c);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<point2d, Kdtree_node<point2d> > iterator;
    iterator a(&fix.node_root);
    iterator b(a);
    BOOST_CHECK(a == b);
  }
}

/////////////////////////////  spatial_exceptions.hpp  /////////////////////////

BOOST_AUTO_TEST_CASE( text_except_check_dimension )
{
  Empty_base_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_dimension_argument(fix.kdtree.dimension(), 0));
  BOOST_CHECK_THROW(except::check_dimension_argument(fix.kdtree.dimension(), 2),
		    invalid_dimension_argument);
}

BOOST_AUTO_TEST_CASE( text_except_check_node )
{
  {
    Node_base::Base_ptr ptr = 0;
    BOOST_CHECK_THROW(except::check_node_argument(ptr),
		      invalid_node_argument);
  }
  {
    Five_Node_base_fixture fix;
    BOOST_CHECK_NO_THROW(except::check_node_argument(&fix.node_root));
    BOOST_CHECK_THROW(except::check_node_argument(&fix.header),
		      invalid_node_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_iterator )
{
  {
    details::Node_iterator<point2d, Kdtree_node<point2d>, false> i(0);
    BOOST_CHECK_THROW(except::check_iterator_argument(i.node),
		      invalid_iterator_argument);
  }
  {
    Five_Kdtree_node_fixture fix;
    details::Node_iterator<point2d, Kdtree_node<point2d>, false> i(&fix.node_root);
    details::Node_iterator<point2d, Kdtree_node<point2d>, false>
      j(static_cast<Kdtree_node<point2d>*>
	(static_cast<Node_base*>(&fix.header)));
    BOOST_CHECK_NO_THROW(except::check_iterator_argument(i.node));
    BOOST_CHECK_THROW(except::check_iterator_argument(j.node),
		      invalid_iterator_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_empty_tree )
{
  Empty_base_2D_fixture fix;
  BOOST_CHECK_THROW(except::check_empty_container_argument(fix.kdtree),
		    invalid_empty_container_argument);
}

BOOST_AUTO_TEST_CASE( text_except_check_range )
{
  Empty_base_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_open_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_closed_range_bounds(fix.kdtree, zeros, zeros));
  BOOST_CHECK_THROW(except::check_open_range_bounds(fix.kdtree, zeros, zeros),
		    invalid_open_range_bounds);
  BOOST_CHECK_THROW(except::check_range_bounds(fix.kdtree, zeros, zeros),
		    invalid_range_bounds);
  BOOST_CHECK_THROW(except::check_closed_range_bounds(fix.kdtree, ones, zeros),
		    invalid_closed_range_bounds);
}

BOOST_AUTO_TEST_CASE( text_except_check_rank )
{
  Empty_base_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_rank_argument(fix.kdtree.dimension(), 1));
  BOOST_CHECK_NO_THROW(except::check_rank_argument(fix.kdtree.dimension(), 2));
  BOOST_CHECK_THROW(except::check_rank_argument(fix.kdtree.dimension(), 3),
		    invalid_rank_argument);
}

/////////////////////////////  spatial_function.hpp  ///////////////////////////

struct point2d_paren
  : std::tr1::array<int, 2>
{
  std::tr1::array<int, 2>::reference
  operator()(size_type n)
  { return std::tr1::array<int, 2>::operator[](n); }

  std::tr1::array<int, 2>::const_reference
  operator()(size_type n) const
  { return std::tr1::array<int, 2>::operator[](n); }
};

template <typename Argument, typename Return>
struct at_accessor
{
  Return operator()(dimension_type dim, const Argument& arg) const
  { return arg.at(dim); }

  Return& operator()(dimension_type dim, Argument& arg) const
  { return arg.at(dim); }
};

BOOST_AUTO_TEST_CASE( test_access_less )
{
  // This test is hard to craft, how to make sure I'm test "less" and not the
  // accessor?
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  accessor_less<at_accessor<point2d, int>, point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(!compare(0, x, x));
}

BOOST_AUTO_TEST_CASE( test_bracket_less )
{
  // This has been used before this test was carried on, but at least we will
  // know here that something could fail.
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  bracket_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(!compare(0, x, x));
}

BOOST_AUTO_TEST_CASE( test_iterator_less )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  iterator_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(!compare(0, x, x));
}

BOOST_AUTO_TEST_CASE( test_paren_less )
{
  point2d_paren x; x[0] = 0; x[1] = 1;
  point2d_paren y; y[0] = 1; y[1] = 0;
  paren_less<point2d_paren> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(!compare(0, x, x));
}

BOOST_AUTO_TEST_CASE( test_equal_bounds )
{
  point2d t = { { 1, 1 } };
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 2 } };
  point2d w = { { 2, 0 } };
  Empty_base_2D_fixture fix;
  // Checking this compiles
  equal_bounds<point2d, bracket_less<point2d> > bounds
    = make_equal_bounds(fix.kdtree, t);
  BOOST_CHECK(bounds(0, t) == matching);
  BOOST_CHECK(bounds(1, t) == matching);
  BOOST_CHECK(bounds(0, x) == below);
  BOOST_CHECK(bounds(1, x) == matching);
  BOOST_CHECK(bounds(0, y) == matching);
  BOOST_CHECK(bounds(1, y) == below);
  BOOST_CHECK(bounds(0, z) == below);
  BOOST_CHECK(bounds(1, z) == above);
  BOOST_CHECK(bounds(0, w) == above);
  BOOST_CHECK(bounds(1, w) == below);
}

BOOST_AUTO_TEST_CASE( test_open_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 1 } };
  point2d y = { { 3, 2 } };
  Empty_base_2D_fixture fix;
  // Checking this compiles
  open_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_open_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l) == below);
  BOOST_CHECK(bounds(1, l) == below);
  BOOST_CHECK(bounds(0, h) == above);
  BOOST_CHECK(bounds(1, h) == above);
  BOOST_CHECK(bounds(0, x) == matching);
  BOOST_CHECK(bounds(1, x) == below);
  BOOST_CHECK(bounds(0, y) == above);
  BOOST_CHECK(bounds(1, y) == matching);
}

BOOST_AUTO_TEST_CASE( test_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 3, 2 } };
  point2d z = { { 0, 0 } };
  Empty_base_2D_fixture fix;
  // Checking this compiles
  range_bounds<point2d, bracket_less<point2d> > bounds
    = make_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l) == matching);
  BOOST_CHECK(bounds(1, l) == matching);
  BOOST_CHECK(bounds(0, h) == above);
  BOOST_CHECK(bounds(1, h) == above);
  BOOST_CHECK(bounds(0, x) == matching);
  BOOST_CHECK(bounds(1, x) == below);
  BOOST_CHECK(bounds(0, y) == above);
  BOOST_CHECK(bounds(1, y) == matching);
  BOOST_CHECK(bounds(0, z) == below);
  BOOST_CHECK(bounds(1, z) == below);
}

BOOST_AUTO_TEST_CASE( test_closed_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 4, 2 } };
  point2d z = { { 0, 0 } };
  point2d w = { { 4, 4 } };
  Empty_base_2D_fixture fix;
  // Checking this compiles
  closed_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_closed_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l) == matching);
  BOOST_CHECK(bounds(1, l) == matching);
  BOOST_CHECK(bounds(0, h) == matching);
  BOOST_CHECK(bounds(1, h) == matching);
  BOOST_CHECK(bounds(0, x) == matching);
  BOOST_CHECK(bounds(1, x) == below);
  BOOST_CHECK(bounds(0, y) == above);
  BOOST_CHECK(bounds(1, y) == matching);
  BOOST_CHECK(bounds(0, z) == below);
  BOOST_CHECK(bounds(1, z) == below);
  BOOST_CHECK(bounds(0, w) == above);
  BOOST_CHECK(bounds(1, w) == above);
}

///////////////////////////  spatial_kdtree_base.hpp  //////////////////////////

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_base )
{
  // Up to now, we only knew it compiled
  Empty_base_2D_fixture fix;
  BOOST_CHECK(fix.kdtree.end().node->left == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->right == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->parent == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.begin().node == fix.kdtree.end().node); // leftmost
  BOOST_CHECK(fix.kdtree.cbegin().node == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.empty());
  BOOST_CHECK(fix.kdtree.begin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cbegin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cbegin() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.cend() == fix.kdtree.end());
  BOOST_CHECK(fix.kdtree.rbegin() == fix.kdtree.rend());
  BOOST_CHECK(fix.kdtree.crbegin() == fix.kdtree.rend());
  BOOST_CHECK(fix.kdtree.crend() == fix.kdtree.rend());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_base_clear )
{
  Empty_base_2D_fixture fix;
  BOOST_REQUIRE_NO_THROW(fix.kdtree.clear());
  BOOST_CHECK(fix.kdtree.cend().node->left == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->right == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->parent == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.begin().node->right == fix.kdtree.end().node); // leftmost
  BOOST_CHECK(fix.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_base_swap )
{
  Empty_base_2D_fixture fix1;
  Empty_base_2D_fixture fix2;
  Empty_base_2D_fixture::kdtree_type::iterator iter1 = fix1.kdtree.end();
  Empty_base_2D_fixture::kdtree_type::iterator iter2 = fix2.kdtree.end();
  swap(fix1.kdtree, fix2.kdtree);
  BOOST_CHECK(fix1.kdtree.end() == iter1);
  BOOST_CHECK(fix2.kdtree.end() == iter2);
  BOOST_CHECK(fix1.kdtree.empty());
  BOOST_CHECK(fix2.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_base_max_size )
{
  Empty_base_2D_fixture fix;
  std::allocator<Kdtree_node<point2d> > node_alloc;
  BOOST_CHECK(fix.kdtree.max_size() == node_alloc.max_size());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_base_accessor )
{
  Empty_base_2D_fixture fix;
  std::allocator<Kdtree_node<point2d> > node_alloc;
  std::allocator<point2d> alloc;
  details::Dynamic_rank rank;
  bracket_less<point2d> compare;
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), 2);
  BOOST_CHECK_NO_THROW(rank = fix.kdtree.rank());
  BOOST_CHECK_NO_THROW(compare = fix.kdtree.compare());
  BOOST_CHECK_NO_THROW(alloc = fix.kdtree.allocator());
  BOOST_CHECK_NO_THROW(node_alloc = fix.kdtree.node_allocator());
}

//////////////////////////////  spatial_kdtree.hpp  ////////////////////////////

using details::Kdtree;
struct Empty_Kdtree_2D_fixture
{
  typedef Kdtree<details::Dynamic_rank, point2d,
		 bracket_less<point2d>,
		 std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  Empty_Kdtree_2D_fixture() : kdtree(details::Dynamic_rank(2)) { }
};

BOOST_AUTO_TEST_CASE( test_empty_Kdtree )
{
  Empty_Kdtree_2D_fixture fix;
  BOOST_CHECK(fix.kdtree.empty());
  BOOST_CHECK(fix.kdtree.begin() == fix.kdtree.end());
  // The rest was tested above...
}

BOOST_AUTO_TEST_CASE( test_Kdtree_insert_100_iterate_forward )
{
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
	= points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20; // to increase chances of collisions
      (*i)[1] = rand() % 20;
      BOOST_REQUIRE_NO_THROW(fix.kdtree.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
    }
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::iterator i
	= fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
    {
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
		  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_TEST_CHECKPOINT("Finding element " << count);
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_Kdtree_insert_100_iterate_backward )
{
  typedef Empty_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
	= points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20;
      (*i)[1] = rand() % 20;
      iterator result; // result iterator must be useful too
      BOOST_REQUIRE_NO_THROW(result = fix.kdtree.insert(*i));
      BOOST_TEST_CHECKPOINT("Adding element " << (i - points.begin()));
      BOOST_CHECK_EQUAL((*i)[0], (*result)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*result)[1]);
    }
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::reverse_iterator i
	= fix.kdtree.rbegin(); i != fix.kdtree.rend(); ++i)
    {
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
		  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_TEST_CHECKPOINT("Finding element " << count);
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

/////////////////////////////  spatial_mapping.hpp  ////////////////////////////

struct pair_less
{
  bool
  operator()(dimension_type dim, const pair_type& a, const pair_type& b) const
  {
    switch(dim)
      {
      case 0: return (a.first < b.first);
      case 1: return (a.second < b.second);
      default: throw std::logic_error("dim is greater than 1");
      }
  }
};

struct Pair_Kdtree_fixture
{
  typedef Kdtree<details::Dynamic_rank, pair_type,
		 pair_less, std::allocator<pair_type>, false> kdtree_type;
  kdtree_type kdtree;
  Pair_Kdtree_fixture() : kdtree(details::Dynamic_rank(2))
  {
    pair_type p(1, 2);
    kdtree.insert(p);
  }
};

struct Unbalanced_right_Kdtree_fixture
{
  typedef Kdtree<details::Dynamic_rank, pair_type,
		 pair_less, std::allocator<pair_type>, false> kdtree_type;
  kdtree_type kdtree;
  Unbalanced_right_Kdtree_fixture()
  : kdtree(details::Dynamic_rank(2))
  {
    pair_type a(1, 2); kdtree.insert(a);
    pair_type b(3, 4); kdtree.insert(b);
    pair_type c(5, 6); kdtree.insert(c);
    pair_type d(7, 8); kdtree.insert(d);
  }
};

struct Unbalanced_left_Kdtree_fixture
{
  typedef Kdtree<details::Dynamic_rank, pair_type,
		 pair_less, std::allocator<pair_type>, false> kdtree_type;
  kdtree_type kdtree;
  Unbalanced_left_Kdtree_fixture()
  : kdtree(details::Dynamic_rank(2))
  {
    pair_type a(7, 8); kdtree.insert(a);
    pair_type b(5, 6); kdtree.insert(b);
    pair_type c(3, 4); kdtree.insert(c);
    pair_type d(1, 2); kdtree.insert(d);
  }
};

struct Hundred_Kdtree_2D_fixture
{
  typedef Kdtree<details::Dynamic_rank, point2d,
		 bracket_less<point2d>,
		 std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  Hundred_Kdtree_2D_fixture() : kdtree(details::Dynamic_rank(2))
  {
    for(int i = 0; i != 100; ++i)
    {
      point2d p;
      p[0] = rand() % 20;
      p[1] = rand() % 20;
      kdtree.insert(p);
    }
  }
};

struct triple
{
  int x; int y; int z;
  triple() { }
  triple(int x, int y, int z) : x(x), y(y), z(z) { }
};

struct triple_less
{
  bool
  operator()(dimension_type dim, const triple& a, const triple& b) const
  {
    switch(dim)
      {
      case 0: return (a.x < b.x);
      case 1: return (a.y < b.y);
      case 2: return (a.z < b.z);
      default: throw std::logic_error("dim is greater than 2");
      }
  }
};

struct Twenty_Kdtree_3D_fixture
{
  typedef Kdtree<details::Dynamic_rank, triple,
		 triple_less, std::allocator<triple>, false> kdtree_type;
  kdtree_type kdtree;
  Twenty_Kdtree_3D_fixture() : kdtree(details::Dynamic_rank(3))
  {
    for(int i = 0; i != 20; ++i)
    {
      triple t;
      t.x = rand() % 10000000 - 5000000;
      t.y = rand() % 10000000 - 5000000;
      t.z = rand() % 10000000 - 5000000;
      kdtree.insert(t);
    }
  }
};

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_equal )
{
  Pair_Kdtree_fixture fix;
  {
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it1
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it2
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_deference )
{
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> itr
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(((*itr).first = 3), 3);
    BOOST_CHECK_EQUAL(((*itr).second = 4), 4);
    BOOST_CHECK_EQUAL((*itr).first, 3);
    BOOST_CHECK_EQUAL((*itr).second, 4);
    BOOST_CHECK_EQUAL((itr->first = 5), 5);
    BOOST_CHECK_EQUAL((itr->second = 6), 6);
    BOOST_CHECK_EQUAL(itr->first, 5);
    BOOST_CHECK_EQUAL(itr->second, 6);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> itr
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_deference )
{
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> itr
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> itr
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_minimum )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, false> iter_type;
    iter_type iter;
    int count = 0;
    int min_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_Kdtree_2D_fixture::kdtree_type::iterator i
	  = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
	int tmp = (*i)[mapping_dim];
	if (tmp < min_value) { min_value = tmp; }
	++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, true> iter_type;
    iter_type iter;
    int count = 0;
    int min_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_Kdtree_2D_fixture::kdtree_type::iterator i
	  = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
	int tmp = (*i)[mapping_dim];
	if (tmp < min_value) { min_value = tmp; }
	++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 2); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 2); // should be (1, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_maximum )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, false> iter_type;
    iter_type iter;
    int count = 0;
    int max_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_Kdtree_2D_fixture::kdtree_type::iterator i
	  = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
	int tmp = (*i)[mapping_dim];
	if (tmp > max_value) { max_value = tmp; }
	++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, true> iter_type;
    iter_type iter;
    int count = 0;
    int max_value = (*fix.kdtree.begin())[mapping_dim];
    for(Hundred_Kdtree_2D_fixture::kdtree_type::iterator i
	  = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
      {
	int tmp = (*i)[mapping_dim];
	if (tmp > max_value) { max_value = tmp; }
	++count;
      }
    BOOST_CHECK_EQUAL(count, 100);
    BOOST_REQUIRE_NO_THROW(iter = iter_type::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 8); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 8); // should be (1, 2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_increment )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, false> iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    int count = 0;
    int tmp = (*iter)[mapping_dim];
    for (; iter != end; ++iter)
    {
      BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
      tmp = (*iter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, true> iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    int count = 0;
    int tmp = (*iter)[mapping_dim];
    for (; iter != end; ++iter)
    {
      BOOST_CHECK_LE(tmp, (*iter)[mapping_dim]);
      tmp = (*iter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    ++iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    ++iter;
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    ++iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_increment )
{
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> end
      (fix.kdtree.rank(), fix.kdtree.compare(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(++it == end);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> end
      (fix.kdtree.rank(), fix.kdtree.compare(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(it++ != end);
    BOOST_CHECK(it == end);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_decrement )
{
  {
    dimension_type mapping_dim = 0;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, false> iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    typedef std::reverse_iterator<iter_type> reverse_iter_type;
    reverse_iter_type rend(iter);
    reverse_iter_type riter(end);
    int count = 0;
    int tmp = (*riter)[mapping_dim];
    for (; riter != rend; ++riter)
    {
      BOOST_CHECK_GE(tmp, (*riter)[mapping_dim]);
      tmp = (*riter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, true> iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    typedef std::reverse_iterator<iter_type> reverse_iter_type;
    reverse_iter_type rend(iter);
    reverse_iter_type riter(end);
    int count = 0;
    int tmp = (*riter)[mapping_dim];
    for (; riter != rend; ++riter)
    {
      BOOST_CHECK_GE(tmp, (*riter)[mapping_dim]);
      tmp = (*riter)[mapping_dim];
      BOOST_REQUIRE_LE(++count, 100);
    }
    BOOST_CHECK_EQUAL(count, 100);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    --iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
			    (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7);
    BOOST_CHECK_EQUAL(iter->second, 8);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 5);
    BOOST_CHECK_EQUAL(iter->second, 6);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 3);
    BOOST_CHECK_EQUAL(iter->second, 4);
    --iter;
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    --iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_decrement )
{
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> it
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, false> end
      (fix.kdtree.rank(), fix.kdtree.compare(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(--end == it);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> it
      (fix.kdtree.rank(), fix.kdtree.compare(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type,
      Kdtree_node<pair_type>, pair_less, true> end
      (fix.kdtree.rank(), fix.kdtree.compare(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(end-- != it);
    BOOST_CHECK(it == end);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_lower_bound )
{
  // return the smallest element in set that is greater or equal to key
  // test with high density and oob values
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, point2d,
      Kdtree_node<point2d>, bracket_less<point2d>, false> iter_type;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
	BOOST_CHECK_GE((*iter)[mapping_dim], flag[mapping_dim]);
	if (iter != begin) // same as above
	  {
	    iter_type tmp = iter;
	    BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
	    BOOST_CHECK_LT((*tmp)[mapping_dim], flag[mapping_dim]);
	  }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    dimension_type mapping_dim = 2;  // triple::z
    Twenty_Kdtree_3D_fixture fix;
    typedef details::Mapping_iterator<details::Dynamic_rank, triple,
      Kdtree_node<triple>, triple_less, false> iter_type;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node->parent), flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
	BOOST_CHECK_GE(iter->z, flag.z);
	if (iter != begin) // same as above
	  {
	    iter_type tmp = iter;
	    BOOST_CHECK_LT((--tmp)->z, iter->z);
	    BOOST_CHECK_LT(tmp->z, flag.z);
	  }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Mapping_iterator_upper_bound )
{
  // return the smallest element in set that is strictly greater than key
  // test with high density and oob values
  {
    typedef details::Const_Mapping_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d>, false> mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<point2d>*>
			    (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are extremely low 1/(2^100)
      {
	BOOST_CHECK_GT((*iter)[mapping_dim], flag[mapping_dim]);
	if (iter != begin) // same as above
	  {
	    mapping_iterator tmp = iter;
	    BOOST_CHECK_LT((*(--tmp))[mapping_dim], (*iter)[mapping_dim]);
	    BOOST_CHECK_LE((*tmp)[mapping_dim], flag[mapping_dim]);
	  }
      }
    BOOST_CHECK(low_iter == begin);
    BOOST_CHECK(high_iter == end);
  }
  // test with high dispersion
  {
    typedef details::Const_Mapping_iterator
      <Twenty_Kdtree_3D_fixture::kdtree_type::rank_type,
      Twenty_Kdtree_3D_fixture::kdtree_type::key_type,
      Twenty_Kdtree_3D_fixture::kdtree_type::node_type,
      triple_less, false> mapping_iterator;
    dimension_type mapping_dim = 2;  // triple::z
    Twenty_Kdtree_3D_fixture fix;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node->parent), flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, 0, static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
			   (fix.kdtree.rank(),
			    fix.kdtree.compare(),
			    mapping_dim, details::decr_dim
			    (fix.kdtree.rank(), 0),
			    static_cast<Kdtree_node<triple>*>
			    (fix.kdtree.end().node)));
    if (iter != end) // chances that this is false are low 1/(2^20)
      {
	BOOST_CHECK_GT(iter->z, flag.z);
	if (iter != begin) // same as above
	  {
	    mapping_iterator tmp = iter;
	    BOOST_CHECK_LT((--tmp)->z, iter->z);
	    BOOST_CHECK_LE(tmp->z, flag.z);
	  }
      }
  }
}

////  spatial_kdtree.hpp (bluk insert, bulk/iter erase, copy & rebalance)  /////

BOOST_AUTO_TEST_CASE( test_kdtree_erase_iter )
{
  // check that erase at edge preserve basic iterators
  {
    Twenty_Kdtree_3D_fixture fix;
    typedef Twenty_Kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    int track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
	iter_type iter = fix.kdtree.begin();
	BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(iter));
	BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
	BOOST_CHECK(iter != fix.kdtree.begin());
	int count = 0;
	for(iter_type i = fix.kdtree.begin();
	    i != fix.kdtree.end(); ++i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  {
    Twenty_Kdtree_3D_fixture fix;
    typedef Twenty_Kdtree_3D_fixture::kdtree_type
      ::const_iterator iter_type;
    typedef Twenty_Kdtree_3D_fixture::kdtree_type
      ::const_reverse_iterator riter_type;
    int track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
	iter_type iter = --fix.kdtree.end();
	BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(iter));
	BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
	BOOST_CHECK(iter != (--fix.kdtree.end()));
	int count = 0;
	for(riter_type i = fix.kdtree.rbegin();
	    i != fix.kdtree.rend(); ++i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
      }
  }
  // erase all and check that total ordering is preserved.
  {
    typedef Hundred_Kdtree_2D_fixture::kdtree_type
      ::const_iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d>, false> mapping_iterator;
    Hundred_Kdtree_2D_fixture fix;
    int track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
	iterator eraser = fix.kdtree.begin();
	std::advance(eraser, rand() % fix.kdtree.size());
	mapping_iterator begin_0;
	BOOST_REQUIRE_NO_THROW(begin_0 = mapping_iterator::minimum
			       (fix.kdtree.rank(),
				fix.kdtree.compare(),
				0, 0,
				static_cast<Kdtree_node<point2d>*>
				(fix.kdtree.end().node->parent)));
	mapping_iterator end_0;
	BOOST_REQUIRE_NO_THROW(end_0 = mapping_iterator
			       (fix.kdtree.rank(),
				fix.kdtree.compare(),
				0, details::decr_dim
				(fix.kdtree.rank(), 0),
				static_cast<Kdtree_node<point2d>*>
				(fix.kdtree.end().node)));
	mapping_iterator begin_1;
	BOOST_REQUIRE_NO_THROW(begin_1 = mapping_iterator::minimum
			       (fix.kdtree.rank(),
				fix.kdtree.compare(),
				1, 0,
				static_cast<Kdtree_node<point2d>*>
				(fix.kdtree.end().node->parent)));
	mapping_iterator end_1;
	BOOST_REQUIRE_NO_THROW(end_1 = mapping_iterator
			       (fix.kdtree.rank(),
				fix.kdtree.compare(),
				1, details::decr_dim
				(fix.kdtree.rank(), 0),
				static_cast<Kdtree_node<point2d>*>
				(fix.kdtree.end().node)));
	int count = 0;
	for(mapping_iterator i = begin_0; i != end_0; ++i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
	if (count != track_size && count < 12) abort();
	count = 0;
	for(mapping_iterator i = begin_1; i != end_1; ++i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
	if (count != track_size && count < 12) abort();
	count = 0;
	for(mapping_iterator i = end_0; i != begin_0; --i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
	count = 0;
	for(mapping_iterator i = end_1; i != begin_1; --i, ++count);
	BOOST_CHECK_EQUAL(count, track_size);
	BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
	BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_kdtree_bulk_insert )
{
  // reuse test_kdtree_insert_100_iterate_forward
  Empty_Kdtree_2D_fixture fix;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
	= points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20;
      (*i)[1] = rand() % 20;
    }
  BOOST_REQUIRE_NO_THROW(fix.kdtree.insert(points.begin(), points.end()));
  BOOST_CHECK(fix.kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(fix.kdtree.size(), 100);
  BOOST_CHECK_EQUAL(fix.kdtree.count(), 100);
  int count = 0;
  for(Empty_Kdtree_2D_fixture::kdtree_type::iterator i
	= fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
    {
      std::tr1::array<point2d, 100>::iterator match;
      BOOST_REQUIRE((match = std::find(points.begin(), points.end(), *i))
		  != points.end());
      (*match)[0] = -1; // Prevent the same point from being found twice.
      (*match)[1] = -1;
      BOOST_REQUIRE_LE(++count, 100);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy_empty )
{
  Empty_Kdtree_2D_fixture fix;
  Empty_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  BOOST_CHECK(copy.begin() == copy.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy )
{
  // Simple copy (no rebalancing) should result in identical tree structure, so
  // iterators should output the same sequence.
  Hundred_Kdtree_2D_fixture fix;
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  typedef Hundred_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  iterator i = fix.kdtree.begin(), j = copy.begin();
  for(; i != fix.kdtree.end() && j != copy.end(); ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
    }
  BOOST_CHECK_EQUAL(count, fix.kdtree.size());
  BOOST_CHECK(j == copy.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_assign_empty_trees )
{
  Empty_Kdtree_2D_fixture empty;
  Hundred_Kdtree_2D_fixture full;
  full.kdtree = empty.kdtree;
  BOOST_CHECK_EQUAL(empty.kdtree.size(), full.kdtree.size());
  BOOST_CHECK_EQUAL(empty.kdtree.dimension(), full.kdtree.dimension());
  BOOST_CHECK(full.kdtree.begin() == full.kdtree.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_assignment )
{
  Hundred_Kdtree_2D_fixture fix1;
  Hundred_Kdtree_2D_fixture fix2;
  fix2.kdtree = fix1.kdtree;
  BOOST_CHECK_EQUAL(fix1.kdtree.size(), fix2.kdtree.size());
  BOOST_CHECK_EQUAL(fix1.kdtree.dimension(), fix2.kdtree.dimension());
  typedef Hundred_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  iterator i = fix1.kdtree.begin(), j = fix2.kdtree.begin();
  for(; i != fix1.kdtree.end() && j != fix2.kdtree.end(); ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
      BOOST_CHECK_EQUAL((*i)[1], (*j)[1]);
    }
  BOOST_CHECK_EQUAL(count, fix1.kdtree.size());
  BOOST_CHECK(j == fix2.kdtree.end());
}

BOOST_AUTO_TEST_CASE( test_kdtree_swap )
{
  Hundred_Kdtree_2D_fixture fix1;
  Hundred_Kdtree_2D_fixture fix2;
  std::vector<point2d> point_fix1; point_fix1.reserve(100);
  std::vector<point2d> point_fix2; point_fix2.reserve(100);
  std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
	    std::back_inserter(point_fix1));
  std::copy(fix2.kdtree.cbegin(), fix2.kdtree.cend(),
	    std::back_inserter(point_fix2));
  using std::swap; // it should still resolve into spatial::swap
  // swap once
  swap(fix1.kdtree, fix2.kdtree);
  typedef Hundred_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  // swap twice, back to normal
  swap(fix1.kdtree, fix2.kdtree);
  typedef Hundred_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix2[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix2[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
}

BOOST_AUTO_TEST_CASE( test_kdtree_swap_empty )
{
  Hundred_Kdtree_2D_fixture fix1;
  Empty_Kdtree_2D_fixture fix2;
  std::vector<point2d> point_fix1; point_fix1.reserve(100);
  std::copy(fix1.kdtree.cbegin(), fix1.kdtree.cend(),
	    std::back_inserter(point_fix1));
  using std::swap; // it should still resolve into spatial::swap
  // swap once
  swap(fix1.kdtree, fix2.kdtree);
  typedef Empty_Kdtree_2D_fixture::kdtree_type::iterator iterator;
  int count = 0;
  for(iterator i = fix2.kdtree.begin();
      i != fix2.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  BOOST_CHECK(fix1.kdtree.begin() == fix1.kdtree.end());
  BOOST_CHECK_EQUAL(fix1.kdtree.size(), 0);
  // swap twice, back to normal
  swap(fix1.kdtree, fix2.kdtree);
  count = 0;
  for(iterator i = fix1.kdtree.begin();
      i != fix1.kdtree.end(); ++i, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], point_fix1[count][0]);
      BOOST_CHECK_EQUAL((*i)[1], point_fix1[count][1]);
    }
  BOOST_CHECK_EQUAL(count, 100);
  BOOST_CHECK(fix2.kdtree.begin() == fix2.kdtree.end());
  BOOST_CHECK_EQUAL(fix2.kdtree.size(), 0);
}

BOOST_AUTO_TEST_CASE( test_kdtree_rebalance )
{
  typedef details::Const_Mapping_iterator
    <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d>, false> mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_Kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_REQUIRE_NO_THROW(copy.rebalance());
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  0, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  0, details::decr_dim
			  (fix.kdtree.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  1, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  1, details::decr_dim
			  (fix.kdtree.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
			 (copy.rank(),
			  copy.compare(),
			  0, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
			 (copy.rank(),
			  copy.compare(),
			  0, details::decr_dim
			  (copy.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
			 (copy.rank(),
			  copy.compare(),
			  1, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
			 (copy.rank(),
			  copy.compare(),
			  1, details::decr_dim
			  (copy.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node)));
  int count = 0;
  mapping_iterator i = orig_begin_0, j = copy_begin_0;
  for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_0);
  BOOST_CHECK(j == copy_end_0);
  count = 0;
  i = orig_begin_1; j = copy_begin_1;
  for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_1);
  BOOST_CHECK(j == copy_end_1);
  count = 0;
  i = orig_end_0; j = copy_end_0;
  for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_0);
  BOOST_CHECK(j == copy_begin_0);
  count = 0;
  i = orig_end_1; j = copy_end_1;
  for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_1);
  BOOST_CHECK(j == copy_begin_1);
}

BOOST_AUTO_TEST_CASE( test_kdtree_copy_and_rebalance )
{
  typedef details::Const_Mapping_iterator
    <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d>, false> mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_Kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree, true);
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  0, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  0, details::decr_dim
			  (fix.kdtree.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  1, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
			 (fix.kdtree.rank(),
			  fix.kdtree.compare(),
			  1, details::decr_dim
			  (fix.kdtree.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
			 (copy.rank(),
			  copy.compare(),
			  0, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
			 (copy.rank(),
			  copy.compare(),
			  0, details::decr_dim
			  (copy.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
			 (copy.rank(),
			  copy.compare(),
			  1, 0,
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
			 (copy.rank(),
			  copy.compare(),
			  1, details::decr_dim
			  (copy.rank(), 0),
			  static_cast<Kdtree_node<point2d>*>
			  (copy.end().node)));
  int count = 0;
  mapping_iterator i = orig_begin_0, j = copy_begin_0;
  for(; i != orig_end_0 && j != copy_end_0; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_0);
  BOOST_CHECK(j == copy_end_0);
  count = 0;
  i = orig_begin_1; j = copy_begin_1;
  for(; i != orig_end_1 && j != copy_end_1; ++i, ++j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_end_1);
  BOOST_CHECK(j == copy_end_1);
  count = 0;
  i = orig_end_0; j = copy_end_0;
  for(; i != orig_begin_0 && j != copy_begin_0; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_0);
  BOOST_CHECK(j == copy_begin_0);
  count = 0;
  i = orig_end_1; j = copy_end_1;
  for(; i != orig_begin_1 && j != copy_begin_1; --i, --j, ++count);
  BOOST_CHECK_EQUAL(count, size);
  BOOST_CHECK(i == orig_begin_1);
  BOOST_CHECK(j == copy_begin_1);
}

BOOST_AUTO_TEST_CASE( test_kdtree_rebalance_empty )
{
  Empty_Kdtree_2D_fixture fix;
  BOOST_REQUIRE_NO_THROW(fix.kdtree.rebalance());
  BOOST_CHECK(fix.kdtree.empty());
}

//////////////////////////////  spatial_range.hpp  /////////////////////////////

BOOST_AUTO_TEST_CASE( test_range_iterator_default_ctor )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> range_iterator_false_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> const_range_iterator_true_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> const_range_iterator_false_type;
  range_iterator_true_type i;
  const_range_iterator_true_type ci;
  range_iterator_false_type j;
  const_range_iterator_false_type cj;
}

BOOST_AUTO_TEST_CASE( test_range_iterator_value_ctor_equal )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> range_iterator_false_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> const_range_iterator_true_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type i(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type j(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ci
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cj
    (details::Dynamic_rank(2), bounds, 1, 0);
  BOOST_CHECK_EQUAL(i.rank()(), 2);
  BOOST_CHECK(i.impl.node == 0);
  BOOST_CHECK_EQUAL(i.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(j.rank()(), 2);
  BOOST_CHECK(j.impl.node == 0);
  BOOST_CHECK_EQUAL(j.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(ci.rank()(), 2);
  BOOST_CHECK(ci.impl.node == 0);
  BOOST_CHECK_EQUAL(ci.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(cj.rank()(), 2);
  BOOST_CHECK(cj.impl.node == 0);
  BOOST_CHECK_EQUAL(cj.impl.node_dim(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_copy_ctor )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> range_iterator_false_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> const_range_iterator_true_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_true_type copy_k(k);
  range_iterator_false_type copy_l(l);
  const_range_iterator_true_type copy_ck(ck);
  const_range_iterator_false_type copy_cl(cl);
  const_range_iterator_true_type const_copy_k(k);
  const_range_iterator_false_type const_copy_l(l);
  BOOST_CHECK_EQUAL(copy_k.rank()(), 2);
  BOOST_CHECK(copy_k.impl.node == 0);
  BOOST_CHECK_EQUAL(copy_k.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(copy_l.rank()(), 2);
  BOOST_CHECK(copy_l.impl.node == 0);
  BOOST_CHECK_EQUAL(copy_l.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(copy_ck.rank()(), 2);
  BOOST_CHECK(copy_ck.impl.node == 0);
  BOOST_CHECK_EQUAL(copy_ck.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(copy_cl.rank()(), 2);
  BOOST_CHECK(copy_cl.impl.node == 0);
  BOOST_CHECK_EQUAL(copy_cl.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(const_copy_k.rank()(), 2);
  BOOST_CHECK(const_copy_k.impl.node == 0);
  BOOST_CHECK_EQUAL(const_copy_k.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(const_copy_l.rank()(), 2);
  BOOST_CHECK(const_copy_l.impl.node == 0);
  BOOST_CHECK_EQUAL(const_copy_l.impl.node_dim(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_assignment )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> range_iterator_false_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> const_range_iterator_true_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_true_type assign_k = k;
  range_iterator_false_type assign_l = l;
  const_range_iterator_true_type assign_ck = ck;
  const_range_iterator_false_type assign_cl = cl;
  const_range_iterator_true_type const_assign_k = k;
  const_range_iterator_false_type const_assign_l = l;
  BOOST_CHECK_EQUAL(assign_k.rank()(), 2);
  BOOST_CHECK(assign_k.impl.node == 0);
  BOOST_CHECK_EQUAL(assign_k.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(assign_l.rank()(), 2);
  BOOST_CHECK(assign_l.impl.node == 0);
  BOOST_CHECK_EQUAL(assign_l.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(assign_ck.rank()(), 2);
  BOOST_CHECK(assign_ck.impl.node == 0);
  BOOST_CHECK_EQUAL(assign_ck.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(assign_cl.rank()(), 2);
  BOOST_CHECK(assign_cl.impl.node == 0);
  BOOST_CHECK_EQUAL(assign_cl.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(const_assign_k.rank()(), 2);
  BOOST_CHECK(const_assign_k.impl.node == 0);
  BOOST_CHECK_EQUAL(const_assign_k.impl.node_dim(), 1);
  BOOST_CHECK_EQUAL(const_assign_l.rank()(), 2);
  BOOST_CHECK(const_assign_l.impl.node == 0);
  BOOST_CHECK_EQUAL(const_assign_l.impl.node_dim(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_equal )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> range_iterator_false_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> const_range_iterator_true_type;
  typedef details::Const_Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    false> const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type k(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type l(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ck
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cl
    (details::Dynamic_rank(2), bounds, 1, 0);
  BOOST_CHECK(k == k);
  BOOST_CHECK(k == l);
  BOOST_CHECK(k == ck);
  BOOST_CHECK(k == cl);
  BOOST_CHECK(l == cl);
  BOOST_CHECK(l == ck);
  BOOST_CHECK(cl == ck);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_deference )
{
  typedef details::Range_iterator<details::Dynamic_rank, pair_type,
    Kdtree_node<pair_type>, range_bounds<pair_type, pair_less>,
    true> range_iterator_true_type;
  Kdtree_node<pair_type> node;
  node.parent = 0;
  node.right = 0;
  node.left = 0;
  node.key_field = pair_type(1, 2);
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type i(details::Dynamic_rank(2), bounds, 0, &node);
  BOOST_CHECK_EQUAL((*i).first, 1);
  BOOST_CHECK_EQUAL((*i).second, 2);
  BOOST_CHECK_EQUAL(i->first, 1);
  BOOST_CHECK_EQUAL(i->second, 2);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_minimum )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.begin().node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Kdtree_2D_fixture::kdtree_type::iterator
      max = --fix.kdtree.end(), tmp = max, begin = fix.kdtree.begin();
    point2d value = *max;
    for (; tmp != begin; --tmp)
      { if (*tmp == value) { max = tmp; } }
    bounds_type equal(fix.kdtree.compare(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == max.node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_minimum_empty )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.compare(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 0; shrink != 20; ++shrink)
      {
	point2d low = { { shrink, shrink } };
	point2d high = { { 20, 20} };
	bounds_type shrinking_bounds(fix.kdtree.compare(), low, high);
	// In this case, the minimum of the interval must be equal to min.
	range_iterator it = range_iterator::minimum
	  (fix.kdtree.rank(), shrinking_bounds,
	   0, fix.kdtree.end().node->parent);
	Hundred_Kdtree_2D_fixture::kdtree_type::iterator
	  min = fix.kdtree.begin(), end = fix.kdtree.end();
	for (; min != end && !spatial::details::match_all
	       (fix.kdtree.rank(), *min, shrinking_bounds); ++min);
	BOOST_CHECK(it.impl.node == min.node);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == (--fix.kdtree.end()).node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Kdtree_2D_fixture::kdtree_type::iterator
      min = fix.kdtree.begin(), tmp = min, end = fix.kdtree.end();
    point2d value = *min;
    for (; tmp != end; ++tmp) { if (*tmp == value) { min = tmp; } }
    bounds_type equal(fix.kdtree.compare(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == min.node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 20; shrink != 0; --shrink)
      {
	point2d low = { { 0, 0} };
	point2d high = { { shrink, shrink } };
	bounds_type shrinking_bounds(fix.kdtree.compare(), low, high);
	// In this case, the minimum of the interval must be equal to max.
	range_iterator it = range_iterator::maximum
	  (fix.kdtree.rank(), shrinking_bounds,
	   0, fix.kdtree.end().node->parent);
	Hundred_Kdtree_2D_fixture::kdtree_type::iterator
	  max = fix.kdtree.end(), begin = fix.kdtree.begin();
	for (; max != begin && !spatial::details::match_all
	       (fix.kdtree.rank(), *(--max), shrinking_bounds););
	if (spatial::details::match_all
	    (fix.kdtree.rank(), *max, shrinking_bounds))
	  { BOOST_CHECK(it.impl.node == max.node); }
	else
	  { BOOST_CHECK(it.impl.node == fix.kdtree.end().node); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum_empty )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.compare(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.compare(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_increment )
{
  {
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl.node;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
	if (spatial::details::match_all
	    (fix.kdtree.rank(), *min, whole_tree_box))
	  {
	    BOOST_CHECK(range_min.impl.node == min.node);
	    ++range_min;
	  }
      }
    BOOST_CHECK(range_min.impl.node == min.node); // at end!
  }
  {
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl.node;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
	if (spatial::details::match_all
	    (fix.kdtree.rank(), *min, small_box))
	  {
	    BOOST_CHECK(range_min.impl.node == min.node);
	    ++range_min;
	  }
      }
    BOOST_CHECK(range_min.impl.node == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_decrement )
{
  {
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl.node;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
	if (spatial::details::match_all
	    (fix.kdtree.rank(), *max, whole_tree_box))
	  {
	    --range_max;
	    BOOST_CHECK(range_max.impl.node == max.node);
	  }
      }
  }
  {
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_Kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl.node;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
	if (spatial::details::match_all
	    (fix.kdtree.rank(), *max, small_box))
	  {
	    --range_max;
	    BOOST_CHECK(range_max.impl.node == max.node);
	  }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_post_decrement )
{
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    range_iterator before = test;
    range_iterator after = test--;
    BOOST_CHECK(before == after);
    --after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_post_increment )
{
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::compare_type> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type, false> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.compare(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box,
       0, fix.kdtree.end().node->parent);
    range_iterator before = test;
    range_iterator after = test++;
    BOOST_CHECK(before == after);
    ++after;
    BOOST_CHECK(after == test);
}

//////////////////  spatial_kdtree.hpp (find, erase value) /////////////////////

BOOST_AUTO_TEST_CASE( test_kdtree_find )
{
  {
    // Find on empty tree should return end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find on empty const tree should return const end
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture*>
      (&fix)->kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.cend());
  }
  {
    // Find the one value of a tree should return beginning.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(some_value);
    BOOST_CHECK(it == fix.kdtree.begin());
  }
  {
    // Find on a const tree with one value should return const beginning.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture::kdtree_type*>
      (&fix.kdtree)->find(some_value);
    BOOST_CHECK(it == fix.kdtree.cbegin());
  }
  {
    // Find a value not in the tree should return end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator it
      = fix.kdtree.find(other_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find a value not in the const tree should return const end.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::const_iterator it
      = const_cast<const Empty_Kdtree_2D_fixture::kdtree_type*>
      (&fix.kdtree)->find(other_value);
    BOOST_CHECK(it == fix.kdtree.end());
  }
  {
    // Find should find all points in the tree, even when const
    Hundred_Kdtree_2D_fixture fix;
    {
      Hundred_Kdtree_2D_fixture::kdtree_type::iterator iter
	= fix.kdtree.begin();
      Hundred_Kdtree_2D_fixture::kdtree_type::iterator end
	= fix.kdtree.end();
      for(; iter != end; ++iter)
	{
	  Hundred_Kdtree_2D_fixture::kdtree_type::iterator it
	    = fix.kdtree.find(*iter);
	  BOOST_REQUIRE(it != end);
	  BOOST_CHECK(*it == *iter);
	}
    }
    {
      Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator iter
	= fix.kdtree.cbegin();
      Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator end
	= fix.kdtree.cend();
      for(; iter != end; ++iter)
	{
	  Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator it
	    = const_cast<const Hundred_Kdtree_2D_fixture::kdtree_type*>
	    (&fix.kdtree)->find(*iter);
	  BOOST_REQUIRE(it != end);
	  BOOST_CHECK(*it == *iter);
	}
    }
    // But it should not find a point that is not in the tree!
    {
      point2d some_value = { { 30, 30 } };
      Hundred_Kdtree_2D_fixture::kdtree_type::iterator iter
	= fix.kdtree.find(some_value);
      BOOST_CHECK(iter == fix.kdtree.end());
    }
    {
      point2d some_value = { { 30, 30 } };
      Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator iter
	= const_cast<const Hundred_Kdtree_2D_fixture::kdtree_type*>
	(&fix.kdtree)->find(some_value);
      BOOST_CHECK(iter == fix.kdtree.end());
    }
    // Verify that the equal operator is indeed used in place of the rest
    {
      typedef Kdtree<details::Dynamic_rank, point2d,
		     bracket_less<point2d>, std::allocator<point2d>, false>
	kdtree_type;
      // 1D tree where we store 2d objects...
      kdtree_type tree(details::Dynamic_rank(1));
      point2d one   = { { 0, 1 } };
      point2d two   = { { 0, 2 } };
      point2d three = { { 0, 3 } };
      point2d four  = { { 0, 4 } };
      tree.insert(one);
      tree.insert(two);
      tree.insert(three);
      tree.insert(four);
      // ... And equal should take the second dimension into account.
      kdtree_type::iterator iter;
      kdtree_type::iterator end = tree.end();
      iter = tree.find(one);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(one == *iter);
      iter = tree.find(two);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(two == *iter);
      iter = tree.find(three);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(three == *iter);
      iter = tree.find(four);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(four == *iter);
    }
  }
}

BOOST_AUTO_TEST_CASE ( test_kdtree_erase_key )
{
  {
    // Erase on empty tree should return 0.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase one value of a tree should return 1 and leave an empty tree.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    fix.kdtree.insert(some_value);
    BOOST_CHECK(!fix.kdtree.empty());
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase a value not in the tree should leave the tree untouched.
    Empty_Kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Kdtree_2D_fixture::kdtree_type::iterator
      begin_before = fix.kdtree.begin();
    Empty_Kdtree_2D_fixture::kdtree_type::iterator
      end_before = fix.kdtree.end();
    size_type count  = fix.kdtree.erase(other_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!fix.kdtree.empty());
    BOOST_CHECK(fix.kdtree.end() != fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.end() == end_before);
    BOOST_CHECK(fix.kdtree.begin() == begin_before);
  }
  {
    // Should be able to erase multiple values
    typedef Kdtree<details::Dynamic_rank, point2d,
		   bracket_less<point2d>, std::allocator<point2d>, false>
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    point2d one   = { { 0, 1 } };
    point2d two   = { { 0, 2 } };
    point2d four  = { { 0, 4 } };
    tree.insert(one);
    tree.insert(two);
    tree.insert(two);
    tree.insert(four);
    // ... And equal should take the second dimension into account.
    BOOST_REQUIRE_EQUAL(tree.count(), 4);
    size_type count = tree.erase(two);
    BOOST_CHECK_EQUAL(count, 2);
    BOOST_REQUIRE_EQUAL(tree.count(), 2);
    BOOST_CHECK(tree.find(one) != tree.end());
    BOOST_CHECK(tree.find(four) != tree.end());
    BOOST_CHECK(tree.find(two) == tree.end());
    kdtree_type::iterator iter = tree.begin();
    BOOST_REQUIRE(iter != tree.end());
    BOOST_REQUIRE_NO_THROW(++iter);
    BOOST_REQUIRE(iter != tree.end());
    BOOST_REQUIRE_NO_THROW(++iter);
    BOOST_REQUIRE(iter == tree.end());
  }
}

////////////////////////////  spatial_geometry.hpp  //////////////////////////////


struct triple_access
{
  int&
  operator()(dimension_type dim, triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::logic_error("dim is greater than 2");
      }
  }

  int
  operator()(dimension_type dim, const triple& x) const
  {
    switch(dim)
      {
      case 0: return x.x;
      case 1: return x.y;
      case 2: return x.z;
      default: throw std::logic_error("dim is greater than 2");
      }
  }
};

inline
double
rand_double()
{
  double out = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  out = (static_cast<double>(rand()) + out)/static_cast<double>(RAND_MAX);
  return out;
}

inline
float
rand_float()
{
  float out = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  out = (static_cast<float>(rand()) + out)/static_cast<float>(RAND_MAX);
  out = (static_cast<float>(rand()) + out)/static_cast<float>(RAND_MAX);
  return out;
}

typedef std::tr1::array<double, 5> point5d;
typedef std::tr1::array<float, 5> fpoint5d;

BOOST_AUTO_TEST_CASE( test_cast_accessor )
{
  using namespace spatial::details::geometry;
  cast_accessor<triple, int, triple_access> accessor;
  triple p(0, 1, 2);
  BOOST_CHECK_EQUAL(accessor(0, p), p.x);
  BOOST_CHECK_EQUAL(accessor(1, p), p.y);
  triple q(2, 3, 4);
  accessor(0, p, q);
  accessor(1, p, q);
  accessor(2, p, q);
  BOOST_CHECK_EQUAL(q.x, p.x);
  BOOST_CHECK_EQUAL(q.y, p.y);
  BOOST_CHECK_EQUAL(q.z, p.z);
}

BOOST_AUTO_TEST_CASE( test_bracket_cast_accessor )
{
  using namespace spatial::details::geometry;
  bracket_cast_accessor<point2d, int> accessor;
  point2d p = { { 0, 1 } };
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d q = { { 1, 0 } };
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_paren_cast_accessor )
{
  using namespace spatial::details::geometry;
  paren_cast_accessor<point2d_paren, int> accessor;
  point2d_paren p; p[0] = 0; p[1] = 1;
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d_paren q; q[0] = 1; q[1] = 0;
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_iterator_cast_accessor )
{
  using namespace spatial::details::geometry;
  iterator_cast_accessor<point2d, int> accessor;
  point2d p = { { 0, 1 } };
  BOOST_CHECK_EQUAL(accessor(0, p), p[0]);
  BOOST_CHECK_EQUAL(accessor(1, p), p[1]);
  point2d q = { { 1, 0 } };
  accessor(0, p, q);
  accessor(1, p, q);
  BOOST_CHECK_EQUAL(q[0], p[0]);
  BOOST_CHECK_EQUAL(q[1], p[1]);
}

BOOST_AUTO_TEST_CASE( test_accessor_rebind )
{
  using namespace spatial::details::geometry;
  {
    typename rebind<point2d, double,
		    accessor_less<at_accessor<point2d,
					      int>, point2d> >::type
      must_compile;
  }
  {
    typename rebind<point2d, double,
		    bracket_less<point2d> >::type must_compile;
  }
  {
    typename rebind<point2d, double,
		    paren_less<point2d> >::type must_compile;
  }
  {
    typename rebind<point2d, double,
		    iterator_less<point2d> >::type must_compile;
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    using namespace std;
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	double dist = math::euclidian_distance_to_key
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, q, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	double other_dist = sqrt((p.x-q.x)*(p.x-q.x)
				 +(p.y-q.y)*(p.y-q.y)
				 +(p.z-q.z)*(p.z-q.z));
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a center at the same position should be null
    double r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    using namespace std;
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
    r = math::euclidian_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, sqrt(2.0), .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, l, h;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	l.x = rand() % 80 - 40;
	l.y = rand() % 80 - 40;
	l.z = rand() % 80 - 40;
	h.x = rand() % 80 - 40;
	h.y = rand() % 80 - 40;
	h.z = rand() % 80 - 40;
	if (h.x < l.x) std::swap(h.x, l.x);
	if (h.y < l.y) std::swap(h.y, l.y);
	if (h.z < l.z) std::swap(h.z, l.z);
	double dist = math::euclidian_distance_to_box_edge
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, l, h, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	double dist_x
	  = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
	  + (static_cast<double>(h.x-l.x)/2.0);
	double dist_y
	  = abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
	  + (static_cast<double>(h.y-l.y)/2.0);
	double dist_z
	  = abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
	  + (static_cast<double>(h.z-l.z)/2.0);
	double other_dist
	  = sqrt(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::euclidian_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	dimension_type dim = rand() % 3;
	float dist = math::euclidian_distance_to_plane
	  <triple, cast_accessor<triple, double, triple_access>, float>
	  (dim, p, q, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	triple_access access;
	float other_dist = abs(access(dim, p) - access(dim, q));
	BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::euclidian_square_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	double dist = math::euclidian_square_distance_to_key
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, q, cast_accessor<triple, double, triple_access>());
	double other_dist = (p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y)
	  + (p.z-q.z)*(p.z-q.z);
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a box at the same position should be null
    double r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // point and box separated by 1 on each dim should return the right amount
    double r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
    r = math::euclidian_square_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, l, h;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	l.x = rand() % 80 - 40;
	l.y = rand() % 80 - 40;
	l.z = rand() % 80 - 40;
	h.x = rand() % 80 - 40;
	h.y = rand() % 80 - 40;
	h.z = rand() % 80 - 40;
	if (h.x < l.x) std::swap(h.x, l.x);
	if (h.y < l.y) std::swap(h.y, l.y);
	if (h.z < l.z) std::swap(h.z, l.z);
	double dist = math::euclidian_square_distance_to_box_edge
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, l, h, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	double dist_x
	  = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
	  + (static_cast<double>(h.x-l.x)/2.0);
	double dist_y
	  = abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
	  + (static_cast<double>(h.y-l.y)/2.0);
	double dist_z
	  = abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
	  + (static_cast<double>(h.z-l.z)/2.0);
	double other_dist
	  = dist_x * dist_x + dist_y * dist_y + dist_z * dist_z;
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_euclidian_square_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::euclidian_square_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	dimension_type dim = rand() % 3;
	float dist = math::euclidian_square_distance_to_plane
	  <triple, cast_accessor<triple, double, triple_access>, float>
	  (dim, p, q, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	triple_access access;
	float other_dist = (access(dim, p) - access(dim, q))
	  * (access(dim, p) - access(dim, q));
	BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_key )
{
  using namespace spatial::details::geometry;
  {
    // distance between 2 points at the same position should be null.
    point2d x = zeros;
    double r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = ones;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = twos;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
    x = threes;
    r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, x, x, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // 2 point separated by 1 on each dim should return the right amount
    double r = math::manhattan_distance_to_key
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	double dist = math::manhattan_distance_to_key
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, q, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	double other_dist = abs(p.x-q.x) + abs(p.y-q.y) + abs(p.z-q.z);
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_box_edge )
{
  using namespace spatial::details::geometry;
  {
    // distance between a point and a box at the same position should be null
    double r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, zeros, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, .0, .000000000001);
  }
  {
    // point and box separated by 1 on each dim should return the right amount
    double r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, ones, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
    r = math::manhattan_distance_to_box_edge
      <point2d, bracket_cast_accessor<point2d, double>, double>
      (2, zeros, zeros, ones, bracket_cast_accessor<point2d, double>());
    BOOST_CHECK_CLOSE(r, 2.0, .000000000001);
  }
  {
    // Distance between 2 points at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, l, h;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	l.x = rand() % 80 - 40;
	l.y = rand() % 80 - 40;
	l.z = rand() % 80 - 40;
	h.x = rand() % 80 - 40;
	h.y = rand() % 80 - 40;
	h.z = rand() % 80 - 40;
	if (h.x < l.x) std::swap(h.x, l.x);
	if (h.y < l.y) std::swap(h.y, l.y);
	if (h.z < l.z) std::swap(h.z, l.z);
	double dist = math::manhattan_distance_to_box_edge
	  <triple, cast_accessor<triple, double, triple_access>, double>
	  (3, p, l, h, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	double other_dist
	  = abs(static_cast<double>(p.x)-(static_cast<double>(l.x+h.x)/2.0))
	  + abs(static_cast<double>(h.x-l.x)/2.0)
	  + abs(static_cast<double>(p.y)-(static_cast<double>(l.y+h.y)/2.0))
	  + abs(static_cast<double>(h.y-l.y)/2.0)
	  + abs(static_cast<double>(p.z)-(static_cast<double>(l.z+h.z)/2.0))
	  + abs(static_cast<double>(h.z-l.z)/2.0);
	BOOST_CHECK_CLOSE(dist, other_dist, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_manhattan_distance_to_plane )
{
  using namespace spatial::details::geometry;
  {
    // distance between points and plane at the same position should be null.
    point2d x = zeros;
    float r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0, .0000001);
    x = ones;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0, .0000001);
    x = twos;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0, .0000001);
    x = threes;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0, .0000001);
  }
  {
    // Distance between points and plane at different positions in 3D
    for (int i=0; i<100; ++i)
      {
	triple p, q;
	p.x = rand() % 80 - 40;
	p.y = rand() % 80 - 40;
	p.z = rand() % 80 - 40;
	q.x = rand() % 80 - 40;
	q.y = rand() % 80 - 40;
	q.z = rand() % 80 - 40;
	dimension_type dim = rand() % 3;
	float dist = math::manhattan_distance_to_plane
	  <triple, cast_accessor<triple, double, triple_access>, float>
	  (dim, p, q, cast_accessor<triple, double, triple_access>());
	using namespace ::std;
	triple_access access;
	float other_dist = abs(access(dim, p) - access(dim, q));
	BOOST_CHECK_CLOSE(dist, other_dist, .0000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_double )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_double<triple, triple_access> geometry(access);
  // distance_type should be declared
  typedef euclidian_double<triple, triple_access>
    ::distance_type distance_type;
  triple p, q;
  p.x = rand() % 80 - 40;
  p.y = rand() % 80 - 40;
  p.z = rand() % 80 - 40;
  q.x = rand() % 80 - 40;
  q.y = rand() % 80 - 40;
  q.z = rand() % 80 - 40;
  {
    distance_type r = geometry.distance_to_key(3, p, q);
    distance_type s = math::euclidian_distance_to_key
      <triple, triple_access, distance_type>(3, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .000000000001);
  }
  {
    dimension_type dim = rand() % 3;
    distance_type r = geometry.distance_to_plane(3, dim, p, q);
    distance_type s = math::euclidian_distance_to_plane
      <triple, cast_accessor<triple, distance_type, triple_access>,
       distance_type>(dim, p, q,
		      cast_accessor<triple, distance_type, triple_access>());
    BOOST_CHECK_CLOSE(r, s, .000000000001);
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    euclidian_double<point5d, bracket_cast_accessor<point5d, double> >
      geometry_double(access_double);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
	{
	  point5d p, q;
	  p[0] = rand_double();
	  p[1] = rand_double();
	  p[2] = rand_double();
	  p[3] = rand_double();
	  p[4] = rand_double();
	  q[0] = p[0];
	  q[1] = p[1];
	  q[2] = -rand_double();
	  q[3] = p[3];
	  q[4] = p[4];
	  distance_type r = geometry_double.distance_to_key(5, p, q);
	  distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
	  BOOST_REQUIRE_MESSAGE
	    (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
	     << r << " > " << s);
	}
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_float )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_float<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_float<triple, triple_access>
    ::distance_type distance_type;
  triple p, q;
  p.x = rand() % 80 - 40;
  p.y = rand() % 80 - 40;
  p.z = rand() % 80 - 40;
  q.x = rand() % 80 - 40;
  q.y = rand() % 80 - 40;
  q.z = rand() % 80 - 40;
  {
    distance_type r = geometry.distance_to_key(3, p, q);
    distance_type s = math::euclidian_distance_to_key
      <triple, triple_access, distance_type>(3, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .0000001);
  }
  {
    dimension_type dim = rand() % 3;
    distance_type r = geometry.distance_to_plane(3, dim, p, q);
    distance_type s = math::euclidian_distance_to_plane
      <triple, triple_access, distance_type>(dim, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .0000001);
  }
  {
    bracket_cast_accessor<fpoint5d, float> access_float;
    euclidian_float
      <fpoint5d, bracket_cast_accessor<fpoint5d, float> >
    geometry_float(access_float);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
	{
	  fpoint5d p, q;
	  p[0] = rand_float();
	  p[1] = rand_float();
	  p[2] = rand_float();
	  p[3] = rand_float();
	  p[4] = rand_float();
	  q[0] = p[0];
	  q[1] = p[1];
	  q[2] = -rand_float();
	  q[3] = p[3];
	  q[4] = p[4];
	  distance_type r = geometry_float.distance_to_key(5, p, q);
	  distance_type s = geometry_float.distance_to_plane(5, 2, p, q);
	  BOOST_REQUIRE_MESSAGE
	    (s <= r, std::setprecision(std::numeric_limits<float>::digits10)
	     << r << " > " << s);
	}
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_square_double )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_square_double<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_square_double<triple, triple_access>
    ::distance_type distance_type;
  triple p, q;
  p.x = rand() % 80 - 40;
  p.y = rand() % 80 - 40;
  p.z = rand() % 80 - 40;
  q.x = rand() % 80 - 40;
  q.y = rand() % 80 - 40;
  q.z = rand() % 80 - 40;
  {
    distance_type r = geometry.distance_to_key(3, p, q);
    distance_type s = math::euclidian_square_distance_to_key
      <triple, triple_access, distance_type>(3, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .000000000001);
  }
  {
    dimension_type dim = rand() % 3;
    distance_type r = geometry.distance_to_plane(3, dim, p, q);
    distance_type s = math::euclidian_square_distance_to_plane
      <triple, triple_access, distance_type>(dim, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .000000000001);
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    euclidian_square_double<point5d, bracket_cast_accessor<point5d, double> >
    geometry_double(access_double);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
	{
	  point5d p, q;
	  p[0] = rand_double();
	  p[1] = rand_double();
	  p[2] = rand_double();
	  p[3] = rand_double();
	  p[4] = rand_double();
	  q[0] = p[0];
	  q[1] = p[1];
	  q[2] = -rand_double();
	  q[3] = p[3];
	  q[4] = p[4];
	  distance_type r = geometry_double.distance_to_key(5, p, q);
	  distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
	  BOOST_REQUIRE_MESSAGE
	    (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
	     << r << " > " << s);
	}
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_euclidian_square_float )
{
  using namespace spatial::details::geometry;
  triple_access access;
  euclidian_square_float<triple, triple_access> geometry (access);
  // distance_type should be declared
  typedef euclidian_square_float<triple, triple_access>
    ::distance_type distance_type;
  triple p, q;
  p.x = rand() % 80 - 40;
  p.y = rand() % 80 - 40;
  p.z = rand() % 80 - 40;
  q.x = rand() % 80 - 40;
  q.y = rand() % 80 - 40;
  q.z = rand() % 80 - 40;
  {
    distance_type r = geometry.distance_to_key(3, p, q);
    distance_type s = math::euclidian_square_distance_to_key
      <triple, triple_access, distance_type>(3, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .0000001);
  }
  {
    dimension_type dim = rand() % 3;
    distance_type r = geometry.distance_to_plane(3, dim, p, q);
    distance_type s = math::euclidian_square_distance_to_plane
      <triple, triple_access, distance_type>(dim, p, q, access);
    BOOST_CHECK_CLOSE(r, s, .0000001);
  }
  {
    bracket_cast_accessor<fpoint5d, float> access_float;
    euclidian_square_float<fpoint5d, bracket_cast_accessor<fpoint5d, float> >
      geometry_float(access_float);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
	{
	  fpoint5d p, q;
	  p[0] = rand_float();
	  p[1] = rand_float();
	  p[2] = rand_float();
	  p[3] = rand_float();
	  p[4] = rand_float();
	  q[0] = p[0];
	  q[1] = p[1];
	  q[2] = -rand_float();
	  q[3] = p[3];
	  q[4] = p[4];
	  distance_type r = geometry_float.distance_to_key(5, p, q);
	  distance_type s = geometry_float.distance_to_plane(5, 2, p, q);
	  BOOST_REQUIRE_MESSAGE
	    (s <= r, std::setprecision(std::numeric_limits<float>::digits10)
	     << r << " > " << s);
	}
    }
  }
}

BOOST_AUTO_TEST_CASE( test_geometry_manhattan )
{
  using namespace spatial::details::geometry;
  triple_access access;
  manhattan<triple, triple_access, int> geometry (access);
  // distance_type should be declared
  typedef manhattan<triple, triple_access, int>
    ::distance_type distance_type;
  triple p, q;
  p.x = rand() % 80 - 40;
  p.y = rand() % 80 - 40;
  p.z = rand() % 80 - 40;
  q.x = rand() % 80 - 40;
  q.y = rand() % 80 - 40;
  q.z = rand() % 80 - 40;
  {
    distance_type r = geometry.distance_to_key(3, p, q);
    distance_type s = math::manhattan_distance_to_key
      <triple, triple_access, distance_type>(3, p, q, access);
    BOOST_CHECK_EQUAL(r, s);
  }
  {
    dimension_type dim = rand() % 3;
    distance_type r = geometry.distance_to_plane(3, dim, p, q);
    distance_type s = math::manhattan_distance_to_plane
      <triple, triple_access, distance_type>(dim, p, q, access);
    BOOST_CHECK_EQUAL(r, s);
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    manhattan<point5d, bracket_cast_accessor<point5d, double>, double>
      geometry_double(access_double);
    // Bad attempt to test that given any 2 points, the result of
    // distance_to_plane is always less or equal to distance to key
    {
      for(int i=0; i<10000; ++i)
	{
	  point5d p, q;
	  p[0] = rand_double();
	  p[1] = rand_double();
	  p[2] = rand_double();
	  p[3] = rand_double();
	  p[4] = rand_double();
	  q[0] = p[0];
	  q[1] = p[1];
	  q[2] = -rand_double();
	  q[3] = p[3];
	  q[4] = p[4];
	  distance_type r = geometry_double.distance_to_key(5, p, q);
	  distance_type s = geometry_double.distance_to_plane(5, 2, p, q);
	  BOOST_REQUIRE_MESSAGE
	    (s <= r, std::setprecision(std::numeric_limits<double>::digits10)
	     << r << " > " << s);
	}
    }
  }
}

///////////////////////////  spatial_neighbor.hpp  /////////////////////////////

struct Hundred_Kdtree_5D_fixture
{
  typedef Kdtree<details::Static_rank<5>, point5d,
		 bracket_less<point5d>,
		 std::allocator<point5d>, false> kdtree_type;
  kdtree_type kdtree;
  Hundred_Kdtree_5D_fixture()
  {
    for(int i = 0; i != 100; ++i)
    {
      point5d p;
      p[0] = static_cast<double>(rand() % 20);
      p[1] = static_cast<double>(rand() % 20);
      p[2] = static_cast<double>(rand() % 20);
      p[3] = static_cast<double>(rand() % 20);
      p[4] = static_cast<double>(rand() % 20);
      kdtree.insert(p);
    }
  }
};

BOOST_AUTO_TEST_CASE( test_neighbor_equal )
{
  using namespace spatial::details::geometry;
  details::Neighbor_iterator<details::Static_rank<2>, point2d,
		    Kdtree_node<point2d>, bracket_less<point2d>,
		    euclidian_double<point2d,
				     bracket_cast_accessor<point2d, double> >,
		    false>
    iter;
  details::Const_Neighbor_iterator<details::Static_rank<2>, point2d,
			  Kdtree_node<point2d>, bracket_less<point2d>,
			  euclidian_double<point2d,
					   bracket_cast_accessor<point2d, double> >,
			  true>
    citer;
  iter.impl.node = 0;
  citer.impl.node = 0;
  BOOST_CHECK(iter == citer);
  BOOST_CHECK(!(iter != citer));
}

BOOST_AUTO_TEST_CASE( test_neighbor_distance )
{
  using namespace spatial::details::geometry;
  details::Neighbor_iterator<details::Static_rank<2>, point2d,
		    Kdtree_node<point2d>, bracket_less<point2d>,
		    euclidian_double<point2d,
				     bracket_cast_accessor<point2d, double> >,
		    false>
    iter;
  iter.impl.distance = .1;
  BOOST_CHECK_EQUAL(iter.distance(), .1);
}

BOOST_AUTO_TEST_CASE( test_neighbor_deference )
{
  using namespace spatial::details::geometry;
  details::Neighbor_iterator<details::Static_rank<2>, point2d,
		    Kdtree_node<point2d>, bracket_less<point2d>,
		    euclidian_double<point2d,
				     bracket_cast_accessor<point2d, double> >,
		    false>
    iter;
  details::Const_Neighbor_iterator<details::Static_rank<2>, point2d,
			  Kdtree_node<point2d>, bracket_less<point2d>,
			  euclidian_double<point2d,
					   bracket_cast_accessor<point2d, double> >,
			  true>
    citer;
  Kdtree_node<point2d> node;
  iter.impl.node = &node;
  node.key_field = ones;
  BOOST_CHECK(*iter == ones);
  citer.impl.node = &node;
  node.key_field = twos;
  BOOST_CHECK(*citer == twos);
  *iter = threes;
  BOOST_CHECK(node.key_field == threes);
}

BOOST_AUTO_TEST_CASE( test_neighbor_minimum )
{
  using namespace spatial::details::geometry;
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef euclidian_double
    <kdtree_type::key_type,
     bracket_cast_accessor<point2d, double> > geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // If tree has one node, it should always return that node.
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, ones, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(iter.impl.node == fix.kdtree.begin().node);
    BOOST_CHECK(iter.impl.node_dim == 0);
    BOOST_CHECK_CLOSE(iter.impl.distance(), std::sqrt(2.0), .000000000001);
    BOOST_CHECK(iter.impl.origin() == ones);
  }
  {
    // Find the expected closest at on a left-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(threes);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, ones, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == ones);
    BOOST_CHECK_EQUAL(iter.distance(), .0);
    BOOST_CHECK(iter.impl.node != fix.kdtree.end().node);
  }
  {
    // Find the expected closest at on a right-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(threes);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, fours, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == threes);
    BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(2.0), .000000000001);
    BOOST_CHECK(iter.impl.node != fix.kdtree.end().node);
  }
  {
    // Find the closest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
	point5d target; // A bit out of the tree interval of points
	target[0] = static_cast<double>(rand() % 22 - 1);
	target[1] = static_cast<double>(rand() % 22 - 1);
	target[2] = static_cast<double>(rand() % 22 - 1);
	target[3] = static_cast<double>(rand() % 22 - 1);
	target[4] = static_cast<double>(rand() % 22 - 1);
	iterator_type min =
	  iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
				 geometry, target, 0,
				 fix.kdtree.end().node->parent);
	BOOST_REQUIRE(min.impl.node != fix.kdtree.end().node);
	// Need to collect the results independantly to cross-check
	kdtree_type::const_iterator iter = fix.kdtree.begin();
	kdtree_type::const_iterator end = fix.kdtree.end();
	point5d best = *iter;
	double best_distance = geometry.distance_to_key(5, target, *iter);
	++iter;
	for (; iter != end; ++iter)
	  {
	    double tmp_distance = geometry.distance_to_key(5, target, *iter);
	    if (tmp_distance < best_distance)
	      {
		best = *iter;
		best_distance = tmp_distance;
	      }
	  }
	BOOST_CHECK_CLOSE(min.distance(), best_distance, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_maximum )
{
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef euclidian_double
    <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // If tree has one node, it should always return that node.
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, ones, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(iter.impl.node == fix.kdtree.begin().node);
    BOOST_CHECK(iter.impl.node_dim == 0);
    BOOST_CHECK_CLOSE(iter.impl.distance(), std::sqrt(2.0), .000000000001);
    BOOST_CHECK(iter.impl.origin() == ones);
  }
  {
    // Find the expected furthest on a left-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(threes);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, ones, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == threes);
    BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(8.0), .000000000001);
    BOOST_CHECK(iter.impl.node != fix.kdtree.end().node);
  }
  {
    // Find the expected furthest on a right-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(threes);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, fours, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == zeros);
    BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(32.0), .000000000001);
    BOOST_CHECK(iter.impl.node != fix.kdtree.end().node);
  }
  {
    // Find the furthest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
	point5d target; // A bit out of the tree interval of points
	target[0] = static_cast<double>(rand() % 22 - 1);
	target[1] = static_cast<double>(rand() % 22 - 1);
	target[2] = static_cast<double>(rand() % 22 - 1);
	target[3] = static_cast<double>(rand() % 22 - 1);
	target[4] = static_cast<double>(rand() % 22 - 1);
	iterator_type max =
	  iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
				 geometry, target, 0,
				 fix.kdtree.end().node->parent);
	BOOST_REQUIRE(max.impl.node != fix.kdtree.end().node);
	// Need to collect the results independantly to cross-check
	kdtree_type::const_iterator iter = fix.kdtree.begin();
	kdtree_type::const_iterator end = fix.kdtree.end();
	point5d best = *iter;
	double best_distance = geometry.distance_to_key(5, target, *iter);
	++iter;
	for (; iter != end; ++iter)
	  {
	    double tmp_distance = geometry.distance_to_key(5, target, *iter);
	    if (tmp_distance > best_distance)
	      {
		best = *iter;
		best_distance = tmp_distance;
	      }
	  }
	BOOST_CHECK_CLOSE(max.distance(), best_distance, .000000000001);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_increment )
{
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef euclidian_square_double
    <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // If tree has one node, it should iterate to the end
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type(fix.kdtree.rank(), fix.kdtree.compare(), geometry, zeros,
		    0, static_cast<kdtree_type::node_type*>
		    (fix.kdtree.end().node->parent), 0.0);
    BOOST_CHECK(iter.impl.node == fix.kdtree.begin().node);
    BOOST_CHECK(iter.impl.node_dim == 0);
    BOOST_CHECK(iter.impl.origin() == zeros);
    BOOST_CHECK_NO_THROW(++iter);
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == zeros);
  }
  {
    // Find the expected nodes on a left-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(threes);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, threes, 0,
			     fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == threes);
    ++iter;
    BOOST_CHECK(*iter == twos);
    BOOST_CHECK(iter.distance() == 2);
    ++iter;
    BOOST_CHECK(*iter == ones);
    BOOST_CHECK(iter.distance() == 8);
    ++iter;
    BOOST_CHECK(*iter == zeros);
    BOOST_CHECK(iter.distance() == 18);
    ++iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == threes);
  }
  {
    // Find the expected furthest on a right-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(threes);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, zeros, 0,
			     fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == zeros);
    ++iter;
    BOOST_CHECK(*iter == ones);
    BOOST_CHECK(iter.distance() == 2);
    ++iter;
    BOOST_CHECK(*iter == twos);
    BOOST_CHECK(iter.distance() == 8);
    ++iter;
    BOOST_CHECK(*iter == threes);
    BOOST_CHECK(iter.distance() == 18);
    ++iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == zeros);
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
  	point5d target; // A bit out of the tree interval of points
  	target[0] = static_cast<double>(rand() % 22 - 1);
  	target[1] = static_cast<double>(rand() % 22 - 1);
  	target[2] = static_cast<double>(rand() % 22 - 1);
  	target[3] = static_cast<double>(rand() % 22 - 1);
  	target[4] = static_cast<double>(rand() % 22 - 1);
  	iterator_type iter =
  	  iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
  				 geometry, target, 0,
  				 fix.kdtree.end().node->parent);
  	iterator_type end =
  	  iterator_type(fix.kdtree.rank(), fix.kdtree.compare(),
  			geometry, target, 4,
  			static_cast<kdtree_type::node_type*>
  			(fix.kdtree.end().node));
  	BOOST_REQUIRE(iter.impl.node != fix.kdtree.end().node);
  	double old_distance = geometry.distance_to_key(5, target, *iter);
  	BOOST_CHECK_EQUAL(iter.distance(), old_distance);
  	++iter;
  	int count = 1;
  	for (; iter != end; ++iter, ++count)
  	  {
  	    double distance = geometry.distance_to_key(5, target, *iter);
  	    BOOST_CHECK_EQUAL(iter.distance(), distance);
  	    BOOST_CHECK(distance >= old_distance);
  	    old_distance = distance;
  	  }
  	BOOST_CHECK_EQUAL(count, 100);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_decrement )
{
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef euclidian_square_double
    <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // If tree has one node, it should iterate to the end
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type(fix.kdtree.rank(), fix.kdtree.compare(), geometry, zeros,
		    0, static_cast<kdtree_type::node_type*>
		    (fix.kdtree.end().node->parent), 0.0);
    BOOST_CHECK(iter.impl.node == fix.kdtree.begin().node);
    BOOST_CHECK(iter.impl.node_dim == 0);
    BOOST_CHECK(iter.impl.origin() == zeros);
    BOOST_CHECK_NO_THROW(--iter);
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == zeros);
  }
  {
    // Find the expected nodes on a left-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(threes);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, threes, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == zeros);
    --iter;
    BOOST_CHECK(*iter == ones);
    BOOST_CHECK(iter.distance() == 8);
    --iter;
    BOOST_CHECK(*iter == twos);
    BOOST_CHECK(iter.distance() == 2);
    --iter;
    BOOST_CHECK(*iter == threes);
    BOOST_CHECK(iter.distance() == 0);
    --iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == threes);
    --iter;
    BOOST_CHECK(*iter == zeros);
    BOOST_CHECK(iter.distance() == 18);
  }
  {
    // Find the expected furthest on a right-unblanced tree
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    fix.kdtree.insert(ones);
    fix.kdtree.insert(twos);
    fix.kdtree.insert(threes);
    geometry_type geometry;
    iterator_type iter =
      iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
			     geometry, threes, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(*iter == zeros);
    --iter;
    BOOST_CHECK(*iter == ones);
    BOOST_CHECK(iter.distance() == 8);
    --iter;
    BOOST_CHECK(*iter == twos);
    BOOST_CHECK(iter.distance() == 2);
    --iter;
    BOOST_CHECK(*iter == threes);
    BOOST_CHECK(iter.distance() == 0);
    --iter;
    BOOST_CHECK(iter.impl.node == fix.kdtree.end().node);
    BOOST_CHECK(iter.impl.node_dim == 1);
    BOOST_CHECK(iter.impl.origin() == threes);
    --iter;
    BOOST_CHECK(*iter == zeros);
    BOOST_CHECK(iter.distance() == 18);
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::compare_type> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for(int i=0; i<20; ++i)
      {
  	point5d target; // A bit out of the tree interval of points
  	target[0] = static_cast<double>(rand() % 22 - 1);
  	target[1] = static_cast<double>(rand() % 22 - 1);
  	target[2] = static_cast<double>(rand() % 22 - 1);
  	target[3] = static_cast<double>(rand() % 22 - 1);
  	target[4] = static_cast<double>(rand() % 22 - 1);
  	iterator_type iter =
  	  iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.compare(),
  				 geometry, target, 0,
  				 fix.kdtree.end().node->parent);
  	iterator_type end =
  	  iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.compare(),
  				 geometry, target, 0,
  				 fix.kdtree.end().node->parent);
  	BOOST_REQUIRE(iter.impl.node != fix.kdtree.end().node);
  	double old_distance = geometry.distance_to_key(5, target, *iter);
  	BOOST_CHECK_EQUAL(iter.distance(), old_distance);
  	--iter;
  	int count = 1;
  	for (; iter != end; --iter, ++count)
  	  {
  	    double distance = geometry.distance_to_key(5, target, *iter);
  	    BOOST_CHECK_EQUAL(iter.distance(), distance);
  	    BOOST_CHECK(distance <= old_distance);
  	    old_distance = distance;
  	  }
  	BOOST_CHECK_EQUAL(count, 99);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_lower_bound )
{
  // Return the smallest element in set that is greater or equal to limit.
  // Test with high density and oob values.
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef manhattan
    <kdtree_type::key_type, kdtree_type::compare_type, float> geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // Check that there is no failure out of limits
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type it = iterator_type::lower_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 1.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
    // Check that there is no failure in limits.
    fix.kdtree.insert(ones);
    it = iterator_type::lower_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 1.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node != fix.kdtree.end().node);
    BOOST_CHECK(*it == ones);
  }
  {
    // Check that there is no failure in limits
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type it = iterator_type::lower_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 0.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node != fix.kdtree.end().node);
    BOOST_CHECK(*it == zeros);
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::compare_type, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
	// manathan distance should not exceed 20.f+20.f
	float limit = (float)(rand() % 42 - 1);
	point5d origin;
	origin[0] = rand() % 22 - 1;
	origin[1] = rand() % 22 - 1;
	origin[2] = rand() % 22 - 1;
	origin[3] = rand() % 22 - 1;
	origin[4] = rand() % 22 - 1;
	iterator_type it = iterator_type::lower_bound
	  (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
	   origin, limit, 0, fix.kdtree.end().node->parent);
	// dist to value found should be greater or equal to limit or end
	if (it.impl.node != fix.kdtree.end().node)
	  { BOOST_CHECK_LE(limit, it.distance()); }
	--it;
	// dist to previous value should be stricly less than limit or end
	if (it.impl.node != fix.kdtree.end().node)
	  { BOOST_CHECK_LT(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_upper_bound )
{
  // Return the smallest element in set that is strictly greater than key.
  // Test with high density and oob values.
  typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
  typedef manhattan
    <kdtree_type::key_type, kdtree_type::compare_type, float> geometry_type;
  typedef details::Neighbor_iterator
    <kdtree_type::rank_type, kdtree_type::key_type,
     kdtree_type::node_type, kdtree_type::compare_type,
     geometry_type, true> iterator_type;
  {
    // Check that there is no failure out of limits
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type it = iterator_type::upper_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 1.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
    // Check that there is no failure in limits.
    fix.kdtree.insert(ones);
    it = iterator_type::upper_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 1.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node != fix.kdtree.end().node);
    BOOST_CHECK(*it == ones);
  }
  {
    // Check that there is no failure in limits
    Empty_Kdtree_2D_fixture fix;
    fix.kdtree.insert(zeros);
    geometry_type geometry;
    iterator_type it = iterator_type::upper_bound
      (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
       zeros, 0.f, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl.node == fix.kdtree.end().node);
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::compare_type, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type,
       kdtree_type::node_type, kdtree_type::compare_type,
       geometry_type, true> iterator_type;
    Hundred_Kdtree_5D_fixture fix;
    geometry_type geometry;
    for (int i = 0; i < 100; ++i)
      {
	// manathan distance should not exceed 20.f+20.f
	float limit = (float)(rand() % 42 - 1);
	point5d origin;
	origin[0] = rand() % 22 - 1;
	origin[1] = rand() % 22 - 1;
	origin[2] = rand() % 22 - 1;
	origin[3] = rand() % 22 - 1;
	origin[4] = rand() % 22 - 1;
	iterator_type it = iterator_type::upper_bound
	  (fix.kdtree.rank(), fix.kdtree.compare(), geometry,
	   origin, limit, 0, fix.kdtree.end().node->parent);
	// dist to value found should be strictly greater than limit or end
	if (it.impl.node != fix.kdtree.end().node)
	  { BOOST_CHECK_LT(limit, it.distance()); }
	--it;
	// dist to previous value should be lower or equal limit or end
	if (it.impl.node != fix.kdtree.end().node)
	  { BOOST_CHECK_LE(it.distance(), limit); }
      }
  }
}

///////////////////////////////  pointset.hpp  /////////////////////////////////

BOOST_AUTO_TEST_CASE( test_pointset_constructors )
{
  pointset<2, point2d> points;
  pointset<0, point2d> dynamic_points;
  dynamic_pointset<point2d> more_dynamic_points;
}

BOOST_AUTO_TEST_CASE( test_pointset_insert_iter_erase )
{
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_equality )
{
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_swap )
{
  BOOST_CHECK(false);
}

///////////////////////////  pointset.hpp views ////////////////////////////////

BOOST_AUTO_TEST_CASE( test_pointset_mapping )
{
  pointset<2, point2d> points;
  view::mapping<pointset<2, point2d> > flat_view(points, 0);
  BOOST_CHECK(flat_view.begin() == flat_view.end());
  // Now add some points to pointset and iterate throught these points.
}

BOOST_AUTO_TEST_CASE( test_pointset_range )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_equal_range )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_close_range )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_open_range )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_range_predicate )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_upper_partition )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_lower_partition )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE( test_pointset_neighborhood )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK(false);
}

/////////////////////////  spatial_relaxed_kdtree.hpp  /////////////////////////


///////////////////////////  spatial_intersect.hpp  ////////////////////////////


////////////////////////////  spatial_include.hpp  /////////////////////////////


////////////////////////////////  boxset.hpp  //////////////////////////////////


/////////////////  spatial_mapping.hpp (with relaxed kd-tree)  /////////////////


//////////////  spatial_proximity.hpp (with relaxed kd-tree)  //////////////////
