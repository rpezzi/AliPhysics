//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TObject to hold V0 configuration + results histogram
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "TList.h"
#include "TH3F.h"
#include "AliVWeakResult.h"
#include "AliCascadeResult.h"
#include "AliLog.h"
#include <iostream>
#include <TROOT.h>
using namespace std;

ClassImp(AliCascadeResult);
//________________________________________________________________
AliCascadeResult::AliCascadeResult() :
  AliVWeakResult(),
fMassHypo(AliCascadeResult::kXiMinus),
//V0 Cuts
fCutDCANegToPV(0.1),
fCutDCAPosToPV(0.1),
fCutDCAV0Daughters(1.0),
fCutV0CosPA(0.998),
fCutV0Radius(5.0),
//Cascade Cuts
fCutDCAV0ToPV(0.05),
fCutV0Mass(0.010),
fCutDCABachToPV(0.03),
fCutDCACascDaughters(2.0),
fCutCascCosPA(0.95),
fCutCascRadius(0.4),
fCutDCABachToBaryon(-1),
fCutBachBaryonCosPA(+2),
fCutMinV0Lifetime(-2),
fCutMaxV0Lifetime(1e+6),
//Miscellaneous
fCutProperLifetime(1000),
fCutLeastNumberOfClusters(70),
fCutTPCdEdx(4.0),
fCutXiRejection(0.008),
fCutMCPhysicalPrimary(kTRUE),
fCutMCPDGCodeAssociation(kTRUE),
fCutMCUseMCProperties(kTRUE),
fCutMCSelectBump(kFALSE),
fCutUseVariableCascCosPA(kFALSE),
fCutVarCascCosPA_Exp0Const(0),
fCutVarCascCosPA_Exp0Slope(0),
fCutVarCascCosPA_Exp1Const(0),
fCutVarCascCosPA_Exp1Slope(0),
fCutVarCascCosPA_Const(1),
fCutUseVariableV0CosPA(kFALSE),
fCutVarV0CosPA_Exp0Const(0),
fCutVarV0CosPA_Exp0Slope(0),
fCutVarV0CosPA_Exp1Const(0),
fCutVarV0CosPA_Exp1Slope(0),
fCutVarV0CosPA_Const(1)
{
    // Dummy Constructor - not to be used!
    //Main output histogram: Centrality, pt, mass
    fHisto = new TH3F("fHisto","", 20,0,100, 200,0,20, 400,0,2 );
    fHisto->Sumw2();
}
//________________________________________________________________
AliCascadeResult::AliCascadeResult(const char * name, AliCascadeResult::EMassHypo lMassHypo, const char * title):
AliVWeakResult(name,title),
fMassHypo(lMassHypo),
//V0 Cuts
fCutDCANegToPV(0.1),
fCutDCAPosToPV(0.1),
fCutDCAV0Daughters(1.0),
fCutV0CosPA(0.998),
fCutV0Radius(5.0),
//Cascade Cuts
fCutDCAV0ToPV(0.05),
fCutV0Mass(0.010),
fCutDCABachToPV(0.03),
fCutDCACascDaughters(2.0),
fCutCascCosPA(0.95),
fCutCascRadius(0.4),
fCutDCABachToBaryon(-1),
fCutBachBaryonCosPA(+2),
fCutMinV0Lifetime(-2),
fCutMaxV0Lifetime(1e+6),
//Miscellaneous
fCutProperLifetime(1000),
fCutLeastNumberOfClusters(70),
fCutTPCdEdx(4.0),
fCutXiRejection(0.008),
fCutMCPhysicalPrimary(kTRUE),
fCutMCPDGCodeAssociation(kTRUE),
fCutMCUseMCProperties(kTRUE),
fCutMCSelectBump(kFALSE),
fCutUseVariableCascCosPA(kFALSE),
fCutVarCascCosPA_Exp0Const(0),
fCutVarCascCosPA_Exp0Slope(0),
fCutVarCascCosPA_Exp1Const(0),
fCutVarCascCosPA_Exp1Slope(0),
fCutVarCascCosPA_Const(1),
fCutUseVariableV0CosPA(kFALSE),
fCutVarV0CosPA_Exp0Const(0),
fCutVarV0CosPA_Exp0Slope(0),
fCutVarV0CosPA_Exp1Const(0),
fCutVarV0CosPA_Exp1Slope(0),
fCutVarV0CosPA_Const(1)
{
    // Constructor
    Double_t lThisMass = GetMass();
    
    //Main output histogram: Centrality, mass, transverse momentum
    fHisto = new TH3F(Form("fHisto_%s",GetName()),"", 20,0,100, 200,0,20, 400,lThisMass-0.1,lThisMass+0.1);
    fHisto->Sumw2();
}
//________________________________________________________________
AliCascadeResult::AliCascadeResult(const char * name, AliCascadeResult::EMassHypo lMassHypo, const char * title, Long_t lNCentBins, Double_t *lCentBins, Long_t lNPtBins, Double_t *lPtBins):
AliVWeakResult(name,title),
fMassHypo(lMassHypo),
//V0 Cuts
fCutDCANegToPV(0.1),
fCutDCAPosToPV(0.1),
fCutDCAV0Daughters(1.0),
fCutV0CosPA(0.998),
fCutV0Radius(5.0),
//Cascade Cuts
fCutDCAV0ToPV(0.05),
fCutV0Mass(0.010),
fCutDCABachToPV(0.03),
fCutDCACascDaughters(2.0),
fCutCascCosPA(0.95),
fCutCascRadius(0.4),
fCutDCABachToBaryon(-1),
fCutBachBaryonCosPA(+2),
fCutMinV0Lifetime(-2),
fCutMaxV0Lifetime(1e+6),
//Miscellaneous
fCutProperLifetime(1000),
fCutLeastNumberOfClusters(70),
fCutTPCdEdx(4.0),
fCutXiRejection(0.008),
fCutMCPhysicalPrimary(kTRUE),
fCutMCPDGCodeAssociation(kTRUE),
fCutMCUseMCProperties(kTRUE),
fCutMCSelectBump(kFALSE),
fCutUseVariableCascCosPA(kFALSE),
fCutVarCascCosPA_Exp0Const(0),
fCutVarCascCosPA_Exp0Slope(0),
fCutVarCascCosPA_Exp1Const(0),
fCutVarCascCosPA_Exp1Slope(0),
fCutVarCascCosPA_Const(1),
fCutUseVariableV0CosPA(kFALSE),
fCutVarV0CosPA_Exp0Const(0),
fCutVarV0CosPA_Exp0Slope(0),
fCutVarV0CosPA_Exp1Const(0),
fCutVarV0CosPA_Exp1Slope(0),
fCutVarV0CosPA_Const(1)
{
    // Constructor
    Double_t lThisMass = GetMass();
    
    //Construct binning in invariant mass as standard: 400 bins from lThisMass-0.1 to lThisMass+1
    const Long_t lNMassBins = 400;
    
    Double_t lMassWindow = 0.1 ;
    Double_t lMassDelta = (lMassWindow * 2.) / lNMassBins;
    Double_t lMassBins[lNMassBins+1];
    
    for( Long_t ibound = 0; ibound<lNMassBins+1; ibound++) lMassBins[ibound] = (lThisMass-0.1) + ( ( (Double_t) ibound )*lMassDelta );
    
    //Main output histogram: Centrality, mass, transverse momentum: Variable binning
    fHisto = new TH3F(Form("fHisto_%s",GetName()),"", lNCentBins, lCentBins, lNPtBins, lPtBins, lNMassBins, lMassBins );
    fHisto->Sumw2();
}
//________________________________________________________________
AliCascadeResult::AliCascadeResult(const AliCascadeResult& lCopyMe, TString lNewName)
: AliVWeakResult(lCopyMe),
fMassHypo(lCopyMe.fMassHypo),
//V0 Cuts
fCutDCANegToPV(lCopyMe.fCutDCANegToPV),
fCutDCAPosToPV(lCopyMe.fCutDCAPosToPV),
fCutDCAV0Daughters(lCopyMe.fCutDCAV0Daughters),
fCutV0CosPA(lCopyMe.fCutV0CosPA),
fCutV0Radius(lCopyMe.fCutV0Radius),
//Cascade Cuts
fCutDCAV0ToPV(lCopyMe.fCutDCAV0ToPV),
fCutV0Mass(lCopyMe.fCutV0Mass),
fCutDCABachToPV(lCopyMe.fCutDCABachToPV),
fCutDCACascDaughters(lCopyMe.fCutDCACascDaughters),
fCutCascCosPA(lCopyMe.fCutCascCosPA),
fCutCascRadius(lCopyMe.fCutCascRadius),
fCutDCABachToBaryon(lCopyMe.fCutDCABachToBaryon),
fCutBachBaryonCosPA(lCopyMe.fCutBachBaryonCosPA),
fCutMinV0Lifetime(lCopyMe.fCutMinV0Lifetime),
fCutMaxV0Lifetime(lCopyMe.fCutMaxV0Lifetime),
//Miscellaneous
fCutProperLifetime(lCopyMe.fCutProperLifetime),
fCutLeastNumberOfClusters(lCopyMe.fCutLeastNumberOfClusters),
fCutTPCdEdx(lCopyMe.fCutTPCdEdx),
fCutXiRejection(lCopyMe.fCutXiRejection),
//MC specific
fCutMCPhysicalPrimary(lCopyMe.fCutMCPhysicalPrimary),
fCutMCPDGCodeAssociation(lCopyMe.fCutMCPDGCodeAssociation),
fCutMCUseMCProperties(lCopyMe.fCutMCUseMCProperties),
fCutMCSelectBump(lCopyMe.fCutMCSelectBump),
fCutUseVariableCascCosPA(lCopyMe.fCutUseVariableCascCosPA),
fCutVarCascCosPA_Exp0Const(lCopyMe.fCutVarCascCosPA_Exp0Const),
fCutVarCascCosPA_Exp0Slope(lCopyMe.fCutVarCascCosPA_Exp0Slope),
fCutVarCascCosPA_Exp1Const(lCopyMe.fCutVarCascCosPA_Exp1Const),
fCutVarCascCosPA_Exp1Slope(lCopyMe.fCutVarCascCosPA_Exp1Slope),
fCutVarCascCosPA_Const(lCopyMe.fCutVarCascCosPA_Const),
fCutUseVariableV0CosPA(lCopyMe.fCutUseVariableV0CosPA),
fCutVarV0CosPA_Exp0Const(lCopyMe.fCutVarV0CosPA_Exp0Const),
fCutVarV0CosPA_Exp0Slope(lCopyMe.fCutVarV0CosPA_Exp0Slope),
fCutVarV0CosPA_Exp1Const(lCopyMe.fCutVarV0CosPA_Exp1Const),
fCutVarV0CosPA_Exp1Slope(lCopyMe.fCutVarV0CosPA_Exp1Slope),
fCutVarV0CosPA_Const(lCopyMe.fCutVarV0CosPA_Const)

{
    SetName( lNewName.Data() );
    
    // Constructor
    Double_t lThisMass = GetMass();
    
    //Main output histogram: Centrality, mass, transverse momentum: Clone from copied object
    fHisto = (TH3F*) lCopyMe.GetHistogramToCopy()->Clone(Form("fHisto_%s",GetName()));
}
//________________________________________________________________
AliCascadeResult::AliCascadeResult(AliCascadeResult *lCopyMe, TString lNewName)
    : AliVWeakResult(*lCopyMe),
      fHisto(0)
{
    SetName(lNewName.Data());
    fMassHypo = lCopyMe->GetMassHypothesis();
    //V0 Cuts
    fCutDCANegToPV     = lCopyMe->GetCutDCANegToPV();
    fCutDCAPosToPV     = lCopyMe->GetCutDCAPosToPV();
    fCutDCAV0Daughters = lCopyMe->GetCutDCAV0Daughters();
    fCutV0CosPA        = lCopyMe->GetCutV0CosPA();
    fCutV0Radius       = lCopyMe->GetCutV0Radius();
    //Cascade Cuts
    fCutDCAV0ToPV = lCopyMe -> GetCutDCAV0ToPV();
    fCutV0Mass    = lCopyMe -> GetCutV0Mass();
    fCutDCABachToPV  = lCopyMe -> GetCutDCABachToPV();
    fCutDCACascDaughters = lCopyMe -> GetCutDCACascDaughters();
    fCutCascCosPA  = lCopyMe -> GetCutCascCosPA();
    fCutCascRadius = lCopyMe -> GetCutCascRadius();
    fCutDCABachToBaryon = lCopyMe -> GetCutDCABachToBaryon();
    fCutBachBaryonCosPA = lCopyMe -> GetCutBachBaryonCosPA();
    fCutMinV0Lifetime = lCopyMe -> GetCutMinV0Lifetime();
    fCutMaxV0Lifetime = lCopyMe -> GetCutMaxV0Lifetime();
    
    //Miscellaneous
    fCutProperLifetime = lCopyMe->GetCutProperLifetime();
    fCutLeastNumberOfClusters = lCopyMe->GetCutLeastNumberOfClusters();
    fCutTPCdEdx = lCopyMe->GetCutTPCdEdx();
    fCutXiRejection = lCopyMe->GetCutXiRejection();
    
    //MC specific
    fCutMCPhysicalPrimary    = lCopyMe -> GetCutMCPhysicalPrimary();
    fCutMCPDGCodeAssociation = lCopyMe -> GetCutMCPDGCodeAssociation();
    fCutMCUseMCProperties    = lCopyMe -> GetCutMCUseMCProperties();
    fCutMCSelectBump         = lCopyMe -> GetCutMCSelectBump();
    
    //Variable CascCosPA
    fCutUseVariableCascCosPA = lCopyMe -> GetCutUseVarCascCosPA();
    fCutVarCascCosPA_Exp0Const = lCopyMe -> GetCutVarCascCosPAExp0Const();
    fCutVarCascCosPA_Exp0Slope = lCopyMe -> GetCutVarCascCosPAExp0Slope();
    fCutVarCascCosPA_Exp1Const = lCopyMe -> GetCutVarCascCosPAExp1Const();
    fCutVarCascCosPA_Exp1Slope = lCopyMe -> GetCutVarCascCosPAExp1Slope();
    fCutVarCascCosPA_Const = lCopyMe -> GetCutVarCascCosPAConst();

    //Variable V0CosPA
    fCutUseVariableV0CosPA = lCopyMe -> GetCutUseVarV0CosPA();
    fCutVarV0CosPA_Exp0Const = lCopyMe -> GetCutVarV0CosPAExp0Const();
    fCutVarV0CosPA_Exp0Slope = lCopyMe -> GetCutVarV0CosPAExp0Slope();
    fCutVarV0CosPA_Exp1Const = lCopyMe -> GetCutVarV0CosPAExp1Const();
    fCutVarV0CosPA_Exp1Slope = lCopyMe -> GetCutVarV0CosPAExp1Slope();
    fCutVarV0CosPA_Const = lCopyMe -> GetCutVarV0CosPAConst();
    
    // Constructor
    Double_t lThisMass = GetMass();
    
    //Main output histogram: Centrality, mass, transverse momentum: Clone from copied object
    fHisto = (TH3F*) lCopyMe->GetHistogramToCopy()->Clone(Form("fHisto_%s",GetName()));
}
//________________________________________________________________
AliCascadeResult::~AliCascadeResult(){
    // Proper destructor: delete pointer data member
    if (fHisto) {
        delete fHisto;
        fHisto = 0x0;
    }
}

//________________________________________________________________
AliCascadeResult& AliCascadeResult::operator=(const AliCascadeResult& lCopyMe)
{
    if (&lCopyMe == this) return *this;
    SetName(lCopyMe.GetName());
    SetTitle(lCopyMe.GetTitle());

    fMassHypo = lCopyMe.GetMassHypothesis();
    //V0 Cuts
    fCutDCANegToPV = lCopyMe.GetCutDCANegToPV();
    fCutDCAPosToPV = lCopyMe.GetCutDCAPosToPV(),
    fCutDCAV0Daughters = lCopyMe.GetCutDCAV0Daughters();
    fCutV0CosPA = lCopyMe.GetCutV0CosPA();
    fCutV0Radius = lCopyMe.GetCutV0Radius();
    //Cascade Cuts
    fCutDCAV0ToPV = lCopyMe.GetCutDCAV0ToPV();
    fCutV0Mass    = lCopyMe.GetCutV0Mass();
    fCutDCABachToPV  = lCopyMe.GetCutDCABachToPV();
    fCutDCACascDaughters = lCopyMe.GetCutDCACascDaughters();
    fCutCascCosPA  = lCopyMe.GetCutCascCosPA();
    fCutCascRadius = lCopyMe.GetCutCascRadius();
    fCutDCABachToBaryon = lCopyMe.GetCutDCABachToBaryon();
    fCutBachBaryonCosPA = lCopyMe.GetCutBachBaryonCosPA();
    fCutMinV0Lifetime = lCopyMe.GetCutMinV0Lifetime();
    fCutMaxV0Lifetime = lCopyMe.GetCutMaxV0Lifetime();
    
    //Miscellaneous
    fCutProperLifetime = lCopyMe.GetCutProperLifetime();
    fCutLeastNumberOfClusters = lCopyMe.GetCutLeastNumberOfClusters();
    fCutTPCdEdx = lCopyMe.GetCutTPCdEdx();
    fCutXiRejection = lCopyMe.GetCutXiRejection();
    
    //MC specific
    fCutMCPhysicalPrimary = lCopyMe.GetCutMCPhysicalPrimary();
    fCutMCPDGCodeAssociation = lCopyMe.GetCutMCPDGCodeAssociation();
    fCutMCUseMCProperties = lCopyMe.GetCutMCUseMCProperties();
    fCutMCSelectBump      = lCopyMe.GetCutMCSelectBump();
    
    //Variable CascCosPA
    fCutUseVariableCascCosPA = lCopyMe.GetCutUseVarCascCosPA();
    fCutVarCascCosPA_Exp0Const = lCopyMe.GetCutVarCascCosPAExp0Const();
    fCutVarCascCosPA_Exp0Slope = lCopyMe.GetCutVarCascCosPAExp0Slope();
    fCutVarCascCosPA_Exp1Const = lCopyMe.GetCutVarCascCosPAExp1Const();
    fCutVarCascCosPA_Exp1Slope = lCopyMe.GetCutVarCascCosPAExp1Slope();
    fCutVarCascCosPA_Const = lCopyMe.GetCutVarCascCosPAConst();

    //Variable V0CosPA
    fCutUseVariableV0CosPA = lCopyMe.GetCutUseVarV0CosPA();
    fCutVarV0CosPA_Exp0Const = lCopyMe.GetCutVarV0CosPAExp0Const();
    fCutVarV0CosPA_Exp0Slope = lCopyMe.GetCutVarV0CosPAExp0Slope();
    fCutVarV0CosPA_Exp1Const = lCopyMe.GetCutVarV0CosPAExp1Const();
    fCutVarV0CosPA_Exp1Slope = lCopyMe.GetCutVarV0CosPAExp1Slope();
    fCutVarV0CosPA_Const = lCopyMe.GetCutVarV0CosPAConst();
    
    if (fHisto) {
        delete fHisto;
        fHisto = 0;
    }
    // Constructor
    Double_t lThisMass = GetMass();
    
    //Main output histogram: Centrality, mass, transverse momentum
    fHisto = new TH3F(Form("fHisto_%s",GetName()),"", 20,0,100, 200,0,20, 400,lThisMass-0.1,lThisMass+0.1);
    fHisto->Sumw2();
    
    return *this;
}

//________________________________________________________________
Long64_t AliCascadeResult::Merge(TCollection *hlist)
//Merging function to allow for usage as analysis output
{
    
    if (!hlist) return 0;
    if (hlist->IsEmpty()) return (Long64_t) GetHistogram()->GetEntries();
    
    if (hlist) {
        AliCascadeResult *xh = 0;
        TIter nxh(hlist);
        while ((xh = (AliCascadeResult *) nxh())) {
            // Check if you're not committing a crime
            if( ! HasSameCuts( xh ) ){
                AliFatal("FATAL: you're trying to sum output that was obtained with different selections!");
            }
            //... if all fine, add this histogram
            GetHistogram()->Add(xh->GetHistogram());
        }
    }
    return (Long64_t) GetHistogram()->GetEntries();
}
//________________________________________________________________
Bool_t AliCascadeResult::HasSameCuts(AliVWeakResult *lCompare, Bool_t lCheckdEdx )
//Function to compare the cuts contained in this result with another
//Returns kTRUE if all selection cuts are identical within 1e-6
//WARNING: Does not check MC association flags 
{
    Bool_t lReturnValue = kTRUE;
    
    if( !lCompare->InheritsFrom(AliCascadeResult::Class() ) ){
        //Apples and oranges! Return kFALSE
        return kFALSE;
    }
    
    AliCascadeResult *lCompareCascade = (AliCascadeResult*) lCompare;
    
    if( fMassHypo != lCompareCascade->GetMassHypothesis() ) lReturnValue = kFALSE;
    
    //V0 Selection Criteria
    if( TMath::Abs( fCutDCANegToPV - lCompareCascade->GetCutDCANegToPV() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutDCAPosToPV - lCompareCascade->GetCutDCAPosToPV() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutDCAV0Daughters - lCompareCascade->GetCutDCAV0Daughters() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutV0CosPA - lCompareCascade->GetCutV0CosPA() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutV0Radius - lCompareCascade->GetCutV0Radius() ) > 1e-6 ) lReturnValue = kFALSE;
    
    //Cascade Selection Criteria
    if( TMath::Abs( fCutDCAV0ToPV - lCompareCascade->GetCutDCAV0ToPV() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutV0Mass - lCompareCascade->GetCutV0Mass() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutDCABachToPV - lCompareCascade->GetCutDCABachToPV() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutDCACascDaughters - lCompareCascade->GetCutDCACascDaughters() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutCascCosPA - lCompareCascade->GetCutCascCosPA() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutCascRadius - lCompareCascade->GetCutCascRadius() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutDCABachToBaryon - lCompareCascade->GetCutDCABachToBaryon() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutBachBaryonCosPA - lCompareCascade->GetCutBachBaryonCosPA() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutMinV0Lifetime - lCompareCascade->GetCutMinV0Lifetime() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutMaxV0Lifetime - lCompareCascade->GetCutMaxV0Lifetime() ) > 1e-6 ) lReturnValue = kFALSE;
    
    if( TMath::Abs( fCutProperLifetime - lCompareCascade->GetCutProperLifetime() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutLeastNumberOfClusters - lCompareCascade->GetCutLeastNumberOfClusters() ) > 1e-6 ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutTPCdEdx - lCompareCascade->GetCutTPCdEdx() ) > 1e-6 && lCheckdEdx ) lReturnValue = kFALSE;
    if( TMath::Abs( fCutXiRejection - lCompareCascade->GetCutXiRejection() ) > 1e-6 ) lReturnValue = kFALSE;
    
    //Variable CascCosPA
    if ( TMath::Abs(fCutUseVariableCascCosPA - lCompareCascade->GetCutUseVarCascCosPA()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarCascCosPA_Exp0Const - lCompareCascade->GetCutVarCascCosPAExp0Const()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarCascCosPA_Exp0Slope - lCompareCascade->GetCutVarCascCosPAExp0Slope()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarCascCosPA_Exp1Const - lCompareCascade->GetCutVarCascCosPAExp1Const()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarCascCosPA_Exp1Slope - lCompareCascade->GetCutVarCascCosPAExp1Slope()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarCascCosPA_Const  - lCompareCascade->GetCutVarCascCosPAConst()) > 1e-6 ) lReturnValue = kFALSE;
    
    //Variable V0CosPA
    if ( TMath::Abs(fCutUseVariableV0CosPA - lCompareCascade->GetCutUseVarV0CosPA()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarV0CosPA_Exp0Const - lCompareCascade->GetCutVarV0CosPAExp0Const()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarV0CosPA_Exp0Slope - lCompareCascade->GetCutVarV0CosPAExp0Slope()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarV0CosPA_Exp1Const - lCompareCascade->GetCutVarV0CosPAExp1Const()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarV0CosPA_Exp1Slope - lCompareCascade->GetCutVarV0CosPAExp1Slope()) > 1e-6 ) lReturnValue = kFALSE;
    if ( TMath::Abs(fCutVarV0CosPA_Const  - lCompareCascade->GetCutVarV0CosPAConst()) > 1e-6 ) lReturnValue = kFALSE;
    
    return lReturnValue;
}
//________________________________________________________________
void AliCascadeResult::Print()
//Function to compare the cuts contained in this result with another
//Returns kTRUE if all selection cuts are identical within 1e-6
//WARNING: Does not check MC association flags
{
    cout<<"========================================"<<endl;
    cout<<"    AliCascadeResult Configuration      "<<endl;
    cout<<"========================================"<<endl;
    cout<<" Object Name........: "<<this->GetName()<<endl;
    cout<<" Histogram Name.....: "<<fHisto->GetName()<<endl;
    if( fMassHypo == AliCascadeResult::kXiMinus      ) cout<<" Mass Hypothesis....: XiMinus"<<endl;
    if( fMassHypo == AliCascadeResult::kXiPlus       ) cout<<" Mass Hypothesis....: XiPlus"<<endl;
    if( fMassHypo == AliCascadeResult::kOmegaMinus   ) cout<<" Mass Hypothesis....: OmegaMinus"<<endl;
    if( fMassHypo == AliCascadeResult::kOmegaPlus    ) cout<<" Mass Hypothesis....: OmegaPlus"<<endl;
    cout<<" Expected mass......: "<<GetMass()<<endl; 
    
    cout<<" DCA Neg to PV......: "<<fCutDCANegToPV<<endl;
    cout<<" DCA Pos to PV......: "<<fCutDCAPosToPV<<endl;
    cout<<" DCA V0 Daughters...: "<<fCutDCAV0Daughters<<endl;
    cout<<" V0 Cos PA..........: "<<fCutV0CosPA<<endl;
    cout<<" Use Var V0CosPA..: "<<fCutUseVariableV0CosPA<<endl;
    if( fCutUseVariableV0CosPA ){
        cout<<" ^--Exp. 0 Const....: "<<fCutVarV0CosPA_Exp0Const<<endl;
        cout<<" ^--Exp. 0 Slope....: "<<fCutVarV0CosPA_Exp0Slope<<endl;
        cout<<" ^--Exp. 1 Const....: "<<fCutVarV0CosPA_Exp1Const<<endl;
        cout<<" ^--Exp. 1 Slope....: "<<fCutVarV0CosPA_Exp1Slope<<endl;
        cout<<" ^--Constant........: "<<fCutVarV0CosPA_Const<<endl;
    }
    cout<<" V0 2D Radius.......: "<<fCutV0Radius<<endl;
    
    cout<<" DCA V0 to PV.......: "<<fCutDCAV0ToPV<<endl;
    cout<<" V0 Mass............: "<<fCutV0Mass<<endl;
    cout<<" DCA Bach to PV.....: "<<fCutDCABachToPV<<endl;
    cout<<" DCA V0 Daughters.: "<<fCutDCACascDaughters<<endl;
    cout<<" Casc Cos PA........: "<<fCutCascCosPA<<endl;
    cout<<" Use Var CascCosPA..: "<<fCutUseVariableCascCosPA<<endl;
    if( fCutUseVariableCascCosPA ){
        cout<<" ^--Exp. 0 Const....: "<<fCutVarCascCosPA_Exp0Const<<endl;
        cout<<" ^--Exp. 0 Slope....: "<<fCutVarCascCosPA_Exp0Slope<<endl;
        cout<<" ^--Exp. 1 Const....: "<<fCutVarCascCosPA_Exp1Const<<endl;
        cout<<" ^--Exp. 1 Slope....: "<<fCutVarCascCosPA_Exp1Slope<<endl;
        cout<<" ^--Constant........: "<<fCutVarCascCosPA_Const<<endl;
    }
    cout<<" Casc 2D Radius.....: "<<fCutCascRadius<<endl;
    cout<<" DCA Bach to Baryon.: "<<fCutDCABachToBaryon<<endl;
    cout<<" Bach Baryon CosPA..: "<<fCutBachBaryonCosPA<<endl;
    cout<<" Min V0 Lifetime....: "<<fCutMinV0Lifetime<<endl;
    cout<<" Max V0 Lifetime....: "<<fCutMaxV0Lifetime<<endl;
    
    cout<<" Proper Lifetime....: "<<fCutProperLifetime<<endl;
    cout<<" Nbr Clusters.......: "<<fCutLeastNumberOfClusters<<endl;
    cout<<" TPC dEdx (sigmas)..: "<<fCutTPCdEdx<<endl;
    cout<<" Xi Rej (for Omega).: "<<fCutXiRejection<<endl;
    
    cout<<" MC PDG Association.: "<<fCutMCPDGCodeAssociation<<endl;
    cout<<" MC Phys Primary....: "<<fCutMCPhysicalPrimary<<endl;
    cout<<" MC Use MC pT, y....: "<<fCutMCUseMCProperties<<endl;
    cout<<" MC Select bump.....: "<<fCutMCSelectBump<<endl;
    cout<<"========================================"<<endl;
    return;
}

//________________________________________________________________
Double_t AliCascadeResult::GetMass () const
//Get Mass under expected hypothesis
//N.B. masses are rounded within 1MeV/c^2 just to simplify binning
{
    Double_t lReturnValue = 0;
    //if( fMassHypo == AliCascadeResult::kXiMinus      ) lThisMass = 1.32171;
    //if( fMassHypo == AliCascadeResult::kXiPlus       ) lThisMass = 1.32171;
    //if( fMassHypo == AliCascadeResult::kOmegaMinus   ) lThisMass = 1.67245;
    //if( fMassHypo == AliCascadeResult::kOmegaPlus    ) lThisMass = 1.67245;
    if( fMassHypo == AliCascadeResult::kXiMinus      ) lReturnValue = 1.322;
    if( fMassHypo == AliCascadeResult::kXiPlus       ) lReturnValue = 1.322;
    if( fMassHypo == AliCascadeResult::kOmegaMinus   ) lReturnValue = 1.672;
    if( fMassHypo == AliCascadeResult::kOmegaPlus    ) lReturnValue = 1.672;
    return lReturnValue;
}

//________________________________________________________________
TString AliCascadeResult::GetParticleName () const
//Get particle name
{
    TString lName = ""; 
    if( fMassHypo == AliCascadeResult::kXiMinus      ) lName = "XiMinus";
    if( fMassHypo == AliCascadeResult::kXiPlus       ) lName = "XiPlus";
    if( fMassHypo == AliCascadeResult::kOmegaMinus   ) lName = "OmegaMinus";
    if( fMassHypo == AliCascadeResult::kOmegaPlus    ) lName = "OmegaPlus";
    return lName;
}



