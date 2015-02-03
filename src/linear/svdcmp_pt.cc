//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   linear/svdcmp.cc
 * \author Kent Budge
 * \date   Wed Aug 11 15:21:38 2004
 * \brief  Specializations of svdcmp
 * \note   Copyright (C) 2006-2015 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <vector>
#include "svdcmp.i.hh"

namespace rtt_linear
{
using std::vector;

template DLL_PUBLIC
void svdcmp(vector<double> &a,
		     const unsigned m,
		     const unsigned n,
		     vector<double> &w,
		     vector<double> &v);

} // end namespace rtt_linear

//---------------------------------------------------------------------------//
// end of svdcmp.cc
//---------------------------------------------------------------------------//
