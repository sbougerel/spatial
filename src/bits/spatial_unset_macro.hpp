// -*- C++ -*-

/**
 *  @file   spatial_unset_macro.hpp
 *  @brief  
 *
 *  Change log:
 *
 *  - 20-03-2010 Sylvain Bougerel <sylvain.bougerel.devel@gmail.com>
 *    Creation of the file.
 *
 */

#ifndef SPATIAL_UNSET_MACRO_HPP
#define SPATIAL_UNSET_MACRO_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#ifdef SPATIAL_KEY
#  undef SPATIAL_KEY
#endif

#ifdef SPATIAL_KEY_CONST
#  undef SPATIAL_KEY_CONST
#endif

#endif // SPATIAL_UNSET_MACRO_HPP
