/**
 *  \page ConstTime Constant Time Complexity
 *  \complex Describe what is Constant Time Complexity.
 *
 *  Functions with constant time complexity \O1 will run and return their output
 *  in very similar amounts of time regardless of the size of the elements that
 *  may be involved in the calculation.
 *
 *  Example:
 *  The function \mono{size()} in a container always in a constant amount of
 *  time, regardless of the number of elements contained in the container.
 *
 *  That is because, in order to improve the speed of the function, we sacrifice
 *  some memory space, by storing a cached value of the number of element. The
 *  function could be written in this way:
 *
 *  \code
 *  size_type size() const
 *  {
 *      return this->cached_sized;
 *  }
 *  \endcode
 *
 *  And fetching an integer value from a location in memory takes similar amount
 *  of time everytime, and it is not dependant on the number of element in the
 *  container.
 *
 *  \sa LinearTime
 *  \sa DimFracTime
 *  \sa LogTime
 */

/**
 *  \page DimFracTime Fractional Time Complexity in Dimension \e d
 *  \complex Describe what is Fractional Time Complexity in Dimension \e d.
 *
 *  Functions with fractional time complexity in dimension \f$d\f$ or \Odfrac
 *  will run and return their output in an amount of time that depends primarily
 *  on the number of dimensions used in the container. Proper balancing of the
 *  tree and choice of dimension for the search will also affect the running
 *  time of the function, but it has less influence has the number of elements
 *  in the container increases.
 *
 *  Function of this time complexity will amortize overtime, however they will
 *  not amortize as fast as \ref LogTime "logarithmic time complexity"
 *  function. And therefore, for the benefit of amortization to show, it is
 *  often necessary that for a given dimension \f$d\f$ and number of elements
 *  \f$n\f$ in the tree, the number of elements in the tree be far superior the
 *  exponent of the dimension: \f[n > 2^{d\,+\,1}\f]
 *
 *  This class of time complexity is very common on many of the function of the
 *  containers based on a \kdtree. This is due to the invarient of the \kdtree
 *  that acts on a single dimension at every depth of the \kdtree. Thus, if a
 *  point is being searched for in a particular dimension, and the space has 3
 *  dimensions to work with, then prunning (i.e. eleminating part of tree from
 *  the search) only happens once every 3 steps down. By comparison, in a
 *  regular binary search tree, prunning occurs at every step down.
 *
 *  To give an idea of how prunning will work with such kind of time complexity,
 *  the table below shows the number of node that will be prunned for a various
 *  ranks and for various number of elements, if the tree is perfectly
 *  balanced. The value below are only representative, to give an idea of the
 *  evolution of the running time for a function of such complexity. The number
 *  of nodes visited versus nodes prunned by an algorithm is generally one of
 *  the most reliable indicator of the running time of a function in a tree.
 *
 *  <table>
 *  <tr><th rowspan="2">Number of elements in the container</th>
 *  <th colspan="2">Number of dimension = 1: \Ologn</th>
 *  <th colspan="2">Number of dimension = 2: \Odfrac</th>
 *  <th colspan="2">Number of dimension = 3: \Odfrac</th>
 *  <th colspan="2">Number of dimension = 4: \Odfrac</th></tr>
 *  <tr><th>nodes visited</th><th>node prunned</th>
 *  <th>nodes visited</th><th>node prunned</th>
 *  <th>nodes visited</th><th>node prunned</th>
 *  <th>nodes visited</th><th>node prunned</th></tr>
 *  <tr><td>16</td>
 *  <td>4</td><td>12</td>
 *  <td>8</td><td>8</td>
 *  <td>8</td><td>8</td>
 *  <td>9</td><td>7</td></tr>
 *  <tr><td>64</td>
 *  <td>6</td><td>58</td>
 *  <td>17</td><td>47</td>
 *  <td>27</td><td>37</td>
 *  <td>31</td><td>33</td></tr>
 *  <tr><td>256</td>
 *  <td>8</td><td>248</td>
 *  <td>35</td><td>221</td>
 *  <td>76</td><td>180</td>
 *  <td>103</td><td>153</td></tr>
 *  <tr><td>1024</td>
 *  <td>10</td><td>1014</td>
 *  <td>67</td><td>957</td>
 *  <td>203</td><td>821</td>
 *  <td>322</td><td>702</td></tr>
 *  <tr><td>4096</td><td>12</td>
 *  <td>4084</td><td>140</td>
 *  <td>3956</td><td>525</td>
 *  <td>3571</td><td>970</td>
 *  <td>3126</td></tr>
 *  <tr><td>16384</td><td>14</td>
 *  <td>16370</td><td>281</td>
 *  <td>16103</td><td>1339</td>
 *  <td>15045</td><td>2852</td>
 *  <td>13532</td></tr>
 *  <tr><td>65536</td>
 *  <td>16</td><td>65520</td>
 *  <td>537</td><td>64999</td>
 *  <td>3394</td><td>62142</td>
 *  <td>8265</td><td>57271</td></tr>
 *  <tr><td>262144</td>
 *  <td>18</td><td>262126</td>
 *  <td>1126</td><td>261018</td>
 *  <td>8576</td><td>253568</td>
 *  <td>23729</td><td>238415</td></tr>
 *  <tr><td>1048576</td><td>20</td>
 *  <td>1048556</td>
 *  <td>2252</td><td>1046324</td>
 *  <td>21643</td><td>1026933</td>
 *  <td>67744</td><td>980832</td></tr>
 *  <tr><td>4194304</td>
 *  <td>22</td><td>4194282</td>
 *  <td>4300</td><td>4190004</td>
 *  <td>54579</td><td>4139725</td>
 *  <td>192729</td><td>4001575</td></tr>
 *  <tr><td>16777216</td>
 *  <td>24</td><td>16777192</td>
 *  <td>9011</td><td>16768205</td>
 *  <td>137576</td><td>16639640</td>
 *  <td>547113</td><td>16230103</td></tr>
 *  <tr><td>67108864</td>
 *  <td>26</td><td>67108838</td>
 *  <td>18022</td><td>67090842</td>
 *  <td>346732</td><td>66762132</td>
 *  <td>1551018</td><td>65557846</td></tr>
 *  <tr><td>268435456</td>
 *  <td>28</td><td>268435428</td>
 *  <td>34406</td><td>268401050</td>
 *  <td>873793</td><td>267561663</td>
 *  <td>4393263</td><td>264042193</td></tr>
 *  </table>
 *
 *  As can be seen from the table above, for a single dimension, the number of
 *  node prunned by an algorithm running in logarithmic amortized time becomes
 *  larger than the number of node visited even when just a few nodes are in the
 *  tree. For space of 4 dimensions, we need to insert around 200 points in
 *  order to count more node prunned than nodes visited.
 *
 *  In practices, because tree does not have a very good locality of reference,
 *  iterating through very large trees is further slown down by the number of
 *  cache misses. If your application depends \e heavily on a function that runs
 *  in fractional time complexity in a high number dimensions (i.e. above 10 for
 *  example), and you do not plan to have a very large number of elements in
 *  your tree (i.e. less 10,000 for example), containers in this library may not
 *  be the most optimized solution for you, and you may need to look at other
 *  advanced solutions. In some cases, a \mono{std::vector<>} may even provide
 *  better raw performance.
 *
 *  \sa LogTime
 *  \sa LinearTime
 */
