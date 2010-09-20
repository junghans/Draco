//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   container/test/tstSlice.cc
 * \author Kent Budge
 * \date   Thu Jul  8 08:02:51 2004
 * \brief  Test the Slice subset container class.
 * \note   Copyright 2004 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "../Assert.hh"
#include "../Release.hh"
#include "../ScalarUnitTest.hh"
#include "../Slice.hh"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace rtt_dsxx;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstSlice(UnitTest &ut)
{
    vector<unsigned> v(5);
    for (unsigned i=0; i<5; i++)
    {
	v[i] = i;
    }
    Slice<vector<unsigned>::iterator> s = slice(v.begin()+1, 2, 2);
    if (s.size()==2)
    {
	ut.passes("size of vector Slice is correct");
    }
    else
    {
	ut.failure("size of vector Slice is NOT correct");
    }
    if (s[1]==3)
    {
	ut.passes("indexing of Slice is correct");
    }
    else
    {
	ut.failure("size of Slice is NOT correct");
    }
    if (s.begin()<s.end())
    {
	ut.passes("ordering of Slice is correct");
    }
    else
    {
	ut.failure("ordering of Slice is NOT correct");
    }
    if (!(s.end()<s.begin()))
    {
	ut.passes("ordering of Slice is correct");
    }
    else
    {
	ut.failure("ordering of Slice is NOT correct");
    }

    Slice<vector<unsigned>::iterator>::iterator i = s.begin();
    if (*i == 1)
    {
	ut.passes("deref of begin is correct");
    }
    else
    {
	ut.failure("deref of begin is NOT correct");
    }
    if (i[0] == 1)
    {
	ut.passes("operator[] of begin is correct");
    }
    else
    {
	ut.failure("operator[] of begin is NOT correct");
    }
    if (*(i+1) == 3)
    {
	ut.passes("operator+ is correct");
    }
    else
    {
	ut.failure("operator+ is NOT correct");
    }
    ++i;
    if (*i == 3)
    {
	ut.passes("deref of ++begin is correct");
    }
    else
    {
	ut.failure("deref of ++begin is NOT correct");
    }
    if (i-s.begin() == 1)
    {
	ut.passes("operator- is correct");
    }
    else
    {
	ut.failure("operator- is NOT correct");
    }
    ++i;
    if (i != s.end())
    {
	ut.failure("++ past end is NOT correct");
    }
    else
    {
	ut.passes("++ past end is correct");
    }


    Slice<vector<unsigned>::iterator> s2(v.begin(), 3, 2);
    if (s2.size()==3)
    {
	ut.passes("size of Slice is correct");
    }
    else
    {
	ut.failure("size of Slice is NOT correct");
    }
    if (s2[1]==2)
    {
	ut.passes("indexing of Slice is correct");
    }
    else
    {
	ut.failure("size of Slice is NOT correct");
    }

    double da[6];
    Slice<double*> das(da, 2, 3);
    if (das.size()==2)
    {
	ut.passes("size of Slice is correct");
    }
    else
    {
	ut.failure("size of Slice is NOT correct");
    }

    vector<double> db_vector(6);
    double *const db = &db_vector[0];
    db[0] = 0;
    Slice<vector<double>::iterator> dbs(db_vector.begin(), 2, 3);
    if (dbs.size()==2)
    {
	ut.passes("size of Slice is correct");
    }
    else
    {
	ut.failure("size of Slice is NOT correct");
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    try
    {
        ScalarUnitTest ut(argc, argv, release);
	tstSlice(ut);
    }
    catch (std::exception &err)
    {
	std::cout << "ERROR: While testing tstSlice, " 
		  << err.what()
		  << std::endl;
	return 1;
    }
    catch( ... )
    {
	std::cout << "ERROR: While testing tstSlice, " 
		  << "An unknown exception was thrown."
		  << std::endl;
	return 1;
    }
    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tstSlice.cc
//---------------------------------------------------------------------------//
