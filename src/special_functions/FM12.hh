//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   sf/FM12.hh
 * \author Kent Budge
 * \date   Tue Sep 21 12:06:09 2004
 * \brief  Compute Fermi-Dirac function of 1/2 order
 * \note   Copyright (C) 2004-2013 Los Alamos National Security, LLC.
 *         All rights reserved.
 *
 * Long description.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef sf_FM12_hh
#define sf_FM12_hh

#include "ds++/config.h"

namespace rtt_sf
{
//! Calculate Fermi-Dirac integral of index -1/2.
template<typename OrderedField> DLL_PUBLIC
OrderedField FM12(OrderedField const &x);

} // end namespace rtt_sf

#endif // sf_FM12_hh

//---------------------------------------------------------------------------//
// end of sf/FM12.hh
//---------------------------------------------------------------------------//
