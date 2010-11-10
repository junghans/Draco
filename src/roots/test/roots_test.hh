//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   roots/test/roots_test.hh
 * \author Kent G. Budge
 * \date   Tue Nov  9 16:49:55 2010
 * \brief  
 * \note   Copyright (C) 2006 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef roots_test_hh
#define roots_test_hh

#include <iostream>
#include <string>

namespace rtt_roots_test
{

//===========================================================================//
// PASS/FAILURE LIMIT
//===========================================================================//

// Returns true for pass
// Returns false for fail
// Failure functions also set rtt_roots_test::passed to false

// These can be used in any combination in a test to print output messages  
// if no fail functions are called then the test will pass
// (rtt_roots_test::passed will have its default value of true)

// Needless to say, these can be used in many different combinations or
// ways.  We do not constrain draco tests except that the output must be of
// the form "Test: pass/fail"

bool fail(int line);

bool fail(int line, char *file);

bool pass_msg(const std::string &);

bool fail_msg(const std::string &);

void unit_test(const bool pass, int line, char *file);

//---------------------------------------------------------------------------//
// PASSING CONDITIONALS
//---------------------------------------------------------------------------//

extern bool passed;

} // end namespace rtt_roots_test

//===========================================================================//
// TEST MACROS
//
// USAGE:
// if (!condition) ITFAILS;
//
// These are a convenience only
//===========================================================================//

#define ITFAILS      rtt_roots_test::fail(__LINE__);
#define FAILURE      rtt_roots_test::fail(__LINE__, __FILE__);
#define PASSMSG(a)   rtt_roots_test::pass_msg(a);
#define FAILMSG(a)   rtt_roots_test::fail_msg(a);
#define UNIT_TEST(x) rtt_roots_test::unit_test(x, __LINE__, __FILE__)
    
#endif // roots_test_hh

//---------------------------------------------------------------------------//
//     end of roots/roots_test.hh
//---------------------------------------------------------------------------//
