//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   mesh/Mesh_DB.cc
 * \author Chris Gesh
 * \date   Mon Mar 13 08:09:09 2000
 * \brief  
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "Mesh_DB.hh"
#include "ds++/Assert.hh"

#include "nml/Group.hh"
#include "nml/Items.hh"

namespace rtt_mesh
{

Mesh_DB::Mesh_DB(int ncx_in, int ncy_in, int ncz_in,
		 double xmin_in, double xmax_in, double ymin_in,
		 double ymax_in, double zmin_in, double zmax_in,
		 bool dumpi_in, bool dumpc_in)
    : ncx(ncx_in), ncy(ncy_in), ncz(ncz_in), xmin(xmin_in), xmax(xmax_in),
      ymin(ymin_in), ymax(ymax_in), zmin(zmin_in), zmax(zmax_in),
      dump_indicies(dumpi_in), dump_coords(dumpc_in),
      dx(ncx_in), dy(ncy_in), dz(ncz_in)
{
    double deltax;
    double deltay;
    double deltaz;

    deltax = (xmax-xmin)/ncx;
    deltay = (ymax-ymin)/ncy;
    deltaz = (zmax-zmin)/ncz;

    for(int i=0; i < ncx; i++)
	dx(i) = deltax;
    for(int i=0; i < ncy; i++)
	dy(i) = deltay;
    for(int i=0; i < ncz; i++)
	dz(i) = deltaz;
}

Mesh_DB::Mesh_DB(const rtt_dsxx::Mat1<double> &dx_in,
		 const rtt_dsxx::Mat1<double> &dy_in,
		 const rtt_dsxx::Mat1<double> &dz_in,
		 int ncx_in, int ncy_in, int ncz_in,
		 double xmin_in, double xmax_in, double ymin_in,
		 double ymax_in, double zmin_in, double zmax_in,
		 bool dumpi_in, bool dumpc_in)
    : ncx(ncx_in), ncy(ncy_in), ncz(ncz_in), xmin(xmin_in), xmax(xmax_in),
      ymin(ymin_in), ymax(ymax_in), zmin(zmin_in), zmax(zmax_in),
      dump_indicies(dumpi_in), dump_coords(dumpc_in)

{
    Assert(dx_in.size() == ncx);
    Assert(dy_in.size() == ncy);
    Assert(dz_in.size() == ncz);

    dx = dx_in;
    dy = dy_in;
    dz = dz_in;
}

void Mesh_DB::setup_namelist( NML_Group& g )
{
#include ".nml_mesh.cc"

    dx.redim(ncx);
    dy.redim(ncy);
    dz.redim(ncz);
    
    double deltax;
    double deltay;
    double deltaz;

    deltax = (xmax-xmin)/ncx;
    deltay = (ymax-ymin)/ncy;
    deltaz = (zmax-zmin)/ncz;

    for(int i=0; i < ncx; i++)
	dx(i) = deltax;
    for(int i=0; i < ncy; i++)
	dy(i) = deltay;
    for(int i=0; i < ncz; i++)
	dz(i) = deltaz;

}

} // end namespace rtt_mesh


//---------------------------------------------------------------------------//
//                              end of Mesh_DB.cc
//---------------------------------------------------------------------------//
