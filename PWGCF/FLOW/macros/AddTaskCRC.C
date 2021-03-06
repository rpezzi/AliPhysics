AliAnalysisTask * AddTaskCRC(Double_t ptMin=0.2,
                             Double_t ptMax=50.,
                             TString analysisTypeUser="AOD",
                             Int_t AODfilterBit=768,
                             TString sDataSet="2010",
                             TString sIntRuns="low",
                             TString sIntRate="low",
                             TString EvTrigger="MB",
                             Bool_t bUseCRCRecenter=kFALSE,
                             Bool_t bUseZDC=kFALSE,
                             TString ZDCCalibFileName,
                             TString sCorrWeight="TPCmVZuZDCu",
                             Bool_t bCorrectForBadChannel=kFALSE,
                             Bool_t bZDCMCCen=kTRUE,
                             Float_t ZDCGainAlpha=0.395,
                             TString Label="",
                             TString sCentrEstimator="V0",
                             Double_t dVertexRange=10.,
                             Double_t dMinClusTPC=70,
                             Double_t dDCAxy=1000.,
                             Double_t dDCAz=1000.,
                             Double_t MaxChi2PerClTPC=4.,
                             Double_t MaxFracSharedTPCCl=0.4,
                             TString sSelecCharge="",
                             Int_t bCutTPCbound=0,
                             Bool_t bCalculateFlow=kFALSE,
                             Int_t NumCenBins=100,
                             Double_t DeltaEta=0.4,
                             Bool_t bUsePtWeights=kFALSE,
                             TString PtWeightsFileName="",
                             TString sPhiEtaWeight="off",
                             Bool_t bRescaleZDC=kFALSE,
                             Bool_t bSetQAZDC=kFALSE,
                             Int_t MinMulZN=1,
                             TString ZDCESEFileName="",
                             Bool_t bCenFlattening=kTRUE,
                             TString CenWeightsFileName="",
                             const char* suffix="") {
 // load libraries
 gSystem->Load("libGeom");
 gSystem->Load("libVMC");
 gSystem->Load("libXMLIO");
 gSystem->Load("libPhysics");
 gSystem->Load("libCore.so");
 gSystem->Load("libTree.so");
 gSystem->Load("libSTEERBase.so");
 gSystem->Load("libESD.so");
 gSystem->Load("libAOD.so");
 gSystem->Load("libANALYSIS.so");
 gSystem->Load("libANALYSISalice.so");
 gSystem->Load("libOADB.so");
 gSystem->Load("libPWGflowBase.so");
 gSystem->Load("libPWGflowTasks.so");
 
 gROOT->ProcessLine(".include $ALICE_ROOT/include");
 gROOT->ProcessLine(".include $ALICE_PHYSICS/include");
 gSystem->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/EMCAL -I$ALICE_ROOT/ANALYSIS -I$ALICE_ROOT/OCDB -I$ALICE_ROOT/STEER/macros -I$ALICE_ROOT/include -I$ALICE_ROOT/ITS -I$ALICE_ROOT/TPC -I$ALICE_ROOT/TRD -I$ALICE_ROOT/ZDC -I$ALICE_ROOT/macros -I$ALICE_PHYSICS -I$ALICE_PHYSICS/include -I$ALICE_PHYSICS/OADB $ALICE_PHYSICS/OADB/macros -I$ALICE_PHYSICS/PWGGA -I$ALICE_PHYSICS/PWGCF -I$ALICE_PHYSICS/PWGHF -I$ALICE_PHYSICS/TENDER -I$ALICE_PHYSICS/TENDER/Tender -I$ALICE_PHYSICS/TENDER/TenderSupplies -I$ALICE_PHYSICS/PARfiles -I$ALICE_PHYSICS/PWGCF/FLOW/macros I$ALICE_PHYSICS/PWGPP/ZDC -g ");
 
 // the manager is static, so get the existing manager via the static method
 AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
 if (!mgr) {
  printf("No analysis manager to connect to!\n");
  return NULL;
 }
 
 // just to see if all went well, check if the input event handler has been connected
 if (!mgr->GetInputEventHandler()) {
  printf("This task requires an input event handler!\n");
  return NULL;
 }
 
 Int_t nCenBin = 10;
 Double_t centrMin=0.;
 Double_t centrMax=100.;
 Double_t CenBinWidth=10.;
 Int_t CRC2nEtaBins=6;
 Bool_t bCalculateCRC2=kFALSE;
 Float_t MaxDevZN=10.;
 Bool_t bCalculateCRCVZ=kFALSE;
  Bool_t bUsePhiEtaCuts=kFALSE;
 TString PhiEtaWeightsFileName="";
  Bool_t bCutsQA=kTRUE;
  Bool_t bCalculateEbEFlow=kFALSE;
  Bool_t bDivSigma=kFALSE;
  Bool_t bCalculateCRCZDC=kFALSE;
  Bool_t bCalculateCME=kFALSE;
  Bool_t bUseVZERO=kFALSE;
  Int_t nHarmonic=2;
  
 // define CRC suffix
 TString CRCsuffix = ":CRC";
 
 TString CentrName = "_";
 CentrName += (Int_t)centrMin;
 CentrName += "-";
 CentrName += (Int_t)centrMax;
 CRCsuffix += CentrName;
 
 TString pTName = "_";
 Int_t rt = (Int_t)(ptMin*10.);
 Int_t r = (Int_t)(ptMin);
 pTName += ( ptMin < 1. ? Form("0.%i",rt) : Form("%i.%i",r,rt-r*10));
 pTName += "-";
 rt = (Int_t)(ptMax*10.);
 r = (Int_t)(ptMax);
 pTName += ( ptMax < 1. ? Form("0.%i",rt) : Form("%i.%i",r,rt-r*10));
 CRCsuffix += pTName;
 
 if(!Label.EqualTo("")) {
  TString Appendix = "_";
  Appendix += Label;
  CRCsuffix += Appendix;
 }
 
 Double_t etaMin=-0.8;
 Double_t etaMax=0.8;
 
 // create instance of the class: because possible qa plots are added in a second output slot,
 // the flow analysis task must know if you want to save qa plots at the time of class construction
 TString taskFEname = "FlowEventTask";
 taskFEname += CRCsuffix;
 taskFEname += suffix;
 // create instance of the class
 AliAnalysisTaskCRCZDC* taskFE = new AliAnalysisTaskCRCZDC(taskFEname, "", bCutsQA);
 taskFE->SetCentralityRange(centrMin,centrMax);
  if(sCentrEstimator=="V0")  taskFE->SetCentralityEstimator(AliAnalysisTaskCRCZDC::kV0M);
  if(sCentrEstimator=="TRK") taskFE->SetCentralityEstimator(AliAnalysisTaskCRCZDC::kTRK);
  if(sCentrEstimator=="CL1") taskFE->SetCentralityEstimator(AliAnalysisTaskCRCZDC::kCL1);
  if(sCentrEstimator=="CL0") taskFE->SetCentralityEstimator(AliAnalysisTaskCRCZDC::kCL0);
 taskFE->SetRejectPileUp(kTRUE);
 taskFE->SetUseMCCen(bZDCMCCen);
 taskFE->SetZDCGainAlpha(ZDCGainAlpha);
  if (sDataSet == "2010") taskFE->SetDataSet(AliAnalysisTaskCRCZDC::k2010);
  if (sDataSet == "2011") taskFE->SetDataSet(AliAnalysisTaskCRCZDC::k2011);
  if (sDataSet == "2015") taskFE->SetDataSet(AliAnalysisTaskCRCZDC::k2015);
 taskFE->SetQAOn(bCutsQA);
 // set the analysis type
 TString analysisType = "AUTOMATIC";
 if (analysisTypeUser != "") analysisType = analysisTypeUser;
 if (analysisTypeUser == "AOD") analysisType = "AUTOMATIC";
 taskFE->SetAnalysisType(analysisType);
 // set the trigger selection
  if (EvTrigger == "Cen")
    taskFE->SelectCollisionCandidates(AliVEvent::kCentral | AliVEvent::kSemiCentral | AliVEvent::kMB);
  if (EvTrigger == "SemiCen")
    taskFE->SelectCollisionCandidates(AliVEvent::kMB | AliVEvent::kSemiCentral);
  if (EvTrigger == "MB")
    taskFE->SelectCollisionCandidates(AliVEvent::kMB);
  if (EvTrigger == "MB" && sDataSet == "2015")
    taskFE->SelectCollisionCandidates(AliVEvent::kINT7);
  if (EvTrigger == "Any")
    taskFE->SelectCollisionCandidates(AliVEvent::kAny);
  
  if(sDataSet=="2010" && !bZDCMCCen) {
    TString ZDCTowerEqFileName = "alien:///alice/cern.ch/user/j/jmargutt/Calib10hZDCEqTowerVtx.root";
    TFile* ZDCTowerEqFile = TFile::Open(ZDCTowerEqFileName,"READ");
    if(!ZDCTowerEqFile) {
      cout << "ERROR: ZDCTowerEqFile not found!" << endl;
      exit(1);
    }
  } // end of if(sDataSet=="2010" && !bZDCMCCen)
  if(sDataSet=="2015") {
    TString ZDCTowerEqFileName = "alien:///alice/cern.ch/user/j/jmargutt/15oHI_EZDCcalib.root";
    TFile* ZDCTowerEqFile = TFile::Open(ZDCTowerEqFileName,"READ");
    gROOT->cd();
    TList* ZDCTowerEqList = (TList*)(ZDCTowerEqFile->FindObjectAny("EZNcalib"));
    if(ZDCTowerEqList) {
      taskFE->SetTowerEqList(ZDCTowerEqList);
      cout << "ZDCTowerEq set (from " <<  ZDCTowerEqFileName.Data() << ")" << endl;
    } else {
      cout << "ERROR: ZDCTowerEqList not found!" << endl;
      exit(1);
    }
    delete ZDCTowerEqFile;
  }
  if(bCorrectForBadChannel) {
    TString ZDCBadTowerFileName = "alien:///alice/cern.ch/user/j/jmargutt/ZDCCalibBadChannel.root";
    TFile* ZDCBadTowerFile = TFile::Open(ZDCBadTowerFileName,"READ");
    gROOT->cd();
    TList* ZDCBadTowerList = (TList*)(ZDCBadTowerFile->FindObjectAny("resp"));
    if(ZDCBadTowerList) {
      taskFE->SetBadTowerCalibList(ZDCBadTowerList);
      cout << "BadTowerCalibList set (from " <<  ZDCBadTowerFileName.Data() << ")" << endl;
    } else {
      cout << "ERROR: BadTowerCalibList not found!" << endl;
      exit(1);
    }
    delete ZDCBadTowerFile;
  }
  
  // add the task to the manager
 mgr->AddTask(taskFE);
 
 // define the event cuts object
 AliFlowEventCuts* cutsEvent = new AliFlowEventCuts("EventCuts");
  cutsEvent->SetCheckPileup(kFALSE);
  // configure some event cuts, starting with centrality
  if(analysisTypeUser == "MCkine" || analysisTypeUser == "MCAOD" || analysisTypeUser == "ESD") {
    // method used for centrality determination
    if(sCentrEstimator=="V0")  cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kV0);
    if(sCentrEstimator=="TRK") cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kTPConly);
    if(sCentrEstimator=="CL1") cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kSPD1clusters);
    if (sDataSet == "2010" || sDataSet == "2011") {
      cutsEvent->SetCentralityPercentileRange(centrMin,centrMax);
    }
    if (sDataSet == "2015") {
      cutsEvent->SetCentralityPercentileRange(centrMin,centrMax,kTRUE);
    }
      cutsEvent->SetPrimaryVertexZrange(-dVertexRange,dVertexRange);
      cutsEvent->SetQA(bCutsQA);
  }
 else if (analysisTypeUser == "AOD") {
   if (sDataSet == "2010" || sDataSet == "2011") {
     cutsEvent->SetCentralityPercentileRange(centrMin,centrMax);
   }
   if (sDataSet == "2015") {
     cutsEvent->SetCentralityPercentileRange(centrMin,centrMax,kTRUE);
   }
  // method used for centrality determination
  if(sCentrEstimator=="V0")  cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kV0);
  if(sCentrEstimator=="TRK") cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kTPConly);
  if(sCentrEstimator=="CL1") cutsEvent->SetCentralityPercentileMethod(AliFlowEventCuts::kSPD1clusters);
  AliFlowTrackCuts* RefMultCuts = new AliFlowTrackCuts("RefMultCuts");
   RefMultCuts->SetParamType(AliFlowTrackCuts::kAODFilterBit);
   RefMultCuts->SetAODfilterBit(768);
   RefMultCuts->SetMinimalTPCdedx(-999999999);
   RefMultCuts->SetMaxDCAToVertexXY(1000.);
   RefMultCuts->SetMaxDCAToVertexZ(1000.);
   RefMultCuts->SetMinNClustersTPC(70.);
   RefMultCuts->SetMinChi2PerClusterTPC(0.1);
   RefMultCuts->SetMaxChi2PerClusterTPC(4.);
   RefMultCuts->SetPtRange(0.2,20.2);
   RefMultCuts->SetEtaRange(-0.8,0.8);
   RefMultCuts->SetAcceptKinkDaughters(kFALSE);
  cutsEvent->SetRefMultCuts(RefMultCuts);
  cutsEvent->SetRefMultMethod(AliFlowEventCuts::kTPConly);
  // vertex-z cut
  cutsEvent->SetPrimaryVertexZrange(-dVertexRange,dVertexRange);
  // enable the qa plots
  cutsEvent->SetQA(bCutsQA);
  // explicit multiplicity outlier cut
  cutsEvent->SetCutTPCmultiplicityOutliersAOD(kTRUE);
  if (sDataSet == "2011") cutsEvent->SetLHC11h(kTRUE);
  if (sDataSet == "2010") cutsEvent->SetLHC10h(kTRUE);
 }
 
 // pass these cuts to your flow event task
 taskFE->SetCutsEvent(cutsEvent);
 AliFlowTrackCuts* cutsRP = new AliFlowTrackCuts("RP cuts");
 AliFlowTrackCuts* cutsPOI = new AliFlowTrackCuts("POI cuts");
 
 if (analysisTypeUser == "MCkine") {
  // Track cuts for RPs
  cutsRP->SetParamType(AliFlowTrackCuts::kMC);
  cutsRP->SetCutMC(kTRUE);
  cutsRP->SetPtRange(ptMin,ptMax);
  cutsRP->SetEtaRange(etaMin,etaMax);
  cutsRP->SetQA(bCutsQA);
  if(bUseVZERO) {
   cutsRP->SetEtaRange(-10.,+10.);
   cutsRP->SetEtaGap(-1.,1.);
  }
  // Track cuts for POIs
  cutsPOI->SetParamType(AliFlowTrackCuts::kMC);
  cutsPOI->SetCutMC(kTRUE);
  cutsPOI->SetPtRange(ptMin,ptMax);
  cutsPOI->SetEtaRange(etaMin,etaMax);
  cutsPOI->SetQA(bCutsQA);
 }
 if (analysisTypeUser == "AOD" || analysisTypeUser == "MCAOD") {
  // Track cuts for RPs
  if(bUseVZERO) {
   if (sDataSet == "2011")
    cutsRP->SetParamType(AliFlowTrackCuts::kDeltaVZERO);
   if (sDataSet == "2010")
    cutsRP->SetParamType(AliFlowTrackCuts::kBetaVZERO);
   if (sDataSet == "2015")
    cutsRP->SetParamType(AliFlowTrackCuts::kKappaVZERO);
   cutsRP->SetEtaRange(-10.,+10.);
   cutsRP->SetEtaGap(-1.,1.);
   cutsRP->SetPhiMin(0.);
   cutsRP->SetPhiMax(TMath::TwoPi());
   // options for the reweighting
   cutsRP->SetVZEROgainEqualizationPerRing(bUseVZERO);
   cutsRP->SetApplyRecentering(bUseVZERO);
   cutsRP->SetDivSigma(bDivSigma);
   if (analysisTypeUser == "MCAOD")
    cutsRP = AliFlowTrackCuts::GetStandardVZEROOnlyTrackCuts();
  } else {
    cutsRP->SetParamType(AliFlowTrackCuts::kAODFilterBit);
    cutsRP->SetAODfilterBit(AODfilterBit);
    cutsRP->SetMinimalTPCdedx(-999999999);
    cutsRP->SetMaxDCAToVertexXY(dDCAxy);
    cutsRP->SetMaxDCAToVertexZ(dDCAz);
    cutsRP->SetMinNClustersTPC(dMinClusTPC);
    cutsRP->SetMinChi2PerClusterTPC(0.1);
    cutsRP->SetMaxChi2PerClusterTPC(MaxChi2PerClTPC);
    cutsRP->SetPtRange(ptMin,ptMax);
    cutsRP->SetEtaRange(etaMin,etaMax);
    cutsRP->SetAcceptKinkDaughters(kFALSE);
    cutsRP->SetMaxFracSharedTPCCluster(MaxFracSharedTPCCl);
    if(bCutTPCbound==1) cutsRP->SetCutTPCSecbound(kTRUE,ptMin); // new cut for LHC15o
    if(bCutTPCbound==2) cutsRP->SetCutTPCSecboundVar(kTRUE); // new cut for LHC15o
    cutsRP->SetQA(bCutsQA);
  }
  // Track cuts for POIs
  cutsPOI->SetParamType(AliFlowTrackCuts::kAODFilterBit);
  cutsPOI->SetAODfilterBit(AODfilterBit);
  cutsPOI->SetMinimalTPCdedx(-999999999);
  cutsPOI->SetMaxDCAToVertexXY(dDCAxy);
  cutsPOI->SetMaxDCAToVertexZ(dDCAz);
  cutsPOI->SetMinNClustersTPC(dMinClusTPC);
  cutsPOI->SetMinChi2PerClusterTPC(0.1);
  cutsPOI->SetMaxChi2PerClusterTPC(MaxChi2PerClTPC);
  cutsPOI->SetPtRange(ptMin,ptMax);
  cutsPOI->SetEtaRange(etaMin,etaMax);
  cutsPOI->SetAcceptKinkDaughters(kFALSE);
  cutsPOI->SetMaxFracSharedTPCCluster(MaxFracSharedTPCCl);
  if(bCutTPCbound==1) cutsPOI->SetCutTPCSecbound(kTRUE,ptMin); // new cut for LHC15o
  if(bCutTPCbound==2) cutsPOI->SetCutTPCSecboundVar(kTRUE); // new cut for LHC15o
  cutsPOI->SetQA(bCutsQA);
 }
 if (analysisTypeUser == "ESD") {
  cutsRP = AliFlowTrackCuts::GetStandardGlobalTrackCuts2010();
  cutsPOI = AliFlowTrackCuts::GetStandardGlobalTrackCuts2010();
 }
 
 taskFE->SetCutsRP(cutsRP);
 taskFE->SetCutsPOI(cutsPOI);
 taskFE->SetSubeventEtaRange(-10.,-1.,1.,10.);
 if (analysisTypeUser == "MCkine")
  taskFE->SetSubeventEtaRange(-3.7,-1.7,2.8,5.1);
 
 // get the default name of the output file ("AnalysisResults.root")
 TString file = "AnalysisResults.root";
 
 // get the common input container from the analysis manager
 AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
 
 // create a data container for the output of the flow event task
 TString taskFECname = "FlowEventContainer";
 taskFECname += CRCsuffix;
 taskFECname += suffix;
 AliAnalysisDataContainer *coutputFE = mgr->CreateContainer(taskFECname,
                                                            AliFlowEventSimple::Class(),
                                                            AliAnalysisManager::kExchangeContainer);
 // connect the input data to the flow event task
 mgr->ConnectInput(taskFE,0,cinput);
 // and connect the output to the flow event task
 mgr->ConnectOutput(taskFE,1,coutputFE);
 
 // QA OUTPUT CONTAINER
  TString taskFEQAname = file;
  taskFEQAname += ":CutsQA";
  taskFEQAname += CRCsuffix;
  taskFEQAname += suffix;
  AliAnalysisDataContainer* coutputFEQA = mgr->CreateContainer(taskFEQAname.Data(),
                                                               TList::Class(),
                                                               AliAnalysisManager::kOutputContainer,
                                                               taskFEQAname);
  // and connect the qa output container to the flow event.
  // this container will be written to the output file
  mgr->ConnectOutput(taskFE,2,coutputFEQA);

 //TString ParticleWeightsFileName = "ParticleWeights2D_FullLHC10h_2030.root";
 
 // create the flow analysis tasks
 TString taskCRCname = "AnalysisTask";
 taskCRCname += CRCsuffix;
 taskCRCname += suffix;
 AliAnalysisTaskCRC *taskQC = new AliAnalysisTaskCRC(taskCRCname, kTRUE);
 // set number of centrality bins
 taskQC->SetnCenBin(nCenBin);
 taskQC->SetCenBinWidth(CenBinWidth);
 taskQC->SetDataSet(sDataSet);
 taskQC->SetInteractionRate(sIntRate);
 taskQC->SetSelectCharge(sSelecCharge);
 // set thei triggers
 if (EvTrigger == "Cen")
  taskQC->SelectCollisionCandidates(AliVEvent::kMB | AliVEvent::kCentral | AliVEvent::kSemiCentral);
 else if (EvTrigger == "MB")
  taskQC->SelectCollisionCandidates(AliVEvent::kMB);
 if (EvTrigger == "MB" && sDataSet == "2015")
  taskQC->SelectCollisionCandidates(AliVEvent::kINT7);
 else if (EvTrigger == "Any")
  taskQC->SelectCollisionCandidates(AliVEvent::kAny);
 // and set the correct harmonic n
 taskQC->SetHarmonic(nHarmonic);
 // set standard flow settings
 taskQC->SetCalculateDiffFlow(kFALSE);
 taskQC->SetCalculateDiffFlowVsEta(kFALSE);
 taskQC->SetCalculateMixedHarmonics(kFALSE); // calculate all multi-partice mixed-harmonics correlators
 taskQC->SetStoreDistributions(kFALSE);
 taskQC->SetFillMultipleControlHistograms(kFALSE);
 taskQC->SetBookOnlyBasicCCH(kTRUE); // book only basic common control histograms
 //  CRC settings
 taskQC->SetStoreVarious(kTRUE);
 taskQC->SetCalculateCRC(kTRUE);
 taskQC->SetCalculateCRC2(bCalculateCRC2);
 taskQC->SetCalculateCRCVZ(bCalculateCRCVZ);
 taskQC->SetCalculateCRCZDC(bCalculateCRCZDC);
 taskQC->SetCalculateEbEFlow(bCalculateEbEFlow);
 taskQC->SetCRC2nEtaBins(CRC2nEtaBins);
 taskQC->SetCalculateCME(bCalculateCME);
 taskQC->SetCalculateFlowQC(bCalculateFlow);
 if(bUseZDC) taskQC->SetCalculateFlowZDC(bCalculateFlow);
 if(bUseVZERO) taskQC->SetCalculateFlowVZ(bCalculateFlow);
 taskQC->SetFlowQCCenBin(NumCenBins);
 taskQC->SetFlowQCDeltaEta(DeltaEta);
 taskQC->SetUseVZERO(bUseVZERO);
 taskQC->SetUseZDC(kTRUE);
  if (ZDCCalibFileName != "" && bUseZDC) {
    taskQC->SetRecenterZDC(kTRUE);
  }
 taskQC->SetNUAforCRC(kTRUE);
 taskQC->SetCRCEtaRange(-0.8,0.8);
 taskQC->SetUseCRCRecenter(bUseCRCRecenter);
 taskQC->SetDivSigma(bDivSigma);
 taskQC->SetInvertZDC(bUseZDC);
 taskQC->SetCorrWeight(sCorrWeight);
 taskQC->SetQAZDCCuts(bSetQAZDC);
 taskQC->SetMinMulZN(MinMulZN);
 taskQC->SetMaxDevZN(MaxDevZN);
  if(bSetQAZDC && bUseZDC && sDataSet != "2015") {
    TFile* ZDCESEFile = TFile::Open(ZDCESEFileName,"READ");
    gROOT->cd();
    if(!ZDCESEFile) {
      cout << "ERROR: ZDCESEFile not found!" << endl;
      exit(1);
    }
    TList* ZDCESEList = (TList*)(ZDCESEFile->FindObjectAny("ZDCESE"));
    if(ZDCESEList) {
      taskQC->SetZDCESEList(ZDCESEList);
      cout << "ZDCESE set (from " <<  ZDCESEFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: ZDCESEList not found!" << endl;
      exit(1);
    }
    delete ZDCESEFile;
  } // end of if(bSetQAZDC)
 
 if(bCenFlattening) {
  TFile* CenWeightsFile = TFile::Open(CenWeightsFileName,"READ");
  if(!CenWeightsFile) {
   cout << "ERROR: CenWeightsFile not found!" << endl;
   exit(1);
  }
  TCanvas* cav = (TCanvas*)(CenWeightsFile->Get("Canvas_1"));
  TH1D* CenHist = (TH1D*)(cav->GetPrimitive("Centrality"));
  if(CenHist) {
   taskQC->SetCenWeightsHist(CenHist);
   cout << "Centrality weights set (from " <<  CenWeightsFileName.Data() << ")" << endl;
  }
  else {
   cout << "ERROR: CenHist not found!" << endl;
   exit(1);
  }
 } // end of if(bCenFlattening)
 
 if(bUsePtWeights) {
  taskQC->SetUsePtWeights(bUsePtWeights);
  TFile* PtWeightsFile = TFile::Open(PtWeightsFileName,"READ");
  if(!PtWeightsFile) {
   cout << "ERROR: PtWeightsFile not found!" << endl;
   exit(1);
  }
  for(Int_t c=0; c<10; c++) {
   TH1D* PtWeightsHist = (TH1D*)(PtWeightsFile->Get(Form("eff_unbiased_%d",c)));
   if(PtWeightsHist) {
    taskQC->SetPtWeightsHist(PtWeightsHist,c);
    cout << "Pt weights centr. "<<c<<" set (from " <<  PtWeightsFileName.Data() << ")" << endl;
   }
   else {
    cout << "ERROR: PtWeightsHist not found!" << endl;
    exit(1);
   }
  }
 } // end of if(bUsePtWeights)
  
  if(MinMulZN==5 && ptMin==0.2 && ptMax==20.2) {
    // set multiplicity weights
    taskQC->SetUseZDCESEMulWeights(kTRUE);
    TString MulWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2011") {
      if(AODfilterBit==768) MulWeightsFileName += "Calib11hZDCESE_MultCorr_FB768.root";
      if(AODfilterBit==128) MulWeightsFileName += "Calib11hZDCESE_MultCorr_FB128.root";
    }
    if(sDataSet=="2010") {
      if(AODfilterBit==768) MulWeightsFileName += "Calib10hZDCESE_MultCorr_FB768.root";
      if(AODfilterBit==128) MulWeightsFileName += "Calib10hZDCESE_MultCorr_FB128.root";
    }
    TFile* MulWeightsFile = TFile::Open(MulWeightsFileName,"READ");
    if(!MulWeightsFile) {
      cout << "ERROR: ZDCESEMulWeightsFile not found!" << endl;
      exit(1);
    }
    TList* ZDCESEList = (TList*)(MulWeightsFile->FindObjectAny("ZDCESE"));
    for(Int_t c=0; c<5; c++) {
      TH2F* MulWeightsHist = (TH2F*)(ZDCESEList->FindObject(Form("CenvsMulWeig[%d]",c)));
      if(MulWeightsHist) {
        taskQC->SetZDCESEMultWeightsHist(MulWeightsHist,c);
        cout << "ZDC-ESE Mult. Weights (class #"<<c<<") set (from " <<  MulWeightsFileName.Data() << ")" << endl;
      }
      else {
        cout << "ERROR: ZDC-ESE Mult. Hist not found!" << endl;
        exit(1);
      }
    }
    
    // set pt weights
    taskQC->SetUseZDCESESpecWeights(kTRUE);
    TString SpecWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2011") {
      if(AODfilterBit==768) SpecWeightsFileName += "Calib11hZDCESE_SpecCorr_FB768.root";
      if(AODfilterBit==128) SpecWeightsFileName += "Calib11hZDCESE_SpecCorr_FB128.root";
    }
    if(sDataSet=="2010") {
      if(AODfilterBit==768) SpecWeightsFileName += "Calib10hZDCESE_SpecCorr_FB768.root";
      if(AODfilterBit==128) SpecWeightsFileName += "Calib10hZDCESE_SpecCorr_FB128.root";
    }
    TFile* SpecWeightsFile = TFile::Open(SpecWeightsFileName,"READ");
    if(!SpecWeightsFile) {
      cout << "ERROR: ZDCESESpecWeightsFile not found!" << endl;
      exit(1);
    }
    TList* ZDCESEList = (TList*)(SpecWeightsFile->FindObjectAny("ZDCESE"));
    for(Int_t c=0; c<5; c++) {
      TH2F* SpecWeightsHist = (TH2F*)(ZDCESEList->FindObject(Form("CenvsSpecWeig[%d]",c)));
      if(SpecWeightsHist) {
        taskQC->SetZDCESESpecWeightsHist(SpecWeightsHist,c);
        cout << "ZDC-ESE Spec. Weights (class #"<<c<<") set (from " <<  SpecWeightsFileName.Data() << ")" << endl;
      }
      else {
        cout << "ERROR: ZDC-ESE Spec. Hist not found!" << endl;
        exit(1);
      }
    }
  }
 
  if(bUseCRCRecenter) {
    TString QVecWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(AODfilterBit==768) QVecWeightsFileName += "15oHI_QnVarCalib_FB768.root";
    if(AODfilterBit==96) QVecWeightsFileName += "15oHI_QnVarCalib_FB96.root";
    if(AODfilterBit==32) QVecWeightsFileName += "15oHI_QnVarCalib_FB32.root";
    TFile* QVecWeightsFile = TFile::Open(QVecWeightsFileName,"READ");
    if(!QVecWeightsFile) {
      cout << "ERROR: QVecWeightsFile not found!" << endl;
      exit(1);
    }
    TList* QVecWeightsList = (TList*)(QVecWeightsFile->FindObjectAny("Q Vectors"));
    if(QVecWeightsList) {
      taskQC->SetQVecList(QVecWeightsList);
      cout << "Q Vector weights set (from " <<  QVecWeightsFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: QVecWeightsList not found!" << endl;
      exit(1);
    }
  } // end of if(bUseCRCRecenter)
  
  if(ZDCCalibFileName != "" && bUseZDC) {
    TFile* ZDCCalibFile = TFile::Open(ZDCCalibFileName,"READ");
    if(!ZDCCalibFile) {
      cout << "ERROR: ZDC calibration not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* ZDCCalibList = (TList*)(ZDCCalibFile->FindObjectAny("Q Vectors"));
    if(ZDCCalibList) {
      taskQC->SetCRCZDCCalibList(ZDCCalibList);
      cout << "ZDC calibration set (from " <<  ZDCCalibFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: ZDCCalibList not found!" << endl;
      exit(1);
    }
    delete ZDCCalibFile;
  } // end of if(bUseZDC)
  if(bRescaleZDC) {
    TString ZDCRecFileName = "alien:///alice/cern.ch/user/j/jmargutt/15o_ZDCRescaling.root";
    TFile* ZDCRecFile = TFile::Open(ZDCRecFileName,"READ");
    if(!ZDCRecFile) {
      cout << "ERROR: ZDC calibration (rescaling) not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* ZDCRecList = (TList*)(ZDCRecFile->FindObjectAny("ZDC Rescaling"));
    if(ZDCRecList) {
      taskQC->SetCRCZDCResList(ZDCRecList);
      cout << "ZDC calibration (rescaling) set (from " <<  ZDCRecFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: ZDCResList not found!" << endl;
      exit(1);
    }
    delete ZDCRecFile;
  }


  if(sPhiEtaWeight=="def") {
    taskQC->SetUsePhiEtaWeights(kTRUE);
    TString PhiEtaWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2015" && sIntRuns=="high") {
      if(sSelecCharge != "") {
        if(bUsePtWeights && AODfilterBit==768) {
          if(sSelecCharge.EqualTo("pos")) PhiEtaWeightsFileName += "15oHI_FB768_pteff_pch_CenPhiEtaWeights.root";
          if(sSelecCharge.EqualTo("neg")) PhiEtaWeightsFileName += "15oHI_FB768_pteff_nch_CenPhiEtaWeights.root";
        }
      } else {
        if(!bUsePtWeights) {
          if(AODfilterBit==32)  PhiEtaWeightsFileName += "15oHI_FB32_CenPhiEtaWeights.root";
          if(AODfilterBit==768) PhiEtaWeightsFileName += "15oHI_FB768_CenPhiEtaWeights.root";
        } else {
          if(PtWeightsFileName.Contains("eff.root")) {
            if(AODfilterBit==32)  PhiEtaWeightsFileName += "15oHI_FB32_pteff_CenPhiEtaWeights.root";
            if(AODfilterBit==96)  PhiEtaWeightsFileName += "15oHI_FB96_pteff_CenPhiEtaWeights.root";
            if(AODfilterBit==768) PhiEtaWeightsFileName += "15oHI_FB768_pteff_CenPhiEtaWeights.root";
          }
          else if(PtWeightsFileName.Contains("eff_2.root")) {
            if(AODfilterBit==768) PhiEtaWeightsFileName += "15oHI_FB768_pteff2_CenPhiEtaWeights.root";
          }
        }
      }
    }
    if(sDataSet=="2015" && sIntRuns=="low") {
      if(AODfilterBit==32)  PhiEtaWeightsFileName += "15oLI_FB32_CenPhiEtaWeights.root";
      if(AODfilterBit==768) PhiEtaWeightsFileName += "15oLI_FB768_CenPhiEtaWeights.root";
    }
    if(sDataSet=="2010") {
      if(sSelecCharge != "") {
        if(bUsePtWeights && AODfilterBit==768) {
          if(sSelecCharge.EqualTo("pos")) PhiEtaWeightsFileName += "10h_FB768_pteff_pch_CenPhiEtaWeights.root";
          if(sSelecCharge.EqualTo("neg")) PhiEtaWeightsFileName += "10h_FB768_pteff_nch_CenPhiEtaWeights.root";
        }
      } else {
        if(bUsePtWeights) {
          if(AODfilterBit==32)  PhiEtaWeightsFileName += "10h_FB32_pteff_CenPhiEtaWeights.root";
          if(AODfilterBit==96)  PhiEtaWeightsFileName += "10h_FB96_pteff_CenPhiEtaWeights.root";
          if(AODfilterBit==768) PhiEtaWeightsFileName += "10h_FB768_pteff_CenPhiEtaWeights.root";
        }
      }
    }
    TFile* PhiEtaWeightsFile = TFile::Open(PhiEtaWeightsFileName,"READ");
    if(!PhiEtaWeightsFile) {
      cout << "ERROR: PhiEtaWeightsFile not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* PhiEtaWeightsList = (TList*)(PhiEtaWeightsFile->FindObjectAny("CenPhiEta Weights"));
    if(PhiEtaWeightsList) {
      taskQC->SetWeightsList(PhiEtaWeightsList);
      cout << "CenPhiEta weights set (from " <<  PhiEtaWeightsFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: CenPhiEtaWeightsList not found!" << endl;
      exit(1);
    }
    delete PhiEtaWeightsFile;
  }
  
  if(sPhiEtaWeight=="chdep") {
    taskQC->SetUsePhiEtaWeightsChDep(kTRUE);
    TString PhiEtaWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2015" && sIntRuns=="high") {
      if(bUsePtWeights && AODfilterBit==768) {
        PhiEtaWeightsFileName += "15oHI_FB768_pteff_ChDep_CenPhiEtaWeights.root";
      }
    }
    TFile* PhiEtaWeightsFileChDep = TFile::Open(PhiEtaWeightsFileName,"READ");
    if(!PhiEtaWeightsFileChDep) {
      cout << "ERROR: PhiEtaWeightsFileChDep not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* PhiEtaWeightsListChDep = (TList*)(PhiEtaWeightsFileChDep->FindObjectAny("CenPhiEta Weights"));
    if(PhiEtaWeightsListChDep) {
      taskQC->SetWeightsListChDep(PhiEtaWeightsListChDep);
      cout << "ChDep CenPhiEta weights set (from " <<  PhiEtaWeightsFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: ChDep CenPhiEtaWeightsList not found!" << endl;
      exit(1);
    }
    delete PhiEtaWeightsFileChDep;
  }
  
  if(sPhiEtaWeight=="vtxdep") {
    taskQC->SetUsePhiEtaWeightsVtxDep(kTRUE);
    TString PhiEtaWeightsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2015" && sIntRuns=="high") {
      if(bUsePtWeights && AODfilterBit==768) {
        PhiEtaWeightsFileName += "15oHI_FB768_pteff2_CenVtxDepPhiEtaWeights.root";
      }
    }
    TFile* PhiEtaWeightsFileVtxDep = TFile::Open(PhiEtaWeightsFileName,"READ");
    if(!PhiEtaWeightsFileVtxDep) {
      cout << "ERROR: PhiEtaWeightsFileVtxDep not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* PhiEtaWeightsListVtxDep = (TList*)(PhiEtaWeightsFileVtxDep->FindObjectAny("CenPhiEta Weights"));
    if(PhiEtaWeightsListVtxDep) {
      taskQC->SetWeightsListVtxDep(PhiEtaWeightsListVtxDep);
      cout << "VtxDep CenPhiEta weights set (from " <<  PhiEtaWeightsFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: VtxDep CenPhiEtaWeightsList not found!" << endl;
      exit(1);
    }
    delete PhiEtaWeightsFileVtxDep;
  }
  
  taskQC->SetUsePhiEtaCuts(bUsePhiEtaCuts);
  if(bUsePhiEtaCuts) {
    TString PhiEtaCutsFileName = "alien:///alice/cern.ch/user/j/jmargutt/";
    if(sDataSet=="2015" && sIntRuns=="high") {
      if(AODfilterBit==32) PhiEtaCutsFileName +=  "15oHI_FB32_CenPhiEtaCut.root";
      if(AODfilterBit==768) PhiEtaCutsFileName += "15oHI_FB768_CenPhiEtaCut.root";
    }
    if(sDataSet=="2015" && sIntRuns=="low") {
      if(AODfilterBit==32) PhiEtaCutsFileName +=  "15oLI_FB32_CenPhiEtaCut.root";
      if(AODfilterBit==768) PhiEtaCutsFileName += "15oLI_FB768_CenPhiEtaCut.root";
    }
      
    TFile* PhiEtaCutsFile = TFile::Open(PhiEtaCutsFileName,"READ");
    if(!PhiEtaCutsFile) {
      cout << "ERROR: PhiEtaCutsFile not found!" << endl;
      exit(1);
    }
    gROOT->cd();
    TList* PhiEtaCutsList = (TList*)(PhiEtaCutsFile->FindObjectAny("CenPhiEta Cut"));
    if(PhiEtaCutsList) {
      taskQC->SetPhiEtaCutsList(PhiEtaCutsList);
      cout << "CenPhiEta cuts set (from " <<  PhiEtaCutsFileName.Data() << ")" << endl;
    }
    else {
      cout << "ERROR: PhiEtaCutsList not found!" << endl;
      exit(1);
    }
    delete PhiEtaCutsFile;
  } // end of if(bUsePhiEtaCuts)

 // connect the task to the analysis manager
 mgr->AddTask(taskQC);
 
 // initialize output name
 TString outputQC = file;
 outputQC += CRCsuffix;
 outputQC += suffix;
 // create and connect the output containers
 AliAnalysisDataContainer *coutputQC = mgr->CreateContainer(outputQC.Data(),
                                                            TList::Class(),
                                                            AliAnalysisManager::kOutputContainer,
                                                            outputQC);
 // connect the output of the flow event task to the flow analysis task
 mgr->ConnectInput(taskQC, 0, coutputFE);
 // and connect the output of the flow analysis task to the output container
 // which will be written to the output file
 mgr->ConnectOutput(taskQC, 1, coutputQC);
 
 return taskQC;
}

