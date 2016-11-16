/**************************************************************************
 * Copyright(c) 1998-2016, ALICE Experiment at CERN, All rights reserved. *
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
#include <array>
#include <iostream>
#include <map>
#include <vector>

#include <THistManager.h>
#include <TMath.h>
#include <TString.h>

#include "AliAnalysisDataContainer.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskEmcalTriggerMultiplicity.h"
#include "AliClusterContainer.h"
#include "AliEmcalAnalysisFactory.h"
#include "AliEmcalList.h"
#include "AliEmcalTrackSelection.h"
#include "AliLog.h"
#include "AliVCluster.h"
#include "AliVEvent.h"
#include "AliVEventHandler.h"
#include "AliVMultiplicity.h"
#include "AliVVZERO.h"

/// \cond CLASSIMP
ClassImp(EMCalTriggerPtAnalysis::AliAnalysisTaskEmcalTriggerMultiplicity)
/// \endcond

namespace EMCalTriggerPtAnalysis {

AliAnalysisTaskEmcalTriggerMultiplicity::AliAnalysisTaskEmcalTriggerMultiplicity():
    AliAnalysisTaskEmcalTriggerBase(),
    fTrackSel(nullptr)
{
}

AliAnalysisTaskEmcalTriggerMultiplicity::AliAnalysisTaskEmcalTriggerMultiplicity(const char *name):
    AliAnalysisTaskEmcalTriggerBase(name),
    fTrackSel(nullptr)
{
}


AliAnalysisTaskEmcalTriggerMultiplicity::~AliAnalysisTaskEmcalTriggerMultiplicity() {
  if(fTrackSel) delete fTrackSel;
}

void AliAnalysisTaskEmcalTriggerMultiplicity::CreateUserHistos(){
  std::vector<float> ptthresh = {0.1, 0.5, 1., 2., 5., 10.};
  for(const auto &t : GetSupportedTriggers()){
    // Event counters
    fHistos->CreateTH1(Form("hEventCount%s", t.Data()), Form("Event count trigger class %s", t.Data()), 1, 0.5, 1.5, "s");
    fHistos->CreateTH1(Form("hVertexZ%s", t.Data()), Form("z-distribution of the primary vertex for trigger class %s", t.Data()), 200, -40., 40., "s");

    // Multiplicity distributions
    fHistos->CreateTH1(Form("VZEROAmult%s", t.Data()), Form("VZERO-A multiplicity distribution for trigger class %s", t.Data()), 1000, 0., 1000., "s");
    fHistos->CreateTH1(Form("VZEROCmult%s", t.Data()), Form("VZERO-C multiplicity distribution for trigger class %s", t.Data()), 1000, 0., 1000., "s");
    fHistos->CreateTH1(Form("TrackletMult%s", t.Data()), Form("SPD tracklet multiplcity for trigger class %s", t.Data()), 1000, 0., 1000., "s");
    fHistos->CreateTH1(Form("EMCALClusterMult%s", t.Data()), Form("EMCAL cluster multiplcity for trigger class %s", t.Data()), 1000, 0., 1000., "s");
    for(auto pt : ptthresh){
      fHistos->CreateTH1(Form("TrackMult%d%s", static_cast<int>(pt * 10.), t.Data()), Form("Global track multiplicity for tracks with pt > %1.f GeV/c for trigger class %s", pt, t.Data()), 1000, 0., 1000., "s");
    }
  }
}

void AliAnalysisTaskEmcalTriggerMultiplicity::CreateUserObjects(){

}

void AliAnalysisTaskEmcalTriggerMultiplicity::UserFillHistosAfterEventSelection(){
  for(const auto &t : fSelectedTriggers){
    Double_t weight = GetTriggerWeight(t);
    fHistos->FillTH1(Form("hEventCount%s", t.Data()), 1, weight);
    fHistos->FillTH1(Form("hVertexZ%s", t.Data()), fVertex[2], weight);
  }
}

bool AliAnalysisTaskEmcalTriggerMultiplicity::Run(){
  // Monitor
  // - VZERO_A multiplicity
  // - ITS tracklet multiplicity
  // - global track multiplicity
  // - EMCAL clusters
  AliVVZERO *vzerodata = fInputEvent->GetVZEROData();

  // Track multiplicity
  std::vector<float> ptthresh = {0.1, 0.5, 1., 2., 5., 10.};
  std::map<float, int> trackmult;
  for(auto t : ptthresh) trackmult[t] = 0;
  for(int itrk = 0; itrk < fInputEvent->GetNumberOfTracks(); itrk++){
    AliVTrack *trk = static_cast<AliVTrack *>(fInputEvent->GetTrack(itrk));
    if(TMath::Abs(trk->Eta()) > 0.8) continue;
    if(fTrackSel->IsTrackAccepted(trk)){
      for(auto t : ptthresh){
        if(TMath::Abs(trk->Pt()) > t) trackmult[t]++;
      }
    }
  }

  // Tracklet multiplicity
  int ntracklets(0);
  AliVMultiplicity *mult = fInputEvent->GetMultiplicity();
  for(int itl = 0; itl < mult->GetNumberOfTracklets(); itl++){
    if(TMath::Abs(mult->GetEta(itl))) ntracklets++;
  }

  // EMCAL cluster multiplicity
  // cluster threshold 500 MeV
  Int_t nclusters(0);
  AliClusterContainer *clustercont = this->GetClusterContainer(0);
  for(auto clust : clustercont->all()){
    if(clust->E() > 0.5) nclusters++;
  }

  // all quantities available
  // fill histograms
  for(const auto &t : fSelectedTriggers){
    Double_t weight = GetTriggerWeight(t);
    fHistos->FillTH1(Form("VZEROAmult%s", t.Data()), vzerodata->GetMTotV0A(), weight);
    fHistos->FillTH1(Form("VZEROCmult%s", t.Data()), vzerodata->GetMTotV0C(), weight);
    fHistos->FillTH1(Form("TrackletMult%s", t.Data()), ntracklets, weight);
    fHistos->FillTH1(Form("EMCALClusterMult%s", t.Data()), nclusters, weight);
    for(auto tmult : trackmult) fHistos->FillTH1(Form("TrackMult%d%s", static_cast<int>(tmult.first *10.), t.Data()), tmult.second, weight);
  }

  return true;
}

void AliAnalysisTaskEmcalTriggerMultiplicity::InitializeTrackCuts(const TString &cutname, bool isAOD){
  fTrackSel = AliEmcalAnalysisFactory::TrackCutsFactory(cutname, isAOD);
}

AliAnalysisTaskEmcalTriggerMultiplicity *AliAnalysisTaskEmcalTriggerMultiplicity::AddTaskEmcalTriggerMultiplicity(const TString &nclusters, const TString &suffix){
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr){
    std::cout << "Analysis manager not provided - cannot initialize task" << std::endl;
    return nullptr;
  }

  TString taskname = "EmcalTriggerMultTask_";
  taskname += suffix;
  AliAnalysisTaskEmcalTriggerMultiplicity *task = new AliAnalysisTaskEmcalTriggerMultiplicity(taskname.Data());
  mgr->AddTask(task);

  // Configuring cluster container
  AliClusterContainer *clustercont = task->AddClusterContainer(
      nclusters == "usedefault" ? AliEmcalAnalysisFactory::ClusterContainerNameFactory(mgr->GetInputEventHandler()->InheritsFrom("AliAODInputHandler")) : nclusters
  );
  clustercont->SetClusECut(0.5);

  // Configuring input-/output-container
  TString outputcont("TriggerMultiplicityHistos_"), outputfile(mgr->GetCommonFileName());
  outputcont += suffix;
  outputfile += TString::Format(":TriggerMultiplicity_") + suffix;
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 1, mgr->CreateContainer(outputcont.Data(), AliEmcalList::Class(), AliAnalysisManager::kOutputContainer, outputfile.Data()));

  return task;
}

} /* namespace EMCalTriggerPtAnalysis */
