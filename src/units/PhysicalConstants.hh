//----------------------------------*-C++-*----------------------------------//
// PhysicalConstants.hh
// Randy M. Roberts
// Thu Mar 19 10:04:52 1998
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#ifndef __units_PhysicalConstants_hh__
#define __units_PhysicalConstants_hh__

//===========================================================================//
// namespace PhysicalConstants - 
//
// Date created : Thu Mar 19 10:04:52 1998
// Purpose      : To encapsulate physical and mathematical constants.
//
// revision history:
// -----------------
// 0) original
// 
//===========================================================================//

#ifndef BEGIN_NS_XTM
#define BEGIN_NS_XTM namespace XTM  {
#define END_NS_XTM }
#endif

BEGIN_NS_XTM
    
namespace PhysicalConstants
{
    // Mathematical constants
    
    const double pi                = 3.14159265358979324; // dimensionless

    // Physical constants in SI Units:
    //    m - meters, kg - kilograms, s - seconds, K - kelvin
    //    W - watts, J - joules, C - coulombs, F - farads
    //    mol - mole
    
    // STEFAN-BOLTZMANN CONSTANT (WATTS/(M**2-K**4)
    const double stefanBoltzmannSI = 5.67032e-8;          // W m^-2 K^-4
    
    // BOLTZMANN'S CONSTANT (JOULES/K)
    const double boltzmannSI       = 1.380662e-23;        // J K^-1
    
    // ELECTRON CHARGE (COULOMBS)
    const double electronChargeSI  = 1.6021892e-19;       // C
    
    // AVOGADRO'S NUMBER (1/MOLE)
    const double avogadro          = 6.022045e23;         // mol^-1
    
    // ELECTRON REST MASS (KG)
    const double electronMassSI    = 9.109534e-31;        // kg
    
    // PROTON REST MASS (KG)
    const double protonMassSI      = 1.6726e-27;          // kg
    
    // SPEED OF LIGHT (M/S)
    const double cLightSI          = 2.99792458e8;        // m s^-1
    
    // PERMITTIVITY OF FREE SPACE (F/M)
    const double eps0SI            = 8.854187818e-12;     // F m^-1

    // Derived constants
    
    // CONVERSION FACTOR FROM ELECTRON-VOLTS TO KELVIN (K/eV)
    // (TEMPERATURE IN eV) * eV2K = (TEMPERATURE IN KELVIN)
    const double eV2K              = electronChargeSI / boltzmannSI;

}

END_NS_XTM  // namespace XTM

#endif                          // __units_PhysicalConstants_hh__

//---------------------------------------------------------------------------//
//                              end of units/PhysicalConstants.hh
//---------------------------------------------------------------------------//
