//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   imc/Mesh_Operations.hh
 * \author Thomas M. Evans
 * \date   Mon Dec 20 15:59:32 1999
 * \brief  Mesh_Operations header file.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __imc_Mesh_Operations_hh__
#define __imc_Mesh_Operations_hh__

#include "Mat_State.hh"
#include "mc/OS_Mesh.hh"
#include "mc/Topology.hh"
#include "rng/Random.hh"
#include "ds++/SP.hh"
#include "ds++/Assert.hh"
#include <vector>

namespace rtt_imc
{
 
//===========================================================================//
/*!
 * \class Mesh_Operations
 *
 * \brief Mesh association class that provides specialized operations to
 * meshes that depend upon extra-mesh data and operations.
 *
 * The Mesh_Operations class is a series of specialized classes that perform
 * mesh-type operations that are highly mesh-dependent but involve data
 * and/or operations that are outside the direct pervue of the mesh itself.
 * An example is T^4 slope and volume-tilt sampling.  Different meshes will
 * perform the "tilt" using different techniques and requiring different
 * data.  However, to perform the tilt, the mesh must have temperature data.
 * The Mesh_Operations classes provide class template specializations for
 * each mesh type so that these tasks may be completed.  This avoids circular
 * references between the mesh and the data that it requires.  It also avoids
 * embedding communication directly in the mesh.  The Mesh_Operations classes
 * contain operations that are only required for IMC algorithms.
 *
 * The Mesh_Operations class is templated on Mesh Type (MT).  Specializations
 * are provided for rtt_mc::OS_Mesh.  Other mesh classes used by IMC will
 * require specializations as they come online.
 *
 * The Mesh_Operations class uses the rtt_mc::Topology class to perform any
 * required communications.
 */
/*! 
 * \example imc/test/tstMesh_Operations.cc
 *
 * Test of Mesh_Operations class.  At present, only the rtt_mc::OS_Mesh
 * specialization of the Mesh_Operations class is tested.  
 */
// revision history: 
// -----------------
// 0) original
// 
//===========================================================================//

template<class MT>
class Mesh_Operations 
{
    // All specializations should have the following:
  
  public:
    typedef MT mesh_type;

  private:
    // typedefs
    typedef std::vector<double>                   sf_double;
    typedef rtt_mc::OS_Mesh::CCVF<double>         ccvf_double;
    typedef dsxx::SP<rtt_mc::OS_Mesh>             SP_Mesh;
    typedef dsxx::SP<rtt_mc::Topology>            SP_Topology;
    typedef dsxx::SP<Mat_State<rtt_mc::OS_Mesh> > SP_Mat_State;
    
  public:
    /*! 
     * \brief Constructor.
     *
     * The constructor builds complete state information necessary to perform
     * required operations.  This include T4 slope data and the like.
     *
     * \param mesh dsxx::SP to a mesh
     * \param state dsxx::SP to a Mat_State
     * \param topology dsxx::SP to a Topology
     */
    Mesh_Operations(SP_Mesh mesh, SP_Mat_State state, SP_Topology topology)
    {/*...*/}

    /*!
     * \brief Sample position in cell with T4 tilt.
     *
     * \param cell cell index
     * \param T temperature in cell
     * \param rand rtt_rng::Sprng random number object
     * \return vector<double> of particle position
     */ 
    sf_double sample_pos_tilt(int cell, double T, rtt_rng::Sprng &rand) const
    {/*...*/} 
};

//---------------------------------------------------------------------------//
// OS_MESH SPECIALIZATION OF MESH_OPERATIONS CLASS
//---------------------------------------------------------------------------//

template<>
class Mesh_Operations<rtt_mc::OS_Mesh>
{
  public:
    // typedefs telling type of mesh
    typedef rtt_mc::OS_Mesh mesh_type;

  private:
    // typedefs
    typedef std::vector<double>                   sf_double;
    typedef rtt_mc::OS_Mesh::CCVF<double>         ccvf_double;
    typedef dsxx::SP<rtt_mc::OS_Mesh>             SP_Mesh;
    typedef dsxx::SP<rtt_mc::Topology>            SP_Topology;
    typedef dsxx::SP<Mat_State<rtt_mc::OS_Mesh> > SP_Mat_State;

  private:
    // T^4 slope data
    ccvf_double t4_slope;

    // IMPLEMENTATION
    void build_replication_T4_slope(SP_Mat_State);
    void build_DD_T4_slope(SP_Mat_State state, SP_Topology topology) {}

  public:
    // Constructor.
    Mesh_Operations(SP_Mesh mesh, SP_Mat_State state, SP_Topology topology);

    // Sample particle position using tilt.
    sf_double sample_pos_tilt(int, double, rtt_rng::Sprng &) const; 

    // ACCESSORS

    // Get values of T4_slope for testing.  This is not part of the standard
    // interface to Mesh_Operations.
    const ccvf_double& get_t4_slope() const { return t4_slope; }
};

} // end namespace rtt_imc

#endif                          // __imc_Mesh_Operations_hh__

//---------------------------------------------------------------------------//
//                              end of imc/Mesh_Operations.hh
//---------------------------------------------------------------------------//
