void AddTask_dNdPtPbPb_TPCITS(Int_t cutMode =222 , char *particleMode ="default", char* eventTrigger="kINT7"){
  
  TString stEventTrigger(eventTrigger);
  TString stParticleMode(particleMode);
  
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {Error("AddTask_dNdPtPbPb_TPCITS", "No analysis manager found.");return 0;}

  Bool_t hasMC=(AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler()!=0x0);

  // Switch off all AliInfo (too much output!!!)
  AliLog::SetGlobalLogLevel(AliLog::kError);
  mgr->SetDebugLevel(0);

  
  /// Create event cuts  
  Float_t zvWindow = 30. ;

  AlidNdPtEventCuts *evtCuts = new AlidNdPtEventCuts("AlidNdPtEventCuts","Event cuts");
  evtCuts->SetZvRange(-zvWindow,zvWindow);
  evtCuts->SetMeanXYZv(0.0,0.0,0.0);
  evtCuts->SetSigmaMeanXYZv(1.0,1.0,10.0);
  evtCuts->SetTriggerRequired(kTRUE);
  
  // Create geom. acceptance cuts

  Float_t etaWindow = 1. ;
  Float_t ptMin = 0.10;

  AlidNdPtAcceptanceCuts *accCuts = new AlidNdPtAcceptanceCuts("AlidNdPtAcceptanceCuts","Geom. acceptance cuts");
  accCuts->SetEtaRange(-etaWindow,etaWindow);
  accCuts->SetPtRange(ptMin,1.e10);
  
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/SPECTRA/ChargedHadrons/dNdPt/macros/CreatedNdPtTrackCuts.C");
  AliESDtrackCuts* esdTrackCuts = CreatedNdPtTrackCuts(cutMode,hasMC);
  if (!esdTrackCuts) { printf("ERROR: esdTrackCuts could not be created\n"); return; }
  esdTrackCuts->SetHistogramsOn(kTRUE);
  
  // Create task
  AlidNdPtTask *task = new AlidNdPtTask("AlidNdPtTask");
  task->SetUseMCInfo(hasMC);

  // trigger selection: MB
  if(stEventTrigger.Contains("kINT7")) task->SelectCollisionCandidates(AliVEvent::kINT7);
  else if(stEventTrigger.Contains("kMB")) task->SelectCollisionCandidates(AliVEvent::kMB);


  // Create cut analysis object  
  AlidNdPtAnalysisPbPb *fdNdPtAnalysisPbPb = new AlidNdPtAnalysisPbPb("dNdPtAnalysisPbPb","dN/dPt Analysis");
  fdNdPtAnalysisPbPb->SetEventCuts(evtCuts);
  fdNdPtAnalysisPbPb->SetAcceptanceCuts(accCuts);
  fdNdPtAnalysisPbPb->SetTrackCuts(esdTrackCuts);
  fdNdPtAnalysisPbPb->SetAnalysisMode(AlidNdPtHelper::kTPCITS);
  fdNdPtAnalysisPbPb->SetCentralityEstimator("V0M");
  if(stEventTrigger.Contains("kINT7")) fdNdPtAnalysisPbPb->SetTriggerMask(AliVEvent::kINT7);
  else if(stEventTrigger.Contains("kMB")) fdNdPtAnalysisPbPb->SetTriggerMask(AliVEvent::kMB);

  
  // SetParticleMode
  if(stParticleMode.Contains("Pion")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCPion);}
  else if (stParticleMode.Contains("Proton")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCProton);}
  else if (stParticleMode.Contains("Kaon")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCKaon);}
  else if (stParticleMode.Contains("RemainingRest")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCRemainingRest);}
  else if (stParticleMode.Contains("Rest")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCRest);}
  else if (stParticleMode.Contains("SigmaPlus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCSigmaPlus);}
  else if (stParticleMode.Contains("SigmaMinus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCSigmaMinus);}
  else if (stParticleMode.Contains("XiMinus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCXiMinus);}
  else if (stParticleMode.Contains("OmegaMinus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCOmegaMinus);}
  else if (stParticleMode.Contains("Plus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kPlus);}
  else if (stParticleMode.Contains("Minus")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMinus);}
  else if (stParticleMode.Contains("Electron")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCElectron);}
  else if (stParticleMode.Contains("Muon")){fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kMCMuon);}
  else{fdNdPtAnalysisPbPb->SetParticleMode(AlidNdPtHelper::kAllPart);}
  // Change binning
  const Int_t ptNbins = 81;
  Double_t bins[82] = {0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 40.0, 45.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 130.0, 140.0, 150.0, 160.0, 180.0, 200.0};
  Double_t* binsPt = new Double_t[82];
  for (int i=0; i<82; i++) {binsPt[i] = bins[i];}
  fdNdPtAnalysisPbPb->SetBinsPt(ptNbins, binsPt);
  fdNdPtAnalysisPbPb->SetBinsPtCorr(ptNbins, binsPt);  
  fdNdPtAnalysisPbPb->SetUseMCInfo(hasMC);
  fdNdPtAnalysisPbPb->SetHistogramsOn(hasMC);
 
  
  task->AddAnalysisObject( fdNdPtAnalysisPbPb );

  // Add task
  mgr->AddTask(task);


  // Create containers for input
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  mgr->ConnectInput(task, 0, cinput);

  TString stContainerName;
  stContainerName = Form("dNdPt_PbPb_%d",cutMode);
  if(!stParticleMode.Contains("default")) {
    stContainerName = stContainerName + "_" +stParticleMode;
  }
    
  AliAnalysisDataContainer *coutput = mgr->CreateContainer(stContainerName,TList::Class(),AliAnalysisManager::kOutputContainer, mgr->GetCommonFileName());
    
  mgr->ConnectOutput(task, 1, coutput);
}

