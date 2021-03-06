/*----------------------------------*-C++-*----------------------------------//
 * \file   field_ts_advisor_pt.cc
 * \author John McGhee
 * \date   Fri May  1 09:51:28 1998
 * \brief  Explicit template instantiation for the time-step manager test
 *         facility. 
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <vector>
#include "field_ts_advisor.t.hh"

namespace rtt_timestep
{

//---------------------------------------------------------------------------//
// Explicit instatiation for FT == vector<double>.
//---------------------------------------------------------------------------//

template DLL_PUBLIC_timestep
void field_ts_advisor::set_floor( std::vector< double > const & y1, 
                                  double                        frac ); 

template DLL_PUBLIC_timestep
void field_ts_advisor::update_tstep( ts_manager            const & tsm,
                                     std::vector< double > const & y1, 
                                     std::vector< double > const & y2 );

} // end namespace rtt_timestep

//---------------------------------------------------------------------------//
// end of field_ts_advisor_pt.cc
//---------------------------------------------------------------------------//
