//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_analytic/Pseudo_Line_Analytic_Odfmg_Opacity.cc
 * \author Thomas M. Evans
 * \date   Tue Nov 13 11:19:59 2001
 * \brief  Pseudo_Line_Analytic_Odfmg_Opacity class member definitions.
 * \note   Copyright (C) 2001-2010 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "ode/rkqs.i.hh"
#include "ode/quad.i.hh"
#include "Pseudo_Line_Analytic_MultigroupOpacity.hh"
#include "Pseudo_Line_Analytic_Odfmg_Opacity.hh"
#include "ds++/Packing_Utils.hh"
#include "ds++/cube.hh"
#include "cdi/CDI.hh"

namespace rtt_cdi_analytic
{
using namespace std;
using namespace rtt_dsxx;
using namespace rtt_ode;
using namespace rtt_cdi;

//---------------------------------------------------------------------------//
Pseudo_Line_Analytic_Odfmg_Opacity::Pseudo_Line_Analytic_Odfmg_Opacity(
        const sf_double         &groups,
        const sf_double         &bands,
        rtt_cdi::Reaction        reaction_in,
        SP<Expression const> const &continuum,
        unsigned number_of_lines,
        double line_peak,
        double line_width,
        unsigned number_of_edges,
        double edge_ratio,
        double Tref,
        double Tpow,
        double emin,
        double emax,
        Averaging averaging,
        unsigned qpoints,
        unsigned seed)
    :
    Analytic_Odfmg_Opacity(groups, bands, reaction_in),
    Pseudo_Line_Base(continuum,
                     number_of_lines,
                     line_peak,
                     line_width,
                     number_of_edges,
                     edge_ratio,
                     Tref,
                     Tpow,
                     emin,
                     emax,
                     seed),
    averaging_(averaging),
    qpoints_(qpoints)
{
    Require(qpoints>0);
}

//---------------------------------------------------------------------------//
// OPACITY INTERFACE FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * \brief Return the group opacities given a scalar temperature and density. 
 *
 * Given a scalar temperature and density, return the group opacities
 * (vector<double>) for the reaction type specified by the constructor.  The
 * analytic opacity model is specified in the constructor
 * (Pseudo_Line_Analytic_Odfmg_Opacity()).
 *
 * \param temperature material temperature in keV
 * \param density material density in g/cm^3
 * \return group opacities (coefficients) in cm^2/g
 *
 */
std::vector< std::vector<double> >
Pseudo_Line_Analytic_Odfmg_Opacity::getOpacity(double T,
                                               double /* rho */ ) const 
{
    sf_double const &group_bounds = this->getGroupBoundaries();
    unsigned const number_of_groups = group_bounds.size()-1U;
    // temporary
    vector<double> bands(2);
    bands[0] = 0.0;
    bands[1] = 1.0;
    vector<vector<double> > Result(number_of_groups, bands);

    switch (averaging_)
    {
        case NONE:
            {
                double g1 = group_bounds[0];
                for (unsigned g=0; g<number_of_groups; ++g)
                {
                    double const g0 = g1;
                    g1 = group_bounds[g+1];
                    double const nu = 0.5*(g0 + g1);
                    Result[g][0] = monoOpacity(nu, T);
                }
            }
            break;
            
        case ROSSELAND:
        {
            double g1 = group_bounds[0];
            for (unsigned g=0; g<number_of_groups; ++g)
            {
                double const g0 = g1;
                g1 = group_bounds[g+1];
                double t = 0.0, b = 0.0;

                for (unsigned ig=0; ig<qpoints_; ++ig)
                {
                    double const x = (ig+0.5)*(g1-g0)/qpoints_ + g0;
                    
                    double w = CDI::integrateRosselandSpectrum(g0,
                                                               g1,
                                                               T);
                    
                    
                    t += w/monoOpacity(x, T);
                    b += w;
                }
                
                Result[g][0] = b/t;
            }
        }
        break;
           
        case PLANCK:
        {
            double g1 = group_bounds[0];
            for (unsigned g=0; g<number_of_groups; ++g)
            {
                double const g0 = g1;
                g1 = group_bounds[g+1];
                double t = 0.0, b = 0.0;
                
                for (unsigned ig=0; ig<qpoints_; ++ig)
                {
                    double const x = (ig+0.5)*(g1-g0)/qpoints_ + g0;
                    
                    double w = CDI::integratePlanckSpectrum(g0,
                                                            g1,
                                                            T);
                    
                    
                    t += w*monoOpacity(x, T);
                    b += w;
                }
                
                Result[g][0] = t/b;
            }
        }
        break;
        
        default:
            Insist(false, "bad case");            
    }

    return Result;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Opacity accessor that returns a vector of multigroupband
 *     opacity 2-D vectors that correspond to the provided vector of
 *     temperatures and a single density value.
 */
std::vector< std::vector< std::vector<double> > >  Pseudo_Line_Analytic_Odfmg_Opacity::getOpacity( 
    const std::vector<double>& targetTemperature,
    double targetDensity ) const
{ 
    std::vector< std::vector< std::vector<double> > > opacity( targetTemperature.size() );

    for ( size_t i=0; i<targetTemperature.size(); ++i )
    {
        opacity[i] = getOpacity(targetTemperature[i], targetDensity);
    }
    return opacity;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Opacity accessor that returns a vector of multigroupband
 *     opacity 2-D vectors that correspond to the provided
 *     temperature and a vector of density values.
 */
std::vector< std::vector< std::vector<double> > >  Pseudo_Line_Analytic_Odfmg_Opacity::getOpacity( 
    double targetTemperature,
    const std::vector<double>& targetDensity ) const
{ 
    std::vector< std::vector< std::vector<double> > > opacity( targetDensity.size() );

    //call our regular getOpacity function for every target density
    for ( size_t i=0; i<targetDensity.size(); ++i )
    {
        opacity[i] = getOpacity(targetTemperature, targetDensity[i]);
    }
    return opacity;
}

//---------------------------------------------------------------------------//
Pseudo_Line_Analytic_Odfmg_Opacity::std_string
Pseudo_Line_Analytic_Odfmg_Opacity::getDataDescriptor() const
{
    std_string descriptor;

    rtt_cdi::Reaction const reaction = getReactionType();
    
    if (reaction == rtt_cdi::TOTAL)
        descriptor = "Pseudo Line Odfmg Total";
    else if (reaction == rtt_cdi::ABSORPTION)
        descriptor = "Pseudo Line Odfmg Absorption";
    else if (reaction == rtt_cdi::SCATTERING)
        descriptor = "Pseudo Line Odfmg Scattering";
    else
    {
        Insist (0, "Invalid Pseudo Line Odfmg model opacity!");
    }

    return descriptor;
}

//---------------------------------------------------------------------------//
// Packing function

Analytic_MultigroupOpacity::sf_char
Pseudo_Line_Analytic_Odfmg_Opacity::pack() const 
{
    sf_char const pdata = Analytic_Odfmg_Opacity::pack();
    sf_char const pdata2 = Pseudo_Line_Base::pack();

    sf_char Result(pdata.size()+pdata2.size());
    copy(pdata.begin(), pdata.end(), Result.begin());
    copy(pdata2.begin(), pdata2.end(), Result.begin()+pdata.size());
    return pdata;
}

} // end namespace rtt_cdi_analytic

//---------------------------------------------------------------------------//
//                              end of Pseudo_Line_Analytic_Odfmg_Opacity.cc
//---------------------------------------------------------------------------//