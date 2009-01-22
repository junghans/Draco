//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/Compare.cc
 * \author Mike Buksas
 * \date   Thu May  1 14:42:10 2008
 * \brief  
 * \note   Copyright (C) 2006 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "Compare.hh"
#include "global.hh"
#include "ds++/Soft_Equivalence.hh"

namespace rtt_c4
{

//---------------------------------------------------------------------------//
/*!
 * \brief Function to check the equivalence of an int across all processors.
 *
 * This function is (hopefully) a temporary parallel check function that more
 * properly belongs in C4.  It is used to check the equivalence of a given
 * integer across all processors.  This is used for Design By Contract
 * analysis in the Source_Builder codes.
 *
 * \param local_value integer value to check against
 * \return true if equivalent across all processors; false if not
 */
bool check_global_equiv(int local_value)
{

    const int node  = rtt_c4::node();
    const int nodes = rtt_c4::nodes();

    bool pass = false;
    
    // return true if serial, if not then do check on all processors
    if (nodes == 1)
	pass = true;
    else
    {
	// value from processor above local processor
	int neighbors_value;

	if (node > 0 && node < nodes - 1)
	{
            rtt_c4::send(&local_value, 1, node - 1, 600);
            rtt_c4::receive(&neighbors_value, 1, node + 1, 600);
	    if (local_value == neighbors_value) pass = true;
	}
	else if (node == nodes - 1)
	{
            rtt_c4::send(&local_value, 1, node - 1, 600);
	    pass = true;
	}
	else if (node == 0)
	{
            rtt_c4::receive(&neighbors_value, 1, node + 1, 600);
	    if (local_value == neighbors_value) pass = true;
	}
	else
	{
	    Insist(0, "Something is wrong with nodes!");
	}
    }

    // sync everything so we don't leave before all processors are finished
    rtt_c4::global_barrier();

    // return result
    return pass;
}

//---------------------------------------------------------------------------//
/*!  
 * \brief Function to check the equivalence of a double across all
 * processors.
 *
 * This function is the same as check_global_equiv(int) except that doubles
 * are compared to precision eps.
 *
 * \param local_value integer value to check against
 * \param eps precision of double, default 1e-8
 * \return true if equivalent across all processors; false if not 
 */
bool check_global_equiv(double local_value, double eps)
{
    using rtt_dsxx::soft_equiv;

    // nodes
    int node  = rtt_c4::node();
    int nodes = rtt_c4::nodes();

    // passing condition
    bool pass = false;
    
    // return true if serial, if not then do check on all processors
    if (nodes == 1)
	pass = true;
    else
    {
	// value from processor above local processor
	double neighbors_value;

	if (node > 0 && node < nodes - 1)
	{
            rtt_c4::send(&local_value, 1, node - 1, 600);
            rtt_c4::receive(&neighbors_value, 1, node + 1, 600);
	    pass = soft_equiv(neighbors_value, local_value, eps);
	}
	else if (node == nodes - 1)
	{
            rtt_c4::send(&local_value, 1, node - 1, 600);
	    pass = true;
	}
	else if (node == 0)
	{
            rtt_c4::receive(&neighbors_value, 1, node + 1, 600);
	    pass = soft_equiv(neighbors_value, local_value, eps);
	}
	else
	{
	    Insist(0, "Something is wrong with nodes!");
	}
    }

    // sync everything so we don't leave before all processors are finished
    rtt_c4::global_barrier();

    // return result
    return pass;
}




} // end namespace rtt_c4

//---------------------------------------------------------------------------//
//                 end of Compare.cc
//---------------------------------------------------------------------------//