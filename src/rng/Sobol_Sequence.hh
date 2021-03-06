//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   rng/Sobol_Sequence.hh
 * \author Kent Budge
 * \brief  Definition of class Sobol_Sequence
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef rng_Sobol_Sequence_hh
#define rng_Sobol_Sequence_hh

#include <vector>
#include "gsl/gsl_qrng.h"
#include "ds++/config.h"

namespace rtt_rng
{

//===========================================================================//
/*!
 * \class Sobol_Sequence
 * \brief Generator for a sequence of subrandom (pseudorandom) numbers.
 *
 * The Sobol' sequence is a subrandom sequence. This means two things. First,
 * when it is used to generate quadrature points for integrating a function,
 * the error for large N goes as 1/N rather than the 1/sqrt(N) of a true Monte
 * Carlo integration. Second, the convergence is smooth, unlike the
 * convergence for the Halton sequence, which for highly peaked functions
 * shows severe oscillations as the sequence marches through the peaked
 * region. The down side is that the Sobol' sequence is difficult to compute
 * and to restart for arbitrarily large random vectors.
 */
//===========================================================================//

class DLL_PUBLIC_rng Sobol_Sequence
{
  public:

    // CREATORS

    //! Default constructor.
    explicit Sobol_Sequence(unsigned const dimension=1);

    //! Destructor
    ~Sobol_Sequence();

    // MANIPULATORS

    //! Advance sequence.
    void shift();

    // ACCESSORS

    //! Examine elements of sequence.
    std::vector<double> const &values() const { return values_; }

    bool check_class_invariants() const;

  private:

    // NESTED CLASSES AND TYPEDEFS

    // IMPLEMENTATION

    //! Not implemented
    Sobol_Sequence(Sobol_Sequence const &);

    //! Not implemented
    Sobol_Sequence &operator=(Sobol_Sequence const&);

    // DATA

    gsl_qrng *gsl_;

    std::vector<double> values_;  // Values of the current element of sequence.
};

} // end namespace rtt_rng

#endif // rng_Sobol_Sequence_hh

//---------------------------------------------------------------------------//
// end of rng/Sobol_Sequence.hh
//---------------------------------------------------------------------------//
