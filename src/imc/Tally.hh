//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   imc/Tally.hh
 * \author Todd J. Urbatsch
 * \date   Mon Apr  6 14:38:03 1998
 * \brief  IMC Tally class header file.
 * \note   Copyright © 2003 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef rtt_imc_Tally_hh
#define rtt_imc_Tally_hh

#include <iostream>
#include <vector>
#include "ds++/SP.hh"
#include "Random_Walk_Sub_Tally.hh"
#include "Surface_Sub_Tally.hh"

namespace rtt_imc 
{

//===========================================================================//
/*!
 *
 * \class Tally
 *
 * \brief Tally class for Fleck and Cummings Implicit Monte Carlo transport.
 *
 * The Tally class tallies pertinent data that is generated by particle
 * transport in an IMC application.  Three principle quantities are tallied,
 * energy deposition, momentum deposition, and energy-weighted path length.
 * These quantities are tallied using the deposit_energy,
 * accumulate_momentum, and accumulate_ewpl functions.
 *
 * Additionally, as required by Fleck and Cummings IMC transport, census
 * information is tallied using the acumulate_cen_info function.  Finally,
 * the Tally class provides accumulate (prefixed by accum_) functions for a
 * host of edit data about the particle transport history.
 *
 * \subsection sub_tally Sub Tally Concept
 *
 * As we add new features to the IMC package, we require new tally
 * quantities.  These are added through a \e sub \e tally concept.  Sub tally
 * objects (e.g. rtt_imc::Random_Walk_Sub_Tally) can be requested from the
 * tally object.  These requests return rtt_dsxx::SP objects; thus, they can
 * be checked for existence of a given sub-tally.  The sub tally objects
 * themselves provide the appropriate interfaces for the types of data that
 * they tally.  The existing sub tally objects are:
 * - rtt_imc::Random_Walk_Sub_Tally
 * - rtt_imc::Surface_Sub_Tally (a vector of these equal to the number of
 * groups) 
 * .
 * Sub tally objects are assigned to the tally class.  It is acceptable to
 * assign null sub-tally objects.  They are accessed through get functions.
 *
 * Nothing about the Tally class is data parallel.  The Tally is designed to
 * be created on each processor that particle transport occurs.  The tallies
 * must, at the end of a timestep, be concatenated in a reasonable way.  The
 * Tally class knows nothing about this process.  It only knows how to tally
 * data on a local processor.
 *
 */
/*!
 * \example imc/test/tstTally.cc
 *
 * Example usage of the rtt_imc::Tally class.
 *
 */
// revision history:
// -----------------
// 0) original

// 1) 23-JUN-98 : Added accumulation of new census information and 
//                energy-weighted path length. 
// 2) 28-JUL-98 : Added accumulation of thomson scatters
// 3) 31-AUG-98 : Added cell volume accessor function 
// 4) 31-AUG-98 : Added additional constructor for evol_net storage
// 5) 19-APR-00 : Added momentum deposition, accumulators, accessors.
// 6) 14-DEC-00 : Added accessors and modified accumulators to be defaulted 
//                to one particle but allow multiple particles.  Enables
//                summing thread-private tallies to a master thread
//                tally. Transparently retains current usage.
// 7) 17-JAN-01 : fixed accumulate_momentum function so that it adds up
//                momentum only in problem spatial dimensions, not based on
//                the dimension of the direction vector (which is always 3-D) 
// 8) 21-MAY-03 : added random walk number accumulation to tally
// 9) 24-JUN-03 : updated inline functions; added more checks; added 
//                sub tally calss aggregation
//
//===========================================================================//

template<class MT>
class Tally 
{
  public:
    // Useful typedefs
    typedef rtt_dsxx::SP<MT>                    SP_MT;
    typedef rtt_dsxx::SP<Random_Walk_Sub_Tally> SP_RW_ST;
    typedef rtt_dsxx::SP<Surface_Sub_Tally>     SP_S_ST;
    typedef std::vector<double>                 sf_double;
    typedef std::vector<sf_double>              vf_double;

  private:
    // >>> IMPLEMENTATION

    //! Check that tallied data is positive.
    template<class T> bool pos(const T d) const { return d >= 0.0; }
    
  private:
    // >>> DATA

    // Energy deposition field and totals.
    typename MT::CCSF_double energy_dep;
    double                   energy_dep_tot;

    // Momentum deposition field.
    typename MT::CCVF_double momentum_dep;

    // Energy-weighted path length field.
    typename MT::CCSF_double eweighted_pathlen;

    // Census energy field and totals.
    typename MT::CCSF_double census_energy;
    double                   new_ecen_tot;

    // Census number field and totals.
    typename MT::CCSF_int new_ncen;
    int                   new_ncen_tot;

    // Particle activity tallies per cycle.
    int    n_effscat;
    int    n_thomscat;
    int    n_killed;
    double ew_killed;
    int    n_escaped;
    double ew_escaped;
    int    n_bndcross;
    int    n_reflections;

    // >>> SUB TALLY OBJECTS
    SP_RW_ST              rw_sub_tally;
    std::vector<SP_S_ST>  surface_sub_tallies;

  public:
    // Tally constructor.
    explicit Tally(rtt_dsxx::SP<MT>);

    // >>> SUB TALLY CREATORS

    //! Assign a Random_Walk_Sub_Tally.
    void assign_RW_Sub_Tally(SP_RW_ST tally) { rw_sub_tally = tally; }

    // Assign a vector of Surface_Sub_Tally
    inline void assign_Surface_Sub_Tally(const std::vector<SP_S_ST>& tallies);

    // Assign a Surface_Sub_Tally
    inline void assign_Surface_Sub_Tally(SP_S_ST tally);

    // >>> SUB TALLY ACCESSORS
    
    //! Get a Random_Walk_Sub_Tally.
    SP_RW_ST get_RW_Sub_Tally() const { return rw_sub_tally; }
    
    //! Get a Surface_Sub_Tally
    SP_S_ST get_Surface_Sub_Tally(int group = 1) const;

    // >>> TALLY ACCUMULATION FUNCTIONS

    // Deposit energy.
    void deposit_energy(const int cell, const double energy);

    // Accumulate momentum deposition.
    void accumulate_momentum(const int, const double, const sf_double &); 

    // Accumulate energy-weighted path length.
    void accumulate_ewpl(const int cell, const double ewpl);

    // Accumulate new census energy and numbers of particles.
    void accumulate_cen_info(const int, const double, const int = 1);

    //! Accumulate the number of effective scatters.
    void accum_n_effscat(const int n = 1) { Check(pos(n)); n_effscat += n; }
    
    //! Accumulate the number of particle (isotropic) scatters.
    void accum_n_thomscat(const int n = 1) {Check(pos(n));  n_thomscat += n; }

    //! Accumulate the number of particles that are killed.
    void accum_n_killed(const int n = 1) { Check(pos(n)); n_killed += n; }

    //! Accumulate the ammount of energy from killed particles.
    void accum_ew_killed(const double ew) { Check(pos(ew)); ew_killed += ew; }

    //! Accumulate the number of particles that escape.
    void accum_n_escaped(const int n = 1) { Check(pos(n)); n_escaped += n; }

    //! Accumulate the amount of energy that escapes.
    void accum_ew_escaped(const double ew) { Check(pos(ew)); ew_escaped += ew; }
    
    //! Accumulate the number of cell boundary crossings.
    void accum_n_bndcross(const int n = 1) { Check(pos(n)); n_bndcross += n; }

    // Accumulate the number of reflections made by particles.
    inline void accum_n_reflections(const int n = 1); 

    //>>> ACCESSORS

    //! Get energy deposition in a cell.
    double get_energy_dep(const int cell) const { return energy_dep(cell); }

    //! Get total energy deposition.
    double get_energy_dep_tot() const { return energy_dep_tot; }

    //! Get momentum deposition vector in a cell.
    sf_double get_momentum_dep(const int c) const { return momentum_dep(c); } 

    //! Get energy-weighted path length in a cell.
    double get_accum_ewpl(const int c) const { return eweighted_pathlen(c); } 

    //! Get new census energy in a cell.
    double get_new_ecen(const int cell) const { return census_energy(cell); } 

    //! Get new, total census energy.
    double get_new_ecen_tot() const { return new_ecen_tot; }

    //! Get new number of census particles in a cell.
    int get_new_ncen(const int cell) const { return new_ncen(cell); }

    //! Get new, total number of census particles. 
    int get_new_ncen_tot() const { return new_ncen_tot; }

    //! Get the number of effective scatters.
    int get_accum_n_effscat() const { return n_effscat; }

    //! Get the number of particle (isotropic) scatters.
    int get_accum_n_thomscat() const { return n_thomscat; }

    //! Get the number of particles that are killed.
    int get_accum_n_killed() const { return n_killed; }

    //! Get the ammount of energy from killed particles.
    double get_accum_ew_killed() const { return ew_killed; }

    //! Get the amount of energy that escapes.
    double get_ew_escaped() const { return ew_escaped; }

    //! Get the number of particles that escape.
    int get_accum_n_escaped() const { return n_escaped; }
    
    //! Get the number of cell boundary crossings.
    int get_accum_n_bndcross() const { return n_bndcross; }

    //! Get the number of reflections made by particles.
    int get_accum_n_reflections() const { return n_reflections; }

    //! Get the number of cells represented by this tally (on-processor).
    int num_cells() const { return energy_dep.get_Mesh().num_cells(); }

    // Get the cell volume.
    inline double volume(int) const;

    // Tally diagnostics.
    void print(std::ostream &) const;
    void cycle_print(std::ostream &) const;
};

//---------------------------------------------------------------------------//
// OVERLOADED OPERATORS
//---------------------------------------------------------------------------//

template<class MT>
std::ostream& operator<<(std::ostream &out, const Tally<MT> &object)
{
    object.print(out);
    return out;
}

//---------------------------------------------------------------------------//
// INLINE MEMBER FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * \brief Get the cell volume.
 */
template<class MT>
double Tally<MT>::volume(int cell) const
{
    Require (cell > 0);
    Require (cell <= energy_dep.get_Mesh().num_cells());
    return energy_dep.get_Mesh().volume(cell);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Accumulate the number of reflections made by particles.
 */
template<class MT>
void Tally<MT>::accum_n_reflections(const int n)
{
    Check (pos(n));
    n_reflections += n;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Get the surface sub-tally for the designated group, or the only one
 * for gray.
 * 
 * \param group The group index
 * \return Smart pointer to the surface sub-tally
 */
template<class MT>
typename Tally<MT>::SP_S_ST Tally<MT>::get_Surface_Sub_Tally(int group) const 
{
    Check ( group >= 1);

    if (surface_sub_tallies.size() > 0)
    {
	Check ( group <= surface_sub_tallies.size() );
	return surface_sub_tallies[group-1]; 
    }

    return SP_S_ST(0);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Assign a vector of Surface_Sub_Tally's.
 *
 * The given tally(s) may be null.
 */
template<class MT>
void Tally<MT>::assign_Surface_Sub_Tally(const std::vector<SP_S_ST>& tallies) 
{ 
    surface_sub_tallies = tallies; 
}

//---------------------------------------------------------------------------//
/*!
 * \brief Assign a Surface_Sub_Tally.
 *
 * The given tally may be null.
 */
template<class MT>
void Tally<MT>::assign_Surface_Sub_Tally(SP_S_ST tally) 
{
    surface_sub_tallies.resize(1, tally); 
} 

} // end of rtt_imc

#endif                          // rtt_imc_Tally_hh

//---------------------------------------------------------------------------//
//                              end of imc/Tally.hh
//---------------------------------------------------------------------------//

