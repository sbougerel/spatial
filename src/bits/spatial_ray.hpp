// -*- C++ -*-

/**
 *  @file   spatial_ray.hpp
 *  @brief  Defines a ray as a segment with a begin and an end and find all
 *  boxes intersecting the ray in the order where they appear.
 *
 *  The idea is to find all the boxes intersecting with the beginning of the
 *  ray, then casting the ray to the closest splitting plane, and thus finding a
 *  new point, checking if any boxes intersect that point, then casting the ray
 *  to the next splitting plane and iterating with the process until the end of
 *  the ray is closer than the next splitting plane or until there are no
 *  further splitting planes.
 *
 *  Change log:
 *
 *  - 20-11-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_RAY_HPP
#define SPATIAL_RAY_HPP



#endif // SPATIAL_RAY_HPP
