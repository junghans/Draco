//----------------------------------*-C++-*----------------------------------//
// P1Coeffs.t.cc
// Randy M. Roberts
// Thu Nov  5 13:14:06 1998
//---------------------------------------------------------------------------//
// @> 
//---------------------------------------------------------------------------//

#include "3T/P1Coeffs.hh"

namespace rtt_3T
{

 //---------------------------------------------------------------------------//
 // P1Coeffs:
 //     The constructur for the P1Coeffs object.
 //     We will allocate memory for our smart pointers,
 //     and then call calcP1Coeffs to load them up.
 //---------------------------------------------------------------------------//
 
 template<class DS>
 P13T<DS>::P1Coeffs::P1Coeffs(const P13T<DS> &p13T_,
			      double dt_,
			      int groupNo_,
			      const P13TOptions &options_,
                              const DiffusionSolver &solver_,
			      const MaterialProperties &matprops_,
			      const ncvsf &velocity_,
			      const RadiationStateField &prevStateField_,
			      const ccsf &QRad_,
			      const ccsf &QElectron_,
			      const ccsf &QIon_,
			      const ccsf &TElectron_,
			      const ccsf &TIon_)
     : p13T(p13T_), spMesh(p13T_.getMesh()),
       dt(dt_), groupNo(groupNo_), options(options_), solver(solver_),
       matprops(matprops_), velocity(velocity_),
       prevStateField(prevStateField_),
       QRad(QRad_), QElectron(QElectron_), QIon(QIon_),
       TElectron(TElectron_), TIon(TIon_),
       xQEEM(spMesh), xREEM(spMesh), xQRadBar(spMesh),
       xQElecStar(spMesh), xCvStar(spMesh), xNu(spMesh),
       xSigmaAbsBar(spMesh), xD(spMesh), xFprime(spMesh),
       xXiTilde(spMesh), xXiBracket(spMesh), xXiOmegaBracket(spMesh)
 {
     calcP1Coeffs();
 }

 //---------------------------------------------------------------------------//
 // calcP1Coeffs:
 //     Calculate the coefficients, e.g. diffusion and removal, and
 //     source terms for solving the P1 equation.
 //---------------------------------------------------------------------------//

 template<class DS>
 void P13T<DS>::P1Coeffs::calcP1Coeffs()
 {
     // Set the radiation physics to the given units.
    
     const RadiationPhysics radPhys(matprops.getUnits());

     // set up some needed scalars, like tau

     double c = radPhys.getLightSpeed();
     double tau = 1.0/(c*dt);

    // If this is diffusion, then tauP1 = 0.0.
    // If this is full P1 then tauP1 = tau.
    
     double tauP1 = tau * options.getP1TauMultiplier();

     // Ask the material properties for sigma total.
     // It is the material properties' responsibility to do
     // any averaging of temperatures, etc. to achieve the correct
     // resulting sigmaTotal.

     fcdsf sigmaTotal(spMesh);
     matprops.getSigmaTotal(groupNo, sigmaTotal);

     //
     // We can now calculate the results
     //

     ccsf sigmaAbs(spMesh);
     matprops.getSigmaAbsorption(groupNo, sigmaAbs);
     fcdsf fcSigmaAbs(spMesh);
     matprops.getSigmaAbsorption(groupNo, fcSigmaAbs);

     // Allocate and calculate the diffusion constant.

     xD = (1.0/3.0) / (sigmaTotal + tauP1);

     // Make shorthand references.

     ccsf &QEEM = xQEEM;
     ccsf &REEM = xREEM;
     ccsf &QRadBar = xQRadBar;
     const ccsf &QElecStar = xQElecStar;
     const ccsf &CvStar = xCvStar;
     const ccsf &nu = xNu;
     const ccsf &xiBracket = xXiBracket;
     const fcdsf &xiOmegaBracket = xXiOmegaBracket;

     if (options.getIsCoupledMaterial())
     {
       	 calcVelocityCorrections();
	 calcStarredFieldsAndNu();

	 cerr << "QElecStar: " << *QElecStar.begin() << endl;
	 cerr << "CvStar: " << *CvStar.begin() << endl;
	 cerr << "nu: " << *nu.begin() << endl;
    
	 // Calculate modified sigma absorption

	 xSigmaAbsBar = (1.0 - nu) * sigmaAbs + tau;

	 // Calculate the emmissive removal coefficient
	
	 REEM = -1.0 * nu * sigmaAbs;
	
	 // We need the Bhat.

	 ccsf Bhat(spMesh);
	 p13T.getBhat(Bhat, radPhys, TElectron);

	 // Get sigma emission
    
	 ccsf sigmaEmission(spMesh);
	 matprops.getSigmaEmission(groupNo, sigmaEmission);

	 // Calculate the emmissive source term.
	
	 QEEM = (1.0 - nu)*sigmaEmission*Bhat + nu*QElecStar;

	 // Calculated modified radiation source

	 QRadBar = tau*prevStateField.phi + xiBracket + QEEM + QRad;
     }
     else
     {
	 xSigmaAbsBar = sigmaAbs + tau;
	 REEM = 0.0;
	 QEEM = 0.0;
	 QRadBar = tau*prevStateField.phi + QRad;
     }

     // Calculate the "telegraph" term to the P1 equation.

     xFprime = (tauP1*prevStateField.F + xiOmegaBracket) / (sigmaTotal + tauP1);

 }

 //------------------------------------------------------------------------//
 // calcVelocityCorrections:
 //    Calculate the velocity correction terms.
 //------------------------------------------------------------------------//

 template<class DS>
 void P13T<DS>::P1Coeffs::calcVelocityCorrections()
 {
     // Set the radiation physics to the given units.
    
     const RadiationPhysics radPhys(matprops.getUnits());

     // obtain the speed of light

     double c = radPhys.getLightSpeed();

     // put the velocities into the right field type

     DiscMomentumField vvelocity(spMesh);
     MT::gather ( vvelocity, velocity, MT::OpAssign() );

     // determine the vertex to cell volume ratios

     ccsf cell_volumes(spMesh);
     spMesh->get_cell_volumes(cell_volumes);
     DiscKineticEnergyField vertex_volumes(spMesh);
     spMesh->get_vertex_volumes(vertex_volumes);
     DiscKineticEnergyField vc_cell_volumes(spMesh);
     MT::gather ( vc_cell_volumes, cell_volumes, MT::OpAssign() );
     DiscKineticEnergyField vc_volume_ratios(spMesh);
     vc_volume_ratios = vertex_volumes/vc_cell_volumes;

     // calculate xiTilde

     fcdsf sigmaAbs(spMesh);
     matprops.getSigmaAbsorption(groupNo, sigmaAbs);
     DiscFluxField sigmaF(spMesh);
     sigmaF = sigmaAbs*prevStateField.F;
     DiscKineticEnergyField KEnergy(spMesh);
     solver.dotProduct(KEnergy, sigmaF, vvelocity);
     // add in the other flux term here

     KEnergy *= vc_volume_ratios;
     ccsf &xiTilde = xXiTilde;
     MT::scatter ( xiTilde, KEnergy, MT::OpAddAssign() );
     xiTilde *= (2./c);

     // calculate xiBracket
     fcdsf sigmaScatter(spMesh);
     matprops.getSigmaScattering(groupNo, sigmaScatter);
     sigmaF = (sigmaAbs - sigmaScatter)*prevStateField.F;
     solver.dotProduct(KEnergy, sigmaF, vvelocity);
     // add in the other flux term here

     KEnergy *= vc_volume_ratios;
     ccsf &xiBracket = xXiBracket;
     MT::scatter ( xiBracket, KEnergy, MT::OpAddAssign() );
     xiBracket /= c;

     // calculate xiOmegaBracket
 }

 //------------------------------------------------------------------------//
 // calcStarredFieldsAndNu:
 //    Calculate Qe*, Cv*, and nu.
 //    These are needed to calculate other coefficients
 //    and delta temperatures.
 //------------------------------------------------------------------------//

 template<class DS>
 void P13T<DS>::P1Coeffs::calcStarredFieldsAndNu()
 {
     // Set the radiation physics to the given units.
    
     const RadiationPhysics radPhys(matprops.getUnits());

     // Calculate Qe* and Cv*.
     // We will then calculate nu ourself.
    
     calcStarredFields();

    // Calculate the 4pi*Planckian's temperature derivative.

     ccsf dBhatdT(spMesh);
     p13T.getdBhatdT(dBhatdT, radPhys, TElectron);
    
    // Get sigma emission
    
     ccsf sigmaEmission(spMesh);
     matprops.getSigmaEmission(groupNo, sigmaEmission);

    // Make shorthand references.

     const ccsf &CvStar = xCvStar;
     ccsf &nu = xNu;

    // Calculate the "nu" used in the 3T modification of sigmaAbs
    
     nu = dt * sigmaEmission * dBhatdT /
	 (CvStar + dt * sigmaEmission * dBhatdT);
 }

 //------------------------------------------------------------------------//
 // calcStarredFields:
 //    Calculate Qe*, Cv*, but not nu.
 //    These are needed to calculate other coefficients
 //    and delta temperatures.
 //------------------------------------------------------------------------//

 template<class DS>
 void P13T<DS>::P1Coeffs::calcStarredFields()
 {
     // Get the electron and ion heat capacities.
    
     ccsf CvElec(spMesh);
     matprops.getElectronSpecificHeat(CvElec);

     ccsf CvIon(spMesh);
     matprops.getIonSpecificHeat(CvIon);

    // We need gamma, the electron-ion coupling coefficient.

     ccsf gamma(spMesh);
     matprops.getElectronIonCoupling(gamma);

    // tmpCoeff is a common term to two calculations.
    // Let's just do it once.
    
     ccsf tmpCoeff(spMesh);
     // tmpCoeff = (gamma*dt) / (CvIon + gamma*dt);
     tmpCoeff = gamma / (CvIon/dt + gamma);

     // Shorthand reference names.

     ccsf &CvStar = xCvStar;
     ccsf &QElecStar = xQElecStar;
     const ccsf &xiTilde = xXiTilde;

    // CvStar is one of the results, as well as intermediate.
    
     CvStar = CvElec + CvIon * tmpCoeff;
    
     // Calculate QElecStar (Qe*).

     QElecStar = QElectron + (CvIon/dt * (TIon - TElectron) + QIon) * tmpCoeff
                 - xiTilde;

 }

} // end namespace rtt_3T

//---------------------------------------------------------------------------//
//                              end of P1Coeffs.t.cc
//---------------------------------------------------------------------------//
