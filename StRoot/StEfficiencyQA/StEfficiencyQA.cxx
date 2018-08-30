#include "StEfficiencyQA.hh"

#include "St_base/StMessMgr.h"
#include "StEvent/StBTofHeader.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

#include "TMath.h"
#include "TTree.h"

#include "TString.h"

#include "StRefMultCorr/CentralityMaker.h"

#include <iostream>

ClassImp(StEfficiencyQA);

StEfficiencyQA::StEfficiencyQA(std::string outputFile, std::string name) : StMaker(name.c_str()) {

    muDstMaker_ = nullptr;
    muDst_ = nullptr;
    muEvent_ = nullptr;

    minFit_ = 20;
    minFitFrac_ = 0.52;
    maxDCA_ = 3.0;
    use_p17id_cent_ = true;

    out_ = new TFile(outputFile.c_str(), "RECREATE");
}

StEfficiencyQA::~StEfficiencyQA() {

}

bool StEfficiencyQA::LoadEfficiencyCurves(std::string filename) {
    TFile tmp(filename.c_str(), "READ");

    for (int i = 0; i < 9; ++i) {
        std::string name = "cent_bin_" + std::to_string(i);
        std::string clone_name = name + "_clone";
        TH1D* h = (TH1D*) tmp.Get(name.c_str())->Clone(clone_name.c_str());
        if (h == nullptr)
            return false;
        eff_curves_.push_back(h);
    }
    return true;
}

int StEfficiencyQA::Init() {
    if (InitInput() != kStOK)
        return kStFatal;
    if (InitOutput() != kStOK)
        return kStFatal;
    return kStOK;
}


Int_t StEfficiencyQA::Make() {
  
    if (LoadEvent() == false) {
        LOG_ERROR << "Could not load MuDst" << endm;
        return kStOK;
    }
  
    if (!SelectVertex())
        return kStOK;

    if (!event_cuts_.AcceptEvent(muEvent_))
        return kStOK;
  
    TH1D* effic_curve_ = nullptr;
    int centrality = -1;
    if (p17id_cent_def_) {
        p17id_cent_def_->setEvent(muEvent_->runId(), muEvent_->refMult(), muEvent_->runInfo().zdcCoincidenceRate(), muEvent_->primaryVertexPosition().z());
        centrality = p17id_cent_def_->centrality9();
        if (centrality < 0 || centrality > 8)
            return kStOK;
        effic_curve_ = eff_curves_[centrality];
    }
    else if (p16id_cent_def_) {
        p16id_cent_def_->init(muEvent_->runId());
        p16id_cent_def_->initEvent(muEvent_->grefmult(), muEvent_->primaryVertexPosition().z(), muEvent_->runInfo().zdcCoincidenceRate());
        centrality = p16id_cent_def_->getCentralityBin9();
        if (centrality < 0 || centrality > 8)
            return kStOK;
        effic_curve_ = eff_curves_[centrality];
    }
    else {
        LOG_ERROR << "error: no centrality definition created" << endm;
        return kStFatal;
    }
  
    if (muEvent_->numberOfPxlInnerHits() + muEvent_->numberOfPxlOuterHits())
        return kStOK;
    if (muEvent_->numberOfIstHits() || muEvent_->numberOfSsdHits())
        return kStOK;
  
    vz_->Fill(muEvent_->primaryVertexPosition().z());
    refmult_->Fill(muEvent_->refMult());
    grefmult_->Fill(muEvent_->grefmult());
    centrality_->Fill(centrality);

    for (int i = 0; i < muDst_->primaryTracks()->GetEntries(); ++i) {
        StMuTrack* muTrack = (StMuTrack*) muDst_->primaryTracks(i);
      std::cout << "cuts" << std::endl;
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
      std::cout << "hisotgrams" << std::endl;
        data_nhit_->Fill(centrality, muTrack->pt(), muTrack->nHitsFit());
        data_dca_->Fill(centrality, muTrack->pt(), muTrack->dcaGlobal().mag());
        data_eta_->Fill(centrality, muTrack->pt(), muTrack->eta());
        data_phi_->Fill(centrality, muTrack->pt(), muTrack->phi());
        data_nhitposs_->Fill(centrality, muTrack->pt(), muTrack->nHitsPoss(kTpcId)+1);
        data_fitfrac_->Fill(centrality, muTrack->pt(), (double)muTrack->nHitsFit()/muTrack->nHitsPoss(kTpcId)+1);
        pt_->Fill(centrality, muTrack->pt());
      
        double efficiency = 1.0;
      std::cout << "efficiency" << std::endl;
        if (effic_curve_ && muTrack->pt() < 4.5)
            efficiency = effic_curve_->GetBinContent(effic_curve_->FindBin(muTrack->pt()));
      std::cout << "pt" << std::endl;
        pt_corr_->Fill(centrality, muTrack->pt(), 1.0 / efficiency);
        ave_effic_->Fill(centrality, muTrack->pt(), efficiency);
    }
  
    return kStOK;
}


Int_t StEfficiencyQA::Finish() {
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


int StEfficiencyQA::InitInput() {
    muDstMaker_ = (StMuDstMaker*) GetMakerInheritsFrom("StMuDstMaker");
    if (muDstMaker_ == nullptr) {
        LOG_ERROR << "No muDstMaker found in chain: StEfficiencyQA init failed" << endm;
        return kStFatal;
    }
  
    if (TString(muDstMaker_->GetFile()).Contains("P17id") ||
        TString(muDstMaker_->GetFile()).Contains("P18if") ) {
  
        p17id_cent_def_ = new CentralityDef();
        p16id_cent_def_ = nullptr;
    }
    else if (TString(muDstMaker_->GetFile()).Contains("P16id") ){
        p16id_cent_def_ = CentralityMaker::instance()->getgRefMultCorr_P16id();
        p16id_cent_def_->setVzForWeight(6, -6.0, 6.0);
        p16id_cent_def_->readScaleForWeight("StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14_P16id.txt");
        p17id_cent_def_ = nullptr;
    }
  else {
    LOG_ERROR << "could not discover production tag" << endm;
    return kStFatal;
  }
  
    return kStOK;
}

int StEfficiencyQA::InitOutput() {

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

bool StEfficiencyQA::LoadEvent() {
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

bool StEfficiencyQA::SelectVertex() {
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
