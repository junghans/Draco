//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   dummy_package.hh
 * \author John McGhee
 * \date   Thu Aug 27 07:48:41 1998
 * \brief  A dummy package to exercize the field time-step advisors.
 * \note   Copyright (C) 1998-2010 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __timestep_test_dummy_package_hh__
#define __timestep_test_dummy_package_hh__

#include <vector>
#include "ds++/SP.hh"

// FORWARD REFERENCES

namespace rtt_timestep 
{
class ts_manager;
class field_ts_advisor;
}

namespace rtt_timestep_test
{

//===========================================================================//
/*! \class dummy_package - Exercizes the field time-step advisors.
 *
 * This class serves as an example of how any particular package can make use
 * of the time-step manaager/advisor utility. It also exercizes the field
 * time-step advisors.
 */
//===========================================================================//

class dummy_package 
{
  public:

    // CREATORS

    dummy_package( rtt_timestep::ts_manager & tsm_ );
    ~dummy_package();

    // MANIPULATORS
    
    //! \brief Method to advance the time-step
    void advance_state();

    //! \brief Provide recommended dt values (TE).
    double get_dt_rec_te() const;

    //! \brief Provide recommended dt values (TI).
    double get_dt_rec_ti() const;

    //! \brief Provide recommended dt values (RI).
    double get_dt_rec_ri() const;
    
  private:

    std::vector< double > element_wise_multiply( 
	double const a,
	std::vector< double > const & v );

    // DATA

    rtt_timestep::ts_manager & tsm;
    rtt_dsxx::SP< rtt_timestep::field_ts_advisor > sp_te;
    rtt_dsxx::SP< rtt_timestep::field_ts_advisor > sp_ti;
    rtt_dsxx::SP< rtt_timestep::field_ts_advisor > sp_ri; 
};

} // end namespace rtt_timestep_test

#endif // __timestep_test_dummy_package_hh__

//---------------------------------------------------------------------------//
//                        end of dummy_package.hh
//---------------------------------------------------------------------------//
