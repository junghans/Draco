//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   imc/Tally.hh
 * \author Todd J. Urbatsch
 * \date   Mon Apr  6 14:38:03 1998
 * \brief  IMC Tally class header file.
 */
//---------------------------------------------------------------------------//
// @> Tally class header file
//---------------------------------------------------------------------------//

#ifndef __imc_Tally_hh__
#define __imc_Tally_hh__

//===========================================================================//
// class Tally - 
//
// Purpose : Accumulate Particle tallies.
//
// revision history:
// -----------------
// 0) original
// 1) 06-23-98  Added accumulation of new census information and 
//              energy-weighted path length. 
// 2) 07-28-98  Added accumulation of thomson scatters
// 3) 08-31-98  Added cell volume accessor function 
// 4) 08-31-98  Added additional constructor for evol_net storage
// 5) 04-19-00  Added momentum deposition, accumulators, accessors.
// 
//===========================================================================//

#include "ds++/SP.hh"
#include <iostream>
#include <vector>

namespace rtt_imc 
{

template<class MT>
class Tally 
{
  private:
    typename MT::CCSF_double energy_dep;
    double energy_dep_tot;

    typename MT::CCVF_double momentum_dep;

    typename MT::CCSF_double eweighted_pathlen;
    typename MT::CCSF_double census_energy;
    double new_ecen_tot;

    typename MT::CCSF_int new_ncen;
    int new_ncen_tot;

    typename MT::CCSF_double evol_net;

    // particle activity tallies, per cycle
    int n_effscat;
    int n_thomscat;
    int n_killed;
    double ew_killed;
    int n_escaped;
    double ew_escaped;
    int n_bndcross;
    int n_reflections;

  public:
    // Tally constructor.
    explicit Tally(rtt_dsxx::SP<MT>);

    // Tally constructor that gets evol_net.
    Tally(rtt_dsxx::SP<MT>, typename MT::CCSF_double);

    // accumulate energy deposited
    void deposit_energy(const int cell, const double energy);

    // access energy deposited for a cell, total.
    inline double get_energy_dep(const int cell) const;
    inline double get_energy_dep_tot() const;
    double get_evol_net(int cell) const { return evol_net(cell); }

    // accumulate momentum deposition
    void accumulate_momentum(const int cell, const double energy_wt, 
			     const std::vector<double> omega);

    // access momentum deposition
    inline std::vector<double> get_momentum_dep(const int cell) const;

    // accumulate energy-weighted path length
    void accumulate_ewpl(const int cell, const double ewpl);

    // accumulate new census energy and numbers of particles
    void accumulate_cen_info(const int cell, const double new_ecen);

    // access accumulated energy-weighted path length per cell
    inline double get_accum_ewpl(const int cell) const;

    // access accumulated new census energy per cell, total
    inline double get_new_ecen(const int cell) const;
    inline double get_new_ecen_tot() const;

    // access accumulated numbers of new census particles per cell, total
    inline int get_new_ncen(const int cell) const;
    inline int get_new_ncen_tot() const;

    // accumulator functions for particle activity
    inline void accum_n_effscat();
    inline void accum_n_thomscat();
    inline void accum_n_killed();
    inline void accum_ew_killed(const double ew);
    inline void accum_n_escaped();
    inline void accum_ew_escaped(const double ew);
    inline void accum_n_bndcross();
    inline void accum_n_reflections();

    // access accumulated particle activity tallies
    double get_ew_escaped() const { return ew_escaped; }

    // accessors
    int num_cells() const { return energy_dep.get_Mesh().num_cells(); }
    inline double volume(int) const;

    // diagnostics for tally
    void print(std::ostream &) const;
    void cycle_print(std::ostream &) const;
};

//---------------------------------------------------------------------------//
// overloaded operators
//---------------------------------------------------------------------------//

template<class MT>
std::ostream& operator<<(std::ostream &out, const Tally<MT> &object)
{
    object.print(out);
    return out;
}

//---------------------------------------------------------------------------//
// inline member functions for Tally
//---------------------------------------------------------------------------//

template<class MT>
inline double Tally<MT>::get_energy_dep(const int cell) const
{
    return energy_dep(cell);
}

//---------------------------------------------------------------------------//

template<class MT>
inline double Tally<MT>::get_energy_dep_tot() const
{ 
    return energy_dep_tot; 
}

//---------------------------------------------------------------------------//

template<class MT>
inline std::vector<double> Tally<MT>::get_momentum_dep(const int cell) const
{
    return momentum_dep(cell); 
}

//---------------------------------------------------------------------------//

template<class MT>
inline double Tally<MT>::get_accum_ewpl(const int cell) const
{
    return eweighted_pathlen(cell);
}

//---------------------------------------------------------------------------//

template<class MT>
inline double Tally<MT>::get_new_ecen(const int cell) const
{
    return census_energy(cell);
}

//---------------------------------------------------------------------------//

template<class MT>
inline double Tally<MT>::get_new_ecen_tot() const
{
    return new_ecen_tot; 
}

//---------------------------------------------------------------------------//

template<class MT>
inline int Tally<MT>::get_new_ncen(const int cell) const
{
    return new_ncen(cell);
}

//---------------------------------------------------------------------------//

template<class MT>
inline int Tally<MT>::get_new_ncen_tot() const
{ 
    return new_ncen_tot; 
}

//---------------------------------------------------------------------------//
// inline accumulator function for particle activity

template<class MT>
inline void Tally<MT>::accum_n_effscat(){ n_effscat += 1; }

template<class MT>
inline void Tally<MT>::accum_n_thomscat(){ n_thomscat += 1; }

template<class MT>
inline void Tally<MT>::accum_n_killed(){ n_killed += 1; }

template<class MT>
inline void Tally<MT>::accum_ew_killed(const double ew){ ew_killed += ew; }

template<class MT>
inline void Tally<MT>::accum_n_escaped(){ n_escaped += 1; }

template<class MT>
inline void Tally<MT>::accum_ew_escaped(const double ew){ ew_escaped += ew; }

template<class MT>
inline void Tally<MT>::accum_n_bndcross(){ n_bndcross += 1; }

template<class MT>
inline void Tally<MT>::accum_n_reflections(){ n_reflections += 1; }

//---------------------------------------------------------------------------//
// return the cell volume

template<class MT>
inline double Tally<MT>::volume(int cell) const
{
    Require (cell <= energy_dep.get_Mesh().num_cells());
    return energy_dep.get_Mesh().volume(cell);
}

} // end namespace rtt_imc

#endif                          // __imc_Tally_hh__

//---------------------------------------------------------------------------//
//                              end of imc/Tally.hh
//---------------------------------------------------------------------------//

