//----------------------------------*-C++-*----------------------------------//
/*! 
 * \file timestep/fixed_ts_advisor.hh
 * \author <a href="http://www.lanl.gov/home/mcghee">John McGhee</a>
 * \date Thu Apr  2 14:06:18 1998
 * \brief Defines the fixed time-step advisor.
 */
//---------------------------------------------------------------------------//

#ifndef __timestep_fixed_ts_advisor_hh__
#define __timestep_fixed_ts_advisor_hh__

#include "ts_advisor.hh"

namespace rtt_timestep {

//===========================================================================//
/*!
 * \brief Introduces a user defined fixed value 
 *        into the time-step calculation.
 *
 * \sa The ts_manager class provides a means to manage this advisor.
 * The \ref timestep_overview page gives a summary of the Draco time 
 * step control utilities. 
 *
 * This is useful to set min and max timesteps, or to force a
 * timestep, etc. The recommendation for the new timestep is
 * simply the user input value. 
 */ 
//===========================================================================//
class fixed_ts_advisor : public ts_advisor {

  // DATA

  private:
    /*!
     * \brief Value used to oompute a fixed advisor recommended timestep.
     */
    double fixed_value; 
   
// CREATORS

  public:

    /*!
     * \brief Constructs a fixed time-step advisor.
     * \param name_ A unique name for the advisor.
     * \param usage_ How the advisor is to be used.
     * \param const_value_ The desired value for the timestep.
     * \param active_ Turns the advisor on/off.
     */
    fixed_ts_advisor( 
	const std::string &name_  = std::string("Unlabeled"),
	const usage_flag usage_ = max, 
	const double const_value_ = large(),
	const bool active_ = true);

    /*!
     * \brief Destroys a fixed time-step advisor.
     */
    ~fixed_ts_advisor();

// MANIPULATORS
    
    /*!
     * \brief Sets the fixed value.
     * \param value_ The fixed value.
     */
    void set_fixed_value(const double value_ = large())
    { 
	fixed_value = value_;
    }

// ACCESSORS

    /*!
     * \brief Returns the time-step recommended by a fixed  advisor.
     * \param tsm The time step manager in which the advisor resides.
     * \return The recommended timestep.
     */
    double get_dt_rec(const ts_manager &tsm) const;
    
    /*! 
      \brief Prints the state of a fixed advisor.
      \return Prints the internal state of the advisor to std out.
     */
    void print_state() const;
    
    /*! 
      \brief Fixed advisor invariant function.
      \return True if the invariant is satisfied.
     */
    bool invariant_satisfied() const;

};

} // end of rtt_timestep namespace

#endif                          // __timestep_fixed_ts_advisor_hh__

//---------------------------------------------------------------------------//
//                              end of fixed_ts_advisor.hh
//---------------------------------------------------------------------------//
