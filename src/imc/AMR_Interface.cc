//----------------------------------*-C++-*----------------------------------//
// AMR_Interface.cc
// Thomas M. Evans
// Thu Jul 16 09:25:43 1998
//---------------------------------------------------------------------------//
// @> AMR (Rage) interface implementation
//---------------------------------------------------------------------------//

#include "imc/AMR_Interface.hh"
#include "imc/AMR_Builder.hh"
#include "imc/IMC_Manager.hh"
#include "c4/global.hh"
#include "ds++/Assert.hh"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

//===========================================================================//
// RAGE INTERFACE FUNCTION
//===========================================================================//

//---------------------------------------------------------------------------//
// F90 Rage interface functions
//---------------------------------------------------------------------------//
// IMC launcher in Rage

void rage_imc_(int *num_cells, double *node_coord, int *layout, 
	       int *num_b_cells, int *b_proc, int *b_cell)
{
  // stl components
    using std::cout;
    using std::endl;
    using std::ofstream;
    using std::ostringstream;
    using std::string;

  // draco components
    using IMC::OS_Mesh;
    using IMC::AMR_Interface;
    using IMC::AMR_Builder;
    using IMC::IMC_Manager;
    using C4::Wtime;
    using C4::node;
    using dsxx::SP;

  // timing info
    double begin;
    double end;

    if (node() == 0)
    {
      // welcome to IMC
	cout << "*********************************" << endl;
	cout << ">>> MILAGRO, 'a true miracle' <<<" << endl;
	cout << ">>> version 1.0               <<<" << endl;
	cout << ">>> Evans and Urbatsch        <<<" << endl;
	cout << "*********************************" << endl;

      // begining time
	begin = Wtime();
    }

  // barrier sync so everything starts at the same time
    C4::gsync();

  // put the data into an arguments list
//     AMR_Interface::Arguments arg(node_coord, layout, b_proc, b_cell,
// 				 *num_cells, *num_b_cells);
//     SP<AMR_Interface> interface = new AMR_Interface(arg);
//     cout << ">> Interface with RAGE on node " << node() << endl;

//     AMR_Builder amr_build(interface);
//     SP<OS_Mesh> mesh = amr_build.build_Mesh();
//     cout << ">> Built Mesh on node " << node() << endl;

//   // print out the mesh
//     ostringstream stitle;
//     stitle << "mesh." << node();
//     string title = stitle.str();
//     ofstream output(title.c_str());
//     output << *mesh;

//   // make a manager
//     IMC_Manager<OS_Mesh, OS_Builder, OS_Interface> manager;

//   // execute IMC
//     manager.execute_IMC("marshak");

  // ending time
    C4::gsync();
    if (node() == 0)
    {
        end = Wtime();
        std::cout << std::endl << ">> Problem Timing" << std::endl;
        std::cout.precision(4);
        std::cout << " ** We ran for " << std::setw(15)
                  << std::setiosflags(std::ios::scientific)
                  << end-begin << " seconds" << std::endl;
    }

}

//---------------------------------------------------------------------------//
// census definitions

GLOBALSPACE

dsxx::SP<IMC::Particle_Buffer<IMC::Particle<IMC::OS_Mesh> >::Census> 
host_census;

CSPACE

//===========================================================================//
// class AMR_Interface
//===========================================================================//

IMCSPACE

//---------------------------------------------------------------------------//
// Arguments constructor
//---------------------------------------------------------------------------//

AMR_Interface::Arguments::Arguments(const double *nc, const int *l, 
				    const int *bp, const int *bc, 
				    const double *de, const double *r,
				    const double *op_abs, const double *t,
				    const double *re, int numc,
				    int numbc, double imp, double dt,
				    double dndt, int nnom, int nmax, 
				    double rst, int s, int b, int pf)
    : node_coord(nc), layout(l), b_proc(bp), b_cell(bc), dedt(de), rho(r),
      opacity_abs(op_abs), tev(t), rev(re), num_cells(numc),
      num_b_cells(numbc), implicitness(imp), delta_t(dt), dnpdt(dndt),
      npnom(nnom), npmax(nmax), rad_s_tend(rst), seed(s), buffer(b),
      print_f(pf) 
{
    Require (num_cells != 0);
}

//---------------------------------------------------------------------------//
// AMR_Interface Constructor
//---------------------------------------------------------------------------//

AMR_Interface::AMR_Interface(const Arguments &arg)
    : arguments(arg), evol_ext(arguments.num_cells), ss_pos(0), ss_temp(0)
{
    Require (arguments.num_cells != 0);
}

CSPACE

//---------------------------------------------------------------------------//
//                              end of AMR_Interface.cc
//---------------------------------------------------------------------------//
