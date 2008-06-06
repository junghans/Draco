//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   quadrature/QuadServices.hh
 * \author Kelly Thompson
 * \date   Mon Nov 8 11:17:12 2004
 * \brief  Provide Moment-to-Discrete and Discrete-to-Moment operations.
 * \note   Copyright 2004 The Regents of the University of California.
 *
 * Long description.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef quadrature_QuadServices_hh
#define quadrature_QuadServices_hh

#include "ds++/SP.hh"
#include "Ordinate.hh"
#include "Quadrature.hh"

namespace rtt_quadrature
{

//! Specify how to compute the Discrete-to-Moment operator.
enum QIM // Quadrature Interpolation Model
{
    SN,        /*!< Use the standard SN method. */
    GALERKIN,  /*!< Use Morel's Galerkin colocation method. */
    SVD        /*!< Let M be an approximate inverse of D. */
};

class QuadServices 
{
  public:

    // NESTED CLASSES AND TYPEDEFS

    /*!
     * \brief The typedef specifies how the moment index \f$ n \f$ maps
     *        to the double index \f$ (\ell,k) \f$.
     */
    typedef std::pair< unsigned, int > lk_index;

    // CREATORS
    
    //! Default constructor assumes that only isotropic scattering is used. 
    QuadServices( rtt_dsxx::SP< const Quadrature > const spQuad_,
                  QIM                              const qm = SN,
                  unsigned                         const expansionOrder = 0 );

//     //! Create a QuadServices from an ordinate set.
//     explicit QuadServices( OrdinateSet const & os );
    
    //! Constructor that allows the user to pick the (k,l) moments to use.
    //! \todo This still needs to be defined.
    QuadServices( rtt_dsxx::SP< const Quadrature > const   spQuad_,
		  std::vector< lk_index >          const & lkMoments_,
                  QIM                              const   qm = SN );

    //! Copy constructor (the long doxygen description is in the .cc file).
    QuadServices( QuadServices const & rhs );

    //! Destructor.
    virtual ~QuadServices() { /* empty */ }

    // MANIPULATORS
    
    //! Assignment operator for QuadServices.
    QuadServices& operator=( QuadServices const & rhs );

    //! Compute extra "moment-to-discrete" entries that are needed for starting direction ordinates.
    double augmentM( unsigned moment, Ordinate const & Omega ) const;

    // ACCESSORS

    //! \brief Return the moment-to-discrete operator.
    std::vector< double > getM() const { return Mmatrix; }

    //! \brief Return the discrete-to-moment operator.
    std::vector< double > getD() const { return Dmatrix; }

    std::vector< double > applyM( std::vector< double > const & phi ) const;
    std::vector< double > applyD( std::vector< double > const & psi ) const;

    //! \brief Provide the number of moments used by QuadServices.
    unsigned getNumMoments() const { return numMoments; }

    //! \brief Pretty print vector<T> in a 2D format.
    template< typename T > 
    void print_matrix( std::string           const & matrix_name,
		       std::vector<T>        const & x,
		       std::vector<unsigned> const & dims ) const;
    
    //! \brief Return the (l,k) index pair associated with moment index n.
    lk_index lkPair( unsigned n ) const { Require( n<numMoments ); return n2lk[n]; }

    //! \brief Provide the number of moments used by QuadServices.
    std::vector< lk_index > get_n2lk() const { return n2lk; }

    //! Helper functions to compute coefficients
    static
    double compute_azimuthalAngle( double const mu,
				   double const eta,
				   double const xi ) ;

    //! Helper function to check validity of LU matrix
    static
    bool diagonal_not_zero( std::vector<double> const & vec,
                            int m, int n ) ;
    //! Checks
    bool D_equals_M_inverse(void) const;
    bool D_0_equals_wt(void) const;

  private:

    // NESTED CLASSES AND TYPEDEFS

    // IMPLEMENTATION
    
    //! \brief constuct maps between moment index n and the tuple (k,l).
    // This can optionally be provided by the user.
    std::vector< lk_index > compute_n2lk(    unsigned const L ) const;
    std::vector< lk_index > compute_n2lk_1D( unsigned const L ) const;
    std::vector< lk_index > compute_n2lk_2D_traditional( unsigned const L ) const;
    std::vector< lk_index > compute_n2lk_3D_traditional( unsigned const L ) const;
    std::vector< lk_index > compute_n2lk_2D_morel( void ) const;
    std::vector< lk_index > compute_n2lk_3D_morel( void ) const;

    //! Build the Mmatrix.
    std::vector< double > computeM(void) const;
    std::vector< double > computeD(void) const;
    std::vector< double > computeD_morel(      void) const;
    std::vector< double > computeD_traditional(void) const;
    std::vector< double > computeD_svd(        void) const;
    
    // DATA
    rtt_dsxx::SP< const Quadrature > const spQuad;
    QIM                              const qm;
    std::vector< lk_index >          const n2lk;
    unsigned                         const numMoments;
    std::vector< double >            const Mmatrix;
    std::vector< double >            const Dmatrix;

};

} // end namespace rtt_quadrature

#include "QuadServices.i.hh"

#endif // quadrature_QuadServices_hh

//---------------------------------------------------------------------------//
//              end of quadrature/QuadServices.hh
//---------------------------------------------------------------------------//
