//----------------------------------*-C++-*----------------------------------//
// XYCoord_sys.hh
// Thomas M. Evans
// Fri Jan 30 16:52:13 1998
//---------------------------------------------------------------------------//
// @> XYCoord_sys derived class header file
//---------------------------------------------------------------------------//

#ifndef __imctest_XYCoord_sys_hh__
#define __imctest_XYCoord_sys_hh__

//===========================================================================//
// class XYCoord_sys - 
//
// Purpose : XY geometry coordinate system functions, derived
//           class of Coord_sys
//
// revision history:
// -----------------
//  0) original
//  1)  2-18-98 : added getCoord() function for debugging
//  2)  3-12-98 : moved Calc and Set_omega functions to Coord_sys because
//                they are the same for XY and XYZ transport, added transform 
//                for 2D meshes
//  3)  3-16-98 : reserve calc_normal function for later if need be
//  4)  3-17-98 : because of a dumb-ass oversight on my part, we don't need
//                a transform for 2D XY, it has been removed
//  5)   5-5-98 : added sample_pos virtual function
// 
//===========================================================================//

#include "imctest/Names.hh"
#include "imctest/Coord_sys.hh"
#include "rng/Sprng.hh"
#include "ds++/Assert.hh"
#include <vector>
#include <string>

IMCSPACE

using std::vector;
using std::string;

using RNG::Sprng;
    
class XYCoord_sys : public Coord_sys
{
  // Begin_Doc xycoord_sys-int.tex
  // Begin_Verbatim 

public:
   // default constructor to set dimension of XY coordinate
  // system, inline
    XYCoord_sys() : Coord_sys(2) {}

  // virtual functions
    virtual string get_Coord() const { string c = "xy"; return c; }
    inline virtual vector<double> sample_pos(string, vector<double>,
					     vector<double>, Sprng &) const;
	     
  // End_Verbatim 
  // End_Doc 
};

//---------------------------------------------------------------------------//
// INLINE Functions
//---------------------------------------------------------------------------//
// sample the position in an XY cell

inline vector<double> 
XYCoord_sys::sample_pos(string dist, vector<double> min, vector<double> max,
			Sprng &random) const
{
  // make return vector
    vector<double> r(2);

  // some assertions
    Check (min.size() == 2);
    Check (max.size() == 2);

    for (int d = 0; d < 2; d++)
    {
      // do uniform sampling
	if (dist == "uniform")
	    r[d] = (max[d] - min[d]) * random.ran() + min[d];
      // add others as needed
    }

  // return assigned array
    return r;
}

CSPACE

#endif                          // __imctest_XYCoord_sys_hh__

//---------------------------------------------------------------------------//
//                              end of imctest/XYCoord_sys.hh
//---------------------------------------------------------------------------//
