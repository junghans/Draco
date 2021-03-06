//----------------------------------*-C++-*----------------------------------------------//
/*!
 * \file   quadrature/Level_Symmetric.cc
 * \author Kelly Thompson
 * \date   Tue Feb 22 10:21:50 2000
 * \brief  A class representing an interval Gauss-Legendre quadrature set.
 * \note   Copyright 2016 Los Alamos National Security, LLC. All rights
 *         reserved. 
 */
//---------------------------------------------------------------------------------------//
// $Id: Quadrature.hh 6718 2012-08-30 20:03:01Z warsa $
//---------------------------------------------------------------------------------------//

#include "parser/utilities.hh"
#include "Level_Symmetric.hh"

namespace rtt_quadrature
{
using namespace rtt_parser;

//---------------------------------------------------------------------------------------//
/*static*/
SP<Quadrature> Level_Symmetric::parse(Token_Stream &tokens)
{
    Token token = tokens.shift();
    tokens.check_syntax(token.text()=="order", "expected an order");

    unsigned sn_order = parse_positive_integer(tokens);
    tokens.check_semantics(sn_order%2==0, "order must be even");

    tokens.check_syntax(tokens.shift().type()==END, "missing end?");

    return SP<Quadrature>(new Level_Symmetric(sn_order));
}

} // end namespace rtt_quadrature

//---------------------------------------------------------------------------------------//
//                       end of quadrature/Quadrature.cc
//---------------------------------------------------------------------------------------//
