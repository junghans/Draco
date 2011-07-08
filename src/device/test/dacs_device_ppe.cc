//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   device/test/dacs_device_ppe.cc
 * \author Gabriel M. Rockefeller
 * \date   Fri Jun 17 15:29:18 2011
 * \brief  A tiny DACS accel-side binary, for testing DACS_Device.
 * \note   Copyright (C) 2011 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "ds++/Assert.hh"

#include <dacs.h>
#include <unistd.h>

int main()
{
    DACS_ERR_T err = dacs_init(DACS_INIT_FLAGS_NONE);
    Insist(err == DACS_SUCCESS, dacs_strerror(err));

    err = dacs_exit();
    Insist(err == DACS_SUCCESS, dacs_strerror(err));

    return 0;
}   

//---------------------------------------------------------------------------//
//                        end of dacs_device_ppe.cc
//---------------------------------------------------------------------------//