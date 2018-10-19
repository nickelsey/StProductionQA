#include "StEfficiencyQARun4.hh"

#include "St_base/StMessMgr.h"
#include "StEvent/StBTofHeader.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

#include "TMath.h"
#include "TTree.h"

#include "TString.h"

#include "StRefMultCorr/CentralityMaker.h"

#include <iostream>

ClassImp(StEfficiencyQARun4);

StEfficiencyQARun4::StEfficiencyQARun4(std::string outputFile, std::string name) : StMaker(name.c_str()) {

    muDstMaker_ = nullptr;
    muDst_ = nullptr;
    muEvent_ = nullptr;

    minFit_ = 20;
    minFitFrac_ = 0.52;
    maxDCA_ = 1.0;

    out_ = new TFile(outputFile.c_str(), "RECREATE");
}

StEfficiencyQARun4::~StEfficiencyQARun4() {

}


int StEfficiencyQARun4::Init() {
    if (InitInput() != kStOK)
        return kStFatal;
    if (InitOutput() != kStOK)
        return kStFatal;
    return kStOK;
}


Int_t StEfficiencyQARun4::Make() {
    
    if (LoadEvent() == false) {
        LOG_ERROR << "Could not load MuDst" << endm;
        return kStOK;
    }
    
    //if (!SelectVertex())
    //    return kStOK;
    
    if (!event_cuts_.AcceptEvent(muEvent_))
        return kStOK;
    
    int centrality = cent_def_.centrality9(muEvent_->refMult());
    if (centrality > 8 || centrality < 0)
        return kStOK;
    TF2* effic_curve_ = run4_eff_.AuAuEff(centrality);
    
    vz_->Fill(muEvent_->primaryVertexPosition().z());
    refmult_->Fill(muEvent_->refMult());
    grefmult_->Fill(muEvent_->grefmult());
    centrality_->Fill(centrality);

    for (int i = 0; i < muDst_->primaryTracks()->GetEntries(); ++i) {
        StMuTrack* muTrack = (StMuTrack*) muDst_->primaryTracks(i);
  
        if (muTrack->flag() < 0)
            continue;
        if (muTrack->dcaGlobal().mag() > maxDCA_)
            continue;
        if (muTrack->nHitsFit() < minFit_)
            continue;
        if ((double) muTrack->nHitsFit() / (muTrack->nHitsPoss(kTpcId) + 1) < minFitFrac_)
            continue;
        if (fabs(muTrack->eta()) > 1.0)
            continue;
  
        data_nhit_->Fill(centrality, muTrack->pt(), muTrack->nHitsFit());
        data_dca_->Fill(centrality, muTrack->pt(), muTrack->dcaGlobal().mag());
        data_eta_->Fill(centrality, muTrack->pt(), muTrack->eta());
        data_phi_->Fill(centrality, muTrack->pt(), muTrack->phi());
        data_nhitposs_->Fill(centrality, muTrack->pt(), muTrack->nHitsPoss(kTpcId)+1);
        data_fitfrac_->Fill(centrality, muTrack->pt(), (double)muTrack->nHitsFit()/muTrack->nHitsPoss(kTpcId)+1);
        pt_->Fill(centrality, muTrack->pt());
      
        double efficiency = 1.0;
      
        if (effic_curve_)
            efficiency = effic_curve_->Eval(muTrack->eta(), muTrack->pt());
  
        pt_corr_->Fill(centrality, muTrack->pt(), 1.0 / efficiency);
        ave_effic_->Fill(centrality, muTrack->pt(), efficiency);
    }
  
    return kStOK;
}


Int_t StEfficiencyQARun4::Finish() {
    if (out_ == nullptr) {
        out_ = new TFile("stefficiencyassessor.root", "RECREATE");
    }

    out_->cd();

    vz_->Write();
    refmult_->Write();
    grefmult_->Write();
    centrality_->Write();

    data_nhit_->Write();
    data_dca_->Write();
    data_nhitposs_->Write();
    data_eta_->Write();
    data_phi_->Write();
    data_fitfrac_->Write();

    pt_->Write();
    pt_corr_->Write();
    ave_effic_->Write();
  
    out_->Close();
    return kStOk;
}


int StEfficiencyQARun4::InitInput() {
    muDstMaker_ = (StMuDstMaker*) GetMakerInheritsFrom("StMuDstMaker");
    if (muDstMaker_ == nullptr) {
        LOG_ERROR << "No muDstMaker found in chain: StEfficiencyQARun4 init failed" << endm;
        return kStFatal;
    }
  
  
    return kStOK;
}

int StEfficiencyQARun4::InitOutput() {

    data_nhit_ = new TH3D("nhit", ";cent;pt;nhit", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, 0, 50);
    data_dca_ = new TH3D("dca", ";cent;pt;DCA[cm]", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, 0, 3.0);
    data_nhitposs_ = new TH3D("nhitposs", ";cent;pt;nhitposs", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, 0, 50);
    data_eta_ = new TH3D("eta", ";cent;pt;#eta", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, -1, 1);
    data_phi_ = new TH3D("phi", ";cent;pt;#phi", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, -TMath::Pi(), TMath::Pi());
    data_fitfrac_ = new TH3D("fitfrac", ";cent;pt;fitfrac", 9, -0.5, 8.5, 20, 0.0, 5.0, 50, 0, 1);

    vz_ = new TH1D("vz", ";v_{z}[cm]", 60, -30, 30);
    refmult_ = new TH1D("refmult", ";refmult", 800, 0, 800);
    grefmult_ = new TH1D("grefmult", ";grefmult", 800, 0, 800);
    centrality_ = new TH1D("centrality", ";centrality", 9, -0.5, 8.5);

    pt_ = new TH2D("pt", ";cent;p_{T}", 9, -0.5, 8.5, 50, 0, 5);
    pt_corr_ = new TH2D("ptcorr", ";cent;p_{T}", 9, -0.5, 8.5, 50, 0, 5);
    ave_effic_ = new TProfile2D("aveeffic", ";cent; effic", 9, -0.5, 8.5, 50, 0, 5);
    return kStOK;
}

bool StEfficiencyQARun4::LoadEvent() {
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

bool StEfficiencyQARun4::SelectVertex() {
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
