//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   sf/F32.hh
 * \author Kent Budge
 * \date   Tue Sep 21 12:06:09 2004
 * \brief  Compute Fermi-Dirac function of 1/2 order
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef sf_F32_hh
#define sf_F32_hh

#include "ds++/config.h"

namespace rtt_sf
{
//! Calculate Fermi-Dirac integral of index 3/2.
template<typename OrderedField> DLL_PUBLIC_special_functions
OrderedField F32(OrderedField const &x);

} // end namespace rtt_sf

#endif // sf_F32_hh

//---------------------------------------------------------------------------//
// end of sf/F32.hh
//---------------------------------------------------------------------------//
