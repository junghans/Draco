//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/ParallelUnitTest.cc
 * \author Kelly Thompson
 * \date   Thu Jun  1 17:15:05 2006
 * \brief  Implementation file for encapsulation of Draco parallel unit tests.
 * \note   Copyright 2006 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <sstream>

#include "ParallelUnitTest.hh"
#include "SpinLock.hh"
#include "global.hh"

namespace rtt_c4
{
//---------------------------------------------------------------------------//
/*!
 * \brief Constructor for ParallelUnitTest
 * \arg argc The number of command line arguments
 * \arg argv A list of strings containg the command line arguments
 * \arg release_ A function pointer to this package's release function.
 * \arg out_ A user specified iostream that defaults to std::cout.
 * \exception rtt_dsxx::assertion An exception with the message "Success" will
 * be thrown if \c --version is found in the argument list.  
 *
 * The constructor initializes the prallel communicator (MPI) and then
 * initializes the base class UnitTest by setting numPasses and numFails to
 * zero.  It also prints a message that declares this to be a scalar unit test
 * and provides the unit test name.
 */
ParallelUnitTest::ParallelUnitTest( int &argc, char **&argv,
                                    string_fp_void release_,
                                    std::ostream & out_ )
    : UnitTest( argc, argv, release_, out_ )
{
    using std::string;

    initialize( argc, argv );
    
    Require( argc > 0 );
    Require( release != NULL );
    
    // header
    
    if( node() == 0 )
        out << "\n============================================="
            << "\n=== Parallel Unit Test: " << testName
            << "\n=== Number of Processors: " << nodes()
            << "\n=============================================\n"
            << std::endl;
    
    // version tag
    
    if( node() == 0 )
        out << testName << ": version " << release() << "\n" << std::endl;
    
    // exit if command line contains "--version"
    
    for( int arg = 1; arg < argc; arg++ )
        if( string( argv[arg] ) == "--version" )
        {
            finalize();
            throw rtt_dsxx::assertion( string( "Success" ) );
        }
    Ensure( numPasses == 0 );
    Ensure( numFails  == 0 );
    Ensure( testName.length() > 0 );
     
    return;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Destructor.
 * The destructor provides a final status report before it calls MPI_Finalize
 * and exits.
 */
ParallelUnitTest::~ParallelUnitTest()
{
    global_barrier();
    if( node() == 0 )
        out << resultMessage() << std::endl;
    global_barrier();
    finalize();
    return;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Print a summary of the pass/fail status of ParallelUnitTest.
 */
void ParallelUnitTest::status()
{
    { // Provide some space before the report -- but keep all the processors
      // in sync. 
        HTSyncSpinLock slock;
        if( node() == 0 )
            out << std::endl;
    }
    {
        HTSyncSpinLock slock;
        out << "Done testing " << testName << " on node "
            << node() << "." << std::endl;
    }
    return;
}

} // end namespace rtt_c4

//---------------------------------------------------------------------------//
//                 end of ParallelUnitTest.cc
//---------------------------------------------------------------------------//
