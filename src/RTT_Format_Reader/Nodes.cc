//----------------------------------*-C++-*--------------------------------//
// Nodes.cc
// B.T. Adams
// 7 June 00
/*! 
 * \file   RTT_Format_Reader/Nodes.cc
 * \author B.T. Adams
 * \date   Wed Jun 7 10:33:26 2000
 * \brief  Implementation file for RTT_Format_Reader/Nodes class.
 */
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#include "Nodes.hh"

namespace rtt_RTT_Format_Reader
{
/*! 
 * \brief Parses the nodes block data from the mesh file via calls to private 
 *        member functions.
 * \param meshfile Mesh file name.
 */
void Nodes::readNodes(ifstream & meshfile)
{
    readKeyword(meshfile);
    readData(meshfile);
    readEndKeyword(meshfile);
}
/*!
 * \brief Reads and validates the nodes block keyword.
 * \param meshfile Mesh file name.
 */
void Nodes::readKeyword(ifstream & meshfile)
{
    string dummyString;

    meshfile >> dummyString;
    Insist(dummyString == "nodes",
	   "Invalid mesh file: nodes block missing");
    std::getline(meshfile, dummyString);
}

/*!
 * \brief Read next integer value from mesh file, skipping comments.
 * \param meshfile Input file stream for meshfile.
 * \return the next integer in the meshfile.
 */
int Nodes::readNextInt( ifstream & meshfile )
{
    using std::atoi;
    
    int retVal(0);
    string dummyString;
    bool commentLine = true;
    while( commentLine ) 
    {
	meshfile >> dummyString;
	if( dummyString.rfind("!") == string::npos )
	{
	    commentLine = false;
	    retVal = atoi( dummyString.c_str() );
	}
	else
	{ // Dump everything from here to the end of the line.
	    std::getline( meshfile, dummyString );
	    commentLine = true;
	}
    }
    return retVal;
}

/*!
 * \brief Reads and validates the nodes block data.
 * \param meshfile Mesh file name.
 */
void Nodes::readData(ifstream & meshfile)
{
    string dummyString;
    int nodeNum;

    for (unsigned i = 0; i < dims.get_nnodes(); ++i)
    {
	nodeNum = readNextInt( meshfile );
	// meshfile >> nodeNum;
	Insist(nodeNum == i+1,
	       "Invalid mesh file: node index out of order");
	Check(i<coords.size());
	for (unsigned j = 0; j < dims.get_ndim(); ++j)
	{
	    Check(j<coords[i].size());
	    meshfile >> coords[i][j];
	}
	Check(i<parents.size());
	meshfile >> parents[i];
	--parents[i];
	Check(i<flags.size());
	for (unsigned j = 0; j < dims.get_nnode_flag_types(); ++j)
	{
	    Check(j<flags[i].size());
	    meshfile >> flags[i][j];
	    Insist(nodeFlags.allowed_flag(j, flags[i][j]),
		   "Invalid mesh file: illegal node flag");
	}
	std::getline(meshfile, dummyString);
    }
}
/*!
 * \brief Reads and validates the end_nodes block keyword.
 * \param meshfile Mesh file name.
 */
void Nodes::readEndKeyword(ifstream & meshfile)
{
    string dummyString;

    meshfile >> dummyString;
    Insist(dummyString == "end_nodes",
	   "Invalid mesh file: nodes block missing end");
    std::getline(meshfile, dummyString);       // read and discard blank line.
}

} // end namespace rtt_RTT_Format_Reader

//---------------------------------------------------------------------------//
//                     end of RTT_Format_Reader/Nodes.cc
//---------------------------------------------------------------------------//
