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
#include <algorithm>
#include <iostream>
#include <vector>
#include <TClonesArray.h>

#include "AliVEvent.h"
#include "AliLog.h"

#include "AliTLorentzVector.h"
#include "AliParticleContainer.h"

/// \cond CLASSIMP
ClassImp(AliParticleContainer)
/// \endcond

/**
 * Default constructor.
 */
AliParticleContainer::AliParticleContainer():
  AliEmcalContainer(),
  fMinDistanceTPCSectorEdge(-1),
  fCharge(-1),
  fGeneratorIndex(-1)
{
  fClassName = "AliVParticle";
}

/**
 * Standard constructor.
 * @param name
 */
AliParticleContainer::AliParticleContainer(const char *name) :
  AliEmcalContainer(name),
  fMinDistanceTPCSectorEdge(-1),
  fCharge(-1),
  fGeneratorIndex(-1)
{
  fClassName = "AliVParticle";
}

/**
 * Get the leading particle in the container. If "p" is contained in the parameter opt,
 * then the absolute momentum is use instead of the transverse momentum.
 * @param[in] opt Options for the selection of the leading particle
 * @return Leading particle in the container
 */
AliVParticle* AliParticleContainer::GetLeadingParticle(const char* opt) 
{
  TString option(opt);
  option.ToLower();

  Int_t tempID = fCurrentID;
  ResetCurrentID();

  AliVParticle *partMax = GetNextAcceptParticle();
  AliVParticle *part = 0;

  if (option.Contains("p")) {
    while ((part = GetNextAcceptParticle())) {
      if (part->P() > partMax->P()) partMax = part;
    }
  }
  else {
    while ((part = GetNextAcceptParticle())) {
      if (part->Pt() > partMax->Pt()) partMax = part;
    }
  }

  fCurrentID = tempID;

  return partMax;
}

/**
 * Get \f$ i^{th} \f$ particle in the container.
 * @param[in] i Index of the particle to access
 * @return Parrticle at the given index (NULL if the index is out of range)
 */
AliVParticle* AliParticleContainer::GetParticle(Int_t i) const 
{
  //
  if (i == -1) i = fCurrentID;
  if (i < 0 || i >= this->fClArray->GetEntriesFast()) return 0;
  AliVParticle *vp = static_cast<AliVParticle*>(fClArray->At(i));
  return vp;
}

/**
 * Get \f$ i^{th} \f$ particle in the container if it is accepted.
 * In case it is not accepted a nullpointer is returned.
 * @param[in] i Index of the particle
 * @return Particle at the index if it is accepted, NULL otherwise
 */
AliVParticle* AliParticleContainer::GetAcceptParticle(Int_t i) const
{
  UInt_t rejectionReason = 0;
  if (i == -1) i = fCurrentID;
  if (AcceptParticle(i, rejectionReason)) {
      return GetParticle(i);
  }
  else {
    AliDebug(2,"Particle not accepted.");
    return 0;
  }
}

/**
 * Iterator over accepted particles in the container. Get the next accepted
 * particle in the array. If the end is reached, NULL is returned.
 * @deprecated Only for backward compatibility - use accept_iterator instead
 * @return Next accepted particle in the array (NULL if the end is reached)
 */
AliVParticle* AliParticleContainer::GetNextAcceptParticle()
{
  const Int_t n = GetNEntries();
  AliVParticle *p = 0;
  do {
    fCurrentID++;
    if (fCurrentID >= n) break;
    p = GetAcceptParticle(fCurrentID);
  } while (!p);

  return p;
}

/**
 * Iterator over all particles in the container. Get the next particle in
 * the array. If the end is reached, NULL is returned.
 * @deprecated Only for backward compatibility - use all_iterator instead.
 * @return Next particle in the array (NULL if the end is reached)
 */
AliVParticle* AliParticleContainer::GetNextParticle()
{
  const Int_t n = GetNEntries();
  AliVParticle *p = 0;
  do {
    fCurrentID++;
    if (fCurrentID >= n) break;
    p = GetParticle(fCurrentID);
  } while (!p);

  return p;
}

/**
 * Retrieve momentum information of a particle (part) and fill a TLorentzVector
 * with it. In case the optional parameter mass is provided, it is used as mass
 * hypothesis, otherwise the mass hypothesis from the particle itself is used.
 * @param[out] mom Momentum vector to be filled
 * @param[in] part Particle from which the momentum information is obtained.
 * @param[in] mass (Optional) Mass hypothesis
 * @return
 */
Bool_t AliParticleContainer::GetMomentum(TLorentzVector &mom, const AliVParticle* part, Double_t mass) const
{
  if (part) {
    if (mass < 0) mass = part->M();
    mom.SetPtEtaPhiM(part->Pt(), part->Eta(), part->Phi(), mass);
    return kTRUE;
  }
  else {
    mom.SetPtEtaPhiM(0, 0, 0, 0);
    return kFALSE;
  }
}

/**
 * Fills a TLorentzVector with the momentum information of the particle provided
 * under a global mass hypothesis.
 * @param[out] mom Momentum vector of the particle provided
 * @param[in] part Particle from which to obtain the momentum information
 * @return Always true
 */
Bool_t AliParticleContainer::GetMomentum(TLorentzVector &mom, const AliVParticle* part) const
{
  return GetMomentum(mom,part,fMassHypothesis);
}

/**
 * Fills a TLorentzVector with the monentum infomation of the
 * \f$ i^{th} \f$ particle in the container, using a global
 * mass hypothesis. In case the provided index is out of
 * range, false is returned as return value.
 * @param[out] mom Momentum vector of the \f$ i^{th} \f$ particle in the array
 * @param[in] i Index of th particle to check
 * @return True if the request was successfull, false otherwise
 */
Bool_t AliParticleContainer::GetMomentum(TLorentzVector &mom, Int_t i) const
{
  Double_t mass = fMassHypothesis;

  if (i == -1) i = fCurrentID;
  AliVParticle *vp = GetParticle(i);
  if (vp) {
    if (mass < 0) mass = vp->M();
    mom.SetPtEtaPhiM(vp->Pt(), vp->Eta(), vp->Phi(), mass);
    return kTRUE;
  }
  else {
    mom.SetPtEtaPhiM(0, 0, 0, 0);
    return kFALSE;
  }
}

/**
 * Fills a TLorentzVector with the monentum infomation of the
 * next particle in the container, using a global mass hypothesis.
 * In case the iterator reached the end of the array, false
 * is returned as return value.
 * @deprecated Old style iterator - use all_iterator instead
 * @param[out] mom Momentum vector of the next particle
 * @return True if the request was successfull, false otherwise
 */
Bool_t AliParticleContainer::GetNextMomentum(TLorentzVector &mom)
{
  Double_t mass = fMassHypothesis;

  AliVParticle *vp = GetNextParticle();
  if (vp) {
    if (mass < 0) mass = vp->M();
    mom.SetPtEtaPhiM(vp->Pt(), vp->Eta(), vp->Phi(), mass);
    return kTRUE;
  }
  else {
    mom.SetPtEtaPhiM(0, 0, 0, 0);
    return kFALSE;
  }
}

/**
 * Fills a TLorentzVector with the monentum infomation of the
 * \f$ i^{th} \f$ accepted particle in the container, using a
 * global mass hypothesis. In case the provided index is out of
 * range, or the particle under the index is not accepted, false
 * is returned as return value.
 * @param[out] mom Momentum vector of the accepted particle
 * @param[in] i Index to check
 * @return True if the request was successfull, false otherwise
 */
Bool_t AliParticleContainer::GetAcceptMomentum(TLorentzVector &mom, Int_t i) const
{
  Double_t mass = fMassHypothesis;

  if (i == -1) i = fCurrentID;
  AliVParticle *vp = GetAcceptParticle(i);
  if (vp) {
    if (mass < 0) mass = vp->M();
    mom.SetPtEtaPhiM(vp->Pt(), vp->Eta(), vp->Phi(), mass);
    return kTRUE;
  }
  else {
    mom.SetPtEtaPhiM(0, 0, 0, 0);
    return kFALSE;
  }
}

/**
 * Fills a TLorentzVector with the monentum infomation of the
 * next accepted particle in the container, using a global
 * mass hypothesis. In case the iteration reached the end of
 * the array, false is returned as return value.
 * @deprecated Old style iterator - use accept_iterator instead
 * @param[out] mom Momentum vector of the next particle in the array
 * @return True if the request was successfull, false (no more entries) otherwise
 */
Bool_t AliParticleContainer::GetNextAcceptMomentum(TLorentzVector &mom)
{
  Double_t mass = fMassHypothesis;

  AliVParticle *vp = GetNextAcceptParticle();
  if (vp) {
    if (mass < 0) mass = vp->M();
    mom.SetPtEtaPhiM(vp->Pt(), vp->Eta(), vp->Phi(), mass);
    return kTRUE;
  }
  else {
    mom.SetPtEtaPhiM(0, 0, 0, 0);
    return kFALSE;
  }
}

/**
 * Perform full particle selection consisting of kinematical and particle property
 * selection on the particle provided. In case the particle is rejected, the
 * reason for the rejection is encoded in the bitmap rejectionReason.
 * @param vp Particle for which to perform the selection
 * @param rejectionReason Bitmap with the reason why the particle was accepted.
 * Note: The value is not set to 0 in the function in order to combine the information
 * with other selection steps.
 * @return True if the particle is accepted, false otherwise.
 */
Bool_t AliParticleContainer::AcceptParticle(const AliVParticle *vp, UInt_t &rejectionReason) const
{
  Bool_t r = ApplyParticleCuts(vp, rejectionReason);
  if (!r) return kFALSE;

  AliTLorentzVector mom;

  Int_t id = fClArray->IndexOf(vp);
  if (id >= 0) {
    GetMomentum(mom, id);
  }
  else {
    GetMomentum(mom, vp);
  }

  return ApplyKinematicCuts(mom, rejectionReason);
}

/**
 * Perform full particle selection consisting of kinematical and particle property
 * selection on the \f$ i^{th} \f$ particle in the array. In case the particle is
 * rejected, the reason for the rejection is encoded in the bitmap rejectionReason.
 * @param[in] i Index of the particle to select.
 * @param[out] rejectionReason Bitmap with the reason why the particle was accepted.
 * Note: The value is not set to 0 in the function in order to combine the information
 * with other selection steps.
 * @return True if the particle was accepted, false otherwise
 */
Bool_t AliParticleContainer::AcceptParticle(Int_t i, UInt_t &rejectionReason) const
{
  Bool_t r = ApplyParticleCuts(GetParticle(i), rejectionReason);
  if (!r) return kFALSE;

  AliTLorentzVector mom;
  GetMomentum(mom, i);

  return ApplyKinematicCuts(mom, rejectionReason);
}

/**
 * Apply cuts on the particle properties. Implemented are
 * - Monte-Carlo label
 * - Charge
 * - Generator index
 *
 * @param[in] vp Particle for which the selection is performed
 * @param[out] rejectionReason Bitmap with the reason why the particle was accepted.
 * Note: The value is not set to 0 in the function in order to combine the information
 * with other selection steps.
 * @return True if the particle is accepted, False otherwise
 */
Bool_t AliParticleContainer::ApplyParticleCuts(const AliVParticle* vp, UInt_t &rejectionReason) const
{
  if (!vp) {
    rejectionReason |= kNullObject;
    return kFALSE;
  }

  if (vp->TestBits(fBitMap) != (Int_t)fBitMap) {
    rejectionReason |= kBitMapCut;
    return kFALSE;
  }

  if (fMinMCLabel >= 0 && TMath::Abs(vp->GetLabel()) > fMinMCLabel) {
    rejectionReason |= kMCLabelCut;
    return kFALSE;
  }

  if (fMaxMCLabel >= 0 && TMath::Abs(vp->GetLabel()) < fMaxMCLabel) {
    rejectionReason |= kMCLabelCut;
    return kFALSE;
  }

  if (fCharge>=0 && fCharge != vp->Charge()) {
    rejectionReason |= kChargeCut;
    return kFALSE;
  }

  if (fGeneratorIndex >= 0 && fGeneratorIndex != vp->GetGeneratorIndex()) {
    rejectionReason |= kMCGeneratorCut;
    return kFALSE;
  }

  return kTRUE;
}

/**
 * Apply kinematical cuts to the momentum vector provided. In addition
 * to the standard kinematical cuts in \f$ p_{t} \f$, \f$ \eta \f$ and
 * \f$ \phi \f$, implemented in the AliEmcalContainer::ApplyKinematicCuts,
 * also the distance to the TPC sector boundary is checked.
 * @param[in] mom Momentum vector for which
 * @param[out] rejectionReason  Bitmap with the reason why the particle was accepted.
 * Note: The value is not set to 0 in the function in order to combine the information
 * with other selection steps.
 * @return True if the momentum vector was selected under the given cuts, false otherwise
 */
Bool_t AliParticleContainer::ApplyKinematicCuts(const AliTLorentzVector& mom, UInt_t &rejectionReason) const
{
  if(fMinDistanceTPCSectorEdge>0.) {
    const Double_t pi = TMath::Pi();
    const Double_t kSector = pi/9;
    Double_t phiDist = TMath::Abs(mom.Phi() - TMath::FloorNint(mom.Phi()/kSector)*kSector);
    if(phiDist<fMinDistanceTPCSectorEdge) {
      rejectionReason |= kMinDistanceTPCSectorEdgeCut;
      return kFALSE;
    }
  }

  return AliEmcalContainer::ApplyKinematicCuts(mom, rejectionReason);
}

/**
 * Get number of accepted particles. In order to get this number,
 * the selection has to be applied to each particle within this
 * function.
 * @return Number of selected particles under the given particle selection
 */
Int_t AliParticleContainer::GetNAcceptedParticles() const
{
  Int_t nPart = 0;
  for(int ipart = 0; ipart < this->GetNParticles(); ipart++){
    UInt_t rejectionReason = 0;
    if(this->AcceptParticle(ipart, rejectionReason)) nPart++;
  }
  return nPart;
}

/**
 * Set the name of the class of the objets inside the underlying array.
 * @param[in] clname Name of the class of the object inside the underlying array.
 */
void AliParticleContainer::SetClassName(const char *clname)
{
  // Set the class name

  TClass cls(clname);
  if (cls.InheritsFrom("AliVParticle")) fClassName = clname;
  else AliError(Form("Unable to set class name %s for a AliParticleContainer, it must inherits from AliVParticle!",clname));
}

/**
 * Make a title of the container name based on the min \f$ p_{t} \f$ used
 * in the particle selection process.
 * @return Title of the container
 */
const char* AliParticleContainer::GetTitle() const
{
  static TString trackString;

  if (GetMinPt() == 0) {
    trackString = TString::Format("%s_pT0000", GetArrayName().Data());
  }
  else if (GetMinPt() < 1.0) {
    trackString = TString::Format("%s_pT0%3.0f", GetArrayName().Data(), GetMinPt()*1000.0);
  }
  else {
    trackString = TString::Format("%s_pT%4.0f", GetArrayName().Data(), GetMinPt()*1000.0);
  }

  return trackString.Data();
}

/**
 * Create standard (forward) iterator over all clusters in the
 * container, for the beginning of the iteration.
 * @return Standard (forward) beginning iterator
 */
AliParticleContainer::all_iterator AliParticleContainer::begin() const {
  return all_iterator(this, 0, true);
}

/**
 * Create standard (forward) iterator over all clusters in the
 * container, marking the end of the iteration.
 * @return Standard (forward) end iterator
 */
AliParticleContainer::all_iterator AliParticleContainer::end() const {
  return all_iterator(this, this->GetNParticles(), true);
}

/**
 * Create standard (forward) iterator over all clusters in the
 * container, for the beginning of the iteration.
 * @return Reverse (backward) beginning operator
 */
AliParticleContainer::all_iterator AliParticleContainer::rbegin() const {
  return all_iterator(this, this->GetNParticles(), false);
}

/**
 * Create reverse (backward) iterator over all clusters in the
 * container, marking the end of the iteration.
 * @return Reverse (backward) end iterator
 */
AliParticleContainer::all_iterator AliParticleContainer::rend() const {
  return all_iterator(this, -1, false);
}
/**
 * Create standard (forward) iterator over accepted clusters in the
 * container, for the beginning of the iteration.
 * @return Standard (forward) beginning iterator
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_begin() const{
  return accept_iterator(this, 0, true);
}

/**
 * Create standard (forward) iterator over accepted clusters in the
 * container, marking the end of the iteration.
 * @return Standard (forward) end iterator
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_end() const{
  return accept_iterator(this, this->GetNAcceptedParticles(), true);
}

/**
 * Create standard (forward) iterator over accepted clusters in the
 * container, for the beginning of the iteration.
 * @return Reverse (backward) beginning operator
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_rbegin() const{
  return accept_iterator(this, this->GetNAcceptedParticles()-1, false);
}

/**
 * Create reverse (backward) iterator over accepted clusters in the
 * container, marking the end of the iteration.
 * @return Reverse (backward) end iterator
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_rend() const{
  return accept_iterator(this, -1, false);
}

///////////////////////////////////////////////////////////////
///
/// Content of accept_iterator
///
///////////////////////////////////////////////////////////////

/**
 * Constructor, inializing the iterator with
 * - Particle container to iterate over
 * - Starting position for the iteration
 * - Direction of the iteration (default: forward)
 * This constructor also builds a lookup table of
 * accepted clusters.
 *
 * Note: Iterators should be created by the object
 * it iterates over and not by hand.
 *
 * @param[in] cont Container to be iterated over (not modified)
 * @param[in] start Start position for the iteration
 * @param[in] forward If true iterator is a forward iterator
 */
AliParticleContainer::accept_iterator::accept_iterator(
    const AliParticleContainer *cont,
    int start,
    bool forward):
    fkContainer(cont),
    fAcceptIndices(),
    fCurrentPos(start),
    fForward(forward)
{
  fAcceptIndices.Set(fkContainer->GetNAcceptedParticles());
  UInt_t rejection = 0;
  Int_t mappos = 0;
  for(int i = 0; i < fkContainer->GetNEntries(); i++){
    if(fkContainer->AcceptParticle(i, rejection)){
      fAcceptIndices[mappos++] = i;
    }
  }
}

/**
 * Copy constructor. For the underlying container the
 * pointer is copied.
 * @param[in] other Reference for the copy
 */
AliParticleContainer::accept_iterator::accept_iterator(const accept_iterator &other):
    fkContainer(other.fkContainer),
    fAcceptIndices(other.fAcceptIndices),
    fCurrentPos(other.fCurrentPos),
    fForward(other.fForward)
{
}

/**
 * Assignmet operator, copying properties of the
 * reference iterator
 * @param[in] other Reference for the assignment
 * @return This object after the assingment
 */
AliParticleContainer::accept_iterator &AliParticleContainer::accept_iterator::operator=(const AliParticleContainer::accept_iterator &other){
  if(this != &other){
    fkContainer = other.fkContainer;
    fAcceptIndices = other.fAcceptIndices;
    fCurrentPos = other.fCurrentPos;
    fForward = other.fForward;
  }
  return *this;
}

/**
 * Comparison operator. The comparison is performed
 * based on the current position of the iterator.
 * @param[in] other Object to compare to.
 * @return If true the positions of the two iterators match
 */
bool AliParticleContainer::accept_iterator::operator!=(const AliParticleContainer::accept_iterator &other) const{
  return fCurrentPos != other.fCurrentPos;
}

/**
 * Incrementation prefix operator. Incrementing/Decrementing
 * the postion of the iterator in the array based on the direction.
 * @return State of the iterator (reference) after iteration.
 */
AliParticleContainer::accept_iterator &AliParticleContainer::accept_iterator::operator++(){
  if(fForward){
    fCurrentPos++;
  } else {
    fCurrentPos--;
  }
  return *this;
}

/**
 * Incrementation postfix operator. Incrementing/Decrementing
 * the postion of the iterator in the array based on the direction.
 * @param[in] Not used, only defining the operator as postfix operator
 * @return State of the iterator (copy) before iteration
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_iterator::operator++(int){
  AliParticleContainer::accept_iterator result(*this);
  this->operator++();
  return result;
}

/**
 * Decrementation prefix operator. Decrementing/Incrementing
 * the postion of the iterator in the array based on the direction.
 * @return State of the iterator (reference) after iteration.
 */
AliParticleContainer::accept_iterator &AliParticleContainer::accept_iterator::operator--(){
  if(fForward){
    fCurrentPos--;
  } else {
    fCurrentPos++;
  }
  return *this;
}

/**
 * Decrementation postfix operator. Decrementing/Incrementing
 * the postion of the iterator in the array based on the direction.
 * @param[in] Not used, only defining the operator as postfix operator
 * @return State of the iterator (copy) before iteration
 */
AliParticleContainer::accept_iterator AliParticleContainer::accept_iterator::operator--(int){
  AliParticleContainer::accept_iterator result(*this);
  this->operator++();
  return result;
}

/**
 * Dereferencing operator. Providing access to the accepted cluster
 * at the current position.
 * @return Accepted cluster at the given position (NULL if iterator is out of range)
 */
AliVParticle *AliParticleContainer::accept_iterator::operator*() const{
  if(fCurrentPos < 0 || fCurrentPos >= fAcceptIndices.GetSize()){
    return NULL;
  }
  return fkContainer->GetParticle(fAcceptIndices[fCurrentPos]);
}

///////////////////////////////////////////////////////////////
///
/// Content of all_iterator
///
///////////////////////////////////////////////////////////////

/**
 * Constructor, inializing the iterator with
 * - Particle container to iterate over
 * - Starting position for the iteration
 * - Direction of the iteration (default: forward)
 *
 * Note: Iterators should be created by the object
 * it iterates over and not by hand.
 *
 * @param[in] cont Container to be iterated over (not modified)
 * @param[in] startpos Start position for the iteration
 * @param[in] forward If true iterator is a forward iterator
 */
AliParticleContainer::all_iterator::all_iterator(
    const AliParticleContainer *cont,
    int startpos,
    bool forward):
    fkContainer(cont),
    fCurrentPos(startpos),
    fForward(forward)
{
}

/**
 * Copy constructor. For the underlying container the
 * pointer is copied.
 * @param[in] other Reference for the copy
 */
AliParticleContainer::all_iterator::all_iterator(const all_iterator &other):
    fkContainer(other.fkContainer),
    fCurrentPos(other.fCurrentPos),
    fForward(other.fForward)
{
}

/**
 * Assignmet operator, copying properties of the
 * reference iterator
 * @param[in] other Reference for the assignment
 * @return This object after the assingment
 */
AliParticleContainer::all_iterator &AliParticleContainer::all_iterator::operator=(const AliParticleContainer::all_iterator &other){
  if(&other != this){
    fkContainer = other.fkContainer;
    fCurrentPos = other.fCurrentPos;
    fForward = other.fForward;
  }
  return *this;
}

/**
 * Comparison operator. The comparison is performed
 * based on the current position of the iterator.
 * @param[in] other Object to compare to.
 * @return If true the positions of the two iterators match
 */
bool AliParticleContainer::all_iterator::operator!=(const AliParticleContainer::all_iterator &other) const{
  return other.fCurrentPos != fCurrentPos;
}

/**
 * Incrementation prefix operator. Incrementing/Decrementing
 * the postion of the iterator in the array based on the direction.
 * @return State of the iterator (reference) after iteration.
 */
AliParticleContainer::all_iterator &AliParticleContainer::all_iterator::operator++(){
  if(fForward){
    fCurrentPos++;
  } else {
    fCurrentPos--;
  }
  return *this;
}

/**
 * Incrementation postfix operator. Incrementing/Decrementing
 * the postion of the iterator in the array based on the direction.
 * @param[in] Not used, only defining the operator as postfix operator
 * @return State of the iterator (copy) before iteration
 */
AliParticleContainer::all_iterator AliParticleContainer::all_iterator::operator++(int){
  all_iterator tmp(*this);
  operator++();
  return tmp;
}

/**
 * Decrementation prefix operator. Decrementing/Incrementing
 * the postion of the iterator in the array based on the direction.
 * @return State of the iterator (reference) after iteration.
 */
AliParticleContainer::all_iterator &AliParticleContainer::all_iterator::operator--(){
  if(fForward){
    fCurrentPos--;
  } else {
    fCurrentPos++;
  }
  return *this;
}

/**
 * Decrementation postfix operator. Decrementing/Incrementing
 * the postion of the iterator in the array based on the direction.
 * @param[in] Not used, only defining the operator as postfix operator
 * @return State of the iterator (copy) before iteration
 */
AliParticleContainer::all_iterator AliParticleContainer::all_iterator::operator--(int){
  all_iterator tmp(*this);
  operator--();
  return tmp;
}

/**
 * Dereferencing operator. Accessing cluster at the current
 * position of the iterator in the cluster container
 * @return
 */
AliVParticle *AliParticleContainer::all_iterator::operator*() const{
  if(fCurrentPos >= 0 && fCurrentPos < fkContainer->GetNParticles())
    return fkContainer->GetParticle(fCurrentPos);
  return NULL;
}

/******************************************
 * Unit tests                             *
 ******************************************/

int TestParticleContainerIterator(const AliParticleContainer *const cont, int iteratorType, bool verbose){
  std::vector<AliVParticle *> reference, variation;
  AliVParticle *test = NULL;
  for(int iclust = 0; iclust < cont->GetNParticles(); iclust++){
    test = cont->GetParticle(iclust);
    if(!iteratorType){
      UInt_t rejectionReason = 0;
      if(!cont->AcceptParticle(test, rejectionReason)) continue;
    }
    reference.push_back(test);
  }

  if(!iteratorType){
    // test accept iterator
    for(AliParticleContainer::accept_iterator iter = cont->accept_begin(); iter != cont->accept_end(); ++iter){
      variation.push_back(*iter);
    }
  } else {
    // test all iterator
    for(AliParticleContainer::all_iterator iter = cont->begin(); iter != cont->end(); ++iter){
      variation.push_back(*iter);
    }
  }

  if(verbose) {
    // Printing cluster addresses:
    if(reference.size() < 30){
      std::cout << "Paritcles in reference container: " << std::endl;
      std::cout << "===========================================" << std::endl;
      for(std::vector<AliVParticle *>::iterator refit = reference.begin(); refit != reference.end(); ++refit){
        std::cout << "Address: " << *refit << std::endl;
      }
    }
    if(variation.size() < 30){
      std::cout << "Paritcles in test container: " << std::endl;
      std::cout << "===========================================" << std::endl;
      for(std::vector<AliVParticle *>::iterator varit = variation.begin(); varit != variation.end(); ++varit){
        std::cout << "Address: " << *varit << std::endl;
      }
    }
  }

  int testresult = 0;
  // compare distributions: all particles in one vector needs to be found in the other vector and vice versa
  bool failure = false;
  // first test: cleck if all particles are found by the iterator
  for(std::vector<AliVParticle *>::iterator clit = reference.begin(); clit != reference.end(); ++clit){
    if(std::find(variation.begin(), variation.end(), *clit) == variation.end()) {
      if(verbose)
        std::cout << "Could not find particle with address " << *clit << " in test container" << std::endl;
      failure = true;
      break;
    }
  }
  if(!failure){
    // second test: check if there are no extra particles found
    for(std::vector<AliVParticle *>::iterator clit = variation.begin(); clit != variation.end(); ++clit){
      if(std::find(reference.begin(), reference.end(), *clit) == reference.end()) {
        if(verbose)
          std::cout << "Could not find particle with address " << *clit << " in reference container" << std::endl;
        failure = true;
        break;
      }
    }
    if(failure) testresult = 2;
  } else testresult = 1;
  if(verbose) {
    std::cout << "Unit test particle container, iterator type " << iteratorType << std::endl;
    std::cout << "Number of expected particles: " << reference.size() << ", number of found particles: " << variation.size() << std::endl;
    std::cout << "Test result: " << testresult << std::endl;
    std::cout << "-----------------------------------------------------------------------" << std::endl;
  }
  return testresult;
}