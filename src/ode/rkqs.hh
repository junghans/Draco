//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ode/rkqs.hh
 * \author Kent Budge
 * \date   Mon Sep 20 15:15:40 2004
 * \brief  Integrate an ordinary differential equation with local error
 * control using fifth-order Cash-Karp Runge-Kutta steps.
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef ode_rkqs_hh
#define ode_rkqs_hh

#include "ds++/config.h"
#include <vector>

namespace rtt_ode
{

//! Perform a single fifth-order Cash-Karp Runge-Kutta step.
template<typename Field, typename Function>
// DLL_PUBLIC_ode 
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
template<typename Field, typename Function> 
// DLL_PUBLIC_ode 
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

#include "rkqs.i.hh"

#endif // ode_rkqs_hh

//---------------------------------------------------------------------------//
// end of ode/rkqs.hh
//---------------------------------------------------------------------------//
