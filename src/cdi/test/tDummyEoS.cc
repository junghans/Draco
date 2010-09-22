//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi/test/tDummyEoS.cc
 * \author Thomas M. Evans
 * \date   Tue Oct  9 10:52:50 2001
 * \brief  EoS class test.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "cdi_test.hh"
#include "DummyEoS.hh"
#include "../EoS.hh"
#include "../Release.hh"
#include "ds++/Assert.hh"
#include "ds++/SP.hh"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

using rtt_cdi_test::match;
using rtt_cdi::EoS;
using rtt_dsxx::SP;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void test_EoS()
{
    // --------------------- //
    // Create an EoS object. //
    // --------------------- //
	    
    // The smart pointer points to a generic EoS object.
    SP<EoS> spEoS;
	    
    // The actual instatniate is specific (dummyEoS).
    if ( spEoS = new rtt_cdi_test::DummyEoS() )
    {
	// If we get here then the object was successfully instantiated.
	PASSMSG("Smart Pointer to new EoS object created.");
    }
    else
    {
	FAILMSG("Unable to create a Smart Pointer to new EoS object.");
    }
	    
    // --------- //
    // EoS Tests //
    // --------- //
	    
    double temperature = 5800.0; // Kelvin
    double density     = 27.0;   // g/cm^3
    double tabulatedSpecificElectronInternalEnergy
	= temperature + 1000.0*density; // kJ/g
	    
    double seie = spEoS->getSpecificElectronInternalEnergy( 
	temperature, density );
	    
    if ( match ( seie, tabulatedSpecificElectronInternalEnergy ) ) 
    {
	ostringstream message;
	message << "The getSpecificElectronInternalEnergy( dbl, dbl) " 
		<< "request returned the expected value.";
	PASSMSG( message.str() );
    }
    else
    {
	ostringstream message;
	message << "The getSpecificElectronInternalEnergy( dbl, dbl) "
		<< "request returned a value that is out of spec.";
	FAILMSG( message.str() );
    }
	    
    // try using a vectors of temps. and densities
    // vtemperature.size() == vdensity.size()
	    
    std::vector< double > vtemperature(3);
    vtemperature[0] = 5000.0; // Kelvin
    vtemperature[1] = 7000.0; // Kelvin
    vtemperature[2] = 3000.0; // Kelvin

    std::vector< double > vdensity(3);
    vdensity[0] = 0.35; // g/cm^3
    vdensity[1] = 1.0;  // g/cm^3
    vdensity[2] = 9.8;  // g/mcm^3

    // Retrieve electron based heat capacities.
    std::vector< double > vRefCve( vtemperature.size() );
    for ( size_t i=0; i<vtemperature.size(); ++i )
	vRefCve[i] = vtemperature[i] + vdensity[i]/1000.0;
	    
    std::vector< double > vCve = spEoS->getElectronHeatCapacity(
	vtemperature, vdensity );
	    
    if ( match ( vCve, vRefCve ) ) 
    {
	ostringstream message;
	message << "The getElectronHeatCapacity( vect, vect ) request"
		<< " returned the expected values.";
	PASSMSG( message.str() );
    }
    else
    {
	ostringstream message;
	message << "The getElectronHeatCapacity( vect, vect ) request"
		<< " returned values that are out of spec.";
	FAILMSG( message.str() );
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    // version tag
    for (int arg = 1; arg < argc; arg++)
	if (string(argv[arg]) == "--version")
	{
	    cout << argv[0] << ": version " << rtt_cdi::release() 
		 << endl;
	    return 0;
	}

    try
    {
	// >>> UNIT TESTS
	test_EoS();
    }
    catch (rtt_dsxx::assertion &ass)
    {
	cout << "While testing tDummyEoS, " << ass.what()
	     << endl;
	return 1;
    }

    // status of test
    cout << endl;
    cout <<     "*********************************************" << endl;
    if (rtt_cdi_test::passed) 
    {
        cout << "**** tDummyEoS Test: PASSED" 
	     << endl;
    }
    cout <<     "*********************************************" << endl;
    cout << endl;

    cout << "Done testing tDummyEoS." << endl;
}   

//---------------------------------------------------------------------------//
//                        end of tDummyEoS.cc
//---------------------------------------------------------------------------//
