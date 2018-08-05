#include "StProductionTrackQA.hh"

#include "St_base/StMessMgr.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StEvent/StBTofHeader.h"
#include "StEvent/StZdcTriggerDetector.h"

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
  LOG_INFO << "event loaded" << endm;
  if (!SelectVertex())
    return kStOK;
  LOG_INFO << "selected vertex" << endm;
  if (!event_cuts_.AcceptEvent(muEvent_))
    return kStOK;
  LOG_INFO << "passed cuts" << endm;
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
  vpdvz_ = muDst_->event()->vpdVz();
  dvz_ = vz_ - vpdvz_;
  
  if (!TrackLoop())
    return kStOK;
  LOG_INFO << "track loop complete" << endm;
  tree_->Fill();
  
  return kStOK;
}

void  StProductionTrackQA::Clear(Option_t* option) {
  
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
  tree_->Branch("zdcrate", &zdcrate_);
  tree_->Branch("nglobal", &nglobal_);
  tree_->Branch("pxl", &pxl_);
  tree_->Branch("ist", &ist_);
  tree_->Branch("ssd", &ssd_);
  tree_->Branch("vz", &vz_);
  tree_->Branch("refmult", &refmult_);
  tree_->Branch("nprim", &nprim_);
  tree_->Branch("rank", &rank_);
  tree_->Branch("ntracks", &ntracks_);
  tree_->Branch("ntrackswhft", &ntrackswhft_);
 
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
  ntracks_ = 0;
  ntrackswhft_ = 0;
  
  // now we loop over all tracks located at that vertex to find average dca & pt
  UInt_t nTracks = muDst_->primaryTracks()->GetEntries();
  
  for (UInt_t j = 0; j < nTracks; ++j) {
    StMuTrack* muTrack = (StMuTrack*) muDst_->primaryTracks(j);
    
    // check if it has hft first
    if (muTrack->nHitsFit(kPxlId) ||
        muTrack->nHitsFit(kIstId))
      ntrackswhft_++;
    
    // apply track quality cuts
    if (muTrack->flag() < 0) continue;
    if (muTrack->nHitsFit() < 20) continue;
    if ((double)muTrack->nHitsFit() / muTrack->nHitsPoss() < 0.52) continue;
    if (muTrack->pt() < 0.2) continue;
    if (fabs(muTrack->eta()) > 1.0) continue;
    if (muTrack->dcaGlobal().mag() > 3.0) continue;
    
    ntracks_++;
    
  }
  
  return kStOK;
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


