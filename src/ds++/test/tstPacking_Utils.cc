//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/test/tstPacking_Utils.cc
 * \author Thomas M. Evans
 * \date   Wed Nov  7 15:58:08 2001
 * \brief  
 * \note   Copyright (C) 2001-2012 Los Alamos National Security, LLC.
 *         All rights reserved
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "../ScalarUnitTest.hh"
#include "../Release.hh"
#include "../Packing_Utils.hh"
#include "../Assert.hh"
#include "../Soft_Equivalence.hh"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <typeinfo>

using namespace std;

using rtt_dsxx::Packer;
using rtt_dsxx::Unpacker;
using rtt_dsxx::pack_data;
using rtt_dsxx::unpack_data;
using rtt_dsxx::soft_equiv;

#define PASSMSG(a) ut.passes(a)
#define ITFAILS    ut.failure(__LINE__);
#define FAILURE    ut.failure(__LINE__, __FILE__);
#define FAILMSG(a) ut.failure(a);

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

void do_some_packing(Packer &p,
                     vector<double> const & vd,
                     vector<int>    const & vi)
{
    for ( size_t i = 0; i < vd.size(); ++i ) p << vd[i];
    for ( size_t i = 0; i < vi.size(); ++i ) p << vi[i];
    return;
}

//---------------------------------------------------------------------------//

void compute_buffer_size_test( rtt_dsxx::UnitTest & ut )
{
    // make data

    int num_vd = 5;
    vector<double> vd(num_vd, 2.3432);
    vd[3] = 22.4;

    int num_vi = 3;
    vector<int> vi(num_vi, 6);
    vi[0] = 7;
    vi[1] = 22;

    char const test_string[]="test";

    unsigned int total_size =
        num_vi * sizeof(int) + num_vd * sizeof(double) + sizeof(test_string);
    // includes one padding byte

    Packer p;

    // Compute the required buffer size.

    p.compute_buffer_size_mode();
    do_some_packing(p, vd, vi); // computes the size

    p.pad(1);
    p.accept(4, test_string);

    if ( total_size != p.size() ) ITFAILS;

    vector<char> buffer(p.size());

    // Pack into buffer.

    p.set_buffer(p.size(), &buffer[0]);
    do_some_packing(p, vd, vi); // actually does the packing

    p.pad(1);
    p.accept(4, test_string);

    if ( static_cast<int>(total_size) != p.end()-p.begin()) ITFAILS;

    // Unpack

    Unpacker u;

    u.set_buffer(p.size(), &buffer[0]);

    if ( static_cast<int>(u.size()) != u.end()-u.begin()) ITFAILS;

    for ( size_t i = 0; i < vd.size(); ++i )
    {
        double d;
        u >> d;
        if ( ! soft_equiv(d, vd[i]) ) ITFAILS;
    }

    for ( size_t i = 0; i < vi.size(); ++i )
    {
        int j;
        u >> j;
        if ( j != vi[i] ) ITFAILS;
    }
    
    // padding byte
    u.skip(1);

    for ( unsigned i=0; i<4; ++i)
    {
        char c;
        u >> c;
        if (c != test_string[i]) ITFAILS;
    }

    if( ut.numFails == 0 )
        PASSMSG("compute_buffer_size_test() worked fine.");

    return;
}

//---------------------------------------------------------------------------//
void packing_test( rtt_dsxx::UnitTest & ut)
{
    // make some data
    double x = 102.45;
    double y = 203.89;
    double z = 203.88;

    int ix   = 10;
    int iy   = 11; 
    int iz   = 12;

    // make 2 buffers for data
    int s1   = 2 * sizeof(double) + 2 * sizeof(int);
    char *b1 = new char[s1];
    int s2   = sizeof(double) + sizeof(int);
    char *b2 = new char[s2];

    // pack the data
    {
        Packer p;

        p.set_buffer(s1, b1);
        p << x << ix;
        p.pack(y);
        p.pack(iy);

        if (p.get_ptr() != b1+s1) ITFAILS;

        p.set_buffer(s2, b2);
        p << iz << z;
        
        if (p.get_ptr() != b2+s2) ITFAILS;

        // Catch a failure when excedding the buffer limit:
        if( ut.dbcOn() && ! ut.dbcNothrow() )
        {
            bool caught = false;
            try
            {
                p << iz;
            }
            catch (const rtt_dsxx::assertion & /* error */ )
            {
                cout << "Good, caught the exception" << endl;
                caught = true;
            }
            if (!caught) ITFAILS;
        }
    }
    
    // unpack the data
    {
        Unpacker u;

        double   d = 0;
        int      i = 0;

        u.set_buffer(s1, b1);
        u >> d >> i;
        if (d != 102.45)          ITFAILS;
        if (i != 10)              ITFAILS;

        u.unpack(d);
        u.unpack(i); 
        if (d != 203.89)          ITFAILS;
        if (i != 11)              ITFAILS;

        if (u.get_ptr() != s1+b1) ITFAILS;

        // If DBC is off or if DBC nothrow is on, then this test is invalid. 
        if( ut.dbcOn() && ! ut.dbcNothrow() )
        {
            // try catching a failure
            bool caught = false;
            try
            {
                u.unpack(i);
            }
            catch (const rtt_dsxx::assertion & /* error */ )
            {
                cout << "Good, caught the exception" << endl;
                caught = true;
            }
            if (!caught) ITFAILS;
        }

        u.set_buffer(s2, b2);
        u >> i >> d;
        if (i != 12)              ITFAILS;
        if (d != 203.88)          ITFAILS;
        
        if (u.get_ptr() != s2+b2) ITFAILS;
    }

    delete [] b1;
    delete [] b2;

    // try packing a vector and char array
    double r = 0;
    srand(125);

    vector<double> vx(100, 0.0);
    vector<double> ref(100, 0.0);

    char c[4] = {'c','h','a','r'};

    for (size_t i = 0; i < vx.size(); i++)
    {
        r      = rand();
        vx[i]  = r / RAND_MAX;
        ref[i] = vx[i];
    }

    int size     = 100 * sizeof(double) + 4;
    char *buffer = new char[size];
    
    // pack
    {
        Packer p;
        p.set_buffer(size, buffer);

        for (size_t i = 0; i < vx.size(); i++)
            p << vx[i];

        for (size_t i = 0; i < 4; i++)
            p << c[i];

        if (p.get_ptr() != buffer+size) ITFAILS;
    }
    
    // unpack
    {
        char cc[4];
        vector<double> x(100, 0.0);

        Unpacker u;
        u.set_buffer(size, buffer);

        for (size_t i = 0; i < x.size(); i++)
            u >> x[i];

        u.extract(4, cc);

        if (u.get_ptr() != buffer+size) ITFAILS;
        
        for (size_t i = 0; i < x.size(); i++)
            if (x[i] != ref[i]) ITFAILS;

        if (c[0] != 'c') ITFAILS;
        if (c[1] != 'h') ITFAILS;
        if (c[2] != 'a') ITFAILS;
        if (c[3] != 'r') ITFAILS;
    }

    // Skip some data and unpack
    {
        char cc[2];
        vector<double> x(100, 0.0);

        Unpacker u;
        u.set_buffer(size, buffer);

        // Skip the first 50 integers.
        u.skip(50*sizeof(double));
        for (size_t i=50; i < x.size(); ++i) u >> x[i];

        // Skip the first two chatacters
        u.skip(2);
        u.extract(2,cc);

        for (size_t i=0;  i<50;       ++i) if (x[i] != 0) ITFAILS;
        for (size_t i=50; i<x.size(); ++i) if (x[i] != ref[i]) ITFAILS;

        if (cc[0] != 'a') ITFAILS;
        if (cc[1] != 'r') ITFAILS;

    }

    delete [] buffer;
}

//---------------------------------------------------------------------------//

void std_string_test( rtt_dsxx::UnitTest & ut )
{
    vector<char> pack_string;

    {
        // make a string
        string hw("Hello World"); 

        // make a packer 
        Packer packer;

        // make a char to write the string into
        pack_string.resize(hw.size() + 1 * sizeof(int));

        packer.set_buffer(pack_string.size(), &pack_string[0]);

        packer << static_cast<int>(hw.size());

        // pack it
        for (string::const_iterator it = hw.begin(); it != hw.end(); it++)
            packer << *it;
        
        if (packer.get_ptr() != &pack_string[0] + pack_string.size()) ITFAILS;
        if (packer.get_ptr() != packer.begin()  + pack_string.size()) ITFAILS;
    }

    // now unpack it
    Unpacker unpacker;
    unpacker.set_buffer(pack_string.size(), &pack_string[0]);
    
    // unpack the size of the string
    int size;
    unpacker >> size;

    string nhw;
    nhw.resize(size);

    // unpack the string
    for (string::iterator it = nhw.begin(); it != nhw.end(); it++)
        unpacker >> *it;

    if (unpacker.get_ptr() != &pack_string[0] + pack_string.size()) ITFAILS;

    // test the unpacked string
    // make a string
    string hw("Hello World"); 

    if (hw == nhw)
    {
        ostringstream message;
        message << "Unpacked string " << nhw << " that matches original "
                << "string " << hw;
        PASSMSG(message.str());
    }
    else
    {
        ostringstream message;
        message << "Failed to unpack string " << hw << " correctly. Instead "
                << "unpacked " << nhw;
        FAILMSG(message.str());
    }
}

//---------------------------------------------------------------------------//
 
void packing_functions_test( rtt_dsxx::UnitTest & ut )
{
    
    // Data to pack:
    // -------------
    vector<double> x(5);
    string         y("Tom Evans is a hack!");
    
    for (int i=0; i<5; ++i) x[i] = 100.0*static_cast<double>(i) + 2.5;


    // Pack the data
    // -------------

    vector<char> packed_vector;
    vector<char> packed_string;

    pack_data(x, packed_vector);
    pack_data(y, packed_string);

    if (packed_vector.size() != 5*sizeof(double) + sizeof(int)) ITFAILS;
    if (packed_string.size() != y.size() + sizeof(int))         ITFAILS;
        
    
    /* We now pack the two packed datums (x and s) together by manually
     * inserting the data, including the size of the already packed arrays,
     * into a new array.
     *
     * Honestly, I can't think of a better demonstration of how brain-dead
     * the packing system is. The root cause is the inistance of only packing
     * and unpacking into vector<char>. These manage their own memory, so any
     * attempt to put non-trivial objects together in a data stream will
     * require copying data. We need the size of the packed data (which
     * already contains size data) because we have to reserve space in a
     * vector<char> when we unpack.
     *
     * The repeated steps for the two vectors performed below could be
     * factored out into a function, but this is another layer of abstraction
     * which shouldn't be necessary. Furthermore, it wouldn't fix all of the
     * extra copies that this approach requires.
     *
     * I was able to improve the unpacking step somewhat with the addition of
     * the extract command in the unpacker. At least we don't have to spoon
     * the packed data between containers one character at a time any more.
     *
     * Someday, I'll fix this mess. But right now, I've got work to do.
     *
     *                                   -MWB. 
     */ 

    // A place the hold the packed sizes of already packed data.
    vector<char> packed_int(sizeof(int));
    Packer p;

    // Storage for the combined packed data.
    vector<char> total_packed;

    p.set_buffer(sizeof(int), &packed_int[0]);

    // Pack the size of the packed vector into packed_int;
    p << static_cast<int>(packed_vector.size());

    // Now, manually copy the packed size of the packed vector into the total
    // data array.
    copy(packed_int.begin(), packed_int.end(), back_inserter(total_packed));
    
    // Now, manually append the data of the packed vector into the total data
    // array. 
    copy(packed_vector.begin(), packed_vector.end(),
         back_inserter(total_packed));

    

    /* Now, we repeat the process for the string data */

    // Reset the packer, to re-use the space for holding packed data sizes:
    p.set_buffer(sizeof(int), &packed_int[0]);

    // Pack the size of the packed string.
    p << static_cast<int>(packed_string.size());

    // Manually copy the packed size of the packed string into the total data
    // array.
    copy(packed_int.begin(), packed_int.end(), back_inserter(total_packed));

    // Manually append the data of the packed string into the total data
    // array.
    copy(packed_string.begin(), packed_string.end(),
         back_inserter(total_packed));



    // Unpack the data
    // ---------------
    
    vector<double> x_new;
    string         y_new;

    // Holding place for the size data.
    int size;

    Unpacker u;
    u.set_buffer(total_packed.size(), &total_packed[0]);
        
    u >> size;
    vector<char> packed_vector_new(size);
    u.extract(size, packed_vector_new.begin());

    u >> size;
    vector<char> packed_string_new(size);
    u.extract(size, packed_string_new.begin());
        
    if (u.get_ptr() != &total_packed[0] + total_packed.size()) ITFAILS;
    
    unpack_data(x_new, packed_vector_new);
    unpack_data(y_new, packed_string_new);


    
    // Compare the results
    // -------------------

    if (!soft_equiv(x_new.begin(), x_new.end(), x.begin(), x.end()))
        ITFAILS;

    if (y_new != y) ITFAILS;

    if( ut.numFails == 0 )
        PASSMSG("pack_data and unpack_data work fine.");

    return;
}


//---------------------------------------------------------------------------//

void endian_conversion_test( rtt_dsxx::UnitTest & ut )
{

    Packer p;
    Unpacker up(true);


    // Test the int type.
    const int moo = 0xDEADBEEF;
    const int length = sizeof(int);

    // Pack
    char data[length];
    p.set_buffer(length, data);
    p << moo;

    // Unpack
    int oom = 0; 
    up.set_buffer(length, data);
    up >> oom;

    // Check
    if (static_cast<unsigned>(oom) != 0xEFBEADDE) ITFAILS;

    

    // Verify that char data (being one byte) is unchanged.
    const char letters[] = "abcdefg";
    const int letter_length = sizeof(letters)/sizeof(char);

    // Pack
    char letter_data[letter_length];
    p.set_buffer(letter_length, letter_data);
    for (int i=0; i<letter_length; ++i) p << letters[i];

    // Unpack
    char unpacked_letters[letter_length];
    up.set_buffer(letter_length, letter_data);
    for (int i=0; i<letter_length; ++i) up >> unpacked_letters[i];

    // Check
    for (int i=0; i<letter_length; ++i)
        if (unpacked_letters[i] != letters[i]) ITFAILS;

    return;
}

//---------------------------------------------------------------------------//
int main(int argc, char *argv[])
{
    rtt_dsxx::ScalarUnitTest ut( argc, argv, rtt_dsxx::release );
    try
    {
        // >>> UNIT TESTS
        packing_test(ut);
        std_string_test(ut);
        packing_functions_test(ut);
        compute_buffer_size_test(ut);
        endian_conversion_test(ut);
    }
    catch (rtt_dsxx::assertion &error)
    {
        cout << "ERROR: While testing " << argv[0] << ", "
             << error.what() << endl;
        ut.numFails++;
    }
    catch( ... )
    {
        cout << "ERROR: While testing " << argv[0] << ", " 
             << "An unknown exception was thrown" << endl;
        ut.numFails++;
    }
    return ut.numFails;
}   

//---------------------------------------------------------------------------//
// end of tstPacking_Utils.cc
//---------------------------------------------------------------------------//
