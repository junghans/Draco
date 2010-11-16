//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   roots/test/tstcubic1.cc
 * \author Kent G. Budge
 * \date   Wed Sep 15 10:12:52 2010
 * \brief  
 * \note   Copyright (C) 2006 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cmath>

#include "ds++/Assert.hh"
#include "ds++/Soft_Equivalence.hh"
#include "../Release.hh"
#include "ds++/ScalarUnitTest.hh"
#include "../cubic1.hh"

using namespace std;
using namespace rtt_dsxx;
using namespace rtt_roots;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstcubic1(UnitTest &ut)
{
    // Solve (x*x+1)*(x-1) = x*x*x-x*x+x-1 = 0
    
    double root = cubic1(-1., 1., -1.);

    if (soft_equiv(root, 1.0))
    {
        ut.passes("Correctly solved cubic equation");
    }
    else
    {
        ut.failure("Did NOT correctly solve cubic equation");
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    rtt_dsxx::ScalarUnitTest ut(argc, argv, release);
    try
    {
        tstcubic1(ut);
    }
    catch (std::exception &err)
    {
        std::cout << "ERROR: While testing tstcubic1, " 
                  << err.what()
                  << endl;
        ut.numFails++;
    }
    catch( ... )
    {
        std::cout << "ERROR: While testing tstcubic1, " 
                  << "An unknown exception was thrown."
                  << endl;
        ut.numFails++;
    }
    return ut.numFails;
}   

//---------------------------------------------------------------------------//
//                        end of tstcubic1.cc
//---------------------------------------------------------------------------//