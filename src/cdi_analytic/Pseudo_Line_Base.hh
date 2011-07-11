//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_analytic/Pseudo_Line_Base.hh
 * \author Kent G. Budge
 * \date   Tue Apr  5 08:36:13 MDT 2011
 * \note   Copyright (C) 2011 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_analytic_Pseudo_Line_Base_hh__
#define __cdi_analytic_Pseudo_Line_Base_hh__

#include "parser/Expression.hh"

namespace rtt_cdi_analytic
{
using std::vector;
using rtt_dsxx::SP;
using rtt_parser::Expression;

//---------------------------------------------------------------------------//
/*!
 * \class Pseudo_Line_Base
 * \brief Defines a random line spectrum for the opacity.
 *
 * The opacity function is a continuum on which is superimposed a number of
 * lines of the specified peak and width. The line locations are chosen at
 * random.
 *
 * The mass opacity coefficient is assumed independent of temperature or
 * density, which allows precalculation of the opacity structure, an important
 * time saver.
 *
 */
class Pseudo_Line_Base
{
  public:

    enum Averaging
    {
        NONE,      //!< evaluate opacity at band center
        ROSSELAND, //!< form a Rosseland (transparency) mean
        PLANCK,     //!< form a Planck (extinction) mean

        END_AVERAGING //!< sentinel value
    };
    
  private:
    // Coefficients
    SP<Expression const> continuum_;  // continuum opacity [cm^2/g]
    unsigned seed_;
    unsigned number_of_lines_;
    double line_peak_; // peak line opacity [cm^2/g]
    double line_width_;  // line width as fraction of line frequency.
    unsigned number_of_edges_;
    double edge_ratio_;

    double Tref_;  // reference temperature for temperature dependence
    double Tpow_;  // temperature dependence exponent

    vector<double> center_; // line centers for this realization
    vector<double> edge_;   // edges for this realization
    vector<double> edge_factor_; // opacity at threshold

  public:

    Pseudo_Line_Base(SP<Expression const> const &cont,
                     unsigned number_of_lines,
                     double line_peak,
                     double line_width,
                     unsigned number_of_edges,
                     double edge_ratio,
                     double Tref,
                     double Tpow,
                     double emin,
                     double emax,
                     unsigned seed);
    
    //! Constructor for packed state.
    explicit  Pseudo_Line_Base(vector<char> const &packed);

    double line_width() const { return line_width_; }

    //! Pack up the class for persistence.
    vector<char> pack() const;

    //! Compute a monochromatic opacity
    double monoOpacity(double nu, double T) const;

    static double BB(double const T, double const x)
    {
        double const e = expm1(x/T);
        if (e>0.0)
            return x*x*x/e;
        else
            return x*x*T;
    }

    static double DBB(double const T, double const x)
    {
        double const e = expm1(x/T);
        if (e>0)
        {
            double const de = -exp(x/T)*x/(T*T);
            return -x*x*x*de/(e*e);
        }
        else
        {
            return x*x;
        }
    }
};

} // end namespace rtt_cdi_analytic

#endif  // __cdi_analytic_Pseudo_Line_Base_hh__

//---------------------------------------------------------------------------//
//       end of cdi_analytic/Pseudo_Line_Base.hh
//---------------------------------------------------------------------------//