//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   shared_lib/test/creator_destroyer.cc
 * \author Rob Lowrie
 * \date   Wed Dec 29 11:16:44 2004
 * \brief  Implementation of creator and destroyer functions for Foo.
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "Foo_Base.hh"
#include "Foo.hh"

using rtt_shared_lib_test::Foo_Base;
using rtt_shared_lib_test::Foo;

// These functions are used by the tst_Shared_Lib to create and destroy Foo
// objects, where Foo is defined within a shared library.

extern "C"
{
    // The Creator
    Foo_Base* my_creator(const double x)
    {
	return new Foo(x);
    }

    // The Destroyer
    void my_destroyer(Foo_Base* p)
    {
	if ( p ) delete p;
    }
}

//---------------------------------------------------------------------------//
//              end of shared_lib/test/creator_destroyer.cc
//---------------------------------------------------------------------------//
