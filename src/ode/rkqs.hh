//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ode/rkqs.hh
 * \author Kent Budge
 * \date   Mon Sep 20 15:15:40 2004
 * \brief  Integrate an ordinary differential equation with local error
 * control using fifth-order Cash-Karp Runge-Kutta steps.
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef ode_rkqs_hh
#define ode_rkqs_hh

namespace rtt_ode
{

//! Perform a single fifth-order Cash-Karp Runge-Kutta step.
template<class Field, class Function>
void rkck(std::vector<Field> const &y,
	  std::vector<Field> const &dydx,
	  double x,
	  double h,
	  std::vector<Field> &yout,
	  std::vector<Field> &yerr,
	  Function derivs);

/*! \brief Integrate an ordinary differential equation with local error
 * control using fifth-order Cash-Karp Runge-Kutta steps.
 */
template<class Field, class Function>
void rkqs(std::vector<Field> &y,
	  std::vector<Field> const &dydx,
	  double &x, 
	  double htry,
	  double eps,
	  std::vector<Field> const &yscal,
	  double &hdid,
	  double &hnext, 
	  Function derivs);

} // end namespace rtt_ode

#endif // ode_rkqs_hh

//---------------------------------------------------------------------------//
//              end of ode/rkqs.hh
//---------------------------------------------------------------------------//