//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   meshTest/TestMTComm.hh
 * \author Randy M. Roberts
 * \date   Mon Aug 23 15:09:07 1999
 * \brief  Header file for the TestMTComm class.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __meshTest_TestMTComm_hh__
#define __meshTest_TestMTComm_hh__

namespace rtt_meshTest
{
 
//===========================================================================//
/*!
 * \class TestMTComm
 *
 * \brief This class tests the communications part of a given MT for
 * the "Solon" MT concept.
 *
 * The TestMTComm class is templated on an MTFactory.
 * The MTFactory is used to create coupled instances of meshes and field
 * constructors, along with typedefs that determine whether the mesh is
 * structured or unstructured.
 *
 * Required MTFactory services include:
 *
 * MTFactory::MT -- The mesh type
 *
 * MTFactory::FieldConstructor -- The field constructor type.
 *
 * MTFactory::Product -- A class that is returned by the MTFactroy::create()
 * method.  This class is responsible for returning references to the mesh
 * and field constructor via the mesh() and fieldConstructor() methods,
 * respectively.
 *
 * MTFactory::Structured -- A tag that, if used, implies that the mesh
 * is a structured mesh.
 *
 * MTFactory::UnStructured -- A tag that, if used, implies that the mesh
 * is an unstructured mesh.
 *
 * MTFactory::Structuring -- A typedef to either, Structured, or UnStructured,
 * used to determine whether the mesh is actually one or the other.
 *
 * MTFactory::Product MTFactory::create() -- The method that returns a new
 * pair of meshes and field constructors.
 */
//
// revision history:
// -----------------
// 0) original
// 
//===========================================================================//

template<class MTFactory>
class TestMTComm 
{

    // NESTED CLASSES AND TYPEDEFS

    typedef typename MTFactory::MT MT;
    typedef typename MTFactory::Product MTFactoryProduct;
    typedef typename MT::FieldConstructor FieldConstructor;

    // DATA
    
    MTFactory &meshFactory_m;
    
    std::ostream &os_m;
    
    bool passed_m;

  public:

    // CREATORS

    //! Constructor
    
    TestMTComm(MTFactory &meshFactory_in, std::ostream &os_in)
	: meshFactory_m(meshFactory_in), os_m(os_in),
	  passed_m(false)
    {
	/* empty */
    }
    
    //! Destructor
    
    ~TestMTComm() { /* empty */ }

    // MANIPULATORS
    
    //! Main interface to testing class.
    
    void run();

    // ACCESSORS

    //! Returns success of previously ran run() method.
    
    bool passed() const { return passed_m; }
    
  private:
    
    // DISSALLOWED CREATORS

    TestMTComm(const TestMTComm &rhs);

    // DISSALLOWED MANIPULATORS
    
    TestMTComm& operator=(const TestMTComm &rhs);

    // IMPLEMENTATION

    void t3();
    void t4();
};

} // end namespace rtt_meshTest

#endif                          // __meshTest_TestMTComm_hh__

//---------------------------------------------------------------------------//
//                              end of meshTest/TestMTComm.hh
//---------------------------------------------------------------------------//
