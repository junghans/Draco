//----------------------------------*-C++-*----------------------------------//
/*! 
 * \file   amr_mesh/Release.hh
 * \author B.T. Adams
 * \date   Fri Aug 27 10:33:26 1999
 * \brief  Header file for amr_mesh library release function.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __amr_Release_hh__
#define __amr_Release_hh__

//===========================================================================//
// namespace version - 
//
// Purpose : Return the version of amr; 
// this can be used to get exact version information in codes that 
// use amr
// 
//===========================================================================//

#include <string>

/*!
 * \brief RTT adaptive mesh refinement (amr) namespace.
 *
 * Provides namespace protection for the Draco (RTT) adaptive mesh refinement
 *  utilities.
 *\sa The CAR_CU_Mesh class and its associated interface and builder classes 
 *    provide most of the functionality of the namespace. The utilities of the
 *    RTT_Format namespace are incorporated to provide parsing capability for
 *    mesh files in the \ref rtt_format_defined. The \ref amr_overview page 
 *    presents a summary of the capabilities provided within the namespace.
 */
namespace rtt_amr 
{
/*!
 * \brief  Gets the release number for the amr_mesh package. 
 * \return release number as a string in the form "amr-\#_\#_\#"
 */
    const std::string release();
}

#endif                          // __amr_Release_hh__


/*!
 * \page amr_overview Overview of the Adaptive Mesh Refinement (AMR) package
 *
 * \version 1_0_0
 *
 * <h3> Introduction </h3>
 * The amr_mesh package consists of numerous functions that allow the creation
 * and manipulation of continous adaptive refinement (CAR) cartesian 
 * unstructured (CU) mesh class objects for use within transport codes. 
 * Although the class is generated in C++, a shadow object interface is
 * provided so that Fortran 90 (F90) codes may also use this data structure.
 * The AMR mesh package has been used in conjunction with both the C++ milagro
 * (implicit monte carlo) code and the F90 zatharus/centuari (P1 Diffusion 
 * with variable eddington factor correction) code. The rtt_amr::CAR_CU_Mesh 
 * class contains several nested  mesh field member classes including:
 * <ul>
 *  <li> Cell-Centered Scalar Field (rtt_amr::CAR_CU_Mesh::CCSF)
 *  <li> Cell-Centered Vector Field (rtt_amr::CAR_CU_Mesh::CCVF)
 *  <li> Face-Centered Scalar Field (rtt_amr::CAR_CU_Mesh::FCSF)
 *  <li> Face-Centered Discontinuous Scalar Field (rtt_amr::CAR_CU_Mesh::FCDSF)
 *  <li> Face-Centered Vector Field (rtt_amr::CAR_CU_Mesh::FCVF)
 *  <li> Node-Centered Scalar Field (rtt_amr::CAR_CU_Mesh::NCSF)
 *  <li> Node-Centered Vector Field (rtt_amr::CAR_CU_Mesh::NCVF).
 * </ul> 
 *
 * <h3> Intended Usage </h3>
 * The rtt_amr::CAR_CU_Mesh class is patterned after the Draco orthogonal 
 * structured mesh (OS_Mesh) class and thus functions similarly. The intended
 * usage is  as follows:
 *
 * 1) An rtt_amr::CAR_CU_Interface class object is instantiated by the main
 *    program. The interface class contains member functions to parse both the
 *    user's input file and a continuous adaptive refinement mesh file that
 *    uses the \ref rtt_format_defined (the latter file can be generated by 
 *    the mesh generation codes ICEM and X3D). An rtt_format::RTT_Format class
 *    object is instantiated and returned by the parsing member function and 
 *    actually reads the designated mesh file. The RTT_Format class is 
 *    equipted with numerous member functions that allow accessing the mesh
 *    data and this object also automatically determines the connectivity of 
 *    the mesh nodes, sides, and cells. In C++ the CAR_CU_Interface parsing 
 *    member functions must be invoked directly within the calling program, 
 *    while these functions are invoked automatically by the shadow object 
 *    interface of the CAR_CU_Interface class constructor when it is called 
 *    from a Fortran 90 code. The CAR_CU_Interface data accessor functions are
 *    also shadowed so that the remainder of the input data is available to 
 *    the calling program. A \ref rtt_amr_input is also provided.
 *
 * 2) An rtt_amr::CAR_CU_Builder class object is instantiated by the main
 *    program. The mesh builder contains the build_Mesh public member function
 *    that constructs the CAR_CU_Mesh class object (and its contained 
 *    Coord_Sys and Layout class objects). This member function takes a smart
 *    pointer to a rtt_format::RTT_Format class object as it single input 
 *    argument and returns a smart pointer to the new rtt_amr::CAR_CU_Mesh 
 *    class object. In C++ this member function must be invoked directly 
 *    within the calling program, while this function is invoked automatically
 *    by the shadow object interface of the CAR_CU_Builder class constructor 
 *    when it is called from a Fortran code. Only the class constructor and 
 *    destructor are shadowed for the CAR_CU_Builder.
 *
 * 3) A smart pointer to the rtt_amr::CAR_CU_Interface class object is the 
 *    single argument to the rtt_imc::Opacity_Builder constructor, which 
 *    constructs an Opacity class object and returns its address. While these
 *    class objects are actually in the Draco implicit monte carlo (imc) 
 *    namespace, a shadow object interface for these C++ class objects is 
 *    provided in the AMR mesh package so that Fortran 90 codes can construct
 *    and access the required Opacity data structure using the available data
 *    in the CAR_CU_Interface class object.
 *
 * 4) A smart pointer to a rtt_amr::CAR_CU_Mesh class object is the single 
 *    argument to the build_Mat public member function of the 
 *    rtt_imc::Opacity_Builder, which builds an rtt_imc::Mat_State class 
 *    object and returns a smart pointer to the new Mat_State class object. 
 *    While the Mat_State class object is actually in the Draco implicit monte
 *    carlo (imc) namespace, a shadow object interface for this C++ class 
 *    object is provided in the AMR mesh package so that Fortran 90 codes can
 *    construct and access the required Mat_State data structure using the 
 *    available data in the CAR_CU_Mesh class object.
 *
 * 5) The rtt_amr::CAR_CU_Interface and rtt_amr::CAR_CU_Builder class objects
 *    may be eliminated by calling their associated destructors after the
 *    rtt_amr::CAR_CU_Mesh, rtt_imc::Opacity, rtt_imc::and Mat_State class 
 *    objects have been constructed (assuming that any needed data in the 
 *    remainder of the CAR_CU_Interface class object has been retained in
 *    alternate data structures).
 *
 * 6) The rtt_amr::CAR_CU_Mesh class contains the required public member
 *    functions that are required to perform either a monte carlo or discrete 
 *    ordinates transport calulation. A shadow object interface exists for the
 *    majority of these functions (some of the member functions specific to
 *    monte carlo are not shadowed) so that they can be utilized within a
 *    Fortran 90 code. Mesh field classes can also be constructed and accessed
 *    at will using either programming language because these member functions
 *    are also shadowed. 
 *
 * <h3> Other Draco Packages </h3>
 * The AMR mesh package uses the Draco ds++ services library, the C4 
 * communication library, the implicit monte carlo (imc) library, and the
 * mesh class (mc) library.
 */

//---------------------------------------------------------------------------//
//                              end of Release.hh
//---------------------------------------------------------------------------//
