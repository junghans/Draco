//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   sf/Factorial.hh
 * \author Kelly Thompson
 * \date   Mon Nov 8 11:17:12 2004
 * \brief  Provide declaration of templatized factorial function.
 * \note   Copyright (C) 2004-2014 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef sf_factorial_hh
#define sf_factorial_hh

#include "ds++/config.h"

namespace rtt_sf
{

//! \brief factorial
template< typename T > DLL_PUBLIC
T factorial( T const k );

//! \brief fraction of factorials, \f$ (k!)/(l!) \f$
template< typename T > DLL_PUBLIC
double factorial_fraction( T const k, T const l );

} // end namespace rtt_sf

#endif // sf_Factorial_hh

//---------------------------------------------------------------------------//
// end of sf/factorial.hh
//---------------------------------------------------------------------------//
