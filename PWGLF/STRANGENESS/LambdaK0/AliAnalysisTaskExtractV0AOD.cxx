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

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Modified version of AliAnalysisTaskCheckCascade.cxx.
// This is a 'hybrid' output version, in that it uses a classic TTree
// ROOT object to store the candidates, plus a couple of histograms filled on
// a per-event basis for storing variables too numerous to put in a tree. 
//
// --- Added bits of code for checking V0s 
//      (from AliAnalysisTaskCheckStrange.cxx)
//
//  --- Algorithm Description 
//   1. Perform Physics Selection
//   2. Perform Primary Vertex |z|<10cm selection
//   3. Perform Primary Vertex NoTPCOnly vertexing selection (>0 contrib.)
//   4. Perform Pileup Rejection
//   5. Analysis Loops: 
//    5a. Fill TTree object with V0 information, candidates
//
//  Please Report Any Bugs! 
//
//   --- David Dobrigkeit Chinellato
//        (david.chinellato@gmail.com)
//
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//   IMPORTANT: This is a development version of the task and there are known
//   problems in it, which are associated to incompatibilities of methods used
//   in AODs/ESDs. The preferred version of the Extract V0 task is the one
//   which runs on ESDs ("AliAnalysisTaskExtractV0"). Use this AOD version at
//   your own risk!
//
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class TTree;
class TParticle;
class TVector3;

//class AliMCEventHandler;
//class AliMCEvent;
//class AliStack;

class AliESDVertex;
class AliAODVertex;
class AliESDv0;
class AliAODv0;

#include <Riostream.h>
#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THnSparse.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "AliLog.h"
#include "AliCentrality.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliV0vertexer.h"
#include "AliCascadeVertexer.h"
#include "AliESDpid.h"
#include "AliESDtrack.h"

#include "AliESDtrackCuts.h"
#include "AliInputEventHandler.h"
#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"

#include "AliCFContainer.h"
#include "AliMultiplicity.h"

#include "AliESDcascade.h"
#include "AliAODcascade.h"
#include "AliESDUtils.h"
#include "AliESDHeader.h"
#include "AliAODTrack.h"
#include "AliAnalysisTaskExtractV0AOD.h"

//debugging purposes
#include "TObjectTable.h"

ClassImp(AliAnalysisTaskExtractV0AOD)

AliAnalysisTaskExtractV0AOD::AliAnalysisTaskExtractV0AOD() 
  : AliAnalysisTaskSE(), fListHistV0(0), fTree(0), fPIDResponse(0),
   fkIsNuclear   ( kFALSE ), 
   fkLowEnergyPP ( kFALSE ),
   fkUseOnTheFly ( kFALSE ),
   fTriggerMask  ( "kMB"  ),
//------------------------------------------------
// Variables for tree 
//------------------------------------------------

	 fTreeVariableChi2V0(0),
	 fTreeVariableDcaV0Daughters(0),
	 fTreeVariableDcaV0ToPrimVertex(0),
	 fTreeVariableDcaPosToPrimVertex(0),
	 fTreeVariableDcaNegToPrimVertex(0),
	 fTreeVariableV0CosineOfPointingAngle(0),
	 fTreeVariableV0Radius(0),
	 fTreeVariablePt(0),
	 fTreeVariableRapK0Short(0),
	 fTreeVariableRapLambda(0),
	 fTreeVariableInvMassK0s(0),
	 fTreeVariableInvMassLambda(0),
	 fTreeVariableInvMassAntiLambda(0),
	 fTreeVariableAlphaV0(0),
	 fTreeVariablePtArmV0(0),
	 fTreeVariableNegTotMomentum(0),
	 fTreeVariablePosTotMomentum(0),
	 fTreeVariableNegdEdxSig(0),
	 fTreeVariablePosdEdxSig(0),
	 fTreeVariableNegEta(0),
	 fTreeVariablePosEta(0),

	 fTreeVariableNSigmasPosProton(0),
	 fTreeVariableNSigmasPosPion(0),
	 fTreeVariableNSigmasNegProton(0),
	 fTreeVariableNSigmasNegPion(0),
	
	 fTreeVariableDistOverTotMom(0),
   fTreeVariableLeastNbrCrossedRows(0),
	 fTreeVariableLeastRatioCrossedRowsOverFindable(0),
	 fTreeVariableMultiplicity(0),

   fTreeVariableRunNumber(0),
   fTreeVariableEventNumber(0),


//------------------------------------------------
// HISTOGRAMS
// --- Filled on an Event-by-event basis
//------------------------------------------------
   fHistV0MultiplicityBeforeTrigSel(0),
   fHistV0MultiplicityForTrigEvt(0),
   fHistV0MultiplicityForSelEvt(0),
   fHistV0MultiplicityForSelEvtNoTPCOnly(0),
   fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup(0),
   fHistMultiplicityBeforeTrigSel(0),
   fHistMultiplicityForTrigEvt(0),
   fHistMultiplicity(0),
   fHistMultiplicityNoTPCOnly(0),
   fHistMultiplicityNoTPCOnlyNoPileup(0),
   fHistPVx(0),
   fHistPVy(0),
   fHistPVz(0),
   fHistPVxAnalysis(0),
   fHistPVyAnalysis(0),
   fHistPVzAnalysis(0),
   fHistSwappedV0Counter(0)
{
  // Dummy Constructor
}

AliAnalysisTaskExtractV0AOD::AliAnalysisTaskExtractV0AOD(const char *name) 
  : AliAnalysisTaskSE(name), fListHistV0(0), fTree(0), fPIDResponse(0),
   fkIsNuclear   ( kFALSE ), 
   fkLowEnergyPP ( kFALSE ),
   fkUseOnTheFly ( kFALSE ),
   fTriggerMask  ( "kMB"  ),
     
//------------------------------------------------
// Variables for tree 
//------------------------------------------------

	 fTreeVariableChi2V0(0),
	 fTreeVariableDcaV0Daughters(0),
	 fTreeVariableDcaV0ToPrimVertex(0),
	 fTreeVariableDcaPosToPrimVertex(0),
	 fTreeVariableDcaNegToPrimVertex(0),
	 fTreeVariableV0CosineOfPointingAngle(0),
	 fTreeVariableV0Radius(0),
	 fTreeVariablePt(0),
	 fTreeVariableRapK0Short(0),
	 fTreeVariableRapLambda(0),
	 fTreeVariableInvMassK0s(0),
	 fTreeVariableInvMassLambda(0),
	 fTreeVariableInvMassAntiLambda(0),
	 fTreeVariableAlphaV0(0),
	 fTreeVariablePtArmV0(0),
	 fTreeVariableNegTotMomentum(0),
	 fTreeVariablePosTotMomentum(0),
	 fTreeVariableNegdEdxSig(0),
	 fTreeVariablePosdEdxSig(0),
	 fTreeVariableNegEta(0),
	 fTreeVariablePosEta(0),

	 fTreeVariableNSigmasPosProton(0),
	 fTreeVariableNSigmasPosPion(0),
	 fTreeVariableNSigmasNegProton(0),
	 fTreeVariableNSigmasNegPion(0),
	
	 fTreeVariableDistOverTotMom(0),
   fTreeVariableLeastNbrCrossedRows(0),
	 fTreeVariableLeastRatioCrossedRowsOverFindable(0),
	 fTreeVariableMultiplicity(0),

   fTreeVariableRunNumber(0),
   fTreeVariableEventNumber(0),

//------------------------------------------------
// HISTOGRAMS
// --- Filled on an Event-by-event basis
//------------------------------------------------
   fHistV0MultiplicityBeforeTrigSel(0),
   fHistV0MultiplicityForTrigEvt(0),
   fHistV0MultiplicityForSelEvt(0),
   fHistV0MultiplicityForSelEvtNoTPCOnly(0),
   fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup(0),
   fHistMultiplicityBeforeTrigSel(0),
   fHistMultiplicityForTrigEvt(0),
   fHistMultiplicity(0),
   fHistMultiplicityNoTPCOnly(0),
   fHistMultiplicityNoTPCOnlyNoPileup(0),
   fHistPVx(0),
   fHistPVy(0),
   fHistPVz(0),
   fHistPVxAnalysis(0),
   fHistPVyAnalysis(0),
   fHistPVzAnalysis(0),
   fHistSwappedV0Counter(0)
{
  // Constructor
  // Output slot #0 writes into a TList container (Lambda Histos and fTree)
   DefineOutput(1, TList::Class());
   DefineOutput(2, TTree::Class());
}


AliAnalysisTaskExtractV0AOD::~AliAnalysisTaskExtractV0AOD()
{
//------------------------------------------------
// DESTRUCTOR
//------------------------------------------------

   if (fListHistV0){
      delete fListHistV0;
      fListHistV0 = 0x0;
   }
   if (fTree){
      delete fTree;
      fTree = 0x0;
   }
}



//________________________________________________________________________
void AliAnalysisTaskExtractV0AOD::UserCreateOutputObjects()
{

   //Create File-resident Tree, please.
   OpenFile(2);
   // Called once
   fTree = new TTree("fTree","V0Candidates");

//------------------------------------------------
// fTree Branch definitions
//------------------------------------------------

//-----------BASIC-INFO---------------------------
/* 1*/  fTree->Branch("fTreeVariableChi2V0",&fTreeVariableChi2V0,"fTreeVariableChi2V0/F");
/* 2*/  fTree->Branch("fTreeVariableDcaV0Daughters",&fTreeVariableDcaV0Daughters,"fTreeVariableDcaV0Daughters/F");
/* 3*/	fTree->Branch("fTreeVariableDcaPosToPrimVertex",&fTreeVariableDcaPosToPrimVertex,"fTreeVariableDcaPosToPrimVertex/F");
/* 4*/	fTree->Branch("fTreeVariableDcaNegToPrimVertex",&fTreeVariableDcaNegToPrimVertex,"fTreeVariableDcaNegToPrimVertex/F");
/* 5*/	fTree->Branch("fTreeVariableV0Radius",&fTreeVariableV0Radius,"fTreeVariableV0Radius/F");
/* 6*/	fTree->Branch("fTreeVariablePt",&fTreeVariablePt,"fTreeVariablePt/F");
/* 7*/	fTree->Branch("fTreeVariableRapK0Short",&fTreeVariableRapK0Short,"fTreeVariableRapK0Short/F");
/* 8*/	fTree->Branch("fTreeVariableRapLambda",&fTreeVariableRapLambda,"fTreeVariableRapLambda/F");
/* 9*/	fTree->Branch("fTreeVariableInvMassK0s",&fTreeVariableInvMassK0s,"fTreeVariableInvMassK0s/F");
/*10*/	fTree->Branch("fTreeVariableInvMassLambda",&fTreeVariableInvMassLambda,"fTreeVariableInvMassLambda/F");
/*11*/	fTree->Branch("fTreeVariableInvMassAntiLambda",&fTreeVariableInvMassAntiLambda,"fTreeVariableInvMassAntiLambda/F");
/*12*/	fTree->Branch("fTreeVariableV0CosineOfPointingAngle",&fTreeVariableV0CosineOfPointingAngle,"fTreeVariableV0CosineOfPointingAngle/F");
/*13*/	fTree->Branch("fTreeVariableAlphaV0",&fTreeVariableAlphaV0,"fTreeVariableAlphaV0/F");
/*14*/	fTree->Branch("fTreeVariablePtArmV0",&fTreeVariablePtArmV0,"fTreeVariablePtArmV0/F");
/*15*/	fTree->Branch("fTreeVariableLeastNbrCrossedRows",&fTreeVariableLeastNbrCrossedRows,"fTreeVariableLeastNbrCrossedRows/I");
/*16*/	fTree->Branch("fTreeVariableLeastRatioCrossedRowsOverFindable",&fTreeVariableLeastRatioCrossedRowsOverFindable,"fTreeVariableLeastRatioCrossedRowsOverFindable/F");
//-----------MULTIPLICITY-INFO--------------------
/*17*/	fTree->Branch("fTreeVariableMultiplicity",&fTreeVariableMultiplicity,"fTreeVariableMultiplicity/I");
//------------------------------------------------
/*18*/	fTree->Branch("fTreeVariableDistOverTotMom",&fTreeVariableDistOverTotMom,"fTreeVariableDistOverTotMom/F");
/*19*/	fTree->Branch("fTreeVariableNSigmasPosProton",&fTreeVariableNSigmasPosProton,"fTreeVariableNSigmasPosProton/F");
/*20*/	fTree->Branch("fTreeVariableNSigmasPosPion",&fTreeVariableNSigmasPosPion,"fTreeVariableNSigmasPosPion/F");
/*21*/	fTree->Branch("fTreeVariableNSigmasNegProton",&fTreeVariableNSigmasNegProton,"fTreeVariableNSigmasNegProton/F");
/*22*/	fTree->Branch("fTreeVariableNSigmasNegPion",&fTreeVariableNSigmasNegPion,"fTreeVariableNSigmasNegPion/F");
/*23*/	fTree->Branch("fTreeVariableNegEta",&fTreeVariableNegEta,"fTreeVariableNegEta/F");
/*24*/	fTree->Branch("fTreeVariablePosEta",&fTreeVariablePosEta,"fTreeVariablePosEta/F");
/*25*/	fTree->Branch("fTreeVariableRunNumber",&fTreeVariableRunNumber,"fTreeVariableRunNumber/I");
/*26*/	fTree->Branch("fTreeVariableEventNumber",&fTreeVariableEventNumber,"fTreeVariableEventNumber/l");

//------------------------------------------------
// Particle Identification Setup
//------------------------------------------------

   AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
   AliInputEventHandler* inputHandler = (AliInputEventHandler*) (man->GetInputEventHandler());
   fPIDResponse = inputHandler->GetPIDResponse();

//Skipped. Will use Local setup.

//------------------------------------------------
// V0 Multiplicity Histograms
//------------------------------------------------

   // Create histograms
   //Create File-resident Tree, please.
   OpenFile(1);

   fListHistV0 = new TList();
   fListHistV0->SetOwner();  // See http://root.cern.ch/root/html/TCollection.html#TCollection:SetOwner

   if(! fHistV0MultiplicityBeforeTrigSel) {
      fHistV0MultiplicityBeforeTrigSel = new TH1F("fHistV0MultiplicityBeforeTrigSel", 
         "V0s per event (before Trig. Sel.);Nbr of V0s/Evt;Events", 
         25, 0, 25);
      fListHistV0->Add(fHistV0MultiplicityBeforeTrigSel);
   }
           
   if(! fHistV0MultiplicityForTrigEvt) {
      fHistV0MultiplicityForTrigEvt = new TH1F("fHistV0MultiplicityForTrigEvt", 
         "V0s per event (for triggered evt);Nbr of V0s/Evt;Events", 
         25, 0, 25);
      fListHistV0->Add(fHistV0MultiplicityForTrigEvt);
   }

   if(! fHistV0MultiplicityForSelEvt) {
      fHistV0MultiplicityForSelEvt = new TH1F("fHistV0MultiplicityForSelEvt", 
         "V0s per event;Nbr of V0s/Evt;Events", 
         25, 0, 25);
      fListHistV0->Add(fHistV0MultiplicityForSelEvt);
   }

   if(! fHistV0MultiplicityForSelEvtNoTPCOnly) {
      fHistV0MultiplicityForSelEvtNoTPCOnly = new TH1F("fHistV0MultiplicityForSelEvtNoTPCOnly", 
         "V0s per event;Nbr of V0s/Evt;Events", 
         25, 0, 25);
      fListHistV0->Add(fHistV0MultiplicityForSelEvtNoTPCOnly);
   }

   if(! fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup) {
      fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup = new TH1F("fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup", 
         "V0s per event;Nbr of V0s/Evt;Events", 
         25, 0, 25);
      fListHistV0->Add(fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup);
   }

//------------------------------------------------
// Track Multiplicity Histograms
//------------------------------------------------

   if(! fHistMultiplicityBeforeTrigSel) {
      fHistMultiplicityBeforeTrigSel = new TH1F("fHistMultiplicityBeforeTrigSel", 
         "Tracks per event;Nbr of Tracks;Events", 
         200, 0, 200); 		
      fListHistV0->Add(fHistMultiplicityBeforeTrigSel);
   }
   if(! fHistMultiplicityForTrigEvt) {
      fHistMultiplicityForTrigEvt = new TH1F("fHistMultiplicityForTrigEvt", 
         "Tracks per event;Nbr of Tracks;Events", 
         200, 0, 200); 		
      fListHistV0->Add(fHistMultiplicityForTrigEvt);
   }
   if(! fHistMultiplicity) {
      fHistMultiplicity = new TH1F("fHistMultiplicity", 
         "Tracks per event;Nbr of Tracks;Events", 
         200, 0, 200); 		
      fListHistV0->Add(fHistMultiplicity);
   }
   if(! fHistMultiplicityNoTPCOnly) {
      fHistMultiplicityNoTPCOnly = new TH1F("fHistMultiplicityNoTPCOnly", 
         "Tracks per event;Nbr of Tracks;Events", 
         200, 0, 200); 		
      fListHistV0->Add(fHistMultiplicityNoTPCOnly);
   }
   if(! fHistMultiplicityNoTPCOnlyNoPileup) {
      fHistMultiplicityNoTPCOnlyNoPileup = new TH1F("fHistMultiplicityNoTPCOnlyNoPileup", 
         "Tracks per event;Nbr of Tracks;Events", 
         200, 0, 200); 		
      fListHistV0->Add(fHistMultiplicityNoTPCOnlyNoPileup);
   }

   if(! fHistPVx) {
      fHistPVx = new TH1F("fHistPVx", 
         "PV x position;Nbr of Evts;x", 
         2000, -0.5, 0.5); 		
      fListHistV0->Add(fHistPVx);
   }
   if(! fHistPVy) {
      fHistPVy = new TH1F("fHistPVy", 
         "PV y position;Nbr of Evts;y", 
         2000, -0.5, 0.5); 		
      fListHistV0->Add(fHistPVy);
   }
   if(! fHistPVz) {
      fHistPVz = new TH1F("fHistPVz", 
         "PV z position;Nbr of Evts;z", 
         400, -20, 20); 		
      fListHistV0->Add(fHistPVz);
   }
   if(! fHistPVxAnalysis) {
      fHistPVxAnalysis = new TH1F("fHistPVxAnalysis", 
         "PV x position;Nbr of Evts;x", 
         2000, -0.5, 0.5);		
      fListHistV0->Add(fHistPVxAnalysis);
   }
   if(! fHistPVyAnalysis) {
      fHistPVyAnalysis = new TH1F("fHistPVyAnalysis", 
         "PV y position;Nbr of Evts;y", 
         2000, -0.5, 0.5);		
      fListHistV0->Add(fHistPVyAnalysis);
   }
   if(! fHistPVzAnalysis) {
      fHistPVzAnalysis = new TH1F("fHistPVzAnalysis", 
         "PV z position;Nbr of Evts;z", 
         400, -20, 20); 		
      fListHistV0->Add(fHistPVzAnalysis);
   }
   if(! fHistSwappedV0Counter) {
      fHistSwappedV0Counter = new TH1F("fHistSwappedV0Counter", 
         "Swap or not histo;Swapped (1) or not (0); count", 
         2, 0, 2); 		
      fListHistV0->Add(fHistSwappedV0Counter);
   }
   //Regular output: Histograms
   PostData(1, fListHistV0);
   //TTree Object: Saved to base directory. Should cache to disk while saving. 
   //(Important to avoid excessive memory usage, particularly when merging)
   PostData(2, fTree);

}// end UserCreateOutputObjects


//________________________________________________________________________
void AliAnalysisTaskExtractV0AOD::UserExec(Option_t *) 
{

   // Main loop
   // Called for each event
   //gObjectTable->Print();
   AliAODEvent *lAODevent = 0x0;

   //AliAODEvent *lAODevent = 0x0;
   Int_t    nV0s                        = -1;

   Double_t lBestPrimaryVtxPos[3]          = {-100.0, -100.0, -100.0};
   Double_t lMagneticField                 = -10.;
	
   // Connect to the InputEvent	
   // After these lines, we should have an ESD/AOD event + the number of cascades in it.

   // Appropriate for ESD analysis! 
		
   lAODevent = dynamic_cast<AliAODEvent*>( InputEvent() );
   if (!lAODevent) {
      AliWarning("ERROR: lAODevent not available \n");
      return;
   }
   fTreeVariableRunNumber = lAODevent->GetRunNumber();
   fTreeVariableEventNumber =  
    ( ( ((ULong64_t)lAODevent->GetPeriodNumber() ) << 36 ) |
      ( ((ULong64_t)lAODevent->GetOrbitNumber () ) << 12 ) |
        ((ULong64_t)lAODevent->GetBunchCrossNumber() )  );

   //REVISED multiplicity estimator after 'multiplicity day' (2011)
   Int_t lMultiplicity = -100; 

   if(fkIsNuclear == kFALSE) lMultiplicity = -1;

   //---> If this is a nuclear collision, then go nuclear on "multiplicity" variable...
   //---> Warning: Experimental
   if(fkIsNuclear == kTRUE){ 
      AliCentrality* centrality;
      centrality = lAODevent->GetCentrality();
      lMultiplicity = ( ( Int_t ) ( centrality->GetCentralityPercentile( "V0M" ) ) );
      if (centrality->GetQuality()>1) {
        PostData(1, fListHistV0);
        PostData(2, fTree);
        return;
      }
   }

   //Set variable for filling tree afterwards!
   //---> pp case......: GetReferenceMultiplicity
   //---> Pb-Pb case...: Centrality by V0M
   fTreeVariableMultiplicity = lMultiplicity;
   fHistMultiplicityBeforeTrigSel->Fill ( lMultiplicity );
   fHistV0MultiplicityBeforeTrigSel->Fill ( lAODevent->GetNumberOfV0s() );

//------------------------------------------------
// Physics Selection
//------------------------------------------------

// new method        
   UInt_t maskIsSelected = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
   Bool_t isSelected = 0;
   //kMB: default selection, also if fTriggerMask is something not understood...
   isSelected = (maskIsSelected & AliVEvent::kMB) == AliVEvent::kMB;

   if( fTriggerMask == "kINT7" )
     isSelected = (maskIsSelected & AliVEvent::kINT7) == AliVEvent::kINT7;
   if( fTriggerMask == "kINT8" )
     isSelected = (maskIsSelected & AliVEvent::kINT8) == AliVEvent::kINT8;
   if( fTriggerMask == "kAnyINT" )
     isSelected = (maskIsSelected & AliVEvent::kAnyINT) == AliVEvent::kAnyINT;

   //pp at 2.76TeV: special case, ignore FastOnly
   if ( (fkLowEnergyPP == kTRUE) && (maskIsSelected& AliVEvent::kFastOnly) ){
      PostData(1, fListHistV0);
      PostData(2, fTree);
      return;
   } 
   //Standard Min-Bias Selection
   if ( ! isSelected ) { 
      PostData(1, fListHistV0);
      PostData(2, fTree);
      return;
   }

//------------------------------------------------
// After Trigger Selection
//------------------------------------------------

	nV0s = lAODevent->GetNumberOfV0s();

  fHistV0MultiplicityForTrigEvt     ->Fill( nV0s       );
  fHistMultiplicityForTrigEvt       ->Fill( lMultiplicity  );

//------------------------------------------------
// Getting: Primary Vertex + MagField Info
//------------------------------------------------
        
	const AliAODVertex *lPrimaryBestAODVtx = lAODevent->GetPrimaryVertex();	
  // get the best primary vertex available for the event
	// As done in AliCascadeVertexer, we keep the one which is the best one available.
	// between : Tracking vertex > SPD vertex > TPC vertex > default SPD vertex
	// This one will be used for next calculations (DCA essentially)
   lPrimaryBestAODVtx->GetXYZ( lBestPrimaryVtxPos );

   Double_t tPrimaryVtxPosition[3];
   const AliVVertex *primaryVtx = lAODevent->GetPrimaryVertex();
   tPrimaryVtxPosition[0] = primaryVtx->GetX();
   tPrimaryVtxPosition[1] = primaryVtx->GetY();
   tPrimaryVtxPosition[2] = primaryVtx->GetZ();
   fHistPVx->Fill( tPrimaryVtxPosition[0] );
   fHistPVy->Fill( tPrimaryVtxPosition[1] );
   fHistPVz->Fill( tPrimaryVtxPosition[2] );

//------------------------------------------------
// Primary Vertex Z position: SKIP
//------------------------------------------------

   if(TMath::Abs(lBestPrimaryVtxPos[2]) > 10.0 ) { 
      AliWarning("Pb / | Z position of Best Prim Vtx | > 10.0 cm ... return !"); 
      PostData(1, fListHistV0);
      PostData(2, fTree);
      return; 
   }

   lMagneticField = lAODevent->GetMagneticField( );
   fHistV0MultiplicityForSelEvt ->Fill( nV0s );
   fHistMultiplicity->Fill(lMultiplicity);

//------------------------------------------------
// Only look at events with well-established PV
//------------------------------------------------
	
   const AliAODVertex *lPrimaryTrackingAODVtxCheck = lAODevent->GetPrimaryVertex();
   const AliAODVertex *lPrimarySPDVtx = lAODevent->GetPrimaryVertexSPD();
   if (!lPrimarySPDVtx && !lPrimaryTrackingAODVtxCheck ){
      AliWarning("Pb / No SPD prim. vertex nor prim. Tracking vertex ... return !");
      PostData(1, fListHistV0);
      PostData(2, fTree);
      return;
   }

   fHistV0MultiplicityForSelEvtNoTPCOnly ->Fill( nV0s );
   fHistMultiplicityNoTPCOnly->Fill(lMultiplicity);

//------------------------------------------------
// Pileup Rejection
//------------------------------------------------

   // FIXME : quality selection regarding pile-up rejection 
   if(lAODevent->IsPileupFromSPD() && !fkIsNuclear){// minContributors=3, minZdist=0.8, nSigmaZdist=3., nSigmaDiamXY=2., nSigmaDiamZ=5.  -> see http://alisoft.cern.ch/viewvc/trunk/STEER/AliESDEvent.h?root=AliRoot&r1=41914&r2=42199&pathrev=42199
      PostData(1, fListHistV0);
      PostData(2, fTree); 
      return;
   }
   fHistV0MultiplicityForSelEvtNoTPCOnlyNoPileup ->Fill( nV0s );
   fHistMultiplicityNoTPCOnlyNoPileup->Fill(lMultiplicity);
  
//------------------------------------------------
// MAIN LAMBDA LOOP STARTS HERE
//------------------------------------------------

   //if( ! (lAODevent->GetPrimaryVertex()->IsFromVertexerZ() )	 ){ 
      fHistPVxAnalysis->Fill( tPrimaryVtxPosition[0] );
      fHistPVyAnalysis->Fill( tPrimaryVtxPosition[1] );
      fHistPVzAnalysis->Fill( tPrimaryVtxPosition[2] );
   //}

//Variable definition
   Int_t    lOnFlyStatus = 0;// nv0sOn = 0, nv0sOff = 0;
   Double_t lChi2V0 = 0;
   Double_t lDcaV0Daughters = 0, lDcaV0ToPrimVertex = 0;
   Double_t lDcaPosToPrimVertex = 0, lDcaNegToPrimVertex = 0;
   Double_t lV0CosineOfPointingAngle = 0;
   Double_t lV0Radius = 0, lPt = 0;
   Double_t lRapK0Short = 0, lRapLambda = 0;
   Double_t lInvMassK0s = 0, lInvMassLambda = 0, lInvMassAntiLambda = 0;
   Double_t lAlphaV0 = 0, lPtArmV0 = 0;

   Double_t fMinV0Pt = 0; 
   Double_t fMaxV0Pt = 100; 

   Int_t nv0s = 0;
   nv0s = lAODevent->GetNumberOfV0s();

   //for (Int_t iV0 = 0; iV0 < nv0s; iV0++) 
   for (Int_t iV0 = 0; iV0 < nv0s; iV0++) //extra-crazy test
   {// This is the begining of the V0 loop
      AliAODv0 *v0 = lAODevent->GetV0(iV0);
      if (!v0) continue;

      //Obsolete at AOD level... 
      //---> Fix On-the-Fly candidates, count how many swapped
      //if( v0->GetParamN()->Charge() > 0 && v0->GetParamP()->Charge() < 0 ){
      //  fHistSwappedV0Counter -> Fill( 1 );
      //}else{
      //  fHistSwappedV0Counter -> Fill( 0 ); 
      //}
      //if ( fkUseOnTheFly ) CheckChargeV0(v0); 
      
      Double_t tDecayVertexV0[3]; v0->GetXYZ(tDecayVertexV0); 
      Double_t tV0mom[3];
      v0->GetPxPyPz( tV0mom ); 
      Double_t lV0TotalMomentum = TMath::Sqrt(
      tV0mom[0]*tV0mom[0]+tV0mom[1]*tV0mom[1]+tV0mom[2]*tV0mom[2] );

      lV0Radius = TMath::Sqrt(tDecayVertexV0[0]*tDecayVertexV0[0]+tDecayVertexV0[1]*tDecayVertexV0[1]);
      lPt = v0->Pt();
      lRapK0Short = v0->RapK0Short();
      lRapLambda  = v0->RapLambda();
      if ((lPt<fMinV0Pt)||(fMaxV0Pt<lPt)) continue;

      //UInt_t lKeyPos = (UInt_t)TMath::Abs(v0->GetPosID());
      //UInt_t lKeyNeg = (UInt_t)TMath::Abs(v0->GetPosID());

      Double_t lMomPos[3]; //v0->GetPPxPyPz(lMomPos[0],lMomPos[1],lMomPos[2]);
      Double_t lMomNeg[3]; //v0->GetNPxPyPz(lMomNeg[0],lMomNeg[1],lMomNeg[2]);
      lMomPos[0] = v0->MomPosX();
      lMomPos[1] = v0->MomPosY();
      lMomPos[2] = v0->MomPosZ();
      lMomNeg[0] = v0->MomNegX();
      lMomNeg[1] = v0->MomNegY();
      lMomNeg[2] = v0->MomNegZ();

      AliAODTrack *pTrack=(AliAODTrack *)v0->GetDaughter(0); //0->Positive Daughter
      AliAODTrack *nTrack=(AliAODTrack *)v0->GetDaughter(1); //1->Negative Daughter
      if (!pTrack || !nTrack) {
         Printf("ERROR: Could not retreive one of the daughter track");
         continue;
      }

      //Daughter Eta for Eta selection, afterwards
      fTreeVariableNegEta = nTrack->Eta();
      fTreeVariablePosEta = pTrack->Eta();

      // Filter like-sign V0 (next: add counter and distribution)
      if ( pTrack->Charge() == nTrack->Charge()){
         continue;
      } 

      //Quick test this far! 
      

      //________________________________________________________________________
      // Track quality cuts 
      Float_t lPosTrackCrossedRows = pTrack->GetTPCClusterInfo(2,1);
      Float_t lNegTrackCrossedRows = nTrack->GetTPCClusterInfo(2,1);
      fTreeVariableLeastNbrCrossedRows = (Int_t) lPosTrackCrossedRows;
      if( lNegTrackCrossedRows < fTreeVariableLeastNbrCrossedRows )
         fTreeVariableLeastNbrCrossedRows = (Int_t) lNegTrackCrossedRows;

      // TPC refit condition (done during reconstruction for Offline but not for On-the-fly)
      if( !(pTrack->GetStatus() & AliESDtrack::kTPCrefit)) continue;
      if( !(nTrack->GetStatus() & AliESDtrack::kTPCrefit)) continue;

      if ( ( ( pTrack->GetTPCClusterInfo(2,1) ) < 70 ) || ( ( nTrack->GetTPCClusterInfo(2,1) ) < 70 ) ) continue;
	
      //GetKinkIndex condition
      //if( pTrack->GetKinkIndex(0)>0 || nTrack->GetKinkIndex(0)>0 ) continue;

      //Findable clusters > 0 condition
      if( pTrack->GetTPCNclsF()<=0 || nTrack->GetTPCNclsF()<=0 ) continue;

      //Compute ratio Crossed Rows / Findable clusters
      //Note: above test avoids division by zero! 
      Float_t lPosTrackCrossedRowsOverFindable = lPosTrackCrossedRows / ((double)(pTrack->GetTPCNclsF())); 
      Float_t lNegTrackCrossedRowsOverFindable = lNegTrackCrossedRows / ((double)(nTrack->GetTPCNclsF())); 

      fTreeVariableLeastRatioCrossedRowsOverFindable = lPosTrackCrossedRowsOverFindable;
      if( lNegTrackCrossedRowsOverFindable < fTreeVariableLeastRatioCrossedRowsOverFindable )
         fTreeVariableLeastRatioCrossedRowsOverFindable = lNegTrackCrossedRowsOverFindable;

      //Lowest Cut Level for Ratio Crossed Rows / Findable = 0.8, set here
      if ( fTreeVariableLeastRatioCrossedRowsOverFindable < 0.8 ) continue;

      //End track Quality Cuts
      //________________________________________________________________________

      
      //ESD code: obsolete 
      //lDcaPosToPrimVertex = TMath::Abs(pTrack->GetD(tPrimaryVtxPosition[0],
			//				tPrimaryVtxPosition[1],
			//				lMagneticField) );

      //lDcaNegToPrimVertex = TMath::Abs(nTrack->GetD(tPrimaryVtxPosition[0],
			//				tPrimaryVtxPosition[1],
			//				lMagneticField) );

      lDcaPosToPrimVertex = v0->DcaPosToPrimVertex();
      lDcaNegToPrimVertex = v0->DcaNegToPrimVertex();

        
      lOnFlyStatus = v0->GetOnFlyStatus();
      lChi2V0 = v0->Chi2V0();
      lDcaV0Daughters = v0->DcaV0Daughters();
      lDcaV0ToPrimVertex = v0->DcaV0ToPrimVertex();
      lV0CosineOfPointingAngle = v0->CosPointingAngle(tPrimaryVtxPosition);
      fTreeVariableV0CosineOfPointingAngle=lV0CosineOfPointingAngle;

      // Getting invariant mass infos directly from ESD
      lInvMassK0s        = v0->MassK0Short();
      lInvMassLambda     = v0->MassLambda();
      lInvMassAntiLambda = v0->MassAntiLambda();
      lAlphaV0 = v0->AlphaV0();
      lPtArmV0 = v0->PtArmV0();

      fTreeVariablePt = v0->Pt();
      fTreeVariableChi2V0 = lChi2V0; 
      fTreeVariableDcaV0ToPrimVertex = lDcaV0ToPrimVertex;
      fTreeVariableDcaV0Daughters = lDcaV0Daughters;
      fTreeVariableV0CosineOfPointingAngle = lV0CosineOfPointingAngle; 
      fTreeVariableV0Radius = lV0Radius;
      fTreeVariableDcaPosToPrimVertex = lDcaPosToPrimVertex;
      fTreeVariableDcaNegToPrimVertex = lDcaNegToPrimVertex;
      fTreeVariableInvMassK0s = lInvMassK0s;
      fTreeVariableInvMassLambda = lInvMassLambda;
      fTreeVariableInvMassAntiLambda = lInvMassAntiLambda;
      fTreeVariableRapK0Short = lRapK0Short;
      fTreeVariableRapLambda = lRapLambda;
      fTreeVariableAlphaV0 = lAlphaV0;
      fTreeVariablePtArmV0 = lPtArmV0;

      //Official means of acquiring N-sigmas 
      fTreeVariableNSigmasPosProton = fPIDResponse->NumberOfSigmasTPC( pTrack, AliPID::kProton );
      fTreeVariableNSigmasPosPion   = fPIDResponse->NumberOfSigmasTPC( pTrack, AliPID::kPion );
      fTreeVariableNSigmasNegProton = fPIDResponse->NumberOfSigmasTPC( nTrack, AliPID::kProton );
      fTreeVariableNSigmasNegPion   = fPIDResponse->NumberOfSigmasTPC( nTrack, AliPID::kPion );

//This requires an Invariant Mass Hypothesis afterwards
      fTreeVariableDistOverTotMom = TMath::Sqrt(
						TMath::Power( tDecayVertexV0[0] - lBestPrimaryVtxPos[0] , 2) +
						TMath::Power( tDecayVertexV0[1] - lBestPrimaryVtxPos[1] , 2) +
						TMath::Power( tDecayVertexV0[2] - lBestPrimaryVtxPos[2] , 2)
					);
      fTreeVariableDistOverTotMom /= (lV0TotalMomentum+1e-10); //avoid division by zero, to be sure

//------------------------------------------------
// Fill Tree! 
//------------------------------------------------

// The conditionals are meant to decrease excessive
// memory usage! 

//First Selection: Reject OnFly
      if( (lOnFlyStatus == 0 && fkUseOnTheFly == kFALSE) || (lOnFlyStatus != 0 && fkUseOnTheFly == kTRUE ) ){
         //Second Selection: rough 20-sigma band, parametric. 
         //K0Short: Enough to parametrize peak broadening with linear function.    
         Double_t lUpperLimitK0Short = (5.63707e-01) + (1.14979e-02)*fTreeVariablePt; 
         Double_t lLowerLimitK0Short = (4.30006e-01) - (1.10029e-02)*fTreeVariablePt;
         //Lambda: Linear (for higher pt) plus exponential (for low-pt broadening)
         //[0]+[1]*x+[2]*TMath::Exp(-[3]*x)
         Double_t lUpperLimitLambda = (1.13688e+00) + (5.27838e-03)*fTreeVariablePt + (8.42220e-02)*TMath::Exp(-(3.80595e+00)*fTreeVariablePt); 
         Double_t lLowerLimitLambda = (1.09501e+00) - (5.23272e-03)*fTreeVariablePt - (7.52690e-02)*TMath::Exp(-(3.46339e+00)*fTreeVariablePt);
         //Do Selection      
         if( (fTreeVariableInvMassLambda     < lUpperLimitLambda  && fTreeVariableInvMassLambda     > lLowerLimitLambda     ) || 
             (fTreeVariableInvMassAntiLambda < lUpperLimitLambda  && fTreeVariableInvMassAntiLambda > lLowerLimitLambda     ) || 
             (fTreeVariableInvMassK0s        < lUpperLimitK0Short && fTreeVariableInvMassK0s        > lLowerLimitK0Short    ) ){
             //Pre-selection in case this is AA...
             //if( fkIsNuclear == kFALSE ) fTree->Fill();
             //if( fkIsNuclear == kTRUE){ 
             //If this is a nuclear collision___________________
             // ... pre-filter with TPC, daughter eta selection
               //if( (fTreeVariableInvMassLambda     < lUpperLimitLambda  && fTreeVariableInvMassLambda     > lLowerLimitLambda 
               //       && TMath::Abs(fTreeVariableNSigmasPosProton) < 6.0 && TMath::Abs(fTreeVariableNSigmasNegPion) < 6.0 ) || 
               //    (fTreeVariableInvMassAntiLambda < lUpperLimitLambda  && fTreeVariableInvMassAntiLambda > lLowerLimitLambda 
               //       && TMath::Abs(fTreeVariableNSigmasNegProton) < 6.0 && TMath::Abs(fTreeVariableNSigmasPosPion) < 6.0 ) ||  
               //    (fTreeVariableInvMassK0s        < lUpperLimitK0Short && fTreeVariableInvMassK0s        > lLowerLimitK0Short 
               //       && TMath::Abs(fTreeVariableNSigmasNegPion)   < 6.0 && TMath::Abs(fTreeVariableNSigmasPosPion) < 6.0 ) ){
                  //insane test
                  if ( TMath::Abs(fTreeVariableNegEta)<0.8 && TMath::Abs(fTreeVariablePosEta)<0.8 ) fTree->Fill();
               //}
             //}//end nuclear_____________________________________
         }
      }

//------------------------------------------------
// Fill tree over.
//------------------------------------------------

   }// This is the end of the V0 loop

  // Post output data.
    PostData(1, fListHistV0);
    PostData(2, fTree);


}

//________________________________________________________________________
void AliAnalysisTaskExtractV0AOD::Terminate(Option_t *)
{
   // Draw result to the screen
   // Called once at the end of the query
   // This will draw the V0 candidate multiplicity, whose 
   // number of entries corresponds to the number of triggered events. 
   TList *cRetrievedList = 0x0;
   cRetrievedList = (TList*)GetOutputData(1);
   if(!cRetrievedList){
      Printf("ERROR - AliAnalysisTaskExtractV0AOD : ouput data container list not available\n");
      return;
   }		
   fHistV0MultiplicityForTrigEvt = dynamic_cast<TH1F*> (  cRetrievedList->FindObject("fHistV0MultiplicityForTrigEvt")  );
   if (!fHistV0MultiplicityForTrigEvt) {
      Printf("ERROR - AliAnalysisTaskExtractV0AOD : fHistV0MultiplicityForTrigEvt not available");
      return;
   }
   TCanvas *canCheck = new TCanvas("AliAnalysisTaskExtractV0AOD","V0 Multiplicity",10,10,510,510);
   canCheck->cd(1)->SetLogy();
   fHistV0MultiplicityForTrigEvt->SetMarkerStyle(22);
   fHistV0MultiplicityForTrigEvt->DrawCopy("E");
}

//________________________________________________________________________
void AliAnalysisTaskExtractV0AOD::CheckChargeV0(AliESDv0 *v0)
{
   // This function checks charge of negative and positive daughter tracks. 
   // If incorrectly defined (onfly vertexer), swaps out. 
   if( v0->GetParamN()->Charge() > 0 && v0->GetParamP()->Charge() < 0 ){
      //V0 daughter track swapping is required! Note: everything is swapped here... P->N, N->P
      Long_t lCorrectNidx = v0->GetPindex();
      Long_t lCorrectPidx = v0->GetNindex();
      Double32_t	lCorrectNmom[3];
      Double32_t	lCorrectPmom[3];
      v0->GetPPxPyPz( lCorrectNmom[0], lCorrectNmom[1], lCorrectNmom[2] );
      v0->GetNPxPyPz( lCorrectPmom[0], lCorrectPmom[1], lCorrectPmom[2] );

      AliExternalTrackParam	lCorrectParamN(
        v0->GetParamP()->GetX() , 
        v0->GetParamP()->GetAlpha() , 
        v0->GetParamP()->GetParameter() , 
        v0->GetParamP()->GetCovariance() 
      );
      AliExternalTrackParam	lCorrectParamP(
        v0->GetParamN()->GetX() , 
        v0->GetParamN()->GetAlpha() , 
        v0->GetParamN()->GetParameter() , 
        v0->GetParamN()->GetCovariance() 
      );
      lCorrectParamN.SetMostProbablePt( v0->GetParamP()->GetMostProbablePt() );
      lCorrectParamP.SetMostProbablePt( v0->GetParamN()->GetMostProbablePt() );

      //Get Variables___________________________________________________
      Double_t lDcaV0Daughters = v0 -> GetDcaV0Daughters();
      Double_t lCosPALocal     = v0 -> GetV0CosineOfPointingAngle(); 
      Bool_t lOnFlyStatusLocal = v0 -> GetOnFlyStatus();

      //Create Replacement Object_______________________________________
      AliESDv0 *v0correct = new AliESDv0(lCorrectParamN,lCorrectNidx,lCorrectParamP,lCorrectPidx);
      v0correct->SetDcaV0Daughters          ( lDcaV0Daughters   );
      v0correct->SetV0CosineOfPointingAngle ( lCosPALocal       );
      v0correct->ChangeMassHypothesis       ( kK0Short          );
      v0correct->SetOnFlyStatus             ( lOnFlyStatusLocal );

      //Reverse Cluster info..._________________________________________
      v0correct->SetClusters( v0->GetClusters( 1 ), v0->GetClusters ( 0 ) );

      *v0 = *v0correct;
      //Proper cleanup..._______________________________________________
      v0correct->Delete();
      v0correct = 0x0;

      //Just another cross-check and output_____________________________
      if( v0->GetParamN()->Charge() > 0 && v0->GetParamP()->Charge() < 0 ) {
        AliWarning("Found Swapped Charges, tried to correct but something FAILED!");
      }else{
        //AliWarning("Found Swapped Charges and fixed.");
      }
      //________________________________________________________________
   }else{
      //Don't touch it! ---
      //Printf("Ah, nice. Charges are already ordered...");
   }
   return;
} 
