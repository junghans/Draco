//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   ds++/Field_Traits.hh
 * \author Kent Budge
 * \brief  Define the Field_Traits class template
 * \note   Copyright (C) 2006-2007 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef dsxx_Field_Traits_hh
#define dsxx_Field_Traits_hh

#include <complex>

namespace rtt_dsxx
{

//===========================================================================//
/*!
 * \class Field_Traits
 * \brief Standardized description of field types
 *
 * Field_Traits is a traits class for field types.  Field types are types such
 * as double or std::complex<double> that represent a mathematical field. The
 * Field_Traits class is useful in template programming for capturing the
 * properties of such field types.
 */
//===========================================================================//

template<class Field>
class Field_Traits 
{
  public:

    //! Field types can be "labeled." For example, a value-plus-derivatives
    //! class has a field value that is labeled with its derivatives. The
    //! following typedef specifies the unlabeled type, by default the field
    //! type itself.
    typedef Field unlabeled_type;

    //! Return the unique zero element of the field. By default, this is
    //! convertible from double 0.
    static Field zero() { return 0.0; }
    //! Return the unique unit element of the field. By default, this is
    //! convertible from double 1.
    static Field one() { return 1.0; }
};

} // end namespace kgb_traits

#endif // traits_Field_Traits_hh

//---------------------------------------------------------------------------//
//              end of ds++/Field_Traits.hh
//---------------------------------------------------------------------------//
