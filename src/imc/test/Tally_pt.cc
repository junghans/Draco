//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   imc/test/Tally_pt.cc
 * \author Thomas M. Evans
 * \date   Fri Aug 18 16:20:15 2000
 * \brief  Tally class instantiations.
 * \note   Copyright � 2003 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "mc/OS_Mesh.hh"
#include "mc/RZWedge_Mesh.hh"
#include "../Tally.t.hh"
#include "../Tally_Builder.t.hh"

namespace rtt_imc
{

using rtt_mc::OS_Mesh;
using rtt_mc::RZWedge_Mesh;

template class Tally<OS_Mesh>;
template class Tally<RZWedge_Mesh>;

template class Tally_Builder<OS_Mesh>;
template class Tally_Builder<RZWedge_Mesh>;

} // end namespace rtt_imc


//---------------------------------------------------------------------------//
//                              end of Tally_pt.cc
//---------------------------------------------------------------------------//
