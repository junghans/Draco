//----------------------------------*-C++-*----------------------------------------------//
/*!
 * \file   quadrature/Square_Chebyshev_Legendre.cc
 * \author Kelly Thompson
 * \date   Wed Sep  1 10:19:52 2004
 * \brief  
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------------------//
// $Id: Square_Chebyshev_Legendre.cc 6718 2012-08-30 20:03:01Z warsa $
//---------------------------------------------------------------------------------------//

#include "Square_Chebyshev_Legendre.hh"
#include "Gauss_Legendre.hh"

#include "ds++/to_string.hh"
#include "units/MathConstants.hh"

namespace rtt_quadrature
{
using namespace rtt_dsxx;

//---------------------------------------------------------------------------------------//
string Square_Chebyshev_Legendre::name() const { return "Square Chebyshev Legendre"; }

//---------------------------------------------------------------------------------------//
string Square_Chebyshev_Legendre::parse_name()  const { return "square cl"; }
        
//---------------------------------------------------------------------------------------//
Quadrature_Class Square_Chebyshev_Legendre::quadrature_class() const
{
    return SQUARE_QUADRATURE;
}

//---------------------------------------------------------------------------------------//
unsigned Square_Chebyshev_Legendre::number_of_levels() const { return sn_order_; }
    
//---------------------------------------------------------------------------------------//
string Square_Chebyshev_Legendre::as_text(string const &indent) const
{
    string Result =
        indent + "type = square cl" +
        indent + "  order = " + to_string(sn_order()) +
        Octant_Quadrature::as_text(indent);

    return Result;
}

//---------------------------------------------------------------------------------------//
void
Square_Chebyshev_Legendre::create_octant_ordinates_(vector<double> &mu,
                                                    vector<double> &eta,
                                                    vector<double> &wt) const
{
    using std::fabs;
    using std::sqrt;
    using std::cos;
    using rtt_dsxx::soft_equiv;

    // The number of quadrature levels is equal to the requested SN order.
    size_t levels = sn_order();

    // We build the 3-D first, then edit as appropriate.

    size_t numOrdinates = levels*levels/4;

    // Force the direction vectors to be the correct length.
    mu.resize(numOrdinates);
    eta.resize(numOrdinates);
    wt.resize(numOrdinates);

    SP<Gauss_Legendre> GL(new Gauss_Legendre(sn_order_));

    // NOTE: this aligns the gauss points with the x-axis (r-axis in cylindrical coords)

    for (unsigned i=0; i<levels/2; ++i)
    {
        double xmu=GL->mu(i);
        double xwt=GL->wt(i);
        double xsr=sqrt(1.0-xmu*xmu);
            
        for (unsigned j=0; j<levels/2; ++j)
        {
            size_t ordinate=j+i*levels/2;
            
            mu[ordinate]  = xsr*cos(rtt_units::PI*(2.0*j+1.0)/levels/2.0);
            eta[ordinate] = xsr*sin(rtt_units::PI*(2.0*j+1.0)/levels/2.0);
            wt[ordinate]  = xwt/levels;
        }
    }
}


} // end namespace rtt_quadrature

//---------------------------------------------------------------------------------------//
//                 end of Square_Chebyshev_Legendre.cc
//---------------------------------------------------------------------------------------//
