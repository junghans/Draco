//----------------------------------*-C++-*----------------------------------//
// Cells.hh
// B.T. Adams
// 7 June 00
/*! 
 * \file   RTT_Format_Reader/Cells.hh
 * \author B.T. Adams
 * \date   Wed Jun 7 10:33:26 2000
 * \brief  Header file for RTT_Format_Reader/Cells class.
 */
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __RTT_Format_Reader_Cells_hh__
#define __RTT_Format_Reader_Cells_hh__

#include "Dims.hh"
#include "CellFlags.hh"
#include "CellDefs.hh"
#include "Nodes.hh"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace rtt_RTT_Format_Reader
{
/*!
 * \brief Controls parsing, storing, and accessing the data specific to the 
 *        cells block of the mesh file.
 */
class Cells
{
    // typedefs
    typedef std::ifstream ifstream;
    typedef std::string string;
    typedef std::vector<int> vector_int;
    typedef std::vector<std::vector<int> > vector_vector_int;

    const CellFlags & cellFlags;
    const Dims & dims;
    const CellDefs & cellDefs;
    const Nodes & nodesClass;
    vector_int cellType;
    vector_vector_int nodes;
    vector_vector_int flags;

  public:
    Cells(const CellFlags & cellFlags_, const Dims & dims_,
	  const CellDefs & cellDefs_, const Nodes & nodesClass_) :
	 cellFlags(cellFlags_), dims(dims_), cellDefs(cellDefs_),
         nodesClass(nodesClass_), cellType(dims.get_ncells()),
         nodes(dims.get_ncells()), flags(dims.get_ncells(), 
         vector_int(dims.get_ncell_flag_types())) {}
    ~Cells() {}

    void readCells(ifstream & meshfile);
    void redefineCells();

  private:
    void readKeyword(ifstream & meshfile);
    void readData(ifstream & meshfile);
    void readEndKeyword(ifstream & meshfile);

  public:
/*!
 * \brief Returns the cell type associated with the specified cell.
 * \param cell_numb Cell number.
 * \return The cell type.
 */
    int get_type(int cell_numb) const { return cellType[cell_numb]; }
/*!
 * \brief Returns all of the node numbers for each of the cells.
 * \return The node numbers for all cells.
 */
    vector_vector_int get_nodes() const { return nodes; }
/*!
 * \brief Returns all of the node numbers associated with the specified cell.
 * \param cell_numb Cell number.
 * \return The cell node numbers.
 */
    vector_int get_nodes(int cell_numb) const { return nodes[cell_numb]; }
/*!
 * \brief Returns the node number associated with the specified cell and 
 *        cell-node index.
 * \param cell_numb Cell number.
 * \param node_numb Cell-node index number.
 * \return The cell node number.
 */
    int get_nodes(int cell_numb,int node_numb) const
    { return nodes[cell_numb][node_numb]; }
/*!
 * \brief Returns the cell flag for the specified cell and flag index
 * \param cell_numb Cell number.
 * \param flag_numb Cell flag index.
 * \return The cell flag.
 */
    int get_flags(int cell_numb,int flag_numb) const
    { return flags[cell_numb][flag_numb]; }
};

} // end namespace rtt_RTT_Format_Reader

#endif                          // __RTT_Format_Reader_Cells_hh__

//---------------------------------------------------------------------------//
//                       end of RTT_Format_Reader/Cells.hh
//---------------------------------------------------------------------------//
