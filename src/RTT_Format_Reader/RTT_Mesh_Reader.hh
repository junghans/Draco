//----------------------------------*-C++-*----------------------------------//
// RTT_Mesh_Reader.hh
// B.T. Adams
// 7 June 00
/*! 
 * \file   RTT_Format_Reader/RTT_Mesh_Reader.hh
 * \author B.T. Adams
 * \date   Wed Jun 7 10:33:26 2000
 * \brief  Header file for RTT_Mesh_Reader library.
 */
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __RTT_Format_Reader_RTT_Mesh_Reader_hh__
#define __RTT_Format_Reader_RTT_Mesh_Reader_hh__

#include "RTT_Format_Reader.hh"
#include "meshReaders/Mesh_Reader.hh"

namespace rtt_RTT_Format_Reader
{
//===========================================================================//
// class RTT_Mesh_Reader - 
//
/*!
 * \brief An input routine to parse an RTT Format mesh file using the DRACO
 *        meshReaders standard interface.
 *
 *\sa The RTT_Mesh_Reader class is a derived-type of the Mesh_Reader abstract 
 *    base class specified in the meshReaders package. Packages using the 
 *    RTT_Mesh_Reader should thus include the RTT_Mesh_Reader.hh decorator
 *    file located in the meshReaders directory to resolve the namespace. 
 *    RTT_Mesh_Reader contains a RTT_Format_Reader as a private data member, 
 *    so none of the RTT_Format_Reader class public accessor functions are 
 *    accessible.
 */ 
//
// revision history:
// -----------------
// 0) original
// 
//===========================================================================//

class RTT_Mesh_Reader : public rtt_meshReaders::Mesh_Reader
{
    // NESTED CLASSES AND TYPEDEFS
    typedef std::string string;
    typedef std::set<int> set_int;
    typedef std::vector<int> vector_int;
    typedef std::vector<std::vector<int> > vector_vector_int;
    typedef std::vector<std::vector<std::vector<int> > > 
        vector_vector_vector_int;
    typedef std::vector<std::vector<double> > vector_vector_dbl;

    // DATA

  private:
     rtt_dsxx::SP<RTT_Format_Reader> rttMesh;
     std::vector<rtt_meshReaders::Element_Definition::Element_Type> 
         element_types;
     std::vector<rtt_meshReaders::Element_Definition::Element_Type> 
         unique_element_types;

  public:

    // Constructors
/*!
 * \brief Constructs an RTT_Mesh_Reader class object.
 * \param RTT_File Mesh file name.
 */
    RTT_Mesh_Reader(const string & RTT_File)
        : rttMesh(new RTT_Format_Reader(RTT_File))
    {
        transform2CGNS();
    }
/*!
 * \brief Destroys an RTT_Mesh_Reader class object
 */
    // Destructors
    ~RTT_Mesh_Reader() {}

    // ACCESSORS
    // Virutal accessor function definitions based on the Mesh_Readers
    // abstract base class.
/*!
 * \brief Returns the coordinate values for each of the nodes.
 * \return The coordinate values for the nodes.
 */
    virtual vector_vector_dbl get_node_coords() const
    { return rttMesh->get_nodes_coords(); }
/*!
 * \brief Returns the problem coordinate units (e.g, cm).
 * \return Coordinate units.
 */
    virtual string get_node_coord_units() const
    { return rttMesh->get_dims_coor_units(); }

    virtual vector_vector_int get_element_nodes() const;
/*!
 * \brief Returns the element (i.e., sides and cells) types (e.g., TRI_3 and
 *        TETRA_4).
 * \return Element definitions.
 */
    virtual std::vector<rtt_meshReaders::Element_Definition::Element_Type> 
        get_element_types() const
    { return element_types; }

/*!
 * \brief Returns the unique element types (e.g., TRI_3 and TETRA_4) that 
 *        are defined in the mesh file.
 * \return Element definitions.
 */
    virtual std::vector<rtt_meshReaders::Element_Definition::Element_Type> 
        get_unique_element_types() const
    { return unique_element_types; }


    virtual std::map<string, set_int > get_node_sets() const;

    virtual std::map<string, set_int > get_element_sets() const;
/*!
 * \brief Returns the mesh file title.
 * \return Mesh file title.
 */
    virtual string get_title() const { return rttMesh->get_header_title(); }

    virtual bool invariant() const;

    // IMPLEMENTATION
  private:
    
    void transform2CGNS();
};

} // end namespace rtt_RTT_Format_Reader

#endif                          // __RTT_Format_Reader_RTT_Mesh_Reader_hh__

//---------------------------------------------------------------------------//
//                  end of RTT_Format_Reader/RTT_Mesh_Reader.hh
//---------------------------------------------------------------------------//
