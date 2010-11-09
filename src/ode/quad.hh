//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ode/quad.hh
 * \author Kent Budge
 * \date   Mon Sep 20 15:30:05 2004
 * \brief  Adaptive quadrature of a function over a specified interval.
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef ode_quad_hh
#define ode_quad_hh

#include "Function_Traits.hh"

namespace rtt_ode
{
//---------------------------------------------------------------------------//
/*! 
 * \brief Helper class for quad
 *
 * This class transforms a quadrature into an ODE problem.  This allows
 * adaptive ODE methods to be used for adaptive quadrature.
 *
 * \arg \a Function A function type for which <code> double operator()(double)
 * </code> is defined.
 */

template<class Function>
class Quad_To_ODE
{
  public:

    Quad_To_ODE(Function &f) : func(f) {}

    void operator()(double x,
		    std::vector<double> const &,
		    std::vector<double> &dydx)
    {
	dydx[0] = func(x);
    }

  private:
    Function &func;
};


//! Adaptive quadrature of a function over a specified interval.
template<class Function, class Rule>
typename Function_Traits<Function>::return_type 
quad(Function func, 
     double x1, 
     double x2,
     double &eps,
     Rule rule);

} // end namespace rtt_ode

#endif // ode_quad_hh

//---------------------------------------------------------------------------//
//              end of ode/quad.hh
//---------------------------------------------------------------------------//