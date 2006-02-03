//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/Index_Converter.hh
 * \author Mike Buksas
 * \date   Fri Jan 20 14:51:51 2006
 * \brief  Decleration and Definition of Index_Converter
 * \note   Copyright 2006 The Regents of the University of California.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef dsxx_Index_Converter_hh
#define dsxx_Index_Converter_hh

#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>

#include "Index_Set.hh"

namespace rtt_dsxx
{

//===========================================================================//
/*!
 * \class Index_Converter
 * \brief Utiltity class for converting one dimension indicies to and from
 * N-dimensional ones.
 *
 * \sa Index_Converter.cc for detailed descriptions.
 *
 */
/*! 
 * \example ds++/test/tstIndex_Converter.cc 
 * 
 */
//===========================================================================//
template<unsigned D, int OFFSET>
class Index_Converter : public Index_Set<D,OFFSET>
{
  public:

    typedef Index_Set<D,OFFSET> Base;

    //! Default constructor
    Index_Converter() { /* ... */ }

    //! Construct with just a pointer to the sizes
    Index_Converter(const unsigned* dimensions) { set_size(dimensions); }

    //! Construct a with all dimensions equal
    Index_Converter(const unsigned dimension)   { set_size(dimension); }

    //! Destructor.
    virtual ~Index_Converter() {/* ... */}

    //! Assignment operator for Index_Converter.
    Index_Converter& operator=(const Index_Converter &rhs);

    //! Re-assignment operator
    void set_size(const unsigned* dimensions);

    //! Uniform size re-assignment operator
    void set_size(unsigned size);


    // ACCESSORS

    //! Convert N-index to 1-index
    template <typename IT> int get_index(IT indices) const;

    //! Convert 1-index to N-index
    std::vector<int> get_indices(int index) const;

    //! Convert 1-index to N-index and store in provided iterator.
    template <typename IT> void get_indices(int index, IT begin) const;

    //! Extract a single N-index from the 1-index
    int get_single_index(int index, unsigned dimension) const;

    // Get the next index from a 1-index and direction
    int get_next_index(int index, int direction) const;


  private:

    // DATA

    //! Sizes of sub-grids of increasing dimension.
    unsigned sub_sizes[D];

    // IMPLEMENTATION
    
    void compute_sub_sizes();

    
};

//---------------------------------------------------------------------------//
// Function Definitions
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/**
 * \brief Resize the index converter object with new dimensions.
 * 
 */
template <unsigned D, int OFFSET>
inline void Index_Converter<D,OFFSET>::set_size(const unsigned* dimensions)
{
    Base::set_size(dimensions);
    compute_sub_sizes();
}

//---------------------------------------------------------------------------//
/**
 * \brief Resize the index converter with a uniform size
 *
 * \arg dimension The new size
 */
template <unsigned D, int OFFSET>
inline void Index_Converter<D,OFFSET>::set_size(unsigned dimension)
{
    Base::set_size(dimension);
    compute_sub_sizes();
}






//---------------------------------------------------------------------------//
/**
 * \brief Convert an N-index to a 1-index
 * 
 */
template <unsigned D, int OFFSET>
template <typename IT>
int Index_Converter<D,OFFSET>::get_index(IT indices) const
{

    Check(Base::indices_in_range(indices));

    int one_index_value = 0;
    int dimension = 0;
    for (IT index = indices; index != indices + D; ++index, ++dimension)
    {
        one_index_value += (*index - OFFSET) * sub_sizes[dimension];
    }
                           
    one_index_value += OFFSET;

    Ensure(Base::index_in_range(one_index_value));

    return one_index_value;
    
}


//---------------------------------------------------------------------------//
/**
 * \brief Convert a 1-index to an N-index. Store in provided pointer
 *
 * \arg index The index
 * \arg iterator The iterator pointing to the place to store the results. Must
 * be a reversible iterator, e.g. implement '--'
 * 
 */
template <unsigned D, int OFFSET>
template <typename IT>
void Index_Converter<D,OFFSET>::get_indices(int index, IT iter) const
{

    Check(Base::index_in_range(index));

    IT point(iter+D);

    index -= OFFSET;

    for (int dimension = D-1; dimension >= 0; --dimension)
    {
        *(--point) = index / sub_sizes[dimension] + OFFSET;
        index %= sub_sizes[dimension];
    }

    Ensure (point == iter);

}

//---------------------------------------------------------------------------//
/**
 * \brief Extract a single N-index from a 1-index
 *
 * \arg index The 1-index
 * \arg dimension The desired index dimension
 * 
 */
template <unsigned D, int OFFSET>
int Index_Converter<D,OFFSET>::get_single_index(int index, unsigned dimension) const
{

    Check(Base::index_in_range(index));
    Check(Base::dimension_okay(index));

    index -= OFFSET;

    if (dimension == 0)   return index % sub_sizes[1]   + OFFSET;
    if (dimension == D-1) return index / sub_sizes[D-1] + OFFSET;

    

        

}

//---------------------------------------------------------------------------//
/**
 * \brief Convert a 1-index to an N-index
 *
 * \arg index The 1-index value
 *
 * This function dispatches to the write-in-place version of the function and
 * stores the result in a local int[] array. It then constructs the return
 * vector in the return statement in order to allow the compiler to perform
 * return value optimization (RVO). This can potentially eliminate the
 * creation of a temporary return object.
 * 
 */
template <unsigned D, int OFFSET>
std::vector<int> Index_Converter<D,OFFSET>::get_indices(int index) const
{

    Check(Base::index_in_range(index));

    static int indices[D];

    get_indices(index, indices);

    // Construct in return statement for RVO.
    return std::vector<int>(indices, indices+D);
    
}




//---------------------------------------------------------------------------//
/**
 * \brief Return the next index in a given direction. Return -1 if this
 * direction is outside the range of indices
 *
 * \arg index   The index in question
 * \arg direction The direction, 1-based numbered (negative,positive) by
 * dimension.
 *
 */
template <unsigned D, int OFFSET>
int Index_Converter<D,OFFSET>::get_next_index(int index, int direction) const
{

    Check(Base::index_in_range(index));
    Check(Base::direction_okay(direction));

    --direction;

    unsigned direction_axis = direction / 2;
    int      direction_sign = 2*(direction % 2) - 1;

    int indices[D];

    get_indices(index, indices);

    indices[direction_axis] += direction_sign;

    if (!Base::index_in_range(indices[direction_axis], direction_axis)) return -1;

    return get_index(indices);

}






//---------------------------------------------------------------------------//
// IMPLEMENTATION ROUTINES
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/**
 * \brief Assign the internal data members.
 *
 * Used once the dimensions array has been filled.
 * 
 */
template <unsigned D, int OFFSET>
void Index_Converter<D,OFFSET>::compute_sub_sizes()
{

    Require(Base::sizes_okay());

    sub_sizes[0] = 1;

    static unsigned const * const dimensions = Base::get_dimensions();
    
    unsigned* end = std::partial_sum(
        dimensions, dimensions+D-1, sub_sizes+1, std::multiplies<unsigned>()
        );

    Ensure(end == sub_sizes+D);

}


} // end namespace rtt_dsxx

#endif // dsxx_Index_Converter_hh

//---------------------------------------------------------------------------//
//              end of ds++/Index_Converter.hh
//---------------------------------------------------------------------------//
