//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/test/tQuadServices.cc
 * \author Kelly Thompson
 * \date   Mon Nov 8 10:48 2004
 * \brief  Quadrature Services.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>

#include "ds++/Assert.hh"
#include "ds++/SP.hh"
#include "ds++/Soft_Equivalence.hh"

#include "quadrature_test.hh"
#include "../Quadrature.hh"
#include "../QuadCreator.hh"
#include "../QuadServices.hh"
#include "../Release.hh"

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

// Legendre Polynomials
double P( int const ell, double const x )
{
    Require( ell >= 0 );
    Require( ell < 7 );
    Require( std::abs(x) <= 1.0 );

    if( ell == 0 ) return 1.0;
    if( ell == 1 ) return x;
    if( ell == 2 ) return (3.0*x*x-1.0)/2.0;
    if( ell == 3 ) return (5.0*x*x*x - 3.0*x )/2.0;
    if( ell == 4 ) return (35.0*x*x*x*x - 30.0*x*x + 3)/8.0;
    if( ell == 5 ) return (63.0*x*x*x*x*x - 70.0*x*x*x + 15.0*x)/8.0;

    Ensure( ell == 6 );
    return (231.0*x*x*x*x*x*x - 315.0*x*x*x*x +105.0*x*x - 5.0)/16.0;
}

//---------------------------------------------------------------------------//

double getclk( unsigned const ell, int const k )
{
    using std::sqrt;
    using std::abs;
    using rtt_quadrature::factorial;
    using rtt_quadrature::kronecker_delta;
    return sqrt( (2.0 - kronecker_delta(k,0) ) 
		 * factorial(ell-abs(k)) / (1.0*factorial(ell+abs(k)) ));
}

//---------------------------------------------------------------------------//
void test_kdelta()
{
    using rtt_quadrature::kronecker_delta;
    
    if( kronecker_delta( 0, 0 ) == 1 )
    {
	PASSMSG("Found kronecker_delta(0,0) == 1, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(0,0) != 1, kronecker_delta is not working.");
    }
    if( kronecker_delta( 0, 1 ) == 0 )
    {
	PASSMSG("Found kronecker_delta(0,1) == 0, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(0,1) != 0, kronecker_delta is not working.");
    }
    if( kronecker_delta( 1, 1 ) == 1 )
    {
	PASSMSG("Found kronecker_delta(1,1) == 1, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(1,1) != 1, kronecker_delta is not working.");
    }
    if( kronecker_delta( 1, 0 ) == 0 )
    {
	PASSMSG("Found kronecker_delta(1,0) == 0, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(1,0) != 0, kronecker_delta is not working.");
    }
    if( kronecker_delta( -1, 0 ) == 0 )
    {
	PASSMSG("Found kronecker_delta(-1,0) == 0, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(-1,0) != 0, kronecker_delta is not working.");
    }
    if( kronecker_delta( -1, -1 ) == 1 )
    {
	PASSMSG("Found kronecker_delta(-1,-1) == 1, kronecker_delta is working.");
    }
    else
    {
	PASSMSG("Found kronecker_delta(-1,-1) != 1, kronecker_delta is not working.");
    }
    return;
}

//---------------------------------------------------------------------------//
void test_factorial()
{
    using rtt_quadrature::factorial;
    if( factorial(0) == 1 )
    {
	PASSMSG("Found factorial(0) == 1, factorial is working.");
    }
    else
    {
	PASSMSG("Found factorial(0) != 1, factorial is not working.");
    }
    if( factorial(1) == 1 )
    {
	PASSMSG("Found factorial(1) == 1, factorial is working.");
    }
    else
    {
	PASSMSG("Found factorial(1) != 1, factorial is not working.");
    }
    if( factorial(2) == 2 )
    {
	PASSMSG("Found factorial(2) == 2, factorial is working.");
    }
    else
    {
	PASSMSG("Found factorial(2) != 2, factorial is not working.");
    }
    if( factorial(3) == 6 )
    {
	PASSMSG("Found factorial(3) == 6, factorial is working.");
    }
    else
    {
	PASSMSG("Found factorial(3) != 6, factorial is not working.");
    }
    if( factorial(-3) == 1 )
    {
	PASSMSG("Found factorial(-3) == 1, factorial is working.");
    }
    else
    {
	PASSMSG("Found factorial(-3) != 1, factorial is not working.");
    }
   return;
}

//---------------------------------------------------------------------------//

void test_quad_services_with_1D_S2_quad()
{   
    using rtt_quadrature::QuadCreator;
    using rtt_quadrature::Quadrature;
    using rtt_quadrature::QuadServices;
    using rtt_dsxx::SP;
    using rtt_dsxx::soft_equiv;
    
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;
    using std::ostringstream;

    //----------------------------------------
    // Setup Quadrature set
    
    // create an object that is responsible for creating quadrature objects.
    // QuadCreator QuadratureCreator;
    
    // we will only look at S2 Sets in this test.
    size_t const sn_ord_ref( 2                   );
    string const qname_ref ( "1D Gauss Legendre" );
    size_t const n_ang_ref ( 2                   );
    
    // Banner
    cout << "\nTesting the "  << qname_ref << "S"
	 << sn_ord_ref << " quadrature set." << endl << endl;
    
    // Create a quadrature set from a temporary instance of a
    // QuadratureCreator factory object.
    SP< const Quadrature > spQuad;
    spQuad = QuadCreator().quadCreate( QuadCreator::GaussLeg, sn_ord_ref ); 
    
    // print the name of the quadrature set that we are testing.
    string const qname   (  spQuad->name()         );
    size_t const snOrder(   spQuad->getSnOrder()   );
    size_t const numAngles( spQuad->getNumAngles() );
    double const sumwt(     spQuad->getNorm() );
    
    // check basic quadrature setup.
    if( snOrder != sn_ord_ref ) 
    {
	FAILMSG("Found incorrect Sn Order.");
    }
    else 
    {
	PASSMSG("Found correct Sn Order.");
    }
    if( numAngles != n_ang_ref  )
    {
	FAILMSG("Found incorrect number of angles.");
    }
    else 
    {
	PASSMSG("Found correct number of angles.");
    }
    if( qname != qname_ref  )
    {
	cout << qname << endl;
	FAILMSG("Found incorrect name of quadrature set.");
    }
    else 
    {
	PASSMSG("Found correct name of quadrature set.");
    }
    
    // Print a table
    spQuad->display();

    //----------------------------------------
    // Setup QuadServices object
    
    QuadServices qs( spQuad );
    
    vector<double> const M( qs.getM() );
    unsigned const numMoments( qs.getNumMoments() );

    std::vector< unsigned > dims;
    dims.push_back( numAngles );
    dims.push_back( numMoments );
    
    qs.print_matrix( "Mmatrix", M, dims );

    //----------------------------------------
    // For 1D Quadrature we have the following:
    //
    // k == 0, n == l, C_lk == 1, Omega_m == mu_m
    //
    //           2*n+1
    // M_{n,m} = ----- * 1 * Y_n( mu_m )
    //           sumwt
    //
    // Y_n( mu_m ) = P(l=0,k=0)(mu_m)*cos(k*theta)
    //             = P(n,mu_m)
    //----------------------------------------

    std::vector< double > const mu( spQuad->getMu() );
    double const clk(1.0);

    for( size_t n=0; n<numMoments; ++n )
    { 
	double const c( (2.0*n+1.0)/sumwt );

	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( M[ n + m*numMoments ], c*clk*P(n,mu[m]) ) )
	    {
		ostringstream msg;
		msg << "M[" << n << "," << m << "] has the expected value." << endl;
		    PASSMSG( msg.str() );
	    }
	    else
	    {		
		ostringstream msg;
		msg << "M[" << n << "," << m 
		    << "] does not have the expected value." << endl
		    << "\tFound M[" << n << "," << m << "] = " 
		    << M[ n + m*numMoments ] << ", but was expecting " 
		    << c*clk*P(n,mu[m]) << endl; 
		FAILMSG( msg.str() );		
	    }
	}
    } 

    //-----------------------------------//

    vector<double> const D( qs.getD() );
    qs.print_matrix( "Dmatrix", D, dims );

    // The first row of D should contain the quadrature weights.
    {
	unsigned n(0);
	std::vector< double > const wt( spQuad->getWt() );
	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( D[ m + n*numAngles ], wt[m] ) )
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " matched the expected value." << endl;
		PASSMSG( msg.str() );
	    }
	    else
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " did not match the expected value of " 
		    << wt[m] << "." << endl;
		FAILMSG( msg.str() );
	    }
	}
    }

    // Ensure D = M^{-1}
    // ------------------------------------------------------------
    {
	if( qs.D_equals_M_inverse() )
	{
	    PASSMSG("Found D = inverse(M).");
	}
	else
	{
	    FAILMSG("Oh no! D != inverse(M).");
	}
    }
    
    // Test applyD function
    // ------------------------------------------------------------
    {
	vector<double> const angularFlux( numAngles, 7.0 );
	vector<double> const fluxMoments( qs.applyD( angularFlux ) );

	if( soft_equiv( fluxMoments[0], 14.0 ) &&
	    soft_equiv( fluxMoments[1], 0.0 ) )
	{
	    PASSMSG("applyD() appears to work.");
	}
	else
	{
	    ostringstream msg;
	    msg << "applyD() failed to work as expected." << endl
		<< "Expected phi = { 14.0, 0.0} but found phi = { "
		<< fluxMoments[0] << ", " << fluxMoments[1] << " }." << endl;
	    FAILMSG(msg.str());
	}
    }

    // Test applyM function
    // ------------------------------------------------------------
    {
	double fm[2] = { 7.0, 0.0 };
	vector<double> const fluxMoments( fm, fm+2 );
	vector<double> const angularFlux( qs.applyM( fluxMoments ) );
	
	if( soft_equiv( angularFlux[0], 3.5 ) &&
	    soft_equiv( angularFlux[1], 3.5 ) )
	{
	    PASSMSG("applyM() appears to work.");
	}
	else
	{
	    ostringstream msg;
	    msg << "applyM() failed to work as expected." << endl
		<< "Expected psi = { 3.5, 3.5 } but found psi = { "
		<< angularFlux[0] << ", " << angularFlux[1] << " }." << endl;
	    FAILMSG(msg.str());
	}
    }	    


    return;
}

//---------------------------------------------------------------------------//

void test_quad_services_with_1D_S8_quad()
{   
    using rtt_quadrature::QuadCreator;
    using rtt_quadrature::Quadrature;
    using rtt_quadrature::QuadServices;
    using rtt_dsxx::SP;
    using rtt_dsxx::soft_equiv;
    
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;
    using std::ostringstream;

    //----------------------------------------
    // Setup Quadrature set
    
    // create an object that is responsible for creating quadrature objects.
    // QuadCreator QuadratureCreator;
    
    // we will only look at S2 Sets in this test.
    const size_t sn_ord_ref( 8                   );
    const string qname_ref ( "1D Gauss Legendre" );
    const size_t n_ang_ref ( 8                   );
    
    // Banner
    cout << "\nTesting the "  << qname_ref << " S"
	 << sn_ord_ref << " quadrature set." << endl << endl;
    
    // Create a quadrature set from a temporary instance of a
    // QuadratureCreator factory object.
    SP< const Quadrature > spQuad;
    spQuad = QuadCreator().quadCreate( QuadCreator::GaussLeg, sn_ord_ref ); 
    
    // print the name of the quadrature set that we are testing.
    const string qname   (  spQuad->name()         );
    const size_t snOrder(   spQuad->getSnOrder()   );
    const size_t numAngles( spQuad->getNumAngles() );
    const double sumwt(     spQuad->getNorm() );
    
    // check basic quadrature setup.
    if( snOrder != sn_ord_ref ) 
    {
	FAILMSG("Found incorrect Sn Order.");
    }
    else 
    {
	PASSMSG("Found correct Sn Order.");
    }
    if( numAngles != n_ang_ref  )
    {
	FAILMSG("Found incorrect number of angles.");
    }
    else 
    {
	PASSMSG("Found correct number of angles.");
    }
    if( qname != qname_ref  )
    {
	cout << qname << endl;
	FAILMSG("Found incorrect name of quadrature set.");
    }
    else 
    {
	PASSMSG("Found correct name of quadrature set.");
    }
    
    // Print a table
    spQuad->display();

    //----------------------------------------
    // Setup QuadServices object
    
    QuadServices qs( spQuad );
    
    vector<double> const M( qs.getM() );
    unsigned const numMoments( qs.getNumMoments() );

    std::vector< unsigned > dims;
    dims.push_back( numAngles );
    dims.push_back( numMoments );
    
    qs.print_matrix( "Mmatrix", M, dims );

    //----------------------------------------
    // For 1D Quadrature we have the following:
    //
    // k == 0, n == l, C_lk == 1, Omega_m == mu_m
    //
    //           2*n+1
    // M_{n,m} = ----- * 1 * Y_n( mu_m )
    //           sumwt
    //
    // Y_n( mu_m ) = P(l=0,k=0)(mu_m)*cos(k*theta)
    //             = P(n,mu_m)
    //----------------------------------------

    std::vector< double > const mu( spQuad->getMu() );
    double const clk(1.0);

    for( size_t n=0; n<numMoments && n<6; ++n )
    { 
	double const c( (2.0*n+1.0)/2.0 );

	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( M[ n + m*numMoments ], c*clk*P(n,mu[m]) ) )
	    {
		ostringstream msg;
		msg << "M[" << n << "," << m << "] has the expected value." << endl;
		    PASSMSG( msg.str() );
	    }
	    else
	    {		
		ostringstream msg;
		msg << "M[" << n << "," << m 
		    << "] does not have the expected value." << endl
		    << "\tFound M[" << n << "," << m << "] = " 
		    << M[ n + m*numMoments ] << ", but was expecting " 
		    << c*clk*P(n,mu[m]) << endl; 
		FAILMSG( msg.str() );		
	    }
	}
    } 

    //-----------------------------------//

    vector<double> const D( qs.getD() );
    qs.print_matrix( "Dmatrix", D, dims );
    
    // The first row of D should contain the quadrature weights.
    {
	unsigned n(0);
	std::vector< double > const wt( spQuad->getWt() );
	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( D[ m + n*numAngles ], wt[m] ) )
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " matched the expected value." << endl;
		PASSMSG( msg.str() );
	    }
	    else
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " did not match the expected value of " 
		    << wt[m] << "." << endl;
		FAILMSG( msg.str() );
	    }
	}
    }

    return;
}

//---------------------------------------------------------------------------//

void test_quad_services_with_3D_S2_quad()
{   
    using rtt_quadrature::QuadCreator;
    using rtt_quadrature::Quadrature;
    using rtt_quadrature::QuadServices;
    using rtt_dsxx::SP;
    using rtt_dsxx::soft_equiv;
    
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;
    using std::ostringstream;

    //----------------------------------------
    // Setup Quadrature set
    
    // create an object that is responsible for creating quadrature objects.
    // QuadCreator QuadratureCreator;
    
    // we will only look at S2 Sets in this test.
    const size_t sn_ord_ref( 4                    );
    const string qname_ref ( "3D Level Symmetric" );
    const size_t n_ang_ref ( 24                   );
    
    // Banner
    cout << "\nTesting the "  << qname_ref << " S"
	 << sn_ord_ref << " quadrature set." << endl << endl;
    
    // Create a quadrature set from a temporary instance of a
    // QuadratureCreator factory object.
    SP< const Quadrature > spQuad;
    spQuad = QuadCreator().quadCreate( QuadCreator::LevelSym, sn_ord_ref ); 
    
    // print the name of the quadrature set that we are testing.
    const string qname   (  spQuad->name()         );
    const size_t snOrder(   spQuad->getSnOrder()   );
    const size_t numAngles( spQuad->getNumAngles() );
    const double sumwt(     spQuad->getNorm() );

    // check basic quadrature setup.
    if( snOrder != sn_ord_ref ) 
    {
	FAILMSG("Found incorrect Sn Order.");
    }
    else 
    {
	PASSMSG("Found correct Sn Order.");
    }
    if( numAngles != n_ang_ref  )
    {
	FAILMSG("Found incorrect number of angles.");
    }
    else 
    {
	PASSMSG("Found correct number of angles.");
    }
    if( qname != qname_ref  )
    {
	cout << qname << endl;
	FAILMSG("Found incorrect name of quadrature set.");
    }
    else 
    {
	PASSMSG("Found correct name of quadrature set.");
    }
    
    // Print a table
    spQuad->display();

    //----------------------------------------
    // Setup QuadServices object
    
    QuadServices qs( spQuad );
    
    vector<double> const M( qs.getM() );
    unsigned const numMoments( qs.getNumMoments() );

    std::vector< unsigned > dims;
    dims.push_back( numAngles );
    dims.push_back( numMoments );
    
    qs.print_matrix( "Mmatrix", M, dims );

    //----------------------------------------
    // For 3D Quadrature we have the following:
    //
    // n maps to the index pair (l,k) via qs.n2kl
    // 
    //                       2*l+1
    // M_{n,m} = M_{l,k,m} = ----- * c_{l,k} * Y_{l,k}( mu_m )
    //                       sumwt
    //
    // Y_n( mu_m ) = P(l=0,k=0)(mu_m)*cos(k*theta)
    //             = P(n,mu_m)
    //----------------------------------------

    std::vector< double > const mu( spQuad->getMu() );

    for( size_t n=0; n<numMoments; ++n )
    { 
	unsigned const ell( qs.lkPair( n ).first  );
	int      const k(   qs.lkPair( n ).second );
	double   const c(   ( 2.0*ell+1.0 ) / sumwt );
	
	if( k == 0 ) 
	{
	    for( size_t m=0; m<numAngles; ++m )
	    {
		
		if( soft_equiv( M[ n + m*numMoments ], 
				c*getclk(ell,k)*P(ell,mu[m]) ) )
		{
		    ostringstream msg;
		    msg << "M[" << n << "," << m 
			<< "] has the expected value." << endl;
		    PASSMSG( msg.str() );
		}
		else
		{		
		    ostringstream msg;
		    msg << "M[" << n << "," << m 
			<< "] does not have the expected value." << endl
			<< "\tFound M[" << n << "," << m << "] = " 
			<< M[ n + m*numMoments ] << ", but was expecting " 
			<< c*getclk(ell,k)*P(ell,mu[m]) << endl; 
		    FAILMSG( msg.str() );		
		}
	    }
	}
    } 

    //-----------------------------------//

    vector<double> const D( qs.getD() );
    qs.print_matrix( "Dmatrix", D, dims );
    
    // The first row of D should contain the quadrature weights.
    {
	unsigned n(0);
	std::vector< double > const wt( spQuad->getWt() );
	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( D[ m + n*numAngles ], wt[m] ) )
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " matched the expected value." << endl;
		PASSMSG( msg.str() );
	    }
	    else
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " did not match the expected value of " 
		    << wt[m] << "." << endl;
		FAILMSG( msg.str() );
	    }
	}
    }

    return;
}

//---------------------------------------------------------------------------//

void test_quad_services_with_2D_S2_quad()
{   
    using rtt_quadrature::QuadCreator;
    using rtt_quadrature::Quadrature;
    using rtt_quadrature::QuadServices;
    using rtt_dsxx::SP;
    using rtt_dsxx::soft_equiv;
    
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;
    using std::ostringstream;

    //----------------------------------------
    // Setup Quadrature set
    
    // create an object that is responsible for creating quadrature objects.
    // QuadCreator QuadratureCreator;
    
    // we will only look at S2 Sets in this test.
    const size_t sn_ord_ref( 6                    );
    const string qname_ref ( "2D Level Symmetric" );
    const size_t n_ang_ref ( 24                   );
    
    // Banner
    cout << "\nTesting the "  << qname_ref << " S"
	 << sn_ord_ref << " quadrature set." << endl << endl;
    
    // Create a quadrature set from a temporary instance of a
    // QuadratureCreator factory object.
    SP< const Quadrature > spQuad;
    spQuad = QuadCreator().quadCreate( QuadCreator::LevelSym2D, sn_ord_ref ); 
    
    // print the name of the quadrature set that we are testing.
    const string qname   (  spQuad->name()         );
    const size_t snOrder(   spQuad->getSnOrder()   );
    const size_t numAngles( spQuad->getNumAngles() );
    const double sumwt(     spQuad->getNorm() );

    // check basic quadrature setup.
    if( snOrder != sn_ord_ref ) 
    {
	FAILMSG("Found incorrect Sn Order.");
    }
    else 
    {
	PASSMSG("Found correct Sn Order.");
    }
    if( numAngles != n_ang_ref  )
    {
	FAILMSG("Found incorrect number of angles.");
    }
    else 
    {
	PASSMSG("Found correct number of angles.");
    }
    if( qname != qname_ref  )
    {
	cout << qname << endl;
	FAILMSG("Found incorrect name of quadrature set.");
    }
    else 
    {
	PASSMSG("Found correct name of quadrature set.");
    }
    
    // Print a table
    spQuad->display();

    //----------------------------------------
    // Setup QuadServices object
    
    QuadServices qs( spQuad );
    
    vector<double> const M( qs.getM() );
    unsigned const numMoments( qs.getNumMoments() );

    std::vector< unsigned > dims;
    dims.push_back( numAngles );
    dims.push_back( numMoments );
    
    qs.print_matrix( "Mmatrix", M, dims );

    //----------------------------------------
    // For 3D Quadrature we have the following:
    //
    // n maps to the index pair (l,k) via qs.n2kl
    // 
    //                       2*l+1
    // M_{n,m} = M_{l,k,m} = ----- * c_{l,k} * Y_{l,k}( mu_m )
    //                       sumwt
    //
    // Y_n( mu_m ) = P(l=0,k=0)(mu_m)*cos(k*theta)
    //             = P(n,mu_m)
    //----------------------------------------

    std::vector< double > const mu( spQuad->getMu() );

    for( size_t n=0; n<numMoments; ++n )
    { 
	unsigned const ell( qs.lkPair( n ).first  );
	int      const k(   qs.lkPair( n ).second );
	double   const c(   ( 2.0*ell+1.0 ) / sumwt );
	
	if( k == 0 ) 
	{
	    for( size_t m=0; m<numAngles; ++m )
	    {
		
		if( soft_equiv( M[ n + m*numMoments ], 
				c*getclk(ell,k)*P(ell,mu[m]) ) )
		{
		    ostringstream msg;
		    msg << "M[" << n << "," << m 
			<< "] has the expected value." << endl;
		    PASSMSG( msg.str() );
		}
		else
		{		
		    ostringstream msg;
		    msg << "M[" << n << "," << m 
			<< "] does not have the expected value." << endl
			<< "\tFound M[" << n << "," << m << "] = " 
			<< M[ n + m*numMoments ] << ", but was expecting " 
			<< c*getclk(ell,k)*P(ell,mu[m]) << endl; 
		    FAILMSG( msg.str() );		
		}
	    }
	}
    } 

    //-----------------------------------//

    vector<double> const D( qs.getD() );
    qs.print_matrix( "Dmatrix", D, dims );
    
    // The first row of D should contain the quadrature weights.
    {
	unsigned n(0);
	std::vector< double > const wt( spQuad->getWt() );
	for( size_t m=0; m<numAngles; ++m )
	{
	    if( soft_equiv( D[ m + n*numAngles ], wt[m] ) )
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " matched the expected value." << endl;
		PASSMSG( msg.str() );
	    }
	    else
	    {
		ostringstream msg;
		msg << "D[" << m << "," << n << "] = " 
		    << D[ m + n*numAngles ] 
		    << " did not match the expected value of " 
		    << wt[m] << "." << endl;
		FAILMSG( msg.str() );
	    }
	}
    }

    return;
}

//---------------------------------------------------------------------------//

void test_quad_services_alt_constructor()
{
    using rtt_quadrature::QuadCreator;
    using rtt_quadrature::Quadrature;
    using rtt_quadrature::QuadServices;
    using rtt_dsxx::SP;
    using rtt_dsxx::soft_equiv;

    using std::endl;
    using std::vector;
    using std::ostringstream;

    typedef std::pair< unsigned, int > lk_index;

    //----------------------------------------
    // Setup Quadrature set
    
    // we will only look at S2 Sets in this test.
    size_t const snOrder( 2 );

    // Create a quadrature set from a temporary instance of a
    // QuadratureCreator factory object.
    SP< const Quadrature > spQuad;
    spQuad = QuadCreator().quadCreate( QuadCreator::LevelSym, snOrder ); 
    
    // Create a vector that designates the (l,k) moments that will be used
    unsigned const numMoments( spQuad->getNumAngles() );
    unsigned n(0);
    vector< lk_index > lkMoments;
    
    // Copy algorithm from compute_n2lk_3D()
    // -------------------------------------
    // Choose: l= 0, ..., N-1, k = -l, ..., l
    for( unsigned ell=0; ell< snOrder; ++ell )
	for( int k(-1*static_cast<int>(ell)); std::abs(k) <= ell; ++k, ++n )
	    lkMoments.push_back( lk_index(ell,k) );

    // Add ell=N and k<0
    {
	unsigned ell( snOrder );
	for( int k(-1*static_cast<int>(ell)); k<0; ++k, ++n )
	    lkMoments.push_back( lk_index(ell,k) );
    }

    // Add ell=N, k>0, k odd
    {
	unsigned ell( snOrder );
	for( int k=1; k<=ell; k+=2, ++n )
	    lkMoments.push_back( lk_index(ell,k) );
    }

    // Add ell=N+1 and k<0, k even
    {
	unsigned ell( snOrder+1 );
	for( int k(-1*static_cast<int>(ell)+1); k<0; k+=2, ++n )
	    lkMoments.push_back( lk_index(ell,k) );
    }

    //----------------------------------------
    // Setup QuadServices object using alternate constructor.
    
    QuadServices qsStd( spQuad );
    QuadServices qsAlt( spQuad, lkMoments );

    for( unsigned n=0; n<numMoments; ++n )
    {
	lk_index stdIndexValues( qsStd.lkPair(n) );
	lk_index altIndexValues( qsAlt.lkPair(n) );
	if( stdIndexValues.first == altIndexValues.first &&
	    stdIndexValues.second == altIndexValues.second )
	{
	    ostringstream msg;
	    msg << "Alternate Constructor -- lk_index has expected value for moment "
		<< n << "." << endl;
	    PASSMSG(msg.str());
	}
	else
	{
	    ostringstream msg;
	    msg << "Alternate Constructor -- "
		<< "lk_index does not have the expected value for moment "
		<< n << "." << endl
		<< "Found lk_index = (" << altIndexValues.first << ", "
		<< altIndexValues.second << ") but expected (" << stdIndexValues.first
		<< ", " << stdIndexValues.second << ")." << endl;
	    PASSMSG(msg.str());
	}
    }
    return;
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    using std::string;
    using std::cout;
    using std::endl;

    // version tag
    for (int arg = 1; arg < argc; arg++)
	if (string(argv[arg]) == "--version")
	{
	    cout << argv[0] << ": version " << rtt_quadrature::release() 
		 << endl;
	    return 0;
	}

    try
    {
	// >>> UNIT TESTS
	test_kdelta();
	test_factorial();
	test_quad_services_with_1D_S2_quad();
	test_quad_services_with_1D_S8_quad();
	test_quad_services_with_3D_S2_quad();
	test_quad_services_with_2D_S2_quad();
	test_quad_services_alt_constructor();
    }
    catch (rtt_dsxx::assertion &ass)
    {
	cout << "While testing tQuadServices, " << ass.what()
	     << endl;
	return 1;
    }

    // status of test
    cout << endl;
    cout <<     "*********************************************" << endl;
    if (rtt_quadrature_test::passed) 
    {
        cout << "**** tQuadServices Test: PASSED" 
	     << endl;
    }
    cout <<     "*********************************************" << endl;
    cout << endl;

    cout << "Done testing tQuadServices." << endl;

    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tQuadServices.cc
//---------------------------------------------------------------------------//
