//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_MPI_gatherv_pt.cc
 * \author Thomas M. Evans
 * \date   Mon Mar 25 14:44:54 2002
 * \brief  C4 MPI determinate and indeterminate gatherv instantiations.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <c4/config.h>
#include "C4_Functions.hh"
#include "C4_Req.hh"
#include "gatherv.t.hh"

namespace rtt_c4
{
using std::vector;

//---------------------------------------------------------------------------//
// EXPLICIT INSTANTIATIONS OF NON-BLOCKING SEND/RECEIVE
//---------------------------------------------------------------------------//

template
void indeterminate_gatherv<unsigned>(
    std::vector<unsigned> &outgoing_data,
    std::vector<std::vector<unsigned> > &incoming_data);

template
void determinate_gatherv<unsigned>(
    std::vector<unsigned> &outgoing_data,
    std::vector<std::vector<unsigned> > &incoming_data);

template
void determinate_gatherv<int>(
    std::vector<int> &outgoing_data,
    std::vector<std::vector<int> > &incoming_data);

template
void determinate_gatherv<double>(
    std::vector<double> &outgoing_data,
    std::vector<std::vector<double> > &incoming_data);

} // end namespace rtt_c4


//---------------------------------------------------------------------------//
//                              end of C4_MPI_gatherv_pt.cc
//---------------------------------------------------------------------------//
