//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   roots/zbrac.hh
 * \author Kent Budge
 * \date   Tue Aug 17 15:30:23 2004
 * \brief  Bracket a root of a function.
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef roots_zbrac_hh
#define roots_zbrac_hh

namespace rtt_roots
{

//---------------------------------------------------------------------------//
// Use explicit instantiation of these template functions.
//---------------------------------------------------------------------------//

//! Bracket a root of a function.
template <class Function, class Real>
void zbrac(Function func, Real &x1, Real &x2);

} // end namespace rtt_roots

// Use implicit instantiation
#include "zbrac.i.hh"

#endif // roots_zbrac_hh

//---------------------------------------------------------------------------//
//              end of roots/zbrac.hh
//---------------------------------------------------------------------------//