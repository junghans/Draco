//----------------------------------*-C++-*--------------------------------//
// CellDefs.cc
// B.T. Adams
// 7 June 00
/*! 
 * \file   RTT_Format_Reader/CellDefs.cc
 * \author B.T. Adams
 * \date   Wed Jun 7 10:33:26 2000
 * \brief  Implementation file for RTT_Format_Reader/CellDefs class.
 */
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#include "CellDefs.hh"

namespace rtt_RTT_Format_Reader
{
/*!
 * \brief Parses the cell_defs (cell definitions) data block from the mesh 
 *        file via calls to private member functions.
 * \param meshfile Mesh file name.
 */
void CellDefs::readCellDefs(ifstream & meshfile)
{
    readKeyword(meshfile);
    readDefs(meshfile);
    readEndKeyword(meshfile);
}
/*!
 * \brief Reads and validates the cell_defs block (cell definitions) keyword.
 * \param meshfile Mesh file name.
 */
void CellDefs::readKeyword(ifstream & meshfile)
{
    string dummyString;

    meshfile >> dummyString;
    Insist(dummyString == "cell_defs",
	   "Invalid mesh file: cell_defs block missing");
    std::getline(meshfile, dummyString);
}
/*!
 * \brief Reads and validates the cell_defs (cell definitions) block data.
 * \param meshfile Mesh file name.
 */
void CellDefs::readDefs(ifstream & meshfile)
{
    int cellDefNum;
    string dummyString;

    for (int i = 0; i < dims.get_ncell_defs(); ++i)
    {
	meshfile >> cellDefNum >> dummyString;
	Insist(cellDefNum == i+1,
	       "Invalid mesh file: cell def out of order");
	// Ignore plurals in cell definitions
	if (dummyString[dummyString.size()-1] == 's')
	    dummyString.resize(dummyString.size()-1);
	defs[i] = new CellDef(*this, dummyString);
	std::getline(meshfile, dummyString);
	defs[i]->readDef(meshfile);
    }
}
/*!
 * \brief Reads and validates the end_cell_defs block keyword.
 * \param meshfile Mesh file name.
 */
void CellDefs::readEndKeyword(ifstream & meshfile)
{
    string dummyString;

    meshfile >> dummyString;
    Insist(dummyString == "end_cell_defs",
	   "Invalid mesh file: cell_defs block missing end");
    std::getline(meshfile, dummyString);       // read and discard blank line.
}
/*!
 * \brief Changes the cell definitions specified in the RTT_Format file
 *        to an alternative cell definition (e.g., CYGNUS).
 * \param cell_side_types New side types for each of the existing cell 
 *        definitions.
 * \param cell_ordered_sides New ordered sides for each of the existing cell 
 *        definitions.
 */
void CellDefs::redefineCellDefs(const vector_vector_int & cell_side_types, 
		      const vector_vector_vector_int & cell_ordered_sides)
{
    Insist(cell_side_types.size() == dims.get_ncell_defs(),
	   "Error in supplied cell redefinition side types data.");
    Insist(cell_ordered_sides.size() == dims.get_ncell_defs(),
	   "Error in supplied cell redefinition ordered side data.");

    redefined = true;

    for (int cd = 0; cd < dims.get_ncell_defs(); cd ++)
        defs[cd]->redefineCellDef(cell_side_types[cd], cell_ordered_sides[cd]);
}
/*!
 * \brief Used by the CellDefs class objects to parse the number of nodes and
 *        sides per cell, the side type indices, and the nodes for each side.
 * \param meshfile Mesh file name.
 */
void CellDef::readDef(ifstream & meshfile)
{
    string dummyString;

    meshfile >> nnodes >> nsides;
    side_types.resize(nsides);
    sides.resize(nsides);
    ordered_sides.resize(nsides);
    std::getline(meshfile, dummyString);

    for (int i = 0; i < nsides; ++i)
    {
	meshfile >> side_types[i];
	--side_types[i];
    }
    if (nsides > 0)
	std::getline(meshfile, dummyString);

    // note that this implementation does not preserve the "right hand rule"
    // of the cell definitions due to the use of a set container (which is 
    // sorted). It is slicker than snail snot when it comes time to implement 
    // the connectivity, however. The ordered_sides vector was added to allow
    // the original ordered data to be retained.
    int side;
    for (int i = 0; i < nsides; ++i)
    {
        int numb_nodes = cellDefs.get_cell_def(side_types[i]).get_nnodes();
        ordered_sides[i].resize(numb_nodes);
	for (int j = 0; j < numb_nodes; ++j)
	{
	    meshfile >> side;
	    --side;
	    sides[i].insert(side);
	    ordered_sides[i][j] = side;
	}
	if (sides[i].size() > 0)
	    std::getline(meshfile, dummyString);
    }
}
/*!
 * \brief Changes the cell definitions specified in the RTT_Format file
 *        to an alternative cell definition (e.g., CYGNUS).
 * \param new_side_types New cell side types.
 * \param new_side_types New cell ordered sides.
 */
void CellDef::redefineCellDef(const vector_int & new_side_types, 
			      const vector_vector_int & new_ordered_sides)
{
    Insist(new_side_types.size() == nsides, "New side types input error");
    Insist(new_ordered_sides.size() == nsides,
	   "New ordered sides input error");

    node_map.resize(nnodes);

    if (name == "point")
    {
        node_map[0] = 0;
    }
    else if (name == "line")
    {
        node_map[ordered_sides[0][0]] = new_ordered_sides[0][0];
        node_map[ordered_sides[1][0]] = new_ordered_sides[1][0];
    }
    else if (name == "triangle" || name == "quad")
    {
        // Arbitrarily assign the first node in the old and the new cell 
        // definitions to be the same. This assumption is necessary because
        // the cell definitions do not assume a specific orientation relative
        // to any coordinate system. The transformed cell may be rotated 
        // about it's outward normal relative to the input cell definition.
        node_map[0] = 0;
	// The right hand rule has to apply, so only the ordering of the
        // nodes (edges) can change for a two-dimensional cell.
	int old_node = 0;
	int new_node = 0;
        for (int n = 0; n < nnodes - 1; n++)
	{
	    // Find the new side that starts with this node.
	    int new_side = 0;
	    while (new_ordered_sides[new_side][0] != new_node)
	    {
	        ++new_side;
		Insist(new_side < nsides, 
		       "Edge error for new two dimensional cell definition.");
	    }
	    new_node = new_ordered_sides[new_side][1];
	    // Find the old side that starts with this node.
	    int old_side = 0;
	    while (ordered_sides[old_side][0] != old_node)
	    {
	        ++old_side;
		Insist(old_side < nsides, 
		       "Edge error for old two dimensional cell definition.");
	    }
	    old_node = ordered_sides[old_side][1];
	    node_map[old_node] = new_node;
	}
    }
    else if (name == "tetrahedron")
    {
        // Arbitrarily assign the first node in the old and the new cell 
        // definitions to be the same. This assumption is necessary because
        // the cell definitions do not assume a specific orientation relative
        // to any coordinate system. The transformed cell may be rotated 
        // about the outward normal of the opposite face relative to the 
        // input cell definition.
        node_map[0] = 0;
	// Find the one side definition that does not contain the first node.
	int new_side = 0;
	while (std::count(new_ordered_sides[new_side].begin(), 
			  new_ordered_sides[new_side].end(), node_map[0]) > 0)
	{
	    ++new_side;
	    Insist(new_side < nsides, 
		   "Side error for new tetrahedron cell definition.");
	}
 	// Find the one side definition that does not contain the first node.
	int old_side = 0;
	while (std::count(ordered_sides[old_side].begin(), 
			  ordered_sides[old_side].end(), node_map[0]) > 0)
	{
	    ++old_side;
	    Insist(old_side < nsides, 
		   "Side error for old tetrahedron cell definition.");
	}
	// Now just apply the right-hand rule.
	for (int n = 0; n < ordered_sides[old_side].size(); n++)
	    node_map[ordered_sides[old_side][n]] = 
	        new_ordered_sides[new_side][n];
    }
    else if (name == "quad_pyr")
    {
        // Find the side that is the quad. The transformed cell may be rotated 
        // about the outward normal of this face relative to the input cell 
        // definition.
        int new_side = 0;
	while (new_ordered_sides[new_side].size() != 4)
	{
	    ++new_side;
	    Insist(new_side < nsides, 
		   "Quad side error for new quad pyramid cell definition.");
	}
        int old_side = 0;
	while (ordered_sides[old_side].size() != 4)
	{
	    ++old_side;
	    Insist(old_side < nsides, 
		   "Quad side error for old quad pyramid cell definition.");
	}
	// Find the single node that is not included in the quad side 
	// definition and assign this to the node map.
	int new_node = 0;
	int old_node = 0;
	for (int n = 0; n < nnodes; n++)
	{
	    if (std::count(new_ordered_sides[new_side].begin(), 
			   new_ordered_sides[new_side].end(), n) == 0)
	        new_node = n;
	    if (std::count(ordered_sides[old_side].begin(), 
			   ordered_sides[old_side].end(), n) == 0)
	        old_node = n;
	}
	node_map[old_node] = new_node;
	// Now just apply the right-hand rule to the quad side.
	for (int n = 0; n < ordered_sides[old_side].size(); n++)
	    node_map[ordered_sides[old_side][n]] = 
	        new_ordered_sides[new_side][n];
    }
    else if (name == "tri_prism")
    {
	// Find the one quad side definition that does not contain the first 
	// node. The transformed cell may be rotated about the outward normal
        // of this face relative to the input cell definition.
	int new_quad = 0;
	while (new_ordered_sides[new_quad].size() != 4 ||
	       std::count(new_ordered_sides[new_quad].begin(), 
			  new_ordered_sides[new_quad].end(), 0) > 0)
	{
	    ++new_quad;
	    Insist(new_quad < nsides, 
		   "Quad side error for new tri-prism cell definition.");
	}
	// Find the one quad side definition that does not contain the first 
	// node.
	int old_quad = 0;
	while (ordered_sides[old_quad].size() != 4 ||
	       std::count(ordered_sides[old_quad].begin(), 
			  ordered_sides[old_quad].end(), 0) > 0)
	{
	    ++old_quad;
	    Insist(old_quad < nsides, 
		   "Quad side error for old tri-prism cell definition.");
	}
	// Apply the right-hand rule to this quad.
	for (int n = 0; n < ordered_sides[old_quad].size(); n++)
	    node_map[ordered_sides[old_quad][n]] = 
	        new_ordered_sides[new_quad][n];
	// Equate the two remaining triangle nodes. Find the first node.
	int old_tri = 0;
	while (ordered_sides[old_tri].size() != 3 ||
	       std::count(ordered_sides[old_tri].begin(), 
			  ordered_sides[old_tri].end(), 0) > 0)
	{
	    ++old_tri;
	    Insist(old_tri < nsides, 
		   "Triangle side error for old tri-prism cell definition.");
	}
	// Find out which of the two triangles this is by identifying one
	// of the nodes common to both the triangle and the previous quad.
	int old_node = 0;
	while (std::count(ordered_sides[old_quad].begin(), 
			  ordered_sides[old_quad].end(),
			  ordered_sides[old_tri][old_node]) == 0)
	{
	    ++old_node;
	    Insist(old_node < ordered_sides[old_tri].size(), 
		   "Node error for old tri-prism cell definition.");
	}
	int new_tri = 0;
	while (new_ordered_sides[new_tri].size() != 3 ||
	       std::count(new_ordered_sides[new_tri].begin(), 
			  new_ordered_sides[new_tri].end(),
			  node_map[ordered_sides[old_tri][old_node]]) == 0)
	{
	    ++new_tri;
	    Insist(new_tri < nsides, 
		   "Triangle side error for new tri-prism cell definition.");
	}
	--old_node;
	int new_node = 0;
	while (std::count(new_ordered_sides[new_quad].begin(), 
			  new_ordered_sides[new_quad].end(),
			  new_ordered_sides[new_tri][new_node]) != 0)
	{
	    ++new_node;
	    Insist(new_node <  new_ordered_sides[new_tri].size(), 
		   "Node error for new tri-prism cell definition.");
	}
	node_map[ordered_sides[old_tri][old_node]] = 
	    new_ordered_sides[new_tri][new_node];
	// The node that is neither in the previous quad or triangle is all
	// that is left.
	for (int n = 0; n < nnodes; n++)
	{
	    if (std::count(new_ordered_sides[new_quad].begin(), 
			   new_ordered_sides[new_quad].end(), n) == 0 &&
		std::count(new_ordered_sides[new_tri].begin(), 
			   new_ordered_sides[new_tri].end(), n) == 0)
	        new_node = n;
	    if (std::count(ordered_sides[old_quad].begin(), 
			   ordered_sides[old_quad].end(), n) == 0 &&
		std::count(ordered_sides[old_tri].begin(), 
			   ordered_sides[old_tri].end(), n) == 0)
	        old_node = n;		
	}	    
	node_map[old_node] = new_node;
    }
    else if (name == "hexahedron")
    {
        // Arbitrarily assign the first quad and the associated nodes in the 
        // old and the new cell definitions to be the same. This assumption is
        // necessary because the cell definitions do not assume a specific 
        // orientation relative to any coordinate system. The transformed cell
        // may be rotated about it's coordinate system relative to the input 
        // cell definition.
        int quad = 0;
	vector_int new_node_count(nnodes,0);
	vector_int old_node_count(nnodes,0);
	for (int n = 0; n < ordered_sides[quad].size(); n++)
	{
	    int new_node = new_ordered_sides[quad][n];
	    int old_node = ordered_sides[quad][n];
	    node_map[old_node] = new_node;
	    for (int s = 0; s < nsides; s++)
	    {
	        if (std::count(new_ordered_sides[s].begin(), 
			       new_ordered_sides[s].end(), new_node) > 0)
		    for (int c = 0; c < new_ordered_sides[s].size(); c++)
		        ++new_node_count[new_ordered_sides[s][c]];
		if (std::count(ordered_sides[s].begin(), 
			       ordered_sides[s].end(), old_node) > 0)
		    for (int c = 0; c < ordered_sides[s].size(); c++)
		        ++old_node_count[ordered_sides[s][c]];
	    }
	    // The node located diagonally across the hexahedron relative to
	    // the first node will have a count of zero from the previous loop.
	    for (int c = 0; c < nnodes; c++)
	    {
	        if (new_node_count[c] == 0)
		    new_node = c;
		if (old_node_count[c] == 0)
		    old_node = c;
	    }
	    node_map[old_node] = new_node;
	    std::fill(new_node_count.begin(), new_node_count.end(), 0);
	    std::fill(old_node_count.begin(), old_node_count.end(), 0);
	}
    }
    // Assign the new side types and ordered sides to this cell definition.
    side_types = new_side_types;
    ordered_sides = new_ordered_sides;
}

} // end namespace rtt_RTT_Format_Reader

//---------------------------------------------------------------------------//
//                    end of RTT_Format_Reader/CellDefs.cc
//---------------------------------------------------------------------------//
