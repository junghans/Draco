//----------------------------------*-C++-*----------------------------------//
// T3_PreCond.hh
// Dave Nystrom
// Thu Oct  2 14:26:12 MDT 1997
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __t3_T3_PreCond_hh__
#define __t3_T3_PreCond_hh__

#include "linalg/PCG_PreCond.hh"

#include "ds++/Mat.hh"

//===========================================================================//
// class T3_PreCond - 

// 
//===========================================================================//

template<class T>
class T3_PreCond : public PCG_PreCond<T> {

  public:
    T3_PreCond();
    ~T3_PreCond();

    void  Left_PreCond( Mat1<T>& x, Mat1<T>&b );
    void Right_PreCond( Mat1<T>& x, Mat1<T>&b );
};

#endif                          // __t3_T3_PreCond_hh__

//---------------------------------------------------------------------------//
//                              end of t3/T3_PreCond.hh
//---------------------------------------------------------------------------//
