//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   mesh/test/Tester.hh
 * \author Randy M. Roberts
 * \date   Mon Nov 22 11:02:57 1999
 * \brief  
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __mesh_test_Tester_hh__
#define __mesh_test_Tester_hh__

#include "meshTest/Release.hh"
#include "../Release.hh"

#include "nml/Group.hh"
#include "c4/global.hh"

#include <iostream>

namespace rtt_mesh_test
{
 
//===========================================================================//
/*!
 * \class Tester
 *
 */
// revision history:
// -----------------
// 0) original
// 
//===========================================================================//

template<class FACTORY>
class Tester 
{

    // NESTED CLASSES AND TYPEDEFS

    // DATA

    FACTORY *factory_m;
    
    int &argc_m;
    char **argv_m;
    
  public:

    // CREATORS
    
    Tester(const std::string &filename, int &argc, char **argv)
	: argc_m(argc), argv_m(argv)
    {
	std::cout << "Calling C4::Init(...)" << std::endl;
	
	C4::Init( argc_m, argv_m );

	factory_m = 0;
	
 	try
	{
	    NML_Group g( "test" );
	    rtt_mesh::Mesh_DB mdb;
	    mdb.setup_namelist( g );
	    g.readgroup( filename.c_str() );

	    factory_m = new FACTORY(mdb);
	    std::cout << "Factory created in Tester" << std::endl;
	}
	catch( rtt_dsxx::assertion& a )
	{
	    std::cout << "Factory not created in Tester" << std::endl;
	    std::cout << "Failed assertion: " << a.what() << std::endl;
	    throw;
	}
    }

    virtual ~Tester()
    {
	delete factory_m;

	std::cout << "Calling C4::Finalize()" << std::endl;
	C4::Finalize();
    }

    // MANIPULATORS
    
    template<class TEST>
    inline void run(const std::string &filename);

    template<class TEST,class T>
    inline void run(const std::string &filename);
    
    FACTORY &factory() { return *factory_m; }

    // ACCESSORS

    const FACTORY &factory() const { return *factory_m; }

  private:

    // IMPLEMENTATION

    void version(const std::string &progname) const
    {
	std::cout << progname << ": version "
		  << rtt_mesh::release() << std::endl;
	std::cout << "meshTest: version "
		  << rtt_meshTest::release() << std::endl;
    }

    bool checkForVersion() const
    {
	for (int arg=1; arg < argc_m; arg++)
	{
	    if (std::string(argv_m[arg]) == "--version")
	    {
		version(argv_m[0]);
		return true;
	    }
	}
	return false;
    }

    void printStatus(const std::string &name, bool passed) const
    {
	std::string stars;
	for (int i=0; i<name.length(); i++)
	    stars += '*';
    
	std::cout << std::endl;
	std::cout << "*****" << stars << "******************" << std::endl;
	if (passed) 
	{
	    std::cout << "**** " << name << " Test: PASSED ****" << std::endl;
	}
	else
	{
	    std::cout << "**** " << name << " Test: FAILED ****" << std::endl;
	}

	std::cout << "*****" << stars << "******************" << std::endl;
	std::cout << std::endl;
    }
};

template <class FACTORY>
template <class TEST>
void Tester<FACTORY>::run(const std::string &name)
{
    if (checkForVersion())
	return;
    
    bool passed = false;
    
    try
    {
	std::cout << "Initiating " << name << " test"
		  << std::endl;

	TEST tester(factory(), std::cout);
	tester.run();

	passed = tester.passed();
    }
    catch( rtt_dsxx::assertion& a )
    {
	std::cout << "Failed assertion: " << a.what() << std::endl;
    }

    // Print the status of the test.

    printStatus(name, passed);
}

template <class FACTORY>
template<class TEST, class T>
void Tester<FACTORY>::run(const std::string &name)
{
    if (checkForVersion())
	return;
    
    bool passed = false;
    
    try {
	std::cout << "Initiating " << name << " test"
		  << std::endl;

	TEST tester(factory(), std::cout);
	tester.run<T>();

	passed = tester.passed();
    }
    catch( rtt_dsxx::assertion& a )
    {
	std::cout << "Failed assertion: " << a.what() << std::endl;
    }

    // Print the status of the test.

    printStatus(name, passed);
}

} // end namespace rtt_mesh_test

#endif                          // __mesh_test_Tester_hh__

//---------------------------------------------------------------------------//
//                              end of mesh/test/Tester.hh
//---------------------------------------------------------------------------//
