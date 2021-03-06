#if !defined(__CINT__) || defined(__MAKECINT__)
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <Riostream.h>
#include <TClassTable.h>
#include <TStyle.h>
#include <TMath.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TLine.h>
#include <TGrid.h>
#include <TBits.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TBranch.h>
#include <TFileMerger.h>
#include <TGridResult.h>
#include <TSystem.h>
#include <TGaxis.h>
#endif

int MakeTrendZDC(char *infile, int run) {
  
  TStopwatch timer;
  timer.Start();
  gSystem->SetIncludePath("-I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_ROOT");
  gSystem->Load("libSTAT");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libANALYSIScalib");
  gSystem->Load("libCORRFW");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libANALYSIScalib");
  gSystem->Load("libTender");
  gSystem->Load("libPWGPP");
  
  char *outfile = "trending.root";
  
  if (!infile) return -1;
  if (!outfile) return -1;
  TFile *f =0;
  f=TFile::Open(infile,"read");
  if (!f) {
    printf("File %s not available\n", infile);
    return -1;
  }  
   
  //access histograms lists
  char zdcQAdirName[20]="ZDC_Performance";
  char genListName[20]="QAZDCHists";
  
  TDirectoryFile * zdcQAdir=(TDirectoryFile*)f->Get(zdcQAdirName);
  if (!zdcQAdir) {
    Printf("ERROR: ZDC QA directory not present in input file.\n");
    return -1;
  }
  
  TList * generalList=(TList*)zdcQAdir->Get(genListName);

  if (!generalList) Printf("WARNING: general QA histograms absent or not accessible\n");

  TH1D    *fhTDCZNC          = (TH1D*)generalList->FindObject("fhTDCZNC");       //! TDC ZNC sum
  TH1D    *fhTDCZNA          = (TH1D*)generalList->FindObject("fhTDCZNA");       //! TDC DIFF sum
  TH1D    *fhTDCZNSum        = (TH1D*)generalList->FindObject("fhTDCZNSum");     //! TDC ZNC sum
  TH1D    *fhTDCZNDiff       = (TH1D*)generalList->FindObject("fhTDCZNDiff");    //! TDC DIFF sum
  TH1D    *fhZNCSumQ         = (TH1D*)generalList->FindObject("fhZNCSumQ");      //! ZNC sum 4Q
  TH1D    *fhZNASumQ         = (TH1D*)generalList->FindObject("fhZNASumQ");      //! ZNA sum 4Q
  TH1D    *fhZPCSumQ         = (TH1D*)generalList->FindObject("fhZPCSumQ");      //! ZPC sum 4Q
  TH1D    *fhZPASumQ         = (TH1D*)generalList->FindObject("fhZPASumQ");      //! ZPA sum 4Q
  TH1D    *fhZEM1Spectrum    = (TH1D*)generalList->FindObject("fhZEM1Spectrum"); //! ZEM1 spectra
  TH1D    *fhZEM2Spectrum    = (TH1D*)generalList->FindObject("fhZEM2Spectrum"); //! ZEM2 spectra
  TH1D    *fhZNCpmc          = (TH1D*)generalList->FindObject("fhZNCpmc");       //! ZNC PMCs
  TH1D    *fhZNApmc          = (TH1D*)generalList->FindObject("fhZNApmc");       //! ZNA PMCs
  TH1D    *fhZPCpmc          = (TH1D*)generalList->FindObject("fhZPCpmc");       //! ZPC PMCs
  TH1D    *fhZPApmc          = (TH1D*)generalList->FindObject("fhZPApmc");       //! ZPA PMCs
  TH1D    *fhPMCZNCemd       = (TH1D*)generalList->FindObject("fhPMCZNCemd");    //! ZNC PMC low gain chain
  TH1D    *fhPMCZNAemd       = (TH1D*)generalList->FindObject("fhPMCZNAemd");    //! ZNA PMC low gain chain
  TH1D    *fhTDCZNAcorr      = (TH1D*)generalList->FindObject("fhTDCZNAcorr");   //! ZNA corrected TDC
  TH1D    *fhTDCZNCcorr      = (TH1D*)generalList->FindObject("fhTDCZNCcorr");   //! ZNC corrected TDC  
  TH2D    *fhZNCCentroid     = (TH2D*)generalList->FindObject("fhZNCCentroid");  //! ZNC centroid
  TH2D    *fhZNACentroid     = (TH2D*)generalList->FindObject("fhZNACentroid");  //! ZNA centroid
  TH2D    *fDebunch          = (TH2D*)generalList->FindObject("fDebunch");       //! TDC sum vs. diff  
  
  Double_t ZNC_mean = 0.;
  Double_t ZNA_mean = 0.;
  Double_t ZPC_mean = 0.;
  Double_t ZPA_mean = 0.;
  Double_t ZEM1_mean = 0.;
  Double_t ZEM2_mean = 0.;  
  
  Int_t fhZNCpmcEntries = fhZNCpmc->GetEntries();
  Int_t fhZNApmcEntries = fhZNApmc->GetEntries();
  Int_t fhZPCpmcEntries = fhZPCpmc->GetEntries();
  Int_t fhZPApmcEntries = fhZPApmc->GetEntries();
  Int_t fhZEM1SpectrumEntries = fhZEM1Spectrum->GetEntries();
  Int_t fhZEM2SpectrumEntries = fhZEM2Spectrum->GetEntries();  
  
  if (fhZNCpmcEntries>0) ZNC_mean = fhZNCpmc->GetMean();
  if (fhZNApmcEntries>0) ZNA_mean = fhZNApmc->GetMean();
  if (fhZPCpmcEntries>0) ZPC_mean = fhZPCpmc->GetMean();
  if (fhZPApmcEntries>0) ZPA_mean = fhZPApmc->GetMean();
  if (fhZEM1SpectrumEntries>0) ZEM1_mean = fhZEM1Spectrum->GetMean();
  if (fhZEM2SpectrumEntries>0) ZEM2_mean = fhZEM2Spectrum->GetMean(); 
  
  Double_t ZNC_XCent = fhZNCCentroid->GetMean(1);
  Double_t ZNC_YCent = fhZNCCentroid->GetMean(2);    
  Double_t ZNA_XCent = fhZNACentroid->GetMean(1);
  Double_t ZNA_YCent = fhZNACentroid->GetMean(2);
  Double_t ZNC_XCent_err = fhZNCCentroid->GetRMS(1);
  Double_t ZNC_YCent_err = fhZNCCentroid->GetRMS(2);    
  Double_t ZNA_XCent_err = fhZNACentroid->GetRMS(1);
  Double_t ZNA_YCent_err = fhZNACentroid->GetRMS(2);     
  Double_t ZN_TDC_Sum = fhTDCZNSum->GetMean();
  Double_t ZN_TDC_Diff = fhTDCZNDiff->GetMean();  
  Double_t ZN_TDC_Sum_err = fhTDCZNSum->GetRMS();
  Double_t ZN_TDC_Diff_err = fhTDCZNDiff->GetRMS();    
  
  TTree * ttree=new TTree("trending","tree of trending variables");
  ttree->Branch("run",&run,"run/I");
  ttree->Branch("ZNC_mean_value",&ZNC_mean,"ZNC_mean_value/D");
  ttree->Branch("ZNA_mean_value",&ZNA_mean,"ZNA_mean_value/D"); 
  ttree->Branch("ZPC_mean_value",&ZPC_mean,"ZPC_mean_value/D");
  ttree->Branch("ZPA_mean_value",&ZPA_mean,"ZPA_mean_value/D");
  ttree->Branch("ZEM1_mean_value",&ZEM1_mean,"ZEM1_mean_value/D");
  ttree->Branch("ZEM2_mean_value",&ZEM2_mean,"ZEM2_mean_value/D");  
  ttree->Branch("ZNC_X_Centroid",&ZNC_XCent,"ZNC_X_Centroid/D");
  ttree->Branch("ZNC_Y_Centroid",&ZNC_YCent,"ZNC_Y_Centroid/D");   
  ttree->Branch("ZNA_X_Centroid",&ZNA_XCent,"ZNA_X_Centroid/D");
  ttree->Branch("ZNA_Y_Centroid",&ZNA_YCent,"ZNA_Y_Centroid/D");  
  ttree->Branch("ZNC_X_Centroid_Err",&ZNC_XCent_err,"ZNC_X_Centroid_Err/D");
  ttree->Branch("ZNC_Y_Centroid_Err",&ZNC_YCent_err,"ZNC_Y_Centroid_Err/D");   
  ttree->Branch("ZNA_X_Centroid_Err",&ZNA_XCent_err,"ZNA_X_Centroid_Err/D");
  ttree->Branch("ZNA_Y_Centroid_Err",&ZNA_YCent_err,"ZNA_Y_Centroid_Err/D");    
  ttree->Branch("ZN_TDC_Sum",&ZN_TDC_Sum,"ZN_TDC_Sum/D");
  ttree->Branch("ZN_TDC_Diff",&ZN_TDC_Diff,"ZN_TDC_Diff/D");    
  ttree->Branch("ZN_TDC_Sum_Err",&ZN_TDC_Sum_err,"ZN_TDC_Sum_Err/D");
  ttree->Branch("ZN_TDC_Diff_Err",&ZN_TDC_Diff_err,"ZN_TDC_Diff_Err/D");  
    
  Printf(":::: Getting post-analysis info for run %i",run);
  TFile * trendFile = new TFile(outfile,"recreate");

  printf("==============  Saving histograms for run %i ===============\n",run);
  
  fhTDCZNC->Write();      
  fhTDCZNA->Write();      
  fhTDCZNSum->Write();    
  fhTDCZNDiff->Write();   
  fhZNCSumQ->Write();     
  fhZNASumQ->Write();     
  fhZPCSumQ->Write();     
  fhZPASumQ->Write();     
  fhZEM1Spectrum->Write();
  fhZEM2Spectrum->Write();
  fhZNCpmc->Write();      
  fhZNApmc->Write();      
  fhZPCpmc->Write();      
  fhZPApmc->Write();      
  fhZNCCentroid->Write(); 
  fhZNACentroid->Write(); 
  fhPMCZNCemd->Write();   
  fhPMCZNAemd->Write();   
  fDebunch->Write();      
  fhTDCZNAcorr->Write();  
  fhTDCZNCcorr->Write();
    
  ttree->Fill();
  printf("==============  Saving trending quantities in tree for run %i ===============\n",run);
  
  trendFile->cd();
  ttree->Write();
  trendFile->Close();

}
