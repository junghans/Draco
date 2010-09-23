//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ratio_ts_advisor.cc
 * \author John McGhee
 * \date   Thu Apr  2 14:06:18 1998
 * \brief  Defines the ratio time-step advisor.
 * \note   Copyright (C) 1998-2010 Los Alamos National Security, LLC.
 *         All rights reserved.
 * \version $Id$
 */
//---------------------------------------------------------------------------//

#include "ratio_ts_advisor.hh"
#include "ts_manager.hh"
#include "ds++/Assert.hh"
#include "ds++/config.h"
#include "c4/global.hh"
#include <iostream>

using std::cout;
using std::endl;

namespace rtt_timestep
{

ratio_ts_advisor::ratio_ts_advisor(const std::string &name_,
				   const usage_flag usage_,
				   const double ratio_value_,
				   const bool active_) 

    : ts_advisor(name_, usage_, active_), ratio_value(ratio_value_)

{
    Ensure(invariant_satisfied());
}


ratio_ts_advisor::~ratio_ts_advisor()
{
// empty
}

double ratio_ts_advisor::get_dt_rec(const ts_manager &tsm) const
{
    Require(invariant_satisfied());
    Require(tsm.get_dt() > 0.);
    return ratio_value*tsm.get_dt();
}

void ratio_ts_advisor::print_state() const
{
    if (C4::node() != 0)
	return;
    
    std::string status = is_active() ? "true " : "false";
    cout << endl;
    cout << "  ** Time-Step Advisor State Listing **" << endl;
    cout << "  Name - " << get_name() << endl;
    cout << "  Type           : " << "Ratio Advisor" << endl;
    cout << "  Active         : " << status << endl;
    cout << "  Usage          : " << usage_flag_name(get_usage()) << endl;
    cout << "  Ratio Value    : " << ratio_value << endl;
    cout << endl;
}

bool ratio_ts_advisor::invariant_satisfied() const
{
    return ts_advisor::invariant_satisfied() &&
        0. < ratio_value;
}

} //end of rtt_timestep namespace

//---------------------------------------------------------------------------//
//                              end of ratio_ts_advisor.cc
//---------------------------------------------------------------------------//
