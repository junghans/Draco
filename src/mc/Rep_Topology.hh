//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   mc/Rep_Topology.hh
 * \author Thomas M. Evans
 * \date   Tue Nov 30 17:01:44 1999
 * \brief  Rep_Topology header file.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __mc_Rep_Topology_hh__
#define __mc_Rep_Topology_hh__

#include "Topology.hh"
#include "c4/global.hh"
#include "ds++/Assert.hh"
#include <iostream>

namespace rtt_mc
{
 
//===========================================================================//
/*!
 * \class Rep_Topology
 *
 * \brief Rep_Topology is a derived rtt_mc::Topology class for replication
 * topologies.
 *
 * Rep_Topology is a full replication topology class.  Because full
 * replication topologies are trivial, the Rep_Topology class provides the
 * full functionality of rtt_mc::Topology with minimal performance and memory
 * overhead.  It should be used instead of rtt_mc::General_Topology for full
 * replication topologies.  Like all Topology classes, an appropriate builder
 * should make the Rep_Topology instances.
 *
 * The constructor for Rep_Topology only requires the number of global cells
 * in a mesh.  All other data is constructed easily from the requirements of
 * a fully replicated spatial topology.  
 *
 * The Rep_Topology class assumes simple replication.  That is, a local cell
 * and global cell have the same index on each processor and all processors
 * hold all cells. No boundary cells exist.  Calls to boundary_to_local will
 * result in a failure. In other words, the mesh looks identical on each
 * processor.  If some different type of full replication topology is desired
 * than the General_Topology class should be used.  
 */
// revision history:
// -----------------
// 0) original
// 
//===========================================================================//

class Rep_Topology : public Topology
{
  private:
    // number of global cells in the problem
    int global_cells;

  public:
    // constructor
    explicit Rep_Topology(int);

    // SERVICES (virtual from Topology)

    //! Return the number of global cells.
    int num_cells() const { return global_cells; }

    //! Return the number of cells on a processor.
    int num_cells(int proc) const { return global_cells; }

    //! Return the number of processors storing a global_cell.
    int num_procs(int gcell) const { return C4::nodes(); }

    //! Get the global cell index on processor.
    int global_cell(int lcell) const { return lcell; }

    //! Get the global cell index on a given processor.
    int global_cell(int lcell, int proc) const { return lcell; }

    //! Get the local cell index on processor.
    int local_cell(int gcell) const { return gcell; }

    //! Get the local cell index on a given processor.
    int local_cell(int gcell, int proc) const { return gcell; }

    //! Get the boundary cell index for a global cell on a given processor.
    int global_to_boundary(int gcell, int proc) const { return 0; }

    //! Get the global cell index for a boundary cell on a given processor.
    inline int boundary_to_global(int c, int p) const;

    //! Get the number of boundary cells on a given processor.
    int get_boundary_cells(int proc) const { return 0; }
    
    // Get a list of global cells on processor.
    inline sf_int get_cells(int) const;

    // Get a list of processors that a global cell is on.
    inline sf_int get_procs(int) const;

    // Diagnostics.
    void print(std::ostream &) const;
};

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS FOR REP_TOPOLOGY
//---------------------------------------------------------------------------//
/*!
 * \brief Get a list of global cells on a processor.
 *
 * This simply returns a vector of all cells in the problem because all cells
 * are replicated on all processors.
 *
 * \param proc processor id which is irrelevant in this case (the same answer
 * is returned for all processor ids) 
 */
Topology::sf_int Rep_Topology::get_cells(int proc) const
{
    Require (proc >= 0 && proc < C4::nodes());

    sf_int cells(global_cells);
    for (int i = 0; i < cells.size(); i++)
	cells[i] = i+1;
    return cells;
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Get a list of processors that a global_cell lives on.
 *
 * This simply returns a vector of all processors in the problem because each
 * processor holds every cell.
 *
 * \param global_cell global cell index which is irrelevant in this case (the
 * same answer is returned for all cells)
 */
Topology::sf_int Rep_Topology::get_procs(int global_cell) const
{
    Require (global_cell > 0 && global_cell <= global_cells);

    sf_int procs(C4::nodes());
    for (int i = 0; i < procs.size(); i++)
	procs[i] = i;
    return procs;
}

//---------------------------------------------------------------------------//

int Rep_Topology::boundary_to_global(int boundary_cell, int proc) const
{
    Insist(0, "Not in rep!");
    return 0;
}

} // end namespace rtt_mc

#endif                          // __mc_Rep_Topology_hh__

//---------------------------------------------------------------------------//
//                              end of mc/Rep_Topology.hh
//---------------------------------------------------------------------------//
