//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   norms/Index_Proc.cc
 * \author Rob Lowrie
 * \date   Fri Jan 14 13:57:58 2005
 * \brief  Implementation of Index_Proc.
 * \note   Copyright (C) 2005-2013 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "Index_Proc.hh"
#include "c4/C4_Functions.hh"

namespace rtt_norms
{

//---------------------------------------------------------------------------//
/*!
  \brief Constructor.
*/
//---------------------------------------------------------------------------//
Index_Proc::Index_Proc(const size_t index_)
    : index(index_)
    , processor(rtt_c4::node())
{
}

//---------------------------------------------------------------------------//
/*!
  \brief Equality operator.
*/
//---------------------------------------------------------------------------//
bool Index_Proc::operator==(const Index_Proc &rhs) const
{
    return (index == rhs.index) && (processor == rhs.processor);
}

} // end namespace rtt_norms

//---------------------------------------------------------------------------//
// enc of Index_Proc.cc
//---------------------------------------------------------------------------//
