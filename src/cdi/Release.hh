//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi/Release.hh
 * \author Kelly Thompson
 * \date   Thu Jun 22 16:18:14 2000
 * \brief  Release function for the cdi library
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_Release_hh__
#define __cdi_Release_hh__

//===========================================================================//
/*!
 * \page cdi_overview Overview of the cdi package
 * \version 1_1_0
 * \author Kelly Thompson and Tom Evans
 * 
 * The cdi package is a Common Data Interface package that allows a client to
 * access material, opacity, and equation of state data from a single
 * interface object.  The package consists of the following core classes:
 *
 * \arg \b CDI interface class (CDI.hh)
 * \arg \b GrayOpacity gray opacity interface (base) class (GrayOpacity.hh)
 * \arg \b MultigroupOpacity multigroup opacity interface (base) class
 * (MultigroupOpacity.hh)
 * \arg \b EoS equation of state interface (base) class (EoS.hh)
 *
 * Additionally, there are enumerations (rtt_cdi::Model and
 * rtt_cdi::Reaction) that describe opacity date in the OpacityCommon.hh
 * file.
 *
 * As can be inferred from the above classes, CDI provides an interface
 * specification to various data.  The CDI class itself provides a "holder"
 * for GrayOpacity, MultigroupOpacity, and EoS derived classes.  The
 * definitions of the derived classes our provided elsewhere (cdi_analytic,
 * cdi_gandolf, cdi_eospac for example). 
 *
 * In general, one can define a derived class for any data format that
 * provides gray, multigroup, or equation of state data.  As long as the
 * derived classes publicly inherit from the interface specifications
 * provided by GrayOpacity, MultigroupOpacity, and EoS, they can be used by
 * CDI.   Examples of usage are provided in the cdi/test directory.
 *
 * To include the CDI package one can include the headers for each component
 * seperately; however, CDI.hh includes all the headers necessary to use
 * CDI (GrayOpacity.hh, MultigroupOpacity.hh, EoS.hh, OpacityCommon.hh).
 *
 */
//===========================================================================//
/*!
 * \namespace rtt_cdi
 *
 * \brief Namespace that contains the cdi package classes and variables.
 *
 */
//===========================================================================//

#include <string>

namespace rtt_cdi 
{
    const std::string release();
}

#endif                          // __cdi_Release_hh__

//---------------------------------------------------------------------------//
//                           end of cdi/Release.hh
//---------------------------------------------------------------------------//
