//----------------------------------*-C++-*----------------------------------//
/*! 
 * \file   ds++/draco_math.hh
 * \author Kent G. Budge
 * \date   Wed Jan 22 15:18:23 MST 2003
 * \brief  New or overloaded <cmath> or cmath-like functions.
 * \note   Copyright (C) 2003-2013 Los Alamos National Security, LLC.
 *         All rights reserved.
 *
 * History:
 * 2013-10-17 Many small header files were combined to create draco_math.hh.
 */
//---------------------------------------------------------------------------//
// $Id$ 
//---------------------------------------------------------------------------//

#ifndef rtt_dsxx_DracoMath_hh
#define rtt_dsxx_DracoMath_hh

#include <cmath>
#include <cstdlib>
#include <complex>
#include <algorithm>
#include <iterator>
#include <functional>

namespace rtt_dsxx
{

//---------------------------------------------------------------------------//
/*
 * abs.hh
 *
 * Absolute values are a mess in the STL, in part because they are a mess in
 * the standard C library. We do our best to give a templatized version here.
 */
//---------------------------------------------------------------------------//
/*! 
 * \brief abs
 * 
 * \arg \a Ordered_Group
 *         A type for which operator< and unary operator- are defined.
 * \param a Argument whose absolute value is to be calculated.
 * \return \f$|a|\f$
 */
template <typename Ordered_Group>
inline Ordered_Group abs(Ordered_Group a)
{ 
    if (a<0) return -a;
    else     return  a;
}

// Specialization for standard types - There is no standard abs function for
// float -- one reason why we define this template!
template <>
inline double abs(double a)
{
    return std::fabs(a);
}
template<>
inline int abs(int a)
{
    return std::abs(a);
}
template<>
inline long abs(long a)
{
    return std::labs(a);
}

//---------------------------------------------------------------------------//
// conj.hh
//---------------------------------------------------------------------------//
/*! 
 * \brief Return the conjugate of a quantity.
 * 
 * The default implementation assumes a field type that is self-conjugate,
 * such as \c double.  An example of a field type that is \em not
 * self-conjugate is \c complex.
 *
 * \arg \a Field type
 */
template<typename Field>
inline Field conj(const Field &x)
{
    return x;
}

// Specializations for non-self-conjugate types
template<>
inline std::complex<double> conj(const std::complex<double> &x)
{
    return std::conj(x);
}

//---------------------------------------------------------------------------//
// cube.hh
//---------------------------------------------------------------------------//
/*! 
 * \brief Return the cube of a value.
 * 
 * \arg \a Semigroup A type representing an algebraic structure closed under
 * multiplication such as the integers or the reals.
 *
 * \param x Value to be cubed.
 * \return \f$x^3\f$
 */
template <typename Semigroup>
inline Semigroup cube(Semigroup const &x)
{
    return x*x*x;
}

//---------------------------------------------------------------------------//
// dim.hh
//---------------------------------------------------------------------------//
/*! 
 * \brief Return the positive difference of the arguments.
 * 
 * This is a replacement for the FORTRAN DIM function.
 *
 * \arg \a Ordered_Group_Element A type for which operator< and unary
 * operator- are defined and which can be constructed from a literal \c 0.
 *
 * \param a
 * Minuend
 *
 * \param b
 * Subtrahend
 *
 * \return \f$max(0, a-b)\f$
 *
 * \deprecated A FORTRAN relic that should disappear eventually.
 */
template <typename Ordered_Group_Element>
inline Ordered_Group_Element dim(Ordered_Group_Element a, 
				 Ordered_Group_Element b)
{
    if (a<b)
        return Ordered_Group_Element(0);
    else
        return a-b;
}

//---------------------------------------------------------------------------//
// square.hh
//---------------------------------------------------------------------------//
/*! 
 * \brief Return the square of a value.
 * 
 * \arg \a Semigroup A type representing an algebraic structure closed under
 * multiplication, such as the integers or the reals.
 *
 * \param x Value to be squared.
 * \return \f$x^2\f$
 */
template <typename Semigroup>
inline Semigroup square(const Semigroup &x)
{
    return x*x;
}

//---------------------------------------------------------------------------//
// pythag.hh
//---------------------------------------------------------------------------//
/*! 
 * \brief Compute the hypotenuse of a right triangle.
 *
 * This function evaluates the expression \f$\sqrt{a^2+b^2}\f$ in a way that
 * is insensitive to roundoff and preserves range.
 *
 * \arg \a Real A real number type
 * \param a First leg of triangle
 * \param b Second leg of triangle
 * \return Hypotenuse, \f$\sqrt{a^2+b^2}\f$
 */

template<typename Real>
inline double pythag(Real a, Real b)
{
    Real absa = abs(a), absb = abs(b);
    if (absa>absb)
    {
	return absa*std::sqrt(1.0+square(absb/absa));
    }
    else
    {
	if (absb==0.0)
	    return 0.0;
	else
	    return absb*std::sqrt(1.0+square(absa/absb));
    }
}

//---------------------------------------------------------------------------//
// sign.hh
//---------------------------------------------------------------------------//
/*!
 * \brief  Transfer the sign of the second argument to the first argument.
 *
 * This is a replacement for the FORTRAN SIGN function.  It is useful in
 * numerical algorithms that are roundoff or overflow insensitive and should
 * not be deprecated.
 *
 * \arg \a Ordered_Group
 * A type for which \c operator< and unary \c operator- are defined and which
 * can be compared to literal \c 0.
 *
 * \param a
 * Argument supplying magnitude of result.
 *
 * \param b
 * Argument supplying sign of result.
 *
 * \return \f$|a|sgn(b)\f$
 */
 
template <typename Ordered_Group>
inline Ordered_Group sign(Ordered_Group a,
			  Ordered_Group b)
{
    using rtt_dsxx::abs; // just to be clear
    
    if (b<0) 
	return -abs(a);
    else 
	return abs(a);
}

} // namespace rtt_dsxx

#endif // rtt_dsxx_DracoMath_hh

//---------------------------------------------------------------------------//
// end of draco_math.hh
//---------------------------------------------------------------------------//


