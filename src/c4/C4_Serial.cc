//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_Serial.cc
 * \author Thomas M. Evans
 * \date   Mon Mar 25 17:06:25 2002
 * \brief  Implementation of C4 serial option.
 * \note   Copyright (C) 2002-2014 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <c4/config.h>

#ifdef C4_SCALAR

#include "C4_Serial.hh"
#include "C4_sys_times.h"
#include "ds++/SystemCall.hh"
#include <cstdlib>

namespace rtt_c4
{

//---------------------------------------------------------------------------//
// Null source/destination rank
//---------------------------------------------------------------------------//

const int proc_null = -1;

//---------------------------------------------------------------------------//
// SETUP FUNCTIONS
//---------------------------------------------------------------------------//

DLL_PUBLIC void initialize(int &/* argc */, char **&/*argv */)
{
}

//---------------------------------------------------------------------------//

DLL_PUBLIC void finalize()
{
}

//---------------------------------------------------------------------------//

DLL_PUBLIC void free_inherited_comm()
{
}

//---------------------------------------------------------------------------------------//

DLL_PUBLIC void type_free(C4_Datatype &old_type)
{
}

//---------------------------------------------------------------------------//
// QUERY FUNCTIONS
//---------------------------------------------------------------------------//

int node()
{
    return 0;
}

//---------------------------------------------------------------------------//

int nodes()
{
    return 1;
}

//---------------------------------------------------------------------------//
// BARRIER FUNCTIONS
//---------------------------------------------------------------------------//

void global_barrier() { /* empty */ }

//---------------------------------------------------------------------------//
// TIMING FUNCTIONS
//---------------------------------------------------------------------------//

#if defined( WIN32 )
DLL_PUBLIC double wall_clock_time()
{
    __timeb64 now;
    return _ftime64_s( &now );
}
double wall_clock_time( __timeb64 & now )
{
    return _ftime64_s( &now );
}
#else
DLL_PUBLIC double wall_clock_time()
{
    tms now;
    return times( &now ) / wall_clock_resolution();
}
double wall_clock_time( tms & now )
{
    return times( &now ) / wall_clock_resolution();
}
#endif

//---------------------------------------------------------------------------//

DLL_PUBLIC double wall_clock_resolution()
{
    return static_cast<double>(DRACO_CLOCKS_PER_SEC);
}

//---------------------------------------------------------------------------//
// PROBE/WAIT FUNCTIONS
//---------------------------------------------------------------------------//

DLL_PUBLIC bool probe(int  /* source */, 
                      int  /* tag */,
                      int &/* message_size */)
{    
    return false;
}

DLL_PUBLIC void blocking_probe(int  /* source */, 
                               int  /* tag */,
                               int &/* message_size */)
{    
    Insist(false, "no messages expected in serial programs!");
}

DLL_PUBLIC void wait_all(int      /*count*/,
                         C4_Req * /*requests*/)
{
    // Insist(false, "no messages expected in serial programs!");
    return;
}

DLL_PUBLIC unsigned wait_any(int      /*count*/,
                             C4_Req * /*requests*/)
{
    Insist(false, "no messages expected in serial programs!");
    return 0;
}

//---------------------------------------------------------------------------//
// ABORT
//---------------------------------------------------------------------------//
int abort(int error)
{
    // This test is not recorded as tested by BullseyeCoverage because abort
    // terminates the execution and BullseyeCoverage only reports coverage for
    // function that return control to main().
    
    // call system exit
    std::abort();
    return error;
}

//---------------------------------------------------------------------------//
// isScalar
//---------------------------------------------------------------------------//
DLL_PUBLIC bool isScalar()
{
    return true;
}

//---------------------------------------------------------------------------//
// get_processor_name
//---------------------------------------------------------------------------//
std::string get_processor_name()
{
    return rtt_dsxx::draco_gethostname();;    
}

} // end namespace rtt_c4

#endif // C4_SCALAR

//---------------------------------------------------------------------------//
//                              end of C4_Serial.cc
//---------------------------------------------------------------------------//
