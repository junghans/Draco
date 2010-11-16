//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   parser/Expression.hh
 * \author Kent Budge
 * \brief  Definition of class Expression
 * \note   Copyright 2006-2007 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef parser_Expression_hh
#define parser_Expression_hh

#include <vector>
#include <utility>
#include <ostream>
#include <map>
#include "ds++/SP.hh"
#include "Token_Stream.hh"
#include "Unit.hh"

namespace rtt_parser
{
using std::vector;
using std::pair;
using std::string;
using std::map;
using std::ostream;
using rtt_dsxx::SP;
using rtt_parser::Unit;
using rtt_parser::Token_Stream;

//===========================================================================//
/*!
 * \class Expression
 * \brief Represents a mathematical expression, typically parsed from user
 * input. 
 *
 * Test problems and numerical experiments often require that a problem domain
 * be initialized or driven in a way that varies in space and time.  For
 * example, one can imagine a radiation transport test problem in which the
 * initial temperature of a block of material is a linear function of radius.
 *
 * We provide the Expression class to represent the kind of simple mathematical
 * expressions that might describe such initalizations or drivers.  Such
 * Expressions will typically be build based on user input using the
 * Expression::parse static function.
 *
 * Expression itself is an abstract class representing all the kinds of
 * expressions that might be specified.  Concrete classes based on Expression
 * include Constant_Expression, Variable_Expression, Product_Expression, and
 * so forth.
 *
 * Expression provides the means to check unit compatibility. This is kept
 * distinct from evaluation to keep the cost of evaluation down. Doing so
 * requires that we hardwire Expression to a particular choice of units;
 * naturally we choose SI.
 *
 * Expressions are evaluated for an arbitrary set of variables. These are
 * specified when the Expression is parsed using a map from variable name (as
 * a std::string) to variable index and units. The map can specify any number
 * of variables. 
 */
//===========================================================================//

class Expression 
{
  public:

    enum Precedence
    {
        COMMA_PRECEDENCE,
        
        OR_PRECEDENCE,

        AND_PRECEDENCE,

        LESS_PRECEDENCE,
        LE_PRECEDENCE = LESS_PRECEDENCE,
        GREATER_PRECEDENCE = LESS_PRECEDENCE,
        GE_PRECEDENCE = LESS_PRECEDENCE,

        SUM_PRECEDENCE,
        DIFFERENCE_PRECEDENCE = SUM_PRECEDENCE,

        PRODUCT_PRECEDENCE,
        QUOTIENT_PRECEDENCE = PRODUCT_PRECEDENCE,

        NOT_PRECEDENCE,
        NEGATE_PRECEDENCE = NOT_PRECEDENCE,
        
        FUNCTION_PRECEDENCE
    };

    // CREATORS
    
    //! Destructor.
    virtual ~Expression(){}

    // MANIPULATORS

    // ACCESSORS

    //! Return the number of variables in the expression.
    unsigned number_of_variables() const { return number_of_variables_; }

    /*! Return the dimensions of the expression.
     *
     * The conversion factor <code> units().conv </code> is not significant.
     */
    Unit units() const { return units_; }

    // SERVICES

    //! Evaluate the expression.
    double operator()(vector<double> const &x) const;

    //! Indicate whether this is a constant expression.
    virtual bool is_constant() const { return false; }

    //! Indicate whether this expression is constant wrt a particular variable.
    bool is_constant(unsigned i) const
    {
        Require(i<number_of_variables_);
        
        return is_constant_(i);
    }

    //! Write a representation of the expression in C syntax.
    void write(Precedence p, vector<string> const &, ostream &out) const;

    //! Write a representation of the expression in C syntax.
    void write(vector<string> const &vars, ostream &out) const
    {
        write(COMMA_PRECEDENCE, vars, out);
    }

    // STATIC

    //! Parse an Expression from a Token_Stream.
    static
    SP<Expression const>
    parse( unsigned number_of_variables,
           map<string, pair<unsigned, Unit> > const &variables,
           Token_Stream &);

  protected:

    // IMPLEMENTATION

    /*! Create an Expression.
     *
     * \param number_of_variables Number of distinct variables in the
     * Expression.
     *
     * \param units Dimensions of the expression..
     */
    Expression(unsigned const number_of_variables,
               Unit const &units)
        :
        number_of_variables_(number_of_variables),
        units_(units)
    {}

    //! allow child classes access to Expression::evaluate
    static double evaluate_def_(SP<Expression const> const &e,
                               double const *const x)
    {
        Require(e!=SP<Expression>());
        
        return e->evaluate_(x);
    }

  private:

    // IMPLEMENTATION
    
    //! virtual hook for operator().
    virtual double evaluate_(double const *const x) const = 0;

    //! virtual hook for is_constant(unsigned)
    virtual bool is_constant_(unsigned) const = 0;

    //! virtual hook for write
    virtual void write_(Precedence precedence,
                        vector<string> const &vars,
                        ostream &) const = 0;


    // DATA

    //! Number of distinct independent variables in the Expression.
    unsigned number_of_variables_;
    
    //! Dimensions of the expression. The value of units_.conv is not
    //! significant except for constant Expressions, where it represents the
    //! value of the constant.
    Unit units_;
};

} // end namespace rtt_parser

#endif // parser_Expression_hh

//---------------------------------------------------------------------------//
//              end of parser/Expression.hh
//---------------------------------------------------------------------------//