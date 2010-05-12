//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_MPI_swap_pt.cc
 * \author Thomas M. Evans
 * \date   Mon Mar 25 14:44:54 2002
 * \brief  C4 MPI determinate and indeterminate swap instantiations.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <c4/config.h>
#include "C4_Functions.hh"
#include "C4_Req.hh"
#include "swap.t.hh"

namespace rtt_c4
{
using std::vector;

//---------------------------------------------------------------------------//
// EXPLICIT INSTANTIATIONS OF NON-BLOCKING SEND/RECEIVE
//---------------------------------------------------------------------------//

template void determinate_swap(vector<unsigned>          const &outgoing_pid,
                               vector<vector<unsigned> > const &outgoing_data,
                               vector<unsigned>          const &incoming_pid,
                               vector<vector<unsigned> >       &incoming_data,
                               int tag);

template void determinate_swap(vector<vector<unsigned> > const &outgoing_data,
                               vector<vector<unsigned> >       &incoming_data,
                               int tag);
} // end namespace rtt_c4


//---------------------------------------------------------------------------//
//                              end of C4_MPI_swap_pt.cc
//---------------------------------------------------------------------------//