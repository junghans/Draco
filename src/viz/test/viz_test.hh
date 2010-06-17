//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   viz/test/viz_test.hh
 * \author Rob Lowrie
 * \date   Mon Mar  8 10:29:48 2004
 * \brief  
 * \note   Copyright � 2003 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef rtt_viz_test_hh
#define rtt_viz_test_hh

#include <iostream>
#include <string>

namespace rtt_viz_test
{

//===========================================================================//
// PASS/FAILURE LIMIT
//===========================================================================//

// Returns true for pass
// Returns false for fail
// Failure functions also set rtt_viz_test::passed to false

// These can be used in any combination in a test to print output messages  
// if no fail functions are called then the test will pass
// (rtt_viz_test::passed will have its default value of true)

// Needless to say, these can be used in many different combinations or
// ways.  We do not constrain draco tests except that the output must be of
// the form "Test: pass/fail"

bool fail(int line);

bool fail(int line, char const * file);

bool pass_msg(const std::string &);

bool fail_msg(const std::string &);

void unit_test(const bool pass, int line, char const * file);

//---------------------------------------------------------------------------//
// PASSING CONDITIONALS
//---------------------------------------------------------------------------//

extern bool passed;

} // end namespace rtt_viz_test

//===========================================================================//
// TEST MACROS
//
// USAGE:
// if (!condition) ITFAILS;
//
// These are a convenience only
//===========================================================================//

#define ITFAILS    rtt_viz_test::fail(__LINE__);
#define FAILURE    rtt_viz_test::fail(__LINE__, __FILE__);
#define PASSMSG(a) rtt_viz_test::pass_msg(a);
#define FAILMSG(a) rtt_viz_test::fail_msg(a);
#define UNIT_TEST(x) rtt_viz_test::unit_test(x, __LINE__, __FILE__)
    
#endif // rtt_viz_test_hh

//---------------------------------------------------------------------------//
//     end of viz/test/viz_test.hh
//---------------------------------------------------------------------------//
