//----------------------------------*-C++-*----------------------------------//
// PCG_MatVec.hh
// Dave Nystrom
// Fri May  9 12:30:20 1997
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __linalg_PCG_MatVec_hh__
#define __linalg_PCG_MatVec_hh__

#include "ds++/Mat.hh"

using dsxx::Mat1;

//===========================================================================//
// class PCG_MatVec - 

// 
//===========================================================================//

template<class T>
class PCG_MatVec {

  public:
    PCG_MatVec();
    ~PCG_MatVec();

    virtual void MatVec( Mat1<T>& b, Mat1<T>& x );
};

#endif                          // __linalg_PCG_MatVec_hh__

//---------------------------------------------------------------------------//
//                              end of linalg/PCG_MatVec.hh
//---------------------------------------------------------------------------//
