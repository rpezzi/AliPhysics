///
/// \file AliFemtoCutMonitorPionPion.cxx
///


#include "AliFemtoCutMonitorPionPion.h"
#include "AliFemtoModelHiddenInfo.h"

#include "AliFemtoEvent.h"

static const double PionMass = 0.13956995;

#include <TH1F.h>
#include <TH1I.h>
#include <TH2F.h>
#include <TH2I.h>
#include <TObjArray.h>


AliFemtoCutMonitorPionPion::Event::Event(const bool passing,
                                         const bool is_identical_analysis,
                                         const bool is_mc_analysis,
                                         const bool suffix_output):
  AliFemtoCutMonitor()
  , _centrality(NULL)
  , _multiplicity(NULL)
  , _vertex_z(NULL)
  , _vertex_xy(NULL)
  , _collection_size_pass(NULL)
  , _collection_size_fail(NULL)
  , _identical_collection_size_pass(NULL)
  , _identical_collection_size_fail(NULL)
  , _prev_ev(NULL)
  , _prev_pion_coll_1_size(0)
  , _prev_pion_coll_2_size(0)
{
  const char *title_suffix = (passing ? " (PASS)" : " (FAIL)");

  const TString pf(suffix_output ? passing ? "_P" : "_F" : "");

  _centrality = new TH1F(
    "centrality" + pf,
    TString::Format("Event Centrality%s", title_suffix),
    100, 0, 100.0
  );
  _centrality->Sumw2();

  _multiplicity = new TH1F(
    "multiplicity" + pf,
    "Event Multiplicity",
    100, 0, 10000.0
  );
  _multiplicity->Sumw2();

  _vertex_z = new TH1F(
    "VertexZ" + pf,
    TString::Format("Vertex Z Distribution%s;z (cm);dN/dz", title_suffix),
    128, -15.0f, 15.0f
  );
  _vertex_z->Sumw2();

  _vertex_xy = new TH2F(
    "VertexXY" + pf,
    TString::Format("Vertex XY Distribution%s;x (cm);y (cm); dN/(dx $\\cdot$ dy)", title_suffix),
    48, -0.3f, 0.3f,
    48, -0.3f, 0.6f
  );
  _vertex_xy->Sumw2();

  // only create _collection_size histograms if this is the passing event cut monitor
  if (passing) {
    // if identical - then skip this
    if (is_identical_analysis) {
      _identical_collection_size_pass = new TH1I("collection_size_p",
                                                 "Size of particle collection;"
                                                 "# pions; N_{ev}",
                                                 100, -0.5, 800.5);
      _identical_collection_size_pass->Sumw2();
      _identical_collection_size_fail = (TH1I*)_identical_collection_size_pass->Clone("collection_size_f");
    } else {
      _collection_size_pass = new TH2I("collection_size_p",
                                       "Size of Particle Collection in Passing Events;"
                                       "# pions (1);"
                                       "# pions (2);"
                                       "N_{ev}",
                                       100, -0.5, 800.5,
                                       100, -0.5, 800.5);
      _collection_size_fail = (TH2I*)_collection_size_pass->Clone("collection_size_f");
    }
  }
}

TList*
AliFemtoCutMonitorPionPion::Event::GetOutputList()
{
  TList *olist = new TList();
  TCollection *output = olist;

  output->Add(_centrality);
  output->Add(_multiplicity);
  output->Add(_vertex_z);
  output->Add(_vertex_xy);

  if (_collection_size_pass) {
     output->Add(_collection_size_pass);
     output->Add(_collection_size_fail);
  } else if (_identical_collection_size_pass) {
    output->Add(_identical_collection_size_pass);
    output->Add(_identical_collection_size_fail);
  }

  return olist;
}

void
AliFemtoCutMonitorPionPion::Event::Fill(const AliFemtoEvent* ev)
{
  const Float_t centrality = ev->CentralityV0();
  const Int_t multiplicty = ev->NumberOfTracks();
  const AliFemtoThreeVector vertex = ev->PrimVertPos();

  _centrality->Fill(centrality);
  _multiplicity->Fill(multiplicty);
  _vertex_z->Fill(vertex.z());
  _vertex_xy->Fill(vertex.x(), vertex.y());

  if (ev == _prev_ev) {
    if (_collection_size_pass) {
      _collection_size_pass->Fill(_prev_pion_coll_1_size, _prev_pion_coll_2_size);
      _prev_ev = NULL;
    } else if (_identical_collection_size_pass) {
      _identical_collection_size_pass->Fill(_prev_pion_coll_1_size);
    }
  }

}

void AliFemtoCutMonitorPionPion::Event::EventBegin(const AliFemtoEvent* ev)
{
  if (_collection_size_pass == NULL && _identical_collection_size_pass == NULL) {
    return;
  }
  _prev_ev = ev;
}

void AliFemtoCutMonitorPionPion::Event::EventEnd(const AliFemtoEvent* ev)
{

  // We were not called with the previous event - must have gone to failed
  if (_prev_ev != NULL) {

    // this cut monitor does not monitor collection size
    if (_collection_size_pass != NULL) {
      _collection_size_fail->Fill(_prev_pion_coll_1_size, _prev_pion_coll_2_size);
    } else if (_identical_collection_size_pass) {
      _identical_collection_size_fail->Fill(_prev_pion_coll_1_size);
    }

    _prev_ev = NULL;
  }
}

void
AliFemtoCutMonitorPionPion::Event::Fill(const AliFemtoParticleCollection *coll_1,
                                        const AliFemtoParticleCollection *coll_2)
{
  _prev_pion_coll_2_size = coll_2->size();
  _prev_pion_coll_1_size = coll_1->size();
}

AliFemtoCutMonitorPionPion::Pion::Pion(const bool passing,
                                       const TString& typestr,
                                       const bool is_mc_analysis,
                                       const bool suffix_output):
  AliFemtoCutMonitor()
  , fYPt(NULL)
  , fPtPhi(NULL)
  , fEtaPhi(NULL)
  , fdEdX(NULL)
  , fMinv(NULL)
{
  // Build 'standard' format for histogram titles
  //  <ParticleType> <Title> <Pass/Fail>; <AxisInfo>
  const TString title_format = TString::Format("%s %%s %s; %%s",
                                               typestr.Data(),
                                               (passing ? "(PASS)" : "(FAIL)"));
  const TString pf(suffix_output ? passing ? "_P" : "_F" : "");

  fYPt = new TH2F(
    "eta_Pt" + pf,
    TString::Format(title_format,
                    "#eta  vs  p_{T}",
             /*X*/  "#eta;"
             /*Y*/  "p_{T} (GeV);"
             /*Z*/  "dN/(p_{T} $\\cdot$ \\eta)"),
    140, -1.4, 1.4,
    100, 0, 3.0);
  fYPt->Sumw2();

  fPtPhi = new TH2F(
    "PtPhi" + pf,
    TString::Format(title_format,
                    "Pt vs Phi",
                    "#phi (rads);"
                    "p_{T} (GeV);"),
    144, -TMath::Pi(), TMath::Pi(),
    144,  0.0, 3.0);
  fPtPhi->Sumw2();

  fEtaPhi = new TH2F(
    "EtaPhi" + pf,
    TString::Format(title_format,
                    "#eta vs Phi",
                    "#phi (rads);"
                    "#eta;"),
    144, -TMath::Pi(), TMath::Pi(),
    144, -1.4, 1.4);
  fEtaPhi->Sumw2();

  fdEdX = new TH2F(
    "dEdX" + pf,
    Form(title_format,
         "dE/dx vs p",
         "p (GeV);"
         "dE/dx;"
         "dN/(p_{T} $\\cdot$ dE/dx)"),
    128, 0, 6.0,
    128, 0, 500.0);
  fdEdX->Sumw2();

  fImpact = new TH2F(
    "impact" + pf,
    Form(title_format,
         "Track impact parameter components",
         "z (cm?); r (cm?); N_{tracks}"
        ),
    256, -4.2, 4.2,
    256, -4.2, 4.2);
  fImpact->Sumw2();

  if (is_mc_analysis) {
    fMinv = new TH1F(
      "mc_Mass" + pf,
      TString::Format(title_format, "M_{inv}",
                                    "M_{inv} (GeV);"
                                    "dN/dM"),
      144, 0.0120, 1.158);
    fMinv->Sumw2();
  }

}


TList*
AliFemtoCutMonitorPionPion::Pion::GetOutputList()
{
  TList *olist = new TList();
  TCollection *output = olist;

  output->Add(fYPt);
  output->Add(fPtPhi);
  output->Add(fEtaPhi);
  output->Add(fdEdX);
  output->Add(fImpact);
  if (fMinv) {
    output->Add(fMinv);
  }

  return olist;
};

void AliFemtoCutMonitorPionPion::Pion::Fill(const AliFemtoTrack* track)
{
  const float pz = track->P().z(),
              pt = track->Pt(),
               p = track->P().Mag(),
             phi = track->P().Phi();

  const double energy = ::sqrt(track->P().Mag2() + PionMass * PionMass),
                  eta = 0.5 * ::log((energy + pz) / (energy - pz));


  if (fMinv) {
    fMinv->Fill(track->GetMass());
  }

  fYPt->Fill(eta, pt);
  fPtPhi->Fill(phi, pt);
  fEtaPhi->Fill(phi, eta);
  fdEdX->Fill(p, track->TPCsignal());
  fImpact->Fill(track->ImpactZ(), track->ImpactD());
}

AliFemtoCutMonitorPionPion::Pair::Pair(const bool passing,
                                       const TString& typestr,
                                       const bool is_mc_analysis,
                                       const bool suffix_output):
  AliFemtoCutMonitor()
  , _minv(NULL)
  , fKt(NULL)
  , fAvgSep_pion(NULL)
  , fAvgSep_proton(NULL)
  , fMCTrue_minv(NULL)
  , fMCTrue_kstar(NULL)
{
  const TString title_format = TString::Format("%s %%s %s; %%s",
                                               typestr.Data(),
                                               (passing ? "(PASS)" : "(FAIL)"));
  const TString pf(suffix_output ? passing ? "_P" : "_F" : "");

  _minv = new TH1F(
    "Pair_Minv" + pf,
    TString::Format(title_format, "M_{inv}", "M_{inv} (GeV)"),
    288, 0.0, 8.0);
  _minv->Sumw2();

  fKt = new TH1F(
    "kt" + pf,
    TString::Format(title_format,
                    "k_{T} Distribution",
                    "k_{T} (GeV); dN/k_{T}"),
    144, 0.0, 4.0);
  fKt->Sumw2();

  fAvgSep_pion = new TH1F(
    "AvgSep_pi" + pf,
    TString::Format(title_format,
      "AvgSep Pion Daughter", "Average Separation (cm)"),
    144, 0.0, 20.0);
  fAvgSep_pion->Sumw2();

  fAvgSep_proton = new TH1F(
    "AvgSep_pro" + pf,
    TString::Format(title_format,
      "AvgSep Proton Daughter", "Average Separation (cm)"),
    144, 0.0, 20.0);
  fAvgSep_proton->Sumw2();

  if (is_mc_analysis) {
    fMCTrue_minv = new TH2F(
      "mc_Minv" + pf,
      TString::Format(title_format,
        "Minv True vs Reconstructed",
        "M_{inv}^{r} (GeV);"
        "M_{inv}^{t} (Gev);"),
      144, 1.0, 6.0,
      144, 1.0, 6.0);
    fMCTrue_minv->Sumw2();

    fMCTrue_kstar = new TH2F(
      "mc_Kstar" + pf,
      TString::Format(title_format,
        "K* True vs Reconstructed",
        "K*^{r} (GeV);"
        "K*^{t} (Gev);"),
      144, 0.0, 4.0,
      144, 0.0, 4.0);
    fMCTrue_kstar->Sumw2();
  }
}

void
AliFemtoCutMonitorPionPion::Pair::Fill(const AliFemtoPair *pair)
{
  const float minv = pair->MInv(),
             kstar = pair->KStar();

  _minv->Fill(minv);
  fKt->Fill(pair->KT());

  if (fMCTrue_minv) {
    const AliFemtoModelHiddenInfo *mc_1 = dynamic_cast<const AliFemtoModelHiddenInfo*>(pair->Track1()->HiddenInfo()),
                                  *mc_2 = dynamic_cast<const AliFemtoModelHiddenInfo*>(pair->Track2()->HiddenInfo());
    if (mc_1 && mc_2) {

      const AliFemtoV0 *lambda = pair->Track1()->V0();
      const AliFemtoTrack *pion = pair->Track2()->Track();

      // get true momentums
      const AliFemtoThreeVector &momentum_1 = *mc_1->GetTrueMomentum(),
                                &momentum_2 = *mc_2->GetTrueMomentum();

      // get true mass and calculate true energy
      const float m1 = mc_1->GetMass(),
                  e1 = TMath::Sqrt(m1 * m1 + momentum_1.Mag2()),

                  m2 = mc_2->GetMass(),
                  e2 = TMath::Sqrt(m2 * m2 + momentum_2.Mag2());

      // build momentum four-vectors
      const AliFemtoLorentzVector p1(e1, momentum_1),
                                  p2(e2, momentum_2),
                                  p_sum = p1 + p2,
                                  p_diff = p1 - p2;

      // Minv is just the magnitude of the momentum sum
      fMCTrue_minv->Fill(minv, p_sum.m());

      const float tQ = pow(m1 * m1 - m2 * m2, 2) / p_sum.m2(),
                  q2 = tQ - p_diff.m2(),

                  mc_kstar = q2 > 0
                           ? TMath::Sqrt(q2) / 2.0
                           : 0.0;

      // kstar calculation
      fMCTrue_kstar->Fill(kstar, mc_kstar);

    }
  }

}


TList* AliFemtoCutMonitorPionPion::Pair::GetOutputList()
{
  TList *olist = new TList();
  TCollection *output = olist;

  output->Add(_minv);
  output->Add(fKt);

  if (fMCTrue_kstar) {
    output->Add(fMCTrue_kstar);
  }

  if (fMCTrue_minv) {
    output->Add(fMCTrue_minv);
  }

  return olist;
};