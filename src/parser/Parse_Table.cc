//----------------------------------*-C++-*----------------------------------//
/*! 
 * \file Parse_Table.cc
 * \author Kent G. Budge
 * \date Wed Jan 22 15:18:23 MST 2003
 * \brief Definitions of member functions of class Parse_Table
 * \note   Copyright � 2003 The Regents of the University of California.
 *
 * revision history:
 * 0) original revision
 * 1) kgbudge (03/12/03):  
 *    Fix DBC assertions. Add new DBC assertions.
 *    Add code to check for and resolve keyword ambiguities.
 * 2) kgbudge (03/08/10): 
 *    Solo inspection of documentation, assertions, and tests. 
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include "ds++/Assert.hh"
#include "Parse_Table.hh"
#include "Token_Stream.hh"

namespace rtt_parser 
{
using namespace std;

//---------------------------------------------------------------------------//
/*! 
 * \brief Construct a parse table with the specified keywords.
 * 
 * \param table 
 * Pointer to an array of keywords.
 * \param count
 * Length of the array of keywords pointed to by \c table.
 *
 * \pre <code>count==0 || table != NULL</code>
 * \pre <code>std::find_if(table, table+count, Is_Well_Formed_Keyword)</code>
 *
 * \post <code>Get_Flags()==0</code>
 *
 * \throw std::bad_alloc If there is not enough memory to initialize the table.
 * \throw rtt_dsxx::assertion If the keyword table is ill-formed or ambiguous.
 *
 * \note See documentation for Parse_Table::Add for an explanation of the
 * low-level argument list.
 */

Parse_Table::Parse_Table(Keyword const *const table, size_t const count)
    : flags(0)
{
    Require(count == 0  ||  table != NULL);
    // Require(std::find_if(table, table+count, Is_Well_Formed_Keyword));

    Add(table, count);

    Ensure(check_class_invariants());
    Ensure(Get_Flags()==0);
}

//-------------------------------------------------------------------------//
/*!
 * \brief Add a set of keywords to a Parse_Table.
 *
 * \param table
 * Array of keywords to be added to the table.
 *   
 * \param count
 * Number of valid elements in the array of keywords.
 *
 * \pre <code>count==0 || table != NULL</code>
 * \pre <code>std::find_if(table, table+count, Is_Well_Formed_Keyword)</code>
 *
 * \throw std::bad_alloc If there is not enough memory to initialize the table.
 * \throw rtt_dsxx::assertion If the keyword table is ill-formed or ambiguous.
 *
 * \note The argument list reflects the convenience of defining raw keyword
 * tables as static C arrays.  This justifies a low-level interface in place
 * of, say, vector<Keyword>.
 */

void Parse_Table::Add(Keyword const *const table, size_t const count)
{
    Require(count == 0  ||  table != NULL);

    // Add the new keywords.
    for (size_t i=0; i<count; i++)
    {
	Require(Is_Well_Formed_Keyword(table[i]));

	push_back(table[i]);
    }

    Keyword_Compare const comp(flags);

    // Sort the parse table.
    std::sort(begin(), end(), comp);

    // Look for ambiguous keywords, and resolve the ambiguity, if possible.
    std::vector<Keyword>::iterator i = begin();
    while (i+1 != end())
    {
	Check(i->moniker != NULL  &&  (i+1)->moniker != NULL);
	if (!comp(i[0], i[1]))
	{
	    // kptr[i] and kptr[i+1] have the same moniker.
	    if (i->func == (i+1)->func  &&  i->index == (i+1)->index)
	    {
		// They have the same parse function and index.  No 
		// real ambiguity.  Delete one.

		// This can occur when there is diamond inheritance
		// in a parse table hierarchy, e.g., this parse table
		// copies keywords from two other parse tables, which
		// in turn copy keywords from a fourth parse table.

		erase(i+1);
	    }
	    else
	    {
		// The keywords are genuinely ambiguous.
		using std::ostringstream;
		using std::endl;
		ostringstream err;
		err << "An ambiguous keyword was detected in a "
		    << "Parse_Table:  " << i->moniker << endl
		    << "Modules: ";
		if (i->module) err << i->module;
		else err << "<null>";
		err << ' ';
		if ((i+1)->module) err << (i+1)->module;
		else err << "<null>";
		err << endl;
		throw rtt_dsxx::assertion(err.str().c_str());
	    }
	}
	else
	{
	    i++;
	}
    }
    
    Ensure(check_class_invariants());
}

//-------------------------------------------------------------------------//
/*!
 * \brief Parse the token stream.
 *
 * Parse the stream of tokens until an END, EXIT, or ERROR token is 
 * reached.
 *
 * \param tokens
 * The Token Stream from which to obtain the stream of tokens.
 *
 * \return The terminating token: either END, EXIT, or ERROR.
 *
 * \throw rtt_dsxx::assertion If the keyword table is ambiguous.
 */

Token Parse_Table::Parse(Token_Stream &tokens) const
{
    // The is_recovering flag is used during error recovery to suppress
    // additional error messages.  This reduces the likelihood that a single
    // error in a token stream will generate a large number of error
    // messages.

    bool is_recovering = false;
        
    // Create a comparator object that will be used to attempt to match
    // keywords in the Token_Stream to keywords in the Parse_Table.  This
    // comparator object incorporates the current settings of the
    // Parse_Table, such as case sensitivity and partial matching options.

    Keyword_Compare const comp(flags);

    // Now begin the process of pulling keywords off the input token stream,
    // and attempting to match these to the keyword table.
    
    for (;;)
    {
	Token const token = tokens.Shift();

	// The END, EXIT, and ERROR tokens are terminating tokens.  EXIT
	// means the end of the token stream has been reached.  END is used
	// to flag the end of a nested parse, where the result of matching a
	// keyword in one parse table is to begin parsing keywords in a
	// second parse table.  An END indicates that the second parse table
	// should return control to the first parse table.  ERROR means that
	// something went very wrong and we're probably hosed, but it allows
	// some error recovery from within a nested parse table.

	if (token.Type()==END || token.Type()==EXIT || token.Type()==ERROR) 
	{
	    return token;
	}
	
	// A Parse_Table assumes that every construct begins with a keyword.
	// This keyword is matched to the keyword table, and if a match is
	// found, control is directed to the associated parse function, which
	// can be written to accept just about any construct you wish.
	// However, by the time return controls from a parse function, the
	// token stream should be pointing either at a terminating token or
	// the next keyword.

	if (token.Type()==KEYWORD)
	{
	    // Attempt to match the keyword to the keyword table.  The
	    // following call returns an iterator pointing to the first
	    // keyword in the table whose lexical ordering is greater than or
	    // equal to the keyword token.  The lexical ordering is supplied
	    // by the comp object.

	    vector<Keyword>::const_iterator const match = 
		lower_bound(begin(), end(), token, comp);
	    
	    if (match == end() || 
		comp.kt_comparison(match->moniker, token.Text().c_str()) != 0)
	    {
		// The token was not lexically equal to anything in the
		// keyword table.  In other words, the keyword is
		// unrecognized by the Parse_Table.  The error recovery
		// procedure is to generate a diagnostic, then pull
		// additional tokens off the token stream (without generating
		// further diagnostics) until one is recognized as either a
		// keyword or a terminating token.  We implement this
		// behavior by setting the is_recovering flag when the first
		// invalid token is encountered, and resetting this flag as
		// soon as a valid token is encountered.

		if (!is_recovering)
		{
		    // We are not recovering from a previous error.  Generate
		    // a diagnostic, and flag that we are now in error
		    // recovery mode. 

		    tokens.Report_Semantic_Error(token,
						  ": unrecognized keyword: " + 
						  token.Text());
		    is_recovering = true;
		}
		// else we are in recovery mode, and additional diagnostics
		// are disabled until we see a valid construct.
	    } 
	    else
	    {
		// We have a valid match.  However, depending on Parse_Table
		// options, the match might be ambiguous.  For example if the
		// Parse_Table option to allow partial matches is active, the
		// keyword token may partially match more than one keyword in
		// the keyword table.  Check for an ambiguous match:

		if (match+1 != end() &&
		    comp.kt_comparison(match[1].moniker,
				       token.Text().c_str()) == 0)
		{
		    // The match is ambiguous.  This is diagnosed whether or
		    // not we are already in recovery mode, but it does put
		    // us into recovery mode.

		    tokens.Report_Semantic_Error(token, 
						  "ambiguous keyword: " +
						  token.Text());
		    is_recovering = true;
		}
		else
		{
		    is_recovering = false;  
		    // We successfully processed something, so we are no
		    // longer in recovery mode.
		    
		    try 
		    {
			// Call the parse function associated with the
			// keyword.
			match->func(tokens, match->index);
		    }
		    catch (const Syntax_Error &)
		    {
			// If the parse function detects a syntax error, and
			// if it does not have its own error recovery policy
			// (or is unable to recover), it should call
			// tokens->Report_Syntax_Error which generates a
			// diagnostic and throws a Syntax_Error
			// exception. This puts the main parser into recovery
			// mode.

			is_recovering = true;
		    }
		}
	    }
	}
	else if (token.Type() == OTHER && token.Text() == ";")
	{
	    // Treat a semicolon token as an empty keyword.  We are no longer
	    // in recovery mode, but we don't actually do anything.

	    is_recovering = false;
	}
	else
	{
	    // The next token in the token stream is not a keyword,
	    // indicating a syntax error. Error recovery consists of
	    // generating a diagnostic message, then continuing to pull
	    // tokens off the token stream (without generating any further
	    // diagnostics) until one is recognized as either a keyword or a
	    // terminating token.  We implement this behavior by setting the
	    // is_recovering flag when the first invalid token is
	    // encountered, and resetting this flag as soon as a valid token
	    // is encountered.

	    if (!is_recovering)
	    {
		// We are not recovering from a previous error.  Generate a
		// diagnostic, and flag that we are now in error recovery
		// mode.

		std::ostringstream msg;
		msg << "expected a keyword, but saw " << token.Text();
		tokens.Report_Semantic_Error(token, msg.str());
		is_recovering = true;
	    }
	    // else we are in recovery mode, and additional diagnostics are
	    // disabled until we see a valid construct.
	}
    }
}

//-------------------------------------------------------------------------//
/*!
 * \brief Get the option flags.
 *
 * \return The option flags, ORed together.
 */

unsigned Parse_Table::Get_Flags() const
{
    return flags;
}

//-------------------------------------------------------------------------//
/*!
 * \brief Set option flags.
 *
 * \param f
 * Flags to be set, ORed together.
 *
 * \post <code>Get_Flags() == f</code>
 */

void Parse_Table::Set_Flags(unsigned f)
{
    Require(check_class_invariants());

    flags = f;

    Add(NULL, 0U);  
    // The keyword list needs to be sorted and checked.  For example, if the
    // options are changed so that a previously case-sensitive Parse_Table is
    // no longer case-sensitive, then the ordering changes, and previously
    // unambiguous keywords may become ambiguous.
    
    Ensure(Get_Flags() == f);

    Ensure(check_class_invariants());
}

//-------------------------------------------------------------------------//
/*!
 * \brief Constructor for comparison functor for sorting keyword tables.
 *
 * The functor is used by a Parse_Table to sort its keyword list
 * using std::sort.
 *
 * \param flags
 * The flags controlling this comparator's operations.
 *
 * \pre <CODE>pt!=NULL</CODE>
 */

Parse_Table::Keyword_Compare::Keyword_Compare(unsigned char const flags)
    : flags(flags)
{
} 

//-------------------------------------------------------------------------//
/*!
 * \author Kent G. Budge
 * \date Wed Jan 22 15:35:42 MST 2003
 * \brief Comparison function for sorting keyword tables.
 *
 * This function is used by a Parse_Table to sort its keyword list using 
 * std::sort.  
 *
 * If no option flags are set, monikers will test equal if they are identical.
 *
 * If the CASE_INSENSITIVE option is set, monikers will test equal if they
 * are identical when converted to uppercase. 
 *
 * Note that PARTIAL_IDENTIFIER_MATCH is ignored when comparing monikers
 * from two keywords. It is relevant only when comparing monikers with input
 * tokens. 
 *
 * A valid Parse_Table may not contain any keywords that test equal.
 * 
 * \param k1
 * The first Keyword to be compared.
 *
 * \param k2
 * The second Keyword to be compared.
 *
 * \pre <CODE>k1.moniker!=NULL</CODE>
 *
 * \pre <CODE>k2.moniker!=NULL</CODE>
 *
 * \return <CODE>kk_comparison(k1.moniker, k2.moniker)<0 </CODE>
 */

bool Parse_Table::Keyword_Compare::operator()(const Keyword &k1, 
					      const Keyword &k2) const
{
    Require(k1.moniker);
    Require(k2.moniker);

    return kk_comparison(k1.moniker, k2.moniker)<0;
}

int Parse_Table::Keyword_Compare::kk_comparison(const char *m1, 
						const char *m2) const
{
    using namespace std;

    Require(m1!=NULL);
    Require(m2!=NULL);

    if (flags & CASE_INSENSITIVE)
    {
	while (*m1 != '\0' && *m2 != '\0')
	{
	    char c1 = *m1++;
	    char c2 = *m2++;
	    if (islower(c1)) c1 = toupper(c1);
	    if (islower(c2)) c2 = toupper(c2);
	    if (c1<c2) return -1;
	    if (c1>c2) return 1;
	}
	char c1 = *m1++;
	char c2 = *m2++;
	if (islower(c1)) c1 = toupper(c1);
	if (islower(c2)) c2 = toupper(c2);
	if (c1<c2) return -1;
	if (c1>c2) return 1;
	return 0;
    }
    else
    {
	return strcmp(m1, m2);
    }
}

//-------------------------------------------------------------------------//
/*!
 * \author Kent G. Budge
 * \date Wed Jan 22 15:35:42 MST 2003
 * \brief Comparison function for finding token match in keyword table.
 *
 * This function is used by a Parse_Table to match keywords to identifier
 * tokens using std::lower_bound.
 *
 * If no option flags are set, the match must be exact.
 *
 * If CASE_INSENSITIVE is set, the match must be exact after conversion
 * to uppercase.
 *
 * If PARTIAL_IDENTIFIER_MATCH is set, each identifier in the token must
 * be a prefix of the corresponding identifier in the keyword, after
 * conversion to uppercase if CASE_INSENSITIVE is also set.  For example,
 * the token "ABD" matches the keyword "ABDEF" but not the keyword "AB".
 *
 * \param keyword
 * The Keyword to be compared.
 *
 * \param token
 * The token to be compared.
 *
 * \pre <CODE>token.Type()==KEYWORD</CODE>
 * \pre <CODE>keyword.moniker!=NULL</CODE>
 *
 * \return <CODE>comparison(keyword.moniker,
 *                          token.Text().c_str())<0 </CODE>
 */

bool Parse_Table::Keyword_Compare::operator()(const Keyword &k1,
					      const Token &k2) const
{
    Require(k1.moniker);

    return kt_comparison(k1.moniker, k2.Text().c_str())<0;
}

bool Parse_Table::Keyword_Compare::operator()( const Token   &k2,
                                               const Keyword &k1 ) const
{
//     Require(k1.moniker);
//     return kt_comparison(k1.moniker, k2.Text().c_str())<0;
    return ! operator()(k1,k2);
}

int Parse_Table::Keyword_Compare::kt_comparison(const char *m1,
						const char *m2) const
{
    using namespace std;

    Require(m1!=NULL);
    Require(m2!=NULL);

    if (flags & PARTIAL_IDENTIFIER_MATCH)
    {
	while (*m1 != '\0' && *m2 != '\0')
	{
	    while (isalnum(*m1) && isalnum(*m2))
	    {
		char c1 = *m1++;
		char c2 = *m2++;
		if (flags & CASE_INSENSITIVE)
		{
		    if (islower(c1)) c1 = toupper(c1);
		    if (islower(c2)) c2 = toupper(c2);
		}
		if (c1<c2) return -1;
		if (c1>c2) return 1;
	    }
	    if (*m1 == ' ' && *m2 != ' ') return -1;
	    while (isalnum(*m1)) m1++;
	    while (isalnum(*m2)) m2++;
	}
	if (*m1 == '\0' && *m2 != '\0') return -1;
	if (*m1 != '\0' && *m2 == '\0') return 1;
	Check(*m1=='\0' && *m2=='\0');
	return 0;
    }
    else
    {
	if (flags & CASE_INSENSITIVE)
	{
	    while (*m1 != '\0' && *m2 != '\0')
	    {
		char c1 = *m1++;
		char c2 = *m2++;
		if (islower(c1)) c1 = toupper(c1);
		if (islower(c2)) c2 = toupper(c2);
		if (c1<c2) return -1;
		if (c1>c2) return 1;
	    }
	    char c1 = *m1++;
	    char c2 = *m2++;
	    if (islower(c1)) c1 = toupper(c1);
	    if (islower(c2)) c2 = toupper(c2);
	    if (c1<c2) return -1;
	    if (c1>c2) return 1;
	    return 0;
	}
	else
	{
	    return strcmp(m1, m2);
	}
    } 
}

//---------------------------------------------------------------------------//
/*! 
 * \brief Check whether a keyword satisfies the requirements for use in
 * a Parse_Table.
 * 
 * \param key Keyword to be checked.
 *
 * \return \c false unless all the following conditions are met:
 * <ul><li>\c key.moniker must point to a null-terminated string consisting
 * of one or more valid C++ identifiers separated by single spaces.</li>
 * <li>\c key.func must point to a parsing function.</li></ul>
 */

bool Is_Well_Formed_Keyword(const Keyword &key)
{
    using namespace std;

    if (key.moniker == NULL  ||  key.func == NULL) return false;
    const char *cptr = key.moniker;
    for (;;)
    {
	// Must be at the start of a C identifier, which begins with an
	// alphabetic character or an underscore.
	if (*cptr != '_' && !isalpha(*cptr)) return false;

	// The subsequent characters in a C identifier must be alphanumeric
	// characters or underscores.
	while (*cptr == '_' || isalnum(*cptr)) cptr++;

	// If the identifier is followed by a null, we're finished scanning a
	// valid keyword.
	if (*cptr == '\0') return true;

	// Otherwise, if the next character is not a space, it's not a valid
	// keyword. 
	if (*cptr != ' ') return false;

	// Skip over the space. cptr should now point to the start of the
	// next C identifier, if this is a valid keyword.
	cptr++;
    }
}
 
//---------------------------------------------------------------------------//
bool Parse_Table::check_class_invariants() const
{
    // The keyword table must be well-formed, sorted, and unambiguous.

    Keyword_Compare const comparator(flags);
    for (std::vector<Keyword>::const_iterator i=begin(); i!=end(); ++i)
    {
	if (!Is_Well_Formed_Keyword(i[0])) return false;
	if (i+1!=end())
	{
	    if (comparator.kk_comparison(i[0].moniker, i[1].moniker)>=0)
		return false;
	}
    }
    return true;
}

}  // rtt_parser
//---------------------------------------------------------------------------//
//                      end of Parse_Table.cc
//---------------------------------------------------------------------------//
