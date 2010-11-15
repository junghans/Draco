//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/test/tstvalue.cc
 * \author Kent G. Budge
 * \date   Wed Nov 10 14:27:59 2010
 * \brief  Test the value function template
 * \note   Copyright (C) 2010 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cmath>

#include "../Assert.hh"
#include "../ScalarUnitTest.hh"
#include "../Release.hh"
#include "../value.hh"

using namespace std;
using namespace rtt_dsxx;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

struct unlabeled
{
    int i;
};

struct labeled
{
    unlabeled s;
    int j;

    operator unlabeled&() { return s; }
};

namespace rtt_dsxx
{

template<>
class Field_Traits<labeled> 
{
  public:

    typedef unlabeled unlabeled_type;
};

}

bool operator==(unlabeled const &a, labeled const &b)
{
    return a.i == b.s.i;
}

void tstvalue(UnitTest &ut)
{
    double x = 3;
    double const cx = 4;

    if (x == value(x) && cx == value(cx))
    {
        ut.passes("value strips double correctly");
    }
    else
    {
        ut.failure("value does NOT strip double correctly");
    }

    labeled s = {{1}, 2};

    if (value(s) == s)
    {
        ut.passes("value strips struct correctly");
    }
    else
    {
        ut.failure("value does NOT strip struct correctly");
    }
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    ScalarUnitTest ut(argc, argv, release);
    try
    {
        tstvalue(ut);
    }
    catch (std::exception &err)
    {
        std::cout << "ERROR: While testing tstvalue, " 
                  << err.what()
                  << endl;
        ut.numFails++;
    }
    catch( ... )
    {
        std::cout << "ERROR: While testing tstvalue, " 
                  << "An unknown exception was thrown."
                  << endl;
        ut.numFails++;
    }
    return ut.numFails;
}   

//---------------------------------------------------------------------------//
//                        end of tstvalue.cc
//---------------------------------------------------------------------------//
