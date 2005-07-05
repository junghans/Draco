//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/test/tstDBC_Ptr.cc
 * \author Paul Henning
 * \brief  DBC_Ptr tests.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "ds_test.hh"
#include "../Release.hh"
#include "../DBC_Ptr.hh"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <typeinfo>
#include <sstream>

using namespace std;
using rtt_dsxx::DBC_Ptr;

struct Base_Class
{
    virtual ~Base_Class() {}
    int a;
};

struct Derived_Class : public Base_Class
{
    int b;
};

struct Owner
{
    Owner() : ptr(new int) {}
    ~Owner() { ptr.delete_data(); }
    DBC_Ptr<int> ptr;
};

//---------------------------------------------------------------------------//

void test_basic()
{
    bool caught = false;
    try
    {
	DBC_Ptr<double> foo(new double);
	foo.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }
    if(caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_basic");
    else
	FAILMSG("test_basic FAILED!");
}

//---------------------------------------------------------------------------//

void test_undeleted()
{
    bool caught = false;
    try
    {
	DBC_Ptr<Base_Class> foo(new Base_Class);
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(!caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_undeleted");
    else
	FAILMSG("test_undeleted FAILED!");
}

//---------------------------------------------------------------------------//

void test_over_deleted()
{
    bool caught = false;
    try
    {
	DBC_Ptr<Base_Class> foo(new Base_Class);
	foo.delete_data();
	foo.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(!caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_over_deleted");
    else
	FAILMSG("test_over_deleted FAILED!");
}

//---------------------------------------------------------------------------//


DBC_Ptr<int> gen_func()
{
    DBC_Ptr<int> retval;
    retval = new int;
    *retval = static_cast<int>(std::rand());
    return retval;
}

// Make sure that, when a DBC_Ptr is created and returned from another
// function, the reference counts get adjusted when the variable in the other
// function (retval) goes out of scope.
void test_function_return()
{
    bool caught = false;
    try
    {
	DBC_Ptr<int> foo = gen_func();
	foo.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_function_return");
    else
	FAILMSG("test_function_return FAILED!");
}


//---------------------------------------------------------------------------//


void update_func(DBC_Ptr<int>& foo)
{
    *foo = 3;
}

// Make sure that you can pass a DBC_Ptr by reference
void test_pass_by_ref()
{
    bool caught = false;
    try
    {
	DBC_Ptr<int> foo(new int);
	update_func(foo);
	Check(*foo == 3);
	foo.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_pass_by_ref");
    else
	FAILMSG("test_pass_by_ref FAILED!");
}


//---------------------------------------------------------------------------//

// Ensure that the reference counting system catches a dangling reference
// (deleting through one pointer while another pointer still points at the
// object).
void test_dangling()
{
    bool caught = false;
    try
    {
	DBC_Ptr<int> foo(new int);
	DBC_Ptr<int> bar(foo);
	foo.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }
    catch(...)
    {
	std::cout << "WTF, mate?" << std::endl;
    }

    if(!caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_dangling");
    else
	FAILMSG("test_dangling FAILED!");
}


//---------------------------------------------------------------------------//


void test_nested()
{
    bool caught = false;
    try
    {
	Owner o;
	o.ptr.release_data();

	// o.ptr.delete_data() gets called here by ~Owner
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }
    catch(...)
    {
	std::cout << "WTF, mate?" << std::endl;
    }

    if(!caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_nested");
    else
	FAILMSG("test_nested FAILED!");
}

//---------------------------------------------------------------------------//


void test_void()
{
    bool caught = false;
    try
    {
	DBC_Ptr<int> foo;
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(caught) ITFAILS;

    caught = false;
    try
    {
	DBC_Ptr<int> foo;
	DBC_Ptr<int> bar = foo;
    }
    catch(rtt_dsxx::assertion &ass)
    {
	caught = true;
    }

    if(caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_void");
    else
	FAILMSG("test_void FAILED!");
}

//---------------------------------------------------------------------------//

void test_polymorph()
{
    bool caught = false;
    try
    {
	DBC_Ptr<Base_Class> base(new Derived_Class);
	base.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	std::cout << ass.what() << std::endl;
	caught = true;
    }
    if(caught) ITFAILS;

    caught = false;
    try
    {
	DBC_Ptr<Base_Class> base;
	DBC_Ptr<Derived_Class> derv(new Derived_Class);
	base = derv;
	derv.release_data();
	derv = base;
	base.release_data();
	derv.delete_data();
    }
    catch(rtt_dsxx::assertion &ass)
    {
	std::cout << ass.what() << std::endl;
	caught = true;
    }
    
    if(caught) ITFAILS;

    if (rtt_ds_test::passed)
	PASSMSG("test_polymorph");
    else
	FAILMSG("test_polymorph FAILED!");
}

//---------------------------------------------------------------------------//

int 
main(int argc, char *argv[])
{
    // version tag
    for (int arg = 1; arg < argc; arg++)
	if (string(argv[arg]) == "--version")
	{
	    cout << argv[0] << ": version " << rtt_dsxx::release() 
		 << endl;
	    return 0;
	}

#if DBC
    try
    {
	// >>> UNIT TESTS
	test_basic();
	test_undeleted();
	test_over_deleted();
	test_dangling();
	test_function_return();
	test_pass_by_ref();
	test_polymorph();
	test_void();
	test_nested();
    }
    catch (rtt_dsxx::assertion &ass)
    {
	cout << "While testing tstDBC_Ptr, " << ass.what()
	     << endl;
	return 1;
    }

    catch (...)
    {
	cout << "caught uncaught exception" << std::endl;
	return 10;
    }
#else
    std::cout << "\n\nNo tests without DBC" << std::endl;
#endif

    // status of test
    cout << endl;
    cout <<     "*********************************************" << endl;
    if (rtt_ds_test::passed) 
    {
        cout << "**** tstDBC_Ptr Test: PASSED" 
	     << endl;
    }
    cout <<     "*********************************************" << endl;
    cout << endl;

    cout << "Done testing tstDBC_Ptr." << endl;
    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tstDBC_Ptr.cc
//---------------------------------------------------------------------------//
