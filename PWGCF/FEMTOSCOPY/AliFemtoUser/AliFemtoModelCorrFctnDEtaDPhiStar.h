///
/// \file AliFemtoModelCorrFctnDEtaDPhiStar.h
///

#pragma once

#ifndef ALIFEMTOMODELCORRFCTNDETADPHISTAR_H
#define ALIFEMTOMODELCORRFCTNDETADPHISTAR_H

#include "AliFemtoModelCorrFctn.h"
#include "AliFemtoPairCutDetaDphi.h"
#include <TString.h>
#include <TH2F.h>
#include <TH1F.h>


typedef struct {Float_t eta, phi_star;} EtaPhiStar;
typedef struct {UInt_t bin_count; Float_t low; Float_t high;} BinData;


/// \class AliFemtoCorrFctnDEtaDPhiStar
/// \brief A two-particle correlation function operating on the pseudorapidity
///        difference (Δη) and angular difference between particle tracks at a
///        given radial distance (Δϕ*), storing both reconstructed and MonteCarlo
///        data.
///
/// \authors: Andrew Kubera, The Ohio State University, andrew.kubera@cern.ch
///
class AliFemtoModelCorrFctnDEtaDPhiStar : public AliFemtoModelCorrFctn {
public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
  /// All parameters required for construction
  struct Parameters {
    TString title;
    BinData phi, eta;
    float radius;
    bool group_output;

    static Parameters Default;
    Parameters WithTitle(const char *title) { Parameters res = *this; res.title = title; return res; }
    Parameters WithPhiBins(UInt_t x) {Parameters res(*this); res.phi.bin_count = x; return res; }
    Parameters WithEtaBins(UInt_t x) {Parameters res(*this); res.eta.bin_count = x; return res; }
    Parameters WithRadius(Float_t x) {Parameters res(*this); res.radius = x; return res; }

  };
#pragma GCC diagnostic pop


  /// Structure used to build a Parameters object and return the AliFemtoModelCorrFctnDEtaDPhiStar.
  struct Builder {
    AliFemtoModelCorrFctnDEtaDPhiStar::Parameters params;

    // construct with default parameters
    Builder(): params(Parameters::Default) {};
    Builder(const Parameters &p): params(p) {};

    operator AliFemtoModelCorrFctnDEtaDPhiStar*() const { return new AliFemtoModelCorrFctnDEtaDPhiStar(params); }

    Builder Title(const char *title) { return Builder(params.WithTitle(title)); }
    Builder& Phi(UInt_t bin_count, Float_t low, Float_t high) { params.phi = {bin_count, low, high}; return *this; }
    Builder& Eta(UInt_t bin_count, Float_t low, Float_t high) { params.eta = {bin_count, low, high}; return *this; }
    Builder& Radius(Float_t radius) { params = params.WithRadius(radius); return *this; }
    Builder& GroupOutput(Bool_t group) { params.group_output = group; return *this; }
    AliFemtoModelCorrFctnDEtaDPhiStar* Build() const;
  };


public:
  /// Construct from a Parameters object - with all the settings.
  AliFemtoModelCorrFctnDEtaDPhiStar(const Parameters&);
  /// Construct with a title and data
  AliFemtoModelCorrFctnDEtaDPhiStar(const char* title, const UInt_t phi_bins, const UInt_t eta_bins);

  /// Destructor
  virtual ~AliFemtoModelCorrFctnDEtaDPhiStar();

  virtual AliFemtoString Report();

  virtual void AddRealPair(AliFemtoPair* aPair);
  virtual void AddMixedPair(AliFemtoPair* aPair);

  virtual void Finish();

  void WriteHistos();
  virtual TList* GetOutputList();
  virtual TList* AppendOutputList(TList &);

  static Builder Build() { return Builder(); };

  virtual void EventBegin(const AliFemtoEvent *);

  /// Return struct containing the values of interest from a pair of particles.
  virtual EtaPhiStar GetDeltaEtaDeltaPhiStar(const AliFemtoTrack&, const AliFemtoTrack&);

protected:

  /// Radius in meters to the phi calculation point
  Float_t fRadius;

  /// Sign of the magnetic field
  Float_t fCurrentMagneticField;

  /// Determines whether output histograms should be grouped together
  Bool_t fGroupOutputList;

  /// Output collection name
  TString fTitle;

  /// Numerator of ΔηΔϕ* true function
  TH2F *fDPhiStarDEtaNumeratorTrue;
  /// Numerator of ΔηΔϕ* fake function
  TH2F *fDPhiStarDEtaNumeratorFake;
  /// Denominator of ΔηΔϕ* function
  TH2F *fDPhiStarDEtaDenominator;

  /// Numerator of colinear ΔηΔϕ* function
  TH2F *fDPhiStarDEtaColNumerator;
  /// Denominator of colinear ΔηΔϕ* function
  TH2F *fDPhiStarDEtaColDenominator;

  /// Numerator of Δϕ* true correlation
  TH1F *fDPhiStarNumeratorTrue;
  /// Numerator of Δϕ* fake correlation
  TH1F *fDPhiStarNumeratorFake;
  // Denominator of Δϕ* correlation
  TH1F *fDPhiStarDenominator;

  /// Numerator of colinearity true correlation
  TH1F *fDCosNumeratorTrue;
  /// Numerator of colinearity fake correlation
  TH1F *fDCosNumeratorFake;
  /// Denominator of colinearity correlation
  TH1F *fDCosDenominator;

  /// Numerator of Δϕ* correlation vs. Pt min
  TH2F *fDPhiStarPtNumerator;
  /// Denominator of Δϕ* correlation vs. Pt min
  TH2F *fDPhiStarPtDenominator;

  /// Numerator of colinearity correlation vs. Pt min
  TH2F *fDCosPtNumerator;
  /// Denominator of colinearity correlation vs. Pt min
  TH2F *fDCosPtDenominator;
};

inline
void
AliFemtoModelCorrFctnDEtaDPhiStar::Finish()
{
}


inline
void
AliFemtoModelCorrFctnDEtaDPhiStar::EventBegin(const AliFemtoEvent *event)
{
  fCurrentMagneticField = event->MagneticField();

  // Correct AliFemto units error for magnetic field (back to Tesla)
  // TODO: Fix this bug in AliFemtoEventReaderAOD::CopyAODtoFemtoEvent
  if (fabs(fCurrentMagneticField) < 1e-10) {
    fCurrentMagneticField *= 1e13;
  }

  AliFemtoModelCorrFctn::EventBegin(event);
}

inline
EtaPhiStar
AliFemtoModelCorrFctnDEtaDPhiStar::GetDeltaEtaDeltaPhiStar(
  const AliFemtoTrack &track1, const AliFemtoTrack &track2)
{
  const AliFemtoThreeVector p1 = track1.P(),
                            p2 = track2.P();
  const Short_t charge1 = track1.Charge(),
                charge2 = track2.Charge();

  const Float_t delta_eta = AliFemtoPairCutDetaDphi::CalculateDEta(p1, p2);
  const Float_t delta_phi_star = AliFemtoPairCutDetaDphi::CalculateDPhiStar(
    p1, charge1,
    p2, charge2,
    fRadius,
    fCurrentMagneticField
  );
  return {delta_eta, delta_phi_star};
}

#endif
