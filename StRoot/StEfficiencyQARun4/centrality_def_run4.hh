
#ifndef CENTRALITY_DEF_RUN4_HH
#define CENTRALITY_DEF_RUN4_HH

// defines a lightweight class that can handle StRefMultCorr corrections
// allows similar cuts to be set, but by hand instead of reading from a
// table

// used to test refmultcorr & centrality definitions in sct

#include <vector>

class CentralityDefRun4 {
public:
  CentralityDefRun4();
  ~CentralityDefRun4();
  
  // given a corrected refmult, calculate the 9 bin centrality
  int centrality9(int refmult);
 
  // get centrality boundaries
  std::vector<unsigned> CentralityBounds9Bin() const {return cent_bin_9_;}
  
private:
   
  std::vector<unsigned> cent_bin_9_;
  
};

#endif // CENTRALITY_DEF_RUN4_HH
