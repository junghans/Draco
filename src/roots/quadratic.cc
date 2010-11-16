//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   roots/quadratic.cc
 * \author Kent Budge
 * \date   Wed Aug 11 15:21:38 2004
 * \brief  Specializations of quadratic
 * \note   Copyright 2004 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "quadratic.hh"

namespace rtt_roots
{
using namespace std;

template void quadratic(double const &a,
			double const &b,
			double const &c,
			double &r1,
			double &r2);

} // end namespace rtt_roots

//---------------------------------------------------------------------------//
//                 end of quadratic.cc
//---------------------------------------------------------------------------//