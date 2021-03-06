#ifndef ALIRSNVALUEDAUGHTER_H
#define ALIRSNVALUEDAUGHTER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////////////////////////////////////
//
//  Values which depend on 4-momentum of the daughters.
//
////////////////////////////////////////////////////////////////////////////////

#include "AliRsnValue.h"

class AliRsnValueDaughter : public AliRsnValue {
public:

   enum EType {
      kP,          // total momentum
      kPt,         // transverse momentum
      kPtpc,       // total momentum in the TPC inner wall
      kEta,        // pseudo-rapidity
      kY,          // rapidity
      kMass,       // mass
      kITSsignal,  // ITS signal
      kTPCsignal,  // TPC signal
      kTOFsignal,  // TOF signal
      kTPCnsigmaPi,// TPC number of sigmas pion
      kTPCnsigmaK, // TPC number of sigmas kaon
      kTPCnsigmaP, // TPC number of sigmas proton
      kTOFnsigmaPi,// TOF number of sigmas pion
      kTOFnsigmaK, // TOF number of sigmas kaon
      kTOFnsigmaP, // TOF number of sigmas proton
      kTOFdeltaPi, // TOF t-texp for pion hypothesis
      kTOFdeltaK, // TOF t-texp for pion hypothesis
      kTOFdeltaP, // TOF t-texp for pion hypothesis
      kNITSclusters,  // n ITS clusters
      kNTPCclusters,  // n TPC clusters
      kNTPCcrossedRows,  // n TPC crossed rows
      kNTPCcrossedRowsFclusters,  // n TPC crossed rows over findable clusters
      kITSchi2,     // ITS chi^2
      kTPCchi2,     // TPC chi^2
      kDCAXY,       // DCA xy
      kDCAZ,        // DCA z
      kCharge,     // charge
      kPhi,        // azimuthal angle at vertex
      kPhiOuterTPC,// azimuthal angle at TPC outer radius
      kV0DCA,       // V0 DCA 
      kV0Radius,       // V0 radius
      kV0Mass,       // V0 mass 
      kV0P,       // V0 momentum
      kV0Pt,       // V0 transverse momentum
      kV0NPt,       // transverse momentum of negative V0 daughter
      kV0PPt,       // transverse momentum of positive V0 daughter
      kDaughterDCA, // DCA of V0 Daughters
      kCosPointAng, // V0 Cosing of Pointing Angle
      kLambdaProtonPIDCut,         //V0 - Lambda number of sigmas proton	   
      kAntiLambdaAntiProtonPIDCut, //V0 - AntiLambda number of sigmas antiproton	
      kLambdaPionPIDCut,	          //V0 - Lambda number of sigmas pion	
      kAntiLambdaAntiPionPIDCut,   //V0 - AntiLambda number of sigmas pion	
      kTypes
   };

   AliRsnValueDaughter(const char *name = "valDaughter", EType type = kTypes);
   AliRsnValueDaughter(const AliRsnValueDaughter &copy);
   AliRsnValueDaughter &operator=(const AliRsnValueDaughter &copy);
   virtual ~AliRsnValueDaughter() { }

   void             SetType(EType type)  {fType = type;}
   EType            GetType()     const  {return fType;}
   const char      *GetTypeName() const;

   virtual Bool_t   Eval(TObject *object);

protected:

   EType           fType;                //  type from enumeration

   ClassDef(AliRsnValueDaughter, 1)  // AliRsnValueDaughter class
};

#endif
