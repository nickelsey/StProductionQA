#include "StProductionTrackQA.hh"

#include "St_base/StMessMgr.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StEvent/StBTofHeader.h"
#include "StEvent/StZdcTriggerDetector.h"
#include "StEvent/StEvent.h"
#include "StEvent/StTpcHitCollection.h"

#include "TMath.h"

ClassImp(StProductionTrackQA);

StProductionTrackQA::StProductionTrackQA(std::string outputFile, string name) : StMaker(name.c_str()) {
  muDstMaker_ = nullptr;
  muDst_ = nullptr;
  muEvent_ = nullptr;
  out_ = new TFile(outputFile.c_str(), "RECREATE");
}

StProductionTrackQA::~StProductionTrackQA() {

}

Int_t StProductionTrackQA::Init() {
  if (InitInput() != kStOK)
    return kStFatal;
  if (InitOutput() != kStOK)
    return kStFatal;
  return StMaker::Init();
}

Int_t StProductionTrackQA::Make() {
  
  // load the matching miniMC event
  if (LoadEvent() == false) {
    LOG_ERROR << "Error with loading muDst" << endm;
    return kStErr;
  }
  
  if (!SelectVertex())
    return kStOK;
  
  if (!event_cuts_.AcceptEvent(muEvent_))
    return kStOK;
  
  runid_ = muEvent_->runId();
  eventid_ = muEvent_->eventId();
  nvertices_ = muDst_->numberOfPrimaryVertices();
  zdcrate_ = muEvent_->runInfo().zdcCoincidenceRate();
  nglobal_ = muDst_->globalTracks()->GetEntries();
  pxl_ = muEvent_->numberOfPxlInnerHits() + muEvent_->numberOfPxlOuterHits();
  ist_ = muEvent_->numberOfIstHits();
  ssd_ = muEvent_->numberOfSsdHits();
  vz_ = muDst_->event()->primaryVertexPosition().z();
  nprim_ = muDst_->primaryTracks()->GetEntries();
  refmult_ = muEvent_->refMult();
  rank_ = muDst_->primaryVertex()->ranking();
  
  vpdvzother_ = muEvent_->vpdVz();
  StBTofHeader* tofheader = muDst_->btofHeader();
  if (tofheader)
    vpdvz_ = tofheader->vpdVz(0);
  else
    vpdvz_ = 1000;
  dvz_ = vz_ - vpdvz_;
  
  if (!TrackLoop())
    return kStOK;
  
  tree_->Fill();
  
  return kStOK;
}

void  StProductionTrackQA::Clear(Option_t* option) {
  pt_.clear();
  dca_.clear();
  eta_.clear();
  phi_.clear();
  nhits_.clear();
  nhitspos_.clear();
  hft_.clear();

  StMaker::Clear(option);
}

Int_t StProductionTrackQA::Finish() {
  if (out_ == nullptr) {
    out_ = new TFile("StProductionTrackQA.root", "RECREATE");
  }

  out_->cd();
  
  tree_->Write();

  out_->Close();
  return kStOk;
}


int StProductionTrackQA::InitInput() {

  muDstMaker_ = (StMuDstMaker*) GetMakerInheritsFrom("StMuDstMaker");
  if (muDstMaker_ == nullptr) {
    LOG_ERROR << "No muDstMaker found in chain: StProductionTrackQA init failed" << endm;
    return kStFatal;
  }

  return kStOK;
}

int StProductionTrackQA::InitOutput() {
  
  tree_ = new TTree("tree", "Event Tree");
  
  tree_->Branch("runid", &runid_);
  tree_->Branch("eventid", &eventid_);
  tree_->Branch("nvertices", &nvertices_);
  tree_->Branch("vpdvz", &vpdvz_);
  tree_->Branch("vpdvzother", &vpdvzother_);
  tree_->Branch("zdcrate", &zdcrate_);
  tree_->Branch("nglobal", &nglobal_);
  tree_->Branch("pxl", &pxl_);
  tree_->Branch("ist", &ist_);
  tree_->Branch("ssd", &ssd_);
  tree_->Branch("vz", &vz_);
  tree_->Branch("refmult", &refmult_);
  tree_->Branch("nprim", &nprim_);
  tree_->Branch("rank", &rank_);
  tree_->Branch("dvz", &dvz_);
  tree_->Branch("ntrackswhft", &ntrackswhft_);
  tree_->Branch("pt", &pt_);
  tree_->Branch("dca", &dca_);
  tree_->Branch("nhits", &nhits_);
  tree_->Branch("nhitspos", &nhitspos_);
  tree_->Branch("eta", &eta_);
  tree_->Branch("phi", &phi_);
  tree_->Branch("hft", &hft_);
  tree_->Branch("ntpchits", &ntpchits_);
 
  return kStOK;
}

bool StProductionTrackQA::LoadEvent() {
  muDst_ = muDstMaker_->muDst();
  if (muDst_ == nullptr) {
    LOG_ERROR << "Could not load MuDst" << endm;
    return kStErr;
  }
  muEvent_ = muDst_->event();
  if (muEvent_ == nullptr) {
    LOG_ERROR << "Could not load MuDstEvent" << endm;
    return kStErr;
  }

  return true;
}

bool StProductionTrackQA::TrackLoop() {
  
  // now we loop over all tracks located at that vertex to find average dca & pt
  UInt_t nTracks = muDst_->primaryTracks()->GetEntries();
  
  pt_.reserve(nTracks);
  dca_.reserve(nTracks);
  eta_.reserve(nTracks);
  phi_.reserve(nTracks);
  nhits_.reserve(nTracks);
  nhitspos_.reserve(nTracks);
  hft_.reserve(nTracks);
  
  for (UInt_t j = 0; j < nTracks; ++j) {
    StMuTrack* muTrack = (StMuTrack*) muDst_->primaryTracks(j);
    
    // apply track quality cuts
    if (muTrack->flag() < 0) continue;
    
    // check if it has hft first
    if (muTrack->nHitsFit(kPxlId) ||
        muTrack->nHitsFit(kIstId))
      ntrackswhft_++;
    
    pt_.push_back(muTrack->pt());
    dca_.push_back(muTrack->dcaGlobal().mag());
    eta_.push_back(muTrack->eta());
    phi_.push_back(muTrack->phi());
    nhits_.push_back(muTrack->nHitsFit());
    nhitspos_.push_back(muTrack->nHitsPoss(kTpcId));
    hft_.push_back(muTrack->nHitsFit(kPxlId) + muTrack->nHitsFit(kIstId));
  }
  
  pt_.shrink_to_fit();
  eta_.shrink_to_fit();
  phi_.shrink_to_fit();
  nhitspos_.shrink_to_fit();
  nhits_.shrink_to_fit();
  dca_.shrink_to_fit();
  hft_.shrink_to_fit();
  
  return true;
}

bool StProductionTrackQA::SelectVertex() {
  
  Int_t nVertices = muDst_->numberOfPrimaryVertices();
  Int_t usedVertex = -1;
  StBTofHeader* tofheader = muDst_->btofHeader();
  if (tofheader) {
    double vpdVz = tofheader->vpdVz(0);
    if (vpdVz == -999) { muDst_->setVertexIndex(0); return kFALSE; }
    for (Int_t i = 0; i < nVertices; ++i) {
      muDst_->setVertexIndex(i);
      StThreeVectorF Vposition = muDst_->event()->primaryVertexPosition();
      Double_t vz = Vposition.z();
      if (fabs(vz-vpdVz) < 3.0) {
        usedVertex = i;
        break;
      }
    }
  }
  if (usedVertex != -1) {
    muDst_->setVertexIndex(usedVertex);
    return kTRUE;
  }
  else {
    muDst_->setVertexIndex(0);
    return kFALSE;
  }
}


