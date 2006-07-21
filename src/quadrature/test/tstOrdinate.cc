//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/test/tstOrdinate.cc
 * \author Kelly Thompson
 * \date   Tue June 20 14:25 2006
 * \brief  Unit test for Ordinate class.
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "ds++/ScalarUnitTest.hh"
#include "ds++/Soft_Equivalence.hh"
#include "units/PhysicalConstants.hh"
#include "special_functions/Ylm.hh"
#include "mesh_element/Geometry.hh"
#include "../QuadCreator.hh"
#include "../Quadrature.hh"
#include "../Q1DGaussLeg.hh"
#include "../QuadServices.hh"
#include "../Release.hh"
#include "../Ordinate.hh"

using namespace rtt_quadrature;
using namespace rtt_dsxx;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void test_ordinate_ctor( rtt_dsxx::UnitTest & ut )
{
    using rtt_units::PI;
    
    { // Test default constructor.
        Ordinate Omega;
        if( soft_equiv( Omega.x, 0.0 ) &&
            soft_equiv( Omega.y, 0.0 ) &&
            soft_equiv( Omega.z, 0.0 ) &&
            soft_equiv( Omega.weight, 0.0 ) )
            ut.passes("Default constructor initializes data to zero.");
        else
            ut.failure("Default constructor did not initialize data to zero.");
    }
    { // Test 1D constructor
        double const sqrtThird(std::sqrt(1.0/3.0)), wt(PI/2);
        Ordinate Omega( sqrtThird, wt );
        if( soft_equiv( Omega.x, sqrtThird ) &&
            soft_equiv( Omega.weight, wt ) )
            ut.passes("Constructor initializes data to correct values.");
        else
            ut.failure("Constructor did not initialize data to correct values.");
    }
    { // Test 3D constructor
        double const sqrtThird(std::sqrt(1.0/3.0)), wt(PI/2);
        Ordinate Omega( sqrtThird, sqrtThird, sqrtThird, wt );
        if( soft_equiv( Omega.x, sqrtThird ) &&
            soft_equiv( Omega.y, sqrtThird ) &&
            soft_equiv( Omega.z, sqrtThird ) &&
            soft_equiv( Omega.weight, wt ) )
            ut.passes("Constructor initializes data to correct values.");
        else
            ut.failure("Constructor did not initialize data to correct values.");
    }
    return;
}

//---------------------------------------------------------------------------//

void test_ordinate_sort( UnitTest & ut )
{
    using namespace rtt_quadrature;
    
    // Generate a quadrature set.
    QuadCreator qc;
    SP< Quadrature const > const spQ( qc.quadCreate( QuadCreator::GaussLeg, 8 ) );
    int const numAngles( spQ->getNumAngles() );
    vector< Ordinate > angles;
    for( int i=0; i<numAngles; ++i )
        angles.push_back( Ordinate( spQ->getMu(i), spQ->getWt(i) ) );

    std::sort(angles.begin(), angles.end(), OrdinateSet::SnCompare );

    bool sorted(true);
    for( int i=0; i<numAngles-1; ++i )
    {
        if( angles[i+1].x < angles[i].x )
        {
            sorted = false;
            break;
        }
    }
    if( sorted )
        ut.passes("Ordinates were sorted correctly using the comparator SnCompare().");
    else
        ut.failure( "Ordinates not sorted correctly when using the comparator SnCompare().");
    
    return;
}

//---------------------------------------------------------------------------//

void test_create_ordinate_set( UnitTest & ut )
{
    using namespace rtt_quadrature;
    
    // Generate a quadrature set.
    QuadCreator qc;
    SP< Quadrature const > const spQ( qc.quadCreate( QuadCreator::GaussLeg, 8 ) );
    int const numAngles( spQ->getNumAngles() );

    // Call the function that we are testing.
    int const dim( 1 );
    OrdinateSet const angles( spQ, rtt_mesh_element::CARTESIAN, dim );

    // Check the result
    bool looksGood(true);
    for( int i=0; i<numAngles; ++i )
    {
        if( ! soft_equiv( angles[i].x, spQ->getMu(i) ) )
        {
            looksGood = false;
            break;
        }
        if( ! soft_equiv( angles[i].weight, spQ->getWt(i) ) )
        {
            looksGood = false;
            break;
        }
    }
    if( looksGood )
        ut.passes("Create_Ordinate_Set works!");
    else
        ut.passes("Create_Ordinate_Set failed for 1D S8.");

    return;
}

//---------------------------------------------------------------------------//
void test_Y( UnitTest & ut)
{
    using namespace rtt_quadrature;
    
    // Generate a quadrature set.
    QuadCreator qc;
    int const quadOrder(2);
    SP< Quadrature const > const spQ( qc.quadCreate( QuadCreator::LevelSym2D, quadOrder ) );
    int    const numAngles( spQ->getNumAngles() );
    double const sumwt(     spQ->getNorm()      );

    // Call the function that we are testing.
    int const dim( 2 );
    OrdinateSet const angles( spQ, rtt_mesh_element::CARTESIAN, dim );

    for( int ell=0; ell < 3; ++ell )
    {
        int k=-1*ell;
        for( ; k<=ell ; ++k )
        {
            unsigned m=0;
            double sph( OrdinateSet::Y(ell,k, angles[m], sumwt ) );
            double theta( std::acos( angles[m].x ) );
            double phi( QuadServices::compute_azimuthalAngle( angles[m].x, angles[m].y, angles[m].z ) );
            
            double sfYlm( rtt_sf::galerkinYlk(ell,k,angles[m].z,phi,sumwt ) );
            
            if( soft_equiv( sfYlm, sph ) )
            {
                std::ostringstream msg;
                msg << "Y(l,k,Omega,sumwt) == galerkinYlk(l,k,xi,phi,sumwt) for l=" << ell
                    << " and k=" << k << "." << std::endl;
                ut.passes( msg.str() );
            }
            else
            {
                std::ostringstream msg;
                msg << "Y(l,k,Omega,sumwt) != galerkinYlk(l,k,xi,phi,sumwt) for l=" << ell
                    << " and k=" << k << ".\n"
                    << "\tFound           Y(" << ell << "," << k << ",Omega,sumwt) = " << sph << "\n"
                    << "\tFound galerkinYlk(" << ell << "," << k << ",xi,phi,sumwt) = " << sfYlm << "\n"
                    << "\tFound   phi = " << phi << "\n"
                    << "\tFound atan2 = " << std::atan2(angles[m].y,angles[m].x) 
                    << std::endl;
                ut.failure( msg.str() );
            }
        }
    }
    return;
}


//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    using std::cout;
    using std::endl;
    
    try
    {
        // Test ctor for ScalarUnitTest (also tests UnitTest ctor and member
        // function setTestName).
        rtt_dsxx::ScalarUnitTest ut( argc, argv, release );
        test_ordinate_ctor(ut);
        test_ordinate_sort(ut);
        test_create_ordinate_set(ut);
        test_Y(ut);
    }
    catch( rtt_dsxx::assertion &err )
    {
        cout << "ERROR: While testing " << argv[0] << ", " << err.what() << endl;
        return 1;
    }
    catch( ... )
    {
        cout << "ERROR: While testing " << argv[0] << ", " 
             << "An unknown exception was thrown" << endl;
        return 1;
    }

    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tQuadServices.cc
//---------------------------------------------------------------------------//