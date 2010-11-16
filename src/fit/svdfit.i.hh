//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   fit/svdfit.i.hh
 * \author Kent Budge
 * \date   Mon Aug  9 13:17:31 2004
 * \brief  Calculate the singular value decomposition of a matrix.
 * \note   Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef fit_svdfit_i_hh
#define fit_svdfit_i_hh

#include <sstream>
#include <algorithm>
#include <vector>

#include "ds++/Assert.hh"

#include "ds++/square.hh"
#include "linear/svdcmp.i.hh"
#include "linear/svbksb.i.hh"
#include "svdfit.hh"

namespace rtt_fit
{
using std::vector;

//---------------------------------------------------------------------------//
/*! 
 * \brief Compute a generalized least squares fit.
 *
 * Given a set of data, find the best linear combination of arbitrary basis
 * functions to fit the data.
 *
 * \arg \a RandomContainer A random access container type
 *
 * \arg \a Functor A functor type taking a double value and a reference to a
 * RandomContainer that stores the values of the basis functions for the
 * double value in the RandomContainer.
 *
 * \param x Ordinates of data set. For multivariable fits, one can let the
 * ordinates be an index into a table containing the full vector of
 * independent variables for each data point.
 *
 * \param y Abscissae of data set.
 * 
 * \param sig Uncertainty in the data. Where unknown or not applicable, one
 * can set all values to 1.
 *
 * \param a On entry, must be sized to the number of basis functions. On exit,
 * contains the coefficients of the fit.
 *
 * \param u On exit, contains the upper matrix of the singular value
 * decomposition of the fit.
 *
 * \param v On exit, containts the diagonal matrix of the singular value
 * decomposition of the fit, e.g., the singular values.
 *
 * \param w On exit, containts the lower matrix of the singular value
 * decomposition of the fit.
 *
 * \param chisq On return, contains the chi square of the fit (meaasure of
 * goodness of fit.)
 *
 * \param funcs Functor to calculate the basis functions for a given argument.
 */

template<class RandomContainer,
         class Functor>
void svdfit(RandomContainer const &x,
            RandomContainer const &y,
            RandomContainer const &sig,
            RandomContainer &a,
            RandomContainer &u,
            RandomContainer &v,
            RandomContainer &w,
            double &chisq,
            Functor &funcs,
            double TOL)
{
    Require(x.size()==y.size());
    Require(x.size()==sig.size());
    Require(a.size()>0);

    using rtt_linear::svdcmp;
    using rtt_linear::svbksb;
    using rtt_dsxx::square;

    unsigned const ndata = x.size();
    unsigned const ma = a.size();

    vector<double> b(ndata), afunc(ma);

    u.resize(ndata*ma);
    
    for (unsigned i=0; i<ndata; ++i)
    {
        funcs(x[i], afunc);
        double const tmp = 1.0/sig[i];
        for (unsigned j=0; j<ma; ++j)
        {
            u[i+ndata*j] = afunc[j]*tmp;
        }
        b[i] = y[i]*tmp;
    }
    svdcmp(u,ndata,ma,w,v);
    double wmax = 0.0;
    for (unsigned j=0; j<ma; ++j)
    {
        if (w[j]>wmax)
        {
            wmax=w[j];
        }
    }
    double const thresh = TOL*wmax;
    for (unsigned j=0; j<ma; ++j)
    {
        if (w[j]<thresh)
        {
            w[j] = 0.0;
        }
    }
    svbksb(u,w,v,ndata,ma,b,a);
    chisq = 0.0;
    for (unsigned i=0; i<ndata; ++i)
    {
        funcs(x[i], afunc);
        double sum = 0.0;
        for (unsigned j=0; j<ma; ++j)
        {
            sum += a[j]*afunc[j];
        }
        chisq += square((y[i]-sum)/sig[i]);
    }
}

} // end namespace rtt_fit

#endif // fit_svdfit_i_hh

//---------------------------------------------------------------------------//
//              end of fit/svdfit.i.hh
//---------------------------------------------------------------------------//