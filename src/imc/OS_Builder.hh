//----------------------------------*-C++-*----------------------------------//
// OS_Builder.hh
// Thomas M. Evans
// Mon Feb  9 16:16:07 1998
//---------------------------------------------------------------------------//
// @> OS_Builder class header file
//---------------------------------------------------------------------------//

#ifndef __imctest_OS_Builder_hh__
#define __imctest_OS_Builder_hh__

//===========================================================================//
// class OS_Builder - 
//
// Purpose : builds an OS_Mesh object, parses the input file, 
//           redistributes the Mesh for parallel purposes
//
// revision history:
// -----------------
//  0) original
//  1)  3-18-98 : added generalized mesh constructor which consists of 
//                calculating vertex-based arrays and sending them to the 
//                OS_Mesh constructor
// 
//===========================================================================//

#include "imctest/Names.hh"
#include "imctest/Coord_sys.hh"
#include "imctest/Layout.hh"
#include "imctest/OS_Mesh.hh"
#include "imctest/OS_Interface.hh"
#include "ds++/SP.hh"
#include <vector>
#include <string>

IMCSPACE

using std::vector;
using std::string;

class OS_Builder
{
private:
  // data from Parser needed to build mesh

  // coordinate system string
    string coord_system;
  // number of fine_cells along each dimension
    OS_Mesh::CCVF_a fine_edge;
  // boundary conditions
    vector<string> bnd_cond;
  
  // member functions for building OS_Mesh

  // build Layout helper functions
    SP<Layout> build_Layout(const Coord_sys &);
    void assign2D(Layout &);
    void assign3D(Layout &);

  // build Coord_sys helper functions
    SP<Coord_sys> build_Coord();

  // build Mesh helper functions
    SP<OS_Mesh> build_2DMesh(SP<Coord_sys>, Layout &);
    SP<OS_Mesh> build_3DMesh(SP<Coord_sys>, Layout &);

  // Begin_Doc os_builder-int.tex
  // Begin_Verbatim 

public:
  // constructor
    inline explicit OS_Builder(SP<OS_Interface>);

  // build Mesh member functions
    SP<OS_Mesh> build_Mesh();
    
  // End_Verbatim 
  // End_Doc 
};

//---------------------------------------------------------------------------//
// inline functions for OS_Builder
//---------------------------------------------------------------------------//

inline OS_Builder::OS_Builder(SP<OS_Interface> interface)
{
  // get data arrays from OS_Interface needed to build OS_Mesh
    coord_system = interface->get_coordinates();
    fine_edge    = interface->get_fine_edge();
    bnd_cond     = interface->get_boundaries();
}

CSPACE

#endif                          // __imctest_OS_Builder_hh__

//---------------------------------------------------------------------------//
//                              end of imctest/OS_Builder.hh
//---------------------------------------------------------------------------//
