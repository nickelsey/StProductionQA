#ifndef EFFICIENCY_RUN4_EFF_H
#define EFFICIENCY_RUN4_EFF_H

#include <vector>
#include <string>

#include "TF2.h"

class Run4Eff {
public:
  Run4Eff();
  
  ~Run4Eff();
  
  double AuAuEff(double pt, double eta, int cent);
  TF2*   AuAuEff(int cent) {return y4_functions[8-cent];} 
private:
  std::vector<double> parset0;
  std::vector<double> parset1;
  std::vector<double> parset2;
  std::vector<double> parset3;
  std::vector<double> parset4;
  std::vector<double> parset5;
  std::vector<double> parset6;
  
  double maxPt;
  
  TF2* y4_functions[9];
  
};


#endif // EFFICIENCY_RUN4_EFF_H
