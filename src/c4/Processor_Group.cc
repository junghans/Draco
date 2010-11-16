//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/Processor_Group.cc
 * \author Kent Budge
 * \brief  Define non-template methods of class Processor_Group.
 * \note   Copyright (C) 2006-2007 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "c4_mpi.h"
#include "C4_Functions.hh"
#include "ds++/Assert.hh"
#include "Processor_Group.hh"

namespace rtt_c4
{
using namespace std;

//---------------------------------------------------------------------------//
/*!
 *
 * \param stride Number of processor groups to create. Each processor with
 * rank \c pid will be assigned to processor group <code> pid mod
 * stride</code>. For example, if the stride is 3, then processors 0, 3, 6,
 * ... form the first processor group, processors 1, 4, 7, ... form the second
 * processor group, and processors 2, 5, 8, ... form the third processor
 * group.
 */
Processor_Group::Processor_Group(unsigned const stride)
{
    int flag;
    MPI_Initialized(&flag);
    Insist(flag, "Processor_Group created before MPI is initialized");
    
    Insist(stride > 1, "Energy group decomposition is temporarily unavailable");

    unsigned const number_of_processors = rtt_c4::nodes();
    unsigned const pid = rtt_c4::node();
    
    MPI_Group parent_group;
    flag = MPI_Comm_group(MPI_COMM_WORLD, &parent_group);

    vector<int> ranks;
    for (unsigned i=0; i<number_of_processors; ++i)
    {
        if (i%stride == pid%stride)
        {
            ranks.push_back(i);
        }
    }
    size_ = ranks.size();
    flag = MPI_Group_incl(parent_group,
                          size_,
                          &ranks[0],
                          &group_);

    flag = MPI_Comm_create(MPI_COMM_WORLD, group_, &comm_);

    Ensure(check_class_invariants());
}

//---------------------------------------------------------------------------//
Processor_Group::~Processor_Group()
{
    int flag;
    MPI_Finalized(&flag);
    if (!flag)
    {
        MPI_Comm_free(&comm_);
        MPI_Group_free(&group_);
    }
    Ensure(check_class_invariants());
}

} // end namespace rtt_c4

//---------------------------------------------------------------------------//
//                 end of Processor_Group.cc
//---------------------------------------------------------------------------//