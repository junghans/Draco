//----------------------------------*-C++-*----------------------------------//
// ADFile.cc
// Geoffrey Furnish
// Fri Dec 23 07:53:47 1994
//---------------------------------------------------------------------------//
// @> An "amorphous data file" for GTS.
//---------------------------------------------------------------------------//

#include "util/ADFile.hh"

//---------------------------------------------------------------------------//
// Constructor.  Just set up the base class, not much else.
//---------------------------------------------------------------------------//

ADFile::ADFile( String _fname, int _mode, int _nrecs )
    : Indx_File<ADKey>( _fname, _mode, _nrecs ),
      fname(_fname), mode(_mode), nrecs(_nrecs)
{}

//---------------------------------------------------------------------------//
// Return the set of all keys in the file.
//---------------------------------------------------------------------------//

Mat1<ADKey> ADFile::Get_keys()
{
    Mat1<ADKey> keys(toc_entries);

    for( int i=0; i < toc_entries; i++ )
	keys[i] = toc[i].key;

    return keys;
}

//---------------------------------------------------------------------------//
//                              end of ADFile.cc
//---------------------------------------------------------------------------//

