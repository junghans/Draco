//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   linear/ludcmp_pt.cc
 * \author Kent Budge
 * \date   Thu Jul  1 10:54:20 2004
 * \brief  LU decomposition
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "ludcmp.i.hh"
#include "ds++/Slice.t.hh"

namespace rtt_linear
{
 
template
void ludcmp(vector<double> &a,
	    vector<unsigned> &indx,
	    double &d);

template
void lubksb(vector<double> const &a,
	    vector<unsigned> const &indx,
	    vector<double> &b);

template
void lubksb(vector<double> const &a,
	    vector<unsigned> const &indx,
	    rtt_dsxx::Slice<vector<double>::iterator> &b);

} // end namespace rtt_linear


//---------------------------------------------------------------------------//
//                 end of ludcmp_pt.cc
//---------------------------------------------------------------------------//