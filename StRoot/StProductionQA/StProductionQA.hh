
#ifndef STPRODUCTIONQA_HH
#define STPRODUCTIONQA_HH

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

class StProductionQA : public StMaker {
public:
  StProductionQA(std::string outFile = "StProductionQA.root", std::string name = "StProductionQA");
  ~StProductionQA();
  
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
  
  int VertexLoop();
  
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
  double vpdvz_alt_;
  double zdcrate_;
  int nglobal_;
  int pxl_;
  int ist_;
  int ssd_;
  
  std::vector<double> vz_;
  std::vector<int> refmult_;
  std::vector<int> nprim_;
  std::vector<double> rank_;
  std::vector<std::vector<double>> dca_;
  std::vector<std::vector<double>> pt_;
  std::vector<std::vector<double>> eta_;
  std::vector<std::vector<int>> nhit_;
  std::vector<std::vector<int>> nhitposs_;
  std::vector<std::vector<int>> hft_;
  ClassDef(StProductionQA, 0);
};

#endif // StProductionQA_HH
