//----------------------------------*-C++-*----------------------------------//
// Shadow_CAR_CU_Mesh_Builder.cc
// B.T. Adams (bta@lanl.gov)
// 27 Sept 99
//---------------------------------------------------------------------------//
// @> Shadow_CAR_CU_Mesh_Builder interface file
//---------------------------------------------------------------------------//

#ifndef __mc_Shadow_CAR_CU_Mesh_Builder_cc__
#define __mc_Shadow_CAR_CU_Mesh_Builder_cc__

#include "CAR_CU_Builder.hh"
#include "CAR_CU_Interface.hh"
#include "RTT_Format.hh"
#include "CAR_CU_Mesh.hh"
#include "Shadow_Opaque_Pointers.hh"
#include <iostream>

//===========================================================================//
// Shadow_CAR_CU_Mesh_Builder - 
//
// Purpose : Provides flat interface functions to the Continuous Adaptive 
// Refinement Cartesion Unstructured Mesh Builder Class for use with Fortran.
//
// revision history:
// -----------------
//  0) original
// 
//===========================================================================//

namespace rtt_mc 
{
using std::cout;

// draco components
using dsxx::SP;
using rtt_imc::CAR_CU_Interface;
using rtt_format::RTT_Format;

extern "C" 
{
//---------------------------------------------------------------------------//
// CAR_CU_Builder F90 to C++ flat interface functions
//---------------------------------------------------------------------------//

//===========================================================================//
// Constructors and destructors
//===========================================================================//
//

    // Construct a CAR_CU_Builder class from a Fortran 90 program call.  This 
    // also constructs the Coord_sys, Layout, and CAR_CU_Mesh class objects.
    // The addresses of both the new CAR_CU_Builder and CAR_CU_Mesh class 
    // objects are set. The CAR_CU_Mesh class contains member functions to 
    // return the addresses of the Coord and Layout class objects, if needed.
    void construct_car_cu_mesh_builder_(long & self, long & itf_ptr, 
					long & rttf_ptr, long & verbosity, 
					long & mesh_ptr)
    {
	bool verbose = verbosity;
	SP<CAR_CU_Interface> interface;
	SP<RTT_Format> rttFormat;
	SP<CAR_CU_Mesh> mesh;

	// Get the addresses of the CAR_CU_Interface (int_ptr) and RTT_Format 
        // (rttf_ptr) class objects.
	interface = opaque_pointers<CAR_CU_Interface>::item(itf_ptr);
	rttFormat = opaque_pointers<RTT_Format>::item(rttf_ptr);

	// Construct a new CAR_CU_Builder class object.
	SP<CAR_CU_Builder> builder(new CAR_CU_Builder(interface));

	// Construct a new CAR_CU_Mesh class object and build the mesh.
	mesh = builder->build_Mesh(rttFormat);
	interface->set_defined_surcells(builder->get_defined_surcells());
	if (verbose)
	    cout << " ** Built mesh ** " << endl;

	// return the addresses of the new CAR_CU_Builder (self) and 
	// CAR_CU_Mesh (mesh_ptr) objects.
	self = opaque_pointers<CAR_CU_Builder>::insert(builder);
	mesh_ptr = opaque_pointers<CAR_CU_Mesh>::insert(mesh);

    }

    // Destroy a CAR_CU_Builder class object from a Fortran 90 program call.
    void destruct_car_cu_mesh_builder_(long & self)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// destroy the CAR_CU_Builder class object by assigning this SP to 
        // a null SP
	builder = SP<CAR_CU_Builder>();
	Ensure (!builder);

	// remove the opaque pointer to the CAR_CU_Interface class object.
	opaque_pointers<CAR_CU_Builder>::erase(self);
    }

//===========================================================================//
// General mesh builder accessor functions
//===========================================================================//
    // Return the number of sets of grouped surface source cells.
    void get_car_cu_ss_size_(long & self, long & ss_size)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	int isize = builder->get_ss_size();

	// Cast the int variable to long
	ss_size = static_cast<long>(isize);
    }

    // Return the number of grouped surface source cells in a given set.
    void get_car_cu_ss_set_size_(long & self, long & surface, long & ss_size)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// Cast the long variable to int
	int isurface = static_cast<int>(surface);
	Insist(isurface > 0 && isurface <= builder->get_ss_size(), 
	    "Invalid surface source number passed to get_ss_set_pos_!");

	int isize = builder->get_ss_size(isurface);

	// Cast the int variable to long
	ss_size = static_cast<long>(isize);
    }

    // Return the position (lox, hix, etc.) of all of the grouped surface 
    // source cell sets.
    void get_car_cu_ss_pos_(long & self, long & surface, char * pos, 
			    long & ss_pos_size)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// Cast the long variable to int
	int isurface = static_cast<int>(surface);
	int isize = static_cast<int>(ss_pos_size);

	Insist(isurface > 0 && isurface <= builder->get_ss_size(), 
	    "Invalid surface source number passed to get_ss_pos_!");
	Insist(isize == builder->get_ss_size(), 
	    "Invalid surface source position size passed to get_ss_pos_!");

	vector<string> ss_pos = builder->get_ss_pos();

	for (int surf = 0; surf < ss_pos.size(); surf++)
	{
	    for (int ind = 0; ind < ss_pos[surf].size(); ind ++)
	    {
	        * pos = ss_pos[surf][ind];
		++pos;
	    }
	}
    }

    // Return the position (lox, hix, etc.) of a set of grouped surface source 
    // cells.
    void get_car_cu_ss_set_pos_(long & self, long & surface, char * pos)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// Cast the long variable to int
	int isurface = static_cast<int>(surface);

	Insist(isurface > 0 && isurface <= builder->get_ss_size(), 
	    "Invalid surface source number passed to get_ss_set_pos_!");

	string ss_pos = builder->get_ss_pos(isurface);

	for (int ind = 0; ind < ss_pos.size(); ind ++)
	{
	    * pos = ss_pos[ind];
	    ++pos;
	}
    }

    // Return all of the defined surface source cell sets.
    void get_car_cu_ss_cells_(long & self, long & ss_set, long & ss_size)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// Cast the long variables to int
	int isize = static_cast<int>(ss_size);
	long * data_array = & ss_set;

	int size_check = 0;
	for (int surf = 1; surf <= builder->get_ss_size(); surf++)
	    size_check += builder->get_ss_size(surf);

	Insist(isize == size_check, 
	    "Invalid surface source size passed to get_ss_cells_!");

	vector<vector<int> > iss_set = builder->get_defined_surcells();

	for (int surf = 0; surf < iss_set.size(); surf++)
	{
	    for (int cell = 0; cell < iss_set[surf].size(); cell++)
	    {
	        * data_array = static_cast<long>(iss_set[surf][cell]);
		++data_array;
	    }
	}
    }

    // Return the defined surface source cells in a given set.
    void get_car_cu_ss_cell_set_(long & self, long & surface, long & ss_set, 
				 long & ss_size)
    {
	// Get the address of the CAR_CU_Builder class object (self).
	SP<CAR_CU_Builder> builder = 
	    opaque_pointers<CAR_CU_Builder>::item(self);

	// Cast the long variables to int
	int isurface = static_cast<int>(surface);
	int isize = static_cast<int>(ss_size);
	long * data_array = & ss_set;

	Insist(isurface > 0 && isurface <= builder->get_ss_size(), 
	    "Invalid surface source number passed to get_ss_cell_set_!");
	Insist(isize == builder->get_ss_size(isurface), 
	    "Invalid surface source size passed to get_ss_cell_set_!");

	vector<int> iss_set = builder->get_defined_surcells(isurface);

	for (int cell = 0; cell < iss_set.size(); cell++)
	{
	    * data_array = static_cast<long>(iss_set[cell]);
	    ++data_array;
	}
    }

}  // end extern "C"


}  // end namespace rtt_mc

#endif                          // __mc_Shadow_CAR_CU_Mesh_Builder_cc__

//---------------------------------------------------------------------------//
//                        end of amr_mesh/Shadow_CAR_CU_Mesh_Builder.cc
//---------------------------------------------------------------------------//
