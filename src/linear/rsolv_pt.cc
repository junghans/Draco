//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   linear/rsolv.cc
 * \author Kent Budge
 * \date   Tue Aug 10 13:01:02 2004
 * \brief  Specializations of rsolv
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <vector>
#include "rsolv.i.hh"

namespace rtt_linear
{
using std::vector;

//---------------------------------------------------------------------------//
// T=vector<double> const &
//---------------------------------------------------------------------------//

template void rsolv(const vector<double> &R,
 		    const unsigned n,
 		    vector<double> &b);

} // end namespace rtt_linear

//---------------------------------------------------------------------------//
//                 end of rsolv.cc
//---------------------------------------------------------------------------//
