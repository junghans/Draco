//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/Packing_Utils.hh
 * \author Thomas M. Evans
 * \date   Thu Jul 19 11:27:46 2001
 * \brief  Packing Utilities, classes for packing stuff.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef rtt_ds_Packing_Utils_hh
#define rtt_ds_Packing_Utils_hh

#include "Assert.hh"

#include <string>
#include <vector>
#include <cstring>

namespace rtt_dsxx
{

//===========================================================================//
/*!
 * \file ds++/Packing_Utils.hh

 * This file contains classes and utilities that are used to "pack" data into
 * byte-streams.  The byte-streams are represented by the char* type.  The
 * following classes are:

 * \arg \b Packer packing class
 * \arg \b Unpacker unpacking class

 */

/*!
 * \example ds++/test/tstPacking_Utils.cc

 * Test the Packer and Unpacker classes.

 */
//===========================================================================//
 
//===========================================================================//
/*!
 * \class Packer
 
 * \brief Pack data into a byte stream.

 * This class allows clients to "register" a char* stream and then load it
 * with data of any type.  This assumes that the sizeof(T) operator works and
 * has meaning for the type.  Under the hood it uses std::memcpy to perform
 * the loading.  This class is easily understood by checking the examples.

 * No memory allocation is performed by the Packer. 

 * The benefit of using the Packer class is that byte copies are isolated
 * into this one section of code, thus obviating the need for
 * reinterpret_cast statements in client code.  In fact, this functionality
 * conforms exactly to the ANSI C++ standard for copying byte-streams of data
 * (sec. 3.9). Additionally, bounds checking is performed on all stream
 * packing operations.  This bounds checking is always on.

 * This class returns real char * pointers through its query functions.  We
 * do not use the STL iterator notation, even though that is how the pointers
 * are used, so as not to confuse the fact that these char * streams are \i
 * continuous \i data byte-streams.  The pointers that are used to "iterate"
 * through the streams are real pointers, not an abstract iterator class.  So
 * one could think of these as iterators (they act like iterators) but they
 * are real pointers into a continguous memory char * stream.

 * Data can be unpacked using the Unpacker class.
 
 */
//===========================================================================//

class Packer
{
  public:
    // Typedefs.
    typedef char *       pointer;
    typedef const char * const_pointer;

  private:
    // Size of packed stream.
    unsigned int stream_size;
    
    // Pointer (mutable) into data stream.
    pointer ptr;

    // Pointers to begin and end of buffers.
    pointer begin_ptr;
    pointer end_ptr;

  public:
    //! Constructor.
    Packer() : stream_size(0), ptr(0), begin_ptr(0), end_ptr(0) {/*...*/}

    // Set the buffer.
    inline void set_buffer(unsigned int, pointer);

    // Pack values into the buffer.
    template<class T> inline void pack(const T&);

    // >>> ACCESSORS

    //! Get a pointer to the current position of the data stream.
    const_pointer get_ptr() const { return ptr; }

    //! Get a pointer to the beginning position of the data stream.
    const_pointer begin() const { return begin_ptr; }

    //! Get a pointer to the ending position of the data stream.
    const_pointer end() const { return end_ptr; }

    //! Get the size of the data stream.
    unsigned int size() const { return stream_size; }
};

//---------------------------------------------------------------------------//
/*!
 * \brief Set an allocated buffer to write data into.

 * This function accepts an allocated char* buffer.  It assigns begin and end
 * pointers and a mutable position pointer that acts like an iterator.  The
 * Packer will write POD (Plain Old Data) data into this buffer starting at
 * the beginning address of the buffer.  This function must be called before
 * any Packer::pack calls can be made.

 * Once Packer::set_buffer is called, all subsequent calls to Packer::pack
 * will write data incrementally into the buffer set by set_buffer.  To write
 * data into a different buffer, call Packer::set_buffer again; at this point
 * the Packer no longer has any knowledge about the old buffer.

 * Note, the buffer must be allocated large enough to hold all the data that
 * the client intends to load into it.  There is no memory allocation
 * performed by the Packer class; thus, the buffer cannot be increased in
 * size if a value is written past the end of the buffer.  See the
 * Packer::pack function for more details.

 * \param size_in size of the buffer
 * \param buffer pointer to the char * buffer

 */
void Packer::set_buffer(unsigned int size_in, pointer buffer)
{
    Require (buffer);
    
    // set the size, begin and end pointers, and iterator
    stream_size  = size_in;
    ptr          = &buffer[0];
    begin_ptr    = &buffer[0];
    end_ptr      = begin_ptr + stream_size;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Pack data into a buffer.

 * This function packs a piece of data (single datum) into the buffer set by
 * Packer::set_buffer.  It advances the pointer (iterator) location to the
 * next location automatically.  It uses the sizeof(T) operator to get the
 * size of the data; thus, only data where sizeof() has meaning will be
 * properly written to the buffer.

 * Packer::pack() does bounds checking to ensure that the buffer and buffer
 * size defined by Packer::set_buffer are consistent.  This bounds-checking
 * is always on as the Packer is not normally used in compute-intensive
 * calculations.

 * \param value data of type T to pack into the buffer; the data size must be
 * accessible using the sizeof() operator

 */
template<class T>
void Packer::pack(const T &value)
{
    Require (begin_ptr);
    Insist  (ptr >= begin_ptr, "Bounds error in packer!");
    Insist  (ptr + sizeof(T) <= end_ptr, "Bounds error in packer!");

    // copy value into the buffer
    std::memcpy(ptr, &value, sizeof(T));

    // advance the iterator pointer to the next location
    ptr += sizeof(T);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Stream out (<<) operator for packing data.

 * The overloaded stream out operator can be used to pack data into streams
 * (Packer p; p.set_buffer(i,b); p << data;).  It simply calls the
 * Packer::pack function.  It returns a reference to the Packer object so
 * that stream out operations can be strung together.

 */
template<class T>
inline Packer& operator<<(Packer &p, const T &value)
{
    // pack the value
    p.pack(value);

    // return the packer object
    return p;
}

//===========================================================================//
/*!
 * \class Unpacker
 
 * \brief Unpack data from a byte stream.

 * This class allows clients to "register" a char* stream and then unload
 * data from it.  This assumes that the sizeof(T) operator works and has
 * meaning for the type.  Under the hood it uses std::memcpy to perform the
 * unloading.  This class is easily understood by checking the examples.

 * No memory allocation is performed by the Unpacker. 

 * The benefit of using the Unpacker class is that byte copies are isolated
 * into this one section of code, thus obviating the need for
 * reinterpret_cast statements in client code.  In fact, this functionality
 * conforms exactly to the ANSI C++ standard for copying byte-streams of data
 * (sec. 3.9). Additionally, bounds checking is performed on all stream
 * packing operations.  This bounds checking is always on.

 * This class returns real char * pointers through its query functions.  We
 * do not use the STL iterator notation, even though that is how the pointers
 * are used, so as not to confuse the fact that these char * streams are \i
 * continuous \i data byte-streams.  The pointers that are used to "iterate"
 * through the streams are real pointers, not an abstract iterator class.  So
 * one could think of these as iterators (they act like iterators) but they
 * are real pointers into a continguous memory char * stream.

 * This class is the complement to the Packer class.
 
 */
//===========================================================================//

class Unpacker
{
  public:
    // Typedefs.
    typedef char *       pointer;
    typedef const char * const_pointer;

  private:
    // Size of packed stream.
    unsigned int stream_size;
    
    // Pointer (mutable) into data stream.
    const_pointer ptr;

    // Pointers to begin and end of buffers.
    const_pointer begin_ptr;
    const_pointer end_ptr;

  public:
    //! Constructor.
    Unpacker() : stream_size(0), ptr(0), begin_ptr(0), end_ptr(0) {/*...*/} 

    // Set the buffer.
    inline void set_buffer(unsigned int, const_pointer);

    // Unpack value from buffer.
    template<class T> inline void unpack(T &);
    
    // >>> ACCESSORS

    //! Get a pointer to the current position of the data stream.
    const_pointer get_ptr() const { return ptr; }

    //! Get a pointer to the beginning position of the data stream.
    const_pointer begin() const { return begin_ptr; }

    //! Get a pointer to the ending position of the data stream.
    const_pointer end() const { return end_ptr; }

    //! Get the size of the data stream.
    unsigned int size() const { return stream_size; }
};

//---------------------------------------------------------------------------//
/*!
 * \brief Set an allocated buffer to read data from.

 * This function accepts an allocated char* buffer.  It assigns begin and end
 * pointers and a mutable position pointer that acts like an iterator.  The
 * Unpacker will read POD data from this buffer starting at the beginning
 * address of the buffer.  This function must be called before any
 * Unpacker::unpack calls can be made.

 * Once Unpacker::set_buffer is called, all subsequent calls to
 * Unpacker::unpack will read data incrementally from the buffer set by
 * set_buffer.  To read data from a different buffer, call
 * Unpacker::set_buffer again; at this point the Unpacker no longer has any
 * knowledge about the old buffer.

 * Note, there is no memory allocation performed by the Unacker class.  Also,
 * the client must know how much data to read from the stream (of course
 * checks can be made telling where the end of the stream is located using
 * the Unpacker::get_ptr, Unpacker::begin, and Unpacker::end functions).

 * \param size_in size of the buffer
 * \param buffer const_pointer to the char * buffer

 */
void Unpacker::set_buffer(unsigned int size_in, const_pointer buffer)
{
    Require (buffer);
    
    // set the size, begin and end pointers, and iterator
    stream_size  = size_in;
    ptr          = &buffer[0];
    begin_ptr    = &buffer[0];
    end_ptr      = begin_ptr + stream_size;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Unpack data from the buffer.

 * This function unpacks a piece of data (single datum) from the buffer set
 * by Unpacker::set_buffer.  It advances the pointer (iterator) location to
 * the next location automatically.  It uses the sizeof(T) operator to get
 * the size of the data; thus, only data where sizeof() has meaning will be
 * properly read from the buffer.POLYNOMIAL_Specific_Heat_ANALYTIC_EoS_MODEL

 * Unpacker::unpack() does bounds checking to ensure that the buffer and
 * buffer size defined by Unpacker::set_buffer are consistent.  This
 * bounds-checking is always on as this should not be used in computation
 * intensive parts of the code.

 * \param value data of type T to unpack from the buffer; the data size must
 * be accessible using the sizeof() operator

 */
template<class T>
void Unpacker::unpack(T &value)
{
    Require (begin_ptr);
    Insist  (ptr >= begin_ptr, "Bounds error in unpacker!");
    Insist  (ptr + sizeof(T) <= end_ptr, "Bounds error in unpacker!");

    // copy data into the value reference
    std::memcpy(&value, ptr, sizeof(T));

    // advance the iterator pointer to the next location
    ptr += sizeof(T);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Stream in (>>) operator for unpacking data.

 * The overloaded stream in operator can be used to unpack data from streams
 * (Unpacker u; u.set_buffer(i,b); u >> data;).  It simply calls the
 * Unpacker::unpack function.  It returns a reference to the Unpacker object
 * so that stream in operations can be strung together.

 */
template<class T>
inline Unpacker& operator>>(Unpacker &u, T &value)
{
    // unpack the value
    u.unpack(value);

    // return the unpacker object
    return u;
}

//===========================================================================//
// PACKING/UNPACKING SHORTCUT FUNCTIONS
//===========================================================================//
/*!
 * \brief Packing function.
 *
 * This function uses the rtt_dsxx::Packer to pack a given field into a
 * vector<char>.  The field type is represented by the template argument FT.
 * The field type must have the following members defined:
 *
 * \arg FT::value_type type stored in the field
 * \arg FT::const_iterator const iterator for the field
 * \arg FT::size() returns the number of elements in the field
 * \arg FT::begin() returns an iterator to the beginning of the field
 * \arg FT::end() returns an iterator to the end of the field
 * \arg FT::empty() determines if a container is empty
 *
 * Given these contraints, the function cannot be used to pack up a pointer
 * array; however, this is accomplished easily enough with the Packer class
 * alone.
 *
 * The data in the field is packed into a vector<char>. The vector<char>
 * passed to the function must be empty; an assertion is thrown if it is
 * not.  We do this for usage protection; we want the user to be aware that
 * data in the vector<char> would be destroyed.
 *
 * In summary, this is a simple function that is a shortcut for using the
 * Packer class for fields (vector<double>, list<int>, string, etc).  The
 * complement of this function is unpack_data, which takes a packed
 * vector<char> and writes data into the field.
 *
 * \sa rtt_dsxx::Packer, tstPacking_Utils.cc, and rtt_dsxx::unpack_data
 *
 * \param field container or string
 * \param packed vector<char> that is empty; data will be packed into it
 */
template<class FT>
void pack_data(const FT &field, std::vector<char> &packed)
{
    Insist (packed.empty(), "Passed a non-empty vector<char> to pack_data.");

    // determine the size of the field
    int field_size = field.size();

    // determine the number of bytes in the field
    int size = field_size * sizeof(typename FT::value_type) + sizeof(int);

    // make a vector<char> large enought to hold the packed field
    packed.resize(size);

    // make an unpacker and set it
    Packer packer;
    packer.set_buffer(size, &packed[0]);

    // pack up the number of elements in the field
    packer << field_size;

    // iterate and pack
    for (typename FT::const_iterator itr = field.begin(); itr != field.end();
	 itr++)
	packer << *itr;

    Ensure (packer.get_ptr() == &packed[0] + size);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Unpacking function.
 *
 * This function uses the rtt_dsxx::Unpacker to unpack a given field from a
 * vector<char>.  The field type is represented by the template argument FT.
 * The field type must have the following members defined:
 *
 * \arg FT::iterator const iterator for the field
 * \arg FT::resize() returns the number of elements in the field
 * \arg FT::begin() returns an iterator to the beginning of the field
 * \arg FT::end() returns an iterator to the end of the field
 * \arg FT::empty() determines if a container is empty
 *
 * Given these contraints, the function cannot be used to unpack a pointer
 * array; however, this is accomplished easily enough with the Unpacker class
 * alone.
 *
 * The data in the field is unpacked from a vector<char>. The data in the
 * vector<char> must be packed in a manner consistent with pack_data.  The
 * function checks for this.  Additionally, the field given to the function
 * must be empty.
 *
 * In summary, this is a simple function that is a shortcut for using the
 * Unpacker class for fields (vector<double>, list<int>, string, etc).  The
 * complement of this function is pack_data, which packs fields into a
 * vector<char>
 *
 * \sa rtt_dsxx::Unpacker, tstPacking_Utils.cc, and rtt_dsxx::pack_data
 *
 * \param field container or string that is empty; data will be unpacked into
 * it
 * \param packed vector<char> created by pack_data function (or in a manner
 * analogous) 
 */
template<class FT>
void unpack_data(FT &field, const std::vector<char> &packed)
{
    Insist (field.empty(), "Passed a non-empty field to unpack_data.");
    Require (packed.size() >= sizeof(int));

    // make an unpacker and set it
    Unpacker unpacker;
    unpacker.set_buffer(packed.size(), &packed[0]);

    // unpack the number of elements in the field
    int field_size = 0;
    unpacker >> field_size;

    // make a field big enough to hold all the elements
    field.resize(field_size);
    
    // unpack the data
    for (typename FT::iterator itr = field.begin(); itr != field.end(); itr++)
	unpacker >> *itr;

    Insist (unpacker.get_ptr() == &packed[0] + packed.size(),
	    "Incorrectly sized packed data given to unpack_data.");
}

} // end namespace rtt_dsxx

#endif                          // rtt_ds_Packing_Utils_hh

//---------------------------------------------------------------------------//
//                              end of ds++/Packing_Utils.hh
//---------------------------------------------------------------------------//
