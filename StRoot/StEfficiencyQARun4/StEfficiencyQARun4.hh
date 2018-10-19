
#ifndef STEFFICIENCYQARUN4__HH
#define STEFFICIENCYQARUN4__HH

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

#include "event_cuts_run4.hh"
#include "centrality_def_run4.hh"
#include "run4_eff.h"
class StEfficiencyQARun4 : public StMaker {
    public:
        StEfficiencyQARun4(std::string outputFile = "StEfficiencyQARun4.root", std::string name = "StEfficiencyQARun4");

        ~StEfficiencyQARun4();

        // allows you to modify the centrality and efficiency definitions
        CentralityDefRun4& CentralityDefinition() {return cent_def_;}
        Run4Eff& EfficiencyCurves() {return run4_eff_;}
        
        // method for setting event cuts
        void SetEventCuts(EventCuts cuts) {event_cuts_ = cuts;}
        EventCutsRun4& GetEventCuts()         {return event_cuts_;}

        // set track cuts for matched tracks
        void SetDCAMax(double dca) {maxDCA_ = dca;}
        double DCAMax() const      {return maxDCA_;}

        void SetMinFitPoints(unsigned fit) {minFit_ = fit;}
        unsigned MinFitPoints() const      {return minFit_;}

        void SetMinFitFrac(double frac) {minFitFrac_ = frac;}
        double MinFitFrac() const       {return minFitFrac_;}

        // (re)creates histograms from current axisDefs
        Int_t Init();

        // process event
        Int_t Make();

        // save result histograms to disk
        Int_t Finish();

    private:

        CentralityDefRun4 cent_def_;
        Run4Eff run4_eff_;
        EventCutsRun4 event_cuts_;
        
        int InitInput();
        int InitOutput();
        bool LoadEvent();
        bool SelectVertex();

        TFile* out_;

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

        ClassDef(StEfficiencyQARun4,1)
};

#endif // STEFFICIENCYQARUN4__HH
