//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   rng/LC_Subrandom_Generator.cc
 * \author Kent Budge
 * \brief  Define methods of class LC_Subrandom_Generator
 * \note   � Copyright 2006-2007 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "ds++/Assert.hh"
#include "LC_Subrandom_Generator.hh"

namespace rtt_rng
{

//---------------------------------------------------------------------------//
/*!
 * \param count Dimension of the vector of random values returned by this
 * object.
 */
LC_Subrandom_Generator::LC_Subrandom_Generator(unsigned const count)
    :
    Subrandom_Generator(count_)
{
    generator_ = gsl_rng_alloc(gsl_rng_mt19937);

    Ensure(check_class_invariants());
}

//---------------------------------------------------------------------------//
LC_Subrandom_Generator::~LC_Subrandom_Generator()
{
    gsl_rng_free(generator_);
}

//---------------------------------------------------------------------------//
void LC_Subrandom_Generator::shift_vector()
{
    ++count_;
    // a no-op
    element_ = 0;

    Ensure(check_class_invariants());
}

//---------------------------------------------------------------------------//
double LC_Subrandom_Generator::shift()
{
    double const Result = gsl_rng_uniform(generator_);
    ++element_;

    Ensure(check_class_invariants());
    Ensure(Result>=0.0 && Result<=1.0);
    return Result;
}

//---------------------------------------------------------------------------//
bool LC_Subrandom_Generator::check_class_invariants() const
{
    return generator_ != NULL;
}

} // end namespace rtt_rng

//---------------------------------------------------------------------------//
//                 end of LC_Subrandom_Generator.cc
//---------------------------------------------------------------------------//