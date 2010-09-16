//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_gandolf/GandolfMultigroupOpacity.cc
 * \author Kelly Thompson
 * \date   Mon Jan 22 15:24210 2001
 * \brief  GandolfMultigroupOpacity templated class implementation file.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "GandolfMultigroupOpacity.hh"

#include "GandolfWrapper.hh"    // we make calls to the wrapper routines.

#include "GandolfFile.hh"       // we have smart pointers to
                                // GandolfFile objects.

#include "GandolfException.hh"  // Since we call wrapper routines we
                                // need to be able to throw exceptions
                                // if the Gandolf libraries return an 
                                // error.

#include "GandolfDataTable.hh"  // we have a smart pointer to a
                                // GandolfDataTable object.

#include <cmath> // we need to define log(double) and exp(double)

#include "ds++/Assert.hh" // we make use of Require()

#include "ds++/Packing_Utils.hh"

namespace rtt_cdi_gandolf
{
    
// ------------ //
// Constructors //
// ------------ //
    
/*!
 * \brief Constructor for GandolfMultigroupOpacity object.
 * 
 * See GandolfMultigroupOpacity.hh for details.
 */
GandolfMultigroupOpacity::GandolfMultigroupOpacity( 
    const rtt_dsxx::SP< const GandolfFile >& in_spGandolfFile,
    size_t in_materialID,
    rtt_cdi::Model in_opacityModel,
    rtt_cdi::Reaction in_opacityReaction )
    : spGandolfFile( in_spGandolfFile ),
      materialID( in_materialID ),
      numKeys( 0 ),
      vKnownKeys(),
      opacityModel( in_opacityModel ),
      opacityReaction( in_opacityReaction ),
      energyPolicyDescriptor( "mg" ),
      spGandolfDataTable()
{
    // Verify that the requested material ID is available in the
    // specified IPCRESS file.
    if ( ! spGandolfFile->materialFound( materialID ) )
	throw gkeysException( -1 );
	    
    // Retrieve keys available fo this material from the IPCRESS
    // file.  wgkeys() returns vKnownKeys, numKeys and errorCode.
    int errorCode = 
	wrapper::wgkeys( spGandolfFile->getDataFilename(),
			 materialID, vKnownKeys,
			 wrapper::maxKeys, numKeys );
    if ( errorCode !=0 ) throw gkeysException( errorCode );
	    
    // Create the data table object and fill it with the table
    // data from the IPCRESS file.
    spGandolfDataTable = new GandolfDataTable(
	energyPolicyDescriptor, opacityModel, opacityReaction,
	vKnownKeys, materialID, spGandolfFile );
	    
} // end of GandolfData constructor
    
/*!
 * \brief Unpacking constructor for GandolfMultigroupOpacity object.
 * 
 * See GandolfMultigroupOpacity.hh for details.
 */
GandolfMultigroupOpacity::GandolfMultigroupOpacity(
    const std::vector<char> &packed)
    : spGandolfFile(),
      materialID(0),
      numKeys(0),
      vKnownKeys(),
      opacityModel(),
      opacityReaction(),
      energyPolicyDescriptor( "mg" ),
      spGandolfDataTable()
{
    Require (packed.size() >= 5 * sizeof(int));

    // make an unpacker
    rtt_dsxx::Unpacker unpacker;
    unpacker.set_buffer(packed.size(), &packed[0]);

    // unpack and check the descriptor
    int         packed_descriptor_size = 0;
    unpacker >> packed_descriptor_size;
    Check (packed_descriptor_size > 0);

    // make a vector<char> for the packed descriptor
    std::vector<char> packed_descriptor(packed_descriptor_size);

    // unpack it
    std::string descriptor;
    for (size_t i = 0; i < static_cast<size_t>(packed_descriptor_size); i++)
	unpacker >> packed_descriptor[i];
    rtt_dsxx::unpack_data(descriptor, packed_descriptor);

    // make sure it is "gray"
    Insist (descriptor == "mg", 
	    "Tried to unpack a non-mg opacity in GandolfMultigroupOpacity.");

    // unpack the size of the packed filename
    int packed_filename_size(0);
    unpacker >> packed_filename_size;

    // make a vector<char> for the packed filename
    std::vector<char> packed_filename(packed_filename_size);

    // unpack it
    std::string filename;
    for (size_t i = 0; i < static_cast<size_t>(packed_filename_size); i++)
	unpacker >> packed_filename[i];
    rtt_dsxx::unpack_data(filename, packed_filename);

    // unpack the material id
    // unpacker >> materialID;
    int itmp(0);
    unpacker >> itmp;
    materialID = static_cast<size_t>(itmp);

    // unpack the model and reaction
    int model    = 0;
    int reaction = 0;
    unpacker >> model >> reaction;

    opacityModel    = static_cast<rtt_cdi::Model>(model);
    opacityReaction = static_cast<rtt_cdi::Reaction>(reaction);

    Ensure (unpacker.get_ptr() == &packed[0] + packed.size());
    
    // build a new GandolfFile
    spGandolfFile = new GandolfFile(filename);
    Check (spGandolfFile);

    // Verify that the requested material ID is available in the
    // specified IPCRESS file.
    if ( ! spGandolfFile->materialFound( materialID ) )
	throw gkeysException( -1 );
	    
    // Retrieve keys available fo this material from the IPCRESS
    // file.  wgkeys() returns vKnownKeys, numKeys and errorCode.
    int errorCode = 
	wrapper::wgkeys( spGandolfFile->getDataFilename(),
			 materialID, vKnownKeys,
			 wrapper::maxKeys, numKeys );
    if ( errorCode !=0 ) throw gkeysException( errorCode );
	    
    // Create the data table object and fill it with the table
    // data from the IPCRESS file.
    spGandolfDataTable = new GandolfDataTable(
	energyPolicyDescriptor, opacityModel, opacityReaction,
	vKnownKeys, materialID, spGandolfFile );

    Ensure (spGandolfFile);
    Ensure (spGandolfDataTable);
} 
    
/*!
 * \brief Default GandolfOpacity() destructor.
 *
 * \sa This is required to correctly release memory when a 
 *     GandolfMultigroupOpacity is destroyed.  This constructor's
 *     * definition must be declared in the implementation file so
 *     that * we can avoid including too many header files
 */
GandolfMultigroupOpacity::~GandolfMultigroupOpacity()
{
    // empty
}
    
    
// --------- //
// Accessors //
// --------- //
    
/*!
 * \brief Returns a "plain English" description of the opacity
 *     data that this class references. (e.g. "Multigroup Rosseland
 *     Scattering".) 
 *
 * The definition of this function is not included here to prevent 
 *     the inclusion of the GandolfFile.hh definitions within this 
 *     header file.
 */
std::string GandolfMultigroupOpacity::getDataDescriptor() const 
{
    // call the correct function from the GandolfDataTable
    // object. 
    return spGandolfDataTable->getDataDescriptor(); 
}
    
/*!
 * \brief Returns the name of the associated IPCRESS file.
 *
 * The definition of this function is not included here to prevent 
 *     the inclusion of the GandolfFile.hh definitions within this 
 *     header file.
 */
std::string GandolfMultigroupOpacity::getDataFilename() const 
{
    return spGandolfFile->getDataFilename(); 
}
    
/*!
 * \brief Opacity accessor that returns a single opacity (or a
 *     vector of opacities for the multigroup EnergyPolicy) that 
 *     corresponds to the provided temperature and density.
 */
std::vector< double > GandolfMultigroupOpacity::getOpacity(
    double targetTemperature,
    double targetDensity ) const
{ 
    // number of groups in this multigroup set.
    const size_t numGroups = spGandolfDataTable->getNumGroupBoundaries() - 1;
	    
    // temporary opacity vector used by the wrapper.  The returned 
    // data will be copied into the opacityIterator.
    std::vector<double> opacity( numGroups );
	    
    // logarithmic interpolation:
    opacity = wrapper::wgintmglog( 
	spGandolfDataTable->getLogTemperatures(),
	spGandolfDataTable->getNumTemperatures(), 
	spGandolfDataTable->getLogDensities(), 
	spGandolfDataTable->getNumDensities(),
	spGandolfDataTable->getNumGroupBoundaries(),
	spGandolfDataTable->getLogOpacities(), 
	spGandolfDataTable->getNumOpacities(),
	std::log( targetTemperature ),
	std::log( targetDensity ) );
    return opacity;
}
    
/*!
 * \brief Opacity accessor that returns a vector of opacities (or a
 *     vector of vectors of opacities for the multigroup
 *     EnergyPolicy) that correspond to the provided vector of
 *     temperatures and a single density value.
 */
std::vector< std::vector< double > > GandolfMultigroupOpacity::getOpacity(
    const std::vector<double>& targetTemperature,
    double targetDensity ) const
{ 
    size_t numGroups = spGandolfDataTable->getNumGroupBoundaries() - 1;
    std::vector< std::vector< double > > opacity( targetTemperature.size() );
    for ( size_t i=0; i<targetTemperature.size(); ++i )
    {
	opacity[i].resize( numGroups );
	// logorithmic interpolation:
	opacity[i] = wrapper::wgintmglog( 
	    spGandolfDataTable->getLogTemperatures(),
	    spGandolfDataTable->getNumTemperatures(), 
	    spGandolfDataTable->getLogDensities(), 
	    spGandolfDataTable->getNumDensities(),
	    spGandolfDataTable->getNumGroupBoundaries(),
	    spGandolfDataTable->getLogOpacities(), 
	    spGandolfDataTable->getNumOpacities(),
	    std::log( targetTemperature[i] ),
	    std::log( targetDensity ) );
    }
    return opacity;
}
    
/*!
 * \brief Opacity accessor that returns a vector of opacities (or a
 *     vector of vectors of opacities for the multigroup
 *     EnergyPolicy) that correspond to the provided vector of
 *     densities and a single temperature value.
 */
std::vector< std::vector< double > > GandolfMultigroupOpacity::getOpacity(
    double targetTemperature,
    const std::vector<double>& targetDensity ) const
{ 
    size_t numGroups = spGandolfDataTable->getNumGroupBoundaries() - 1;
    std::vector< std::vector< double > > opacity( targetDensity.size() );
    for ( size_t i=0; i<targetDensity.size(); ++i )
    {
	opacity[i].resize(numGroups);
	// logorithmic interpolation:
	opacity[i] = wrapper::wgintmglog( 
	    spGandolfDataTable->getLogTemperatures(),
	    spGandolfDataTable->getNumTemperatures(), 
	    spGandolfDataTable->getLogDensities(), 
	    spGandolfDataTable->getNumDensities(),
	    spGandolfDataTable->getNumGroupBoundaries(),
	    spGandolfDataTable->getLogOpacities(), 
	    spGandolfDataTable->getNumOpacities(),
	    std::log( targetTemperature ),
	    std::log( targetDensity[i] ) );
    }
    return opacity;
}
    
/*!
 * \brief Returns a vector of temperatures that define the cached
 *     opacity data table.
 */
std::vector< double > GandolfMultigroupOpacity::getTemperatureGrid() const
{
    return spGandolfDataTable->getTemperatures();
}
    
/*!
 * \brief Returns the size of the temperature grid.
 */
size_t GandolfMultigroupOpacity::getNumTemperatures() const
{
    return spGandolfDataTable->getNumTemperatures();
}
    
/*!
 * \brief Returns a vector of densities that define the cached
 *     opacity data table.
 */
std::vector<double> GandolfMultigroupOpacity::getDensityGrid() const
{
    return spGandolfDataTable->getDensities();
}
    
/*! 
 * \brief Returns the size of the density grid.
 */
size_t GandolfMultigroupOpacity::getNumDensities() const
{
    return spGandolfDataTable->getNumDensities();
}
    
/*!
 * \brief Returns a vector of energy values (keV) that define the
 *     energy boundaries of the cached multigroup opacity data
 *     table.  (This accessor is only valid for the Multigroup
 *     EnergyPolicy version of GandolfMultigroupOpacity.)
 */
std::vector< double > GandolfMultigroupOpacity::getGroupBoundaries() const
{
    return spGandolfDataTable->getGroupBoundaries();
}
    
/*!
 * \brief Returns the number of group boundaries found in the
 *     current multigroup data set.
 */
size_t GandolfMultigroupOpacity::getNumGroupBoundaries() const
{
    return spGandolfDataTable->getNumGroupBoundaries();
}

// ------- //
// Packing //
// ------- //

/*!
 * Pack the GandolfMultigroupOpacity state into a char string represented by
 * a vector<char>. This can be used for persistence, communication, etc. by
 * accessing the char * under the vector (required by implication by the
 * standard) with the syntax &char_string[0]. Note, it is unsafe to use
 * iterators because they are \b not required to be char *.
 */
std::vector<char> GandolfMultigroupOpacity::pack() const
{
    using std::vector;
    using std::string;

    Require (spGandolfFile);

    // pack up the energy policy descriptor
    vector<char> packed_descriptor;
    rtt_dsxx::pack_data(energyPolicyDescriptor, packed_descriptor);

    // pack up the ipcress file name
    string       filename = spGandolfFile->getDataFilename();
    vector<char> packed_filename;
    rtt_dsxx::pack_data(filename, packed_filename);

    // determine the total size: 3 ints (reaction, model, material id) + 2
    // ints for packed_filename size and packed_descriptor size + char in
    // packed_filename and packed_descriptor
    size_t size = 5 * sizeof(int) + packed_filename.size() + 
	packed_descriptor.size();

    // make a container to hold packed data
    vector<char> packed(size);

    // make a packer and set it
    rtt_dsxx::Packer packer;
    packer.set_buffer(size, &packed[0]);

    // pack the descriptor
    packer << static_cast<int>(packed_descriptor.size());
    for (size_t i = 0; i < packed_descriptor.size(); i++)
	packer << packed_descriptor[i];

    // pack the filename (size and elements)
    packer << static_cast<int>(packed_filename.size());
    for (size_t i = 0; i < packed_filename.size(); i++)
	packer << packed_filename[i];

    // pack the material id
    packer << static_cast<int>(materialID);

    // pack the model and reaction
    packer << static_cast<int>(opacityModel) 
	   << static_cast<int>(opacityReaction);

    Ensure (packer.get_ptr() == &packed[0] + size);
    return packed;
}
    
} // end namespace rtt_cdi_gandolf

//---------------------------------------------------------------------------//
// end of GandolfMultigroupOpacity.cc
//---------------------------------------------------------------------------//
