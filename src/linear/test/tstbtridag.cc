//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   linear/test/tstbtridag.cc
 * \author Kent Budge * \brief  
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>

#include "ds++/ScalarUnitTest.hh"
#include "ds++/Soft_Equivalence.hh"

#include "../Release.hh"
#include "../btridag.hh"

using namespace std;
using namespace rtt_dsxx;
using namespace rtt_linear;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstbtridag(UnitTest &ut)
{
    vector<double> a(2), b(2), c(2), r(2);
    b[0] = 2.;
    c[0] = 3.;
    a[1] = 1.;
    b[1] = 5.;

    r[0] = 1.0;
    r[1] = 2.0;

    vector<double> u(2);

    btridag(a,
            b,
            c,
            r,
            2U,
            1U,
            u);
     
    if (soft_equiv(1.0, u[0]*2 + u[1]*3))
    {
	ut.passes("0 is correct");
    }
    else
    {
	ut.failure("0 is NOT correct");
    }
    if (soft_equiv(2.0, u[0]*1 + u[1]*5))
    {
	ut.passes("0 is correct");
    }
    else
    {
	ut.failure("0 is NOT correct");
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    try
    {
        UnitTest ut( argc, argv, release );
	tstbtridag(ut);
    }
    catch (exception &err)
    {
	cout << "ERROR: While testing tstsvbksb, " << err.what() << endl;
	return 1;
    }
    catch( ... )
    {
	cout << "ERROR: While testing tstsvbksb, " 
             << "An unknown exception was thrown." << endl;
	return 1;
    }
    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tstsvbksb.cc
//---------------------------------------------------------------------------//