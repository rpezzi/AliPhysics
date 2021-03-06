#ifndef ALIANALYSISTASKPIDBF1_H
#define ALIANALYSISTASKPIDBF1_H

// Analysis task for the PID BF code:
// Base Class : AliBalancePsi.cxx
// Noor Alam(VECC, Kolkata) : sk.noor.alam@cern.ch, noor1989phyalam@gmail.com
// Supervisor: Subhasis Chattopadhyay: sub.chattopadhyay@gmail.com
//[Special thanks to Michael Weber(m.weber@cern.ch) and Panos Christakoglou(panos.christakoglou@cern.ch)] 


class TList;
class TH1F;
class TH2F;
class TH3F; 
class TF1;
class TH3D;
class TParticle;

class AliBalancePsi;
class AliESDtrackCuts;
class AliEventPoolManager;
class AliAnalysisUtils;
class AliAODTrack;
//class AliTHn;

#include "AliAnalysisTaskSE.h"
#include "AliBalancePsi.h"
//#include "AliTHn.h"

#include "AliPID.h"  
#include "AliPIDResponse.h"
#include "AliPIDCombined.h"
 
//================================correction
#define kCENTRALITY 101  
//const Double_t centralityArrayForPbPb[kCENTRALITY+1] = {0.,5.,10.,20.,30.,40.,50.,60.,70.,80.};
//const TString centralityArrayForPbPb_string[kCENTRALITY] = {"0-5","5-10","10-20","20-30","30-40","40-50","50-60","60-70","70-80"};
//================================correction

class AliAnalysisTaskPIDBF : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskPIDBF(const char *name = "AliAnalysisTaskPIDBF");
  virtual ~AliAnalysisTaskPIDBF(); 
   
  virtual void   UserCreateOutputObjects();
  virtual void   UserExec(Option_t *option);
  virtual void   FinishTaskOutput();
  virtual void   Terminate(Option_t *);

  //========================correction
  virtual void   SetInputCorrection(TString filename, 
				    Int_t nCentralityBins, 
				    Double_t *centralityArrayForCorrections);
  //========================correction
  // void SetDebugLevel() {fDebugLevel = kTRUE;} //hides overloaded virtual function

  void SetAnalysisObject(AliBalancePsi *const analysis) {
    fBalance         = analysis;
    }
  void SetShufflingObject(AliBalancePsi *const analysisShuffled) {
    fRunShuffling = kTRUE;
    fShuffledBalance = analysisShuffled;
  }
  void SetMixingObject(AliBalancePsi *const analysisMixed) {
    fRunMixing = kTRUE;
    fMixedBalance = analysisMixed;
  }
  void SetMixingWithEventPlane(Bool_t bMixingWithEventPlane = kTRUE) { fRunMixingEventPlane = bMixingWithEventPlane; }
  void SetMixingTracks(Int_t tracks) { fMixingTracks = tracks; }
  void SetAnalysisCutObject(AliESDtrackCuts *const trackCuts) {
    fESDtrackCuts = trackCuts;}
  void SetVertexDiamond(Double_t vx, Double_t vy, Double_t vz) {
    fVxMax = vx;
    fVyMax = vy;
    fVzMax = vz;
  }

  //==============AOD analysis==============//
  void SetAODtrackCutBit(Int_t bit){
    fnAODtrackCutBit = bit;
  }

  void SetKinematicsCutsAOD(Double_t ptmin, Double_t ptmax, Double_t etamin, Double_t etamax){
    fPtMin  = ptmin;  fPtMax  = ptmax;
    fEtaMin = etamin; fEtaMax = etamax;
  }

  void SetExtraDCACutsAOD(Double_t DCAxy, Double_t DCAz){
    fDCAxyCut  = DCAxy;
    fDCAzCut = DCAz;
  }

   void SetExtraTPCCutsAOD(Double_t maxTPCchi2, Int_t minNClustersTPC){
    fTPCchi2Cut      = maxTPCchi2;
    fNClustersTPCCut = minNClustersTPC;
  }

   void SetExtraTPCCutsSharedAOD(Int_t minTPCsharedCut){
    fTPCsharedCut = minTPCsharedCut;
  }

  //==============MC analysis==============//
  void SetKinematicsCutsMC(Double_t ptmin, Double_t ptmax,
                           Double_t etamin, Double_t etamax){
    fPtMin  = ptmin; fPtMax  = ptmax;
    fEtaMin = etamin; fEtaMax = etamax;
  }
  void UseFlowAfterBurner(TF1 *gDifferentialV2) {
    fDifferentialV2 = gDifferentialV2;
    fUseFlowAfterBurner = kTRUE;
  }

  void ExcludeWeakDecaysInMC() {fExcludeWeakDecaysInMC = kTRUE;}
  void ExcludeResonancesInMC() {fExcludeResonancesInMC = kTRUE;}
  void ExcludeElectronsInMC()  {fExcludeElectronsInMC = kTRUE;}   // This is for Exclude electron when MC data use
  void ExcludeParticlesExtra() {fExcludeParticlesExtra = kTRUE;}
  void ExcludeResonancePDGInMC(Double_t pdgValue) {fExcludeResonancePDGInMC = pdgValue;}

  void SetPDGCode(Int_t gPdgCode) {
    fUseMCPdgCode = kTRUE;
    fPDGCodeToBeAnalyzed = gPdgCode;
  }

  //Centrality
  void SetCentralityEstimator(const char* centralityEstimator) {fCentralityEstimator = centralityEstimator;}
  const char* GetCentralityEstimator(void)  const              {return fCentralityEstimator;}
  void SetCentralityPercentileRange(Double_t min, Double_t max) { 
    fUseCentrality = kTRUE;
    fCentralityPercentileMin=min;
    fCentralityPercentileMax=max;
  }
  void SetImpactParameterRange(Double_t min, Double_t max) { 
    fUseCentrality = kTRUE;
    fImpactParameterMin=min;
    fImpactParameterMax=max;
  }

  //multiplicity
  void SetMultiplicityEstimator(const char* multiplicityEstimator) {fMultiplicityEstimator = multiplicityEstimator;}
  const char* GetMultiplicityEstimator(void)  const              {return fMultiplicityEstimator;}
  void SetMultiplicityRange(Double_t min, Double_t max) {
    fUseMultiplicity = kTRUE;
    fNumberOfAcceptedTracksMin = min;
    fNumberOfAcceptedTracksMax = max;}
  
  // additional event cuts (default = kFALSE)
  void UseOfflineTrigger() {fUseOfflineTrigger = kTRUE;}
  void CheckFirstEventInChunk() {fCheckFirstEventInChunk = kTRUE;}
  void CheckPileUp() {fCheckPileUp = kTRUE;}
  void CheckPrimaryFlagAOD() {fCheckPrimaryFlagAOD = kTRUE;}
  void UseMCforKinematics() {fUseMCforKinematics = kTRUE;}
  void SetCentralityWeights(TH1* hist) { fCentralityWeights = hist; }
  Bool_t AcceptEventCentralityWeight(Double_t centrality);
  
  // function to exclude the weak decay products
  Bool_t IsThisAWeakDecayingParticle(TParticle *thisGuy);
    
  //Acceptance filter
  void SetAcceptanceParameterization(TF1 *parameterization) {
    fAcceptanceParameterization = parameterization;}

  //pid
  enum kDetectorUsedForPID { kTPCpid, kTOFpid, kTPCTOF }; // default TPC & TOF pid (via GetTPCpid & GetTOFpid)  
  enum kDetectorPID_ { kTPCTOFpid_, kTogether_, kTPC_,kTOF_}; // default TPC & TOF pid (via GetTPCpid & GetTOFpid)  
  enum kParticleOfInterest { kMuon, kElectron, kPion, kKaon, kProton };
  enum kParticleType_ { kPion_, kKaon_, kProton_ };

  void SetUseBayesianPID(Double_t gMinProbabilityValue) {
    fUsePID = kTRUE; fUsePIDnSigma = kFALSE; fUsePIDPropabilities = kTRUE;
    fMinAcceptedPIDProbability = gMinProbabilityValue; }

  void SetUseNSigmaPID(Double_t gMaxNSigma) {
    fUsePID = kTRUE; fUsePIDPropabilities = kFALSE; fUsePIDnSigma = kTRUE;
    fPIDNSigma = gMaxNSigma; }

  void SetParticleOfInterest(kParticleOfInterest poi) {
    fParticleOfInterest = poi;}

  void SetParticleType(kParticleType_ particletype_) {
    fParticleType_ = particletype_; } 
  void SetDetectorPID(kDetectorPID_ detectorpid_) {
    fDetectorPID_ = detectorpid_; } 

  void SetDetectorUsedForPID(kDetectorUsedForPID detConfig) {
    fPidDetectorConfig = detConfig;}
    void SetEventClass(TString receivedEventClass){
        fEventClass = receivedEventClass;
    }
    
  void SetCustomBinning(TString receivedCustomBinning) { fCustomBinning = receivedCustomBinning; }


    // electron rejection
    void SetElectronRejection(Double_t gMaxNSigma){
      fElectronRejection = kTRUE;
      fElectronRejectionNSigma = gMaxNSigma;
    }

    void SetElectronOnlyRejection(Double_t gMaxNSigma){
      fElectronRejection       = kTRUE;
      fElectronOnlyRejection   = kTRUE;
      fElectronRejectionNSigma = gMaxNSigma;
    }

    void SetElectronRejectionPt(Double_t minPt,Double_t maxPt){
      fElectronRejectionMinPt  = minPt;
      fElectronRejectionMaxPt  = maxPt;
    }

    void SetVZEROCalibrationFile(const char* filename, const char* lhcPeriod);
 // TOF ant TPC Pt for PID 
    
    void SetTOFPtMinMax(Double_t ptmin, Double_t ptmax){           
       fPtTOFMin = ptmin;
       fPtTOFMax = ptmax;
      }        
       
    void SetTPCPtMinMax(Double_t ptmin, Double_t ptmax){
     fPtTPCMin = ptmin;
     fPtTPCMax = ptmax;
      }

// For QA after and before correction 

  void SetQACorrection(Bool_t qacorrection) {fQACorrection = qacorrection;}  

  void SetMisMatchTOFProb(Double_t fmistmatchTOF,Bool_t ftofMisMatch){
       fMistMatchTOFProb=fmistmatchTOF;
       fTOFMisMatch=ftofMisMatch;
    }

  void SetRapidityUse(Bool_t rapidityUse) {fRapidityInsteadOfEta=rapidityUse;}



 private:
  Double_t    IsEventAccepted(AliVEvent* event);
  Double_t    GetRefMultiOrCentrality(AliVEvent* event);
  Double_t    GetReferenceMultiplicityFromAOD(AliVEvent* event);
  Double_t    GetEventPlane(AliVEvent* event);
  //===============================correction
  Double_t    GetTrackbyTrackCorrectionMatrix(Double_t vEta, 
					      Double_t vPhi, 
					      Double_t vPt, 
					      Short_t vCharge, 
					      Double_t gCentrality);
  //===============================correction
  TObjArray* GetAcceptedTracks(AliVEvent* event, Double_t gCentrality, Double_t gReactionPlane);
  TObjArray* GetShuffledTracks(TObjArray* tracks, Double_t gCentrality);

  Double_t GetNsigmas(AliPIDResponse* fPIDResponse , AliAODTrack* track , Int_t specie);


  Double_t GetChannelEqualizationFactor(Int_t run, Int_t channel);
  Double_t GetEqualizationFactor(Int_t run, const char *side);


// Add On 17.11.2016 For Finding Min sigma between Pion , Koan and Proton

   Int_t MinNsigma(Int_t n, const Double_t *a);

// Add By N.Alam on 13/12/2015
   Bool_t IsTOF(AliAODTrack *track) const; // Here we use TOF Track Pt .6 to 2.0 GeV
   Bool_t IsTPC(AliAODTrack *track) const;  // For TPC Track Pt .2 to .6 GeV
   Double_t Beta(AliAODTrack *track); // Particle v/c=Beta calculation


 
  Bool_t fDebugLevel; // debug level

  TClonesArray* fArrayMC; //! AOD object  //+++++++++++++++++++++
  AliBalancePsi *fBalance; //BF object
  Bool_t fRunShuffling;//run shuffling or not
  AliBalancePsi *fShuffledBalance; //BF object (shuffled)
  Bool_t fRunMixing;//run mixing or not
  Bool_t fRunMixingEventPlane;//run mixing with Event Plane
  Int_t  fMixingTracks;
  AliBalancePsi *fMixedBalance; //TriggeredBF object (mixed)
  AliEventPoolManager*     fPoolMgr;         //! event pool manager

  TList *fList; //fList object
  TList *fListBF; //fList object
  TList *fListBFS; //fList object
  TList *fListBFM; //fList object
  TList *fHistListPIDQA;  //! list of histograms

  TH2F *fHistEventStats; //event stats
  TH2F *fHistCentStats; //centrality stats
  TH2F *fHistCentStatsUsed; //centrality stats USED
  TH1F *fHistTriggerStats; //trigger stats
  TH1F *fHistTrackStats; //Track filter bit stats
  TH1F *fHistVx; //x coordinate of the primary vertex
  TH1F *fHistVy; //y coordinate of the primary vertex
  TH2F *fHistVz; //z coordinate of the primary vertex


  TH2F *fHistMixEvents; //number of events that is mixed with in the current pool
  TH2F *fHistMixTracks; //number of tracks that is mixed with in the current pool

  TH2F *fHistTPCvsVZEROMultiplicity; //VZERO vs TPC reference multiplicity
  TH2F *fHistVZEROSignal; //VZERO channel vs signal

  TH2F *fHistEventPlane; //event plane distribution

  TH2F *fHistClus;//number of clusters (QA histogram)
  TH2F *fHistDCA;//DCA  (QA histogram)
  TH2F *fHistChi2;//track chi2 (QA histogram)
  TH2F *fHistPt;//transverse momentum (QA histogram)
  TH2F *fHistEta;//pseudorapidity (QA histogram)
  TH2F *fHistRapidity;//rapidity (QA histogram)
  TH2F *fHistPhi;//phi (QA histogram)
  TH3F *fHistEtaPhiPos;//eta-phi pos particles (QA histogram) 		 	 
  TH3F *fHistEtaPhiNeg;//eta-phi neg particles (QA histogram)

  TH1F *fHistEta1D;
  TH1F *fHistPhi1D;


//  TH2F *fHistPhiBefore;//phi before v2 afterburner (QA histogram)
//  TH2F *fHistPhiAfter;//phi after v2 afterburner (QA histogram)
  TH2F *fHistPhiPos;//phi for positive particles (QA histogram)
  TH2F *fHistPhiNeg;//phi for negative particles (QA histogram)
  TH2F *fHistV0M;//V0 multiplicities (QA histogram)
  TH2F *fHistRefTracks;//reference track multiplicities (QA histogram)

  //============PID============//

  // Beta and dEdX plot ------------
  TH2F *fHistdEdxTPC;
  TH2F *fHistBetaTOF;

   

  TH2D *fHistdEdxVsPTPCbeforePIDelectron; //!
  TH2D *fHistNSigmaTPCvsPtbeforePIDelectron; //!
  TH2D *fHistdEdxVsPTPCafterPIDelectron; //!
  TH2D *fHistNSigmaTPCvsPtafterPIDelectron; //!
  
  TH3F *fHistCorrectionPlus[kCENTRALITY]; //====correction  Changed it from TH3F to TH3D
  TH3F *fHistCorrectionMinus[kCENTRALITY]; //===correction  Changed it from TH3F to TH3D
  Double_t fCentralityArrayForCorrections[kCENTRALITY];
  Int_t fCentralityArrayBinsForCorrections;
  
  // For Nsigma value :
  Double_t fNsigmaTPC[6];
  Double_t fNsigmaTOF[6];

  TH1* fCentralityWeights;		     // for centrality flattening

  AliPIDResponse *fPIDResponse;     //! PID response object
  AliPIDCombined       *fPIDCombined;     //! combined PID object
  
  kParticleOfInterest  fParticleOfInterest;//analyzed particle
  kParticleType_ fParticleType_; // particle type for analysis
  kDetectorPID_ fDetectorPID_; // particle type for analysis
  kDetectorUsedForPID   fPidDetectorConfig;//used detector for PID

  Bool_t fUsePID; //flag to use PID 
  Bool_t fUsePIDnSigma;//flag to use nsigma method for PID
  Bool_t fUsePIDPropabilities;//flag to use probability method for PID

// For TPC and TOF Pt cut variables 

//  Bool_t fHasTOFPID;  //TOF PID is or not
// Bool_t fHasTPCPID;  // TPC PID is or not

  Double_t fPtTOFMin;  // TOF Min Pt
  Double_t fPtTOFMax;  // TOF Max Pt
  Double_t fPtTPCMin;  // TPC Min Pt
  Double_t fPtTPCMax;  // TPC Max Pt

  Double_t fPIDNSigma;//nsigma cut for PID
  Double_t fMinAcceptedPIDProbability;//probability cut for PID

  Bool_t   fElectronRejection;//flag to use electron rejection
  Bool_t   fElectronOnlyRejection;//flag to use electron rejection with exclusive electron PID (no other particle in nsigma range)
  Double_t fElectronRejectionNSigma;//nsigma cut for electron rejection
  Double_t fElectronRejectionMinPt;//minimum pt for electron rejection (default = 0.)
  Double_t fElectronRejectionMaxPt;//maximum pt for electron rejection (default = 1000.)
  //============PID============//

  AliESDtrackCuts *fESDtrackCuts; //ESD track cuts

  TString fCentralityEstimator;      //"V0M","TRK","TKL","ZDC","FMD"
  Bool_t fUseCentrality;//use the centrality (PbPb) or not (pp)
  Double_t fCentralityPercentileMin;//centrality percentile min
  Double_t fCentralityPercentileMax;//centrality percentile max
  Double_t fImpactParameterMin;//impact parameter min (used for MC)
  Double_t fImpactParameterMax;//impact parameter max (used for MC)

  TString fMultiplicityEstimator;//"V0M","V0A","V0C","TPC"
  Bool_t fUseMultiplicity;//use the multiplicity cuts
  Double_t fNumberOfAcceptedTracksMin;//min. number of number of accepted tracks (used for the multiplicity dependence study - pp)
  Double_t fNumberOfAcceptedTracksMax;//max. number of number of accepted tracks (used for the multiplicity dependence study - pp)
  TH2F *fHistNumberOfAcceptedTracks;//hisot to store the number of accepted tracks
  TH1F *fHistMultiplicity;//hisot to store the number of accepted tracks

  Bool_t fUseOfflineTrigger;//Usage of the offline trigger selection
  Bool_t fCheckFirstEventInChunk;//Usage of the "First Event in Chunk" check (not needed for new productions)
  Bool_t fCheckPileUp;//Usage of the "Pile-Up" event check
  Bool_t fCheckPrimaryFlagAOD;// Usage of check on AliAODtrack::kPrimary (default = OFF)
  Bool_t fUseMCforKinematics;//Usage of MC information for filling the kinematics information of particles (only in MCAODrec mode)

  Double_t fVxMax;//vxmax
  Double_t fVyMax;//vymax
  Double_t fVzMax;//vzmax

  Int_t fnAODtrackCutBit;//track cut bit from track selection (only used for AODs)

  Double_t fPtMin;//only used for AODs
  Double_t fPtMax;//only used for AODs
  Double_t fEtaMin;//only used for AODs
  Double_t fEtaMax;//only used for AODs
  Double_t fPhiMin;//only used for AODs
  Double_t fPhiMax;//only used for AODs 

  Double_t fDCAxyCut;//only used for AODs
  Double_t fDCAzCut;//only used for AODs

  Double_t fTPCchi2Cut;//only used for AODs
  Int_t fNClustersTPCCut;//only used for AODs
  Int_t fTPCsharedCut;//only used for AODs

  TF1 *fAcceptanceParameterization;//acceptance filter used for MC

  TF1 *fDifferentialV2;//pt-differential v2 (from real data)
  Bool_t fUseFlowAfterBurner;//Usage of a flow after burner

  Bool_t fExcludeWeakDecaysInMC;//flag to exclude the weak decay products (if not done by IsPhysicalPrimary) from the MC analysis
  Bool_t fExcludeResonancesInMC;//flag to exclude the resonances' decay products (and conversion) from the MC analysis
  Bool_t fExcludeElectronsInMC;//flag to exclude the electrons from the MC analysis
  Bool_t fExcludeParticlesExtra;//flag to exclude particles from the MC analysis (extra)
  Bool_t fUseMCPdgCode; //Boolean to analyze a set of particles in MC
  Int_t fPDGCodeToBeAnalyzed; //Analyze a set of particles in MC
  Int_t fExcludeResonancePDGInMC;// exclude the resonance with this PDG from the MC analysis
  TString fEventClass; //Can be "EventPlane", "Centrality", "Multiplicity"
  TString fCustomBinning;//for setting customized binning (for output AliTHn of AliBalancePsi)
  
  //VZERO calibration
  TH1F *fHistVZEROAGainEqualizationMap;//VZERO calibration map
  TH1F *fHistVZEROCGainEqualizationMap;//VZERO calibration map
  TH2F *fHistVZEROChannelGainEqualizationMap; //VZERO calibration map
  
   
  // For QA  after and before correction
  Bool_t fQACorrection; 
  TH1D  *fHistQAPtBeforeCorrection;
  TH2D  *fHistQAPtBeforeCorrectionWithCentrality;
  TH1D  *fHistQAPtBeforeCorrectionPos;
  TH2D  *fHistQAPtBeforeCorrectionWithCentralityPos;
  TH1D  *fHistQAPtBeforeCorrectionNeg;
  TH2D  *fHistQAPtBeforeCorrectionWithCentralityNeg;
  TH1D  *fHistQAEtaBeforeCorrection;
  TH2D  *fHistQAEtaBeforeCorrectionWithCentrality;
  TH1D  *fHistQAEtaBeforeCorrectionPos;
  TH2D  *fHistQAEtaBeforeCorrectionWithCentralityPos;
  TH1D  *fHistQAEtaBeforeCorrectionNeg;
  TH2D  *fHistQAEtaBeforeCorrectionWithCentralityNeg;
  TH1D  *fHistQAPhiBeforeCorrection;
  TH2D  *fHistQAPhiBeforeCorrectionWithCentrality;
  TH1D  *fHistQAPhiBeforeCorrectionPos;
  TH2D  *fHistQAPhiBeforeCorrectionWithCentralityPos;
  TH1D  *fHistQAPhiBeforeCorrectionNeg;
  TH2D  *fHistQAPhiBeforeCorrectionWithCentralityNeg;
  TH1D  *fHistQAPtAfterCorrection;
  TH2D  *fHistQAPtAfterCorrectionWithCentrality;
  TH1D  *fHistQAPtAfterCorrectionPos;
  TH2D  *fHistQAPtAfterCorrectionWithCentralityPos;
  TH1D  *fHistQAPtAfterCorrectionNeg;
  TH2D  *fHistQAPtAfterCorrectionWithCentralityNeg;
  TH1D  *fHistQAEtaAfterCorrection;
  TH2D  *fHistQAEtaAfterCorrectionWithCentrality;
  TH1D  *fHistQAEtaAfterCorrectionPos;
  TH2D  *fHistQAEtaAfterCorrectionWithCentralityPos;
  TH1D  *fHistQAEtaAfterCorrectionNeg;
  TH2D  *fHistQAEtaAfterCorrectionWithCentralityNeg;
  TH1D  *fHistQAPhiAfterCorrection;
  TH2D  *fHistQAPhiAfterCorrectionWithCentrality;
  TH1D  *fHistQAPhiAfterCorrectionPos;
  TH2D  *fHistQAPhiAfterCorrectionWithCentralityPos;
  TH1D  *fHistQAPhiAfterCorrectionNeg;
  TH2D  *fHistQAPhiAfterCorrectionWithCentralityNeg;

  // Histogram for Species 

   TH1D *fPIDSpeciesHisto;

// Histogram for NSigma Plot before and after Cut......

  TH2F  *fHistNsigmaTPCPionBeforePIDCut;
  TH2F  *fHistNsigmaTPCKaonBeforePIDCut;
  TH2F  *fHistNsigmaTPCProtonBeforePIDCut;

  TH2F  *fHistNsigmaTOFPionBeforePIDCut;
  TH2F  *fHistNsigmaTOFKaonBeforePIDCut;
  TH2F  *fHistNsigmaTOFProtonBeforePIDCut;

  TH2F  *fHistNsigmaTPCTOFPionBeforePIDCut;
  TH2F  *fHistNsigmaTPCTOFKaonBeforePIDCut;
  TH2F  *fHistNsigmaTPCTOFProtonBeforePIDCut;

  TH2F  *fHistdEdxTPCAfterPIDCut;
  TH2F  *fHistBetaTOFAfterPIDCut;
  TH2F  *fHistNsigmaTPCTOFAfterPIDCut;

  TH1D  *fHistMostProbableNsigma;


  TH2F *fHistNsigmaTPCPionAfterPIDCut;
  TH2F *fHistNsigmaTPCKaonAfterPIDCut;
  TH2F *fHistNsigmaTPCProtonAfterPIDCut;
  TH2F *fHistNsigmaTOFPionAfterPIDCut;
  TH2F *fHistNsigmaTOFKaonAfterPIDCut;
  TH2F *fHistNsigmaTOFProtonAfterPIDCut;


// TOF Mismatch: 
Double_t fMistMatchTOFProb;
Bool_t fTOFMisMatch;
Bool_t fRapidityInsteadOfEta;



  //AliAnalysisUtils
  AliAnalysisUtils *fUtils;//AliAnalysisUtils

  AliAnalysisTaskPIDBF(const AliAnalysisTaskPIDBF&); // not implemented
  AliAnalysisTaskPIDBF& operator=(const AliAnalysisTaskPIDBF&); // not implemented
  
  ClassDef(AliAnalysisTaskPIDBF, 1); // example of analysis
};



#endif
