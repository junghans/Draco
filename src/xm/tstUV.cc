//----------------------------------*-C++-*----------------------------------//
// Copyright 1996 The Regents of the University of California. 
// All rights reserved.
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
// Created on: Thu Sep 12 10:56:25 1996
// Created by: Geoffrey Furnish
// Also maintained by:
//
//---------------------------------------------------------------------------//

#include "xm/UserVec.hh"

#include <iostream.h>

//---------------------------------------------------------------------------//
// Function to print out the contents of a UserVec.
//---------------------------------------------------------------------------//

template<class T> void dump( UserVec<T>& u )
{
    for( int i=0; i < u.size(); i++ )
	cout << "u[" << i << "]=" << u[i] << endl;
}

// template<class T, class E, class D> void dump( Xpr<T,E,D>& u )
// {
//     for( int i=0; i < u.size(); i++ )
// 	cout << "u[" << i << "]=" << u[i] << endl;
// }

//---------------------------------------------------------------------------//
// Check that we can index UserVec the usual way.
//---------------------------------------------------------------------------//

void t1()
{
    cout << "t1: start\n";

    UserVec<float> u(5);

    for( int i=0; i < 5; i++ )
	u[i] = 2.*i;

    dump(u);

    cout << "t1: end\n";
}

//---------------------------------------------------------------------------//
// Check a simple binary operation, with an assignment.
//---------------------------------------------------------------------------//

void t2()
{
    cout << "t2: start\n";

    UserVec<float> u(5), v(5), w(5);

    float a = 2.0;

    for( int i=0; i < 5; i++ ) {
	v[i] = 2.*i;
	w[i] = 5.-i;
    }
    u = 0;

    u += a;
    u += v;
    u += v + w;

//     u -= v;
//     u *= v;
//     u /= v;

    u = v + w;

    dump(u);

    cout << "t2: end\n";
}

//---------------------------------------------------------------------------//
// Now a more involved test of binary operations.
//---------------------------------------------------------------------------//

void t3()
{
    cout << "t3: start\n";

    UserVec<float> a(5), b(5), c(5), d(5), e(5), f(5);

    a = 4.;
    b = 2.;
    c = 2.;
    d = 5.;
    e = 2.;

// HP C is dieing on this, grrr.
//     f = (a+b)*c/(d-e);
    f = (a+b)*c/d;//(d-e);

    dump(f);

    cout << "t3: end\n";
}

//---------------------------------------------------------------------------//
// Now a ...
//---------------------------------------------------------------------------//

void t4()
{
    cout << "t4: start\n";

    UserVec<float> a(5), b(5), c(5), d(5);

    a = 4.;
    b = 2.;

    //c = pow( a, 2.f );

    dump(c);

    c = 2.;
    //d = pow( c, 2 );

    dump(d);

    //d = 1.f + pow( c, 2 );

    dump(d);

    cout << "t4: end\n";
}

//---------------------------------------------------------------------------//
// Check incompatible participation.
//---------------------------------------------------------------------------//

void t5()
{
    cout << "t5: start\n";

    UserVec<float> a(5), b(5);
    FooBar<float>  c(5);

    a = -99.;

    b = 2.; c = 3.;

// This statement should be illegal!
//     a = b + c;

    dump(a);

    cout << "t5: end\n";
}

//---------------------------------------------------------------------------//
// Check operator+= with various arguments.
//---------------------------------------------------------------------------//

void t6()
{
    cout << "t6: start\n";

    UserVec<float> u(5), v(5), w(5);

    float a = 2.0;

    for( int i=0; i < 5; i++ ) {
	v[i] = 2.*i;
	w[i] = 5.-i;
    }
    u = 0;

    cout << "-----------------------------------" << endl;
    cout << "Output of u"                         << endl;
    cout << "-----------------------------------" << endl;
    dump(u);

    cout << "-----------------------------------" << endl;
    cout << "Output of v"                         << endl;
    cout << "-----------------------------------" << endl;
    dump(v);

    cout << "-----------------------------------" << endl;
    cout << "Output of w"                         << endl;
    cout << "-----------------------------------" << endl;
    dump(w);

    cout << "-----------------------------------" << endl;
    cout << "Output of u += 2.0"                  << endl;
    cout << "-----------------------------------" << endl;
    u += a;
    dump(u);

    cout << "-----------------------------------" << endl;
    cout << "Output of u += v"                    << endl;
    cout << "-----------------------------------" << endl;
    u += v;
    dump(u);

    cout << "-----------------------------------" << endl;
    cout << "Output of u += v + w"                << endl;
    cout << "-----------------------------------" << endl;
    u += v + w;
    dump(u);

    cout << "t6: end\n";
}

//---------------------------------------------------------------------------//
// Main program, just run through each test in turn.
//---------------------------------------------------------------------------//

int main( int argc, char *argv[] )
{
    t1();
    t2();
    t3();
    t4();
    t5();
    t6();

// Print the status of the test.

    cout << "PASSED" << endl;

    return 0;
}

//---------------------------------------------------------------------------//
//                              end of tstUV.cc
//---------------------------------------------------------------------------//
