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
#include <limits>
#include <iomanip>

#ifdef __GLIBCXX__
#  include <tr1/array>
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <array>
#endif

#define BOOST_TEST_MAIN // single (a bit) long test file
#include <boost/test/included/unit_test.hpp>

#define SPATIAL_ENABLE_ASSERT // detect interal issues that should not occur
#include "../src/pointset.hpp"
#include "../src/frozen_pointset.hpp"
#include "../src/boxset.hpp"
#include "../src/frozen_boxset.hpp"
#include "../src/pointmap.hpp"
#include "../src/frozen_pointmap.hpp"
#include "../src/boxmap.hpp"
#include "../src/frozen_boxmap.hpp"

using namespace spatial;

BOOST_AUTO_TEST_CASE ( install_srand )
{
  // not sure where else to put this...
  srand((unsigned int)time(NULL));
}

///////////////////////////  spatial_traits.hpp  ///////////////////////////////

template <typename T> void silence_unused(const T&) { }

BOOST_AUTO_TEST_CASE( test_traits_condition )
{
  typedef details::condition<true, std::pair<int, int>,
    std::allocator<int> >::type type1;
  type1 must_compile_1 = std::pair<int, int>();
  silence_unused(must_compile_1);
  typedef details::condition<false, std::pair<int, int>,
    std::allocator<int> >::type type2;
  type2 must_compile_2 = std::allocator<int>();
  silence_unused(must_compile_2);
}

BOOST_AUTO_TEST_CASE( test_node_traits )
{
  typedef details::node_traits<details::Kdtree_node<int> >
    ::invariant_category type1;
  type1 must_compile_1 = details::strict_invariant_tag();
  silence_unused(must_compile_1);
  typedef details::node_traits<details::Relaxed_kdtree_node<int> >
    ::invariant_category type2;
  type2 must_compile_2 = details::relaxed_invariant_tag();
  silence_unused(must_compile_2);
}

BOOST_AUTO_TEST_CASE( test_true_or_false_type )
{
  // Just have to compile it to verify it :)
  std::tr1::false_type one = details::true_or_false_type<false>::type();
  std::tr1::true_type two = details::true_or_false_type<true>::type();
  silence_unused(one);
  silence_unused(two);
}

///////////////////////////  spatial_details.hpp  //////////////////////////////

typedef std::tr1::array<int, 2> point2d;

point2d zeros = { { 0, 0 } };
point2d ones = { { 1, 1 } };
point2d twos = { { 2, 2 } };
point2d threes = { { 3, 3 } };
point2d fours = { { 4, 4 } };
point2d fives = { { 5, 5 } };

void swap(point2d& left, point2d& right)
{
  point2d tmp = right;
  right = left;
  left = tmp;
}

using details::Kdtree_node;

struct Empty_basic_2D_fixture
{
  typedef details::Kdtree<details::Static_rank<2>, point2d, point2d,
                          bracket_less<point2d>,
                          std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  Empty_basic_2D_fixture() : kdtree() { }
};

struct closed_test_range
{
  relative_order
  operator()(dimension_type dim, const point2d& point, dimension_type rank)
    const
  {
    if (dim >= rank) throw std::out_of_range("'dim' must be lower than 'rank'");
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
  // written, which is important for subsequent tests.
  point2d x = { { 0, 0 } };
  point2d y = { { 1, 1 } };
  point2d _x = { { -1, -1 } };
  point2d x_ = { { 2, 2 } };
  BOOST_CHECK(closed_test_range()(0, x, 2) == matching);
  BOOST_CHECK(closed_test_range()(1, y, 2) == matching);
  BOOST_CHECK(closed_test_range()(0, _x, 2) == below);
  BOOST_CHECK(closed_test_range()(1, x_, 2) == above);
}

BOOST_AUTO_TEST_CASE( test_details_match_all )
{
  Empty_basic_2D_fixture fix;
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
  Empty_basic_2D_fixture fix;
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
  Empty_basic_2D_fixture fix;
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

struct Node_basic_empty_fixture
{
  Node_base header;
  Node_basic_empty_fixture()
  {
    header.parent = &header;
    header.left = &header;
    header.right = &header;
  }
};

struct Five_Node_basic_fixture
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
  Five_Node_basic_fixture()
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
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.value = zeros;
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
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.weight = 3;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_right.weight = 1;
    node_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.weight = 1;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.weight = 1;
    node_left_left.value = zeros;
  }
};

BOOST_AUTO_TEST_CASE( test_empty_header )
{
  Node_basic_empty_fixture fix;
  BOOST_CHECK(Node_base::header(&fix.header));
}

BOOST_AUTO_TEST_CASE( test_5_node )
{
  Five_Node_basic_fixture fix;
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
    Five_Node_basic_fixture fix; // swap with non-root
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
    Five_Node_basic_fixture fix; // swap with non-root, invert args
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
    Five_Node_basic_fixture fix; // swap with root
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
    Five_Node_basic_fixture fix; // swap with root, invert args
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
    Five_Node_basic_fixture fix; // swap with left child
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
    Five_Node_basic_fixture fix; // swap with left child, invert args
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
    Five_Node_basic_fixture fix; // swap with right child
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
    Five_Node_basic_fixture fix; // swap with right child, invert args
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
    Five_Node_basic_fixture fix; // swap root with left child
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
    Five_Node_basic_fixture fix; // swap root with left child, invert args
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
    Five_Node_basic_fixture fix; // swap root with right child
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
    Five_Node_basic_fixture fix; // swap root with right child, invert args
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
  BOOST_CHECK(fix.node_left_left.value == zeros);
  BOOST_CHECK(fix.node_right.value == threes);
}

BOOST_AUTO_TEST_CASE( test_swap_relaxed_kdtree_node )
{ // swap position and weight, but not values!
  Five_Relaxed_kdtree_node_fixture fix;
  Relaxed_kdtree_node<point2d>* left = &fix.node_left;
  Relaxed_kdtree_node<point2d>* right = &fix.node_right;
  swap(*left, *right);
  BOOST_CHECK_EQUAL(fix.node_left.weight, 1);
  BOOST_CHECK_EQUAL(fix.node_right.weight, 3);
  BOOST_CHECK(fix.node_left.value == ones);
  BOOST_CHECK(fix.node_right.value == threes);
}

BOOST_AUTO_TEST_CASE( test_Node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Node_iterator<pair_type,
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
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
      Relaxed_kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
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
    typedef details::Node_iterator<point2d, Kdtree_node<point2d> > iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Node_iterator<point2d, Kdtree_node<point2d> > iterator;
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
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    typedef details::Const_Node_iterator<pair_type,
      Relaxed_kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    // Make sure that constant iterator does not invalidate anything
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<point2d, Kdtree_node<point2d> > iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<point2d, Kdtree_node<point2d> > iterator;
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
    test_node.value = test_object;
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
  Empty_basic_2D_fixture fix;
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
    Five_Node_basic_fixture fix;
    BOOST_CHECK_NO_THROW(except::check_node_argument(&fix.node_root));
    BOOST_CHECK_THROW(except::check_node_argument(&fix.header),
                      invalid_node_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_iterator )
{
  {
    details::Node_iterator<point2d, Kdtree_node<point2d> > i(0);
    BOOST_CHECK_THROW(except::check_node_iterator_argument(i.node),
                      invalid_iterator_argument);
  }
  {
    Five_Kdtree_node_fixture fix;
    details::Node_iterator<point2d, Kdtree_node<point2d> >
      i(&fix.node_root);
    details::Node_iterator<point2d, Kdtree_node<point2d> >
      j(static_cast<Kdtree_node<point2d>*>
        (static_cast<Node_base*>(&fix.header)));
    BOOST_CHECK_NO_THROW(except::check_node_iterator_argument(i.node));
    BOOST_CHECK_THROW(except::check_node_iterator_argument(j.node),
                      invalid_iterator_argument);
    BOOST_CHECK_NO_THROW(except::check_iterator_argument(i, i));
    BOOST_CHECK_THROW(except::check_iterator_argument(i, j),
                      invalid_iterator_argument);
  }
}

BOOST_AUTO_TEST_CASE( text_except_check_empty_tree )
{
  Empty_basic_2D_fixture fix;
  BOOST_CHECK_THROW(except::check_empty_container_argument(fix.kdtree),
                    invalid_empty_container_argument);
}

BOOST_AUTO_TEST_CASE( text_except_check_range )
{
  Empty_basic_2D_fixture fix;
  BOOST_CHECK_NO_THROW(except::check_open_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_range_bounds(fix.kdtree, zeros, ones));
  BOOST_CHECK_NO_THROW(except::check_closed_range_bounds(fix.kdtree, zeros, zeros));
  BOOST_CHECK_THROW(except::check_open_range_bounds(fix.kdtree, zeros, zeros),
                    invalid_range_bounds);
  BOOST_CHECK_THROW(except::check_range_bounds(fix.kdtree, zeros, zeros),
                    invalid_range_bounds);
  BOOST_CHECK_THROW(except::check_closed_range_bounds(fix.kdtree, ones, zeros),
                    invalid_range_bounds);
  // For boxes, point2d are used as 1D boxes, or intervals.
  point2d lh_box = { { 0, 1 } };
  point2d hl_box = { { 1, 0 } };
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, lh_box, llhh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, hl_box, llhh_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, lh_box, lhlh_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, hl_box, lhlh_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, hl_box, hhll_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, lh_box, hhll_layout_tag()),
     invalid_box_argument);
  BOOST_CHECK_NO_THROW
    (except::check_box_argument(fix.kdtree, hl_box, hlhl_layout_tag()));
  BOOST_CHECK_THROW
    (except::check_box_argument(fix.kdtree, lh_box, hlhl_layout_tag()),
     invalid_box_argument);
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
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_bracket_less )
{
  // This has been used before this test was carried on, but at least we will
  // know here that something could fail.
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  bracket_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_iterator_less )
{
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  iterator_less<point2d> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_paren_less )
{
  point2d_paren x; x[0] = 0; x[1] = 1;
  point2d_paren y; y[0] = 1; y[1] = 0;
  paren_less<point2d_paren> compare;
  BOOST_CHECK(compare(0, x, y));
  BOOST_CHECK(compare(0, x, 0, y));
  BOOST_CHECK(!compare(0, y, x));
  BOOST_CHECK(!compare(0, y, 0, x));
  BOOST_CHECK(!compare(1, x, y));
  BOOST_CHECK(!compare(1, x, 1, y));
  BOOST_CHECK(compare(1, y, x));
  BOOST_CHECK(compare(1, y, 1, x));
  BOOST_CHECK(!compare(0, x, x));
  BOOST_CHECK(!compare(0, x, 0, x));
  BOOST_CHECK(compare(0, x, 1, x));
  BOOST_CHECK(!compare(0, x, 1, y));
}

BOOST_AUTO_TEST_CASE( test_equal_bounds )
{
  point2d t = { { 1, 1 } };
  point2d x = { { 0, 1 } };
  point2d y = { { 1, 0 } };
  point2d z = { { 0, 2 } };
  point2d w = { { 2, 0 } };
  Empty_basic_2D_fixture fix;
  // Checking this compiles
  equal_bounds<point2d, bracket_less<point2d> > bounds
    = make_equal_bounds(fix.kdtree, t);
  BOOST_CHECK(bounds(0, t, 2) == matching);
  BOOST_CHECK(bounds(1, t, 2) == matching);
  BOOST_CHECK(bounds(0, x, 2) == below);
  BOOST_CHECK(bounds(1, x, 2) == matching);
  BOOST_CHECK(bounds(0, y, 2) == matching);
  BOOST_CHECK(bounds(1, y, 2) == below);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == above);
  BOOST_CHECK(bounds(0, w, 2) == above);
  BOOST_CHECK(bounds(1, w, 2) == below);
}

BOOST_AUTO_TEST_CASE( test_open_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 1 } };
  point2d y = { { 3, 2 } };
  Empty_basic_2D_fixture fix;
  // Checking this compiles
  open_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_open_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == below);
  BOOST_CHECK(bounds(1, l, 2) == below);
  BOOST_CHECK(bounds(0, h, 2) == above);
  BOOST_CHECK(bounds(1, h, 2) == above);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
}

BOOST_AUTO_TEST_CASE( test_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 3, 2 } };
  point2d z = { { 0, 0 } };
  Empty_basic_2D_fixture fix;
  // Checking this compiles
  range_bounds<point2d, bracket_less<point2d> > bounds
    = make_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == matching);
  BOOST_CHECK(bounds(1, l, 2) == matching);
  BOOST_CHECK(bounds(0, h, 2) == above);
  BOOST_CHECK(bounds(1, h, 2) == above);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == below);
}

BOOST_AUTO_TEST_CASE( test_closed_range_bounds )
{
  point2d l = { { 1, 1 } };
  point2d h = { { 3, 3 } };
  point2d x = { { 2, 0 } };
  point2d y = { { 4, 2 } };
  point2d z = { { 0, 0 } };
  point2d w = { { 4, 4 } };
  Empty_basic_2D_fixture fix;
  // Checking this compiles
  closed_range_bounds<point2d, bracket_less<point2d> > bounds
    = make_closed_range_bounds(fix.kdtree, l, h);
  BOOST_CHECK(bounds(0, l, 2) == matching);
  BOOST_CHECK(bounds(1, l, 2) == matching);
  BOOST_CHECK(bounds(0, h, 2) == matching);
  BOOST_CHECK(bounds(1, h, 2) == matching);
  BOOST_CHECK(bounds(0, x, 2) == matching);
  BOOST_CHECK(bounds(1, x, 2) == below);
  BOOST_CHECK(bounds(0, y, 2) == above);
  BOOST_CHECK(bounds(1, y, 2) == matching);
  BOOST_CHECK(bounds(0, z, 2) == below);
  BOOST_CHECK(bounds(1, z, 2) == below);
  BOOST_CHECK(bounds(0, w, 2) == above);
  BOOST_CHECK(bounds(1, w, 2) == above);
}

BOOST_AUTO_TEST_CASE( test_overlap_range_bounds )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

BOOST_AUTO_TEST_CASE( test_enclosed_range_bounds )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

///////////////////////////  spatial_kdtree_basic.hpp  //////////////////////////

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic )
{
  // Up to now, we only knew it compiled
  Empty_basic_2D_fixture fix;
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

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_clear )
{
  Empty_basic_2D_fixture fix;
  BOOST_REQUIRE_NO_THROW(fix.kdtree.clear());
  BOOST_CHECK(fix.kdtree.cend().node->left == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->right == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.end().node->parent == fix.kdtree.end().node);
  BOOST_CHECK(fix.kdtree.begin().node->right == fix.kdtree.end().node); // leftmost
  BOOST_CHECK(fix.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_swap )
{
  Empty_basic_2D_fixture fix1;
  Empty_basic_2D_fixture fix2;
  Empty_basic_2D_fixture::kdtree_type::iterator iter1 = fix1.kdtree.end();
  Empty_basic_2D_fixture::kdtree_type::iterator iter2 = fix2.kdtree.end();
  swap(fix1.kdtree, fix2.kdtree);
  BOOST_CHECK(fix1.kdtree.end() == iter1);
  BOOST_CHECK(fix2.kdtree.end() == iter2);
  BOOST_CHECK(fix1.kdtree.empty());
  BOOST_CHECK(fix2.kdtree.empty());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_max_size )
{
  Empty_basic_2D_fixture fix;
  std::allocator<Kdtree_node<point2d> > node_alloc;
  BOOST_CHECK(fix.kdtree.max_size() == node_alloc.max_size());
}

BOOST_AUTO_TEST_CASE( test_empty_Kdtree_basic_accessor )
{
  Empty_basic_2D_fixture fix;
  std::allocator<point2d> alloc;
  Empty_basic_2D_fixture::kdtree_type::rank_type rank;
  Empty_basic_2D_fixture::kdtree_type::key_compare compare;
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), 2);
  BOOST_CHECK_NO_THROW(rank = fix.kdtree.rank());
  BOOST_CHECK_NO_THROW(compare = fix.kdtree.key_comp());
  BOOST_CHECK_NO_THROW(alloc = fix.kdtree.get_allocator());
}

using details::Kdtree;
struct Empty_Kdtree_2D_fixture
{
  typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                 bracket_less<point2d>,
                 std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  Empty_Kdtree_2D_fixture() : kdtree(details::Dynamic_rank(2)) { }
};

BOOST_AUTO_TEST_CASE( test_Kdtree_basic_equal_range )
{
  Empty_Kdtree_2D_fixture fix;
  fix.kdtree.insert(zeros);
  fix.kdtree.insert(ones);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(threes);
  fix.kdtree.insert(fours);
  // and more twos
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  // There are 4 twos.
  typedef Empty_Kdtree_2D_fixture::kdtree_type::equal_iterator
    equal_iterator;
  std::pair<equal_iterator, equal_iterator>
    eq_pair = fix.kdtree.equal_range(twos);
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_CHECK(eq_pair.first == eq_pair.second);
  // There are no fives...
  eq_pair = fix.kdtree.equal_range(fives);
  BOOST_REQUIRE(eq_pair.first == eq_pair.second);
}

BOOST_AUTO_TEST_CASE( test_Kdtree_basic_const_equal_range )
{
  Empty_Kdtree_2D_fixture fix;
  fix.kdtree.insert(zeros);
  fix.kdtree.insert(ones);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(threes);
  fix.kdtree.insert(fours);
  // and more twos
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  fix.kdtree.insert(twos);
  // There are 4 twos.
  typedef Empty_Kdtree_2D_fixture::kdtree_type::const_equal_iterator
    equal_iterator;
  std::pair<equal_iterator, equal_iterator>
    eq_pair = fix.kdtree.equal_range(twos);
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_REQUIRE(eq_pair.first != eq_pair.second);
  BOOST_CHECK(*eq_pair.first == twos);
  ++eq_pair.first;
  BOOST_CHECK(eq_pair.first == eq_pair.second);
  // There are no fives...
  eq_pair = fix.kdtree.equal_range(fives);
  BOOST_REQUIRE(eq_pair.first == eq_pair.second);
}

//////////////////////////////  spatial_kdtree.hpp  ////////////////////////////

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
  typedef Kdtree<details::Dynamic_rank, pair_type, pair_type,
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
  typedef Kdtree<details::Dynamic_rank, pair_type, pair_type,
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
  typedef Kdtree<details::Dynamic_rank, pair_type, pair_type,
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
  typedef Kdtree<details::Dynamic_rank, point2d, point2d,
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
  triple(int a, int b, int c) : x(a), y(b), z(c) { }
};

inline bool operator== (const triple& a, const triple& b)
{
  return ( a.x == b.x && a.y == b.y && a.z == b.z );
}

inline bool operator!= (const triple& a, const triple& b)
{
  return !(a == b);
}

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
  typedef Kdtree<details::Dynamic_rank, triple, triple,
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
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
  {
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it1(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
       pair_less> it2(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                      static_cast<Kdtree_node<pair_type>*>
                      (fix.kdtree.begin().node));
    BOOST_CHECK(it1 == it2);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_deference )
{
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                              Kdtree_node<pair_type>, pair_less> itr
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
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
    details::Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                              Kdtree_node<pair_type>, pair_less> itr
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
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
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                                    Kdtree_node<pair_type>, pair_less>
      itr(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
          static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    BOOST_CHECK_EQUAL((*itr).first, 1);
    BOOST_CHECK_EQUAL((*itr).second, 2);
    BOOST_CHECK_EQUAL(itr->first, 1);
    BOOST_CHECK_EQUAL(itr->second, 2);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator<details::Dynamic_rank, pair_type, pair_type,
                                    Kdtree_node<pair_type>, pair_less>
      itr(fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
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
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
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
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], min_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 1); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 2); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
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
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  {
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
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
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_REQUIRE_EQUAL((*iter)[mapping_dim], max_value);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<pair_type>*>
                            (fix.kdtree.end().node->parent)));
    BOOST_CHECK_EQUAL(iter->first, 7); // should be (1, 2);
    BOOST_CHECK_EQUAL(iter->second, 8); // should be (1, 2);
  }
  { // test at the limit: an unbalanced tree!
    dimension_type mapping_dim = 0;
    Unbalanced_left_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_increment )
{
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(++it == end);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
  { // test at the limit: an unbalanced tree
    dimension_type mapping_dim = 1;
    Unbalanced_right_Kdtree_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> iterator;
    iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = iterator::maximum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_Mapping_iterator_pre_post_decrement )
{
  {
    Pair_Kdtree_fixture fix;
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
       details::decr_dim(fix.kdtree.rank(), 0),
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.end().node));
    BOOST_CHECK(--end == it);
  }
  {
    Pair_Kdtree_fixture fix;
    details::Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> it
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
       static_cast<Kdtree_node<pair_type>*>(fix.kdtree.begin().node));
    details::Const_Mapping_iterator
      <details::Dynamic_rank, pair_type, pair_type,
       Kdtree_node<pair_type>, pair_less> end
      (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, point2d, point2d,
       Kdtree_node<point2d>, bracket_less<point2d> > iter_type;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    typedef details::Mapping_iterator
      <details::Dynamic_rank, triple, triple,
       Kdtree_node<triple>, triple_less> iter_type;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_Kdtree_2D_fixture fix;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
      Twenty_Kdtree_3D_fixture::kdtree_type::value_type,
      Twenty_Kdtree_3D_fixture::kdtree_type::node_type,
      triple_less> mapping_iterator;
    dimension_type mapping_dim = 2;  // triple::z
    Twenty_Kdtree_3D_fixture fix;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
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
    std::size_t track_size = fix.kdtree.size();
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
      ::iterator iter_type;
    typedef Twenty_Kdtree_3D_fixture::kdtree_type
      ::reverse_iterator riter_type;
    std::size_t track_size = fix.kdtree.size();
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
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    Hundred_Kdtree_2D_fixture fix;
    std::size_t track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser, static_cast<std::size_t>(rand())
                     % fix.kdtree.size());
        mapping_iterator begin_0;
        BOOST_REQUIRE_NO_THROW(begin_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                0, 0,
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_0;
        BOOST_REQUIRE_NO_THROW(end_0 = mapping_iterator
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                0, details::decr_dim
                                (fix.kdtree.rank(), 0),
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node)));
        mapping_iterator begin_1;
        BOOST_REQUIRE_NO_THROW(begin_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                1, 0,
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_1;
        BOOST_REQUIRE_NO_THROW(end_1 = mapping_iterator
                               (fix.kdtree.rank(),
                                fix.kdtree.key_comp(),
                                1, details::decr_dim
                                (fix.kdtree.rank(), 0),
                                static_cast<Kdtree_node<point2d>*>
                                (fix.kdtree.end().node)));
        std::size_t count = 0;
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

BOOST_AUTO_TEST_CASE( test_kdtree_bulk_erase )
{
  Hundred_Kdtree_2D_fixture fix;
  std::vector<point2d> store;
  store.reserve(100);
  store.insert(store.end(), fix.kdtree.begin(), fix.kdtree.end());
  BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(store.begin(), store.end()));
  BOOST_CHECK(fix.kdtree.empty() == true);
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

BOOST_AUTO_TEST_CASE( test_kdtree_copy_rebalance )
{
  // Simple copy (rebalancing) should result in a tree that has the same nodes,
  // and mapping iteration in one dimension can prove it.
  Hundred_Kdtree_2D_fixture fix;
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree, true);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  BOOST_CHECK_EQUAL(fix.kdtree.dimension(), copy.dimension());
  int count = 0;
  typedef details::mapping::iterator
    <Hundred_Kdtree_2D_fixture::kdtree_type>::type iterator;
  iterator i = details::mapping::begin(fix.kdtree, 0),
    j = details::mapping::begin(copy, 0),
    i_end = details::mapping::end(fix.kdtree, 0),
    j_end = details::mapping::end(copy, 0);
  for(; i != i_end && j != j_end; ++i, ++j, ++count)
    {
      BOOST_CHECK_EQUAL((*i)[0], (*j)[0]);
    }
  BOOST_CHECK_EQUAL(count, fix.kdtree.size());
  BOOST_CHECK(i == i_end);
  BOOST_CHECK(j == j_end);
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
  std::size_t count = 0;
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
  std::size_t count = 0;
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
    Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d> > mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_Kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_REQUIRE_NO_THROW(copy.rebalance());
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(),
                          0, 0, static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 0,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 1,
                          details::decr_dim(copy.rank(), 0),
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
    Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
    Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
    bracket_less<point2d> > mapping_iterator;
  // The performance gain of rebalance cannot be mesured now (see performance
  // executable) but we can make sure that total ordering as remained.
  Hundred_Kdtree_2D_fixture fix;
  size_type size = fix.kdtree.size();
  Hundred_Kdtree_2D_fixture::kdtree_type copy(fix.kdtree, true);
  mapping_iterator orig_begin_0;
  BOOST_REQUIRE_NO_THROW(orig_begin_0 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_0;
  BOOST_REQUIRE_NO_THROW(orig_end_0 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator orig_begin_1;
  BOOST_REQUIRE_NO_THROW(orig_begin_1 = mapping_iterator::minimum
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node->parent)));
  mapping_iterator orig_end_1;
  BOOST_REQUIRE_NO_THROW(orig_end_1 = mapping_iterator
                         (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                          details::decr_dim(fix.kdtree.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (fix.kdtree.end().node)));
  mapping_iterator copy_begin_0;
  BOOST_REQUIRE_NO_THROW(copy_begin_0 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 0, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_0;
  BOOST_REQUIRE_NO_THROW(copy_end_0 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 0,
                          details::decr_dim(copy.rank(), 0),
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node)));
  mapping_iterator copy_begin_1;
  BOOST_REQUIRE_NO_THROW(copy_begin_1 = mapping_iterator::minimum
                         (copy.rank(), copy.key_comp(), 1, 0,
                          static_cast<Kdtree_node<point2d>*>
                          (copy.end().node->parent)));
  mapping_iterator copy_end_1;
  BOOST_REQUIRE_NO_THROW(copy_end_1 = mapping_iterator
                         (copy.rank(), copy.key_comp(), 1,
                          details::decr_dim(copy.rank(), 0),
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
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_iterator_true_type i;
  const_range_iterator_true_type ci;
  range_iterator_false_type j;
  const_range_iterator_false_type cj;
}

BOOST_AUTO_TEST_CASE( test_range_iterator_value_ctor_equal )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
  range_bounds<pair_type, pair_less> bounds;
  range_iterator_true_type i(details::Dynamic_rank(2), bounds, 1, 0);
  range_iterator_false_type j(details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_true_type ci
    (details::Dynamic_rank(2), bounds, 1, 0);
  const_range_iterator_false_type cj
    (details::Dynamic_rank(2), bounds, 1, 0);
  BOOST_CHECK_EQUAL(i.rank()(), 2);
  BOOST_CHECK(i.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(i.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(j.rank()(), 2);
  BOOST_CHECK(j.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(j.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(ci.rank()(), 2);
  BOOST_CHECK(ci.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(ci.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(cj.rank()(), 2);
  BOOST_CHECK(cj.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(cj.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_copy_ctor )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
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
  BOOST_CHECK(copy_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_l.rank()(), 2);
  BOOST_CHECK(copy_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_l.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_ck.rank()(), 2);
  BOOST_CHECK(copy_ck.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_ck.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(copy_cl.rank()(), 2);
  BOOST_CHECK(copy_cl.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(copy_cl.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_copy_k.rank()(), 2);
  BOOST_CHECK(const_copy_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_copy_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_copy_l.rank()(), 2);
  BOOST_CHECK(const_copy_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_copy_l.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_assignment )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
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
  BOOST_CHECK(assign_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_l.rank()(), 2);
  BOOST_CHECK(assign_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_l.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_ck.rank()(), 2);
  BOOST_CHECK(assign_ck.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_ck.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(assign_cl.rank()(), 2);
  BOOST_CHECK(assign_cl.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(assign_cl.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_assign_k.rank()(), 2);
  BOOST_CHECK(const_assign_k.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_assign_k.impl_.node_dim_(), 1);
  BOOST_CHECK_EQUAL(const_assign_l.rank()(), 2);
  BOOST_CHECK(const_assign_l.impl_.node_ == 0);
  BOOST_CHECK_EQUAL(const_assign_l.impl_.node_dim_(), 1);
}

BOOST_AUTO_TEST_CASE( test_range_iterator_equal )
{
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    range_iterator_false_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_true_type;
  typedef details::Const_Range_iterator
    <details::Dynamic_rank, pair_type, pair_type,
     Kdtree_node<pair_type>, range_bounds<pair_type, pair_less> >
    const_range_iterator_false_type;
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
  typedef details::Range_iterator
    <details::Dynamic_rank, pair_type, pair_type, Kdtree_node<pair_type>,
     range_bounds<pair_type, pair_less> >
    range_iterator_true_type;
  Kdtree_node<pair_type> node;
  node.parent = 0;
  node.right = 0;
  node.left = 0;
  node.value = pair_type(1, 2);
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
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.begin().node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Kdtree_2D_fixture::kdtree_type::iterator
      max = --fix.kdtree.end(), tmp = max, begin = fix.kdtree.begin();
    point2d value = *max;
    for (; tmp != begin; --tmp)
      { if (*tmp == value) { max = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == max.node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_minimum_empty )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 0; shrink != 20; ++shrink)
      {
        point2d low = { { shrink, shrink } };
        point2d high = { { 20, 20} };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to min.
        range_iterator it = range_iterator::minimum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_Kdtree_2D_fixture::kdtree_type::iterator
          min = fix.kdtree.begin(), end = fix.kdtree.end();
        for (; min != end && !spatial::details::match_all
               (fix.kdtree.rank(), *min, shrinking_bounds); ++min);
        BOOST_CHECK(it.impl_.node_ == min.node);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == (--fix.kdtree.end()).node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Kdtree_2D_fixture::kdtree_type::iterator
      min = fix.kdtree.begin(), tmp = min, end = fix.kdtree.end();
    point2d value = *min;
    for (; tmp != end; ++tmp) { if (*tmp == value) { min = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == min.node);
  }
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 20; shrink != 0; --shrink)
      {
        point2d low = { { 0, 0} };
        point2d high = { { shrink, shrink } };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
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
          { BOOST_CHECK(it.impl_.node_ == max.node); }
        else
          { BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_maximum_empty )
{
  {
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_increment )
{
  {
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, whole_tree_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
  {
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, small_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_pre_decrement )
{
  {
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, whole_tree_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
  {
    typedef open_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_Kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, small_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_range_iterator_post_decrement )
{
    typedef closed_range_bounds
      <Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
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
      Hundred_Kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
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

///////////////////  spatial_kdtree.hpp (find, erase value) ////////////////////

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
      typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                     bracket_less<point2d>, std::allocator<point2d>, false>
        kdtree_type;
      // 1D tree where we store 2d objects...
      kdtree_type tree(details::Dynamic_rank(1));
      point2d one   = { { 0, 1 } };
      point2d two   = { { 0, 2 } };
      point2d three = { { 0, 3 } };
      tree.insert(one);
      tree.insert(two);
      tree.insert(three);
      kdtree_type::iterator iter;
      kdtree_type::iterator end = tree.end();
      iter = tree.find(one);
      BOOST_REQUIRE(iter != end);
      BOOST_CHECK(one == *iter || two == *iter || three == *iter);
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
    typedef Kdtree<details::Dynamic_rank, point2d, point2d,
                   bracket_less<point2d>, std::allocator<point2d>, false>
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    point2d one   = { { 1, 0 } };
    point2d two   = { { 2, 0 } };
    point2d four  = { { 4, 0 } };
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

////////////////////////////  spatial_geometry.hpp  ////////////////////////////


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
    rebind<point2d, double,
                    accessor_less<at_accessor<point2d,
                                              int>, point2d> >::type
      must_compile;
  }
  {
    rebind<point2d, double,
                    bracket_less<point2d> >::type must_compile;
  }
  {
    rebind<point2d, double,
                    paren_less<point2d> >::type must_compile;
  }
  {
    rebind<point2d, double,
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
        double other_dist = sqrt(static_cast<double>((p.x-q.x)*(p.x-q.x)
                                 +(p.y-q.y)*(p.y-q.y)
                                 +(p.z-q.z)*(p.z-q.z)));
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
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::euclidian_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist = abs(static_cast<float>(access(dim, p) - access(dim, q)));
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
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::euclidian_square_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist
          = static_cast<float>((access(dim, p) - access(dim, q))
                               * (access(dim, p) - access(dim, q)));
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
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = ones;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (1, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = twos;
    r = math::manhattan_distance_to_plane
      <point2d, bracket_cast_accessor<point2d, float>, float>
      (0, x, x, bracket_cast_accessor<point2d, float>());
    BOOST_CHECK_CLOSE(r, .0f, .0000001f);
    x = threes;
    r = math::manhattan_distance_to_plane
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
        dimension_type dim = static_cast<dimension_type>(rand()) % 3;
        float dist = math::manhattan_distance_to_plane
          <triple, cast_accessor<triple, double, triple_access>, float>
          (dim, p, q, cast_accessor<triple, double, triple_access>());
        using namespace ::std;
        triple_access access;
        float other_dist = abs(static_cast<float>(access(dim, p) - access(dim, q)));
        BOOST_CHECK_CLOSE(dist, other_dist, .0000001f);
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
  {
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
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_distance_to_plane
        <triple, cast_accessor<triple, distance_type, triple_access>,
         distance_type>(dim, p, q,
                        cast_accessor<triple, distance_type, triple_access>());
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
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
  {
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
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
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
  {
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
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_square_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .000000000001);
    }
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
  {
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
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
    {
      dimension_type dim = static_cast<dimension_type>(rand()) % 3;
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::euclidian_square_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_CLOSE(r, s, .0000001f);
    }
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
  {
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
      dimension_type dim = static_cast<dimension_type>(rand() % 3);
      distance_type r = geometry.distance_to_plane(3, dim, p, q);
      distance_type s = math::manhattan_distance_to_plane
        <triple, triple_access, distance_type>(dim, p, q, access);
      BOOST_CHECK_EQUAL(r, s);
    }
  }
  {
    bracket_cast_accessor<point5d, double> access_double;
    manhattan<point5d, bracket_cast_accessor<point5d, double>, double>
      geometry_double(access_double);
    typedef manhattan<point5d, bracket_cast_accessor<point5d, double>, double>
      ::distance_type distance_type;
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
  typedef Kdtree<details::Static_rank<5>, point5d, point5d,
                 bracket_less<point5d>, std::allocator<point5d>, false>
  kdtree_type;
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
  details::Neighbor_iterator<details::Static_rank<2>, point2d, point2d,
                    Kdtree_node<point2d>, bracket_less<point2d>,
                    euclidian_double<point2d,
                                     bracket_cast_accessor<point2d, double> > >
    iter;
  details::Const_Neighbor_iterator<details::Static_rank<2>, point2d, point2d,
                          Kdtree_node<point2d>, bracket_less<point2d>,
                          euclidian_double<point2d,
                                           bracket_cast_accessor<point2d, double> > >
    citer;
  iter.impl_.node_ = 0;
  citer.impl_.node_ = 0;
  BOOST_CHECK(iter == citer);
  BOOST_CHECK(!(iter != citer));
}

BOOST_AUTO_TEST_CASE( test_neighbor_distance )
{
  using namespace spatial::details::geometry;
  details::Neighbor_iterator<details::Static_rank<2>, point2d, point2d,
                    Kdtree_node<point2d>, bracket_less<point2d>,
                    euclidian_double<point2d,
                                     bracket_cast_accessor<point2d, double> > >
    iter;
  iter.impl_.distance_ = .1;
  BOOST_CHECK_EQUAL(iter.distance(), .1);
}

BOOST_AUTO_TEST_CASE( test_neighbor_deference )
{
  using namespace spatial::details::geometry;
  details::Neighbor_iterator<details::Static_rank<2>, point2d, point2d,
                    Kdtree_node<point2d>, bracket_less<point2d>,
                    euclidian_double<point2d,
                                     bracket_cast_accessor<point2d, double> > >
    iter;
  details::Const_Neighbor_iterator<details::Static_rank<2>, point2d, point2d,
                          Kdtree_node<point2d>, bracket_less<point2d>,
                          euclidian_double<point2d,
                                           bracket_cast_accessor<point2d, double> > >
    citer;
  Kdtree_node<point2d> node;
  iter.impl_.node_ = &node;
  node.value = ones;
  BOOST_CHECK(*iter == ones);
  citer.impl_.node_ = &node;
  node.value = twos;
  BOOST_CHECK(*citer == twos);
  *iter = threes;
  BOOST_CHECK(node.value == threes);
}

BOOST_AUTO_TEST_CASE( test_neighbor_minimum )
{
  {
    using namespace spatial::details::geometry;
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type,
       bracket_cast_accessor<point2d, double> > geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
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
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK_EQUAL(iter.distance(), .0);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
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
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the closest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(min.impl_.node_ != fix.kdtree.end().node);
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
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
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
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(8.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
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
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(32.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the furthest in a tree with a lot of positions, cross-check
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(max.impl_.node_ != fix.kdtree.end().node);
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
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(++iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
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
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
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
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(),
                        geometry, target, 4,
                        static_cast<kdtree_type::node_type*>
                        (fix.kdtree.end().node));
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
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
  {
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(--iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
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
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
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
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, threes, 0,
                               fix.kdtree.end().node->parent);
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
  }
  {
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
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
  {
    // Return the smallest element in set that is greater or equal to limit.
    // Test with high density and oob values.
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == zeros);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be greater or equal to limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(limit, it.distance()); }
        --it;
        // dist to previous value should be stricly less than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_neighbor_upper_bound )
{
  {
    // Return the smallest element in set that is strictly greater than key.
    // Test with high density and oob values.
    typedef Empty_Kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
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
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be strictly greater than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(limit, it.distance()); }
        --it;
        // dist to previous value should be lower or equal limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(it.distance(), limit); }
      }
  }
}

///////////////////////////////  pointset.hpp  /////////////////////////////////

BOOST_AUTO_TEST_CASE( test_pointset_constructors )
{
  pointset<2, point2d> points;
  pointset<0, point2d> runtime_points;
  runtime_pointset<point2d> more_runtime_points;
}

BOOST_AUTO_TEST_CASE( test_frozen_pointset_constructors )
{
  frozen_pointset<2, point2d> points;
  frozen_pointset<0, point2d> runtime_points;
  runtime_frozen_pointset<point2d> more_runtime_points;
}

BOOST_AUTO_TEST_CASE( test_pointset_copy_assignment )
{
  pointset<2, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  pointset<2, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_pointset_copy_assignment )
{
  pointset<0, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  pointset<0, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_runtime_pointset_copy_assignment )
{
  runtime_pointset<point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  runtime_pointset<point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_frozen_pointset_copy_assignment )
{
  frozen_pointset<2, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  frozen_pointset<2, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_zero_frozen_pointset_copy_assignment )
{
  frozen_pointset<0, point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  frozen_pointset<0, point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

BOOST_AUTO_TEST_CASE( test_runtime_frozen_pointset_copy_assignment )
{
  runtime_frozen_pointset<point2d> points;
  points.insert(zeros);
  points.insert(ones);
  points.insert(twos);
  runtime_frozen_pointset<point2d> copy(points);
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
  points = copy;
  BOOST_CHECK_EQUAL(points.size(), copy.size());
  BOOST_CHECK(*points.begin() == *copy.begin());
}

///////////////////////////  pointset.hpp views ////////////////////////////////

BOOST_AUTO_TEST_CASE( test_pointset_mapping_view )
{
  pointset<2, point2d> points;
  mapping_view<pointset<2, point2d> > set_zero(points, 0);
  BOOST_CHECK(set_zero.begin() == set_zero.end());
  // Now add some points to pointset and iterate throught these points.
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(zeros);
  points.insert(fives);
  points.insert(fives);
  BOOST_CHECK(set_zero.begin() != set_zero.end());
  BOOST_CHECK(set_zero.cbegin() != set_zero.cend());
  BOOST_CHECK(set_zero.rbegin() != set_zero.rend());
  BOOST_CHECK(set_zero.crbegin() != set_zero.crend());
  BOOST_CHECK(*set_zero.begin() == zeros);
  BOOST_CHECK(*set_zero.cbegin() == zeros);
  BOOST_CHECK(*--set_zero.end() == fives);
  BOOST_CHECK(*--set_zero.cend() == fives);
  BOOST_CHECK(*set_zero.rbegin() == fives);
  BOOST_CHECK(*set_zero.crbegin() == fives);
  BOOST_CHECK(*set_zero.lower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.clower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.upper_bound(ones) == twos);
  BOOST_CHECK(*set_zero.cupper_bound(ones) == twos);
}

BOOST_AUTO_TEST_CASE( test_pointset_const_mapping_view )
{
  pointset<2, point2d> points;
  mapping_view<const pointset<2, point2d> > set_zero(points, 0);
  BOOST_CHECK(set_zero.begin() == set_zero.end());
  // Now add some points to pointset and iterate throught these points.
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(zeros);
  points.insert(fives);
  points.insert(fives);
  BOOST_CHECK(set_zero.begin() != set_zero.end());
  BOOST_CHECK(set_zero.cbegin() != set_zero.cend());
  BOOST_CHECK(set_zero.rbegin() != set_zero.rend());
  BOOST_CHECK(set_zero.crbegin() != set_zero.crend());
  BOOST_CHECK(*set_zero.begin() == zeros);
  BOOST_CHECK(*set_zero.cbegin() == zeros);
  BOOST_CHECK(*--set_zero.end() == fives);
  BOOST_CHECK(*--set_zero.cend() == fives);
  BOOST_CHECK(*set_zero.rbegin() == fives);
  BOOST_CHECK(*set_zero.crbegin() == fives);
  BOOST_CHECK(*set_zero.lower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.clower_bound(ones) == ones);
  BOOST_CHECK(*set_zero.upper_bound(ones) == twos);
  BOOST_CHECK(*set_zero.cupper_bound(ones) == twos);
}

BOOST_AUTO_TEST_CASE( test_pointset_range )
{
  pointset<2, point2d> points;
  // Now add some points to pointset and iterate throught these points.
  points.insert(zeros);
  points.insert(ones);
  points.insert(ones);
  points.insert(twos);
  points.insert(twos);
  points.insert(fours);
  points.insert(fives);
  range_predicate_view<pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range1(points, make_range_bounds(points, ones, threes));
  BOOST_CHECK(range1.begin() != range1.end());
  BOOST_CHECK(range1.cbegin() != range1.cend());
  range_predicate_view<pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range2(points, make_range_bounds(points, threes, fours));
  BOOST_CHECK(range2.begin() == range2.end());
  BOOST_CHECK(range2.cbegin() == range2.cend());
  range_predicate_view<const pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range3(points, make_range_bounds(points, threes, fours));
  BOOST_CHECK(range3.begin() == range3.end());
  BOOST_CHECK(range3.cbegin() == range3.cend());
  range_predicate_view<const pointset<2, point2d>,
    range_bounds<typename spatial::container_traits<pointset<2, point2d> >::key_type,
                 typename spatial::container_traits<pointset<2, point2d> >::key_compare> >
    range4(points, make_range_bounds(points, ones, threes));
  BOOST_CHECK(range4.begin() != range4.end());
  BOOST_CHECK(range4.cbegin() != range4.cend());
}

BOOST_AUTO_TEST_CASE( test_pointset_other_range_view )
{
  pointset<2, point2d> points;
  range_view<pointset<2, point2d> > view1(points, zeros, ones);
  range_view<const pointset<2, point2d> > view2(points, zeros, ones);
  closed_range_view<pointset<2, point2d> > view3(points, zeros, ones);
  closed_range_view<const pointset<2, point2d> > view4(points, zeros, ones);
  open_range_view<pointset<2, point2d> > view5(points, zeros, ones);
  open_range_view<const pointset<2, point2d> > view6(points, zeros, ones);
}

BOOST_AUTO_TEST_CASE( test_pointset_neighborhood_view )
{
  // Now add some points to pointset and iterate throught these points.
  BOOST_CHECK_MESSAGE(false, "test not implemented");
  pointset<2, point2d> points;
}

BOOST_AUTO_TEST_CASE( test_pointset_neighborhood_other_view )
{
  BOOST_CHECK_MESSAGE(false, "test not implemented");
}

/////////////////////////  spatial_relaxed_kdtree.hpp  /////////////////////////

BOOST_AUTO_TEST_CASE( test_loose_balancing )
{
  details::Dynamic_rank rank(2);
  loose_balancing test;
  // A leaf node is always balanced!
  BOOST_CHECK_EQUAL(test(rank, 0, 0), false);
  // rebalance even if no right.
  BOOST_CHECK_EQUAL(test(rank, 3, 0), true);
  // reblance even if no left.
  BOOST_CHECK_EQUAL(test(rank, 0, 3), true);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 4, 2), false);
  // should fail cause we are right under loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 6, 3), false);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 2, 6), true);
  // should pass cause we are above loose balancing conditions
  BOOST_CHECK_EQUAL(test(rank, 6, 2), true);
}

BOOST_AUTO_TEST_CASE( test_tight_balancing )
{
  {
    details::Dynamic_rank rank(2);
    tight_balancing test;
    // A leaf node is always balanced!
    BOOST_CHECK_EQUAL(test(rank, 0, 0), false);
    // rebalance even if no right.
    BOOST_CHECK_EQUAL(test(rank, 9, 0), true);
    // rebalance even if no left.
    BOOST_CHECK_EQUAL(test(rank, 0, 9), true);
    // should fail cause we are right under tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 4, 2), false);
    // should fail cause we are right under tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 2, 4), false);
    // should pass cause we are above tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 3, 6), true);
    // should pass cause we are above tight balancing conditions
    BOOST_CHECK_EQUAL(test(rank, 6, 3), true);
  }
  {
    details::Dynamic_rank rank(9);
    tight_balancing test;
    // high dimension prevent rebalance.
    BOOST_CHECK_EQUAL(test(rank, 1, 8), false);
    BOOST_CHECK_EQUAL(test(rank, 8, 1), false);
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_ctor )
{
  typedef details::Relaxed_kdtree
    <details::Dynamic_rank, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, true> kdtree_type;
  kdtree_type instance_one;
  kdtree_type instance_two(details::Dynamic_rank(2));
  kdtree_type instance_three(details::Dynamic_rank(2), bracket_less<point2d>());
  kdtree_type instance_four(details::Dynamic_rank(2), bracket_less<point2d>(),
                            tight_balancing());
  kdtree_type instance_five(details::Dynamic_rank(2), bracket_less<point2d>(),
                            tight_balancing(), std::allocator<point2d>());
  // Trees should be empty.
  BOOST_CHECK(instance_one.empty());
  BOOST_CHECK(instance_two.empty());
  BOOST_CHECK(instance_three.empty());
  BOOST_CHECK(instance_four.empty());
  BOOST_CHECK(instance_five.empty());
  // That also mean that beginning equal end for each of them
  BOOST_CHECK(instance_one.begin() == instance_one.end());
  BOOST_CHECK(instance_two.begin() == instance_two.end());
  BOOST_CHECK(instance_three.begin() == instance_three.end());
  BOOST_CHECK(instance_four.begin() == instance_four.end());
  BOOST_CHECK(instance_five.begin() == instance_five.end());
  // And that also mean that have a size of 0
  BOOST_CHECK_EQUAL(instance_one.size(), 0);
  BOOST_CHECK_EQUAL(instance_two.size(), 0);
  BOOST_CHECK_EQUAL(instance_three.size(), 0);
  BOOST_CHECK_EQUAL(instance_four.size(), 0);
  BOOST_CHECK_EQUAL(instance_five.size(), 0);
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_tight )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, false> kdtree_type;
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     tight_balancing, std::allocator<point2d>, true> const_kdtree_type;
  kdtree_type tree;
  kdtree_type::const_iterator it;
  BOOST_CHECK_NO_THROW(it = tree.insert(zeros));
  BOOST_CHECK(*it == zeros);
  BOOST_CHECK(!tree.empty());
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK(zeros == *tree.begin());
  BOOST_CHECK(++tree.begin() == tree.end());
  BOOST_CHECK(tree.begin() == --tree.end());
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_loose )
{
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, false> kdtree_type;
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, true> const_kdtree_type;
  kdtree_type tree;
  kdtree_type::const_iterator it;
  BOOST_CHECK_NO_THROW(it = tree.insert(zeros));
  BOOST_CHECK(*it == zeros);
  BOOST_CHECK(!tree.empty());
  BOOST_CHECK(tree.begin() != tree.end());
  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK(zeros == *tree.begin());
  BOOST_CHECK(++tree.begin() == tree.end());
  BOOST_CHECK(tree.begin() == --tree.end());
}

struct Twenty_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<3>, triple, triple, triple_less,
   tight_balancing, std::allocator<triple>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<triple> mem;
  Twenty_Relaxed_kdtree_3D_fixture() : kdtree(), mem()
  {
    for(int i = 0; i != 20; ++i)
    {
      mem.reserve(20);
      triple t;
      t.x = rand() % 10 - 5; // small range to have colisions
      t.y = rand() % 10 - 5;
      t.z = rand() % 10 - 5;
      mem.push_back(t);
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Twenty_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<point2d> mem;
  Twenty_Relaxed_kdtree_2D_fixture() : kdtree(), mem()
  {
    for(int i = 0; i != 20; ++i)
    {
      mem.reserve(20);
      point2d t;
      t[0] = rand() % 10 - 5; // small range to have colisions
      t[1] = rand() % 10 - 5;
      mem.push_back(t);
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Grow_Relaxed_Kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Grow_Relaxed_Kdtree_2D_fixture() : kdtree()
  {
    for(int i = 0; i != 20; ++i)
    {
      point2d t;
      t[0] = i;
      t[1] = i;
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Reduce_Relaxed_Kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   tight_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Reduce_Relaxed_Kdtree_2D_fixture() : kdtree()
  {
    for(int i = 20; i != 0; --i)
    {
      point2d t;
      t[0] = i;
      t[1] = i;
      kdtree_type::const_iterator it = kdtree.insert(t);
      BOOST_CHECK(*it == t);
    }
  }
};

struct Hundred_Relaxed_kdtree_5D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<5>, point5d, point5d, bracket_less<point5d>,
   loose_balancing, std::allocator<point5d>, false> kdtree_type;

  kdtree_type kdtree;
  std::vector<point5d> array;
  Hundred_Relaxed_kdtree_5D_fixture() : kdtree()
  {
    array.reserve(100);
    for(int i = 0; i != 100; ++i)
    {
      point5d p;
      p[0] = rand() % 20 - 10;
      p[1] = rand() % 20 - 10;
      p[2] = rand() % 20 - 10;
      p[3] = rand() % 20 - 10;
      p[4] = rand() % 20 - 10;
      array.push_back(p);
      kdtree_type::const_iterator it = kdtree.insert(p);
      BOOST_CHECK(*it == p);
    }
  }
};

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_growing )
{
  Grow_Relaxed_Kdtree_2D_fixture fix;
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 0;
  for (Grow_Relaxed_Kdtree_2D_fixture::kdtree_type::iterator it
         = fix.kdtree.begin(); it != fix.kdtree.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_reduce )
{
  Reduce_Relaxed_Kdtree_2D_fixture fix;
  // because the tree is ordered on all dimensions and nodes are all sequential
  // values, iteration should be ordered as well.
  int i = 1; // Values inserted in [1:20], not [0:19]
  for (Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type::iterator it
         = fix.kdtree.begin(); it != fix.kdtree.end(); ++it, ++i)
    {
      BOOST_CHECK_EQUAL((*it)[0], i);
      BOOST_CHECK_EQUAL((*it)[1], i);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_insert_lots )
{
  {
    // build repeatitively a lot of them, should not fail.
    // This tree has multiple similar node, by construction
    for (int i=0; i<100; ++i)
      {
        Twenty_Relaxed_kdtree_2D_fixture fix;
        int count = 0;
        for (Twenty_Relaxed_kdtree_2D_fixture::kdtree_type
               ::iterator it = fix.kdtree.begin(); it != fix.kdtree.end(); ++it)
          {
            BOOST_CHECK(std::find(fix.mem.begin(), fix.mem.end(), *it)
                        != fix.mem.end());
            ++count;
          }
        BOOST_CHECK_EQUAL(count, 20);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_copy )
{
  // Copy an 3d kdtree and iterate over it. Both iterators should yield same
  // results and exhibit same properties.
  Twenty_Relaxed_kdtree_3D_fixture fix;
  Twenty_Relaxed_kdtree_3D_fixture::kdtree_type copy(fix.kdtree);
  BOOST_CHECK_EQUAL(fix.kdtree.size(), copy.size());
  typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type::iterator iterator_type;
  iterator_type orig_begin = fix.kdtree.begin();
  iterator_type orig_end = fix.kdtree.end();
  iterator_type copy_begin = copy.begin();
  iterator_type copy_end = copy.end();
  for (; orig_begin != orig_end && copy_begin != copy_end;
       ++copy_begin, ++orig_begin)
    {
      BOOST_CHECK(*orig_begin == *copy_begin);
      if (orig_begin.node->left == 0) BOOST_CHECK(copy_begin.node->left == 0);
      if (orig_begin.node->right == 0) BOOST_CHECK(copy_begin.node->right == 0);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_assignment )
{
  // Copy an 3d kdtree and iterate over it. Both iterators should yield same
  // results and exhibit same properties.
  Twenty_Relaxed_kdtree_3D_fixture fix;
  Twenty_Relaxed_kdtree_3D_fixture fix_other;
  fix_other.kdtree = fix.kdtree;
  typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type::iterator iterator_type;
  iterator_type orig_begin = fix.kdtree.begin();
  iterator_type orig_end = fix.kdtree.end();
  iterator_type copy_begin = fix_other.kdtree.begin();
  iterator_type copy_end = fix_other.kdtree.end();
  for (; orig_begin != orig_end && copy_begin != copy_end;
       ++copy_begin, ++orig_begin)
    {
      BOOST_CHECK(*orig_begin == *copy_begin);
      if (orig_begin.node->left == 0) BOOST_CHECK(copy_begin.node->left == 0);
      if (orig_begin.node->right == 0) BOOST_CHECK(copy_begin.node->right == 0);
    }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_bulk_insert )
{
  // reuse test_kdtree_insert_100_iterate_forward
  typedef details::Relaxed_kdtree
    <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
     loose_balancing, std::allocator<point2d>, false> kdtree_type;
  kdtree_type kdtree;
  std::tr1::array<point2d, 100> points;
  for(std::tr1::array<point2d, 100>::iterator i
        = points.begin(); i != points.end(); ++i)
    {
      (*i)[0] = rand() % 20;
      (*i)[1] = rand() % 20;
    }
  BOOST_REQUIRE_NO_THROW(kdtree.insert(points.begin(), points.end()));
  BOOST_CHECK(kdtree.empty() == false);
  BOOST_REQUIRE_EQUAL(kdtree.size(), 100);
  BOOST_CHECK_EQUAL(kdtree.count(), 100);
  int count = 0;
  for(kdtree_type::iterator i = kdtree.begin(); i != kdtree.end(); ++i)
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

struct Empty_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   loose_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Empty_Relaxed_kdtree_2D_fixture() : kdtree()
  { }
};

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_key )
{
  {
    // Erase on empty tree should return 0.
    Empty_Relaxed_kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    size_type count = fix.kdtree.erase(some_value);
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(fix.kdtree.end() == fix.kdtree.begin());
    BOOST_CHECK(fix.kdtree.empty());
  }
  {
    // Erase one value of a tree should return 1 and leave an empty tree.
    Empty_Relaxed_kdtree_2D_fixture fix;
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
    Empty_Relaxed_kdtree_2D_fixture fix;
    point2d some_value = { {0, 0} };
    point2d other_value = { {1, 1} };
    fix.kdtree.insert(some_value);
    Empty_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      begin_before = fix.kdtree.begin();
    Empty_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
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
    typedef details::Relaxed_kdtree
      <details::Dynamic_rank, point2d, point2d, bracket_less<point2d>,
       tight_balancing, std::allocator<point2d>, false>
      kdtree_type;
    // 1D tree where we store 2d objects...
    kdtree_type tree(details::Dynamic_rank(1));
    point2d one   = { { 1, 0 } };
    point2d two   = { { 2, 2 } };
    point2d four  = { { 4, 0 } };
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

struct Empty_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<3>, triple, triple, triple_less,
   loose_balancing, std::allocator<triple>, false> kdtree_type;

  kdtree_type kdtree;
  Empty_Relaxed_kdtree_3D_fixture() : kdtree()
  { }
};

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_iterator )
{
  // check that erase at edge preserve basic iterators
  {
    Twenty_Relaxed_kdtree_3D_fixture fix;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    size_type track_size = fix.kdtree.size();
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
    Twenty_Relaxed_kdtree_3D_fixture fix;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::iterator iter_type;
    typedef Twenty_Relaxed_kdtree_3D_fixture::kdtree_type
      ::reverse_iterator riter_type;
    size_type track_size = fix.kdtree.size();
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
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::node_type,
      bracket_less<point5d> > mapping_iterator;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser, static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator begin_0;
        BOOST_REQUIRE_NO_THROW(begin_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_0;
        BOOST_REQUIRE_NO_THROW(end_0 = mapping_iterator
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0,
                                details::decr_dim(fix.kdtree.rank(), 0),
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node)));
        mapping_iterator begin_1;
        BOOST_REQUIRE_NO_THROW(begin_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator end_1;
        BOOST_REQUIRE_NO_THROW(end_1 = mapping_iterator
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1,
                                details::decr_dim(fix.kdtree.rank(), 0),
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node)));
        size_type count = 0;
        for(mapping_iterator i = begin_0; i != end_0; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 23) abort(); // for inspection of tree
        count = 0;
        for(mapping_iterator i = begin_1; i != end_1; ++i, ++count);
        BOOST_CHECK_EQUAL(count, track_size);
        if (count != track_size && count < 23) abort(); // for inspection of tree
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

BOOST_AUTO_TEST_CASE( test_Relaxed_kdtree_erase_bulk )
{
  Hundred_Relaxed_kdtree_5D_fixture fix;
  std::vector<point5d> store;
  store.reserve(100);
  store.insert(store.end(), fix.kdtree.begin(), fix.kdtree.end());
  BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(store.begin(), store.end()));
  BOOST_CHECK(fix.kdtree.empty() == true);
}

////////////////// spatial_mapping.hpp (with Relaxed_kdtree) ///////////////////

struct Seven_Relaxed_kdtree_node_fixture
{
  /*              H
                  |
               (2,2)7
            /          \
      (1,1)3           (3,3)3
       /  \             /
  (0,0)1  (1,1)1     (3,3)2
                       \
                     (3,3)1     */
  Node_base header;
  Node_base::Base_ptr leftmost;
  Relaxed_kdtree_node<point2d> node_root;
  Relaxed_kdtree_node<point2d> node_left;
  Relaxed_kdtree_node<point2d> node_left_left;
  Relaxed_kdtree_node<point2d> node_left_right;
  Relaxed_kdtree_node<point2d> node_right;
  Relaxed_kdtree_node<point2d> node_right_left;
  Relaxed_kdtree_node<point2d> node_right_left_right;
  Seven_Relaxed_kdtree_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    leftmost = &node_left_left;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_root.weight = 7;
    node_root.value = twos;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_left.weight = 3;
    node_left.value = ones;
    node_right.parent = &node_root;
    node_right.left = &node_right_left;
    node_right.right = 0;
    node_right.weight = 3;
    node_right.value = threes;
    node_right_left.parent = &node_right;
    node_right_left.left = 0;
    node_right_left.right = &node_right_left_right;
    node_right_left.weight = 2;
    node_right_left.value = threes;
    node_right_left_right.parent = &node_right_left;
    node_right_left_right.left = 0;
    node_right_left_right.right = 0;
    node_right_left_right.weight = 1;
    node_right_left_right.value = threes;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_right.weight = 1;
    node_left_right.value = ones;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
    node_left_left.weight = 1;
    node_left_left.value = zeros;
  }
};

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_increment )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it(details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
                     &fix.node_left_left);
    BOOST_CHECK(*it == zeros);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_CHECK(it.impl_.node_ == &fix.header);
  }
  {
    Grow_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Grow_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    int old = (*begin)[0];
    int count = 0;
    for(; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[0] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[0];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    old = (*begin)[1];
    for(count = 0; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[1] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[1];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
  {
    Reduce_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    int old = (*begin)[0];
    int count = 0;
    for(; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[0] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[0];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    old = (*begin)[1];
    for(count = 0; count != 20; ++begin, ++count)
      {
        BOOST_CHECK((*begin)[1] >= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*begin)[1];
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_maxium )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it = iterator_type::maximum
      (details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
       static_cast<Node_base*>(&fix.node_root));
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    --it;
    BOOST_REQUIRE(it.impl_.node_ == &fix.header);
  }
  // erase and check that max value found match returned
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::node_type,
       bracket_less<point5d> > mapping_iterator;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser,
                     static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator max_0;
        BOOST_REQUIRE_NO_THROW(max_0 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator max_1;
        BOOST_REQUIRE_NO_THROW(max_1 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator max_2;
        BOOST_REQUIRE_NO_THROW(max_2 = mapping_iterator::maximum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 2, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        double found_max_0 = -11.0; // absolute lower
        Node_base* found_max_0_address = 0;
        double found_max_1 = -11.0;
        Node_base* found_max_1_address = 0;
        double found_max_2 = -11.0;
        Node_base* found_max_2_address = 0;
        for(iterator i = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
          {
            if ((*i)[0] > found_max_0)
              {
                found_max_0 = (*i)[0];
                found_max_0_address = i.node;
              }
            if ((*i)[0] == found_max_0 && found_max_0_address < i.node)
              {
                found_max_0_address = i.node;
              }
            if ((*i)[1] > found_max_1)
              {
                found_max_1 = (*i)[1];
                found_max_1_address = i.node;
              }
            if ((*i)[1] == found_max_1 && found_max_1_address < i.node)
              {
                found_max_1_address = i.node;
              }
            if ((*i)[2] > found_max_2)
              {
                found_max_2 = (*i)[2];
                found_max_2_address = i.node;
              }
            if ((*i)[2] == found_max_2 && found_max_2_address < i.node)
              {
                found_max_2_address = i.node;
              }
          }
        BOOST_CHECK_EQUAL((*max_0)[0], found_max_0);
        BOOST_CHECK_EQUAL(max_0.impl_.node_, found_max_0_address);
        BOOST_CHECK_EQUAL((*max_1)[1], found_max_1);
        BOOST_CHECK_EQUAL(max_1.impl_.node_, found_max_1_address);
        BOOST_CHECK_EQUAL((*max_2)[2], found_max_2);
        BOOST_CHECK_EQUAL(max_2.impl_.node_, found_max_2_address);
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_minimum )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it = iterator_type::minimum
      (details::Static_rank<2>(), bracket_less<point2d>(), 0, 0,
       static_cast<Node_base*>(&fix.node_root));
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    ++it;
    BOOST_CHECK(it.impl_.node_ == &fix.header);
  }
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type
      ::iterator iterator;
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_5D_fixture::kdtree_type::node_type,
      bracket_less<point5d> > mapping_iterator;
    Hundred_Relaxed_kdtree_5D_fixture fix;
    size_type track_size = fix.kdtree.size();
    while (fix.kdtree.size() != 0)
      {
        iterator eraser = fix.kdtree.begin();
        std::advance(eraser,
                     static_cast<size_type>(rand()) % fix.kdtree.size());
        mapping_iterator min_0;
        BOOST_REQUIRE_NO_THROW(min_0 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 0, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator min_1;
        BOOST_REQUIRE_NO_THROW(min_1 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 1, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        mapping_iterator min_2;
        BOOST_REQUIRE_NO_THROW(min_2 = mapping_iterator::minimum
                               (fix.kdtree.rank(), fix.kdtree.key_comp(), 2, 0,
                                static_cast<Relaxed_kdtree_node<point5d>*>
                                (fix.kdtree.end().node->parent)));
        double found_min_0 = 11.0; // absolute upper
        Node_base* found_min_0_address = 0;
        double found_min_1 = 11.0;
        Node_base* found_min_1_address = 0;
        double found_min_2 = 11.0;
        Node_base* found_min_2_address = 0;
        for(iterator i = fix.kdtree.begin(); i != fix.kdtree.end(); ++i)
          {
            if ((*i)[0] < found_min_0)
              {
                found_min_0 = (*i)[0];
                found_min_0_address = i.node;
              }
            if ((*i)[0] == found_min_0 && found_min_0_address > i.node)
              {
                found_min_0_address = i.node;
              }
            if ((*i)[1] < found_min_1)
              {
                found_min_1 = (*i)[1];
                found_min_1_address = i.node;
              }
            if ((*i)[1] == found_min_1 && found_min_1_address > i.node)
              {
                found_min_1_address = i.node;
              }
            if ((*i)[2] < found_min_2)
              {
                found_min_2 = (*i)[2];
                found_min_2_address = i.node;
              }
            if ((*i)[2] == found_min_2 && found_min_2_address > i.node)
              {
                found_min_2_address = i.node;
              }
          }
        BOOST_CHECK_EQUAL((*min_0)[0], found_min_0);
        BOOST_CHECK_EQUAL(min_0.impl_.node_, found_min_0_address);
        BOOST_CHECK_EQUAL((*min_1)[1], found_min_1);
        BOOST_CHECK_EQUAL(min_1.impl_.node_, found_min_1_address);
        BOOST_CHECK_EQUAL((*min_2)[2], found_min_2);
        BOOST_CHECK_EQUAL(min_2.impl_.node_, found_min_2_address);
        BOOST_REQUIRE_NO_THROW(fix.kdtree.erase(eraser));
        BOOST_CHECK_EQUAL(fix.kdtree.size(), --track_size);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_Relaxed_mapping_decrement )
{
  {
    // Test basic 7 node tree first.
    Seven_Relaxed_kdtree_node_fixture fix;
    typedef details::Const_Mapping_iterator
      <details::Static_rank<2>, point2d, point2d,
       Relaxed_kdtree_node<point2d>, bracket_less<point2d> > iterator_type;
    iterator_type it(details::Static_rank<2>(), bracket_less<point2d>(), 0, 1,
                     static_cast<Relaxed_kdtree_node<point2d>*>(&fix.header));
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == threes);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == twos);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == ones);
    --it;
    BOOST_REQUIRE(it.impl_.node_ != &fix.header);
    BOOST_CHECK(*it == zeros);
    --it;
    BOOST_REQUIRE(it.impl_.node_ == &fix.header);
  }
  {
    Grow_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Grow_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    iterator_type before_end = end; --before_end;
    int old = (*before_end)[0];
    int count = 0;
    for(; count != 20; --end, ++count)
      {
        BOOST_CHECK(old >= (*before_end)[0]);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[0];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    before_end = end; --before_end;
    old = (*before_end)[1];
    for(count = 0; count != 20; --end, ++count)
      {
        BOOST_CHECK((*before_end)[1] <= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[1];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
  {
    Reduce_Relaxed_Kdtree_2D_fixture fix;
    typedef details::mapping::iterator
      <Reduce_Relaxed_Kdtree_2D_fixture::kdtree_type>::type iterator_type;
    iterator_type end = details::mapping::end(fix.kdtree, 0);
    iterator_type begin = details::mapping::begin(fix.kdtree, 0);
    iterator_type before_end = end; --before_end;
    int old = (*before_end)[0];
    int count = 0;
    for(; count != 20; --end, ++count)
      {
        BOOST_CHECK(old >= (*before_end)[0]);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[0];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
    end = details::mapping::end(fix.kdtree, 1);
    begin = details::mapping::begin(fix.kdtree, 1);
    before_end = end; --before_end;
    old = (*before_end)[1];
    for(count = 0; count != 20; --end, ++count)
      {
        BOOST_CHECK((*before_end)[1] <= old);
        BOOST_CHECK(begin != end);
        if (begin == end) break;
        old = (*before_end)[1];
        if (before_end != begin) --before_end;
      }
    BOOST_CHECK(begin == end);
    BOOST_CHECK_EQUAL(count, 20);
  }
}

struct Dispersed_Relaxed_kdtree_3D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Dynamic_rank, triple, triple, triple_less, tight_balancing,
   std::allocator<triple>, false> kdtree_type;
  kdtree_type kdtree;
  Dispersed_Relaxed_kdtree_3D_fixture() : kdtree(details::Dynamic_rank(3))
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

struct Hundred_Relaxed_kdtree_2D_fixture
{
  typedef details::Relaxed_kdtree
  <details::Static_rank<2>, point2d, point2d, bracket_less<point2d>,
   loose_balancing, std::allocator<point2d>, false> kdtree_type;

  kdtree_type kdtree;
  Hundred_Relaxed_kdtree_2D_fixture() : kdtree()
  {
    for(int i = 0; i != 100; ++i)
    {
      point2d p;
      p[0] = rand() % 20;
      p[1] = rand() % 20;
      kdtree_type::const_iterator it = kdtree.insert(p);
      BOOST_CHECK(*it == p);
    }
  }
};

BOOST_AUTO_TEST_CASE( test_Relaxed_Mapping_lower_bound )
{
  // return the smallest element in set that is greater or equal to key
  // test with high density and oob values
  {
    dimension_type mapping_dim = 1;
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef details::Mapping_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare>
        iter_type;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), low_flag));
    iter_type high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = iter_type::lower_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), high_flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Hundred_Relaxed_kdtree_2D_fixture
                            ::kdtree_type::node_type*>
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
    Dispersed_Relaxed_kdtree_3D_fixture fix;
    typedef details::Mapping_iterator
      <Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::rank_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::value_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::node_type,
       Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_compare>
      iter_type;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    iter_type iter;
    BOOST_REQUIRE_NO_THROW(iter = iter_type::lower_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent), flag));
    iter_type begin;
    BOOST_REQUIRE_NO_THROW(begin = iter_type::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0,
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
                            (fix.kdtree.end().node->parent)));
    iter_type end;
    BOOST_REQUIRE_NO_THROW(end = iter_type
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Dispersed_Relaxed_kdtree_3D_fixture
                            ::kdtree_type::node_type*>
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

BOOST_AUTO_TEST_CASE( test_Relaxed_Mapping_upper_bound )
{
  // return the smallest element in set that is strictly greater than key
  // test with high density and oob values
  {
    typedef details::Const_Mapping_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bracket_less<point2d> > mapping_iterator;
    dimension_type mapping_dim = 1;
    Hundred_Relaxed_kdtree_2D_fixture fix;
    point2d flag = { { 10, 10 } };
    point2d low_flag = { { -10, -10 } };
    point2d high_flag = { { 30, 30 } };
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator low_iter;
    BOOST_REQUIRE_NO_THROW(low_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), low_flag));
    mapping_iterator high_iter;
    BOOST_REQUIRE_NO_THROW(high_iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent), high_flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<point2d>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(), fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Relaxed_kdtree_node<point2d>*>
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
      <Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::rank_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::key_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::value_type,
      Dispersed_Relaxed_kdtree_3D_fixture::kdtree_type::node_type,
      triple_less> mapping_iterator;
    dimension_type mapping_dim = 2;  // triple::z
    Dispersed_Relaxed_kdtree_3D_fixture fix;
    triple flag; flag.x = 0; flag.y = 0; flag.z = 0;
    mapping_iterator iter;
    BOOST_REQUIRE_NO_THROW(iter = mapping_iterator::upper_bound
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent), flag));
    mapping_iterator begin;
    BOOST_REQUIRE_NO_THROW(begin = mapping_iterator::minimum
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, 0, static_cast<Relaxed_kdtree_node<triple>*>
                            (fix.kdtree.end().node->parent)));
    mapping_iterator end;
    BOOST_REQUIRE_NO_THROW(end = mapping_iterator
                           (fix.kdtree.rank(),
                            fix.kdtree.key_comp(),
                            mapping_dim, details::decr_dim
                            (fix.kdtree.rank(), 0),
                            static_cast<Relaxed_kdtree_node<triple>*>
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

/////////////////// spatial_range.hpp (with Relaxed_kdtree) ////////////////////

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_minimum )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.begin().node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      max = --fix.kdtree.end(), tmp = max, begin = fix.kdtree.begin();
    point2d value = *max;
    for (; tmp != begin; --tmp)
      { if (*tmp == value) { max = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == max.node);
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_minimum_empty )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::minimum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 0; shrink != 20; ++shrink)
      {
        point2d low = { { shrink, shrink } };
        point2d high = { { 20, 20} };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to min.
        range_iterator it = range_iterator::minimum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
          min = fix.kdtree.begin(), end = fix.kdtree.end();
        for (; min != end && !spatial::details::match_all
               (fix.kdtree.rank(), *min, shrinking_bounds); ++min);
        BOOST_CHECK(it.impl_.node_ == min.node);
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_maximum )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == (--fix.kdtree.end()).node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef equal_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
      min = fix.kdtree.begin(), tmp = min, end = fix.kdtree.end();
    point2d value = *min;
    for (; tmp != end; ++tmp) { if (*tmp == value) { min = tmp; } }
    bounds_type equal(fix.kdtree.key_comp(), value);
    // In this case, the minimum of the interval must be equal to max.
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), equal, 0, fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == min.node);
  }
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // reverse in-order iteration until we hit a different key
    for (int shrink = 20; shrink != 0; --shrink)
      {
        point2d low = { { 0, 0} };
        point2d high = { { shrink, shrink } };
        bounds_type shrinking_bounds(fix.kdtree.key_comp(), low, high);
        // In this case, the minimum of the interval must be equal to max.
        range_iterator it = range_iterator::maximum
          (fix.kdtree.rank(), shrinking_bounds,
           0, fix.kdtree.end().node->parent);
        Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::iterator
          max = fix.kdtree.end(), begin = fix.kdtree.begin();
        for (; max != begin && !spatial::details::match_all
               (fix.kdtree.rank(), *(--max), shrinking_bounds););
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, shrinking_bounds))
          { BOOST_CHECK(it.impl_.node_ == max.node); }
        else
          { BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_maximum_empty )
{
  {
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds are too narrow to contain anything
    point2d mid = { {10, 10} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), mid, mid);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {20, 20} };
    point2d high = { {30, 30} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
  {
    // These bounds do not intersect with the tree bounding box
    Hundred_Relaxed_kdtree_2D_fixture fix;
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    // These bounds do not intersect with the tree bounding box
    point2d low = { {-10, -10} };
    point2d high = { {0, 0} };
    bounds_type empty_bounds(fix.kdtree.key_comp(), low, high);
    // In this case, the minimum of the interval must be equal to begin().
    range_iterator it = range_iterator::maximum
      (fix.kdtree.rank(), empty_bounds, 0,
       fix.kdtree.end().node->parent);
    BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_pre_increment )
{
  {
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), whole_tree_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, whole_tree_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
  {
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 8} };
    point2d high = { {11, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_min = range_iterator::minimum
      (fix.kdtree.rank(), small_box, 0,
       fix.kdtree.end().node->parent);
    const_iterator min; min.node = range_min.impl_.node_;
    const_iterator end(fix.kdtree.end());
    for(; min != end; ++min)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *min, small_box))
          {
            BOOST_CHECK(range_min.impl_.node_ == min.node);
            ++range_min;
          }
      }
    BOOST_CHECK(range_min.impl_.node_ == min.node); // at end!
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_pre_decrement )
{
  {
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, whole_tree_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
  {
    typedef open_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds encloses only a small part of the tree..
    point2d low = { {7, 7} };
    point2d high = { {12, 12} };
    bounds_type small_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator range_max = range_iterator
      (fix.kdtree.rank(), small_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    const_iterator max; max.node = range_max.impl_.node_;
    const_iterator begin(fix.kdtree.begin());
    for(; max != begin; --max)
      {
        if (spatial::details::match_all
            (fix.kdtree.rank(), *max, small_box))
          {
            --range_max;
            BOOST_CHECK(range_max.impl_.node_ == max.node);
          }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_post_decrement )
{
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
       Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
       bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
    // Check in-order transversal
    range_iterator test = range_iterator
      (fix.kdtree.rank(), whole_tree_box, 1,
       static_cast<Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type *>
       (fix.kdtree.end().node));
    range_iterator before = test;
    range_iterator after = test--;
    BOOST_CHECK(before == after);
    --after;
    BOOST_CHECK(after == test);
}

BOOST_AUTO_TEST_CASE( test_relaxed_range_iterator_post_increment )
{
    typedef closed_range_bounds
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_compare> bounds_type;
    typedef details::Const_Range_iterator
      <Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::rank_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::key_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::value_type,
      Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::node_type,
      bounds_type> range_iterator;
    typedef Hundred_Relaxed_kdtree_2D_fixture::kdtree_type::const_iterator
      const_iterator;
    // in order range iteration.
    Hundred_Relaxed_kdtree_2D_fixture fix;
    // bounds totally encloses the tree, whose elements are between 0 and 20.
    point2d low = { {0, 0} };
    point2d high = { {20, 20} };
    bounds_type whole_tree_box(fix.kdtree.key_comp(), low, high);
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

///////////////// spatial_neighbor.hpp (with Relaxed_kdtree) ///////////////////

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_minimum )
{
  {
    using namespace spatial::details::geometry;
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type,
       bracket_cast_accessor<point2d, double> > geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected closest at on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == ones);
      BOOST_CHECK_EQUAL(iter.distance(), .0);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected closest at on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the closest in a tree with a lot of positions, cross-check
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(min.impl_.node_ != fix.kdtree.end().node);
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

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_maximum )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should always return that node.
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK_CLOSE(iter.impl_.distance_(), std::sqrt(2.0), .000000000001);
      BOOST_CHECK(iter.impl_.target_() == ones);
    }
    {
      // Find the expected furthest on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, ones, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == threes);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(8.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                               geometry, fours, 0,
                               fix.kdtree.end().node->parent);
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK_CLOSE(iter.distance(), std::sqrt(32.0), .000000000001);
      BOOST_CHECK(iter.impl_.node_ != fix.kdtree.end().node);
    }
  }
  {
    // Find the furthest in a tree with a lot of positions, cross-check
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(max.impl_.node_ != fix.kdtree.end().node);
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

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_increment )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(++iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
  }
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(),
                        geometry, target, 4,
                        static_cast<kdtree_type::node_type*>
                        (fix.kdtree.end().node));
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
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

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_decrement )
{
  {
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef euclidian_square_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // If tree has one node, it should iterate to the end
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type(fix.kdtree.rank(), fix.kdtree.key_comp(), geometry, zeros,
                      0, static_cast<kdtree_type::node_type*>
                      (fix.kdtree.end().node->parent), 0.0);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.begin().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 0);
      BOOST_CHECK(iter.impl_.target_() == zeros);
      BOOST_CHECK_NO_THROW(--iter);
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == zeros);
    }
    {
      // Find the expected nodes on a left-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(threes);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
    {
      // Find the expected furthest on a right-unblanced tree
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      fix.kdtree.insert(ones);
      fix.kdtree.insert(twos);
      fix.kdtree.insert(threes);
      geometry_type geometry;
      iterator_type iter =
        iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
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
      BOOST_CHECK(iter.impl_.node_ == fix.kdtree.end().node);
      BOOST_CHECK(iter.impl_.node_dim_ == 1);
      BOOST_CHECK(iter.impl_.target_() == threes);
      --iter;
      BOOST_CHECK(*iter == zeros);
      BOOST_CHECK(iter.distance() == 18);
    }
  }
  {
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef euclidian_double
      <kdtree_type::key_type, kdtree_type::key_compare> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          iterator_type::maximum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        iterator_type end =
          iterator_type::minimum(fix.kdtree.rank(), fix.kdtree.key_comp(),
                                 geometry, target, 0,
                                 fix.kdtree.end().node->parent);
        BOOST_REQUIRE(iter.impl_.node_ != fix.kdtree.end().node);
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

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_lower_bound )
{
  {
    // Return the smallest element in set that is greater or equal to limit.
    // Test with high density and oob values.
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::lower_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == zeros);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be greater or equal to limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(limit, it.distance()); }
        --it;
        // dist to previous value should be stricly less than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(it.distance(), limit); }
      }
  }
}

BOOST_AUTO_TEST_CASE( test_relaxed_neighbor_upper_bound )
{
  {
    // Return the smallest element in set that is strictly greater than key.
    // Test with high density and oob values.
    typedef Empty_Relaxed_kdtree_2D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    {
      // Check that there is no failure out of limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
      // Check that there is no failure in limits.
      fix.kdtree.insert(ones);
      it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 1.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ != fix.kdtree.end().node);
      BOOST_CHECK(*it == ones);
    }
    {
      // Check that there is no failure in limits
      Empty_Relaxed_kdtree_2D_fixture fix;
      fix.kdtree.insert(zeros);
      geometry_type geometry;
      iterator_type it = iterator_type::upper_bound
        (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
         zeros, 0.f, 0, fix.kdtree.end().node->parent);
      BOOST_CHECK(it.impl_.node_ == fix.kdtree.end().node);
    }
  }
  {
    // On random sets, check that the appropriate nodes are found.
    typedef Hundred_Relaxed_kdtree_5D_fixture::kdtree_type kdtree_type;
    typedef manhattan
      <kdtree_type::key_type, kdtree_type::key_compare, float> geometry_type;
    typedef details::Neighbor_iterator
      <kdtree_type::rank_type, kdtree_type::key_type, kdtree_type::value_type,
       kdtree_type::node_type, kdtree_type::key_compare,
       geometry_type> iterator_type;
    Hundred_Relaxed_kdtree_5D_fixture fix;
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
          (fix.kdtree.rank(), fix.kdtree.key_comp(), geometry,
           origin, limit, 0, fix.kdtree.end().node->parent);
        // dist to value found should be strictly greater than limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LT(limit, it.distance()); }
        --it;
        // dist to previous value should be lower or equal limit or end
        if (it.impl_.node_ != fix.kdtree.end().node)
          { BOOST_CHECK_LE(it.distance(), limit); }
      }
  }
}

///////////////////////////  spatial_intersect.hpp  ////////////////////////////


////////////////////////////  spatial_include.hpp  /////////////////////////////


////////////////////////////////  boxset.hpp  //////////////////////////////////


/////////////////  spatial_mapping.hpp (with relaxed kd-tree)  /////////////////


//////////////  spatial_proximity.hpp (with relaxed kd-tree)  //////////////////
