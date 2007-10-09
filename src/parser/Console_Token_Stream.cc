//----------------------------------*-C++-*----------------------------------//
/*! 
 * \file Console_Token_Stream.cc
 * \author Kent G. Budge
 * \brief Definitions of Console_Token_Stream methods.
 * \note   Copyright � 2006-2007 Los Alamos National Security, LLC
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <iostream>
#include <sstream>
#include "Console_Token_Stream.hh"

namespace rtt_parser 
{
using namespace std;
//-------------------------------------------------------------------------//
/*!
 * 
 * Use the default Text_Token_Stream user-defined whitespace characters.
 */

Console_Token_Stream::Console_Token_Stream()
{
    Ensure(check_class_invariants());
    Ensure(location_() == "input");
}

//-------------------------------------------------------------------------//
/*!
 * \param ws User-defined whitespace characters.
 */

Console_Token_Stream::Console_Token_Stream(set<char> const &ws)
    : Text_Token_Stream(ws)
{
    Ensure(check_class_invariants());
    Ensure(location_() == "input");
    Ensure(whitespace()==ws);
}

//-------------------------------------------------------------------------//
/*!
 *
 * For a Console_Token_Stream, location is not a terribly  meaningful
 * concept.  So we return "input" as the location, which is true enough.
 *
 * \return The string "input".
 */

std::string Console_Token_Stream::location_() const
{
    return "input";
}
  
//-------------------------------------------------------------------------//
/*!
 * This function moves the next character from cin into the character buffer.
 */

void Console_Token_Stream::fill_character_buffer_()
{
    char c = cin.get();
    if (cin.fail())
    {
	character_push_back_('\0');
    }
    else
    {
	if (c=='\n')
        {
            c=';';
        }
	character_push_back_(c);
    }
    
    Ensure(check_class_invariants());
}

//-------------------------------------------------------------------------//
bool Console_Token_Stream::error_() const
{
    return cin.fail();
}

//-------------------------------------------------------------------------//
/*!
 * This function may be used to check whether the user has typed an end of
 * file character (ctrl-D on most Unix systems).
 *
 * \return \c true if an end of file character has been typed; \c false
 * otherwise.
 */

bool Console_Token_Stream::end_() const
{
    return cin.eof();
}

//-------------------------------------------------------------------------//
/*!
 * This function sends a message by writing it to the error console stream.
 */

void Console_Token_Stream::report(Token const &token,
                                  string const &message)
{
    std::cerr << token.location() << ": " << message << std::endl;

    Ensure(check_class_invariants());
}

//-------------------------------------------------------------------------//
/*!
 * This function sends a message by writing it to the error console stream.
 * This version assumes that the cursor gives the correct message location.
 */

void Console_Token_Stream::report(string const &message)
{
    Token token = lookahead();
    std::cerr << token.location() << ": " << message << std::endl;

    Ensure(check_class_invariants());
}
  
//-------------------------------------------------------------------------//
/*!
 * \author Kent G. Budge
 * \date Wed Jan 22 15:35:42 MST 2003
 * \brief Rewind the token stream.
 *
 * This function flushes cin and resets the error count.
 */

void Console_Token_Stream::rewind()
{
    cin.clear();    // Must clear the error/end flag bits.
    cin.seekg(0);

    Text_Token_Stream::rewind();

    Ensure(check_class_invariants());
    Ensure(cin.rdstate() == 0);
    Ensure(location_() == "input");
}

} // namespace rtt_parser

//---------------------------------------------------------------------------//
//                      end of Console_Token_Stream.cc
//---------------------------------------------------------------------------//
