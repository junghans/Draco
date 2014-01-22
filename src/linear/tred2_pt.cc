//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   linear/tred2_pt.cc
 * \author Kent Budge
 * \date   Thu Sep  2 14:49:55 2004
 * \brief  Specializations of tred2
 * \note   Copyright (C) 2006-2014 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <vector>
#include <cmath>
#include "tred2.i.hh"

namespace rtt_linear
{

using std::vector;

//---------------------------------------------------------------------------//
// T1=T2=T3=vector<double>
//---------------------------------------------------------------------------//

template DLL_PUBLIC
void tred2(vector<double> &a,
		    unsigned n,
		    vector<double> &d,
		    vector<double> &e);

} // end namespace rtt_linear

//---------------------------------------------------------------------------//
// end of tred2_pt.cc
//---------------------------------------------------------------------------//
