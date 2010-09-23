//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_gandolf/test/cdi_gandolf_test.cc
 * \author Thomas M. Evans
 * \date   Fri Oct 12 15:36:36 2001
 * \brief  cdi_gandolf test functions.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "cdi_gandolf_test.hh"
#include "ds++/Soft_Equivalence.hh"
#include <iostream>
#include <iomanip>
#include <cmath>

using rtt_dsxx::soft_equiv;

namespace rtt_cdi_gandolf_test
{

//===========================================================================//
// PASS/FAILURE
//===========================================================================//

bool fail(int line)
{
    std::cout << "Test: failed on line " << line << std::endl;
    passed = false;
    return false;
}

//---------------------------------------------------------------------------//

bool fail(int line, char *file)
{
    std::cout << "Test: failed on line " << line << " in " << file
              << std::endl;
    passed = false;
    return false;
}

//---------------------------------------------------------------------------//

bool pass_msg(const std::string &passmsg)
{
    std::cout << "Test: passed" << std::endl;
    std::cout << "     " << passmsg << std::endl;
    return true;
}

//---------------------------------------------------------------------------//

bool fail_msg(const std::string &failmsg)
{
    std::cout << "Test: failed" << std::endl;
    std::cout << "     " << failmsg << std::endl;
    passed = false;
    return false;
}

//---------------------------------------------------------------------------//
// BOOLEAN PASS FLAG
//---------------------------------------------------------------------------//

bool passed = true;

//---------------------------------------------------------------------------//
// DATA EQUIVALENCE FUNCTIONS USED FOR TESTING
//---------------------------------------------------------------------------//

bool match( double computedValue, double referenceValue )
{
    return soft_equiv(computedValue, referenceValue );
} 

bool match( const std::vector< double > &computedValue, 
            const std::vector< double > &referenceValue ) 
{
    // If the vector sizes don't match then die
    if ( computedValue.size() != referenceValue.size() )
        return false;

    for ( size_t i=0; i<computedValue.size(); ++i )
    {
        if (! soft_equiv(computedValue[i], referenceValue[i] ))
            return false;
    }
    return true;
}

bool match(const std::vector< std::vector< double > >& computedValue, 
           const std::vector< std::vector< double > >& referenceValue )
{
    // If the vector sizes don't match then die
    if ( computedValue.size() != referenceValue.size() )
    {
        std::cout << "computedValue's size " << computedValue.size()
                  << " is not equal to referenceValue's size " << referenceValue.size()
                  << std::endl;
        return false;
    }

    // Test each item in the list
    for ( size_t i=0; i<computedValue.size(); ++i )
    {
        // If the vector sizes don't match then die
        if ( computedValue[i].size() != referenceValue[i].size() )
        {
            std::cout << "computedValue[" << i << "]'s size "
                      << computedValue.size()
                      << " is not equal to referenceValue[" << i << "]'s size "
                      << referenceValue.size() << std::endl;
            return false;
        }

        for ( size_t j=0; j<computedValue[i].size(); ++j )
        {
            // If the comparison fails then stop testing and
            // return "false" to indicate that the test
            // failed. 
            if (! soft_equiv(computedValue[i][j], referenceValue[i][j] ))
            {
                std::cout << std::setprecision(14)
                          << "At index [" << i << "][" << j << "], "
                          << "computed value " << computedValue[i][j] << " "
                          << "does not match reference value " << referenceValue[i][j]
                          << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool match(
    const std::vector< std::vector< std::vector< double > > >& computedValue, 
    const std::vector< std::vector< std::vector< double > > >& referenceValue )
{
    // If the vector sizes don't match then die
    if ( computedValue.size() != referenceValue.size() )
    {
        std::cout << "computedValue's size " << computedValue.size()
                  << " is not equal to referenceValue's size " << referenceValue.size()
                  << std::endl;
        return false;
    }

    // Test each item in the list
    for ( size_t i=0; i<computedValue.size(); ++i )
    {
        //call the 2-D vector comparison
        if (!match(computedValue[i], referenceValue[i]))
        {
            std::cout << "... returned for base index "<< i << "." << std::endl;
            return false;
        }
    }
    return true;
}

} // end namespace rtt_cdi_gandolf_test

//---------------------------------------------------------------------------//
//                              end of cdi_gandolf_test.cc
//---------------------------------------------------------------------------//
