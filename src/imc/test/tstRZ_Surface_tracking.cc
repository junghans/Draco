//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   imc/test/tstRZ_Surface_tracking.cc
 * \author Mike Buksas
 * \date   Wed Jul  9 10:44:54 2003
 * \brief  Unit test for the surface tracking feature on RZ meshes.  
 * \note   Copyright © 2003 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cmath>

#include "ds++/SP.hh"
#include "ds++/Assert.hh"
#include "../Release.hh"
#include "imc_test.hh"
#include "mc/RZWedge_Mesh.hh"
#include "mc/RZWedge_Builder.hh"
#include "mc/Global_Mesh_Data.hh"
#include "mc/Rep_Topology.hh"
#include "IMC_Test.hh"

#include "../Opacity.hh"
#include "../Tally.hh"
#include "../Frequency.hh"
#include "../Fleck_Factors.hh"
#include "../Random_Walk.hh"
#include "../Gray_Particle.hh"
#include "../Multigroup_Particle.hh"
#include "../Extrinsic_Surface_Tracker.hh"
#include "../Extrinsic_Tracker_Builder.hh"
#include "../Azimuthal_Mesh.hh"
#include "rng/Random.hh"
#include "mc/Sphere.hh"
#include "ds++/Soft_Equivalence.hh"

using rtt_dsxx::SP;
using rtt_dsxx::soft_equiv;
using rtt_rng::Sprng;
using rtt_rng::Rnd_Control;
using namespace std;
using namespace rtt_mc;
using namespace rtt_imc;
using namespace rtt_imc_test;

// random number seed
int seed = 395731;

//---------------------------------------------------------------------------//
// Test interface implemetation
//---------------------------------------------------------------------------//

struct Surface_Tracking_Tester : public Surface_Tracking_Interface
{

    vector<Surface_Descriptor> descriptor;

    vector<double> bin_cosines;

    Surface_Tracking_Tester();

    int number_of_surfaces() const { return 3; }

    const vector<Surface_Descriptor>& get_surface_data() const 
    { 
	return descriptor;
    }

    const vector<double>& get_bin_cosines() const { return bin_cosines; }
    
    ~Surface_Tracking_Tester() { /* ... */ }
};

Surface_Tracking_Tester::Surface_Tracking_Tester()
{
    descriptor.resize(3);

    descriptor[0].type = Surface_Descriptor::SPHERE;
    descriptor[0].data.resize(2);
    descriptor[0].data[0] =  0.0;
    descriptor[0].data[1] =  2.0;
    
    descriptor[1].type = Surface_Descriptor::SPHERE;
    descriptor[1].data.resize(2);
    descriptor[1].data[0] =  1.0;
    descriptor[1].data[1] =  2.0;
    
    descriptor[2].type = Surface_Descriptor::SPHERE;
    descriptor[2].data.resize(2);
    descriptor[2].data[0] = -1.0;
    descriptor[2].data[1] =  2.0;

    double bin_data[5] = {-1.0, -0.5*sqrt(2.0), 0.0, 0.5*sqrt(2.0), 1.0};
    bin_cosines.assign(bin_data, bin_data+5);
}

//---------------------------------------------------------------------------//
// BUILDERS
//---------------------------------------------------------------------------//

SP<RZWedge_Mesh> build_an_RZWedge()
{
    // Make a builder from the RZWedge input
    SP<Parser> parser(new Parser("RZWedge_Input"));
    RZWedge_Builder builder(parser);

    SP<RZWedge_Mesh> mesh = builder.build_Mesh();

    Ensure(mesh);

    return mesh;
}

//---------------------------------------------------------------------------//

SP<Azimuthal_Mesh> build_an_az_mesh(const Surface_Tracking_Interface &interface)
{
    SP<Azimuthal_Mesh> az_mesh ( new Azimuthal_Mesh(interface) );

    Ensure (az_mesh);

    return az_mesh;
}

//---------------------------------------------------------------------------//

SP<Extrinsic_Surface_Tracker> build_extrinsic_tracker(
    const RZWedge_Mesh& mesh,
    const Surface_Tracking_Interface& interface)
{
    SP<Topology> topology(new Rep_Topology(mesh.num_cells()));
    Global_Mesh_Data<RZWedge_Mesh> md(topology, mesh);
    
    Extrinsic_Tracker_Builder<RZWedge_Mesh> builder(mesh, md, interface);

    SP<Extrinsic_Surface_Tracker> tracker = builder.build_tracker();

    Ensure(tracker);

    return tracker;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Create a gray opacity with sigma = 1.0 everywhere on the given mesh
 * 
 * \param mesh the mesh
 * \return the opacity object
 */
SP<Opacity<RZWedge_Mesh, Gray_Frequency> > build_a_gray_opacity(
    SP<RZWedge_Mesh> mesh)
{
    SP<Gray_Frequency> frequency ( new Gray_Frequency );

    RZWedge_Mesh::CCSF<double> absorption(mesh);
    RZWedge_Mesh::CCSF<double> thomson(mesh);

    SP<Fleck_Factors<RZWedge_Mesh> > fleck ( 
	new Fleck_Factors<RZWedge_Mesh>(mesh) );

    for (int cell = 1; cell != mesh->num_cells(); ++cell)
    {
	fleck->fleck(cell) = 1.0;
	absorption(cell) = 1.0;
	thomson(cell) = 0.0;
    }

    SP<Opacity<RZWedge_Mesh, Gray_Frequency> > 	opacity 
	( new  Opacity<RZWedge_Mesh, Gray_Frequency>
	  (frequency, absorption, thomson, fleck)  
	    );
    
    Ensure (opacity);

    return opacity;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Create a multigroup opacity with sigma = 0.0 everywhere on the
 * given mesh 
 * 
 * \param mesh the mesh
 * \return the opacity object
 */
SP<Opacity<RZWedge_Mesh, Multigroup_Frequency> > build_a_mg_opacity(
    SP<RZWedge_Mesh> mesh)
{
    vector<double> grps(3);
    grps[0] = 0.01;
    grps[1] = 0.1;
    grps[2] = 1.0;
    SP<Multigroup_Frequency> frequency(new Multigroup_Frequency(grps));
    
    // make cross sections
    RZWedge_Mesh::CCSF<vector<double> > planck(mesh);
    RZWedge_Mesh::CCSF<double>          int_planck(mesh);
    vector<double>                      xs(2, 0.0);

    SP<Fleck_Factors<RZWedge_Mesh> >    
	fleck(new Fleck_Factors<RZWedge_Mesh>(mesh));
    
    for (int i = 1; i <= mesh->num_cells(); i++)
    {
	int_planck(i)   = 1.0;
	planck(i)       = xs;
	fleck->fleck(i) = 1.0;
    }
    
    SP<Opacity<RZWedge_Mesh, Multigroup_Frequency> > opacity 
	( new Opacity<RZWedge_Mesh, Multigroup_Frequency> 
	  (frequency, planck, planck,
	   fleck, int_planck, planck) );
    
    Ensure (opacity);

    return opacity;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Make a diffusion opacity
 * 
 * \param mesh the mesh
 * \return the diffusion opacity object via smart pointer
 */
SP<Diffusion_Opacity<RZWedge_Mesh> > get_diff_opacity(SP<RZWedge_Mesh> mesh)
{
    // make data
    SP<Fleck_Factors<RZWedge_Mesh> > fleck(
	new Fleck_Factors<RZWedge_Mesh>(mesh));
    RZWedge_Mesh::CCSF<double> ross(mesh);
    RZWedge_Mesh::CCSF<double> scat(mesh);

    for (int i = 1; i <= mesh->num_cells(); i++)
    {
	fleck->fleck(i) = 1.0 / 2.5;
	ross(i)         = 100.0 + static_cast<double>(i);
	scat(i)         = 0.0;
    }

    // make the diffusion opacity
    SP<Diffusion_Opacity<RZWedge_Mesh> > diff(
	new Diffusion_Opacity<RZWedge_Mesh>(fleck, ross, scat));

    Ensure(diff);
					
    return diff;
}

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void test_gray_particle_straight()
{
    // Make a mesh
    SP<RZWedge_Mesh> mesh = build_an_RZWedge();

    std::cout << "Num cells: " << mesh->num_cells() << std::endl;
    for (int cell = 1; cell <= mesh->num_cells(); ++cell)
    {
	std::cout << "Cell: " << cell << std::endl;
	std::cout << "\t x: " << mesh->get_low_x(cell) << "," 
		  << mesh->get_high_x(cell) << std::endl;
	std::cout << "\t z: " << mesh->get_low_z(cell) << "," 
		  << mesh->get_high_z(cell) << std::endl << std::endl;
    }

    // Make an test interface object
    Surface_Tracking_Tester tester;
    
    // Make an azimuthal mesh
    SP<Azimuthal_Mesh> az_mesh = build_an_az_mesh(tester);

    // Make a surface-tracker
    SP<Extrinsic_Surface_Tracker> tracker = 
	build_extrinsic_tracker(*mesh, tester);

    // Make a tally
    SP<Tally<RZWedge_Mesh> > tally ( new Tally<RZWedge_Mesh>(mesh) );

    // Make a surface tracking sub-tally
    SP<Surface_Sub_Tally> surface_tally ( 
	new Surface_Sub_Tally(az_mesh, tester));
    tally->assign_Surface_Sub_Tally(surface_tally);

    // Make a gray opacity
    SP<Opacity<RZWedge_Mesh, Gray_Frequency> > opacity =
	build_a_gray_opacity(mesh);

    // Make an (empty) random walk pointer
    SP<Random_Walk<RZWedge_Mesh> > rwalk;

    // Make a particle
    vector<double> r(3), o(3);
    Rnd_Control control(seed);
    Sprng rnd = control.get_rn(1);

    r[0] = 1.0;
    r[1] = 0.0;
    r[2] =-1.0;

    o[0] = 0.0;
    o[1] = 0.0;
    o[2] = 1.0;

    double ew = 1.0;
    int cell = mesh->get_cell(r);
    Gray_Particle<RZWedge_Mesh> particle(r, o, ew, cell, rnd, 1.0, 1.0);

    // Do the transport
    particle.transport(*mesh, *opacity, *tally, rwalk, tracker);

    // Check the results
    if (particle.status() != false ) ITFAILS;

    if (particle.get_descriptor() != 
	Gray_Particle<RZWedge_Mesh>::ESCAPE) ITFAILS;

    // Check the tallies:

    // 1st: inward across surface 2
    if (!soft_equiv(surface_tally->weight(2, false, 4) , exp(-2.0+sqrt(3.0))))
	ITFAILS;

    // 2nd: outward across surface 1
    if (!soft_equiv(surface_tally->weight(1, true,  4) , exp(-1.0-sqrt(3.0))))
	ITFAILS;

    // 3rd: outward across surface 3
    if (!soft_equiv(surface_tally->weight(3, true,  4) , exp(-sqrt(3.0))))
	ITFAILS;

    // 4th: outrward across surface 2
    if (!soft_equiv(surface_tally->weight(2, true,  4) , exp(-2.0-sqrt(3.0))))
	ITFAILS;

    if (surface_tally->crossings(2, false, 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(1, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(3, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(2, true , 4) != 1 ) ITFAILS;
}

//---------------------------------------------------------------------------//

void test_gray_particle_rw()
{

    // Make an test interface object
    Surface_Tracking_Tester tester;

    // Make a mesh
    SP<RZWedge_Mesh> mesh = build_an_RZWedge();

    // Make an azimuthal mesh
    SP<Azimuthal_Mesh> az_mesh = build_an_az_mesh(tester);

    // Make a surface-tracker
    SP<Extrinsic_Surface_Tracker> tracker = 
	build_extrinsic_tracker(*mesh, tester);

    // Make a tally
    SP<Tally<RZWedge_Mesh> > tally ( new Tally<RZWedge_Mesh>(mesh) );

    // Make a surface tracking sub-tally
    SP<Surface_Sub_Tally> surface_tally ( new Surface_Sub_Tally(az_mesh, 
								tester));
    SP<Random_Walk_Sub_Tally> rw_tally(new Random_Walk_Sub_Tally);
    tally->assign_Surface_Sub_Tally(surface_tally);
    tally->assign_RW_Sub_Tally(rw_tally);

    // Make a gray opacity
    SP<Opacity<RZWedge_Mesh, Gray_Frequency> > opacity =
	build_a_gray_opacity(mesh);

    // Make a diffusion opacity
    SP<Diffusion_Opacity<RZWedge_Mesh> > diff = get_diff_opacity(mesh);

    // make random walk
    SP<Random_Walk<RZWedge_Mesh> > rw( 
	new Random_Walk<RZWedge_Mesh>(mesh, diff) );

    // Make a particle
    vector<double> r(3), o(3);
    Rnd_Control control(seed);
    Sprng rnd = control.get_rn(1);

    r[0] = 1.0;
    r[1] = 0.0;
    r[2] =-1.0;

    o[0] = 0.0;
    o[1] = 0.0;
    o[2] = 1.0;

    double ew = 1.0;
    int cell = mesh->get_cell(r);
    Gray_Particle<RZWedge_Mesh> particle(r, o, ew, cell, rnd, 1.0, 1.0);

    // Do the transport
    particle.transport(*mesh, *opacity, *tally, rw, tracker);

    // Check the results
    if (particle.status() != false ) ITFAILS;

    if (particle.get_descriptor() != 
	Gray_Particle<RZWedge_Mesh>::ESCAPE) ITFAILS;

    // Check the tallies:

    // 1st: inward across surface 2
    if (!soft_equiv(surface_tally->weight(2, false, 4) , exp(-2.0+sqrt(3.0))))
	ITFAILS;

    // 2nd: outward across surface 1
    if (!soft_equiv(surface_tally->weight(1, true,  4) , exp(-1.0-sqrt(3.0))))
	ITFAILS;

    // 3rd: outward across surface 3
    if (!soft_equiv(surface_tally->weight(3, true,  4) , exp(-sqrt(3.0))))
	ITFAILS;

    // 4th: outrward across surface 2
    if (!soft_equiv(surface_tally->weight(2, true,  4) , exp(-2.0-sqrt(3.0))))
	ITFAILS;

    if (surface_tally->crossings(2, false, 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(1, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(3, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(2, true , 4) != 1 ) ITFAILS;

    SP<Random_Walk_Sub_Tally> rwsb = tally->get_RW_Sub_Tally();

    if (rwsb->get_accum_n_random_walks() != 0) ITFAILS;
}

//---------------------------------------------------------------------------//

void test_mg_particle()
{
    // Make an test interface object
    Surface_Tracking_Tester tester;

    // Make a mesh
    SP<RZWedge_Mesh> mesh = build_an_RZWedge();

    // Make an azimuthal mesh
    SP<Azimuthal_Mesh> az_mesh = build_an_az_mesh(tester);

    // Make a surface-tracker
    SP<Extrinsic_Surface_Tracker> tracker = 
	build_extrinsic_tracker(*mesh, tester);

    // Make a tally
    SP<Tally<RZWedge_Mesh> > tally ( new Tally<RZWedge_Mesh>(mesh) );

    // Make a multigroup opacity
    SP<Opacity<RZWedge_Mesh, Multigroup_Frequency> > opacity = 
	build_a_mg_opacity(mesh);

    // Make a surface tracking sub-tallies
    vector<SP<Surface_Sub_Tally> > surface_tallies(
        opacity->get_Frequency()->get_num_groups()); 

    if (surface_tallies.size() != 2) ITFAILS;

    for (int i = 0; i < 2; i++)
    {
        SP<Surface_Sub_Tally> surface_tally (
            new Surface_Sub_Tally(az_mesh, tester));
        surface_tallies[i] = surface_tally;
    }
    tally->assign_Surface_Sub_Tally(surface_tallies);

    // Make an (empty) random walk pointer
    SP<Random_Walk<RZWedge_Mesh> > rwalk;

    // Make a particle
    vector<double> r(3), o(3);
    Rnd_Control control(seed);
    Sprng rnd = control.get_rn(1);

    r[0] = 1.0;
    r[1] = 0.0;
    r[2] =-1.0;

    o[0] = 0.0;
    o[1] = 0.0;
    o[2] = 1.0;

    double ew = 1.0;
    int cell = mesh->get_cell(r);
    Multigroup_Particle<RZWedge_Mesh> particle(r, o, ew, cell, rnd, 2, 1.0, 1.0);

    // Do the transport
    particle.transport(*mesh, *opacity, *tally, rwalk, tracker);

    // Check the results
    if (particle.status() != false ) ITFAILS;

//     vector<double> final_r ( particle.get_r() );
//     const double correct_final_r[3] = {1.0, 0.0, 3.0};
//     if (!soft_equiv(final_r.begin(), final_r.end(), 
// 		    correct_final_r, correct_final_r+3) ) ITFAILS;

    // Check the tallies:

    SP<Surface_Sub_Tally> surface_tally;

    // all results are in group 2
    surface_tally = tally->get_Surface_Sub_Tally(2);
    if (!surface_tally) ITFAILS;

    // 1st: inward across surface 2
    if (!soft_equiv(surface_tally->weight(2, false, 4) , 1.0))
	ITFAILS;

    // 2nd: outward across surface 1
    if (!soft_equiv(surface_tally->weight(1, true,  4) , 1.0))
	ITFAILS;

    // 3rd: outward across surface 3
    if (!soft_equiv(surface_tally->weight(3, true,  4) , 1.0))
	ITFAILS;

    // 4th: outward across surface 2
    if (!soft_equiv(surface_tally->weight(2, true,  4) , 1.0))
	ITFAILS;

    if (surface_tally->crossings(2, false, 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(1, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(3, true , 4) != 1 ) ITFAILS;
    if (surface_tally->crossings(2, true , 4) != 1 ) ITFAILS;
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    // version tag
    for (int arg = 1; arg < argc; arg++)
	if (std::string(argv[arg]) == "--version")
	{
	    std::cout << argv[0] << ": version " 
		      << rtt_imc::release() 
		      << std::endl;
	    return 0;
	}

    try
    {
	// >>> UNIT TESTS
	test_gray_particle_straight();

	test_gray_particle_rw();

	test_mg_particle();
    }
    catch (rtt_dsxx::assertion &ass)
    {
	std::cout << "While testing tstRZ_Surface_tracking, " << ass.what()
		  << std::endl;
	return 1;
    }

    // status of test
    std::cout << std::endl;
    std::cout << "*********************************************" << std::endl;
    if (rtt_imc_test::passed) 
    {
        std::cout << "**** tstRZ_Surface_tracking Test: PASSED" 
		  << std::endl;
    }
    std::cout << "*********************************************" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Done testing tstRZ_Surface_tracking." << std::endl;
    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of tstRZ_Surface_tracking.cc
//---------------------------------------------------------------------------//
