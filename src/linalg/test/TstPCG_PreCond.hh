//----------------------------------*-C++-*----------------------------------//
// TstPCG_PreCond.hh
// Dave Nystrom
// Fri May  9 13:40:54 1997
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __pcgWrap_test_TstPCG_PreCond_hh__
#define __pcgWrap_test_TstPCG_PreCond_hh__

#include "../PCG_PreCond.hh"

#include "ds++/Mat.hh"

//===========================================================================//
// class TstPCG_PreCond - 

// 
//===========================================================================//

template<class T>
class TstPCG_PreCond
    : public rtt_pcgWrap::PCG_PreCond<T>
{

  public:
    TstPCG_PreCond();
    ~TstPCG_PreCond();

    void Left_PreCond( rtt_dsxx::Mat1<T>& x,
		       const rtt_dsxx::Mat1<T>&b );
    
    void Right_PreCond( rtt_dsxx::Mat1<T>& x,
			const rtt_dsxx::Mat1<T>&b );
};

#endif                          // __pcgWrap_test_TstPCG_PreCond_hh__

//---------------------------------------------------------------------------//
//                              end of pcgWrap/test/TstPCG_PreCond.hh
//---------------------------------------------------------------------------//
