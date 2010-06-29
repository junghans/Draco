//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/test/tAngle_Quadrature.cc
 * \author Kent Budge
 * \date   Tue Aug 26 10:10:44 2008
 * \brief  
 * \note   Copyright (C) 2006 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>

#include "ds++/Assert.hh"
#include "ds++/ScalarUnitTest.hh"
#include "parser/String_Token_Stream.hh"
#include "../Release.hh"
#include "../QuadCreator.hh"
#include "../Angle_Operator.hh"

using namespace std;
using namespace rtt_dsxx;
using namespace rtt_quadrature;

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void tstAngle_Quadrature(ScalarUnitTest &ut)
{
    ostringstream contents;
    
    contents << "type = level symmetric\n"
             << "order = 2\n"
             << "end\n"
             << endl;
    
    rtt_parser::String_Token_Stream tokens( contents.str() );
    
    // Create a Quadrature from this set of tokens.

    SP< Quadrature const > spQuad = QuadCreator().quadCreate( tokens );

    SP<Angle_Operator> const spAngle_Operator =
        SP<Angle_Operator>(new Angle_Operator(spQuad,
                                              rtt_mesh_element::AXISYMMETRIC,
                                              2));

    if (spAngle_Operator->Number_Of_Levels()==2)
    {
        ut.passes("Number of levels correct");
    }
    else
    {
        ut.failure("Number of levels NOT correct");
    }

    if (!spAngle_Operator->Is_Dependent(0) &&
        spAngle_Operator->Is_Dependent(1) &&
        spAngle_Operator->Is_Dependent(2) &&
        !spAngle_Operator->Is_Dependent(3) &&
        spAngle_Operator->Is_Dependent(4) &&
        spAngle_Operator->Is_Dependent(5))
    {
        ut.passes("Dependencies correct");
    }
    else
    {
        ut.failure("Dependencies NOT correct");
    }

    vector<double> const &alpha = spAngle_Operator->Alpha();
    if (alpha.size()==6)
    {
        ut.passes("Number of alphas correct");
    }
    else
    {
        ut.failure("Number of alphas NOT correct");
    }
    if (soft_equiv(alpha[0], 0.0) &&
        soft_equiv(alpha[1], -0.144337567297406) &&
        soft_equiv(alpha[2], 0.0) &&
        soft_equiv(alpha[3], 0.0) &&
        soft_equiv(alpha[4], -0.144337567297406) &&
        soft_equiv(alpha[5], 0.0))
    {
        ut.passes("alphas correct");
    }
    else
    {
        ut.failure("alphas NOT correct");
    }

    vector<unsigned> const &levels = spAngle_Operator->Levels();
    if (levels.size()==6)
    {
        ut.passes("Number of levels correct");
    }
    else
    {
        ut.failure("Number of levels NOT correct");
    }
    if (levels[0] == 0 &&
        levels[1] == 0 &&
        levels[2] == 0 &&
        levels[3] == 1 &&
        levels[4] == 1 &&
        levels[5] == 1)
    {
        ut.passes("levels correct");
    }
    else
    {
        ut.failure("levels NOT correct");
    }
    
    vector<double> const &tau = spAngle_Operator->Tau();
    if (tau.size()==6)
    {
        ut.passes("Number of taus correct");
    }
    else
    {
        ut.failure("Number of taus NOT correct");
    }
    if (soft_equiv(tau[0], 1.0) &&
        soft_equiv(tau[1], 0.29289321881345248277) &&
        soft_equiv(tau[2], 0.70710678118654746172) &&
        soft_equiv(tau[3], 1.0) &&
        soft_equiv(tau[4], 0.29289321881345248277) &&
        soft_equiv(tau[5], 0.70710678118654746172))
    {
        ut.passes("taus correct");
    }
    else
    {
        ut.failure("taus NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Psi_Coefficient(1), -1.9711971193069772745) &&
        soft_equiv(spAngle_Operator->Psi_Coefficient(2), 0.0) &&
        soft_equiv(spAngle_Operator->Psi_Coefficient(4), -1.9711971193069772745) &&
        soft_equiv(spAngle_Operator->Psi_Coefficient(5), 0.0))
    {
        ut.passes("psis correct");
    }
    else
    {
        ut.failure("psis NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Source_Coefficient(1), -1.3938468501173515435) &&
        soft_equiv(spAngle_Operator->Source_Coefficient(2), -0.57735026918962573106) &&
        soft_equiv(spAngle_Operator->Source_Coefficient(4), -1.3938468501173515435) &&
        soft_equiv(spAngle_Operator->Source_Coefficient(5), -0.57735026918962573106))
    {
        ut.passes("sources correct");
    }
    else
    {
        ut.failure("sources NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(1), 3.4142135623730949234) &&
        soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(2), 1.4142135623730951455) &&
        soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(4), 3.4142135623730949234) &&
        soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(5), 1.4142135623730951455))
    {
        ut.passes("bookkeeping correct");
    }
    else
    {
        ut.failure("bookkeeping NOT correct");
    }

    if (Angle_Operator::is_compatible(spQuad, rtt_mesh_element::AXISYMMETRIC, 2, cerr))
    {
        ut.passes("compatibility correct");
    }
    else
    {
        ut.failure("compatibility NOT correct");
    }

    vector<double> ord = spAngle_Operator->Projected_Ordinate(0);
    if (ord.size()==2 &&
        soft_equiv(ord[0], -sqrt(2.0/3.0)) &&
        soft_equiv(ord[1], -1/sqrt(3.0)))
    {
        ut.passes("bookkeeping correct");
    }
    else
    {
        ut.failure("bookkeeping NOT correct");
    }
}

//---------------------------------------------------------------------------//
void tstAngle_Quadrature_Sphere(ScalarUnitTest &ut)
{
    ostringstream contents;
    
    contents << "type = gauss legendre\n"
             << "order = 2\n"
             << "end\n"
             << endl;
    
    rtt_parser::String_Token_Stream tokens( contents.str() );
    
    // Create a Quadrature from this set of tokens.

    SP< Quadrature const > spQuad = QuadCreator().quadCreate( tokens );

    SP<Angle_Operator> const spAngle_Operator =
        SP<Angle_Operator>(new Angle_Operator(spQuad,
                                              rtt_mesh_element::SPHERICAL,
                                              1));

    if (spAngle_Operator->Number_Of_Levels()==1)
    {
        ut.passes("Number of levels correct");
    }
    else
    {
        ut.failure("Number of levels NOT correct");
    }

    if (!spAngle_Operator->Is_Dependent(0) &&
        spAngle_Operator->Is_Dependent(1))
    {
        ut.passes("Dependencies correct");
    }
    else
    {
        ut.failure("Dependencies NOT correct");
    }

    vector<double> const &alpha = spAngle_Operator->Alpha();
    if (alpha.size()==3)
    {
        ut.passes("Number of alphas correct");
    }
    else
    {
        ut.failure("Number of alphas NOT correct");
    }
    if (soft_equiv(alpha[0], 0.0) &&
        soft_equiv(alpha[1], -0.577350269189626) &&
        soft_equiv(alpha[2], 0.0))
    {
        ut.passes("alphas correct");
    }
    else
    {
        ut.failure("alphas NOT correct");
    }

    vector<unsigned> const &levels = spAngle_Operator->Levels();
    if (levels.size()==3)
    {
        ut.passes("Number of levels correct");
    }
    else
    {
        ut.failure("Number of levels NOT correct");
    }
    if (levels[0] == 0 &&
        levels[1] == 0 &&
        levels[2] == 0)
    {
        ut.passes("levels correct");
    }
    else
    {
        ut.failure("levels NOT correct");
    }
    
    vector<double> const &tau = spAngle_Operator->Tau();
    if (tau.size()==3)
    {
        ut.passes("Number of taus correct");
    }
    else
    {
        ut.failure("Number of taus NOT correct");
    }
    if (soft_equiv(tau[0], 1.0) &&
        soft_equiv(tau[1], 0.42264973081037443547) &&
        soft_equiv(tau[2], 0.57735026918962639719))
    {
        ut.passes("taus correct");
    }
    else
    {
        ut.failure("taus NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Psi_Coefficient(1), -2.7320508075688758609) &&
        soft_equiv(spAngle_Operator->Psi_Coefficient(2), 0.0))
    {
        ut.passes("psis correct");
    }
    else
    {
        ut.failure("psis NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Source_Coefficient(1), -1.5773502691896243988) &&
        soft_equiv(spAngle_Operator->Source_Coefficient(2), -1.1547005383792516842))
    {
        ut.passes("sources correct");
    }
    else
    {
        ut.failure("sources NOT correct");
    }
    
    if (soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(1), 2.3660254037844374864) &&
        soft_equiv(spAngle_Operator->Bookkeeping_Coefficient(2), 1.7320508075688754168))
    {
        ut.passes("bookkeeping correct");
    }
    else
    {
        ut.failure("bookkeeping NOT correct");
    }

    if (Angle_Operator::is_compatible(spQuad, rtt_mesh_element::SPHERICAL, 1, cerr))
    {
        ut.passes("compatibility correct");
    }
    else
    {
        ut.failure("compatibility NOT correct");
    }

    vector<double> ord = spAngle_Operator->Projected_Ordinate(0);
    if (ord.size()==1 &&
        soft_equiv(ord[0], -1.0))
    {
        ut.passes("projection correct");
    }
    else
    {
        ut.failure("projection NOT correct");
    }
}


//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    ScalarUnitTest ut(argc, argv, release);
    try
    {
        tstAngle_Quadrature(ut);
        tstAngle_Quadrature_Sphere(ut);
    }
    catch (std::exception &err)
    {
        std::cout << "ERROR: While testing tAngle_Quadrature, " 
                  << err.what()
                  << endl;
        ut.numFails++;
    }
    catch( ... )
    {
        std::cout << "ERROR: While testing tAngle_Quadrature, " 
                  << "An unknown exception was thrown."
                  << endl;
        ut.numFails++;
    }
    return ut.numFails;
}   

//---------------------------------------------------------------------------//
//                        end of tAngle_Quadrature.cc
//---------------------------------------------------------------------------//
