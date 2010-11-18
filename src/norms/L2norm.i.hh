//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   norms/L2norm.i.hh
 * \author Kent Budge
 * \date   Tue Sep 18 08:22:09 2007
 * \brief  Member definitions of class L2norm
 * \note   Copyright (C) 2006 Los Alamos National Security, LLC
 *
 * This class is deprecated. New code should use Norm_Index instead.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef norms_L2norm_i_hh
#define norms_L2norm_i_hh

#include <cmath>
#include <numeric>
#include "c4/global.hh"
#include "norm.hh"

namespace rtt_norms
{

//---------------------------------------------------------------------------//
/*! Helper type for L2norm.
 *
 * \arg \a Field A real type such as float or double.
 */
template<class Field>
double accumulate_norm_(double const init, Field const &x)
{
    return init + norm<Field>(x);
}

//---------------------------------------------------------------------------//
/*!
 *
 * \arg \a In An input container type whose elements are real, such as
 * <code>vector<double></code> or <code>list<float></code>.
 *
 * \param x Container representing a real vector whose norm is desired.
 */
template<class In>
double L2norm(In const &x)
{
    double norm =
        std::accumulate(x.begin(),
                        x.end(),
                        0.0,
                        accumulate_norm_<typename In::value_type>);
    
    rtt_c4::global_sum(norm);

    unsigned xlength(x.size());

    rtt_c4::global_sum(xlength);
    Require(xlength>0);

    norm = sqrt(norm/xlength);

    Ensure(norm>=0.0);
    return norm;
}


} // end namespace rtt_norms

#endif // norms_L2norm_i_hh

//---------------------------------------------------------------------------//
//              end of norms/L2norm.i.hh
//---------------------------------------------------------------------------//
