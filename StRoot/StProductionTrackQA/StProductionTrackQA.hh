
#ifndef STPRODUCTIONTRACKQA_HH
#define STPRODUCTIONTRACKQA_HH

#include "centrality_def.hh"
#include "event_cuts.hh"

#include <string>
#include <vector>
#include <set>

#include "TTree.h"
#include "TFile.h"

#include "StMaker.h"

class StMuDstMaker;
class StMuDst;
class StMuEvent;

class StProductionTrackQA : public StMaker {
public:
  StProductionTrackQA(std::string outFile = "StProductionTrackQA.root", std::string name = "StProductionTrackQA");
  ~StProductionTrackQA();
  
  // Called during chain initialization - finds MuDstMaker in the chain, and
  // creates all histograms
  Int_t Init();
  
  // called once per event, does all event-by-event analysis
  Int_t Make();
  
  // called at the end of each event - for us it is currently empty
  void  Clear(Option_t* option = "");
  
  // saves histograms to disk
  Int_t Finish();
  
  CentralityDef& CentDef() {return cent_def_;}
  
  EventCuts& Cuts() {return event_cuts_;}
  
private:
  
  int InitInput();
  int InitOutput();
  bool LoadEvent();
  
  bool TrackLoop();
  bool SelectVertex();
  
  TFile* out_;
  
  CentralityDef cent_def_;
  EventCuts event_cuts_;
  
  StMuDstMaker* muDstMaker_;
  StMuDst* muDst_;
  StMuEvent* muEvent_;
  
  TTree* tree_;
  
  int runid_;
  int eventid_;
  int nvertices_;
  double vpdvz_;
  double zdcrate_;
  int nglobal_;
  int pxl_;
  int ist_;
  int ssd_;
  double vz_;
  double refmult_;
  double nprim_;
  double rank_;
  double dvz_;
  int ntracks_;
  int ntrackswhft_;
  
  ClassDef(StProductionTrackQA, 0);
};

#endif // StProductionTrackQA_HH
