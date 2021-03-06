AliAnalysisTaskEmcalJetFlavourTagExample* AddTaskEmcalJetFlavourTagExample(
   const char *outfilename    = "AnalysisOutput.root",
   const char *nJets          = "Jets",
   const char *nClusters      = "CaloClustersCorr",
   UInt_t type                = 0, //AliAnalysisTaskEmcal::kTPC,
   const char *nrho      = "rhoChEm",
   const Double_t minPhi      = 1.8,
   const Double_t maxPhi      = 2.74,
   const Double_t minEta      = -0.3,
   const Double_t maxEta      = 0.3,
   const Double_t minArea     = 0.4,
   const char *nPicoTracks        = "PicoTracks",
   const Double_t hiPTjet     = 50.0,
   const Double_t trptcut     = 2.0,
   const Double_t trketa      = 0.9,
   Bool_t   isESD              = 0,
   const char *typeDET         = "EMCAL",
   Double_t    jetradius          = 0.2,
   Double_t    jetptcut           = 1,
   Double_t    jetareacut         = 0.08,
   Int_t       leadhadtype        = 0,
   const char *tag	           = ""
)
{
  // Get the pointer to the existing analysis manager via the static access method.
  //==============================================================================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr)
  {
    ::Error("AddTaskEmcalJetFlavourTagExample", "No analysis manager to connect to.");
    return NULL;
  }

  // Check the analysis type using the event handlers connected to the analysis manager.
  //==============================================================================
  AliVEventHandler *evhand = mgr->GetInputEventHandler();
  //if (!mgr->GetInputEventHandler())
  if (!evhand) {
    Error("AddTaskEmcalJetHadEPpid", "This task requires an input event handler");
    return NULL;
  }
  
  // check on type of event
  TString dType("ESD");
  if (!evhand->InheritsFrom("AliESDInputHandler"))
    dType = "AOD";
  if (dType == "AOD") nTracks = "AODFilterTracks";
  if (dType == "ESD") nTracks = "ESDFilterTracks";
  //ESD Trk Cuts
  //if(isESD > 0){
  AliESDtrackCuts *esdTrackCuts = 0x0;
  gROOT->LoadMacro("$ALICE_ROOT/PWGJE/macros/CreateTrackCutsPWGJE.C");

  //}
  
  
  
  //-------------------------------------------------------
  // Init the task and do settings
  //-------------------------------------------------------

//  TString name(Form("Spectra_%s", nJets));
  TString name(Form("Spectra_%s_%s%s", nJets, nrho, tag));
  AliAnalysisTaskEmcalJetFlavourTagExample *spectratask = new AliAnalysisTaskEmcalJetFlavourTagExample(name);
  spectratask->AddJetContainer(nJets);
  spectratask->AddClusterContainer(nClusters);
  spectratask->SetAnaType(type);
  spectratask->SetRhoName(nrho);
  spectratask->SetJetPhi(minPhi,maxPhi);
  spectratask->SetJetEta(minEta,maxEta);
  spectratask->SetJetAreaCut(minArea);
  spectratask->AddParticleContainer(nTracks);
  spectratask->SetJetPt(hiPTjet); 
  spectratask->SetTrackPtCut(trptcut);
  spectratask->SetTrackEta(trketa);
  spectratask->SetTrackCuts(esdTrackCuts);
  //spectratask->SetDataType(isESD);

  // =================== set up containers ================================================

  
  AliParticleContainer *trackCont  = spectratask->AddParticleContainer(nTracks);
  if(trackCont){
    trackCont->SetClassName("AliVTrack");
    trackCont->SetParticleEtaLimits(-0.9,0.9);
    trackCont->SetParticlePhiLimits(1.4,3.2);
  }
  
  AliClusterContainer *clusterCont = spectratask->AddClusterContainer(nClusters);
  
  AliParticleContainer *trackJetCont  = spectratask->AddParticleContainer(nTracks);
  if(trackJetCont){
    trackJetCont->SetClassName("AliVTrack");
    trackJetCont->SetParticleEtaLimits(-0.9,0.9);
    //trackJetCont->SetParticlePhiLimits(1.4,3.2);
  }
  
  AliClusterContainer *clusterJetCont = spectratask->AddClusterContainer(nClusters);
  

  TString strType(typeDET);
  AliJetContainer *jetCont = spectratask->AddJetContainer(nJets,strType,jetradius);
  if(jetCont) {
    jetCont->SetRhoName(nrho);
    jetCont->ConnectParticleContainer(trackJetCont);
    jetCont->ConnectClusterContainer(clusterJetCont);
    //jetCont->ConnectParticleContainer(trackCont);
    //jetCont->ConnectClusterContainer(clusterCont);
    //jetCont->SetZLeadingCut(0.98,0.98);
    jetCont->SetPercAreaCut(jetareacut); // 0.6
    jetCont->SetJetPtCut(jetptcut);
    //jetCont->SetLeadingHadronType(leadhadtype);
  }

  //-------------------------------------------------------
  // Final settings, pass to manager and set the containers
  //-------------------------------------------------------

  mgr->AddTask(spectratask);

  // Create containers for input/output
  mgr->ConnectInput (spectratask, 0, mgr->GetCommonInputContainer() );
  AliAnalysisDataContainer *cospectra = mgr->CreateContainer(name,
                                                           TList::Class(),
                                                           AliAnalysisManager::kOutputContainer,
                                                           outfilename);
  mgr->ConnectOutput(spectratask,1,cospectra);

  return spectratask;
}

