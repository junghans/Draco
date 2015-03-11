//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   sf/F1.hh
 * \author Kent Budge
 * \brief  Fermi-Dirac integral of index 1.
 * \note   Copyright (C) 2004-2015 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef sf_F1_hh
#define sf_F1_hh

#include "ds++/config.h"

namespace rtt_sf
{
//! Compute the Fermi-Dirac function of index 1.
DLL_PUBLIC_special_functions double F1(double eta);

} // end namespace rtt_sf

#endif // sf_F1_hh

//---------------------------------------------------------------------------//
// end of sf/F1.hh
//---------------------------------------------------------------------------//
