//----------------------------------*-C++-*----------------------------------//
// txyz.cc
// Geoffrey M. Furnish
// Wed May 13 09:53:44 1998
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#include "mesh/Mesh_XYZ.hh"

#include "nml/Group.hh"

#include "c4/global.hh"

#include <iostream>
using namespace std;

int main( int argc, char *argv[] )
{
    C4::Init( argc, argv );

    cout << "t1: passed\n";

    NML_Group g( "test" );

    Mesh_DB mdb;
    mdb.setup_namelist( g );

    g.readgroup( "test.in" );

    dsxx::SP<Mesh_XYZ> spm = new Mesh_XYZ( mdb );

    cout << "t2: passed" << endl;


    Mesh_XYZ::cell_array<double> x( spm ), y( spm ), z( spm );
    Mesh_XYZ::cell_array<int> xi( spm ), yi( spm ), zi( spm );
    Mesh_XYZ::fcdsf xf( spm ), yf( spm ), zf( spm );

    x = 1.;
    y = 2.;
    z = x + y;

    xi = 1;
    yi = 2;
    zi = xi + yi;

    xf = 1.;
    yf = xf;
    zf = xf + yf;
    zf = xf - yf;

    x = 1.;
    xf = x;

    Mesh_XYZ::cell_array<double> oneCC( spm );
    oneCC = 1.0;
//    dump( oneCC, "oneCC, before" );
    Mesh_XYZ::fcdsf twoFC( spm );
    twoFC = 0.0;
//    dump( twoFC, "twoFC, before" );
    Mesh_XYZ::scatter<Mesh_XYZ::AddOp>( twoFC, oneCC );
    dump( oneCC, "oneCC, after" );
    dump( twoFC, "twoFC, after" );

    Mesh_XYZ::cell_array<double> threeCC( spm );
    threeCC = 3.0;
//    dump( threeCC, "threeCC, before" );
    Mesh_XYZ::fcdsf nineFC( spm );
    nineFC = 1.0;
//    dump( nineFC, "nineFC, before" );
    Mesh_XYZ::scatter<Mesh_XYZ::MultOp>( nineFC, threeCC );
//    dump( threeCC, "threeCC, after" );
//    dump( nineFC, "nineFC, after" );

    Mesh_XYZ::guarded_cell_array<double> xgc( spm );

    x = 1.;
    xgc = x;
    xf = 0.;
    xf += xgc;
    xf = 1.;
    xf *= xgc;

    C4::Finalize();

    return 0;
}

//---------------------------------------------------------------------------//
//                              end of txyz.cc
//---------------------------------------------------------------------------//
