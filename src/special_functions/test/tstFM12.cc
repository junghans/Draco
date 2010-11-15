//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   sf/test/tstFM12.cc
 * \author Kent Budge
 * \date   Tue Sep 21 11:57:47 2004
 * \brief  
 * \note   Copyright 2006 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <fstream>

#include <gsl/gsl_sf_gamma.h>

#include "ds++/ScalarUnitTest.hh"
#include "ds++/Soft_Equivalence.hh"
#include "units/PhysicalConstants.hh"

#include "../Release.hh"
#include "../FM12.hh"
//#include "../FM12inv.hh"

using namespace std;
using namespace rtt_dsxx;
using namespace rtt_sf;
using rtt_units::PI;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstFM12( UnitTest & ut )
{
    double f1 = FM12(-10.0);   
    if (soft_equiv(f1,
                   exp(-10.0+gsl_sf_lngamma(0.5))*(1-exp(-10.0)/sqrt(2.)),
                   2e-6))
    {
	ut.passes("correct FM12 for -10.0");
    }
    else
    {
	ut.failure("NOT correct FM12 for -10.0");
    }
    f1 = FM12(1000.0);   
    if (soft_equiv(f1,
                   pow(1000.0, 0.5)/0.5
                   - PI*PI*0.5*pow(1000.0, -1.5)/6.0, 1.0e-10))
    {
	ut.passes("correct FM12 for 1000.0");
    }
    else
    {
	ut.failure("NOT correct FM12 for 1000.0");
    }

    ofstream out("debug.dat");
    for (double eta = -10; eta<20; eta += 0.1)
    {
        out << eta << ' ' << FM12(eta) << endl;
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    try
    {
        ScalarUnitTest ut( argc, argv, release );
	tstFM12(ut);
    }
    catch (exception &err)
    {
        cout << "ERROR: While testing " << argv[0] << ", "
             << err.what() << endl;
        return 1;
    }
    catch( ... )
    {
        cout << "ERROR: While testing " << argv[0] << ", " 
             << "An unknown exception was thrown." << endl;
        return 1;
    }
    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tstFM12.cc
//---------------------------------------------------------------------------//
