//----------------------------------*-C++-*----------------------------------//
// Allocators.hh
// Geoffrey Furnish
// Mon Jan 27 12:11:50 1997
//---------------------------------------------------------------------------//
// @> Memory management classes for DS++.
//---------------------------------------------------------------------------//

#ifndef __ds_Allocators_hh__
#define __ds_Allocators_hh__

#include "config.hh"
#include "Assert.hh"

//---------------------------------------------------------------------------//
// The allocators in this file are provided for use by the DS++ containers,
// or even by DS++ clients.  What is most important to understand about these
// allocators is that they follow the STL model for allocators.
// Specifically, they /only/ do /allocation/.  They do not concern themselves
// with object creation per se.  When you get back memory from one of these
// allocators, you have to initialize that memory with C++ object constructor
// calls yourself.  Likewise, before you release memory, you must destroy the
// objects contained in the memory pool before calling the release method on
// the allocator.
//---------------------------------------------------------------------------//

#include <memory>

NAMESPACE_DS_BEG

// These functions were in the April '96 draft, but disappeared by December
// '96.  Too bad, b/c they are extremely helpful for building allocators.

template<class T>
inline T *ds_allocate( int size, const T * /* hint */ )
{
    return (T *) (::operator new( size * sizeof(T) ) );
}

template<class T>
inline void ds_deallocate( T *buf ) { ::operator delete( buf ); }

//===========================================================================//
// class Simple_Allocator<T> - Minimalistic storage allocator

// This allocator class does the least sophisticated job possible.
// Specifically, it allocates exactly and only the amount of memory required
// to hold the requested number of objects of type T.  This is very similar
// to new T[n], except that object constructors are /not/ called.
//===========================================================================//

template<class T>
class Simple_Allocator {
  public:
    static T *fetch( int n, const T *hint = 0 )
    {
	return ds_allocate( n, hint );
    }

    static T *validate( T *v, int n ) { return v; }

    static void release( T *v, int n =0 )
    {
	ds_deallocate( validate( v, n ) );
    }

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
};

//===========================================================================//
// class Guarded_Allocator<T> - Range checking allocator

// This allocator class allocates enough memory to hold the requested number
// of objects of the specified type T, but also allocates a little extra to
// pad each end of the memory region.  This extra padding area is seeded with
// a specific byte pattern on allocation, and is checked for this same
// pattern on deallocation.  If the memory has been corrupted at the time of
// deallocation, you have a clear indication of an erroneous program, and an
// exception is consequently thrown.
//===========================================================================//

template<class T>
class Guarded_Allocator {
  public:
    static T *fetch( int n, const T* hint = 0 )
    {
	Assert( n >= 0 );
	T *v = ds_allocate( n+2, hint );

    // Write magic info into T[0] and T[n+1]
	char *pb = reinterpret_cast<char *>(v);
	char *pe = reinterpret_cast<char *>( v+n+1 );
	for( int i=0; i < sizeof(T); i++ ) {
	    pb[i] = static_cast<char>( 0xE8 );
	    pe[i] = static_cast<char>( 0xE9 );
	}
	
	return v+1;
    }

    static T *validate( T *v, int n )
    {
	v--;

	Check( guard_elements_ok(v,n) );

	return v;
    }

// Check magic data in T[0] and T[n+1]

    static bool guard_elements_ok( T *v, int n )
    {
	char *pb = reinterpret_cast<char *>(v);
	char *pe = reinterpret_cast<char *>( v+n+1 );

	for( int i=0; i < sizeof(T); i++ ) {
	    if (pb[i] != static_cast<char>( 0xE8 )) return false;
	    if (pe[i] != static_cast<char>( 0xE9 )) return false;
	}

	return true;
    }

    static void release( T *v, int n )
    {
	if (!v) return;

	ds_deallocate( validate( v, n ) );
    }

    typedef T *iterator;
    typedef const T *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
};

//===========================================================================//
// class alloc_traits<T> - Traits class for specifying client allocators

// This allocator traits class is provided to allow traits specialization of
// the default allocator to be used by DS++ container classes for various
// user defined types.  The default DS++ allocator will be the
// Guarded_Allocator (eventually, once I am sure it is working right), but
// individual user defined types can override this default on a case by case
// basis using template specialization of this class.  For example, to change
// the default allocator for wombats, you could put the following at the top
// of wombat.hh:
//
// #include "Allocators.hh"
// template<> class alloc_traits<wombat> {
//   public:
//     typedef Simple_Allocator<wombat> Default_Allocator;
// }
//
// This way, when a client code does this:
// 
// #include "wombat.hh"
// #include "Mat.hh"
//
// Mat1<wombat> wombatmat;
//
// The Simple_Allocator will be used rather than the Guarded_Allocator.
//===========================================================================//

template<class T>
class alloc_traits {
  public:
    typedef Simple_Allocator<T> Default_Allocator;
};

template<> class alloc_traits<int> {
  public:
    typedef Guarded_Allocator<int> Default_Allocator;
};

NAMESPACE_DS_END

#endif                          // __ds_Allocators_hh__

//---------------------------------------------------------------------------//
//                              end of ds++/Allocators.hh
//---------------------------------------------------------------------------//
