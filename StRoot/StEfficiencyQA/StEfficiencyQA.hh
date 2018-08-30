
#ifndef STEFFICIENCYQA__HH
#define STEFFICIENCYQA__HH

#include <string>
#include <vector>
#include <set>

#include "StMaker.h"
#include "TChain.h"
#include "TFile.h"
#include "TH3F.h"
#include "TH2D.h"
#include "TProfile2D.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"

#include "event_cuts.hh"

#include "centrality_def.hh"
#include "StRefMultCorr/StRefMultCorr.h"

class StEfficiencyQA : public StMaker {
    public:
        StEfficiencyQA(std::string outputFile = "StEfficiencyQA.root", std::string name = "StEfficiencyQA");

        ~StEfficiencyQA();

        // allows you to modify the centrality and StRefMultCorr definitions
        CentralityDef* CentralityDefinitionP17id() {return p17id_cent_def_;}
        StRefMultCorr* CentralityDefinitionP16id() {return p16id_cent_def_;}
        
        bool LoadEfficiencyCurves(std::string filename);

        void SetEventCuts(EventCuts cuts) {event_cuts_ = cuts;}
        EventCuts& GetEventCuts()         {return event_cuts_;}

        // set track cuts for matched tracks
        void SetDCAMax(double dca) {maxDCA_ = dca;}
        double DCAMax() const      {return maxDCA_;}

        void SetMinFitPoints(unsigned fit) {minFit_ = fit;}
        unsigned MinFitPoints() const      {return minFit_;}
        
        void SetMinFitFrac(double frac) {minFitFrac_ = frac;}
        double MinFitFrac() const       {return minFitFrac_;}
  
        void UseP17idCent(bool flag) {use_p17id_cent_ = flag;}
        

        // (re)creates histograms from current axisDefs
        Int_t Init();

        // process event
        Int_t Make();

        // save result histograms to disk
        Int_t Finish();

    private:

        CentralityDef* p17id_cent_def_;
        StRefMultCorr* p16id_cent_def_;

        int InitInput();
        int InitOutput();
        bool LoadEvent();
        bool SelectVertex();
        
        TFile* out_;

        EventCuts event_cuts_;

        StMuDstMaker* muDstMaker_;
        StMuDst* muDst_;
        StMuEvent* muEvent_;

        TH3D* data_nhit_;
        TH3D* data_dca_;
        TH3D* data_nhitposs_;
        TH3D* data_eta_;
        TH3D* data_phi_;
        TH3D* data_fitfrac_;

        TH1D* vz_;
        TH1D* refmult_;
        TH1D* grefmult_;
        TH1D* centrality_;
  
        TH2D* pt_;

        TH2D* pt_corr_;
        TProfile2D* ave_effic_;

        int minFit_;
        double minFitFrac_;
        double maxDCA_;
        bool use_p17id_cent_;
        
        std::vector<TH1D*> eff_curves_;
        ClassDef(StEfficiencyQA,1)
};

#endif // STEFFICIENCYQA__HH
