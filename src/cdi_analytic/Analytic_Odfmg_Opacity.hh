//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_analytic/Analytic_Odfmg_Opacity.hh
 * \author Thomas M. Evans
 * \date   Tue Nov 13 11:19:59 2001
 * \brief  Analytic_Odfmg_Opacity class definition.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_analytic_Analytic_Odfmg_Opacity_hh__
#define __cdi_analytic_Analytic_Odfmg_Opacity_hh__

#include "Analytic_Models.hh"
#include "cdi/OdfmgOpacity.hh"
#include "cdi/OpacityCommon.hh"
#include "ds++/Assert.hh"
#include "ds++/SP.hh"
#include <vector>
#include <string>

namespace rtt_cdi_analytic
{

//===========================================================================//
/*!
 * \class Analytic_Odfmg_Opacity
 *
 * \brief Derived rtt_cdi::OdfmgOpacity class for analytic opacities.
 *
 * Primarily code from Analytic_Multigroup_Opacity.
 */
// 
//===========================================================================//

class Analytic_Odfmg_Opacity : public rtt_cdi::OdfmgOpacity
{
public:
	// Useful typedefs.
	typedef rtt_dsxx::SP<Analytic_Opacity_Model>       SP_Analytic_Model;
	typedef rtt_dsxx::SP<const Analytic_Opacity_Model> const_Model; 
	typedef std::vector<SP_Analytic_Model>             sf_Analytic_Model;
	typedef std::vector<double>                        sf_double;
	typedef std::vector<sf_double>                     vf_double;
	typedef std::string                                std_string;
	typedef std::vector<char>                          sf_char;

private:
	// Group structure.
	sf_double groupBoundaries;

	// Analytic models for each group.
	sf_Analytic_Model group_models;

	// Reaction model.
	rtt_cdi::Reaction reaction;

	// CDI model.
	rtt_cdi::Model model;

	/*!
	 * \brief The band boundaries should be the same as in the rest of the
	 * problem.
	 * It should be a vector with size(the number of bands + 1), 0 on the left,
	 * and 1 on the right.
	 */
	std::vector<double> bandBoundaries;

public:
	// Constructor.
	Analytic_Odfmg_Opacity(
			const sf_double         &groups,
			const sf_double         &bands,
			const sf_Analytic_Model &models,
			rtt_cdi::Reaction        reaction_in,
			rtt_cdi::Model           model_in = rtt_cdi::ANALYTIC);

	// Constructor for packed Analytic_Odfmg_Opacities
	explicit Analytic_Odfmg_Opacity(const sf_char &);

	// >>> ACCESSORS
	const_Model get_Analytic_Model(int g) const
	{ return group_models[g-1]; }

	//! right now, all bands have same model (same opacity) 
	const_Model get_Analytic_Model(int g, int b) const
  	{ return group_models[g-1]; }

	// >>> INTERFACE SPECIFIED BY rtt_cdi::OdfmgOpacity

	/*!
	 * \brief Opacity accessor that returns a 2-D vector of opacities (
	 *     groups * bands ) that correspond to the
	 *     provided temperature and density. 
	 *
	 * \param targetTemperature The temperature value for which an
	 *     opacity value is being requested.
	 * \param targetDensity The density value for which an opacity 
	 *     value is being requested.
	 * \return A vector of opacities.
	 */
	std::vector< std::vector<double> > getOpacity( 
			double targetTemperature,
			double targetDensity ) const; 

	/*!
	 * \brief Opacity accessor that returns a vector of multigroupband
	 *     opacity 2-D vectors that correspond to the provided vector of
	 *     temperatures and a single density value.
	 *
	 * \param targetTemperature A vector of temperature values for
	 *     which opacity values are being requested.
	 * \param targetDensity The density value for which an opacity 
	 *     value is being requested.
	 * \return A vector of vectors of opacities.
	 */
	std::vector< std::vector< std::vector<double> > > getOpacity( 
			const std::vector<double>& targetTemperature,
			double targetDensity ) const; 

	/*!
	 * \brief Opacity accessor that returns a vector of 2-D vectors of
	 *     opacities that correspond to the provided vector of
	 *     densities and a single temperature value.
	 *
	 * \param targetTemperature The temperature value for which an 
	 *     opacity value is being requested.
	 * \param targetDensity A vector of density values for which
	 *     opacity values are being requested.
	 * \return A vector of vectors of opacities.
	 */
	std::vector< std::vector< std::vector<double> > > getOpacity( 
			double targetTemperature,
			const std::vector<double>& targetDensity ) const; 


	//! Query to see if data is in tabular or functional form (false).
	bool data_in_tabular_form() const { return false; }

	//! Query to get the reaction type.
	rtt_cdi::Reaction getReactionType() const { return reaction; }

	//! Query for model type.
	rtt_cdi::Model getModelType() const { return model; }

	// Return the energy policy (gray).
	inline std_string getEnergyPolicyDescriptor() const;

	// Get the data description of the opacity.
	inline std_string getDataDescriptor() const;

	// Get the name of the associated data file.
	inline std_string getDataFilename() const;

	//! Get the temperature grid (size 0 for function-based analytic data).
	sf_double getTemperatureGrid() const { return sf_double(0); }

	//! Get the density grid (size 0 for function-based analytic data).
	sf_double getDensityGrid() const { return sf_double(0); }

	//! Get the group boundaries (keV) of the multigroup set.
	sf_double getGroupBoundaries() const { return groupBoundaries; }

	//! Get the size of the temperature grid (size 0).
	int getNumTemperatures() const { return 0; }

	//! Get the size of the density grid (size 0).
	int getNumDensities() const { return 0; }

	//! Get the number of frequency group boundaries.
	int getNumGroupBoundaries() const { return groupBoundaries.size(); }

	//! Get the number of frequency group boundaries.
	int getNumGroups() const { return groupBoundaries.size() - 1; }

	/*!
	 * \brief Returns a vector of points along the cumulative opacity 
	 * 		distribution that mark the fraction of each band. First point is
	 * 		always zero, last point is always one.
	 */
	std::vector<double> getBandBoundaries() const {
		return bandBoundaries;
	}

	/*!
	 * \brief Returns the number of group boundaries found in the
	 *     current multigroup data set.
	 */
	int getNumBandBoundaries() const {
		return bandBoundaries.size();
	}

	/*!
	 * \brief Returns the number of band boundaries set in the
	 *     current multigroup data set. The lower boundary is always zero,
	 *     and the upper boundary is always one. Analagous to multigroup:
	 *     numBands = numBandBoundaries - 1.
	 */
	int getNumBands() const {
		return getNumBandBoundaries() - 1;
	}

	// Pack the Analytic_Odfmg_Opacity into a character string.
	sf_char pack() const;

	/*!
	 * \brief Returns the general opacity model type, defined in OpacityCommon.hh
	 *
	 * Since this is an analytic model, return 1 (rtt_cdi::ANALYTIC_TYPE)
	 */
	rtt_cdi::OpacityModelType getOpacityModelType() const {
		return rtt_cdi::ANALYTIC_TYPE;
	}
};

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * \brief Return the energy policy descriptor (mg). 
 */
Analytic_Odfmg_Opacity::std_string 
Analytic_Odfmg_Opacity::getEnergyPolicyDescriptor() const 
{
	return std_string("odfmg");
}

//---------------------------------------------------------------------------//
/*!
 * \brief Return a string describing the opacity model.
 */
Analytic_Odfmg_Opacity::std_string
Analytic_Odfmg_Opacity::getDataDescriptor() const
{
	std_string descriptor;

	if (reaction == rtt_cdi::TOTAL)
		descriptor = "Analytic Odfmg Total";
	else if (reaction == rtt_cdi::ABSORPTION)
		descriptor = "Analytic Odfmg Absorption";
	else if (reaction == rtt_cdi::SCATTERING)
		descriptor = "Analytic Odfmg Scattering";
	else
	{
		Insist (0, "Invalid analytic multigroup model opacity!");
	}

	return descriptor;
}

//---------------------------------------------------------------------------//
/*!
 * \brief Return NULL string for the data filename.
 */
Analytic_Odfmg_Opacity::std_string
Analytic_Odfmg_Opacity::getDataFilename() const 
{
	return std_string();
}

} // end namespace rtt_cdi_analytic

#endif              // __cdi_analytic_Analytic_Odfmg_Opacity_hh__

//---------------------------------------------------------------------------//
//                   end of cdi_analytic/Analytic_Odfmg_Opacity.hh
//---------------------------------------------------------------------------//