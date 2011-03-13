// -*- C++ -*-

/**
 *  @file   spatial_set_macro.hpp
 *  @brief  
 *
 *  Change log:
 *
 *  - 20-03-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_SET_MACRO_HPP
#define SPATIAL_SET_MACRO_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#ifndef SPATIAL_KEY
#  define SPATIAL_KEY(node) static_cast<Link_type>((node))->key_field
#endif

#ifndef SPATIAL_KEY_CONST
#  define SPATIAL_KEY_CONST(node) static_cast<Const_Link_type>((node))->key_field
#endif

#endif // SPATIAL_SET_MACRO_HPP
