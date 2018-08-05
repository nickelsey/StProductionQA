#include "StProductionQA.hh"

#include "St_base/StMessMgr.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StEvent/StBTofHeader.h"
#include "StEvent/StZdcTriggerDetector.h"

#include "TMath.h"

ClassImp(StProductionQA);

StProductionQA::StProductionQA(std::string outputFile, string name) : StMaker(name.c_str()) {
  muDstMaker_ = nullptr;
  muDst_ = nullptr;
  muEvent_ = nullptr;

  out_ = new TFile(outputFile.c_str(), "RECREATE");
}

StProductionQA::~StProductionQA() {

}

Int_t StProductionQA::Init() {
  if (InitInput() != kStOK)
    return kStFatal;
  if (InitOutput() != kStOK)
    return kStFatal;
  return StMaker::Init();
}

Int_t StProductionQA::Make() {
  
  // load the matching miniMC event
  if (LoadEvent() == false) {
    LOG_ERROR << "Error with loading muDst" << endm;
    return kStErr;
  }
  
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
  
  StBTofHeader* tofheader = muDst_->btofHeader();
  if (tofheader)
    vpdvz_ = tofheader->vpdVz(0);
  else
    vpdvz_ = -9999;
  
  switch(VertexLoop()) {
    case kStOK:
      break;
    case kStFatal:
      return kStFatal;
    default:
      return kStOK;
  }
  
  if (vz_.size())
    tree_->Fill();
  
  return kStOK;
}

void  StProductionQA::Clear(Option_t* option) {
  vz_.clear();
  refmult_.clear();
  nprim_.clear();
  rank_.clear();
  dca_.clear();
  pt_.clear();
  StMaker::Clear(option);
}

Int_t StProductionQA::Finish() {
  if (out_ == nullptr) {
    out_ = new TFile("StProductionQA.root", "RECREATE");
  }

  out_->cd();
  
  tree_->Write();

  out_->Close();
  return kStOk;
}


int StProductionQA::InitInput() {

  muDstMaker_ = (StMuDstMaker*) GetMakerInheritsFrom("StMuDstMaker");
  if (muDstMaker_ == nullptr) {
    LOG_ERROR << "No muDstMaker found in chain: StProductionQA init failed" << endm;
    return kStFatal;
  }

  return kStOK;
}

int StProductionQA::InitOutput() {
  
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
  tree_->Branch("dca", &dca_);
  tree_->Branch("pt", &pt_);
  return kStOK;
}

bool StProductionQA::LoadEvent() {
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

int StProductionQA::VertexLoop() {
  // get # of saved vertices
  Int_t nVertices = muDst_->numberOfPrimaryVertices();
  if (nVertices == 0) {
    LOG_DEBUG <<"TStarJetPicoMaker: Event has no vertices" << endm;
    return kStErr;
  }
  
  for (int i = 0; i < nVertices; ++i) {
    muDst_->setVertexIndex(i);
    
    StThreeVectorF Vposition = muDst_->event()->primaryVertexPosition();
    double vz = Vposition.z();
    int nprim = muDst_->primaryTracks()->GetEntries();
    int refmult = muEvent_->refMult();
    double rank = muDst_->primaryVertex()->ranking();
    
    // now we loop over all tracks located at that vertex to find average dca & pt
    UInt_t nTracks = muDst_->primaryTracks()->GetEntries();
    double pt = 0.0;
    double dca = 0.0;
    for (UInt_t j = 0; j < nTracks; ++j) {
        StMuTrack* muTrack = (StMuTrack*) muDst_->primaryTracks(j);
        pt += muTrack->momentum().perp() / (double) nTracks;
        dca += muTrack->dcaGlobal().mag() / (double) nTracks;
    }
    
    vz_.push_back(vz);
    nprim_.push_back(nprim);
    refmult_.push_back(refmult);
    rank_.push_back(rank);
    pt_.push_back(pt);
    dca_.push_back(dca);
  }
  return kStOK;
}


