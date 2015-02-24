//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/test/tstDouble_Gauss.cc
 * \author Kent G. Budge
 * \date   Tue Nov  6 13:08:49 2012
 * \note   Copyright (C) 2006-2015 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id: template_test.cc 5830 2011-05-05 19:43:43Z kellyt $
//---------------------------------------------------------------------------//

#include "ds++/ScalarUnitTest.hh"
#include "ds++/Release.hh"
#include "quadrature_test.hh"
#include "quadrature/Double_Gauss.hh"

using namespace std;
using namespace rtt_dsxx;
using namespace rtt_quadrature;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    ScalarUnitTest ut(argc, argv, release);
    try
    {
        Double_Gauss quadrature(8); // SN order = 8
        quadrature_test(ut, quadrature);
    }
    UT_EPILOG(ut);
}

//---------------------------------------------------------------------------//
// end of tstDouble_Gauss.cc
//---------------------------------------------------------------------------//
