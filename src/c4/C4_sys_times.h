//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_sys_times.h
 * \author Kelly Thompson
 * \date   Mon Sep 20 21:54:18 2010
 * \brief  Encapsulate system headers for timing information.
 * \note   Copyright (C) 2010-2014 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef c4_C4_sys_times_h
#define c4_C4_sys_times_h

#include "ds++/config.h"

#if defined(WIN32) || defined(MINGW)
#  include <WinSock2.h> // Must be included before Windows.h
#  include <windows.h>
#  include <time.h>
#else
#  include <unistd.h>
#  include <sys/times.h>
#endif

#if defined(WIN32)
#  define DRACO_TIME_TYPE time_t
#  define DRACO_CLOCKS_PER_SEC CLOCKS_PER_SEC
#else
// Consider using GetProcessTimes instead of this as a higher resolution
// timer. 
#  define DRACO_TIME_TYPE tms
#  define DRACO_CLOCKS_PER_SEC sysconf(_SC_CLK_TCK)
#endif

#endif // c4_C4_sys_times_h

//---------------------------------------------------------------------------//
// end of c4/C4_sys_times.h
//---------------------------------------------------------------------------//
