
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

template class std::vector<float>;
template class std::vector<short>;

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
  std::vector<std::vector<float>> dca_;
  std::vector<std::vector<float>> pt_;
  std::vector<std::vector<float>> eta_;
  std::vector<std::vector<short>> nhit_;
  std::vector<std::vector<short>> nhitposs_;
  std::vector<std::vector<short>> hft_;
  ClassDef(StProductionQA, 0);
};

#endif // StProductionQA_HH
