/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/
//
// Class AliHFEtaggedTrackAnalysis
// Analyses tracks with an apriori PID information (i.e. using the daugther
// tracks from well-identified decays of neutral charged particles).
// More information can be found inside the implementation file.
//
#ifndef ALIHFETAGGEDTRACKANALYSIS_H
#define ALIHFETAGGEDTRACKANALYSIS_H

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class AliHFEcollection;
class AliHFEcontainer;
class AliHFEcuts;
class AliHFEpid;
class AliHFEpidQAmanager;
class AliHFEvarManager;

class AliHFEtaggedTrackAnalysis : public TNamed{
  public:
    AliHFEtaggedTrackAnalysis();
    AliHFEtaggedTrackAnalysis(const char *name);
    AliHFEtaggedTrackAnalysis(const AliHFEtaggedTrackAnalysis &ref);
    AliHFEtaggedTrackAnalysis &operator=(const AliHFEtaggedTrackAnalysis &ref);
    ~AliHFEtaggedTrackAnalysis();
    
    void InitContainer();
    void ProcessTrack(AliVTrack *track, Int_t abinitioPID);
        
    AliHFEcontainer *GetContainer() const { return fContainer; }
    AliHFEpidQAmanager *GetPIDqa() const { return fPIDqa; }
    TList * GetPIDQA() const;
    TList * GetCutQA() const;
    AliHFEcollection * GetQAcollection() const { return fQAhistos; }
    Bool_t  GetClean() const { return fClean; }; 
    Bool_t IsPbPb() const { return fIsPbPb; }
    Bool_t IsPP() const { return !fIsPbPb; }
    Bool_t IspPb() const { return fIspPb; }
    Double_t GetMagneticField() const { return fMagneticField; };
    AliHFEvarManager *GetVarManager() const { return fVarManager; }

    void SetCuts(AliHFEcuts *cuts);
    void SetPID(AliHFEpid *pid);
    void SetCentrality(Int_t centrality) { fCentralityF = centrality; };
    void SetPbPb(){ fIsPbPb = kTRUE; }
    void SetPP() { fIsPbPb = kFALSE; }
    void SetpPb() { fIsPbPb = kFALSE; fIspPb=kTRUE; }
    void SetAOD() { fIsAOD = kTRUE; }
    void SetClean(Bool_t clean) { fClean = clean; };
    void SetMagneticField(Double_t magneticField) { fMagneticField = magneticField; };
    void SetVariablesTRD(Bool_t variablesTRD) { fVariablesTRD = variablesTRD; };

  private:
    enum{
      kIsOwner = BIT(14),
      kIsOwnerCuts = BIT(15)
    };
    AliHFEvarManager    *fVarManager;   // Variable Manager
    AliHFEcontainer     *fContainer;    // Output container
    AliHFEpid           *fPID;          // PID selection
    AliHFEpidQAmanager  *fPIDqa;        // PID monitoring
    AliHFEcuts          *fCuts;         // Single track cuts
    AliCFManager        *fCFM;          // CF Manager used for the track filtering
    AliHFEcollection    *fQAhistos;     // QA histos
    Int_t              fCentralityF;  // Centrality
    Bool_t               fClean;        // Clean
    Double_t             fMagneticField; // Magnetic field
    Bool_t               fVariablesTRD;  //  Use phi angle at the first plane of the TRD
    Bool_t               fIsPbPb;        // Analysis Type: PbPb or no PbPb
    Bool_t               fIspPb;        // Analysis Type: pPb or no pPb
    Bool_t               fIsAOD;        // Analysis Type: AOD
    
  ClassDef(AliHFEtaggedTrackAnalysis, 0)
};
#endif
