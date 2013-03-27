//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_MPI.hh
 * \author Thomas M. Evans
 * \date   Thu Mar 21 16:56:16 2002
 * \brief  C4 MPI function declarations.
 * \note   Copyright (C) 2002-2013 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef c4_C4_MPI_hh
#define c4_C4_MPI_hh

#include "c4/config.h"
#include <algorithm>

#ifdef C4_MPI

#include "C4_Functions.hh"
#include "MPI_Traits.hh"
#include "ds++/Assert.hh"
#include "c4_mpi.h"

namespace rtt_c4
{

//---------------------------------------------------------------------------//
// MPI Communicator
//---------------------------------------------------------------------------//

DLL_PUBLIC extern MPI_Comm communicator;
extern bool initialized;

//---------------------------------------------------------------------------//
// Null source/destination rank
//---------------------------------------------------------------------------//

extern const int proc_null;

//---------------------------------------------------------------------------//
// SETUP FUNCTIONS
//---------------------------------------------------------------------------//

template<typename Comm>
void inherit(const Comm &comm)
{
    Remember(int result = )
        MPI_Comm_dup(comm, &communicator);
    Check (result == MPI_SUCCESS);
}

template<class T>
int create_vector_type(unsigned count,
                       unsigned blocklength,
                       unsigned stride,
                       C4_Datatype &new_type)
{
    int info = MPI_Type_vector(count, blocklength, stride,
                               MPI_Traits<T>::element_type(),
                               &new_type);

    if (info != C4_SUCCESS)
        return info;

    info = MPI_Type_commit(&new_type);

    return info;
}

//---------------------------------------------//
/*!
 * Broadcast the range [first, last) from proc 0 
 * into [result, ...) on all other processors.
 */
template<typename ForwardIterator, typename OutputIterator>
void broadcast(
    ForwardIterator first,
    ForwardIterator last,
    OutputIterator  result)
{
    typedef typename std::iterator_traits<ForwardIterator>::value_type
	value_type;
    typedef typename std::iterator_traits<ForwardIterator>::difference_type 
	diff_type;

    // Proc 0 does not copy any data into the result iterator.

    diff_type size;
    if( rtt_c4::node() == 0)
	size = std::distance(first, last);
    broadcast(&size, 1, 0);
    
    value_type *buf = new value_type[size];
    if ( rtt_c4::node() == 0)
	std::copy(first, last, buf);
    broadcast(buf, size, 0);

    if ( rtt_c4::node() != 0)
	std::copy(buf, buf+size, result);
    
    delete [] buf;
    return;
}

// safer version of broadcast using stl ranges
template<typename ForwardIterator, typename OutputIterator>
void broadcast(ForwardIterator first,
	       ForwardIterator last,
	       OutputIterator  result,
               OutputIterator  result_end)
{
    // Check that the result is large enough to hold the data that is
    // currently in buf.
    Insist( std::distance(first,last) == std::distance(result,result_end),
            "Destination must be same size as source data." );
    broadcast(first,last,result);    
    return;
}

} // end namespace rtt_c4

#endif // C4_MPI

#endif // c4_C4_MPI_hh

//---------------------------------------------------------------------------//
// end of c4/C4_MPI.hh
//---------------------------------------------------------------------------//
