//----------------------------------*-C++-*----------------------------------//
// MultiMatCellMatProps.hh
// John McGhee
// Mon Sep 14 08:38:38 1998
//---------------------------------------------------------------------------//
// @> A material property class for cells which contain more than one material.
//---------------------------------------------------------------------------//

#ifndef __matprops_MultiMatCellMatProps_hh__
#define __matprops_MultiMatCellMatProps_hh__

#include <vector>
#include "ds++/SP.hh"
#include "units/Units.hh" 

namespace rtt_matprops {

//===========================================================================//
// class MultiMatCellMatProps - Material properties for multi-material cells.

// This class provides a means to deal with computational cells which 
// contain more than one material. This is most often seen in Eulerian codes.
// The method used is a simplistic one modeled on a
// "Temperature-Equilibration" principle.


// 
//===========================================================================//

template <class UMCMP>  // Uni-Material Cell Material Properties
class MultiMatCellMatProps {

// NESTED  CLASSES AND TYPEDEFS

  public:

    template<class FT, class FT1> class MaterialStateField;

// FRIENDS
    
  private:

    template <class U>
    template <class FT, class FT1>
    friend class MultiMatCellMatProps<U>::MaterialStateField;

// DATA

  private:
    
    dsxx::SP<UMCMP> spumcmp;

// CREATORS

  public:
    MultiMatCellMatProps(const dsxx::SP<UMCMP> &spumcmp_)
	:spumcmp(spumcmp_)
    {
    //empty
    }

    ~MultiMatCellMatProps()
    {
    //empty
    }

// ACCESSORS

  public:

    const XTM::Units &getUnits() const
    {
	return spumcmp->getUnits();
    }

    int getNumGroups(int materialId) const
    {
	return spumcmp->getNumGroups(materialId);
    }

    int getMaxScatteringPnOrder(int materialId) const
    {
	return spumcmp->getMaxScatteringPnOrder(materialId);
    }

//------------------------------------------------------------------------//
// getMaterialState:
//    Return a material state field from density, temperature,
//    and materia id fields.
//------------------------------------------------------------------------//

    template<class FT, class FT1, class FT2>
    MaterialStateField<FT, FT1> 
    getMaterialState(const FT1 &density_,
		     const FT1 &electronTemp_,
		     const FT1 &ionTemp_,
		     const FT1 &volumeFraction_,
		     const FT2 &matId_) const
    {
	return MaterialStateField<FT,FT1>(*this, density_, electronTemp_,
					  ionTemp_, volumeFraction_, matId_);
    }
};


//===========================================================================//
// class MaterialStateField
//===========================================================================//

template <class UMCMP>
template <class FT, class FT1>
class MultiMatCellMatProps<UMCMP>::MaterialStateField
{

// NESTED TYPEDEFS

    typedef typename FT::value_type value_type;
    typedef typename FT1::value_type::value_type value_type1;
    typedef std::vector<value_type1> VV1;
    typedef typename UMCMP:: template MaterialStateField<VV1> UmcMsf;

// FRIENDS
    
    friend class MultiMatCellMatProps;

// DATA
    
  private:

//  std::vector<UMCMP::MaterialStateField<VV1> >  matState;
    std::vector<UmcMsf>  matState;
    
    std::vector<std::vector<value_type1> > volumeFraction;
    
// matState.size() = volumeFraction.size() = ncells
// matState[i].size() = volumeFraction[i].size() = nmat in cell i

// CREATORS

  private:

    template<class FT2>
    MaterialStateField( const MultiMatCellMatProps<UMCMP> &matprops_,
			const FT1 &density_, const FT1 &electronTemp_,
			const FT1 &ionTemp_, const FT1 &volumeFraction_,
			const FT2 &matId_);
  public:
    
    ~MaterialStateField()
    {
    // empty
    }

// ACCESSORS

  public:

// Average properties
    
    void getSigmaAbsorption(int group, FT &results) const;
    void getSigmaTotal(int group, FT &results) const;
    void getSigmaEmission(int group, FT &results) const;
    void getElectronIonCoupling(FT &results) const;
    void getElectronConductionCoeff(FT &results) const;
    void getIonConductionCoeff(FT &results) const;
    void getElectronSpecificHeat(FT &results) const;
    void getIonSpecificHeat(FT &results) const;
    void getElectronTemperature(FT &results) const;
    void getIonTemperature(FT &results) const;
    
// Properties by material

    void getElectronSpecificHeatByMat(FT1 &results) const;
    void getIonSpecificHeatByMat(FT1 &results) const;
    void getElectronTempByMat(FT1 &results) const;
    void getIonTempByMat(FT1 &results) const;

};

} // end of rtt_matprops namespace

#endif                          // __matprops_MultiMatCellMatProps_hh__

//---------------------------------------------------------------------------//
//                              end of matprops/MultiMatCellMatProps.hh
//---------------------------------------------------------------------------//
