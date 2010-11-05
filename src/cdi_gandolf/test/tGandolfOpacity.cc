//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_gandolf/test/tGandolfOpacity.cc
 * \author Thomas M. Evans
 * \date   Fri Oct 26 10:50:44 2001
 * \brief
 * \note   Copyright (C) 2001-2010 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "cdi_gandolf_test.hh"
#include "../Release.hh"
#include "../GandolfFile.hh"
#include "../GandolfException.hh"
#include "../GandolfGrayOpacity.hh"
#include "../GandolfMultigroupOpacity.hh"
#include "cdi/OpacityCommon.hh"
#include "ds++/Assert.hh"
#include "ds++/SP.hh"
#include "ds++/Soft_Equivalence.hh"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

using rtt_cdi_gandolf::GandolfGrayOpacity;
using rtt_cdi_gandolf::GandolfMultigroupOpacity;
using rtt_cdi_gandolf::GandolfFile;
using rtt_cdi::GrayOpacity;
using rtt_cdi::MultigroupOpacity;
using rtt_dsxx::SP;
using rtt_dsxx::soft_equiv;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void file_check_Al_BeCu()
{
    // Gandolf data filename (IPCRESS format required)
    string op_data_file = "Al_BeCu.ipcress";
	    
    // ------------------------- //
    // Create GandolfFile object //
    // ------------------------- //
	    
    SP<GandolfFile> spGFABC;
	    
    // Attempt to instantiate the object.
    try
    {
	spGFABC = new GandolfFile( op_data_file ); 
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
    {
	FAILMSG(GandError.what());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we make it here then spGFABC was successfully instantiated.
    PASSMSG("SP to new GandolfFile object created for Al_BeCu.ipcress data.");
	    
    // Test the GandolfFile object.
    if ( spGFABC->getDataFilename() == op_data_file )
    {
	ostringstream message;
	message << "GandolfFile object is now linked to the "
		<< "Al_BeCu.ipcress data file.";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << "GandolfFile object failed to link itself to the "
		<< "Al_BeCu.ipcress  data file.";
	FAILMSG(message.str());
    }
	    
    if ( spGFABC->getNumMaterials() == 2 )
    {
	ostringstream message;
	message << "The correct number of materials was found in the "
		<< "Al_BeCu.ipcress data file.";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << "spGFABC did not find the correct number of materials "
		<< "in the Al_BeCu.ipcress data file.";
	FAILMSG(message.str());
    }
	    
    // ---------------------- //
    // Create Opacity object. //
    // ---------------------- //
	    
    // Material identifier.  This data file has two materials: Al and
    // BeCu.  Al has the id tag "10001".
    const int matid=10001;
	    
    // Try to instantiate the Opacity object. (Rosseland, Gray Total
    // for material 10001 in the IPCRESS file pointed to by spGFABC).
    SP<GrayOpacity> spOp_Al_rgt;
	    
    try
    {
	spOp_Al_rgt = new GandolfGrayOpacity( 
	    spGFABC, matid, rtt_cdi::ROSSELAND, rtt_cdi::TOTAL );
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
	// Alternatively, we could use:
	// catch ( rtt_cdi_gandolf::gkeysException GandError )
	// catch ( rtt_cdi_gandolf::gchgridsException GandError )
	// catch ( rtt_cdi_gandolf::ggetmgException GandError )
	// catch ( rtt_cdi_gandolf::ggetgrayException GandError )
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "Al_BeCu.ipcress data."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
    
    // If we get here then the object was successfully instantiated.
    PASSMSG("SP to new Opacity object created for Al_BeCu.ipcress data.");
	    
    // ----------------- //
    // Gray Opacity Test //
    // ----------------- //
	    
    double temperature          = 0.1; // keV
    double density              = 27.0; // g/cm^3
    double tabulatedGrayOpacity = 4271.7041147070677; // cm^2/g
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Al_rgt, temperature, density, tabulatedGrayOpacity ) )
    {
	FAILMSG("Aborting tests.")
	return;
    }

    // Check accessor functions

    rtt_cdi::OpacityModelType omt( spOp_Al_rgt->getOpacityModelType() );
    if( omt == rtt_cdi::GANDOLF_TYPE )
        PASSMSG( "OpacityModelType() returned expected value.")
    else
        FAILMSG( "OpacityModelType() did not return the expected value.");

    std::string edp( spOp_Al_rgt->getEnergyPolicyDescriptor() );
    if( edp == std::string("gray") )
        PASSMSG( "EDP = gray" )
    else
        FAILMSG( "EDP != gray" );

    if( ! spOp_Al_rgt->data_in_tabular_form() ) ITFAILS;
    
    size_t nd( spOp_Al_rgt->getNumDensities() );
    size_t nt( spOp_Al_rgt->getNumTemperatures() );
    if( nd != 5  ) FAILMSG( "Found wrong number of density values." );
    if( nt != 10 ) FAILMSG( "Found wrong number of temperature values." );
    
    std::vector<double> densGrid( spOp_Al_rgt->getDensityGrid() );
    std::vector<double> tempGrid( spOp_Al_rgt->getTemperatureGrid() );
    if( densGrid.size() != nd ) ITFAILS;
    if( tempGrid.size() != nt ) ITFAILS;

    double expected_densGrid[] = { 0.01, 0.1, 1.0, 10.0, 100.0 };
    double expected_tempGrid[] = { 0.0005, 0.0015, 0.004, 0.0125, 0.04,
                                   0.125, 0.4, 1.25, 4, 15 };

    for( size_t i=0; i<densGrid.size(); ++i )
        if( ! soft_equiv( densGrid[i], expected_densGrid[i] ) ) ITFAILS;
    for( size_t i=0; i<tempGrid.size(); ++i )
        if( ! soft_equiv( tempGrid[i], expected_tempGrid[i] ) ) ITFAILS;
            
            
    // --------------- //
    // MG Opacity test //
    // --------------- //
	    
    // Create a Multigroup Rosseland Total Opacity object (again for Al).
    SP<MultigroupOpacity> spOp_Al_rtmg;
	    
    // Try to instantiate the Opacity object.
    try
    {
	spOp_Al_rtmg = new GandolfMultigroupOpacity( 
	    spGFABC, matid, rtt_cdi::ROSSELAND, rtt_cdi::TOTAL );
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "Al_BeCu.ipcress data."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // Setup the test point.
    temperature = 0.01; // keV
    density     = 2.0; // g/cm^3

    // Check accessor functions
    
    omt = spOp_Al_rtmg->getOpacityModelType();
    if( omt == rtt_cdi::GANDOLF_TYPE )
        PASSMSG( "OpacityModelType() returned expected value.")
    else
        FAILMSG( "OpacityModelType() did not return the expected value.");

    edp = spOp_Al_rtmg->getEnergyPolicyDescriptor();
    if( edp == std::string("mg") )
        PASSMSG( "EDP = mg" )
    else
        FAILMSG( "EDP != mg" );

    if( ! spOp_Al_rtmg->data_in_tabular_form() ) ITFAILS;

    size_t numGroups = 33;
    if( spOp_Al_rtmg->getNumGroups() != numGroups ) ITFAILS;

    // The solution to compare against:
    double tabulatedMGOpacityArray[] = {2.4935245299837247e+08,
					2.6666789027326573e+04,
					1.6270621515227660e+04,
					1.7634711671468522e+04,
					4.4999455359684442e+04,
					9.9917674335613032e+04,
					8.3261383385464113e+04,
					5.9742975304886764e+04,
					4.0373209722602740e+04,
					2.6156503146710609e+04,
					1.6356701105166874e+04,
					1.0007184686170869e+04,
					5.9763667878215247e+03,
					3.5203912630050986e+03,
					2.0765528559140448e+03,
					6.8550529299142445e+03,
					4.1257095227186965e+03,
					2.4199006949490426e+03,
					1.3894677080938793e+03,
					7.9046985091966621e+02,
					4.4088463936537232e+02,
					2.4514360684176387e+02,
					1.3541656611912146e+02,
					7.1828886317050177e+01,
					3.9793827527329107e+01,
					2.3312673181867030e+01,
					1.4879458895157605e+01,
					1.0862672679200283e+01,
					9.0590676798691288e+00,
					8.2841367649864175e+00,
					7.3809286930540363e+00,
					7.1057875403123791e+00,
					6.8907716134926735e+00
    }; // KeV, numGroups entries.
	    
    vector< double > tabulatedMGOpacity( numGroups );
    copy( tabulatedMGOpacityArray, tabulatedMGOpacityArray+numGroups,
	  tabulatedMGOpacity.begin() );    
    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Al_rtmg, temperature, density, tabulatedMGOpacity ) )
    {
	FAILMSG("Aborting tests.")
	return;
    }
}

//---------------------------------------------------------------------------//

void file_check_analytic()
{
    // ----------------------------------------------------------------
    // The Opacities in this file are computed from the following
    // analytic formula:
    //     opacity = rho * T^4,
    // rho is the density and T is the temperature.
    //
    // The grid in this data file has the following structure:
    //    T   = { 0.1, 1.0, 10.0 } keV.
    //    rho = { 0.1, 0.5, 1.0 } g/cm^3
    //    E_bounds = { 0.01, 0.03, 0.07, 0.1, 0.3, 0.7, 1.0, 3.0, 7.0 
    //                 10.0, 30.0, 70.0 100.0 } keV.
    //-----------------------------------------------------------------
    
    // Gandolf data filename (IPCRESS format required)
    string op_data_file = "analyticOpacities.ipcress";
	    
    // ------------------------- //
    // Create GandolfFile object //
    // ------------------------- //
	    
    // Create a smart pointer to a GandolfFile object
    SP<GandolfFile> spGFAnalytic;
	    
    // Try to instantiate the object.
    try 
    {
	spGFAnalytic = new rtt_cdi_gandolf::GandolfFile( op_data_file ); 
    }
    catch (const rtt_cdi_gandolf::gmatidsException& GandError)
    {
	ostringstream message;
	FAILMSG(GandError.what());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we make it here then spGFAnalytic was successfully instantiated.
    PASSMSG("SP to new GandolfFile object created (spGFAnalytic).");
	    
    // Test the GandolfFile object.
    if ( spGFAnalytic->getDataFilename() == op_data_file )
    {
	ostringstream message;
	message << "GandolfFile object is now linked to the data file.";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << "GandolfFile object failed to link itself "
		<< "to the data file.";
	FAILMSG(message.str());
    }
	    
    if ( spGFAnalytic->getNumMaterials() == 1 )
    {
	ostringstream message;
	message << "The correct number of materials was found "
		<< "in the data file.";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;	
	message << "spGFAnalytic did not find the correct number "
		<< "of materials in the data file.";
	FAILMSG(message.str());
    }

    // --------------------- //
    // Create Opacity object //
    // --------------------- //
	    
    // Create a smart pointer to an Opacity object.
    SP<GrayOpacity> spOp_Analytic_ragray;

    // material ID
    const int matid=10001;
	    
    // Try to instantiate the Opacity object.
    try 
    {
	spOp_Analytic_ragray = new GandolfGrayOpacity( 
	    spGFAnalytic, matid, rtt_cdi::ROSSELAND, rtt_cdi::ABSORPTION );
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
	// Alternatively, we could use:
	// catch ( rtt_cdi_gandolf::gkeysException GandError )
	// catch ( rtt_cdi_gandolf::gchgridsException GandError )
	// catch ( rtt_cdi_gandolf::ggetmgException GandError )
	// catch ( rtt_cdi_gandolf::ggetgrayException GandError )
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "analyticOpacities.ipcress data."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we get here then the object was successfully instantiated.
    PASSMSG("SP to new Opacity object created for analyticOpacities.ipcress.");
	    
    // ----------------- //
    // Gray Opacity Test //
    // ----------------- //
	    
    double temperature          = 10.0; // keV
    double density              = 1.0; // g/cm^3
    double tabulatedGrayOpacity = density * pow( temperature, 4 );
	    
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    spOp_Analytic_ragray, temperature, density, tabulatedGrayOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }	
	    
    //---------------- //
    // MG Opacity test //
    //---------------- //
	    
    // Create a smart pointer to an Opacity object.
    SP<MultigroupOpacity> spOp_Analytic_ramg;
	    
    // Try to instantiate the Opacity object.
    try 
    { 
	spOp_Analytic_ramg = new GandolfMultigroupOpacity( 
	    spGFAnalytic, matid, rtt_cdi::ROSSELAND, rtt_cdi::ABSORPTION );
    } 
    catch ( const rtt_cdi_gandolf::GandolfException& GandError ) 
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "analyticOpacities.ipcress data." << std::endl << "\t" 
		<< GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we get here then the object was successfully instantiated.
    PASSMSG("SP to new Opacity object created for analyticOpacities.ipcress.");
	    
    // Set up the new test problem.
	    
    temperature = 0.3; // keV
    density     = 0.7; // g/cm^3
	    
    // This is the solution we compare against.   
    int numGroups = 12; 
    vector< double > tabulatedMGOpacity( numGroups );
    for ( int i=0; i<numGroups; ++i )
	tabulatedMGOpacity[i] = density * pow( temperature, 4 ); // cm^2/gm
	    
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    spOp_Analytic_ramg, temperature, density, tabulatedMGOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------------------------------------------------ //
    // Test the Plank routines using analyticOpacities.ipcress data //
    // ------------------------------------------------------------ //
	    
    // The Opacities in this file are computed from the following
    // analytic formula:
    //     opacity = rho * T^4,
    // rho is the density and T is the temperature.
	    
    // spGFAnalytic already points to the correct file so we don't repeat the 
    // coding.
	    
    // Dito for spOpacityAnalytic.
	    
    // ----------------- //
    // Gray Opacity Test //
    // ----------------- //
	    
    // Create a smart pointer to an Opacity object.
    SP<GrayOpacity> spOp_Analytic_pgray;
	    
    // Try to instantiate the Opacity object.
    try 
    { 
	spOp_Analytic_pgray = new GandolfGrayOpacity( 
	    spGFAnalytic, matid, rtt_cdi::PLANCK, rtt_cdi::ABSORPTION);
    } 
    catch ( const rtt_cdi_gandolf::GandolfException& GandError ) 
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "analyticOpacities.ipcress data." << std::endl << "\t"
		<< GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we get here then the object was successfully instantiated.
    {
	ostringstream message;
	message << "SP to new Gray Plank Total Opacity object "
		<< "created for analyticOpacities.ipcress.";
	PASSMSG(message.str());
    }
	    
    // Setup the test problem.
	    
    temperature           = 3.0; // keV
    density               = 0.7; // g/cm^3
    double tabulatedValue = density * pow( temperature, 4 ); // cm^2/g
	    
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    spOp_Analytic_pgray, temperature, density, tabulatedValue))
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // --------------- //
    // MG Opacity test //
    // --------------- //
	    
    // Create a smart pointer to an Opacity object.
    SP<MultigroupOpacity> spOp_Analytic_pmg;
	    
    // Try to instantiate the Opacity object.
    try 
    { 
	spOp_Analytic_pmg = new GandolfMultigroupOpacity( 
	    spGFAnalytic, matid, rtt_cdi::PLANCK, rtt_cdi::ABSORPTION); 
    } 
    catch ( const rtt_cdi_gandolf::GandolfException& GandError ) 
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfOpacity object for "
		<< "analyticOpacities.ipcress data." << std::endl << "\t" 
		<< GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // If we get here then the object was successfully instantiated.
    {
	ostringstream message;
	message << "SP to new Multigroup Plank Total Opacity object "
		<< "created \n\t for \"analyticOpacities.ipcress.\"";
	PASSMSG(message.str());
    }
	    
    // Setup the test problem.
	    
    size_t ng = 12;
    tabulatedMGOpacity.resize( ng );
    temperature = 0.4; // keV
    density     = 0.22; // g/cm^3
    for ( size_t ig=0; ig<ng; ++ig )
	tabulatedMGOpacity[ig] = density * pow( temperature, 4 ); // cm^2/g
	    
    // If this test fails then stop testing.
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    spOp_Analytic_pmg, temperature, density, tabulatedMGOpacity)) 
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------------ //
    // Access temperature grid. //
    // ------------------------ //
	    
    rtt_cdi_gandolf_test::testTemperatureGridAccessor( spOp_Analytic_pmg );
	    
    // ------------------------ //
    // Access the density grid. //
    // ------------------------ //
	    
    rtt_cdi_gandolf_test::testDensityGridAccessor( spOp_Analytic_pmg );
	    
    // ----------------------------- //
    // Access the energy boundaries. //
    // ----------------------------- //
	    
    rtt_cdi_gandolf_test::testEnergyBoundaryAccessor( spOp_Analytic_pmg );

    // ------------------------------------------------------------ //
    // Test alternate (vector-based) accessors for getGrayRosseland //
    // ------------------------------------------------------------ //
	    
    // ---------------------- //
    // Vector of temperatures //
    // ---------------------- //
	    
    std::vector<double> vtemperature(2);
    vtemperature[0] = 0.5; // keV
    vtemperature[1] = 0.7; // keV
    density         = 0.35; // g/cm^3
	    
    std::vector<double> vtabulatedGrayOpacity( vtemperature.size() );
    for ( size_t i=0; i< vtabulatedGrayOpacity.size(); ++i )
	vtabulatedGrayOpacity[i] = density * pow ( vtemperature[i], 4 );
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_ragray, vtemperature, density,
	     vtabulatedGrayOpacity ) )
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ---------------------- //
    // Vector of densities    //
    // ---------------------- //
	    
    std::vector<double> vdensity(3);
    temperature = 0.3; //keV
    vdensity[0] = 0.2; // g/cm^3
    vdensity[1] = 0.4; // g/cm^3
    vdensity[2] = 0.6; // g/cm^3
	    
    vtabulatedGrayOpacity.resize( vdensity.size() );
    for ( size_t i=0; i< vtabulatedGrayOpacity.size(); ++i )
	vtabulatedGrayOpacity[i] = vdensity[i] * pow ( temperature, 4 );
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_ragray, temperature, vdensity,
	     vtabulatedGrayOpacity ) )
    {
	FAILMSG("Aborting tests.");
	return;
    }

    // -------------------------------------------------------- //
    // Test alternate (vector-based) accessors for getGrayPlank //
    // -------------------------------------------------------- //
	    
    // ---------------------- //
    // Vector of temperatures //
    // ---------------------- //
	    
    vtemperature.resize(2);
    vtemperature[0] = 0.5; // keV
    vtemperature[1] = 0.7; // keV
    density         = 0.35; // g/cm^3q
	    
    vtabulatedGrayOpacity.resize( vtemperature.size() );
    for ( size_t i=0; i< vtabulatedGrayOpacity.size(); ++i )
	vtabulatedGrayOpacity[i] = density * pow ( vtemperature[i], 4 );
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_pgray, vtemperature, density,
	     vtabulatedGrayOpacity ) )
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------- //
    // Vector of densities //
    // ------------------- //
	    
    vdensity.resize(3);
    temperature = 0.3; //keV
    vdensity[0] = 0.2; // g/cm^3
    vdensity[1] = 0.4; // g/cm^3
    vdensity[2] = 0.6; // g/cm^3
	    
    vtabulatedGrayOpacity.resize( vdensity.size() );
    for ( size_t i=0; i< vtabulatedGrayOpacity.size(); ++i )
	vtabulatedGrayOpacity[i] = vdensity[i] * pow ( temperature, 4 );
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_pgray, temperature, vdensity,
	     vtabulatedGrayOpacity  ) )
    {
	FAILMSG("Aborting tests.");
	return;
    }

    // ---------------------------------------------------------- //
    // Test alternate (vector-based) accessors for getMGRosseland //
    // ---------------------------------------------------------- //
	    
    // ---------------------- //
    // Vector of temperatures //
    // ---------------------- //
	    
    vtemperature.resize(2);
    vtemperature[0] = 0.5;  // keV
    vtemperature[1] = 0.7;  // keV
    density         = 0.35; // g/cm^3
    ng              = spOp_Analytic_ramg->getNumGroupBoundaries() - 1;
	    
    vector< vector< double > > vtabulatedMGOpacity( vtemperature.size() );
    for ( size_t i=0; i< vtemperature.size(); ++i )
    {
	vtabulatedMGOpacity[i].resize(ng);
	for (size_t ig=0; ig<ng; ++ig )
	    vtabulatedMGOpacity[i][ig] = 
		density * pow ( vtemperature[i], 4 );
    }
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed(
	     spOp_Analytic_ramg, vtemperature, density, vtabulatedMGOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------- //
    // Vector of densities //
    // ------------------- //
	    
    vdensity.resize(2);
    vdensity[0] = 0.3; // g/cm^3
    vdensity[1] = 0.7; // g/cm^3
    temperature = 7.0; // keV
    ng          = spOp_Analytic_ramg->getNumGroupBoundaries() - 1;
	    
    vtabulatedMGOpacity.resize( vdensity.size() );
    for ( size_t i=0; i<vdensity.size(); ++i )
    {
	vtabulatedMGOpacity[i].resize(ng);
	for ( size_t ig=0; ig<ng; ++ig )
	    vtabulatedMGOpacity[i][ig] = 
		vdensity[i] * pow ( temperature, 4 );
    }
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_ramg, temperature, vdensity, vtabulatedMGOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }

	    
    // ------------------------------------------------------ //
    // Test alternate (vector-based) accessors for getMGPlank //
    // ------------------------------------------------------ //
	    
    // ---------------------- //
    // Vector of temperatures //
    // ---------------------- //
	    
    vtemperature.resize(2);
    vtemperature[0] = 0.5;  // keV
    vtemperature[1] = 0.7;  // keV
    density         = 0.35; // g/cm^3
    ng              = spOp_Analytic_pmg->getNumGroupBoundaries() - 1;
	    
    vtabulatedMGOpacity.resize( vtemperature.size() );
    for ( size_t i=0; i< vtemperature.size(); ++i )
    {
	vtabulatedMGOpacity[i].resize(ng);
	for ( size_t ig=0; ig<ng; ++ig )
	    vtabulatedMGOpacity[ i ][ ig ] = 
		density * pow ( vtemperature[i], 4 );
    }
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_pmg, vtemperature, density, vtabulatedMGOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------- //
    // Vector of densities //
    // ------------------- //
	    
    vdensity.resize(2);
    vdensity[0] = 0.3; // g/cm^3
    vdensity[1] = 0.7; // g/cm^3
    temperature = 7.0; // keV
    ng          = spOp_Analytic_pmg->getNumGroupBoundaries() - 1;
	    
    vtabulatedMGOpacity.resize( vdensity.size() );
    for ( size_t i=0; i<vdensity.size(); ++i )
    {
	vtabulatedMGOpacity[i].resize(ng);
	for ( size_t ig=0; ig<ng; ++ig )
	    vtabulatedMGOpacity[ i ][ ig ] = 
		vdensity[i] * pow ( temperature, 4 );
    }
	    
    if ( ! rtt_cdi_gandolf_test::opacityAccessorPassed( 
	     spOp_Analytic_pmg, temperature, vdensity, vtabulatedMGOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }	    
}

//---------------------------------------------------------------------------//

void check_gandolf_stl_accessors()
{
    // Gandolf data filename (IPCRESS format required)
    string op_data_file = "analyticOpacities.ipcress";
	    
    // ------------------------- //
    // Create GandolfFile object //
    // ------------------------- //
	    
    // Create a smart pointer to a GandolfFile object
    SP<GandolfFile> spGFAnalytic;
	    
    // Try to instantiate the object.
    try 
    {
	spGFAnalytic = new rtt_cdi_gandolf::GandolfFile( op_data_file ); 
    }
    catch (const rtt_cdi_gandolf::gmatidsException& GandError)
    {
	ostringstream message;
	FAILMSG(GandError.what());
	FAILMSG("Aborting tests.");
	return;
    }

    // material ID
    const int matid=10001;
	    
    // -------------------------------------- //
    // Test the STL-like getOpacity accessor  //
    // Using const iterators for Gray objects //
    // -------------------------------------- //
	    
    // These accessors are only available in GandolfOpacity objects
    // so the SP must be templated on GandolfGrayOpacity and not on
    // cdi/GrayOpacity. 
	    
    // Create a new smart pointer to a GandolfGrayOpacity object.
    SP<GandolfGrayOpacity > spGGOp_Analytic_ra;    

    // try to instantiate the Opacity object.
    try
    {
	spGGOp_Analytic_ra = new GandolfGrayOpacity(
	    spGFAnalytic, matid, rtt_cdi::ROSSELAND, rtt_cdi::ABSORPTION );
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfGrayOpacity object for "
		<< "\n\t analyticOpacityies.ipcress data (SP not templated on "
		<< "cdi/GrayOpacity)." << std::endl << "\t" 
		<< GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }
    
    vector<double> vdensity;
    vector<double> vtemperature;
    vector<double> vtabulatedGrayOpacity;

    double density     = 0.35; // g/cm^3
    double temperature = 7.0;  // kev

    // Setup the temperature and density parameters for this test.
    vdensity.resize(6);
    vtemperature.resize(6);
	    
    // (temperature,density) tuples.
	    
    vtemperature[0] = 0.5; // keV
    vdensity[0] = 0.2; // g/cm^3
	    
    vtemperature[1] = 0.7; // keV
    vdensity[1] = 0.2; // g/cm^3
	    
    vtemperature[2] = 0.5; // keV
    vdensity[2] = 0.4; // g/cm^3
	    
    vtemperature[3] = 0.7; // keV
    vdensity[3] = 0.4; // g/cm^3
	    
    vtemperature[4] = 0.5; // keV
    vdensity[4] = 0.6; // g/cm^3
	    
    vtemperature[5] = 0.7; // keV
    vdensity[5] = 0.6; // g/cm^3
	    
    // we want to test the const_iterator version of getOpacity() so
    // we need to create const vectors with the tuple data.
    const std::vector<double> cvdensity = vdensity;
    const std::vector<double> cvtemperature = vtemperature;
    
    int nt = cvtemperature.size();
    int nd = cvdensity.size();
	    
    // Here is the reference solution
    vtabulatedGrayOpacity.resize( nt ); 
    for ( int i=0; i<nt; ++i )
	vtabulatedGrayOpacity[i] = 
	    cvdensity[i] * pow ( cvtemperature[i], 4 );
	    
    // Here is the solution from Gandolf
    std::vector< double > graOp(nt);
    spGGOp_Analytic_ra->getOpacity( cvtemperature.begin(),
				    cvtemperature.end(), 
				    cvdensity.begin(),
				    cvdensity.end(), 
				    graOp.begin() );
    if ( rtt_cdi_gandolf_test::match( graOp, vtabulatedGrayOpacity ) )
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (const-iterator accessor, temp x density).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	FAILMSG(message.str());
    }
	    
    // ------------------------------------- //
    // Test the STL-like getOpacity accessor //
    // Using non-const iterator              //
    // ------------------------------------- //
	    
    spGGOp_Analytic_ra->getOpacity( vtemperature.begin(),
				    vtemperature.end(), 
				    vdensity.begin(),
				    vdensity.end(), 
				    graOp.begin() );
    if ( rtt_cdi_gandolf_test::match( graOp, vtabulatedGrayOpacity ) )
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	FAILMSG(message.str());
    }

    // ------------------------------------- //
    // Test the STL-like getOpacity accessor //
    // const iterator (temperature only)     //
    // ------------------------------------- //
	    
    graOp.resize( nt );
    vtabulatedGrayOpacity.resize( nt );
    for ( int it=0; it<nt; ++it )
	vtabulatedGrayOpacity[it] = density * pow( vtemperature[it], 4 );
	    
    spGGOp_Analytic_ra->getOpacity( cvtemperature.begin(),
				    cvtemperature.end(), 
				    density,
				    graOp.begin() );
    if ( rtt_cdi_gandolf_test::match( graOp, vtabulatedGrayOpacity ) )
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vtemps).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vtemps).";
	FAILMSG(message.str());
    }
	    
    // ------------------------------------- //
    // Test the STL-like getOpacity accessor //
    // const iterator ( density only)        //
    // ------------------------------------- //
	    
    graOp.resize( nd );
    vtabulatedGrayOpacity.resize( nd );
    for ( int id=0; id<nd; ++id )
	vtabulatedGrayOpacity[id] = vdensity[id] * pow( temperature, 4 );
	    
    spGGOp_Analytic_ra->getOpacity( temperature,
				    cvdensity.begin(),
				    cvdensity.end(), 
				    graOp.begin() );
    if ( rtt_cdi_gandolf_test::match( graOp, vtabulatedGrayOpacity ) )
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vdensity).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vdensity).";
	FAILMSG(message.str());
    }	    

    // -------------------------------------- //
    // Test the STL-like getOpacity accessor  //
    // Using const iterators for MG objects   //
    // -------------------------------------- //
	    
    // These accessors are only available in GandolfOpacity objects
    // so the SP must be templated on GandolfMultigroupOpacity and not on
    // cdi/MultigroupOpacity. 
	    
    // Create a new smart pointer to a GandolfGrayOpacity object.
    SP<GandolfMultigroupOpacity > spGMGOp_Analytic_ra;
	    
    // try to instantiate the Opacity object.
    try
    {
	spGMGOp_Analytic_ra = new GandolfMultigroupOpacity(
	    spGFAnalytic, matid, rtt_cdi::ROSSELAND, rtt_cdi::ABSORPTION);
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
    {
	ostringstream message;
	message << "Failed to create SP to new GandolfGrayOpacity "
		<< "object for \n\t analyticOpacities.ipcress data "
		<< "(SP not templated on cdi/GrayOpacity)."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str()); 
	FAILMSG("Aborting tests."); 
	return; 
    }

    // If we get here then the object was successfully instantiated.
    PASSMSG("SP to new Opacity object created for analyticOpacities.ipcress.");
	    
    // Here is the reference solution
    int ng = spGMGOp_Analytic_ra->getNumGroupBoundaries() - 1;
    std::vector< double > vtabulatedOpacity( ng * nt );
	    
    for ( int i=0; i<nt; ++i )
	for ( int ig=0; ig<ng; ++ig )
	    vtabulatedOpacity[ i*ng + ig ] = 
		cvdensity[i] * pow ( cvtemperature[i], 4 );
	    
    // Here is the solution from Gandolf
    std::vector< double > mgOp( nt*ng );
    spGMGOp_Analytic_ra->getOpacity( cvtemperature.begin(),
				     cvtemperature.end(), 
				     cvdensity.begin(),
				     cvdensity.end(), 
				     mgOp.begin() );
	    
    if ( rtt_cdi_gandolf_test::match( mgOp, vtabulatedOpacity ) )
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (const-iterator accessor, temp x density).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	FAILMSG(message.str());
    }
	    
    // --------------------------------------- //
    // Test the STL-like getOpacity accessor   //
    // Using non-const iterator for MG objects //
    // --------------------------------------- //
	    
    // clear old data
    for ( int i=0; i<nt*ng; ++i) mgOp[i]=0.0;
	    
    // use Gandolf to obtain new data
    spGMGOp_Analytic_ra->getOpacity( vtemperature.begin(),
				     vtemperature.end(), 
				     vdensity.begin(),
				     vdensity.end(), 
				     mgOp.begin() );
	    
    // compare the results to the reference solution and report our
    // findings. 
    if ( rtt_cdi_gandolf_test::match( mgOp, vtabulatedOpacity ) )
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (non-const-iterator accessor, temp x density).";
	FAILMSG(message.str());
    }
	    
    // ------------------------------------------------ //
    // Test the STL-like getOpacity accessor            //
    // const iterator (temperature only) for MG data    //
    // ------------------------------------------------ //
	    
    // clear old data
    for ( int i=0; i<nt*ng; ++i) mgOp[i]=0.0;
	    
    // Calculate the reference solution.
    for ( int it=0; it<nt; ++it )
	for ( int ig=0; ig<ng; ++ig )
	    vtabulatedOpacity[ it*ng + ig ] 
		= density * pow( vtemperature[it], 4 );
	    
    // Obtain new solution
    spGMGOp_Analytic_ra->getOpacity( cvtemperature.begin(),
				     cvtemperature.end(), 
				     density,
				     mgOp.begin() );
	    
    // Compare solutions and report the results.
    if ( rtt_cdi_gandolf_test::match( mgOp, vtabulatedOpacity ) )
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vtemps).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vtemps).";
	FAILMSG(message.str());
    }
	    
	    
    // ------------------------------------------ //
    // Test the STL-like getOpacity accessor      //
    // const iterator ( density only) for MG data //
    // ------------------------------------------ //
	    
    // clear old data
    for ( int i=0; i<nd*ng; ++i) mgOp[i]=0.0;
	    
    // Calculate the reference solution.
    for ( int id=0; id<nd; ++id )
	for ( int ig=0; ig<ng; ++ig )
	    vtabulatedOpacity[ id*ng + ig ] 
		= vdensity[id] * pow( temperature, 4 );
	    
    // Obtain new solution
    spGMGOp_Analytic_ra->getOpacity( temperature,
				     cvdensity.begin(),
				     cvdensity.end(), 
				     mgOp.begin() );
	    
    // Compare solutions and report the results.
    if ( rtt_cdi_gandolf_test::match( mgOp, vtabulatedOpacity ) )
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity computation was good for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vdensity).";
	PASSMSG(message.str());
    }
    else
    {
	ostringstream message;
	message << spGMGOp_Analytic_ra->getDataDescriptor()
		<< " opacity value is out of spec. for \n\t"
		<< spGMGOp_Analytic_ra->getDataFilename()
		<< " (const iterator accessor, vdensity).";
	FAILMSG(message.str());
    }
}

//---------------------------------------------------------------------------//

void gray_opacity_packing_test()
{   
    vector<char> packed;

    {
	// Gandolf data filename (IPCRESS format required)
	string op_data_file = "analyticOpacities.ipcress";

	// ------------------------- //
	// Create GandolfFile object //
	// ------------------------- //
	    
	// Create a smart pointer to a GandolfFile object
	SP<GandolfFile> spGFAnalytic;
	    
	// Try to instantiate the object.
	try 
	{
	    spGFAnalytic = new rtt_cdi_gandolf::GandolfFile( op_data_file ); 
	}
	catch (const rtt_cdi_gandolf::gmatidsException& GandError)
	{
	    ostringstream message;
	    FAILMSG(GandError.what());
	    FAILMSG("Aborting tests.");
	    return;
	}

	// Create a smart pointer to an Opacity object.
	SP<GrayOpacity> spOp_Analytic_ragray;

	// material ID
	const int matid=10001;
    
	spOp_Analytic_ragray = new GandolfGrayOpacity( 
	    spGFAnalytic, matid, rtt_cdi::ROSSELAND, rtt_cdi::ABSORPTION );

	// pack up the opacity
	packed = spOp_Analytic_ragray->pack();
    }

    // make a new GandolfGrayOpacity from packed data
    SP<GrayOpacity> unpacked_opacity;  

    // Try to instantiate the Opacity object.
    try 
    {
	unpacked_opacity = new GandolfGrayOpacity(packed); 
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
	// Alternatively, we could use:
	// catch ( rtt_cdi_gandolf::gkeysException GandError )
	// catch ( rtt_cdi_gandolf::gchgridsException GandError )
	// catch ( rtt_cdi_gandolf::ggetmgException GandError )
	// catch ( rtt_cdi_gandolf::ggetgrayException GandError )
    {
	ostringstream message;
	message << "Failed to create SP to unpacked GandolfOpacity object for "
		<< "analyticOpacities.ipcress data."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }

    // some simple tests
    if (unpacked_opacity->getDataFilename() != "analyticOpacities.ipcress")
	ITFAILS;

    if (unpacked_opacity->getReactionType() != rtt_cdi::ABSORPTION) ITFAILS;
    if (unpacked_opacity->getModelType() != rtt_cdi::ROSSELAND)     ITFAILS;
	    
    // ----------------- //
    // Gray Opacity Test //
    // ----------------- //
	    
    double temperature          = 10.0; // keV
    double density              = 1.0; // g/cm^3
    double tabulatedGrayOpacity = density * pow( temperature, 4 );
	    
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    unpacked_opacity, temperature, density, tabulatedGrayOpacity))
    {
	FAILMSG("Aborting tests.");
	return;
    }	

    // try to unpack gray opacity as multigroup opacity
    bool caught = false;
    try
    {
	SP<MultigroupOpacity> opacity(new GandolfMultigroupOpacity(packed));
    }
    catch (const rtt_dsxx::assertion &error)
    {
	caught = true;
	ostringstream message;
	message << "Good, we caught the following assertion, \n"
		<< error.what();
	PASSMSG(message.str());
    }
    if (!caught)
    {
	FAILMSG("Failed to catch an illegal packing asserion.");
    }

    if (rtt_cdi_gandolf_test::passed)
    {
	PASSMSG("GandolfGrayOpacity packing test successfull.");
    }
}

//---------------------------------------------------------------------------//

void mg_opacity_packing_test()
{
    vector<char> packed;

    {
	// Gandolf data filename (IPCRESS format required)
	string op_data_file = "analyticOpacities.ipcress";
	
	// ------------------------- //
	// Create GandolfFile object //
	// ------------------------- //
	    
	// Create a smart pointer to a GandolfFile object
	SP<GandolfFile> spGFAnalytic;
	    
	// Try to instantiate the object.
	try 
	{
	    spGFAnalytic = new rtt_cdi_gandolf::GandolfFile( op_data_file ); 
	}
	catch (const rtt_cdi_gandolf::gmatidsException& GandError)
	{
	    ostringstream message;
	    FAILMSG(GandError.what());
	    FAILMSG("Aborting tests.");
	    return;
	}

	// material ID
	const int matid=10001;

	//---------------- //
	// MG Opacity test //
	//---------------- //
	    
	// Create a smart pointer to an Opacity object.
	SP<MultigroupOpacity> spOp_Analytic_pmg;
	
	spOp_Analytic_pmg = new GandolfMultigroupOpacity( 
	    spGFAnalytic, matid, rtt_cdi::PLANCK, rtt_cdi::ABSORPTION );

	packed = spOp_Analytic_pmg->pack();
    }

    // make a new GandolfGrayOpacity from packed data
    SP<MultigroupOpacity> unpacked_opacity;  

    // Try to instantiate the Opacity object.
    try 
    {
	unpacked_opacity = new GandolfMultigroupOpacity(packed); 
    }
    catch ( const rtt_cdi_gandolf::GandolfException& GandError )
	// Alternatively, we could use:
	// catch ( rtt_cdi_gandolf::gkeysException GandError )
	// catch ( rtt_cdi_gandolf::gchgridsException GandError )
	// catch ( rtt_cdi_gandolf::ggetmgException GandError )
	// catch ( rtt_cdi_gandolf::ggetgrayException GandError )
    {
	ostringstream message;
	message << "Failed to create SP to unpacked GandolfOpacity object for "
		<< "analyticOpacities.ipcress data."
		<< std::endl << "\t" << GandError.what();
	FAILMSG(message.str());
	FAILMSG("Aborting tests.");
	return;
    }

    // some simple tests
    if (unpacked_opacity->getDataFilename() != "analyticOpacities.ipcress")
	ITFAILS;

    if (unpacked_opacity->getReactionType() != rtt_cdi::ABSORPTION) ITFAILS;
    if (unpacked_opacity->getModelType() != rtt_cdi::PLANCK)        ITFAILS;

    // Setup the test problem.
	    
    int ng             = 12;
    vector<double> tabulatedMGOpacity( ng );
    double temperature = 0.4;  // keV
    double density     = 0.22; // g/cm^3
    for ( int ig=0; ig<ng; ++ig )
	tabulatedMGOpacity[ig] = density * pow( temperature, 4 ); // cm^2/g
	    
    // If this test fails then stop testing.
    if (!rtt_cdi_gandolf_test::opacityAccessorPassed( 
	    unpacked_opacity, temperature, density, tabulatedMGOpacity)) 
    {
	FAILMSG("Aborting tests.");
	return;
    }
	    
    // ------------------------ //
    // Access temperature grid. //
    // ------------------------ //
	    
    rtt_cdi_gandolf_test::testTemperatureGridAccessor(unpacked_opacity);
	    
    // ------------------------ //
    // Access the density grid. //
    // ------------------------ //
	    
    rtt_cdi_gandolf_test::testDensityGridAccessor(unpacked_opacity);
	    
    // ----------------------------- //
    // Access the energy boundaries. //
    // ----------------------------- //
	    
    rtt_cdi_gandolf_test::testEnergyBoundaryAccessor(unpacked_opacity);

    // try to unpack multigroup as gray opacity
    bool caught = false;
    try
    {
	SP<GrayOpacity> opacity(new GandolfGrayOpacity(packed));
    }
    catch (const rtt_dsxx::assertion &error)
    {
	caught = true;
	ostringstream message;
	message << "Good, we caught the following assertion, \n"
		<< error.what();
	PASSMSG(message.str());
    }
    if (!caught)
    {
	FAILMSG("Failed to catch an illegal packing asserion.");
    }

    if (rtt_cdi_gandolf_test::passed)
    {
	PASSMSG("GandolfMultigroupOpacity packing test successfull.");
    }
}

//---------------------------------------------------------------------------//
void test_exception_classes()
{
    using namespace rtt_cdi_gandolf;
    
    bool all_ok(true);
    std::string const dummyMsg( "Dummy fail" );
    std::string msg;


    // ggetmgException(int)
    { 
        std::string const expectedMsg(
            "Unknown error returned from Gandolf::ggetmg()." );
        try
        {
            all_ok=false;
            throw ggetmgException( dummyMsg );
        }
        catch( ggetmgException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }
    
    // ggetmgException(string)
    {
        std::string const expectedMsg("The IPCRESS file was not found.");
        try
        {
            all_ok=false;
            // 1 == "IPCRESS file was not found."
            throw ggetmgException( 1 );
        }
        catch( ggetmgException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

    // ggetgrayException(string)
    { 
        std::string const expectedMsg(
            "Unknown error returned from Gandolf::ggetgray()." );
        try
        {
            all_ok=false;
            throw ggetgrayException( dummyMsg );
        }
        catch( ggetgrayException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }
    
    // ggetgrayException(int):
    {
        std::string const expectedMsg("The IPCRESS file was not found.");
        try
        {
            all_ok=false;
            // 1 == "IPCRESS file was not found."
            throw ggetgrayException( 1 );
        }
        catch( ggetgrayException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

   // gmatidsException(string)
    { 
        std::string const expectedMsg(
            "Unknown error returned from Gandolf::gmatids()." );
        try
        {
            all_ok=false;
            throw gmatidsException( dummyMsg );
        }
        catch( gmatidsException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

    // gmatidsException(int)
    {
        std::string const expectedMsg("The IPCRESS file was not found.");
        try
        {
            all_ok=false;
            throw gmatidsException( 1 );
        }
        catch( gmatidsException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }
    
    // gchgridsException(string)
    { 
        std::string const expectedMsg(
            "Unknown error returned from Gandolf::gchgrids()." );
        try
        {
            all_ok=false;
            throw gchgridsException( dummyMsg );
        }
        catch( gchgridsException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

    // gchgridsException(int)
    {
        std::string const expectedMsg("The IPCRESS file was not found.");
        try
        {
            all_ok=false;
            throw gchgridsException( 1 );
        }
        catch( gchgridsException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

     // gkeysException(string)
    { 
        std::string const expectedMsg(
            "Unknown error returned from Gandolf::gkeys()." );
        try
        {
            all_ok=false;
            throw gkeysException( dummyMsg );
        }
        catch( gkeysException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

    // gkeysException(int)
    {
        std::string const expectedMsg("The IPCRESS file was not found.");
        try
        {
            all_ok=false;
            throw gkeysException( 1 );
        }
        catch( gkeysException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg = error.getErrorMessage();
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )              ITFAILS;
        if( ! (msg==expectedMsg) )  ITFAILS;
    }

    // Check the GandolfException base class
    {
        std::string const expectedMsg("bogus error message.");
        std::string gfn;
        int ec(0);
        std::string es;
        std::string whatMsg;
        try
        {
            all_ok=false;
            throw GandolfException( dummyMsg );
        }
        catch( GandolfException const & error)
        {
            PASSMSG("Caught an expected exception");
            msg     = error.getErrorMessage();
            gfn     = error.getGandolfFunctionName();
            ec      = error.getErrorCode();
            es      = error.errorSummary();
            whatMsg = error.what();
            
            all_ok=true;        
        }
        catch(...)
        {
            FAILMSG("Caught an unexpected exception");
            all_ok=false;
        }
        
        if( ! all_ok )                      ITFAILS;
        if( msg != expectedMsg )            ITFAILS;
        if( ec != -99 )                     ITFAILS;
        if( gfn != std::string("unknown") ) ITFAILS;
        if( whatMsg != es )                 ITFAILS;

    }
    
    return;
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    // version tag
    for (int arg = 1; arg < argc; arg++)
	if (string(argv[arg]) == "--version")
	{
	    cout << argv[0] << ": version " << rtt_cdi_gandolf::release() 
		 << endl;
	    return 0;
	}

    std::cout << "\nWe are testing cdi_gandolf: version "
              << rtt_cdi_gandolf::release() << "\n" << std::endl;
    
    try
    {
	// >>> UNIT TESTS
	file_check_Al_BeCu();
	file_check_analytic();
	check_gandolf_stl_accessors();

	gray_opacity_packing_test();
	mg_opacity_packing_test();

        test_exception_classes();
    }
    catch (rtt_dsxx::assertion &error)
    {
	cout << "While testing tGandolfOpacity, " << error.what()
	     << endl;
	return 1;
    }

    // status of test
    cout << endl;
    cout <<     "*********************************************" << endl;
    if (rtt_cdi_gandolf_test::passed) 
    {
        cout << "**** tGandolfOpacity Test: PASSED" 
	     << endl;
    }
    cout <<     "*********************************************" << endl;
    cout << endl;

    cout << "Done testing tGandolfOpacity." << endl;
}   

//---------------------------------------------------------------------------//
//                        end of tGandolfOpacity.cc
//---------------------------------------------------------------------------//
