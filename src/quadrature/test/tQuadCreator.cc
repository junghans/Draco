//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/test/tQuadCreator.cc
 * \author Kelly Thompson
 * \date   Fri Aug 18 12:46:41 2006
 * \brief  Unit test for quadcreator class.
 * \note   � Copyright 2006 LANSLLC All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//


#include <string>
#include <iostream>
#include <sstream>

#include "ds++/SP.hh"
#include "ds++/ScalarUnitTest.hh"
#include "parser/String_Token_Stream.hh"

#include "../Quadrature.hh"
#include "../QuadCreator.hh"
#include "../Release.hh"

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstTokenCtor( rtt_dsxx::ScalarUnitTest & ut )
{
    using namespace std;
    using namespace rtt_quadrature;

    try
    {
        ostringstream contents;

        contents << "level symmetric\n"
                 << "  order 2\n"
                 << "end" << endl;
        
        rtt_parser::String_Token_Stream tokens( contents.str() );
        
        // Create a Quadrature from this set of tokens.
        
        rtt_dsxx::SP< Quadrature const > spQuad = QuadCreator().quadCreate( tokens );
        
        if( spQuad->name() == "2D Level Symmetric" )
            ut.passes("Found correct quadrature name.");
        else
            ut.failure("Did not find expected quadrature name.");
        
        if( spQuad->getSnOrder() == 2 )
            ut.passes("Found correct SnOrder.");
        else
            ut.failure("Did not find expected SnOrder.");
    }
    catch(...)
    {
        ut.failure("Encountered a C++ Exception.");
        throw;
    }

//---------------------------------------------------------------------------//

    try
    {
        ostringstream contents;

        contents << "square CL\n"
                 << "  order 17\n"
                 << "end" << endl;

        rtt_parser::String_Token_Stream tokens( contents.str() );
        
        // Create a Quadrature from this set of tokens.
        
        rtt_dsxx::SP< Quadrature const > spQuad = QuadCreator().quadCreate( tokens );

        if( spQuad->name() == "2D Square Chebyshev Legendre" )
            ut.passes("Found correct quadrature name.");
        else
            ut.failure("Did not find expected quadrature name.");
        
        if( spQuad->getSnOrder() == 2 )
            ut.passes("Found correct SnOrder.");
        else
            ut.failure("Did not find expected SnOrder.");
        
    }
    catch(...)
    {
        ut.failure("Encountered a C++ Exception.");
        throw;
    }
    return;
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    using namespace std;

    try
    {
        rtt_dsxx::ScalarUnitTest ut( argc, argv, rtt_quadrature::release );
        tstTokenCtor( ut );
    }
    catch( rtt_dsxx::assertion &err )
    {
        std::string msg = err.what();
        if( msg != std::string( "Success" ) )
        { cout << "ERROR: While testing " << argv[0] << ", "
               << err.what() << endl;
            return 1;
        }
        return 0;
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
             << "An unknown exception was thrown" << endl;
        return 1;
    }

    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tQuadrature.cc
//---------------------------------------------------------------------------//
